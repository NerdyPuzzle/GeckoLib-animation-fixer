#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma warning(disable : 4996)
#include <iostream>
#include <raylib.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "rlImGui.h" 
#include "imgui_notify.h"
#include "icon.h"

namespace fs = std::filesystem;

using json = nlohmann::ordered_json;

pthread_t workload_thread;

std::string workspace_dir = (std::string)getenv("USERPROFILE") + "\\MCreatorWorkspaces";

std::vector<std::pair<std::string, std::string>> geckolib_workspaces; // foldername, modid

void SetupImGuiStyle() {
	// Photoshop style by Derydoca from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 4.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 0.f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 13.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabMinSize = 7.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 1.5f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.5f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2980392277240753f, 0.2980392277240753f, 0.2980392277240753f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.6588235294117647f, 0.6392156862745098f, 0.6392156862745098f, 0.5f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.3910000026226044f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6700000166893005f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5843137502670288f, 0.5843137502670288f, 0.5843137502670288f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
}

namespace ImGui {

	void LoadingIndicatorCircle(const char* label, const float indicator_radius,
		const ImVec4& main_color, const ImVec4& backdrop_color,
		const int circle_count, const float speed) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) {
			return;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(label);

		const ImVec2 pos = window->DC.CursorPos;
		const float circle_radius = indicator_radius / 10.0f;
		const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.0f,
			pos.y + indicator_radius * 2.0f));
		ItemSize(bb, GetStyle().FramePadding.y);
		if (!ItemAdd(bb, id)) {
			return;
		}
		const float t = g.Time;
		const auto degree_offset = 2.0f * IM_PI / circle_count;
		for (int i = 0; i < circle_count; ++i) {
			const auto x = indicator_radius * std::sin(degree_offset * i);
			const auto y = indicator_radius * std::cos(degree_offset * i);
			const auto growth = std::max(0.0f, std::sin(t * speed - i * degree_offset));
			ImVec4 color;
			color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
			color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
			color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
			color.w = 1.0f;
			window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x,
				pos.y + indicator_radius - y),
				circle_radius + growth * circle_radius,
				GetColorU32(color));
		}
	}

}

bool scanning = true;
bool fixing = false;
bool notification = false;

bool fixer = false;
bool fixer_set_pos = true;
std::string animation;

int fixcount = 0;
int deletecount = 0;
int samecount = 0;

int selected_workspace = -1;

void* ScanWorkspaces(void* args) {
	scanning = true;

	for (fs::path entry : fs::directory_iterator(workspace_dir)) {
		if (fs::is_directory(entry)) {
			if (fs::exists(entry.string() + "\\src\\main\\resources\\assets")) {
				for (fs::path entry2 : fs::directory_iterator(entry.string() + "\\src\\main\\resources\\assets")) {
					if (fs::exists(entry2.string() + "\\animations")) {
						geckolib_workspaces.push_back({ entry.filename().string(), entry2.filename().string() });
					}
				}
			}
		}
	}
	_sleep(1000);

	scanning = false;
	return NULL;
}

void* FixAnimations(void* args) {
	fixing = true;

	for (fs::path entry : fs::directory_iterator(workspace_dir + "\\" + geckolib_workspaces[selected_workspace].first + "\\src\\main\\resources\\assets\\" + geckolib_workspaces[selected_workspace].second + "\\animations")) {
		if (entry.filename().extension().string() != ".json")
			continue;
		
		animation = "Fixing " + entry.filename().string();

		std::vector<std::string> anim;
		std::string temp;

		bool foundvector = false;
		bool foundissue = false;

		std::ifstream format(entry.string());

		bool first = true;

		json file = json::parse(format);
		
		format.close();

		std::ofstream dump(entry.string());

		dump << file.dump(2);

		dump.close();

		std::ifstream in(entry.string());

		while (std::getline(in, temp)) {
			if (!foundvector && temp.find("\"vector\": [") != std::string::npos)
				foundvector = true;

			anim.push_back(temp);

			if (first)
				first = false;
		}

		in.close();

		if (!foundvector) {
			deletecount++;
			fs::remove(entry.string());
		}
		else {
			std::string newanim = "";

			bool premode = false;
			bool catmode = false;
			
			int i = 0;
			for (const std::string line : anim) {
				if (line.find("\"pre\": {") != std::string::npos) {
					foundissue = true;
					anim[i].clear();
					anim[i + 6].clear();
					anim[i + 7].clear();
					anim[i + 8].clear();
					anim[i + 9].clear();
					anim[i + 10].clear();
					anim[i + 11].clear();
					anim[i + 12].clear();
					anim[i + 13].clear();
				}
				else if (line.find("\"post\": {") != std::string::npos) {
					foundissue = true;
					anim[i].clear();
					anim[i + 6].clear();
					anim[i + 7].clear();
				}
				else if (line.find("\"lerp_mode\": \"catmullrom\"") != std::string::npos) {
					foundissue = true;
					anim[i].clear();
				}
				else {
					newanim += " " + line;
				}
				i++;
			}

			json newfile = json::parse(newanim);

			std::ofstream out(entry.string());
			out << newfile.dump(2);
			out.close();
			
			if (foundissue)
				fixcount++;
		}

		if (foundvector && !foundissue)
			samecount++;

		_sleep(500);
	}

	if (fixcount > 0) {
		std::string content = "Fixed " + std::to_string(fixcount) + " animations from the workspace " + geckolib_workspaces[selected_workspace].first;
		ImGui::InsertNotification({ ImGuiToastType::Success, 3500, content.c_str() });
	}

	if (deletecount > 0) {
		std::string content = "Deleted " + std::to_string(deletecount) + " bedrock format animations from the workspace " + geckolib_workspaces[selected_workspace].first;
		ImGui::InsertNotification({ ImGuiToastType::Warning, 3500, content.c_str() });
	}
	
	if (fixcount == 0 && deletecount == 0) {
		ImGui::InsertNotification({ ImGuiToastType::Info, 3500, "No animation file issues found" });
	}

	fixing = false;
	fixer = false;
	return NULL;
}

