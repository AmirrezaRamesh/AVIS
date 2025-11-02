#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "std_msgs/msg/float32.hpp"

#include "pid.cpp"
#include "stanley.h"
#include "longitudinal.h"

using namespace std;

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

        double gain_yaw = this->get_parameter("gain_yaw").as_double();
        double gain_crosstack_error = this->get_parameter("gain_crosstack_error").as_double();
        double gain_ks = this->get_parameter("gain_ks").as_double();
        double steer_limit = this->get_parameter("steer_limit").as_double();
        double legal_error = this->get_parameter("legal_error").as_double();

        std::vector<int> area_range = this->get_parameter("area_range").as_integer_array();
        int average_range = this->get_parameter("average_range").as_int();
        double base_speed = this->get_parameter("base_speed").as_double();
        double decreasment_speed = this->get_parameter("decreasment_speed").as_double();
        std::vector<double> pid_gains = this->get_parameter("pid_gains").as_double_array();

        // ######## controller ########
        stan = std::make_unique<Stanley>(gain_yaw, gain_crosstack_error, gain_ks, steer_limit, legal_error);

        // ######## ROS config ########
        subscription_line_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
            "/line",
            10,
            std::bind(&ControlNode::line_callback, this, std::placeholders::_1));

        subscription_get_speed_ = this->create_subscription<std_msgs::msg::Float32>(
            "/distance",
            10,
            std::bind(&ControlNode::get_speed_callback, this, std::placeholders::_1)
    );


        // subscription_distance_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
        //     "/distance",
        //     10,
        //     std::bind(&ControlNode::distance_callback, this, std::placeholders::_1)
        // );

        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/actuate",
            10);

        init_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(200),
            std::bind(&ControlNode::init_controller, this));

        RCLCPP_INFO(this->get_logger(), "Control Node Started");
    }

private:
    void init_controller()
    {
        double gain_yaw, gain_crosstack_error, gain_ks, steer_limit, legal_error;

        this->get_parameter("gain_yaw", gain_yaw);
        this->get_parameter("gain_crosstack_error", gain_crosstack_error);
        this->get_parameter("gain_ks", gain_ks);
        this->get_parameter("steer_limit", steer_limit);
        this->get_parameter("legal_error", legal_error);

        stan = std::make_unique<Stanley>(gain_yaw, gain_crosstack_error, gain_ks, steer_limit, legal_error);

        std::vector<int> area_range;
        int average_range;
        double base_speed;
        double decreasment_speed;
        std::vector<double> pid_gains;

        this->get_parameter("area_range", area_range);
        this->get_parameter("average_range", average_range);
        this->get_parameter("base_speed", base_speed);
        this->get_parameter("decreasment_speed", decreasment_speed);
        this->get_parameter("pid_gains", pid_gains);

        longitudinal = std::make_unique<Longitudinal>(area_range, average_range, base_speed, decreasment_speed, pid_gains);

        RCLCPP_INFO(this->get_logger(),
                    "Stanley initialized with params:\n"
                    "  gain_yaw = %.3f\n"
                    "  gain_crosstack_error = %.3f\n"
                    "  gain_ks = %.3f\n"
                    "  steer_limit = %.3f\n"
                    "  legal_error = %.3f",
                    gain_yaw, gain_crosstack_error, gain_ks, steer_limit, legal_error);

        RCLCPP_INFO(this->get_logger(),
                    "Longitudinal initialized with:\n"
                    "  average_range = %d\n"
                    "  base_speed = %.2f\n"
                    "  decreasment_speed = %.2f\n"
                    "  area_range size = %zu\n"
                    "  pid_gains size = %zu",
                    average_range, base_speed, decreasment_speed,
                    area_range.size(), pid_gains.size());

        init_timer_->cancel();
    }

    void line_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
    {
        if (msg->data.size() < 3)
        {
            RCLCPP_WARN(this->get_logger(), "Received line data with less than 2 elements");
            return;
        }

        float offset = msg->data[0];
        float angle = msg->data[1];
        float curve = msg->data[2];

        controller(angle, offset, curve);
    }

    void get_speed_callback(const std_msgs::msg::Float32::SharedPtr msg)
    {
        // No need to check size — it's a single float
        get_speed_ = msg->data;
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
    void controller(float angle, float offset, float curve)
    {
        float real_speed = get_speed_;
        real_speed += longitudinal->set_speed(curve, real_speed);
        float speed = 2.2 * real_speed + 3.41;
        float steering = stan->calculate_steer(offset, angle, real_speed);

        stan->save_data(angle, offset, curve, real_speed, steering);

        // float steering = pid.get_pid(angle, 0.02);

        // RCLCPP_INFO(this->get_logger(), "data is %f", steering);

        std_msgs::msg::Float32MultiArray msg;
        msg.data = {speed, steering};
        publisher_->publish(msg);
    }

    // ###### ROS Objects #######
    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_line_;
    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_get_speed_;

    // rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr subscription_distance_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr init_timer_;

    //  float left_sensor_ = 1500, middle_sensor_=1500, right_sensor_=1500;

    std::unique_ptr<Stanley> stan;
    std::unique_ptr<Longitudinal> longitudinal;
    // PID pid;

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
