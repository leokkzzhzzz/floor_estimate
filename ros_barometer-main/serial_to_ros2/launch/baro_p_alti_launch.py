import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument)
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import  Node



def generate_launch_description():
    share_dir = get_package_share_directory('serial_to_ros2')

    
    esp32_serial_baro_params_file = DeclareLaunchArgument(
        'esp32_serial_baro_params_file',
        default_value=os.path.join(
            share_dir, 'config', 'esp32_serial_baro.yaml'),
        description='File path to the ROS2 parameters file to use'
    )
    esp32_serial_baro = Node(
        package='serial_to_ros2',
        executable='esp32_serial_baro',
        name='esp32_serial_baro',
        namespace='',
        parameters=[LaunchConfiguration('esp32_serial_baro_params_file')],
        output='screen',
    )
    return LaunchDescription([
        esp32_serial_baro_params_file,
        esp32_serial_baro
    ])