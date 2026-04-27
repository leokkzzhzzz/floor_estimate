#!/usr/bin/env python3
"""
Relay base-station ESP32 barometer POSTs to the robot over Tailscale.

Input endpoint:
  POST /data
  Headers:
    Device-Mac: <base_mac>
  JSON:
    {
      "timestamp_ms": 1777267439065,
      "pressure_hpa": 1005.79,
      "temperature_c": 29.2
    }

Default output:
  Set with --robot-url or ROBOT_URL, for example:
  http://<robot_tailscale_ip>:18080/data
"""

from __future__ import annotations

import argparse
import logging
import math
import os
import time
from typing import Any

from aiohttp import ClientError, ClientSession, ClientTimeout, web


DEFAULT_LISTEN_HOST = "0.0.0.0"
DEFAULT_LISTEN_PORT = 8080
DEFAULT_ROBOT_URL = ""
DEFAULT_TIMEOUT_S = 5.0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Relay ESP32 barometer POSTs to a robot Tailscale endpoint.")
    parser.add_argument("--listen-host", default=DEFAULT_LISTEN_HOST)
    parser.add_argument("--listen-port", type=int, default=DEFAULT_LISTEN_PORT)
    parser.add_argument(
        "--robot-url",
        default=os.getenv("ROBOT_URL", DEFAULT_ROBOT_URL),
        help=(
            "Robot receiver URL, e.g. http://<robot_tailscale_ip>:18080/data. "
            "Can also be set by ROBOT_URL."
        ),
    )
    parser.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT_S)
    parser.add_argument("--log-level", default="INFO", choices=["DEBUG", "INFO", "WARNING", "ERROR"])
    args = parser.parse_args()
    if not args.robot_url:
        parser.error("--robot-url or ROBOT_URL is required, e.g. http://<robot_tailscale_ip>:18080/data")
    return args


def validate_payload(data: dict[str, Any]) -> tuple[bool, str]:
    required = ("timestamp_ms", "pressure_hpa", "temperature_c")
    missing = [key for key in required if key not in data]
    if missing:
        return False, f"missing fields: {', '.join(missing)}"

    try:
        timestamp_ms = int(data["timestamp_ms"])
        pressure_hpa = float(data["pressure_hpa"])
        temperature_c = float(data["temperature_c"])
    except (TypeError, ValueError) as exc:
        return False, f"invalid field type: {exc}"

    if timestamp_ms <= 0:
        return False, "timestamp_ms must be positive"
    if math.isnan(pressure_hpa) or math.isinf(pressure_hpa) or not (900.0 <= pressure_hpa <= 1100.0):
        return False, f"pressure_hpa out of range: {pressure_hpa}"
    if math.isnan(temperature_c) or math.isinf(temperature_c) or not (-30.0 <= temperature_c <= 60.0):
        return False, f"temperature_c out of range: {temperature_c}"

    return True, ""


async def handle_health(request: web.Request) -> web.Response:
    return web.json_response(
        {
            "status": "ok",
            "robot_url": request.app["robot_url"],
            "uptime_s": round(time.monotonic() - request.app["started_at"], 3),
        }
    )


async def handle_data(request: web.Request) -> web.Response:
    robot_url: str = request.app["robot_url"]
    session: ClientSession = request.app["session"]
    logger: logging.Logger = request.app["logger"]

    try:
        data = await request.json()
    except Exception as exc:
        logger.warning("invalid JSON from %s: %s", request.remote, exc)
        return web.json_response({"status": "error", "message": "invalid JSON"}, status=400)

    if not isinstance(data, dict):
        return web.json_response({"status": "error", "message": "JSON body must be an object"}, status=400)

    valid, reason = validate_payload(data)
    if not valid:
        logger.warning("invalid payload from %s: %s data=%r", request.remote, reason, data)
        return web.json_response({"status": "error", "message": reason}, status=400)

    mac = request.headers.get("Device-Mac", "").strip()
    headers = {"Device-Mac": mac} if mac else {}

    try:
        async with session.post(robot_url, json=data, headers=headers) as resp:
            text = await resp.text()
            logger.info(
                "forwarded mac=%s pressure=%.3f temp=%.3f -> %s status=%d",
                mac or "-",
                float(data["pressure_hpa"]),
                float(data["temperature_c"]),
                robot_url,
                resp.status,
            )
            return web.Response(text=text, status=resp.status, content_type=resp.content_type)
    except TimeoutError:
        logger.error("timeout forwarding to %s", robot_url)
        return web.json_response({"status": "error", "message": "robot endpoint timeout"}, status=504)
    except ClientError as exc:
        logger.error("forward failed to %s: %s", robot_url, exc)
        return web.json_response({"status": "error", "message": str(exc)}, status=502)


async def on_startup(app: web.Application) -> None:
    timeout = ClientTimeout(total=app["timeout_s"])
    app["session"] = ClientSession(timeout=timeout)


async def on_cleanup(app: web.Application) -> None:
    session: ClientSession | None = app.get("session")
    if session:
        await session.close()


def create_app(robot_url: str, timeout_s: float, logger: logging.Logger) -> web.Application:
    app = web.Application()
    app["robot_url"] = robot_url
    app["timeout_s"] = timeout_s
    app["logger"] = logger
    app["started_at"] = time.monotonic()
    app.on_startup.append(on_startup)
    app.on_cleanup.append(on_cleanup)
    app.router.add_get("/health", handle_health)
    app.router.add_post("/data", handle_data)
    return app


def main() -> int:
    args = parse_args()
    logging.basicConfig(
        level=getattr(logging, args.log_level),
        format="%(asctime)s %(levelname)s %(message)s",
    )
    logger = logging.getLogger("relay_to_robot")
    logger.info("relay listening on %s:%d", args.listen_host, args.listen_port)
    logger.info("forwarding /data to %s", args.robot_url)

    app = create_app(args.robot_url, args.timeout, logger)
    web.run_app(app, host=args.listen_host, port=args.listen_port)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
