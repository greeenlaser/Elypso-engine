//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <filesystem>

//engine
#include "compile.hpp"
#include "console.hpp"
#include "core.hpp"
#include "stringUtils.hpp"
#include "fileUtils.hpp"
#include "gameobject.hpp"

using std::cout;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::exception;
using std::filesystem::exists;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;
using Utils::String;
using Utils::File;
using Graphics::Shape::GameObject;
using Graphics::Shape::GameObjectManager;
using Graphics::Shape::Mesh;

namespace Core
{
	void Compilation::Compile()
	{
		string gameBatPath = Engine::gamePath + "\\build.bat";
		gameBatPath = String::CharReplace(gameBatPath, '/', '\\');
		if (!exists(gameBatPath))
		{
			return;
		}
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
			return;
		}

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

		string engineProjectPath = path(Engine::projectPath).parent_path().string();
		for (const auto& item : directory_iterator(path(engineProjectPath)))
		{
			string itemPath = item.path().string();

			string ending = item.is_directory()
				? path(item).filename().string()
				: path(item).stem().string() + path(item).extension().string();
			string targetItemPath = gameProjectFolder + "\\" + ending;

			File::CopyFileOrFolder(itemPath, targetItemPath);
		}

		cout << "started running game exe...\n";

		File::RunApplication(Engine::gameParentPath, Engine::gameExePath);
	}
}