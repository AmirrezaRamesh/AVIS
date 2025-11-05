# A ROS2 project for AV simulation on AVIS engine

### how to run : 
1. download the AVIS 1.2.4 from [their website](https://www.avisengine.com) and run the simulator
2. start the server from the panel.
3. clone our project and `cd` inside it, then build it using `colcon build` and also source it using `source ./install/setup.bash`
4. run the command `ros2 launch avis avis-launch.py`. \
P.S. : ROS2 version is `humble`, `OpenCV` and `numpy` are also  prerequisites of the project.

### about the project
this was a task from auriga robotics team with a deadline of a week. no further improvments are planned at the moment since the simulator doesn't offer much to work with.

### contributors
- Yegane Rahmati : line detection
- Shayan Setayandeh : control algorithms
- Amirreza Ramesh : ros system


