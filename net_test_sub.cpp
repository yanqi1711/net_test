#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sys/time.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>

timeval T1, T2;
int hz;
char buffer[80];

std::vector<int> count;
std::vector<std::string> timeout;

int discard = 0;
int count_temp = 0;
std::string date_head;

void doMsg(const std_msgs::String::ConstPtr &msg_p)
{

    gettimeofday(&T1, nullptr);

    if (discard < 10)
    {
        discard++;
    }
    else
    {
        int T0 = (T1.tv_sec - T2.tv_sec) * 1000 + (T1.tv_usec - T2.tv_usec) / 1000;
        char *p = strtok(const_cast<char *>(msg_p->data.c_str()), "!");
        date_head = p;
        date_head += ", interval: ";
        date_head += std::to_string(T0);
        date_head += "ms";
        if (T0 > hz)
        {
            date_head += " \t\t +";
            date_head += std::to_string(T0 - hz);
        }
        else
        {
            date_head += " \t\t -";
            date_head += std::to_string(hz - T0);
        }

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));

        if (T0 > ((float)hz + (float)hz * 0.2) || T0 < ((float)hz - (float)hz * 0.2))
        {
            ROS_INFO("\e[31m%s, %sms, interval: %dms\e[0m\n", buffer, p, T0);
            timeout.push_back(date_head);
        }
        else
        {
            ROS_INFO("%s, %sms, interval: %dms\n", buffer, p, T0);
        }

        char *count0 = strtok(const_cast<char *>(msg_p->data.c_str()), "count: ");
        char *count1 = strtok(count0, ",");
        int count3 = std::stoi(count1);
        if (count_temp != 0)
        {
            if ((count3 - count_temp) != 1)
            {
                count.push_back(count3 - 1);
            }
        }
        count_temp = count3;
    }
    T2 = T1;
}
int main(int argc, char *argv[])
{
    // 设置编码
    setlocale(LC_ALL, "");
    // 2.初始化 ROS 节点:命名(唯一)
    ros::init(argc, argv, argv[1]);
    // 3.实例化 ROS 句柄
    ros::NodeHandle nh;
    // std::cout << "发送端频率(hz): ";
    // std::cin >> hz;
    hz = 1000 / std::stoi(argv[3]);
    // 4.实例化 订阅者 对象
    ros::Subscriber sub = nh.subscribe<std_msgs::String>(argv[2], 10, doMsg);
    // 5.处理订阅的消息(回调函数)

    // 6.设置循环调用回调函数
    ros::spin(); // 循环读取接收的数据，并调用回调函数处理

    std::cout << "\n帧间隔：" << hz << "ms" << std::endl;
    std::cout << "丢包：" << count.size() << std::endl;
    if (count.size() != 0)
    {
        for (auto i : count)
        {
            std::cout << i << std::endl;
        }
    }

    std::cout << "超时：" << timeout.size() << std::endl;
    if (timeout.size() != 0)
    {
        for (auto i : timeout)
        {
            std::cout << i << std::endl;
        }
    }

    return 0;
}
