#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "serial/serial.h"
#include "roscpp/Logger.h"
#define START_FRAME 0xABCD
const float RADIUS  = 0.19f / 2;
const float DISTANCE = 0.4f;

serial::Serial ser;

void msg_cb(const geometry_msgs::Twist::ConstPtr& msg);



struct CmdMSG
{
    uint16_t start;
    int16_t steer;
    int16_t speed;
    uint16_t checksum;
};


int main(int argc, char **argv)
{
    ros::init(argc, argv,"base_comm");
    ros::NodeHandle n;

    try
    {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch(const serial::IOException& e)
    {
        // std::cerr << e.what() << '\n';
        ROS_ERROR("%s",e.what());
    
        return -1;
    }

    ros::Subscriber sub = n.subscribe("/hoverboard_velocity_controller/cmd_vel",1, msg_cb);

    ros::spin();

    ser.close();

    return 0;
}

void msg_cb(const geometry_msgs::Twist::ConstPtr& msg)
{
    double linear = msg->linear.x;
    double angular = msg->angular.z;

    double LW = (linear - (angular*(DISTANCE/2))) / RADIUS;
    double RW = (linear + (angular*(DISTANCE/2))) / RADIUS;

    LW /= 0.10472;
    RW /= 0.10472;

    double speed = (LW + RW)/2;
    double steer = (LW - RW)*2;

    CmdMSG payload;
    payload.start = (uint16_t)START_FRAME;
    payload.speed = (int16_t)speed;
    payload.steer = (int16_t)steer;
    payload.checksum = (uint16_t)(payload.start ^ payload.steer ^ payload.speed);

    ser.write((const uint8_t *) &payload, sizeof(CmdMSG));

}