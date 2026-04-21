#!/usr/bin/env python3
"""
Offline floor-estimation replay utility.

Paper-aligned methods in this script:
1) EKF recursive filtering for x=[h, v, b]
2) NIS gate for measurement outlier rejection
3) MAP/IEKF-style iterative parameter fitting (coordinate search + repeated relinearized replay)
"""

from __future__ import annotations

import argparse
import csv
import math
import random
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple


@dataclass
class ReplayParams:
    sigma_accel_process: float = 0.35
    sigma_bias_random_walk: float = 0.03
    sigma_pressure_meas: float = 0.12
    floor_height_m: float = 3.0
    nis_gate: float = 9.21
    move_up_speed_mps: float = 0.25
    move_down_speed_mps: float = -0.25
    arrival_speed_mps: float = 0.08
    arrival_hold_ms: int = 1500
    floor_change_band_m: float = 1.2
    floor_change_confirm_samples: int = 3
    min_floor: int = 0
    max_floor: int = 60
    calibration_sample_count: int = 30


@dataclass
class Sample:
    timestamp_ms: int
    pressure_hpa: float
    temperature_c: float
    seq: int = 0
    floor_gt: Optional[int] = None


@dataclass
class ReplayPoint:
    timestamp_ms: int
    pressure_hpa: float
    temperature_c: float
    h_m: float
    v_mps: float
    b_m: float
    floor_est: int
    floor_gt: Optional[int]
    nis: float
    accepted: bool
    confidence: float


