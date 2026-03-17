import os
import yaml
from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('icart_driver')

    main_param_path = os.path.join(package_share_dir, 'config', 'main_params.yaml')
    ekf_param_path = os.path.join(package_share_dir, 'config', 'ekf.yaml')
    ypspur_param_path = os.path.join(package_share_dir, 'config', 'ypspur.param')

    # ypspurコーディネータの起動コマンドの作成
    script_path = os.path.join(package_share_dir, 'scripts', 'ypspur_coordinator')
    ypspur_coordinator = ExecuteProcess(
        cmd=[script_path, ypspur_param_path],
        shell=True
    )

    with open(main_param_path, 'r') as file:
        launch_params = yaml.safe_load(file)['launch']['ros__parameters']

    livox_launch_params = launch_params.get('livox_launch', {})
    pointcloud_topics = launch_params.get('pointcloud_to_laserscan_topics', {})

    main_exec_node = Node(
        package='icart_driver',
        executable='icart_driver_main',
        parameters=[main_param_path],
        output='screen'
    )
    ekf_node = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        output='screen',
        parameters=[ekf_param_path],
        remappings=[
            ('odometry/filtered', '/odom_filtered'),
        ],
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
    livox_launch_path = os.path.join(
        get_package_share_directory('livox_ros_driver2'),
        'launch_ROS2',
        'rviz_MID360_launch.py'
    )
    livox_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(livox_launch_path),
        launch_arguments={
            'enable_driver': str(launch_params.get('livox', True)).lower(),
            'enable_rviz': str(livox_launch_params.get('enable_rviz', True)).lower(),
            'xfer_format': str(livox_launch_params.get('xfer_format', 0)),
            'multi_topic': str(livox_launch_params.get('multi_topic', 0)),
            'data_src': str(livox_launch_params.get('data_src', 0)),
            'publish_freq': str(livox_launch_params.get('publish_freq', 10.0)),
            'output_data_type': str(livox_launch_params.get('output_data_type', 0)),
            'frame_id': str(livox_launch_params.get('frame_id', 'livox_frame')),
            'lvx_file_path': str(livox_launch_params.get('lvx_file_path', '/home/livox/livox_test.lvx')),
            'cmdline_input_bd_code': str(livox_launch_params.get('cmdline_input_bd_code', 'livox0000000001')),
        }.items(),
    )
    pointcloud_to_laserscan_node = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan',
        output='screen',
        parameters=[main_param_path],
        remappings=[
            ('cloud_in', pointcloud_topics.get('cloud_in', '/livox/lidar')),
            ('scan', pointcloud_topics.get('scan', '/scan')),
        ],
    )

    # 起動エンティティクラスの作成
    launch_description = LaunchDescription()

    # 起動の追加
    if launch_params.get('joy', True):
        launch_description.add_entity(controller_launch)
    if launch_params.get('planner', True):
        launch_description.add_entity(planner_launch)
    if launch_params.get('ekf', True):
        launch_description.add_entity(ekf_node)
    if launch_params.get('livox', True):
        launch_description.add_entity(livox_launch)
        launch_description.add_entity(pointcloud_to_laserscan_node)
    
    launch_description.add_entity(ypspur_coordinator)
    launch_description.add_entity(main_exec_node)


    return launch_description
