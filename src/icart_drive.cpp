#include "icart_driver/icart_drive.hpp"

IcartDriver::IcartDriver(const rclcpp::NodeOptions & options)
: Node("icart_drive", options)
{
    cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10, std::bind(&IcartDriver::cmd_vel_callback, this, std::placeholders::_1));
}

IcartDriver::~IcartDriver()
{
    Spur_stop();
}

void IcartDriver::cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    Spur_vel(msg->linear.x, msg->angular.z);
}