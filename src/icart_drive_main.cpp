#include <rclcpp/rclcpp.hpp>
#include "icart_driver/icart_drive.hpp"

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exec;
    rclcpp::NodeOptions nodes_option;

    nodes_option.allow_undeclared_parameters(true);
    nodes_option.automatically_declare_parameters_from_overrides(true);

    auto drive_node = std::make_shared<IcartDriver>(nodes_option);

    exec.add_node(drive_node);

    exec.spin();
    rclcpp::shutdown();
    return 0;
}
