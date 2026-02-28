import os
import yaml
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('icart_driver')

    main_param_path = os.path.join(package_share_dir, 'config', 'main_params.yaml')
    ypspur_param_path = os.path.join(package_share_dir, 'config', 'ypspur.param')

    # ypspurコーディネータの起動コマンドの作成
    script_path = os.path.join(package_share_dir, 'scripts', 'ypspur_coordinator.sh')
    ypspur_coordinator = ExecuteProcess(
        cmd=['sh', script_path, ypspur_param_path],
        output='screen'
    )

    with open(main_param_path, 'r') as file:
        launch_params = yaml.safe_load(file)['launch']['ros__parameters']

    main_exec_node = Node(
        package = 'icart_driver',
        executable = 'icart_drive_main',
        parameters = [main_param_path],
        output='screen'
    )

    joy_node = Node(
        package = 'joy',
        executable = 'joy_node',
        output='screen'
    )

    # 起動エンティティクラスの作成
    launch_description = LaunchDescription()

    # 起動の追加
    if(launch_params['joy'] is True):
        launch_description.add_entity(joy_node)

    launch_description.add_entity(ypspur_coordinator)
    launch_description.add_entity(main_exec_node)

    return launch_description
