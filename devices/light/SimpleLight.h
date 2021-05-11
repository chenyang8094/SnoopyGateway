#pragma once

#include "DeviceMgr.h"

class LightDevice : public Device {
  public:
    LightDevice(const std::string &name, const std::string &sid, MODEL_TYPE type);
    ~LightDevice() = default;

  public:
    virtual int SetCommand(nlohmann::json &cmd);
    virtual int GetCommand(nlohmann::json &cmd);
    virtual int GetRegisterInfo(nlohmann::json &info);
};