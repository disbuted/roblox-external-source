#define IMGUI_DEFINE_MATH_OPERATORS

#include "overlay.hpp"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_toggle.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dwmapi.h>

#include "../../roblox/esp/esp.hpp"
#include "../../roblox/globals/globals.hpp"

// lua env
#include "../../lua_env/lua_overlay.hpp"
#include "../../lua_env/LuaVM.hpp"

#include "ckeybind/keybind.hpp"
#include "../skcrypt/skStr.hpp"
#include "../xorstr/xorstr.hpp"

#include "../configs/configs.hpp"
#include "../json/json.hpp"
#include <thread>

#include <filesystem>

//keyauth info removed -> change globals::show_auth to false
//auto name = skCrypt("111");
//auto ownerid = skCrypt("111");
//auto secret = skCrypt("111");
//auto version = skCrypt("1.0");
//auto url = skCrypt("https://keyauth.win/api/1.2/");

//KeyAuth::api KeyAuthApp(name.decrypt(), ownerid.decrypt(), secret.decrypt(), version.decrypt(), url.decrypt());

ID3D11Device* silence::utils::overlay::d3d11_device = nullptr;

ID3D11DeviceContext* silence::utils::overlay::d3d11_device_context = nullptr;

IDXGISwapChain* silence::utils::overlay::dxgi_swap_chain = nullptr;

ID3D11RenderTargetView* silence::utils::overlay::d3d11_render_target_view = nullptr;

static const char* combo_items_4[5] = { ("Head"), ("UpperTorso"), ("HumanoidRootPart"), ("LowerTorso"), ("Bones")};
static const char* box_items_3[3] = { ("2D Box"), ("2D Corner Box"), ("3d Box")};

bool Keybind(CKeybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;
	// SetCursorPosX(window->Size.x - 14 - size_arg.x);
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(keybind->get_name().c_str());
	const ImVec2 label_size = ImGui::CalcTextSize(keybind->get_name().c_str(), NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
		style.FramePadding.y <
		window->DC.CurrLineTextBaseOffset)  // Try to vertically align buttons that are smaller/have no padding so that
		// text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = ImGui::CalcItemSize(
		size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	bool value_changed = false;
	int key = keybind->key;

	auto io = ImGui::GetIO();

	std::string name = keybind->get_key_name();

	if (keybind->waiting_for_input)
		name = "waiting";

	if (ImGui::GetIO().MouseClicked[0] && hovered)
	{
		if (g.ActiveId == id)
		{
			keybind->waiting_for_input = true;
		}
	}
	else if (ImGui::GetIO().MouseClicked[1] && hovered)
	{
		ImGui::OpenPopup(keybind->get_name().c_str());
	}
	else if (ImGui::GetIO().MouseClicked[0] && !hovered)
	{
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	if (keybind->waiting_for_input)
	{
		if (ImGui::GetIO().MouseClicked[0] && !hovered)
		{
			keybind->key = VK_LBUTTON;

			ImGui::ClearActiveID();
			keybind->waiting_for_input = false;
		}
		else
		{
			if (keybind->set_key())
			{
				ImGui::ClearActiveID();
				keybind->waiting_for_input = false;
			}
		}
	}

	// Render
	ImVec4 textcolor = ImLerp(ImVec4(201 / 255.f, 204 / 255.f, 210 / 255.f, 1.f), ImVec4(1.0f, 1.0f, 1.0f, 1.f), 1.f);

	window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(33 / 255.0f, 33 / 255.0f, 33 / 255.0f, 1.f), 2.f);
	//window->DrawList->AddRect( bb.Min, bb.Max, ImColor( 0.f, 0.f, 0.f, 1.f ) );

	window->DrawList->AddText(
		bb.Min +
		ImVec2(
			size_arg.x / 2 - ImGui::CalcTextSize(name.c_str()).x / 2,
			size_arg.y / 2 - ImGui::CalcTextSize(name.c_str()).y / 2),
		ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
		name.c_str());

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar;
	//ImGui::SetNextWindowPos( pos + ImVec2( 0, size_arg.y - 1 ) );
	//ImGui::SetNextWindowSize( ImVec2( size_arg.x, 47 * 1.f ) );

	{
		if (ImGui::BeginPopup(keybind->get_name().c_str(), 0))
		{

			{
				{
					ImGui::BeginGroup();
					{
						if (ImGui::Selectable("hold", keybind->type == CKeybind::HOLD))
							keybind->type = CKeybind::HOLD;
						if (ImGui::Selectable("toggle", keybind->type == CKeybind::TOGGLE))
							keybind->type = CKeybind::TOGGLE;
					}
					ImGui::EndGroup();
				}
			}

			ImGui::EndPopup();
		}
	}

	return pressed;
}

