#include <iostream>
#include "Camera.h"
#include <memory>
#include <opencv2/opencv.hpp>
int main(int argc, char const *argv[]){
    auto camera = std::make_unique<Camera>();
    auto devices = camera->GetDevices();
    if(devices.empty()) {
        std::println("没有获取到相机设备");
        return 0;
    }
    for(auto const &device:devices){
        std::println("{}",device.curr_ip);
        std::println("{}",device.model_name);
        std::println("{}",device.serial_num);
    }
    Params p;
    auto result = camera->OpenCamera(0,p);
    if(result.index() == 0){
        auto error = std::get<std::string>(result);
        std::println("错误：{}",error);
        return 0;
    }
    auto fameinfo =  std::get<FrameInfo>(result);
    std::println("[{},{},{}]",fameinfo.channels,fameinfo.width,fameinfo.height);
    while (true){
        auto r = camera->GetFrame();
        if(r.index() == 0){
            auto error = std::get<std::string>(r);
            std::println("错误：{}",error);
            break;
        }
        auto img_data = std::get<std::vector<uchar>>(r);
        cv::Mat frame;
        if(fameinfo.channels == 1)
            frame = cv::Mat(fameinfo.height, fameinfo.width, CV_8UC1, img_data.data());
        else
            frame = cv::Mat(fameinfo.height, fameinfo.width, CV_8UC3, img_data.data());
        cv::imshow("frame", frame);
        int key = cv::waitKey(5);
        if (key == 'q' || key == 27) break;
    }
    return 0;
}
