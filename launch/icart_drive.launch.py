import os
import yaml
from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('icart_driver')

    main_param_path = os.path.join(package_share_dir, 'config', 'main_param.yaml')
    ypspur_param_path = os.path.join(package_share_dir, 'config', 'ypspur.param')

    # ypspurコーディネータの起動コマンドの作成
    script_path = os.path.join(package_share_dir, 'scripts', 'ypspur_coordinator')
    ypspur_coordinator = ExecuteProcess(
        cmd=[script_path, ypspur_param_path],
        shell=True
    )

    main_exec_node = Node(
        package='icart_driver',
        executable='icart_driver_main',
        parameters=[main_param_path],
        output='screen'
    )


    # 起動エンティティクラスの作成
    launch_description = LaunchDescription()

    launch_description.add_entity(ypspur_coordinator)
    launch_description.add_entity(main_exec_node)

    return launch_description
