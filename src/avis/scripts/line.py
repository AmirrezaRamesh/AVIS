#!/usr/bin/env python3
from avis.yegane import LaneDetector

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray
from cv_bridge import CvBridge
import cv2


class LineNode(Node):
    def __init__(self):
        super().__init__('line_node')

        self.camera_sub = self.create_subscription(Image, '/camera', self.camera_callback, 10)
        self.line_pub = self.create_publisher(Float32MultiArray, '/line', 10)

        self.bridge = CvBridge()
        self.lane_detector = LaneDetector()   

        self.get_logger().info('Line Node Started.')

    def camera_callback(self, msg):

        try:
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
            self.process_image(cv_image)
        except Exception as e:
            self.get_logger().error(f"Camera callback error: {e}")

    def publish_line(self, angle, offset):

        msg = Float32MultiArray()
        msg.data = [float(angle), float(offset)]
        self.line_pub.publish(msg)
        self.get_logger().info(f'Published line: [angle={angle:.2f}, offset={offset:.2f}]')

    def process_image(self, image):

        try:
            road, distance, degree = self.lane_detector.fit_polynomial(
                self.lane_detector.line_detection(image)
            )

            cv2.imshow("Detected Lanes", road)
            cv2.waitKey(1)

            self.publish_line(degree, distance)

        except Exception as e:
            self.get_logger().error(f"Lane detection failed: {e}")

            msg = Float32MultiArray()
            msg.data = [0.0, 0.0]
            self.line_pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = LineNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()
        cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
