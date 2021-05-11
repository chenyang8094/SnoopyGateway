#include <stdlib.h>

#include "DeviceMgr.h"
#include "Log.h"
#include "cc2530.h"
#include "light/SimpleLight.h"

static void cc2530_recv_callback(cc2530_buf_t buf) {
    auto data = nlohmann::json::parse(std::string(buf.base, buf.len));
    if (!data.is_object()) {
        zlog_error(g_log, "data parse error.");
        return;
    }
}

int DeviceManager::Start() {
    CreateDevice(MODEL_TYPE::SIMPLE_LIGHT, "test", "0x0017880106117ff9");

    // if (cc2530_init("/dev/ttyUSB0", 115200) != 0)
    // {
    //     zlog_error(g_log, "cc2530 init error.");
    //     return -1;
    // }

    // cc2530_on_receive(cc2530_recv_callback);
    return 0;
}

int DeviceManager::Stop() {

    return 0;
}

std::shared_ptr<Device> DeviceManager::FindDevice(const std::string &sid) {
    std::lock_guard<std::mutex> lk(lock_);
    auto iter = devices_.find(sid);
    if (iter != devices_.end()) {
        return iter->second;
    }

    return nullptr;
}

std::shared_ptr<Device> DeviceManager::CreateDevice(MODEL_TYPE type, const std::string &name, const std::string &sid) {
    if (FindDevice(sid)) {
        // if the Device already exists, just reture an nil
        return nullptr;
    }

    std::shared_ptr<Device> device = nullptr;
    switch (type) {
        case MODEL_TYPE::SIMPLE_LIGHT:
            device = std::make_shared<LightDevice>(name, sid, type);
            break;
        case MODEL_TYPE::SIMPLE_SWITCH:
            break;
        default:
            break;
    }

    if (device) {
        std::lock_guard<std::mutex> lk(lock_);
        devices_[sid] = device;
    }

    return device;
}

int DeviceManager::DelDevice(const std::string &sid) {
    std::lock_guard<std::mutex> lk(lock_);
    auto iter = devices_.find(sid);

    if (iter != devices_.end()) {
        devices_.erase(iter);
        return 1;
    }

    return 0;
}

// int report(device_t* device, cJSON* data)
// {
//     lfqueue_enq(&msg_queue, (void*)data);
// }
