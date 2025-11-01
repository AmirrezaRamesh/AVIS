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
<<<<<<< HEAD
<<<<<<< HEAD
                    'gain_yaw': 0.55,
                    'gain_crosstack_error': 0.2,
=======
                    'gain_yaw': 0.65,
                    'gain_crosstack_error': 0.08,
>>>>>>> f6bbdc10739b01f95bc001c6680ef42096ca8900
=======
                    'gain_yaw': 0.7,
                    'gain_crosstack_error': 0.04,
>>>>>>> 226c0cb8f434cd5370b29c995b65c852aba09c6b
                    'gain_ks': 0.001,
                    'steer_limit': 70.0,
                    'legal_error': 100.0
                }]
            )]
        ),
    ])
