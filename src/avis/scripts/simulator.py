#!/usr/bin/env python3

import time
import cv2
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray
from cv_bridge import CvBridge
from avis import avisengine, config
 
class CarPublisher(Node):
    def __init__(self):
        super().__init__('car_publisher_node')

        # self.sensor_pub = self.create_publisher(Float32MultiArray, '/distance', 10)
        self.camera_pub = self.create_publisher(Image, '/camera', 10)
        self.create_subscription(Float32MultiArray, '/actuate', self.actuate_callback, 10)
        self.speed_pub = self.create_publisher(float, '/speed', 10)

        self.bridge = CvBridge()
        self.speed = 0.0
        self.steering = 0.0
        
        self.car = avisengine.Car()
        self.connect_to_simulator()

        time.sleep(2)

        self.get_logger().info('CarPublisher Node Started.')

        self.timer = self.create_timer(0.05, self.update)

    def actuate_callback(self, msg: Float32MultiArray):
        if len(msg.data) >= 2:
            self.speed, self.steering = msg.data[:2]
            
            self.car.setSpeed(self.speed)
            self.car.setSteering(self.steering)

            # self.get_logger().info(f'set: {self.speed}')
            # self.get_logger().info(f'get: {self.current_speed}')
            
        else:
            self.get_logger().warn('Received actuate data with less than 2 elements')

    def update(self):
        t1 = time.time()

        try:
            self.car.getData()
            # sensors = self.car.getSensors()  # [Left, Middle, Right]
            # self.car.setSensorAngle(40)
            self.current_speed = self.car.getSpeed()
            self.speed_pub.publish(self.current_speed)
            # Publish sensor data
            # sensor_msg = Float32MultiArray(data=[float(s) for s in sensors])
            # self.sensor_pub.publish(sensor_msg)

            image = self.car.getImage()
            if image is not None and image.size != 0:
                resized = cv2.resize(image, (1280, 720), interpolation=cv2.INTER_AREA)
                image_msg = self.bridge.cv2_to_imgmsg(resized, encoding='bgr8')
                self.camera_pub.publish(image_msg)

        except BrokenPipeError:
            self.get_logger().error("Broken pipe: simulator not ready or connection lost")
            self.connect_to_simulator()
        except Exception as e:
            self.get_logger().error(f"Error in update loop: {e}")
            self.connect_to_simulator()
            return

        f = 1.0 / max(1e-6, (time.time() - t1))
        # self.get_logger().info(f'Update frequency: {f:.2f} Hz')

    def connect_to_simulator(self):

        MAX_RETRIES = 30
        RETRY_DELAY = 1.0

        for attempt in range(MAX_RETRIES):
            try:
                self.car.connect(config.SIMULATOR_IP, config.SIMULATOR_PORT)
                self.get_logger().info("Connected to simulator successfully.")
                return
            except Exception as e:
                self.get_logger().warn(f"connect attempt {attempt+1}/{MAX_RETRIES} failed: {e}")
                time.sleep(RETRY_DELAY)

        self.get_logger().error("Failed to reconnect to simulator after several attempts.")

    def stop_car(self):
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
