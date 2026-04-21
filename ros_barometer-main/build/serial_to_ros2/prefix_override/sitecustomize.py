import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/leo/floor_estimate/ros_barometer-main/install/serial_to_ros2'
