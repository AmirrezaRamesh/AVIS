#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

#include "pid.cpp"
#include "stanley.h"

using namespace std;

class ControlNode : public rclcpp::Node
{
public:
    ControlNode() : Node("control_node"),
                    stan(0.5, 0.8, 0.005, 35, -1),
                    pid(1, 0, 0, 30)
    {

        subscription_line_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
            "/line",
            10,
            std::bind(&ControlNode::line_callback, this, std::placeholders::_1));

        // subscription_distance_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
        //     "/distance",
        //     10,
        //     std::bind(&ControlNode::distance_callback, this, std::placeholders::_1)
        // );

        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/actuate",
            10);

        RCLCPP_INFO(this->get_logger(), "Control Node Started");

        // ######## controller ########

        // ######## controller ########
    }

private:
    void line_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 2)
        {
            RCLCPP_WARN(this->get_logger(), "Received line data with less than 2 elements");
            return;
        }

        float offset = msg->data[0];
        float angle = msg->data[1];

        controller(angle, offset);
    }

    // void distance_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    // {
    //     if (msg->data.size() < 3) {
    //         RCLCPP_WARN(this->get_logger(), "Received distance data with less than 3 elements");
    //         return;
    //     }

    //     left_sensor_ = msg->data[0];
    //     middle_sensor_ = msg->data[1];
    //     right_sensor_ = msg->data[2];

    //     distance_received_ = true;
    // }

    // #############  controller block  ############
    void controller(float angle, float offset)
    {
        // constant speed  version
        float speed = 30.0;
        float steering = stan.calculate_steer(offset, angle, speed);
        // float steering = pid.get_pid(angle, 0.02);

        // RCLCPP_INFO(this->get_logger(), "data is %f", steering);

        std_msgs::msg::Float32MultiArray msg;
        msg.data = {speed, steering};
        publisher_->publish(msg);
    }
    // #############  controller block  ############

    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_line_;
    // rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_distance_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;

    Stanley stan;
    PID pid;

    // 1500 means nothing has been detected
    //  float left_sensor_ = 1500, middle_sensor_=1500, right_sensor_=1500;
    //  bool distance_received_ = false;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