bool silence::utils::overlay::fullsc(HWND windowHandle)
{
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	if (GetMonitorInfo(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
	{
		RECT windowRect;
		if (GetWindowRect(windowHandle, &windowRect))
		{
			return windowRect.left == monitorInfo.rcMonitor.left
				&& windowRect.right == monitorInfo.rcMonitor.right
				&& windowRect.top == monitorInfo.rcMonitor.top
				&& windowRect.bottom == monitorInfo.rcMonitor.bottom;
		}
	}
}

std::string GetConfigFolderPath()
{
	std::string configFolderPath = silence::utils::appdata_path() + "\\fent\\configs";

	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directory(configFolderPath);
	}

	return configFolderPath;
}

bool isValid(const std::string& str) {
	bool startsWithNumber = std::isdigit(str[0]);
	bool containsSymbol = false;

	for (char ch : str) {
		if (!std::isalnum(ch) && ch != '_') {
			containsSymbol = true;
			break;
		}
	}

	return startsWithNumber || containsSymbol;
}


void DisplayInstanceTree(silence::roblox::instance_t instance, std::string parentPath = "NULL")
{
	std::string instancePath;
	std::string istanceName = instance.name();

	if (parentPath == "NULL") {
		instancePath = "game:GetService(\"";
	}
	else {
		instancePath = (parentPath == "game:GetService(\"") ? parentPath + istanceName + "\")" : (!isValid(istanceName)) ? parentPath + "[\"" + istanceName + "\"]" : parentPath + "." + instance.name();
	}

	for (auto& child : instance.children())
	{
		std::string childName = child.name();
		std::string childPath = instancePath + "[\"" + childName + "\"]";

		ImGui::PushID(childPath.c_str());
		bool isNodeOpen = ImGui::TreeNodeEx(childName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(1))
		{
			ImGui::OpenPopup("Context Menu");
		}

		if (ImGui::BeginPopup("Context Menu"))
		{
			if (ImGui::MenuItem("Copy Path"))
			{
				ImGui::SetClipboardText(childPath.c_str());
			}
			ImGui::EndPopup();
		}

		if (isNodeOpen)
		{
			DisplayInstanceTree(child, instancePath);
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

static char text[999] = "";
char configname[100];

bool silence::utils::overlay::init = false;

bool if_first = false;

bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Button(label);
}

const char* fontPath = "C:/Users/Admin/Downloads/DOWNLOADS/Websites CCs and Fonts/Circular-Std-Bold.ttf";

bool silence::utils::overlay::render()
{
	//name.clear(); ownerid.clear(); secret.clear(); version.clear(); url.clear();

	/*if (if_first)
	{
		KeyAuthApp.init();
		if_first = false;
	}*\

	nlohmann::json json;

	/*if (globals::show_auth)
	{
		std::string filePath = appdata_path() + XorStr("\\silence\\license.json");
		if (std::filesystem::exists(filePath))
		{
			std::ifstream file(filePath);
			if (file.is_open())
			{
				file >> json;
				file.close();

				std::string stored_license = json["license"];
				KeyAuthApp.license(stored_license);

				if (KeyAuthApp.data.success)
				{
					globals::show_auth = false;
				}
				else
				{
					exit(0);
				}
			}
		}
		*/
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpfnWndProc = window_proc;
	wc.lpszClassName = TEXT("flexmusix");
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wc);
	const HWND hw = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, TEXT("4daflex"),
		WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

	SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
	const MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(hw, &margin);

	if (!create_device_d3d(hw))
	{
		cleanup_device_d3d();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	ShowWindow(hw, SW_SHOW);
	UpdateWindow(hw);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui::GetIO().IniFilename = nullptr;

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 originalButtonColor = style.Colors[ImGuiCol_Button];

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, 14.0f);

	io.FontDefault = font;

	style.WindowRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.ChildRounding = 3.0f;
	style.ScrollbarRounding = 0;

    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.90f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.00f, 0.50f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.82f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.84f);

    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.85f, 0.85f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 0.80f, 0.45f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.82f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.50f, 0.00f, 0.50f, 0.65f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.50f, 0.00f, 0.50f, 0.65f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.50f, 0.00f, 0.50f, 0.65f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.45f, 0.45f, 0.45f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 0.91f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.83f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.00f, 0.50f, 0.65f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.15f, 0.15f, 0.83f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.83f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.05f, 0.05f, 0.05f, 0.83f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.05f, 0.05f, 0.05f, 0.83f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.95f, 0.95f, 0.95f, 0.85f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.90f, 0.90f, 0.90f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.85f, 0.85f, 0.85f, 0.90f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);


	ImGui_ImplWin32_Init(hw);
	ImGui_ImplDX11_Init(d3d11_device, d3d11_device_context);

	const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	init = true;

	bool draw = false;
	bool done = false;
	int tab = 0;

	silence::lua_overlay::init();
	globals::luaVM.init();
	while (!done)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}

		move_window(hw);

		if (GetAsyncKeyState(VK_INSERT) & 1)
			draw = !draw;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			if (GetForegroundWindow() == FindWindowA(0, XorStr("Roblox")) || GetForegroundWindow() == hw)
			{
				ImGui::Begin("overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
				{
					silence::roblox::hook_esp();
					ImGui::End();
				}

				if (draw)
				{

					ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
					ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.80f, 0.80f, 0.80f, 1.00f));
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.80f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.05f, 0.05f, 0.05f, 0.80f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.10f, 0.10f, 0.10f, 0.80f));

					ImGui::SetNextWindowSize(ImVec2(705, 420));
					ImGui::Begin(XorStr("Fent External [DEV BUILD]"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
					ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(685, 35), true);

					if (tab == 0)
					{
						ImGui::EndChild();

						ImGui::SetCursorPosX(8);
						ImGui::SetCursorPosY(70);

						float windowWidth = 678;

						ImGui::BeginChild(XorStr("LeftTab"), ImVec2(windowWidth / 2, 340), true, ImGuiWindowFlags_NoScrollbar);
						{
							const ImVec4 green(1.0f, 0.7137f, 0.7568f, 0.65f);
							const ImVec4 green_hover(1.0f, 0.70f, 0.80f, 0.85f);

							ImGui::PushStyleColor(ImGuiCol_Button, green);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green_hover);

							ImGui::Toggle(XorStr("Enable"), &globals::aimbot);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Enables Aimbot");
								ImGui::EndTooltip();
							}

							ImGui::SameLine(); Keybind(&globals::aimbotkey, ImVec2(60, 20));
							ImGui::Toggle(XorStr("Sticky Aim"), &globals::sticky_aim);


							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Locks Onto Person You Toggle On And Sticks");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Prediction"), &globals::prediction);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Changes The Predication Of The Aimlock");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Auto Pred"), &globals::autopred);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Auto Predicts To Your Ping [BETA]");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Shake"), &globals::shake);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Shakes Cursor When Locked On");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Resolver"), &globals::resolver);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Resolves Anti-Aim");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Deadzone"), &globals::deadzone);

							ImGui::Toggle(XorStr("Stabilizer"), &globals::stabilizer);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Stabilizes Aimlock");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Teamcheck"), &globals::team_check);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Team Checks Who You Lock Onto");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Disable Outside of FOV"), &globals::disable_outside_fov);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Wont Lock Onto Anyone Outside Of Your FOV");
								ImGui::EndTooltip();
							}


							ImGui::PopStyleColor(2);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild(XorStr("RightTab"), ImVec2(windowWidth / 2, 340), true, ImGuiWindowFlags_NoScrollbar);
						{
							ImGui::Combo(XorStr("Part"), &globals::aimpart, combo_items_4, 4);
							ImGui::SliderInt(XorStr("FOV"), &globals::fov, 1, 1000);
							ImGui::SliderInt(XorStr("Deadzone"), &globals::deadzone_value, 1, 1000);
							ImGui::Separator();
							ImGui::SliderFloat(XorStr("Smoothness X"), &globals::smoothness_x, 1, 30);
							ImGui::SliderFloat(XorStr("Smoothness Y"), &globals::smoothness_y, 1, 30);
							ImGui::Separator();
							ImGui::SliderFloat(XorStr("Prediction X"), &globals::prediction_x, 1, 10);
							ImGui::SliderFloat(XorStr("Prediction Y"), &globals::prediction_y, 1, 10);
							ImGui::Separator();
							ImGui::SliderFloat(XorStr("Stabilizer X"), &globals::stabilizer_x, 1, 5);
							ImGui::SliderFloat(XorStr("Stabilizer Y"), &globals::stabilizer_y, 1, 5);
							ImGui::Separator();
							ImGui::SliderFloat(XorStr("Shake Strenght"), &globals::shake_value, 0, 10);
							ImGui::SliderFloat(XorStr("Sensitivity"), &globals::sensitivity, 0, 5);
						}
						ImGui::EndChild();

						ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(110, 358), true);
					}

					if (tab == 1)
					{
						static ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

						ImGui::EndChild();

						ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(110, 358), true);
						ImGui::EndChild();

						ImGui::SetCursorPosX(8);
						ImGui::SetCursorPosY(70);

						float windowWidth = 678;

						ImGui::BeginChild(XorStr("LeftTab"), ImVec2(windowWidth / 2, 340), true, ImGuiWindowFlags_NoScrollbar);
						{

							const ImVec4 green(1.0f, 0.7137f, 0.7568f, 0.65f);
							const ImVec4 green_hover(1.0f, 0.70f, 0.80f, 0.85f);

							ImGui::PushStyleColor(ImGuiCol_Button, green);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green_hover);

							ImGui::Toggle(XorStr("Enabled"), &globals::esp);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Enables ESP");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Box"), &globals::box);
							
							ImGui::Toggle(XorStr("Outline"), &globals::outline);
							ImGui::Toggle(XorStr("Fill Box"), &globals::filledbox);
							ImGui::Toggle(XorStr("Rainbow Box"), &globals::rainbowbox);
							ImGui::Combo(XorStr("Type"), &globals::boxtype, box_items_3, 2);
							ImGui::Separator();
							ImGui::Toggle(XorStr("Name"), &globals::name_esp);
							ImGui::Toggle(XorStr("Healthbar"), &globals::healthbar);
							ImGui::Toggle(XorStr("Distance"), &globals::distance_esp);
							ImGui::Toggle(XorStr("Tracers"), &globals::tracers);
							ImGui::Toggle(XorStr("Health Info"), &globals::healthinfo);
							ImGui::Toggle(XorStr("Local ESP"), &globals::local_box);
							ImGui::Toggle(XorStr("Teamcheck"), &globals::team_check_esp);
							ImGui::Separator();
							ImGui::Toggle(XorStr("Draw FOV"), &globals::fov_on);
							ImGui::Toggle(XorStr("Fill FOV"), &globals::fov_filled);
							ImGui::Toggle(XorStr("Draw Deadzone"), &globals::show_deadzone);
							ImGui::Toggle(XorStr("Crosshair"), &globals::crosshair_on);

							ImGui::PopStyleColor(2);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild(XorStr("RightTab"), ImVec2(windowWidth / 2, 340), true, ImGuiWindowFlags_NoScrollbar);
						{
							ImGui::ColorEdit4("Box Color", globals::box_color, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit4("Fill Color", globals::fill_color, ImGuiColorEditFlags_NoInputs);
							ImGui::Separator();
							ImGui::ColorEdit4("Name Color", globals::name_color, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit4("Tracers Color", globals::tracers_color, ImGuiColorEditFlags_NoInputs);
							ImGui::Separator();
							ImGui::ColorEdit4("FOV Color", globals::fov_color, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit4("FOV Filled Color", globals::fov_fill_color, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit4("Deadzone Color", globals::deadzone_color, ImGuiColorEditFlags_NoInputs);
						}
						ImGui::EndChild();

						ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(110, 358), true);
					}



					if (tab == 2)
					{
						ImGui::EndChild();

						ImGui::SetCursorPosX(8);
						ImGui::SetCursorPosY(70);

						ImGui::BeginChild(XorStr("SettingsChildWindow"), ImVec2(685, 340), true);
						{
							const ImVec4 green(1.0f, 0.7137f, 0.7568f, 0.65f);
							const ImVec4 green_hover(1.0f, 0.70f, 0.80f, 0.85f);

							ImGui::PushStyleColor(ImGuiCol_Button, green);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green_hover);

							ImGui::Toggle(XorStr("Healthcheck"), &globals::healthcheck);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Checks For Health Of Players And Checks Priority");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Knockcheck"), &globals::knock_check);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Checks To See If Target Is Knocked Down / Dead");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("Streamproof"), &globals::streamproof);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Hides From Recording Software; Medal, Obs Etc");
								ImGui::EndTooltip();
							}

							ImGui::Toggle(XorStr("V-Sync"), &globals::vsync);

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Turn V-Sync off if you have a good PC");
								ImGui::EndTooltip();
							}

							ImGui::PopStyleColor(2);

							ImGui::Separator();

							ImGui::Text(XorStr("Configs"));

							std::vector<std::string> configFiles;	
							std::string configFolderPath = GetConfigFolderPath();
							for (auto file : std::filesystem::directory_iterator(configFolderPath)) {
								std::filesystem::path filePath = file;
								std::string extension = filePath.extension().string();

								if (extension == ".cfg") {
									if (!std::filesystem::is_directory(file.path())) {
										auto path2 = file.path().string().substr(configFolderPath.length() + 1);
										configFiles.push_back(path2.c_str());
									}
								}
							}

							static std::string current_item = configFiles.empty() ? "" : configFiles[0];

							for (int n = 0; n < configFiles.size(); n++) {
								if (configFiles[n] == "")
									continue;

								bool is_selected = (current_item == configFiles[n]);
								if (ImGui::Selectable(configFiles[n].c_str(), is_selected)) {
									current_item = configFiles[n];

									size_t extensionPos = current_item.find(".cfg");
									if (extensionPos != std::string::npos) {
										current_item.erase(extensionPos);
									}

									strncpy(configname, current_item.c_str(), IM_ARRAYSIZE(configname));
								}
							}

							ImGui::InputText(XorStr("Config Name"), configname, IM_ARRAYSIZE(configname));
							if (ImGui::Button(XorStr("Load"))) {
								silence::utils::configs::load(configname);
							}

							ImGui::SameLine();

							if (ImGui::Button(XorStr("Save"))) {
								silence::utils::configs::save(configname);
							}

							ImGui::SameLine();

							if (ImGui::Button(XorStr("Open Folder Location")))
							{
								ShellExecute(NULL, "open", "explorer.exe", GetConfigFolderPath().c_str(), NULL, SW_SHOWNORMAL);
							}

							ImGui::SameLine();

							if (ImGui::Button(XorStr("Unload")))
							{
								exit(1);
							}

							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::Text("Unloads External From Roblox");
								ImGui::EndTooltip();
							}


						}
						ImGui::EndChild();

						ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(110, 358), true);
					}

					if (tab == 3)
					{
						ImGui::EndChild();
						ImGui::SetCursorPosX(8);
						ImGui::SetCursorPosY(70);

						ImGui::BeginChild(XorStr("LuaEnvWindow"), ImVec2(685, 340), true, ImGuiWindowFlags_MenuBar);
						{
							silence::lua_overlay::render();
						}
						ImGui::EndChild();

						ImGui::BeginChild(XorStr("ChildWindow"), ImVec2(110, 358), true);
					}

					if (ImGui::Button(XorStr("Aimbot")))
					{
						tab = 0;
					}
					ImGui::SameLine();

					if (ImGui::Button(XorStr("Visuals")))
					{
						tab = 1;
					}
					ImGui::SameLine();

					if (ImGui::Button(XorStr("Settings")))
					{
						tab = 2;
					}
					ImGui::SameLine();

					if (ImGui::Button(XorStr("Discord")))
					{
						tab = 3;
					}

					ImGui::PopStyleColor(4);

					ImGui::EndChild();
					ImGui::End();
				}
			}


			if (globals::streamproof)
			{
				SetWindowDisplayAffinity(hw, WDA_EXCLUDEFROMCAPTURE);
			}
			else
			{
				SetWindowDisplayAffinity(hw, WDA_NONE);
			}

			//if (globals::show_auth)
			if (draw)
			{
				SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			}
			else
			{
				SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			}

			ImGui::EndFrame();
			ImGui::Render();

			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d11_device_context->OMSetRenderTargets(1, &d3d11_render_target_view, nullptr);
			d3d11_device_context->ClearRenderTargetView(d3d11_render_target_view, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (globals::vsync)
			{
				dxgi_swap_chain->Present(1, 0);
			}
			else
			{
				dxgi_swap_chain->Present(0, 0);

			}
		}
	}

	init = false;


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d3d();
	DestroyWindow(hw);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}


