#include "ros/ros.h"
#include "std_msgs/String.h" //普通文本类型的消息
#include <sstream>
#include <iostream>
#include <chrono>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    // 设置编码
    setlocale(LC_ALL, "");
    // 2.初始化 ROS 节点:命名(唯一)
    //  参数1和参数2 后期为节点传值会使用
    //  参数3 是节点名称，是一个标识符，需要保证运行后，在 ROS 网络拓扑中唯一
    ros::init(argc, argv, argv[1]);
    // 3.实例化 ROS 句柄
    ros::NodeHandle nh; // 该类封装了 ROS 中的一些常用功能
    // 4.实例化 发布者 对象
    // 泛型: 发布的消息类型
    // 参数1: 要发布到的话题
    // 参数2: 队列中最大保存的消息数，超出此阀值时，先进的先销毁(时间早的先销毁)
    ros::Publisher pub = nh.advertise<std_msgs::String>(argv[2], 10);

    int bandwidth;
    std::cout << "需求带宽(Mbps): ";
    std::cin >> bandwidth;

    int bag_size;
    std::cout << "数据包大小(Mb): ";
    std::cin >> bag_size;

    unsigned int n = bandwidth / bag_size;
    std::cout << "发送频率: " << n << "hz" << std::endl;
    sleep(5);

    std::string head;
    std::string date((float)bag_size / 8 * 1024 * 1024, 'c');

    std_msgs::String msg;
    int count = 0;
    long milliseconds = 0;

    timeval currentTime;
    gettimeofday(&currentTime, nullptr);

    ros::Rate r(n);
    while (ros::ok())
    {
        head = "count: ";
        head += std::to_string(count);
        head += ", date: ";
        head += std::to_string(milliseconds);
        head += "!";
        head += date;

        msg.data = head.c_str();
        std::cout << count << ", " << milliseconds << ", " << msg.data.size() << std::endl;

        // 发布消息
        pub.publish(msg);

        // 根据前面制定的发送贫频率自动休眠,休眠时间 = 1/频率;
        r.sleep();

        count++;// 循环结束前，让 count 自增
        gettimeofday(&currentTime, nullptr); // 更新时间
        milliseconds = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;

        // 暂无应用
        ros::spinOnce();
    }

    return 0;
}
