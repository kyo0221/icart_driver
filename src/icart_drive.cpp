#include "icart_driver/icart_drive.hpp"

#include <tf2/LinearMath/Quaternion.h>

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

    odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/odom", 10);
    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    
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
    (void)msg;
    bringup_ypspur();
    Spur_unfreeze();
    RCLCPP_INFO(this->get_logger(), "Restart ypspur");
}

void IcartDriver::emergency_callback(const std_msgs::msg::Empty::SharedPtr msg)
{
    (void)msg;
    Spur_freeze();
    RCLCPP_INFO(this->get_logger(), "Emergency stop ypspur");
}

void IcartDriver::publish_odom()
{
    double x = 0.0;
    double y = 0.0;
    double yaw = 0.0;
    double linear_vel = 0.0;
    double angular_vel = 0.0;

    Spur_get_pos_GL(&x, &y, &yaw);
    Spur_get_vel(&linear_vel, &angular_vel);

    tf2::Quaternion quaternion;
    quaternion.setRPY(0.0, 0.0, yaw);

    const auto stamp = this->now();

    geometry_msgs::msg::TransformStamped odom_tf;
    odom_tf.header.stamp = stamp;
    odom_tf.header.frame_id = "odom";
    odom_tf.child_frame_id = "base_link";
    odom_tf.transform.translation.x = x;
    odom_tf.transform.translation.y = y;
    odom_tf.transform.translation.z = 0.0;
    odom_tf.transform.rotation.x = quaternion.x();
    odom_tf.transform.rotation.y = quaternion.y();
    odom_tf.transform.rotation.z = quaternion.z();
    odom_tf.transform.rotation.w = quaternion.w();
    tf_broadcaster_->sendTransform(odom_tf);

    nav_msgs::msg::Odometry odom_msg;
    odom_msg.header.stamp = stamp;
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "base_link";
    odom_msg.pose.pose.position.x = x;
    odom_msg.pose.pose.position.y = y;
    odom_msg.pose.pose.position.z = 0.0;
    odom_msg.pose.pose.orientation = odom_tf.transform.rotation;
    odom_msg.twist.twist.linear.x = linear_vel;
    odom_msg.twist.twist.angular.z = angular_vel;
    odom_pub_->publish(odom_msg);
}

void IcartDriver::loop()
{
    if(!ypspur_flag_) {
        bringup_ypspur();
    }else{
        publish_odom();
    }
}
