# Floor_estimate
The method of floor estimate.
该项目复现这篇论文，并且这些内容也是由论文里面的仓库拉出来的，我在这里面的改动是加了一个calc_offsets_eq89.py的计算偏移量的代码，还加了一个baro_p_alti_launch.py这个估计楼层高度的代码
# ESP32-S3 + BMP390 配置流程（已实测）
## 1. 适用范围

本流程适用于你当前本地工程：

- `/home/leo/floor_estimate/ESP32_Barometer-main`
- `/home/leo/floor_estimate/ros_barometer-main`

确认这些部分已经改好配置：
```
line47
/home/leo/floor_estimate/ESP32_Barometer-main/platformio.ini
[env:barometer_node_s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200

lib_deps =
    Wire
    ArduinoJson
    adafruit/Adafruit BMP3XX Library
    adafruit/Adafruit BusIO
    adafruit/Adafruit Unified Sensor
build_flags =
    -D BARO_I2C_SDA_PIN=41 #选板子上的空闲的GOPIO口 用于初始化输出
    -D BARO_I2C_SCL_PIN=42 #选板子上的空闲的GOPIO口 
    -D ARDUINO_USB_MODE=1  
    -D ARDUINO_USB_CDC_ON_BOOT=1 #让 Serial 日志走 USB CDC（/dev/ttyACM0）可直接看
extra_scripts = pre:prebuild.py
```

```
line66
/home/leo/floor_estimate/ESP32_Barometer-main/platformio.ini
[env:barometer_base_s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200

lib_deps =
    Wire
    ArduinoJson
    adafruit/Adafruit BMP3XX Library
    adafruit/Adafruit BusIO
    adafruit/Adafruit Unified Sensor
build_flags =
    -D BARO_I2C_SDA_PIN=41
    -D BARO_I2C_SCL_PIN=42
    -D ARDUINO_USB_MODE=1
    -D ARDUINO_USB_CDC_ON_BOOT=1
extra_scripts = pre:prebuild.py
```

并且硬件为：

- ESP32-S3（USB 串口设备：`/dev/ttyACM0`）

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/QxqQs7HG/a7eb00f84350e193f899509318e7ce18.png' border='0' alt='a7eb00f84350e193f899509318e7ce18'></a>

- BMP390（I2C）

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/Dw2ty16z/3912ed50af568746d0c0d930a4c6f74d.png' border='0' alt='3912ed50af568746d0c0d930a4c6f74d'></a>

## 2. 接线（单板）

- `3V3 -> BMP390 VCC`
- `GND -> BMP390 GND`
- `GPIO41 -> BMP390 SDA`
- `GPIO42 -> BMP390 SCL`
- `CS -> 3V3`（固定 I2C）
- `SDO -> GND`（地址 `0x76`）或 `3V3`（地址 `0x77`）

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/6Qg0skzC/jie-tu-2026-04-08-17-26-09.png' border='0' alt='jie-tu-2026-04-08-17-26-09'></a>



说明：如果你只有一个 3V3/GND，可分线并联给 VCC/CS 和 GND/SDO。

## 3. 一次性安装依赖

```bash
python3 -m pip install --user -U platformio aiohttp pyserial-asyncio
```

ROS2（Humble）环境（若还没 source）：

```bash
source /opt/ros/humble/setup.bash
```

## 4. 固件工程关键点

当前工程已包含以下 S3 环境：

- `barometer_node_s3`（单板串口输出）
- `barometer_base_s3`（基站 Wi-Fi 推送）

并且已启用：

- `BARO_I2C_SDA_PIN=41`
- `BARO_I2C_SCL_PIN=42`
- `ARDUINO_USB_CDC_ON_BOOT=1`

以及预构建脚本：

- `extra_scripts = pre:prebuild.py`

## 5. 单板模式：编译与烧录

进入固件工程：

```bash
cd /home/leo/floor_estimate/ESP32_Barometer-main
```

编译：

```bash
python3 -m platformio run -e barometer_node_s3
```

烧录：

```bash
python3 -m platformio run -e barometer_node_s3 -t upload --upload-port /dev/ttyACM0
```

## 6. 单板模式：串口验证

读取串口（12 秒）：

```bash
stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb -ixon -ixoff raw -echo
timeout 12s cat /dev/ttyACM0
```

期望看到：

- `BAROD>timestamp,pressure_hpa,temp_c`
- `BAROT>mac`

你当前实测样例：

```text
BAROD>...,1006.04,28.04
BAROT>24:EC:4A:01:43:20
```

## 7. 时间同步

向板子发一次时间同步命令：

```bash
python3 - <<'PY'
import serial, time
port='/dev/ttyACM0'
now_ms=int(time.time()*1000)
with serial.Serial(port,115200,timeout=1) as s:
    s.write(f'TS>{now_ms}\n'.encode())
    s.flush()
    for _ in range(8):
        line=s.readline().decode(errors='ignore').strip()
        if line:
            print(line)
PY
```