void silence::utils::overlay::move_window(HWND hw)
{
	HWND target = FindWindowA(0, XorStr("Roblox"));
	HWND foregroundWindow = GetForegroundWindow();

	if (target != foregroundWindow && hw != foregroundWindow)
	{
		MoveWindow(hw, 0, 0, 0, 0, true);
	}
	else
	{
		RECT rect;
		GetWindowRect(target, &rect);

		int rsize_x = rect.right - rect.left;
		int rsize_y = rect.bottom - rect.top;

		if (fullsc(target))
		{
			rsize_x += 16;
			rsize_y -= 24;
		}
		else
		{
			rsize_y -= 63;
			rect.left += 8;
			rect.top += 31;
		}

		MoveWindow(hw, rect.left, rect.top, rsize_x, rsize_y, TRUE);
	}
}

bool silence::utils::overlay::create_device_d3d(HWND hw)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hw;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const UINT create_device_flags = 0;
	D3D_FEATURE_LEVEL d3d_feature_level;
	const D3D_FEATURE_LEVEL feature_level_arr[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_arr, 2, D3D11_SDK_VERSION, &sd, &dxgi_swap_chain, &d3d11_device, &d3d_feature_level, &d3d11_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_arr, 2, D3D11_SDK_VERSION, &sd, &dxgi_swap_chain, &d3d11_device, &d3d_feature_level, &d3d11_device_context);
	if (res != S_OK)
		return false;

	create_render_target();
	return true;
}

void silence::utils::overlay::cleanup_device_d3d()
{
	cleanup_render_target();

	if (dxgi_swap_chain)
	{
		dxgi_swap_chain->Release();
		dxgi_swap_chain = nullptr;
	}

	if (d3d11_device_context)
	{
		d3d11_device_context->Release();
		d3d11_device_context = nullptr;
	}

	if (d3d11_device)
	{
		d3d11_device->Release();
		d3d11_device = nullptr;
	}
}

void silence::utils::overlay::create_render_target()
{
	ID3D11Texture2D* d3d11_back_buffer;
	dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&d3d11_back_buffer));
	if (d3d11_back_buffer != nullptr)
	{
		d3d11_device->CreateRenderTargetView(d3d11_back_buffer, nullptr, &d3d11_render_target_view);
		d3d11_back_buffer->Release();
	}
}

void silence::utils::overlay::cleanup_render_target()
{
	if (d3d11_render_target_view)
	{
		d3d11_render_target_view->Release();
		d3d11_render_target_view = nullptr;
	}
}

LRESULT __stdcall silence::utils::overlay::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (d3d11_device != nullptr && wParam != SIZE_MINIMIZED)
		{
			cleanup_render_target();
			dxgi_swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			create_render_target();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}