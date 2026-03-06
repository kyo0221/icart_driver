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

    with open(main_param_path, 'r') as file:
        launch_params = yaml.safe_load(file)['launch']['ros__parameters']

    main_exec_node = Node(
        package='icart_driver',
        executable='icart_driver_main',
        parameters=[main_param_path],
        output='screen'
    )

    # コントローラーの起動コマンドの作成
    controller_launch_path = os.path.join(
        get_package_share_directory('controller'),
        'launch',
        'controller.launch.py'
    )
    controller_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(controller_launch_path),
        launch_arguments={
            'param_file': main_param_path,
        }.items(),
    )
    # ml_plannerの起動コマンドの作成
    planner_launch_path = os.path.join(
        get_package_share_directory('ml_planner'),
        'launch',
        'planner.launch.py'
    )
    planner_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(planner_launch_path),
        launch_arguments={
            'param_file': main_param_path,
        }.items(),
    )

    # 起動エンティティクラスの作成
    launch_description = LaunchDescription()

    # 起動の追加
    if launch_params.get('joy', True):
        launch_description.add_entity(controller_launch)
    if launch_params.get('planner', True):
        launch_description.add_entity(planner_launch)
    
    launch_description.add_entity(ypspur_coordinator)
    launch_description.add_entity(main_exec_node)

    return launch_description
