#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

class ControlNode : public rclcpp::Node
{
public:
    ControlNode() : Node("control_node")
    {

        subscription_line_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
            "/line",
            10,
            std::bind(&ControlNode::line_callback, this, std::placeholders::_1)
        );

        subscription_distance_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
            "/distance",
            10,
            std::bind(&ControlNode::distance_callback, this, std::placeholders::_1)
        );

        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
        "/actuate",
        10
        );

        RCLCPP_INFO(this->get_logger(), "Control Node Started");

    }

private:
    void line_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 2) {
            RCLCPP_WARN(this->get_logger(), "Received line data with less than 2 elements");
            return;
        }

        float offset = msg->data[0];
        float angle = msg->data[1];

        if (!distance_received_) {
            RCLCPP_WARN(this->get_logger(), "Distance data not received yet, skipping control");
            return;
        }

        controller(angle, offset, left_sensor_, middle_sensor_, right_sensor_);
    }

    void distance_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 3) {
            RCLCPP_WARN(this->get_logger(), "Received distance data with less than 3 elements");
            return;
        }

        left_sensor_ = msg->data[0];
        middle_sensor_ = msg->data[1];
        right_sensor_ = msg->data[2];

        distance_received_ = true; 
    }


    void controller(float angle, float offset, float l_s, float m_s, float r_s){
        std::cout<<"shayan its your turn";
        
        //test:
        float speed = 10.0;
        float steering = -10;
        std_msgs::msg::Float32MultiArray msg;
        msg.data = {speed, steering};
        publisher_->publish(msg);
    }

    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_line_;
    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_distance_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;

    //1500 means nothing has been detected
    float left_sensor_ = 1500, middle_sensor_=1500, right_sensor_=1500;
    bool distance_received_ = false;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
