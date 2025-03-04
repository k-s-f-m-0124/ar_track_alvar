

from launch import LaunchDescription
import launch_ros.actions
import os 

def generate_launch_description():
    return LaunchDescription([
        launch_ros.actions.Node(
            package='ar_track_alvar', 
            executable='individualMarkersNoKinect', 
            output='screen',
            remappings=[
                ("camera_image", "wide_stereo/left/image_color"),
                ("camera_info","wide_stereo/left/camera_info")
            ],
            parameters=[
                {"marker_size":4.4},
		        {"max_new_marker_error":0.08},
		        {"max_track_error":0.2},
		        {"output_frame":"torso_lift_link"}
            ],)
    ])