class NodeFloorEstimator:
    def __init__(self, params: ReplayParams):
        self.params = params
        self.reset()

    def reset(self) -> None:
        self.calibrated = False
        self.calib_pressures: List[float] = []
        self.p_ref_hpa = 1013.25

        self.x = [0.0, 0.0, 0.0]  # [h, v, b]
        self.P = [
            [1.0, 0.0, 0.0],
            [0.0, 0.25, 0.0],
            [0.0, 0.0, 1.0],
        ]

        self.last_ts_ms = 0
        self.floor_est = 0
        self.floor_candidate = 0
        self.floor_candidate_count = 0
        self.vertical_state = "IDLE"
        self.arrival_hold_start_ms = 0

        self.last_nis = 0.0
        self.rejected_updates = 0
        self.reject_streak = 0
        self.confidence = 0.0

    @staticmethod
    def _mat_mul(A: List[List[float]], B: List[List[float]]) -> List[List[float]]:
        C = [[0.0] * 3 for _ in range(3)]
        for i in range(3):
            for j in range(3):
                C[i][j] = sum(A[i][k] * B[k][j] for k in range(3))
        return C

    @staticmethod
    def _mat_transpose(A: List[List[float]]) -> List[List[float]]:
        return [[A[j][i] for j in range(3)] for i in range(3)]

    @staticmethod
    def _mat_add(A: List[List[float]], B: List[List[float]]) -> List[List[float]]:
        return [[A[i][j] + B[i][j] for j in range(3)] for i in range(3)]

    def _symmetrize(self) -> None:
        for i in range(3):
            for j in range(i + 1, 3):
                avg = 0.5 * (self.P[i][j] + self.P[j][i])
                self.P[i][j] = avg
                self.P[j][i] = avg
            self.P[i][i] = max(self.P[i][i], 1e-6)

    def _pressure_from_state(self, h: float, b: float) -> float:
        term = 1.0 - (h + b) / 44330.0
        term = min(1.5, max(0.05, term))
        return self.p_ref_hpa * (term ** 5.255)

    def _pressure_jacobian_hb(self, h: float, b: float) -> float:
        term = 1.0 - (h + b) / 44330.0
        term = min(1.5, max(0.05, term))
        return -self.p_ref_hpa * (5.255 / 44330.0) * (term ** 4.255)

    def _predict(self, dt: float) -> None:
        # Paper method: EKF prediction.
        self.x[0] += self.x[1] * dt

        F = [
            [1.0, dt, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
        ]
        FT = self._mat_transpose(F)

        FP = self._mat_mul(F, self.P)
        P_pred = self._mat_mul(FP, FT)

        s_a2 = self.params.sigma_accel_process ** 2
        s_b2 = self.params.sigma_bias_random_walk ** 2
        dt2 = dt * dt
        dt3 = dt2 * dt
        dt4 = dt2 * dt2

        Q = [
            [0.25 * dt4 * s_a2, 0.5 * dt3 * s_a2, 0.0],
            [0.5 * dt3 * s_a2, dt2 * s_a2, 0.0],
            [0.0, 0.0, dt * s_b2],
        ]

        self.P = self._mat_add(P_pred, Q)
        self._symmetrize()

    def _update(self, z_pressure_hpa: float) -> bool:
        # Paper method: EKF nonlinear pressure-domain correction.
        z_pred = self._pressure_from_state(self.x[0], self.x[2])
        d_hb = self._pressure_jacobian_hb(self.x[0], self.x[2])
        H = [d_hb, 0.0, d_hb]

        PHt = [sum(self.P[i][j] * H[j] for j in range(3)) for i in range(3)]
        S = sum(H[i] * PHt[i] for i in range(3)) + self.params.sigma_pressure_meas ** 2
        if S <= 1e-6:
            return False

        innovation = z_pressure_hpa - z_pred
        self.last_nis = (innovation * innovation) / S

        # Paper method: NIS gate.
        if self.last_nis > self.params.nis_gate:
            self.rejected_updates += 1
            self.reject_streak += 1
            return False

        K = [PHt[i] / S for i in range(3)]
        self.x = [self.x[i] + K[i] * innovation for i in range(3)]

        KH = [[K[i] * H[j] for j in range(3)] for i in range(3)]
        I_KH = [
            [1.0 - KH[0][0], -KH[0][1], -KH[0][2]],
            [-KH[1][0], 1.0 - KH[1][1], -KH[1][2]],
            [-KH[2][0], -KH[2][1], 1.0 - KH[2][2]],
        ]

        tmp = self._mat_mul(I_KH, self.P)
        I_KH_T = self._mat_transpose(I_KH)
        joseph = self._mat_mul(tmp, I_KH_T)

        R = self.params.sigma_pressure_meas ** 2
        KRKt = [[K[i] * R * K[j] for j in range(3)] for i in range(3)]
        self.P = self._mat_add(joseph, KRKt)
        self._symmetrize()

        self.reject_streak = 0
        return True

    def _update_vertical_and_floor(self, now_ms: int) -> None:
        v = self.x[1]

        if v >= self.params.move_up_speed_mps:
            self.vertical_state = "MOVING_UP"
            self.arrival_hold_start_ms = 0
        elif v <= self.params.move_down_speed_mps:
            self.vertical_state = "MOVING_DOWN"
            self.arrival_hold_start_ms = 0
        elif abs(v) <= self.params.arrival_speed_mps:
            if self.vertical_state in ("MOVING_UP", "MOVING_DOWN"):
                self.vertical_state = "ARRIVAL_HOLD"
                self.arrival_hold_start_ms = now_ms
            elif self.vertical_state == "ARRIVAL_HOLD":
                if now_ms - self.arrival_hold_start_ms >= self.params.arrival_hold_ms:
                    self.vertical_state = "IDLE"
            else:
                self.vertical_state = "IDLE"

        floor_raw = int(round(self.x[0] / self.params.floor_height_m))
        floor_raw = min(self.params.max_floor, max(self.params.min_floor, floor_raw))
        target_alt = floor_raw * self.params.floor_height_m
        near_target = abs(self.x[0] - target_alt) <= self.params.floor_change_band_m
        low_speed = abs(self.x[1]) <= self.params.arrival_speed_mps

        if floor_raw != self.floor_est and near_target and low_speed:
            if self.floor_candidate == floor_raw:
                self.floor_candidate_count += 1
            else:
                self.floor_candidate = floor_raw
                self.floor_candidate_count = 1

            if self.floor_candidate_count >= self.params.floor_change_confirm_samples:
                self.floor_est = floor_raw
                self.floor_candidate_count = 0
        elif floor_raw == self.floor_est:
            self.floor_candidate_count = 0

    def _update_confidence(self, now_ms: int) -> None:
        sigma_h = math.sqrt(max(self.P[0][0], 0.0))
        cov_score = max(0.0, min(1.0, 1.0 - sigma_h / 4.0))
        nis_score = max(0.0, min(1.0, 1.0 - self.last_nis / 20.0))

        age_ms = 0
        if self.last_ts_ms > 0:
            age_ms = max(0, now_ms - self.last_ts_ms)
        freshness = max(0.0, min(1.0, 1.0 - age_ms / 5000.0))

        reject_score = max(0.0, min(1.0, 1.0 - 0.2 * self.reject_streak))

        self.confidence = 0.45 * cov_score + 0.25 * nis_score + 0.20 * freshness + 0.10 * reject_score

    def ingest(self, s: Sample) -> Tuple[bool, ReplayPoint]:
        if not self.calibrated:
            self.calib_pressures.append(s.pressure_hpa)
            if len(self.calib_pressures) >= self.params.calibration_sample_count:
                sorted_p = sorted(self.calib_pressures)
                mid = len(sorted_p) // 2
                if len(sorted_p) % 2:
                    self.p_ref_hpa = sorted_p[mid]
                else:
                    self.p_ref_hpa = 0.5 * (sorted_p[mid - 1] + sorted_p[mid])
                self.calibrated = True
                self.last_ts_ms = s.timestamp_ms

            rp = ReplayPoint(
                timestamp_ms=s.timestamp_ms,
                pressure_hpa=s.pressure_hpa,
                temperature_c=s.temperature_c,
                h_m=self.x[0],
                v_mps=self.x[1],
                b_m=self.x[2],
                floor_est=self.floor_est,
                floor_gt=s.floor_gt,
                nis=self.last_nis,
                accepted=False,
                confidence=self.confidence,
            )
            return False, rp

        dt = 1.0
        if s.timestamp_ms > self.last_ts_ms:
            dt = (s.timestamp_ms - self.last_ts_ms) / 1000.0
        dt = min(2.0, max(0.2, dt))
        self.last_ts_ms = s.timestamp_ms

        self._predict(dt)
        accepted = self._update(s.pressure_hpa)
        self._update_vertical_and_floor(s.timestamp_ms)
        self._update_confidence(s.timestamp_ms)

        rp = ReplayPoint(
            timestamp_ms=s.timestamp_ms,
            pressure_hpa=s.pressure_hpa,
            temperature_c=s.temperature_c,
            h_m=self.x[0],
            v_mps=self.x[1],
            b_m=self.x[2],
            floor_est=self.floor_est,
            floor_gt=s.floor_gt,
            nis=self.last_nis,
            accepted=accepted,
            confidence=self.confidence,
        )
        return accepted, rp


def parse_csv(path: str) -> List[Sample]:
    samples: List[Sample] = []
    with open(path, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if not row.get("timestamp_ms"):
                continue
            floor_gt = None
            if row.get("floor_gt") not in (None, ""):
                floor_gt = int(float(row["floor_gt"]))

            samples.append(
                Sample(
                    timestamp_ms=int(float(row["timestamp_ms"])),
                    pressure_hpa=float(row["pressure_hpa"]),
                    temperature_c=float(row.get("temperature_c", 0.0) or 0.0),
                    seq=int(float(row.get("seq", 0) or 0)),
                    floor_gt=floor_gt,
                )
            )
    return samples


def run_replay(samples: Iterable[Sample], params: ReplayParams) -> Tuple[List[ReplayPoint], Dict[str, float]]:
    estimator = NodeFloorEstimator(params)
    points: List[ReplayPoint] = []

    accepted = 0
    rejected = 0
    floor_match_all = 0
    floor_total_all = 0
    floor_match_stationary = 0
    floor_total_stationary = 0

    for s in samples:
        accepted_now, p = estimator.ingest(s)
        points.append(p)
        if accepted_now:
            accepted += 1
        elif estimator.calibrated:
            rejected += 1

        if p.floor_gt is not None:
            floor_total_all += 1
            if p.floor_est == p.floor_gt:
                floor_match_all += 1
            if abs(p.v_mps) <= params.arrival_speed_mps:
                floor_total_stationary += 1
                if p.floor_est == p.floor_gt:
                    floor_match_stationary += 1

    floor_acc_all = (floor_match_all / floor_total_all) if floor_total_all else float("nan")
    floor_acc_stationary = (
        floor_match_stationary / floor_total_stationary if floor_total_stationary else float("nan")
    )

    summary = {
        "accepted_updates": float(accepted),
        "rejected_updates": float(rejected),
        "floor_accuracy_all": floor_acc_all,
        "floor_accuracy_stationary": floor_acc_stationary,
        "final_floor": float(points[-1].floor_est) if points else float("nan"),
        "final_altitude_m": float(points[-1].h_m) if points else float("nan"),
        "final_confidence": float(points[-1].confidence) if points else float("nan"),
    }
    return points, summary


def map_iekf_style_objective(points: List[ReplayPoint], params: ReplayParams) -> float:
    # MAP-style objective: weighted data fit + smoothness + parameter prior.
    if not points:
        return 1e9

    floor_mse = 0.0
    floor_count = 0
    vel_smooth = 0.0
    nis_penalty = 0.0

    prev_v = points[0].v_mps
    for p in points:
        if p.floor_gt is not None:
            err = p.floor_est - p.floor_gt
            floor_mse += err * err
            floor_count += 1
        dv = p.v_mps - prev_v
        vel_smooth += dv * dv
        prev_v = p.v_mps
        nis_penalty += max(0.0, p.nis - params.nis_gate)

    floor_term = floor_mse / max(1, floor_count)
    smooth_term = vel_smooth / max(1, len(points))
    nis_term = nis_penalty / max(1, len(points))

    # Weak Gaussian prior around nominal values.
    prior = (
        ((params.sigma_accel_process - 0.35) / 0.20) ** 2
        + ((params.sigma_bias_random_walk - 0.03) / 0.02) ** 2
        + ((params.sigma_pressure_meas - 0.12) / 0.08) ** 2
    )

    return 2.0 * floor_term + 0.5 * smooth_term + 0.2 * nis_term + 0.1 * prior


def map_iekf_style_tune(samples: List[Sample], init_params: ReplayParams, iterations: int = 5) -> Tuple[ReplayParams, float]:
    """
    Paper-inspired MAP/IEKF-style tuning:
    - Repeatedly replay with current parameters (relinearized each run)
    - Coordinate-search the key noise parameters
    - Pick params minimizing MAP-style objective
    """

    best = ReplayParams(**vars(init_params))
    points, _ = run_replay(samples, best)
    best_loss = map_iekf_style_objective(points, best)

    keys = ["sigma_accel_process", "sigma_bias_random_walk", "sigma_pressure_meas"]
    multipliers = [0.7, 1.0, 1.4]

    for _ in range(max(1, iterations)):
        improved = False
        for key in keys:
            current_value = getattr(best, key)
            local_best = best
            local_loss = best_loss

            for m in multipliers:
                candidate = ReplayParams(**vars(best))
                setattr(candidate, key, max(1e-4, current_value * m))

                cand_points, _ = run_replay(samples, candidate)
                loss = map_iekf_style_objective(cand_points, candidate)

                if loss < local_loss:
                    local_loss = loss
                    local_best = candidate

            if local_loss < best_loss:
                best = local_best
                best_loss = local_loss
                improved = True

        if not improved:
            break

    return best, best_loss


def write_points_csv(path: str, points: List[ReplayPoint]) -> None:
    fields = [
        "timestamp_ms",
        "pressure_hpa",
        "temperature_c",
        "h_m",
        "v_mps",
        "b_m",
        "floor_est",
        "floor_gt",
        "nis",
        "accepted",
        "confidence",
    ]
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader()
        for p in points:
            writer.writerow(
                {
                    "timestamp_ms": p.timestamp_ms,
                    "pressure_hpa": p.pressure_hpa,
                    "temperature_c": p.temperature_c,
                    "h_m": f"{p.h_m:.6f}",
                    "v_mps": f"{p.v_mps:.6f}",
                    "b_m": f"{p.b_m:.6f}",
                    "floor_est": p.floor_est,
                    "floor_gt": "" if p.floor_gt is None else p.floor_gt,
                    "nis": f"{p.nis:.6f}",
                    "accepted": int(p.accepted),
                    "confidence": f"{p.confidence:.6f}",
                }
            )


def generate_synthetic_elevator_samples(seed: int = 7) -> List[Sample]:
    random.seed(seed)

    p_ref = 1005.0
    floor_h = 3.0
    dt_ms = 1000
    total_seconds = 200

    def altitude_profile(t: int) -> float:
        # 0-40s: floor 0 hold (covers calibration window)
        if t < 40:
            return 0.0
        # 40-70s: move to floor 3
        if t < 70:
            return (t - 40) * (3.0 * floor_h / 30.0)
        # 70-110s: hold floor 3
        if t < 110:
            return 3.0 * floor_h
        # 110-140s: move down to floor 0
        if t < 140:
            return (140 - t) * (3.0 * floor_h / 30.0)
        # 140-200s: hold floor 0
        return 0.0

    samples: List[Sample] = []
    base_ts = 1_776_670_000_000

    bias_m = 0.0
    for t in range(total_seconds):
        true_h = altitude_profile(t)
        true_floor = int(round(true_h / floor_h))

        # Slow pressure-equivalent bias drift in altitude domain.
        bias_m += random.gauss(0.0, 0.005)
        h_for_pressure = true_h + bias_m

        term = max(0.05, min(1.5, 1.0 - h_for_pressure / 44330.0))
        pressure = p_ref * (term ** 5.255)
        pressure += random.gauss(0.0, 0.08)

        temp = 26.0 + random.gauss(0.0, 0.1)

        samples.append(
            Sample(
                timestamp_ms=base_ts + t * dt_ms,
                pressure_hpa=pressure,
                temperature_c=temp,
                seq=t + 1,
                floor_gt=true_floor,
            )
        )

    return samples


def run_self_test() -> int:
    print("[self-test] generating synthetic elevator trajectory...")
    samples = generate_synthetic_elevator_samples()

    base_params = ReplayParams()
    base_points, base_summary = run_replay(samples, base_params)

    tuned_params, tuned_loss = map_iekf_style_tune(samples, base_params, iterations=5)
    tuned_points, tuned_summary = run_replay(samples, tuned_params)

    print("[self-test] baseline summary:", base_summary)
    print("[self-test] tuned summary:", tuned_summary)
    print("[self-test] tuned params:", vars(tuned_params))
    print(f"[self-test] tuned objective={tuned_loss:.6f}")

    floor_acc_stationary = tuned_summary["floor_accuracy_stationary"]
    final_floor = int(tuned_summary["final_floor"])
    max_floor = max(p.floor_est for p in tuned_points)

    ok = True
    if math.isnan(floor_acc_stationary) or floor_acc_stationary < 0.80:
        print(f"[self-test][FAIL] stationary floor accuracy too low: {floor_acc_stationary}")
        ok = False
    if final_floor != 0:
        print(f"[self-test][FAIL] final floor expected 0 but got {final_floor}")
        ok = False
    if max_floor < 2:
        print(f"[self-test][FAIL] estimated max floor too low: {max_floor}")
        ok = False

    if ok:
        print("[self-test][PASS] estimator replay is usable.")
        return 0
    return 1


def print_summary(summary: Dict[str, float]) -> None:
    print("Replay summary:")
    for k, v in summary.items():
        print(f"  {k}: {v}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Replay EKF floor estimator and run MAP/IEKF-style tuning")
    parser.add_argument("--input", type=str, help="Input CSV path")
    parser.add_argument("--output", type=str, help="Output CSV path for replay points")
    parser.add_argument("--self-test", action="store_true", help="Run built-in synthetic self test")
    parser.add_argument("--tune-map-iekf", action="store_true", help="Run MAP/IEKF-style parameter tuning")
    parser.add_argument("--iterations", type=int, default=5, help="Tuning iterations")
    parser.add_argument("--export-params", type=str, help="Path to export tuned params as CSV")
    args = parser.parse_args()

    if args.self_test:
        return run_self_test()

    if not args.input:
        parser.error("--input is required unless --self-test is used")

    samples = parse_csv(args.input)
    if not samples:
        print("No valid samples in input.")
        return 1

    params = ReplayParams()
    if args.tune_map_iekf:
        params, loss = map_iekf_style_tune(samples, params, iterations=args.iterations)
        print(f"MAP/IEKF-style tuned objective: {loss:.6f}")
        print("Tuned params:")
        for k, v in vars(params).items():
            print(f"  {k}={v}")

    points, summary = run_replay(samples, params)
    print_summary(summary)

    if args.output:
        write_points_csv(args.output, points)
        print(f"Wrote replay points to: {args.output}")

    if args.export_params:
        with open(args.export_params, "w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(["param", "value"])
            for k, v in vars(params).items():
                writer.writerow([k, v])
        print(f"Exported params to: {args.export_params}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
