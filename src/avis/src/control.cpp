#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "std_msgs/msg/float32.hpp"

#include "pid.h"
#include "stanley.h"
#include "longitudinal.h"

class ControlNode : public rclcpp::Node
{
public:
    ControlNode() : Node("control")
    {
        // ######## ROS params ########
        this->declare_parameter<double>("gain_yaw", 0.55);
        this->declare_parameter<double>("gain_crosstack_error", 0.2);
        this->declare_parameter<double>("gain_ks", 0.0);
        this->declare_parameter<double>("steer_limit", 40.0);
        this->declare_parameter<double>("legal_error", 20.0);

        this->declare_parameter<std::vector<int>>("area_range", std::vector<int>(10, 0));
        this->declare_parameter<int>("average_range", 0);
        this->declare_parameter<double>("base_speed", 0.0);
        this->declare_parameter<double>("decreasment_speed", 0.0);
        this->declare_parameter<std::vector<double>>("pid_gains", std::vector<double>(5, 0.0));

        // ######## ROS config ########
        subscription_line_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
            "/line", 10,
            std::bind(&ControlNode::line_callback, this, std::placeholders::_1));

        subscription_get_speed_ = this->create_subscription<std_msgs::msg::Float32>(
            "/speed", 10,
            std::bind(&ControlNode::get_speed_callback, this, std::placeholders::_1));

        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/actuate", 10);

        init_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(200),
            std::bind(&ControlNode::init_controller, this));

        RCLCPP_INFO(this->get_logger(), "Control Node Started");
    }

private:
    void init_controller()
    {
        // Retrieve Stanley params
        double gain_yaw, gain_crosstack_error, gain_ks, steer_limit, legal_error;
        this->get_parameter("gain_yaw", gain_yaw);
        this->get_parameter("gain_crosstack_error", gain_crosstack_error);
        this->get_parameter("gain_ks", gain_ks);
        this->get_parameter("steer_limit", steer_limit);
        this->get_parameter("legal_error", legal_error);

        stan = std::make_unique<Stanley>(
            static_cast<float>(gain_yaw),
            static_cast<float>(gain_crosstack_error),
            static_cast<float>(gain_ks),
            static_cast<float>(steer_limit),
            static_cast<float>(legal_error));

        // Retrieve Longitudinal params
        std::vector<long> area_range_long;
        int average_range;
        double base_speed, decreasment_speed;
        std::vector<double> pid_gains_vec;

        this->get_parameter("area_range", area_range_long);  
        this->get_parameter("average_range", average_range);
        this->get_parameter("base_speed", base_speed);
        this->get_parameter("decreasment_speed", decreasment_speed);
        this->get_parameter("pid_gains", pid_gains_vec);

        // Convert to fixed-size arrays
        int area_range[10] = {0};
        float pid_gains[5] = {0.0f};

        for (size_t i = 0; i < 10 && i < area_range_long.size(); ++i)
            area_range[i] = static_cast<int>(area_range_long[i]);  // convert long → int

        for (size_t i = 0; i < 5 && i < pid_gains_vec.size(); ++i)
            pid_gains[i] = static_cast<float>(pid_gains_vec[i]);

        longitudinal = std::make_unique<Longitudinal>(
            area_range,
            static_cast<float>(average_range),
            static_cast<float>(base_speed),
            static_cast<float>(decreasment_speed),
            pid_gains);

        RCLCPP_INFO(this->get_logger(),
                    "Stanley initialized. Longitudinal initialized.");
        init_timer_->cancel();
    }

    void line_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 3)
        {
            RCLCPP_WARN(this->get_logger(), "Received line data with less than 3 elements");
            return;
        }

        controller(msg->data[1], msg->data[0], msg->data[2]); // angle, offset, curve
    }

    void get_speed_callback(const std_msgs::msg::Float32::SharedPtr msg)
    {
        get_speed_ = msg->data;
    }

    void controller(float angle, float offset, float curve)
    {
        float real_speed = get_speed_;
        real_speed += longitudinal->set_speed(curve, real_speed);

        float speed = 2.2f * real_speed + 3.41f;
        float steering = stan->calculate_steer(offset, angle, real_speed);

        stan->save_data(angle, offset, curve, real_speed, steering);

        std_msgs::msg::Float32MultiArray msg;
        msg.data = {speed, steering};
        publisher_->publish(msg);
    }

    // ###### ROS Objects #######
    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_line_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr subscription_get_speed_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr init_timer_;

    std::unique_ptr<Stanley> stan;
    std::unique_ptr<Longitudinal> longitudinal;

    float get_speed_ = 0.0;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