void main() {

    InitWindow(800, 395, "GeckoLib animation fixer");
    SetTargetFPS(60);

	Image icon = { 0 };
	icon.data = ICON_DATA;
	icon.width = ICON_WIDTH;
	icon.height = ICON_HEIGHT;
	icon.format = ICON_FORMAT;
	icon.mipmaps = 1;

	SetWindowIcon(icon);

	rlImGuiSetup(true);
	SetupImGuiStyle();

	pthread_create(&workload_thread, NULL, ScanWorkspaces, NULL);

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
		rlImGuiBegin();

		if (fixer) {
			if (fixer_set_pos) {
				fixer_set_pos = false;
				ImGui::OpenPopup("fixer");
				ImGui::SetNextWindowPos({ (float)(GetScreenWidth() - 300) / 2, (float)(GetScreenHeight() - 150) / 2 });
				ImGui::SetNextWindowSize({ 300, 150 });
				pthread_create(&workload_thread, NULL, FixAnimations, NULL);
			}
			if (ImGui::BeginPopupModal("fixer", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
				ImGui::SetCursorPos({ 115, 30 });
				ImGui::LoadingIndicatorCircle(" ", 35, ImVec4(0.6588235294117647f, 0.6392156862745098f, 0.6392156862745098f, 0.5f), ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f), 10, 10);
				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
				if (!animation.empty()) {
					ImVec2 content_max = ImGui::GetWindowContentRegionMax();
					ImVec2 content_min = ImGui::GetWindowContentRegionMin();
					ImGui::SetCursorPosX((content_min.x + content_max.x - ImGui::CalcTextSize(animation.c_str()).x) / 2.0f);
					ImGui::Text(animation.c_str());
				}
				ImGui::EndPopup();
			}
		}
		else {
			if (!fixer_set_pos) {
				fixer_set_pos = true;
				selected_workspace = -1;
				animation.clear();
				fixcount = 0;
				deletecount = 0;
				samecount = 0;
			}
		}

		ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ (float) GetScreenWidth(), (float) GetScreenHeight() }, ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		if (ImGui::Begin("main", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
			
			ImGui::BeginChild("a", {ImGui::GetColumnWidth(), 350}, true);

			if (scanning) {
				ImGui::SetCursorPosX(345);
				ImGui::SetCursorPosY(125);
				ImGui::LoadingIndicatorCircle(" ", 50, ImVec4(0.6588235294117647f, 0.6392156862745098f, 0.6392156862745098f, 0.5f), ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f), 10, 10);
			}
			else {
				int i = 0;
				for (const std::pair<std::string, std::string> str : geckolib_workspaces) {
					if (ImGui::Selectable(str.first.c_str(), selected_workspace == i))
						selected_workspace = i;
					i++;
				}
			}
			
			ImGui::EndChild();

			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3882352941176471f, 0.6588235294117647f, 0.2549019607843137f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2980392156862745f, 0.5058823529411765f, 0.196078431372549f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5490196078431373f, 0.7764705882352941f, 0.4235294117647059f, 1.0f });

			if (ImGui::Button("Fix workspace animations") && !scanning && selected_workspace != -1 && !fixer) {
				fixer = true;
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::SameLine();

			if (ImGui::Button("Scan again") && !scanning) {
				selected_workspace = -1;
				geckolib_workspaces.clear();
				pthread_create(&workload_thread, NULL, ScanWorkspaces, NULL);
			}

			ImGui::End();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		ImGui::RenderNotifications();

		rlImGuiEnd();
        BeginDrawing();
        EndDrawing();
    }

	rlImGuiShutdown();
    CloseWindow();
}
