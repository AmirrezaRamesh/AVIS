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
            respawn_delay=5.0
        ),

        TimerAction(
            period=2.0,
            actions=[Node(
                package='avis',
                executable='line.py',
                name='line',
                output='log',
                respawn=True,
                respawn_delay=5.0
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
                respawn_delay=5.0,
                parameters=[{
                    'gain_crosstack_error': 0.35,
                    'gain_crosstack_error': 0.2,
                    'gain_ks': 0.0,
                    'steer_limit': 60.0,
                    'legal_error': 100.0
                }]
            )]
        ),
    ])