期望看到：

- `DEBUG: Received time sync request: TS>...`
- 后续 `BAROD>` 时间戳变为正常当前毫秒时间。

## 8. ROS2 端接收（单板 self-relative）

### 8.1 构建 ROS 工作区

```bash
source /opt/ros/humble/setup.bash
cd /home/leo/floor_estimate/ros_barometer-main
colcon build --symlink-install
source install/setup.bash
```

### 8.2 启动串口桥接节点

```bash
source /opt/ros/humble/setup.bash
source /home/leo/floor_estimate/ros_barometer-main/install/setup.bash
ros2 run serial_to_ros2 esp32_serial_baro --ros-args -p serial_port:=/dev/ttyACM0 -p output_mode:=self-relative -p default_local_pressure:=1005 #参考压换成当地压强
```

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/NMzHL8mW/jie-tu-2026-04-08-15-43-29.png' border='0' alt='jie-tu-2026-04-08-15-43-29'></a>

### 8.3 查看话题数据

新终端执行：

```bash
source /opt/ros/humble/setup.bash
source /home/leo/floor_estimate/ros_barometer-main/install/setup.bash
ros2 topic echo /barometer --once
```
实测样例：

```text
header:
  stamp:
    sec: 1775629956
    nanosec: 165000000
  frame_id: barometer_link
altitude: 79.3833
pressure: 100564.0
temperature: 29.49
```
### 不同楼层测试气压

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/WprsK6tM/jie-tu-2026-04-08-15-52-13.png' border='0' alt='jie-tu-2026-04-08-15-52-13'></a>

- 得到在一楼气压稳定为1006.00 在三楼气压大概为1005.51

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/XYPh4mz3/jie-tu-2026-04-08-15-53-09.png' border='0' alt='jie-tu-2026-04-08-15-53-09'></a>

- 下一步将一楼的稳定气压固定为本地气压输入指令可得三楼高度ros2 run serial_to_ros2 esp32_serial_baro --ros-args -p serial_port:=/dev/ttyACM0 -p output_mode:=self-relative -p default_local_pressure:=1006.00 #参考压换成当地压强


## 9. 双板差分模式（base-relative）

## 9.1 基站板固件

```bash
cd /home/leo/floor_estimate/ESP32_Barometer-main
python3 -m platformio run -e barometer_base_s3 -t upload --upload-port /dev/ttyACM0
```
说明：`barometer_base_s3` 需要在 `src/config.cpp` 填好 Wi-Fi 账号密码。

文件：

- `/home/leo/floor_estimate/ESP32_Barometer-main/src/config.cpp`
- `const char *SSID_IOT = "xxx"`
- `const char *SSID_IOT_PASSWORD = "xxxxxxxx"`

烧录固件后启动串口监测
-  `python3 -m platformirun -e barometer_base_s3 -t upload --upload-port /dev/ttyACM1` 
- ` python3 -m platformio device monitor -p /dev/ttyACM1 -b 115200` #要得到base_ip地址

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/W3FP4QBZ/jie-tu-2026-04-09-09-38-04.png' border='0' alt='jie-tu-2026-04-09-09-38-04'></a>

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/N0TR6MqS/jie-tu-2026-04-09-10-30-13.png' border='0' alt='jie-tu-2026-04-09-10-30-13'></a>

得到的base ip后续要填到移动端里面的config.cpp里，让移动端和基站建立联系：

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/5t64FyqK/jie-tu-2026-04-20-16-00-44.png' border='0' alt='jie-tu-2026-04-20-16-00-44'></a>



## 9.2 ROS 参数

在 ROS 侧使用：

- `output_mode:=base-relative`
- `base_ip:=<基站IP>` 
- `source /opt/ros/humble/setup.bash`
- `source install/setup.bash`
- `ros2 run serial_to_ros2 esp32_serial_baro --ros-args -p serial_port:=/dev/ttyACM1 -p output_mode:=base-relative -p base_ip:=192.168.100.42 -p default_local_pressure:=1006.90  `
- `ros2 launch serial_to_ros2 baro_p_alti_launch.py esp32_serial_baro_params_file:=/home/leo/floor_estimate/ros_barometer-main/serial_to_ros2/config/esp32_serial_baro.yaml`
- `python3 /home/leo/floor_estimate/ESP32_Barometer-main/tools/realtime_floor_validation.py     --duration 120     --live-interval 1     --floor-height 3     --floor-count 5`

<a href='https://postimages.org/' target='_blank'><img src='https://i.postimg.cc/SK6Fw6bf/jie-tu-2026-04-09-11-00-30.png' border='0' alt='jie-tu-2026-04-09-11-00-30'></a>
