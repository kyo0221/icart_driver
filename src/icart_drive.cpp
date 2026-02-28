#include "icart_driver/icart_drive.hpp"

IcartDriver::IcartDriver(const rclcpp::NodeOptions & options)
: Node("icart_drive", options),
interval_ms(get_parameter("interval_ms").as_int()),
linear_max(get_parameter("linear_max.vel").as_double()),
angular_max(get_parameter("angular_max.vel").as_double())
{
    cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10, std::bind(&IcartDriver::cmd_vel_callback, this, std::placeholders::_1));

    read_params();
}

IcartDriver::~IcartDriver()
{
    Spur_stop();
}

IcartDriver::read_params()
{
    int port;
    this->declare_parameter("port", 0);
    this->get_parameter("port", port);
    Spur_init(port);
}

void IcartDriver::cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    Spur_vel(msg->linear.x, msg->angular.z);
}