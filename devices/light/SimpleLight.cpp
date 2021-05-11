#include "SimpleLight.h"

#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

#include "Log.h"

LightDevice::LightDevice(const std::string &name, const std::string &sid, MODEL_TYPE type)
  : Device(name, sid, type) {
    // pinMode(LED, OUTPUT);
}

int LightDevice::SetCommand(nlohmann::json &cmd) {
    std::string state;
    if (cmd.contains("state") && cmd["state"].is_string()) {
        cmd["state"].get_to(state);
        if (state == "ON") {
            zlog_info(g_log, "LEN ON");
            // digitalWrite(LED, HIGH);
        } else {
            zlog_info(g_log, "LEN OFF");
            // digitalWrite(LED, LOW);
        }
    }
}

int LightDevice::GetCommand(nlohmann::json &cmd) {
}

int LightDevice::GetRegisterInfo(nlohmann::json &info) {
    info = nlohmann::json::object();
    info["date_code"] = "20200706";

    return 0;
}
