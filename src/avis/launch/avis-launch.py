from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import TimerAction

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='avis',
            executable='simulator.py',
            name='simulator',
            output='screen',
            respawn=True,
        ),

        TimerAction(
            period=2.0,
            actions=[Node(
                package='avis',
                executable='line.py',
                name='line',
                output='log',
                respawn=True,
            )]
        ),

        TimerAction(
            period=3.0,
            actions=[Node(
                package='avis',
                executable='control',
                name='control',
                output='log',
                respawn=True,
                parameters=[{
                    'gain_yaw': 0.55,
                    'gain_crosstack_error': 0.08,
                    'gain_ks': 0.0,
                    'steer_limit': 70.0,
                    'legal_error': 100.0,

                    'area_range': [10, 12, 15, 18, 22, 30, 40, 45, 50, 55],
                    'average_range': 5,
                    'base_speed': 120.0,
                    'decreasment_speed': 10.5,
                    'pid_gains': [0.5, 0.1, 0.05, 0.0, 0.0],
                }]
            )]
        ),
    ])
