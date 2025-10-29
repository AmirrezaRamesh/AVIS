#!/usr/bin/env python3

from avis import avisengine
from avis import config
from avis import utils

import time
import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray
from cv_bridge import CvBridge

class CarPublisher(Node):
    def __init__(self):
        super().__init__('car_publisher_node')

        # Publishers
        self.camera_pub = self.create_publisher(Image, '/camera', 10)
        self.sensor_pub = self.create_publisher(Float32MultiArray, '/distance', 10)

        # Subscriber for actuate commands
        self.actuate_sub = self.create_subscription(
            Float32MultiArray,
            '/actuate',
            self.actuate_callback,
            10
        )

        # OpenCV bridge
        self.bridge = CvBridge()

        # Car connection
        self.car = avisengine.Car()
        connected = False
        for i in range(5):
            try:
                self.car.connect(config.SIMULATOR_IP, config.SIMULATOR_PORT)
                connected = True
                break
            except Exception as e:
                self.get_logger().warn(f"Failed to connect to simulator, retrying ({i+1}/5)...")
                time.sleep(1)

        if not connected:
            self.get_logger().error("Could not connect to simulator. Node will exit.")
            raise RuntimeError("Simulator connection failed")

        # Initialize actuate variables
        self.speed = 0.0
        self.steering = 0.0

        # Timer for update loop
        self.timer = self.create_timer(0.1, self.update)

        # Ensure simulator is ready
        time.sleep(3)

        self.get_logger().info('CarPublisher Node Started.')

    def actuate_callback(self, msg: Float32MultiArray):
        """Receive steering and speed commands from /actuate."""
        if len(msg.data) < 2:
            self.get_logger().warn('Received actuate data with less than 2 elements')
            return

        # msg.data = [steering, speed]
        self.speed = msg.data[0]
        self.steering = msg.data[1]

    def update(self):
        t1 = time.time()
        try:
            self.car.getData()
            sensors = self.car.getSensors()  # [Left, Middle, Right]
            image = self.car.getImage()
            self.car.setSensorAngle(45)  # Optional

            # Publish sensor data
            sensor_msg = Float32MultiArray()
            sensor_msg.data = [float(s) for s in sensors]
            self.sensor_pub.publish(sensor_msg)

            # Publish camera image
            if image is not None and image.any():
                image = cv2.resize(image, (1280, 720))
                image_msg = self.bridge.cv2_to_imgmsg(image, encoding='bgr8')
                self.camera_pub.publish(image_msg)

            # Apply actuate commands
            try:
                self.car.setSpeed(self.speed)
                self.car.setSteering(self.steering)
            except BrokenPipeError:
                self.get_logger().error("Broken pipe: simulator not ready or connection lost")

        except Exception as e:
            self.get_logger().error(f"Error in update loop: {e}")
        #frequncy
        f = 1/(time.time() - t1)
        print(f)

    def stop_car(self):
        """Stop the car safely."""
        try:
            self.car.stop()
        except Exception:
            pass
        self.get_logger().info('Car stopped and node shutting down.')

def main(args=None):
    rclpy.init(args=args)
    node = CarPublisher()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.stop_car()
        node.destroy_node()
        rclpy.shutdown()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
