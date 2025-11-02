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
                    'gain_yaw': 0.0,
                    'gain_crosstack_error': 0.5,
                    'gain_ks': 0.0005,
                    'steer_limit': 70.0,
                    'legal_error': 100.0,

                    'area_range': [40, 80, 120, 160, 200, 240, 280, 320, 360, 400],
                    'average_range': 20,
                    'base_speed': 40.0,
                    'decreasment_speed': 1.0,
                    'pid_gains': [0.02, 0.0, 0.0, 5.0, 0.01],
                }]
            )]
        ),
    ])
