//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>
#include <thread>
#include <iostream>

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glfw3.h"

//engine
#include "compile.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "gameobject.hpp"
#include "render.hpp"
#include "gui_settings.hpp"

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::exception;
using std::filesystem::exists;
using std::thread;
using std::ofstream;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::Mesh;
using Graphics::Render;
using Graphics::GUI::GUISettings;

namespace Core
{
	void Compilation::Compile()
	{
		renderBuildingWindow = true;

		thread CompileThread([]()
			{
				//
				// START BUILDING GAME FROM SOURCE CODE
				//

				string gameBatPath = Engine::gamePath + "\\build.bat";
				gameBatPath = String::CharReplace(gameBatPath, '/', '\\');
				if (!exists(gameBatPath)) return;
				int result = File::RunBatFile(gameBatPath, false, File::BatType::compile);
				File::MoveOrRenameFileOrFolder(
					Engine::gameParentPath + "/Game.exe",
					Engine::gameExePath,
					true);

				if (result != 0)
				{
					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Compilation failed! Please contact developers for more info.\n");

					renderBuildingWindow = false;

					return;
				}

				//
				// REMOVE OLD PROJECT FILES FROM GAME
				//

				string gameProjectFolder = Engine::gameParentPath + "\\files\\project";
				for (const auto& item : directory_iterator(path(gameProjectFolder)))
				{
					if (is_directory(item.path())
						|| is_regular_file(item.path()))
					{
						string itemPath = item.path().string();

						File::DeleteFileOrfolder(itemPath);
					}
				}

				//
				// ADD NEW PROJECT FILES TO GAME
				//

				string scenePath = path(Engine::projectPath).parent_path().string();
				for (const auto& item : directory_iterator(path(scenePath)))
				{
					string itemPath = item.path().string();

					string ending = item.is_directory()
						? path(item).filename().string()
						: path(item).stem().string() + path(item).extension().string();
					string targetItemPath = gameProjectFolder + "\\" + ending;

					File::CopyFileOrFolder(itemPath, targetItemPath);
				}

				//
				// CREATE DUPLICATE SCENE FILES WHICH THE GAME WILL USE 
				// TO KNOW WHERE TO FIND ITS PROJECT FILES FROM
				//

				/*
				string gameProjectPath = path(Engine::gameExePath).parent_path().string() + "\\files\\project";
				gameProjectPath = String::CharReplace(gameProjectPath, '/', '\\');
				//cout << "game project path: " << gameProjectPath << "\n\n";

				for (const auto& sceneFile : directory_iterator(Engine::sceneParentPath))
				{

				}
				*/

				//
				// FINISHED COMPILATION
				//

				renderBuildingWindow = false;

				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::INFO,
					"Compilation succeeded! Running game.\n");

				File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
			});

		CompileThread.detach();
	}

	void Compilation::RenderBuildingWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoDocking;

		if (renderBuildingWindow
			&& ImGui::Begin("##Building", NULL, windowFlags))
		{
			string text = "Building " + GUISettings::gameName + "...";
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

			float windowWidth = ImGui::GetWindowWidth();
			float windowHeight = ImGui::GetWindowHeight() / 2 - 25;

			windowWidth = (windowWidth - textSize.x) / 2.0f;

			ImVec2 textPos = ImVec2(windowWidth, windowHeight);
			ImGui::SetCursorPos(textPos);

			ImGui::Text(text.c_str());

			ImGui::End();
		}
	}
}