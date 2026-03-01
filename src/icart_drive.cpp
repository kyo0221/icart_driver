#include "icart_driver/icart_drive.hpp"

IcartDriver::IcartDriver(const rclcpp::NodeOptions & options)
: Node("icart_driver", options),
interval_ms(get_parameter("interval_ms").as_int()),
linear_max_vel(get_parameter("linear_max.vel").as_double()),
linear_max_acc(get_parameter("linear_max.acc").as_double()),
angular_max_vel(get_parameter("angular_max.vel").as_double()),
angular_max_acc(get_parameter("angular_max.acc").as_double())
{
    cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        "cmd_vel", 10, std::bind(&IcartDriver::cmd_vel_callback, this, std::placeholders::_1));

    restart_sub_ = this->create_subscription<std_msgs::msg::Empty>(
        "restart", 10, std::bind(&IcartDriver::restart_callback, this, std::placeholders::_1));

    emergency_sub_ = this->create_subscription<std_msgs::msg::Empty>(
        "emergency", 10, std::bind(&IcartDriver::emergency_callback, this, std::placeholders::_1));
    
    bringup_ypspur();

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(interval_ms),
        std::bind(&IcartDriver::loop, this));
}

IcartDriver::~IcartDriver()
{
    Spur_free();
}

void IcartDriver::init_params()
{
    Spur_set_vel(linear_max_vel);
    Spur_set_angvel(angular_max_vel);
    Spur_set_accel(linear_max_acc);
    Spur_set_angaccel(angular_max_acc);

    Spur_set_pos_GL(0, 0, 0);
    Spur_free();
}

void IcartDriver::bringup_ypspur()
{
    if(Spur_init() == 1){
        RCLCPP_INFO(this->get_logger(), "Bring up ypspur");
        ypspur_flag_ = true;
        init_params();
    }else{
        RCLCPP_ERROR(this->get_logger(), "Disconnected ypspur");
    }
}

void IcartDriver::cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    Spur_vel(msg->linear.x, msg->angular.z);
}

void IcartDriver::restart_callback(const std_msgs::msg::Empty::SharedPtr msg)
{
    bringup_ypspur();
    Spur_unfreeze();
    RCLCPP_INFO(this->get_logger(), "Restart ypspur");
}

void IcartDriver::emergency_callback(const std_msgs::msg::Empty::SharedPtr msg)
{
    Spur_freeze();
    RCLCPP_INFO(this->get_logger(), "Emergency stop ypspur");
}

void IcartDriver::loop()
{
    if(!ypspur_flag_)    bringup_ypspur();
}