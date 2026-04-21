#!/bin/bash

# 找出所有匹配的串口端口
PORTS=$(platformio device list | awk 'BEGIN { RS=""; FS="\n" } /LOCATION=1-8/ { print $1 }')

# 检查是否找到了设备: 请替换 /LOCATION=1-8/ 为你实际的设备 port 地址!!!
if [ -z "$PORTS" ]; then
  echo "未找到匹配的设备 LOCATION=1-8* 的相关设备"
  exit 1
fi

echo "找到的设备端口: $PORTS"
echo "开始构建文件系统..."
platformio run --target buildfs --environment ESP32_Barometer

# 设置一个简单的锁机制函数
lock_port() {
  echo "🔒 锁定端口 $1 进行操作..."
}

unlock_port() {
  echo "🔓 解锁端口 $1，操作完成。"
}

# 对每个端口执行上传操作
for port in $PORTS; do
  lock_port "$port"
  
  echo "📤 上传文件系统到 $port ..."
  platformio run --target uploadfs --environment ESP32_Barometer --upload-port "$port"

  # 检查uploadfs是否成功，再上传固件
  if [ $? -eq 0 ]; then
    echo "📤 上传固件到 $port ..."
    platformio run --target upload --environment ESP32_Barometer --upload-port "$port"
    
    # 检查固件上传结果
    if [ $? -eq 0 ]; then
      echo "✅ 端口 $port 上传完成"
    else
      echo "❌ 端口 $port 固件上传失败"
    fi
  else
    echo "❌ 端口 $port 文件系统上传失败，跳过固件上传"
  fi
  
  unlock_port "$port"
  echo "------------------------"
done

echo "✅ 所有匹配设备上传过程已完成"