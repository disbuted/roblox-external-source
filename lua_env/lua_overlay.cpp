#include "../../utils/overlay/imgui/imgui.h"
#include "../../utils/overlay/imgui/TextEditor.h"
#include "../roblox/globals/globals.hpp"

namespace silence
{
	namespace lua_overlay
	{
		// init the editor
		void init()
		{
			auto lang = TextEditor::LanguageDefinition::Lua();
			globals::lua_editor.SetLanguageDefinition(lang);
		}
		void render()
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save"))
					{
						auto textToSave = globals::lua_editor.GetText();
						/// save text....
					}
					if (ImGui::MenuItem("Load"))
					{
						auto textToSave = globals::lua_editor.GetText();
						/// save text....
					}
						
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = globals::lua_editor.IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						globals::lua_editor.SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && globals::lua_editor.CanUndo()))
						globals::lua_editor.Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && globals::lua_editor.CanRedo()))
						globals::lua_editor.Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, globals::lua_editor.HasSelection()))
						globals::lua_editor.Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && globals::lua_editor.HasSelection()))
						globals::lua_editor.Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && globals::lua_editor.HasSelection()))
						globals::lua_editor.Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						globals::lua_editor.Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						globals::lua_editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(globals::lua_editor.GetTotalLines(), 0));

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::MenuItem("Dark palette"))
						globals::lua_editor.SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Light palette"))
						globals::lua_editor.SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Retro blue palette"))
						globals::lua_editor.SetPalette(TextEditor::GetRetroBluePalette());
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Execute"))
				{
					if (ImGui::MenuItem("Run", "F5"))
					{
						globals::luaVM.RunScriptSafe(globals::lua_editor.GetText());
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			auto cpos = globals::lua_editor.GetCursorPosition();
			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, globals::lua_editor.GetTotalLines(),
				globals::lua_editor.IsOverwrite() ? "Ovr" : "Ins",
				globals::lua_editor.CanUndo() ? "*" : " ",
				globals::lua_editor.GetLanguageDefinition().mName.c_str(), "Nofile");
			globals::lua_editor.Render("TextEditor");

		}
	}
}