#pragma once

#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "json.hpp"

enum class MODEL_TYPE : int {
    SIMPLE_LIGHT,
    SIMPLE_SWITCH
};

class Device {
  public:
    Device(const std::string &name, const std::string &sid, MODEL_TYPE type)
      : name_(name)
      , sid_(sid)
      , type_(type) {}

    virtual ~Device() {}

    std::string GetName();
    int SetName(const std::string &name);
    std::string GetSid();
    int SetSid(const std::string &sid);
    MODEL_TYPE GetType();
    int SetType(MODEL_TYPE type);

    virtual int SetCommand(nlohmann::json &cmd) = 0;
    virtual int GetCommand(nlohmann::json &cmd) = 0;
    virtual int GetRegisterInfo(nlohmann::json &info) = 0;

  private:
    std::string name_;
    std::string sid_;
    MODEL_TYPE type_;
};

class DeviceManager {
  public:
    DeviceManager() = default;

    static DeviceManager &Instance() {
        static DeviceManager manager;
        return manager;
    }

    int Start();
    int Stop();

    std::shared_ptr<Device> CreateDevice(MODEL_TYPE type, const std::string &name, const std::string &sid);
    std::shared_ptr<Device> FindDevice(const std::string &sid);
    int DelDevice(const std::string &sid);

  private:
    bool stopped_{true};
    std::unordered_map<std::string, std::shared_ptr<Device>> devices_;
    std::mutex lock_;
};
