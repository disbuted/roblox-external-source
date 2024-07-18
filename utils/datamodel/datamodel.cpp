#include "datamodel.hpp"
#include "../xorstr/xorstr.hpp"

std::uint64_t silence::utils::datamodel::get_game_address()
{
    auto latest_log = silence::utils::log::get_latest_log();

    std::ifstream rbx_log(latest_log);
    std::stringstream log_content;
    log_content << rbx_log.rdbuf();

    std::string rbx_log_data = log_content.str();

    if (rbx_log_data.find(XorStr("UGCGameController::finishTeleportWithJoinScriptPayload")) != std::string::npos)
    {
        size_t posTeleport = rbx_log_data.find(XorStr("start dataModel("), rbx_log_data.find(XorStr("UGCGameController::finishTeleportWithJoinScriptPayload")));

        if (posTeleport != std::string::npos)
        {
            rbx_log_data = rbx_log_data.substr(posTeleport + 16);
            rbx_log_data = rbx_log_data.substr(0, rbx_log_data.find(')'));
            std::uint64_t game = std::strtoull(rbx_log_data.c_str(), nullptr, 16);
            return game + 0x148;
        }
    }

    size_t posMain = rbx_log_data.find(XorStr("initialized DataModel("));

    if (posMain != std::string::npos)
    {
        rbx_log_data = rbx_log_data.substr(posMain + 22);
        rbx_log_data = rbx_log_data.substr(0, rbx_log_data.find(')'));
        std::uint64_t game = std::strtoull(rbx_log_data.c_str(), nullptr, 16);
        return game + 0x148;
    }

    return 0;
}

std::uint64_t silence::utils::datamodel::get_place_id()
{
    auto latest_log = silence::utils::log::get_latest_log();

    std::ifstream rbx_log(latest_log);
    std::stringstream log_content;
    log_content << rbx_log.rdbuf();

    std::string rbx_log_data = log_content.str();

    std::regex placeIdRegex(R"("placeId":(\d+),)");

    std::smatch match;
    if (std::regex_search(rbx_log_data, match, placeIdRegex))
    {
        if (match.size() > 1)
        {
            std::uint64_t placeId = std::stoull(match[1].str());
            return placeId;
        }
    }

    return 0;
}