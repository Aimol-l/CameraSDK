#include "Camera.h"

std::vector<DeviceInfo> Camera::GetDevices(){
    std::vector<DeviceInfo> infos;
    if(MV_CC_EnumDevices(MV_GIGE_DEVICE, &device_lists)!=MV_OK) return infos;
    for(int i = 0;i<device_lists.nDeviceNum;++i){
        auto pinfo = device_lists.pDeviceInfo;
        auto gige_info = (*pinfo)->SpecialInfo.stGigEInfo;
        // curr_ip
        int ip1 = (gige_info.nCurrentIp & 0xff000000) >> 24;
        int ip2 = (gige_info.nCurrentIp & 0x00ff0000) >> 16;
        int ip3 = (gige_info.nCurrentIp & 0x0000ff00) >> 8;
        int ip4 = (gige_info.nCurrentIp & 0x000000ff);
        std::string curr_ip = std::format("{}.{}.{}.{}",ip1,ip2,ip3,ip4);
        // curr_sub_net_mask
        ip1 = (gige_info.nCurrentSubNetMask & 0xff000000) >> 24;
        ip2 = (gige_info.nCurrentSubNetMask & 0x00ff0000) >> 16;
        ip3 = (gige_info.nCurrentSubNetMask & 0x0000ff00) >> 8;
        ip4 = (gige_info.nCurrentSubNetMask & 0x000000ff);
        std::string curr_sub_net_mask = std::format("{}.{}.{}.{}",ip1,ip2,ip3,ip4);
        // curr_gate_way
        ip1 = (gige_info.nDefultGateWay & 0xff000000) >> 24;
        ip2 = (gige_info.nDefultGateWay & 0x00ff0000) >> 16;
        ip3 = (gige_info.nDefultGateWay & 0x0000ff00) >> 8;
        ip4 = (gige_info.nDefultGateWay & 0x000000ff);
        std::string curr_gate_way = std::format("{}.{}.{}.{}",ip1,ip2,ip3,ip4);
        // net_export
        ip1 = (gige_info.nNetExport & 0xff000000) >> 24;
        ip2 = (gige_info.nNetExport & 0x00ff0000) >> 16;
        ip3 = (gige_info.nNetExport & 0x0000ff00) >> 8;
        ip4 = (gige_info.nNetExport & 0x000000ff);
        std::string net_export = std::format("{}.{}.{}.{}",ip1,ip2,ip3,ip4);

        auto manufacturers = std::string(reinterpret_cast<const char*>(gige_info.chManufacturerName),32);
        auto manufacturers_spec = std::string(reinterpret_cast<const char*>(gige_info.chManufacturerSpecificInfo),48);
        auto model_name = std::string(reinterpret_cast<const char*>(gige_info.chModelName),32);
        auto decice_ver = std::string(reinterpret_cast<const char*>(gige_info.chDeviceVersion),32);
        auto serial_num = std::string(reinterpret_cast<const char*>(gige_info.chSerialNumber),16);
        
        DeviceInfo info{
            (*pinfo)->nMajorVer,
            (*pinfo)->nMinorVer,
            (*pinfo)->nMacAddrHigh,
            (*pinfo)->nMacAddrLow,
            (*pinfo)->nTLayerType,
            curr_ip,
            curr_sub_net_mask,
            curr_gate_way,
            net_export,
            manufacturers,
            manufacturers_spec,
            model_name,
            decice_ver,
            serial_num
        };
        infos.push_back(info);
    }
    return infos;
}
bool Camera::SetParams(Params &params){
    if (handle == nullptr){
        std::println("Error set_params(): There is no device!");
        return false;
    }
    int code;
    code = MV_CC_SetEnumValue(handle, "GammaSelector", MV_GAMMA_SELECTOR_SRGB);
    if(params.auto_gain){
        code = MV_CC_SetEnumValue(handle, "GainAuto", MV_GAIN_MODE_CONTINUOUS);
    }else{
        code = MV_CC_SetEnumValue(handle, "GainAuto", MV_GAIN_MODE_OFF);
        code = MV_CC_SetFloatValue(handle, "Gain", params.gain);  // 设置增益值
    }
    if (code != MV_OK) {
        std::println("Error set_params: auto_gain failed! code={}",code);
    }
    if(params.auto_exposure){
        code = MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_CONTINUOUS);
    }else{
        code = MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
        code = MV_CC_SetFloatValue(handle, "ExposureTime",  params.exposure); // 30ms
    }
    if (code != MV_OK) {
        std::println("Error set_params: auto_exposure failed! code={}",code);
    }
    code = MV_CC_SetBoolValue(handle, "AcquisitionFrameRateEnable", true);
    code = MV_CC_SetFloatValue(handle, "AcquisitionFrameRate", params.fps); // 30帧每秒
    if (code != MV_OK) {
        std::println("Error set_params: fps failed! code={}",code);
    }
    if(params.auto_balance){
        code = MV_CC_SetEnumValue(handle, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
    }else{
        code = MV_CC_SetFloatValue(handle, "BalanceRatioRed", params.white_balance_r);  // 红色增益
        code = MV_CC_SetFloatValue(handle, "BalanceRatioBlue", params.white_balance_b); // 蓝色增益
    }
    if (code != MV_OK) {
        std::println("Error set_params: balance failed! code={}",code);
    }
    // 图像水平翻转
    code = MV_CC_SetBoolValue(handle, "ReverseX", params.horizon_flip);
    if (code != MV_OK) {
        std::println("Error set_params: horizon_flip failed! code={}",code);
    }
    // 图像垂直翻转
    code = MV_CC_SetBoolValue(handle, "ReverseY", params.vertical_flip);
    if (code != MV_OK) {
        std::println("Error set_params: vertical_flip failed! code={}",code);
    }
    // 启用去噪功能
    code = MV_CC_SetBoolValue(handle, "NoiseReductionEnable", params.noise_reduction);
    if (code != MV_OK) {
        std::println("Error set_params: noise_reduction failed! code={}",code);
    }
    return true;
}
bool Camera::is_color(MvGvspPixelType enType){
    switch (enType){
        case PixelType_Gvsp_RGB8_Packed:
        case PixelType_Gvsp_BGR8_Packed:
        case PixelType_Gvsp_YUV422_Packed:
        case PixelType_Gvsp_YUV422_YUYV_Packed:
        case PixelType_Gvsp_BayerGR8:
        case PixelType_Gvsp_BayerRG8:
        case PixelType_Gvsp_BayerGB8:
        case PixelType_Gvsp_BayerBG8:
        case PixelType_Gvsp_BayerGB10:
        case PixelType_Gvsp_BayerGB10_Packed:
        case PixelType_Gvsp_BayerBG10:
        case PixelType_Gvsp_BayerBG10_Packed:
        case PixelType_Gvsp_BayerRG10:
        case PixelType_Gvsp_BayerRG10_Packed:
        case PixelType_Gvsp_BayerGR10:
        case PixelType_Gvsp_BayerGR10_Packed:
        case PixelType_Gvsp_BayerGB12:
        case PixelType_Gvsp_BayerGB12_Packed:
        case PixelType_Gvsp_BayerBG12:
        case PixelType_Gvsp_BayerBG12_Packed:
        case PixelType_Gvsp_BayerRG12:
        case PixelType_Gvsp_BayerRG12_Packed:
        case PixelType_Gvsp_BayerGR12:
        case PixelType_Gvsp_BayerGR12_Packed:
        return true;
    default:
        return false;
    }
}
bool Camera::is_mono(MvGvspPixelType enType){
    switch (enType){
        case PixelType_Gvsp_Mono8:
        case PixelType_Gvsp_Mono10:
        case PixelType_Gvsp_Mono10_Packed:
        case PixelType_Gvsp_Mono12:
        case PixelType_Gvsp_Mono12_Packed:
        return true;
    default:
        return false;
    }
}
std::variant<std::string,FrameInfo> Camera::OpenCamera(int index,Params& params){
    // 需要判断之前是否打开过相机，如果有就要先销毁
    if(handle != nullptr){
        // 先停止采集
        MV_CC_StopGrabbing(handle);
        // 再关闭设备
        MV_CC_CloseDevice(handle);
        // 最后销毁handle
        MV_CC_DestroyHandle(handle);
        handle = nullptr;
    }
    MV_CC_CreateHandle(&handle, device_lists.pDeviceInfo[index]); // 创建设备
    MV_CC_OpenDevice(handle);       // 打开设备
    // 设置参数
    this->SetParams(params);
    MV_CC_StartGrabbing(handle);    // 开始采集图像
    // 先获取输出图像的信息
    MV_FRAME_OUT stImageInfo = { 0 };
    if (MV_CC_GetImageBuffer(handle, &stImageInfo, 1000) != MV_OK) {
        MV_CC_FreeImageBuffer(handle, &stImageInfo);
        return "获取图像信息失败";
    }
    frameinfo.height = stImageInfo.stFrameInfo.nHeight;
    frameinfo.width  = stImageInfo.stFrameInfo.nWidth;
    if (this->is_color(stImageInfo.stFrameInfo.enPixelType)){
        frameinfo.channels = 3;
        this->pixel_type = MvGvspPixelType::PixelType_Gvsp_RGB8_Packed; //如果是彩色则转成RGB8
    }else if (this->is_mono(stImageInfo.stFrameInfo.enPixelType)){
        frameinfo.channels = 1;
        this->pixel_type = MvGvspPixelType::PixelType_Gvsp_Mono8;  //如果是黑白则转换成Mono8
    }else{
        MV_CC_FreeImageBuffer(handle, &stImageInfo);
        return "无法获取图像格式类型";
    }
    MV_CC_FreeImageBuffer(handle, &stImageInfo);
    return this->frameinfo;
}
std::variant<std::string,std::vector<uint8_t>> Camera::GetFrame(){
    if(handle == nullptr) return "camera not get";
    MV_FRAME_OUT stImageInfo = {0};
    int code = MV_CC_GetImageBuffer(handle, &stImageInfo, 1000);
    if (code != MV_OK) {
        MV_CC_FreeImageBuffer(handle, &stImageInfo);
        return "获取图像信息失败";
    }
    auto buffer_size = frameinfo.width*frameinfo.height*frameinfo.channels;
    std::vector<uint8_t> frame(buffer_size);
    MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
    stConvertParam.nWidth = stImageInfo.stFrameInfo.nWidth;                 //ch:图像宽 | en:image width
    stConvertParam.nHeight = stImageInfo.stFrameInfo.nHeight;               //ch:图像高 | en:image height
    stConvertParam.pSrcData = stImageInfo.pBufAddr;                         //ch:输入数据缓存 | en:input data buffer
    stConvertParam.nSrcDataLen = stImageInfo.stFrameInfo.nFrameLen;         //ch:输入数据大小 | en:input data size
    stConvertParam.enSrcPixelType = stImageInfo.stFrameInfo.enPixelType;    //ch:输入像素格式 | en:input pixel format
    stConvertParam.enDstPixelType = pixel_type;                             //ch:输出像素格式 | en:output pixel format
    stConvertParam.pDstBuffer = frame.data();                               //ch:输出数据缓存 | en:output data buffer
    stConvertParam.nDstBufferSize = buffer_size;                            //ch:输出缓存大小 | en:output buffer size
    MV_CC_FreeImageBuffer(handle, &stImageInfo);

    code = MV_CC_ConvertPixelType(handle, &stConvertParam);
    if (MV_OK != code) {
        return "convert image failed";
    }
    return std::move(frame);
}
