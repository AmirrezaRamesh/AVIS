from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import TimerAction

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='avis',
            executable='simulator.py',
            name='simulator',
            output='screen'
        ),

        TimerAction(
            period=2.0,
            actions=[Node(
                package='avis',
                executable='line.py',
                name='line',
                output='screen'
            )]
        ),

        TimerAction(
            period=3.0,
            actions=[Node(
                package='avis',
                executable='control',
                name='control',
                output='screen'
            )]
        ),

        # TimerAction(
        #     period=4.0,
        #     actions=[Node(
        #         package='avis',
        #         executable='actuate.py',
        #         name='actuate',
        #         output='screen'
        #     )]
        # ),
    ])
