
#include "base_control.hpp"

using namespace std;

PWM pwm;
// ros::Publisher move_body_pub;
ros::Subscriber cmd_vel_sub;



void setup(){
	ROS_INFO ("Start initialization Base Control");
}

void VelCmdCallback(const geometry_msgs::TwistConstPtr& msg){
	float x_vel = msg->linear.x;
	float z_ang_vel = msg->angular.z;
	float w_l, w_r;

	float x_vel_max = 0.3, z_ang_vel_max = 1.5;
	x_vel = constrain(x_vel, -x_vel_max, x_vel_max);
	z_ang_vel = constrain(z_ang_vel, -z_ang_vel_max, z_ang_vel_max);

	float L = 0.135, R = 0.035, mu = 2.8;
	w_r = (2*x_vel + mu*z_ang_vel*L) / (2*R);
	w_l = (2*x_vel - mu*z_ang_vel*L) / (2*R);

	float w_max = 8;
	w_r = constrain(w_r/w_max, -1, 1);
	w_l = constrain(w_l/w_max, -1, 1);

	ROS_INFO ("w_l %f w_r %f", w_l, w_r);
	pwm.WheelNorm(w_r, w_l);
}

int main(int argc, char **argv){

	ros::init(argc, argv, "base_control");
	ros::Time::init();
	// ros::Rate loop_rate(20);
	ros::NodeHandle node;
	setup();

	cmd_vel_sub = node.subscribe<geometry_msgs::Twist>("/cmd_vel", 1, VelCmdCallback);
	// move_body_pub = node.advertise<crab_msgs::BodyState>("/teleop/move_body",1);
	ros::spin();
	return 0;
}



