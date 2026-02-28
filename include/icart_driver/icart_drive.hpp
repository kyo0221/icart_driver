#include <rclcpp/rclcpp.hpp>

#include <ypspur.h>

class IcartDriver : public rclcpp::Node
{
public:
    IcartDriver(const rclcpp::NodeOptions & options);
    ~IcartDriver();
private:
    void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
};