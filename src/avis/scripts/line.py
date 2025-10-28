#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray
from cv_bridge import CvBridge

class LineNode(Node):
    def __init__(self):
        super().__init__('line_node')

        self.camera_sub = self.create_subscription(Image, '/camera', self.camera_callback, 10)

        self.line_pub = self.create_publisher(Float32MultiArray, '/line', 10)

        self.bridge = CvBridge()    

        self.get_logger().info('Line Node Started.')


    def camera_callback(self, msg):
        
        cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        self.process_image(cv_image)

    def publish_line(self, angle, offset):

        msg = Float32MultiArray()
        msg.data = [float(angle), float(offset)]
        self.line_pub.publish(msg)
        self.get_logger().info(f'Published line: [{angle}, {offset}]')

    def process_image(self, image):
        print('yegane its your turn')
        #test:
        msg = Float32MultiArray()
        msg.data = [0.0, 0.0]
        self.line_pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = LineNode()

    try:
        rclpy.spin(node)

    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
