#pragma once

#include <chrono>

#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/empty.hpp>

#include <ypspur.h>

class IcartDriver : public rclcpp::Node
{
public:
  explicit IcartDriver(const rclcpp::NodeOptions & options);
  ~IcartDriver();

private:
  void init_params();
  void apply_limits();
  void bringup_ypspur();
  void loop();

  void cmd_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);
  void restart_callback(const std_msgs::msg::Empty::SharedPtr msg);
  void emergency_callback(const std_msgs::msg::Empty::SharedPtr msg);

  int interval_ms;
  int cmd_vel_timeout_ms;
  double linear_max_vel;
  double linear_max_acc;
  double angular_max_vel;
  double angular_max_acc;
  bool ypspur_flag_ = false;
  int loop_count_ = 0;
  bool cmd_vel_active_ = false;
  std::chrono::steady_clock::time_point last_cmd_vel_;

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr restart_sub_;
  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr emergency_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
};
