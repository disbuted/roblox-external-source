#include "roblox/classes/classes.hpp"
#include "utils/configs/configs.hpp"

#include <filesystem>
#include <iostream>
#include <thread>

#include "keyauth/auth.hpp"
#include <string>
#include "keyauth/utils.hpp"
#include "keyauth/skStr.h"

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);

using namespace KeyAuth;

auto name = skCrypt("");
auto ownerid = skCrypt("");
auto secret = skCrypt("");
auto version = skCrypt("1.0");
auto url = skCrypt("https://keyauth.win/api/1.2/"); // change if you're self-hosting

api KeyAuthApp(name.decrypt(), ownerid.decrypt(), secret.decrypt(), version.decrypt(), url.decrypt());

HWND fnt = NULL;
char title[100];
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
void getrandomnumber(char* sStr, unsigned int iLen)
{
    char Syms[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned int Ind = 0;
    srand(time(NULL) + rand());
    while (Ind < iLen)
    {
        sStr[Ind++] = Syms[rand() % 62];
    }
    sStr[iLen] = '\0';
}

int main(/* credits to AGC / mogus (lua env)  ego @ 0x0 for fixing the imgui and other existing problems*/)
{
    name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();

    getrandomnumber(title, 40);
    SetConsoleTitle(title);
     //std::string consoleTitle = skCrypt("[Dev Build 3/30/24]").decrypt();
     //SetConsoleTitleA(consoleTitle.c_str());
    // system("Color 0C");
    KeyAuthApp.init();
    if (!KeyAuthApp.data.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
        Sleep(1500);
        exit(1);
    }

    if (std::filesystem::exists("fent.json"))
    {
        if (!CheckIfJsonKeyExists("fent.json", "username"))
        {
            std::string key = ReadFromJson("fent.json", "license");
            KeyAuthApp.license(key);
            if (!KeyAuthApp.data.success)
            {
                std::remove("fent.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
                Sleep(1500);
                exit(1);
            }
            std::cout << skCrypt("[fent external] > auto logged in\n");
        }
        else
        {
            std::string username = ReadFromJson("fent.json", "username");
            std::string password = ReadFromJson("fent.json", "password");
            KeyAuthApp.login(username, password);
            if (!KeyAuthApp.data.success)
            {
                std::remove("fent.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
                Sleep(1500);
                exit(1);
            }
            std::cout << skCrypt("[fent external] > auto logged in\n");
        }
    }
    else
    {
        int option;
        std::string username;
        std::string password;
        std::string key;

        std::cout << skCrypt("[fent external] > license key -> ");
        std::cin >> key;
        KeyAuthApp.license(key);

        if (!KeyAuthApp.data.success)
        {
            std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
            Sleep(1500);
            exit(1);
        }
        if (username.empty() || password.empty())
        {
            WriteToJson("fent.json", "license", key, false, "", "");
            std::cout << skCrypt("[fent external] > created file for auto-login\n");
        }
        else
        {
            WriteToJson("fent.json", "username", username, true, "password", password);
            std::cout << skCrypt("[fent external] > created file for auto-login\n");
        }
    }

    std::string appdata = silence::utils::appdata_path();
    if (!std::filesystem::exists(appdata + "\\fent"))
    {
        std::filesystem::create_directory(appdata + "\\fent");
    }

    if (!std::filesystem::exists(appdata + "\\fent\\configs"))
    {
        std::filesystem::create_directory(appdata + "\\fent\\configs");

    }

    silence::roblox::init();
    printf("[fent external] > press enter to close console [closes cheat]");
    std::cin.get();

    //system("pause");
}