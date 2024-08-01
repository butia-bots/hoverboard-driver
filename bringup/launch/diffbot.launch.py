import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
import launch_ros.parameter_descriptions
from launch_ros.substitutions import FindPackageShare
import launch_ros.actions

def generate_launch_description():
    port_param = DeclareLaunchArgument('port', default_value='/dev/ttySHARK')

    hardware_yaml = os.path.join(
        get_package_share_directory('hoverboard_driver'),
        'config',
        'hardware.yaml'
    )

    controllers_yaml = os.path.join(
        get_package_share_directory('hoverboard_driver'),
        'config',
        'controllers.yaml'
    )

    controllers_yaml = PathJoinSubstitution(
        [
            FindPackageShare("hoverboard_driver"),
            "config",
            "hoverboard_controllers.yaml",
        ]
    )

    hoverboard_driver_node = Node(
        package='hoverboard_driver',
        executable='hoverboard_driver',
        name='hoverboard_driver',
        output='screen',
        parameters=[hardware_yaml, controllers_yaml, {'port': LaunchConfiguration('port')}],
        remappings=[('/hoverboard_base_controller/cmd_vel_unstamped', '/cmd_vel')]
    )

    controller_spawner_node = Node(
        package='controller_manager',
        executable='spawner',
        name='controller_spawner',
        output='screen',
        arguments=['hoverboard_joint_publisher', 'hoverboard_velocity_controller']
    )

    return LaunchDescription([
        port_param,
        hoverboard_driver_node,
        controller_spawner_node
    ])
