#include <variant>
#include <vector>
#include <string>
#include <print>
#include <format>
#include "MVS/MvCameraControl.h"
using u_char = unsigned char;
using uint = unsigned int;
using uint32_t = unsigned int;


struct Params{
    float gain = 10.0f;             //增益
    bool auto_gain = false;         // 自动 & 手动
    float exposure = 50000.0f;      // 30ms
    bool auto_exposure = true;      // 自动 & 手动
    float fps = 60.0f;              // 限制帧率
    bool auto_balance = true;
    float white_balance_r = 1.5f;
    float white_balance_b = 1.2f;
    float contrast = 1.5f;
    float saturation = 1.2f;
    bool horizon_flip = false;
    bool vertical_flip = false;
    bool noise_reduction = true;
    MvGvspPixelType pixel_type = PixelType_Gvsp_RGB8_Packed;
};

struct DeviceInfo{
    uint16_t major_ver;         // 主版本号
    uint16_t minor_ver;         // 次版本号
    uint32_t mac_addr_high;     // 高mac地址
    uint32_t mac_addr_low;      // 低mac地址
    uint32_t protocol_type;     // 传输层协议类型
    std::string curr_ip;           // 当前设备的ip地址
    std::string curr_sub_net_mask; // 当前设备的子网掩码
    std::string curr_gate_way;     // 当前设备的网关
    std::string net_export;        // 网口ip
    std::string manufacturers;  // 制造商名称
    std::string manufacturers_spec;//制造商具体信息
    std::string model_name;     // 相机型号
    std::string decice_ver;     // 相机设备版本
    std::string serial_num;     // 设备序列号
};
struct FrameInfo{
    int width = 0;
    int height = 0;
    int channels = 0; // 1通道就是Mono8类型的灰度图片，3通道就是RGB8类型的彩图
};

class Camera {
private:
    FrameInfo frameinfo;
    void* handle = nullptr;
    MV_CC_DEVICE_INFO_LIST device_lists;
    MvGvspPixelType pixel_type = MvGvspPixelType::PixelType_Gvsp_Undefined;
    bool is_color(MvGvspPixelType enType);
    bool is_mono(MvGvspPixelType enType);
public:
    Camera(){}
    ~Camera() {
        if(handle != nullptr){
            // 先停止采集
            MV_CC_StopGrabbing(handle);
            // 再关闭设备
            MV_CC_CloseDevice(handle);
            // 最后销毁handle
            MV_CC_DestroyHandle(handle);
            handle = nullptr;
        }
    }
    bool SetParams(Params &params); // 设置相机参数
    std::vector<DeviceInfo>  GetDevices();               // 只会去获取GigE协议的设备，其他协议的不管
    std::variant<std::string,FrameInfo> OpenCamera(int index,Params& params);// 选择具体是哪个设备
    std::variant<std::string,std::vector<uint8_t>> GetFrame(); // 获取一帧图像
};