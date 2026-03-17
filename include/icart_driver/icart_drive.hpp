#pragma once

#include <chrono>

#include <geometry_msgs/msg/transform_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>
#include <tf2_ros/transform_broadcaster.h>

#include <ypspur.h>

class IcartDriver : public rclcpp::Node
{
public:
  explicit IcartDriver(const rclcpp::NodeOptions & options);
  ~IcartDriver();

private:
  void init_params();
  void bringup_ypspur();
  void publish_odom();
  void loop();

  void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);
  void restart_callback(const std_msgs::msg::Empty::SharedPtr msg);
  void emergency_callback(const std_msgs::msg::Empty::SharedPtr msg);

  int interval_ms;
  double linear_max_vel;
  double linear_max_acc;
  double angular_max_vel;
  double angular_max_acc;
  bool ypspur_flag_ = false;

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr restart_sub_;
  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr emergency_sub_;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  rclcpp::TimerBase::SharedPtr timer_;
};
