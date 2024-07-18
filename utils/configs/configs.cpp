#include "configs.hpp"

#include <iostream>
#include <fstream>

#include "../../roblox/globals/globals.hpp"

#include "../json/json.hpp"

#include <ShlObj.h>

using namespace nlohmann;

void replace(std::string &str, const std::string &from, const std::string &to) {
    std::size_t startPosition = str.find(from);
    if (startPosition == std::string::npos) return;
    str.replace(startPosition, from.length(), to);
}

void saveFile(const std::string &filePath, const std::string &data) {
    std::ofstream outFile;
    outFile.open(filePath);
    outFile << data;
    outFile.close();
}

void loadFile(const std::string &filePath, std::string &data) {
    std::ifstream inFile;
    inFile.open(filePath);
    inFile >> data;
    inFile.close();
}


template<typename T>
void loadConfigOption(T &globalSetting, const json &configObject, const std::string &configSetting) {
    if (!configObject.contains(configSetting)) return;
    globalSetting = configObject[configSetting];
}


void silence::utils::configs::save(const char *name)
{
    json config;

    config["esp"] = globals::esp;
    config["box"] = globals::box;
    config["name"] = globals::name_esp;
    config["tracers"] = globals::tracers;
    config["aimbot"] = globals::aimbot;
    config["resolver"] = globals::resolver;

    config["smoothness_x"] = globals::smoothness_x;
    config["smoothness_y"] = globals::smoothness_y;

    config["stabilizer"] = globals::stabilizer_x;
    config["stabilizer_x"] = globals::stabilizer_x;
    config["stabilizer_x"] = globals::stabilizer_y;

    config["prediction"] = globals::prediction;
    config["prediction_x"] = globals::prediction_x;
    config["prediction_y"] = globals::prediction_y;

    config["fov"] = globals::fov;
    config["fovcircle"] = globals::fov_on;

    config["aimpart"] = globals::aimpart;

    config["sensitivity"] = globals::sensitivity;

    config["teamcheck"] = globals::team_check;
    config["healthcheck"] = globals::healthcheck;
    config["knockcheck"] = globals::knock_check;

    config["sticky"] = globals::sticky_aim;
    config["streamproof"] = globals::streamproof;
    config["vsync"] = globals::vsync;

    config["aimkey"] = globals::aimbotkey.key;
    config["aimkeymethod"] = globals::aimbotkey.type;

    saveFile(appdata_path() + "\\silence\\" + "\\configs\\" + static_cast<std::string>(name) + ".cfg", config.dump());
}

void silence::utils::configs::load(const char *name)
{
    std::string configJson = "";
    loadFile(appdata_path() + "\\silence\\" + "\\configs\\" + static_cast<std::string>(name) + ".cfg", configJson);

    if (!configJson[0])
        return;

    json config = json::parse(configJson);

    loadConfigOption<bool>(globals::esp, config, "esp");
    loadConfigOption<bool>(globals::aimbot, config, "aimbot");
    loadConfigOption<bool>(globals::box, config, "box");
    loadConfigOption<bool>(globals::name_esp, config, "name");
    loadConfigOption<bool>(globals::tracers, config, "tracers");
    loadConfigOption<bool>(globals::fov_on, config, "fovcircle");

    loadConfigOption<float>(globals::smoothness_x, config, "smoothness_x");
    loadConfigOption<float>(globals::smoothness_y, config, "smoothness_y");

    loadConfigOption<bool>(globals::prediction, config, "prediction");
    loadConfigOption<float>(globals::prediction_x, config, "prediction_x");
    loadConfigOption<float>(globals::prediction_y, config, "prediction_y");

    loadConfigOption<int>(globals::aimpart, config, "aimpart");
    loadConfigOption<int>(globals::boxtype, config, "boxtype");
    loadConfigOption<int>(globals::fov, config, "fov");
    loadConfigOption<int>(globals::aimbotkey.key, config, "aimkey");
    loadConfigOption<CKeybind::c_keybind_type>(globals::aimbotkey.type, config, "aimkeymethod");

    loadConfigOption<float>(globals::sensitivity, config, "sensitivity");

    loadConfigOption<bool>(globals::healthcheck, config, "healthcheck");
    loadConfigOption<bool>(globals::team_check, config, "teamcheck");
    loadConfigOption<bool>(globals::knock_check, config, "knockcheck");

    loadConfigOption<bool>(globals::sticky_aim, config, "sticky");
    loadConfigOption<bool>(globals::streamproof, config, "streamproof");
    loadConfigOption<bool>(globals::vsync, config, "vsync");
}