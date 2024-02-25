//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

//external
#include "glm.hpp"

//engine
#include "configFile.hpp"
#include "render.hpp"
#include "console.hpp"
#include "stringUtils.hpp"
#include "gui.hpp"
#include "gui_console.hpp"
#include "gui_debugmenu.hpp"
#include "gui_inspector.hpp"
#include "gui_projecthierarchy.hpp"
#include "core.hpp"
#include "input.hpp"

using glm::vec3;
using std::cout;
using std::endl;
using std::cerr;
using std::to_string;
using std::ifstream;
using std::exception;
using std::find_if;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::current_path;
using std::filesystem::create_directory;

using Core::Input;
using Core::Engine;
using Graphics::Render;
using Core::ConsoleManager;
using Utils::String;
using Graphics::GUI::EngineGUI;
using Graphics::GUI::GUIConsole;
using Graphics::GUI::GUIDebugMenu;
using Graphics::GUI::GUIInspector;
using Graphics::GUI::GUIProjectHierarchy;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace EngineFile
{
	void ConfigFileManager::SetConfigValuesToDefaultValues()
	{
		EngineGUI::fontScale = 1.5f;
		//Render::SCR_WIDTH = 1280; //do not uncomment! edit in render.hpp instead!
		//Render::SCR_WIDTH = 720; //do not uncomment! edit in render.hpp instead!
		Render::useMonitorRefreshRate = true;
		Input::fov = 90.0f;
		Input::moveSpeedMultiplier = 1.0f;
		Input::nearClip = 0.001f;
		Input::farClip = 100.0f;
		vec3 newPosition = vec3(0.0f, 1.0f, 0.0f);
		Render::camera.SetCameraPosition(newPosition);
		Input::objectSensitivity = 0.1f;
		//Render::camera.SetCameraRotation(vec3(-90.0f, 0.0f, 0.0f)); //editing this has no effect because camera is initialized later
		GUIConsole::allowScrollToBottom = true;
		ConsoleManager::sendDebugMessages = false;
		GUIDebugMenu::renderDebugMenu = false;
		GUIConsole::renderConsole = false;
		GUIInspector::renderInspector = false;
		GUIProjectHierarchy::renderProjectHierarchy = false;
	}
	
	void ConfigFileManager::ProcessFirstConfigValues()
	{
		string configFilePath = Engine::docsPath + "/config.txt";
		string debugMessagesCheck = "consoleDebugMessages";
		string fontScale = "fontScale";

		SetConfigValuesToDefaultValues();

		ifstream configFileStream(configFilePath);

		if (!configFileStream.is_open())
		{
			cout << "Error opening config file at " << configFilePath << endl;
			return;
		}

		string line;
		while (getline(configFileStream, line))
		{
			vector<string> splitLine = String::Split(line, ' ');
			string cleanedVariable = String::StringReplace(splitLine[0], ":", "");
			string cleanedValue = splitLine[1];
			if (cleanedVariable == debugMessagesCheck
				&& String::CanConvertStringToInt(cleanedValue)
				&& (stoi(cleanedValue) == 0
				|| stoi(cleanedValue) == 1))
			{
				ConsoleManager::sendDebugMessages = stoi(cleanedValue) == 1;
			}
			else if (cleanedVariable == fontScale
					 && String::CanConvertStringToFloat(cleanedValue) 
					 && stof(cleanedValue) >= 1.0f
					 && stof(cleanedValue) <= 2.0f)
			{
				EngineGUI::fontScale = stof(cleanedValue);
			}
		}

		configFileStream.close();
	}

	void ConfigFileManager::ProcessConfigFile(const string& fileName)
	{
		ifstream configFile(Engine::docsPath + "/config.txt");
		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open config.txt!\n\n");
			return;
		}
		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			"Reading from config.txt...\n");

		string line;
		while (getline(configFile, line))
		{
			line.erase(remove(line.begin(), line.end(), ' '), line.end());
			vector<string> lineSplit = String::Split(line, ':');
			vector<string> lineVariables;

			string name = lineSplit[0];
			string variables = lineSplit[1];
			if (variables.find(',') != string::npos)
			{
				variables = lineSplit[1];
				lineVariables = String::Split(variables, ',');
			}
			else lineVariables.push_back(lineSplit[1]);

			if (name == "fontScale")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					EngineGUI::fontScale = stof(lineVariables[0]);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set font scale to " + to_string(EngineGUI::fontScale) + ".\n");
				}
				else
				{
					EngineGUI::fontScale = 1.5f;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Font scale value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue fontScale(
					"fontScale",
					to_string(EngineGUI::fontScale),
					"1.0",
					"2.0",
					ConfigFileValue::Type::type_float);
				AddValue(fontScale);
			}
			else if (name == "resolution")
			{
				if (ConfigFileManager::IsValueInRange("width", lineVariables[0])
					&& ConfigFileManager::IsValueInRange("height", lineVariables[1]))
				{
					unsigned int width = stoul(lineVariables[0]);
					Render::SCR_WIDTH = width;
					unsigned int height = stoul(lineVariables[1]);
					Render::SCR_HEIGHT = height;
					glfwSetWindowSize(Render::window, width, height);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set resolution to " +
						to_string(Render::SCR_WIDTH) + ", " +
						to_string(Render::SCR_HEIGHT) + ".\n");
				}
				else
				{
					glfwSetWindowSize(Render::window, 1280, 720);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Height or width value " + lineVariables[0] + " for resolution is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue resolution(
					"resolution",
					to_string(Render::SCR_WIDTH) + ", " + to_string(Render::SCR_HEIGHT),
					"1280, 720",
					"7860, 3840",
					ConfigFileValue::Type::type_vec2);
				AddValue(resolution);
			}
			else if (name == "vsync")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					Render::useMonitorRefreshRate = static_cast<bool>(stoi(lineVariables[0]));
					glfwSwapInterval(Render::useMonitorRefreshRate ? 1 : 0);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set vsync to " + to_string(Render::useMonitorRefreshRate) + ".\n");
				}
				else
				{
					Render::useMonitorRefreshRate = true;
					glfwSwapInterval(1);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"VSync value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue vsync(
					"vsync",
					to_string(Render::useMonitorRefreshRate),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(vsync);
			}
			else if (name == "fov")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					Input::fov = stof(lineVariables[0]);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set fov to " + to_string(Input::fov) + ".\n");
				}
				else
				{
					Input::fov = 90;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"FOV value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue fov(
					"fov",
					to_string(Input::fov),
					"70",
					"110",
					ConfigFileValue::Type::type_float);
				AddValue(fov);
			}
			else if (name == "camNearClip")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					Input::nearClip = stof(lineVariables[0]);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set camera near clip to " + to_string(Input::nearClip) + ".\n");
				}
				else
				{
					Input::nearClip = 0.001f;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Camera near clip value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue camNearClip(
					"camNearClip",
					to_string(Input::nearClip),
					"0.001",
					"10000.0",
					ConfigFileValue::Type::type_float);
				AddValue(camNearClip);
			}
			else if (name == "camFarClip")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					Input::farClip = stof(lineVariables[0]);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set camera far clip to " + to_string(Input::farClip) + ".\n");
				}
				else
				{
					Input::farClip = 100.0f;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Camera far clip value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue camFarClip(
					"camFarClip",
					to_string(Input::farClip),
					"0.001",
					"10000.0",
					ConfigFileValue::Type::type_float);
				AddValue(camFarClip);
			}
			else if (name == "objectSensitivity")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					Input::objectSensitivity = stof(lineVariables[0]);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set object sensitivity to " + to_string(Input::objectSensitivity) + ".\n");
				}
				else
				{
					Input::objectSensitivity = 0.1f;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Object sensitivity value " + lineVariables[0] + " is out of range or not a float! Resetting to default.\n");
				}

				ConfigFileValue objectSensitivity(
					"objectSensitivity",
					to_string(Input::objectSensitivity),
					"0.0",
					"5.0",
					ConfigFileValue::Type::type_float);
				AddValue(objectSensitivity);
			}
			else if (name == "camPos")
			{
				if (ConfigFileManager::IsValueInRange(name + "X", lineVariables[0])
					&& ConfigFileManager::IsValueInRange(name + "Y", lineVariables[1])
					&& ConfigFileManager::IsValueInRange(name + "Z", lineVariables[2]))
				{
					vec3 newPosition = vec3(
						stof(lineVariables[0]),
						stof(lineVariables[1]),
						stof(lineVariables[2]));
					Render::camera.SetCameraPosition(newPosition);

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set camera position to to " +
						to_string(Render::camera.GetCameraPosition().x) + ", " +
						to_string(Render::camera.GetCameraPosition().y) + ", " +
						to_string(Render::camera.GetCameraPosition().z) + ".\n");
				}
				else
				{
					Render::camera.SetCameraPosition(vec3(0));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"X, Y or Z position for value " + lineVariables[0] + " camera is out of range or not a float! Resetting to default.\n");
				}

				string camPosValue = 
					to_string(Render::camera.GetCameraPosition().x) + ", " +
					to_string(Render::camera.GetCameraPosition().y) + ", " +
					to_string(Render::camera.GetCameraPosition().z);
				ConfigFileValue camPos(
					"camPos",
					camPosValue,
					"-1000000.0, -1000000.0, -1000000.0",
					"1000000.0, 1000000.0, 1000000.0",
					ConfigFileValue::Type::type_vec3);
				AddValue(camPos);
			}
			else if (name == "camRot")
			{
				if (ConfigFileManager::IsValueInRange(name + "X", lineVariables[0])
					&& ConfigFileManager::IsValueInRange(name + "Y", lineVariables[1])
					&& ConfigFileManager::IsValueInRange(name + "Z", lineVariables[2]))
				{
					Render::camera.SetCameraRotation(vec3(
						stof(lineVariables[0]),
						stof(lineVariables[1]),
						stof(lineVariables[2])));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set camera rotation to to " +
						to_string(Render::camera.GetCameraRotation().x) + ", " +
						to_string(Render::camera.GetCameraRotation().y) + ", " +
						to_string(Render::camera.GetCameraRotation().z) + ".\n");
				}
				else
				{
					Render::camera.SetCameraRotation(vec3(0));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"X, Y or Z rotation value " + lineVariables[0] + " for camera is out of range or not a float! Resetting to default.\n");
				}

				string camRotValue =
					to_string(Render::camera.GetCameraRotation().x) + ", " +
					to_string(Render::camera.GetCameraRotation().y) + ", " +
					to_string(Render::camera.GetCameraRotation().z);
				ConfigFileValue camRot(
					"camRot",
					camRotValue,
					"-359.99, -359.99, -359.99",
					"359.99, 359.99, 359.99",
					ConfigFileValue::Type::type_vec3);
				AddValue(camRot);
			}
			else if (name == "consoleForceScroll")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					GUIConsole::allowScrollToBottom = static_cast<bool>(stoi(lineVariables[0]));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set console force scroll to " + to_string(GUIConsole::allowScrollToBottom) + ".\n");
				}
				else
				{
					GUIConsole::allowScrollToBottom = true;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Console force scroll value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue consoleForceScroll(
					"consoleForceScroll",
					to_string(GUIConsole::allowScrollToBottom),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(consoleForceScroll);
			}
			else if (name == "showDebugMenu")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					GUIDebugMenu::renderDebugMenu = static_cast<bool>(stoi(lineVariables[0]));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set show debug menu to " + to_string(GUIDebugMenu::renderDebugMenu) + ".\n");
				}
				else
				{
					GUIDebugMenu::renderDebugMenu = true;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Show debug menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue showDebugMenu(
					"showDebugMenu",
					to_string(GUIDebugMenu::renderDebugMenu),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(showDebugMenu);
			}
			else if (name == "showConsole")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					GUIConsole::renderConsole = static_cast<bool>(stoi(lineVariables[0]));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set show console to " + to_string(GUIConsole::renderConsole) + ".\n");
				}
				else
				{
					GUIConsole::renderConsole = true;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Show console value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue showConsole(
					"showConsole",
					to_string(GUIConsole::renderConsole),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(showConsole);
			}
			else if (name == "showSceneMenu")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					GUIInspector::renderInspector = static_cast<bool>(stoi(lineVariables[0]));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set show scene menu to " + to_string(GUIInspector::renderInspector) + ".\n");
				}
				else
				{
					GUIInspector::renderInspector = true;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Show scene menu value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue showSceneMenu(
					"showSceneMenu",
					to_string(GUIInspector::renderInspector),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(showSceneMenu);
			}
			else if (name == "showProjectHierarchyWindow")
			{
				if (ConfigFileManager::IsValueInRange(name, lineVariables[0]))
				{
					GUIProjectHierarchy::renderProjectHierarchy = static_cast<bool>(stoi(lineVariables[0]));

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::DEBUG,
						"Set show project hierarchy window to " + to_string(GUIProjectHierarchy::renderProjectHierarchy) + ".\n");
				}
				else
				{
					GUIProjectHierarchy::renderProjectHierarchy = true;

					ConsoleManager::WriteConsoleMessage(
						Caller::ENGINE,
						Type::EXCEPTION,
						"Show project hierarchy window value " + lineVariables[0] + " is out of range or not an int! Resetting to default.\n");
				}

				ConfigFileValue showProjectHierarchyWindow(
					"showProjectHierarchyWindow",
					to_string(GUIProjectHierarchy::renderProjectHierarchy),
					"0",
					"1",
					ConfigFileValue::Type::type_int);
				AddValue(showProjectHierarchyWindow);
			}
		}

		configFile.close();
	}

	void ConfigFileManager::SaveDataAtShutdown()
	{
		if (exists(Engine::docsPath + "/config.txt"))
		{
			if (!remove(Engine::docsPath + "/config.txt"))
			{
				ConsoleManager::WriteConsoleMessage(
					Caller::ENGINE,
					Type::EXCEPTION,
					"Couldn't delete config.txt!\n\n");
				return;
			}

			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::DEBUG,
				"Deleted file: config.txt\n");
		}

		//open the file for writing
		ofstream configFile(Engine::docsPath + "/config.txt");

		if (!configFile.is_open())
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::ENGINE,
				Type::EXCEPTION,
				"Couldn't open new config.txt!\n\n");
			return;
		}

		for (const auto& variable : ConfigFileManager::GetValuesVector())
		{
			configFile << variable.GetName() << ": " << variable.GetValue() << "\n";
		}

		configFile.close();

		ConsoleManager::WriteConsoleMessage(
			Caller::ENGINE,
			Type::DEBUG,
			"Sucessfully saved data to config.txt!\n");
	}

	string ConfigFileManager::GetValue(const string& name)
	{
		for (const auto& configFileValue : values)
		{
			if (configFileValue.GetName() == name)
			{
				return configFileValue.GetValue();
			}
		}
		return "";
	}

	void ConfigFileManager::SetValue(
		const string& name, 
		const string& newValue)
	{
		for (auto& configFileValue : values)
		{
			if (configFileValue.GetName() == name)
			{
				ConfigFileValue::Type type = configFileValue.GetType();

				if (type == ConfigFileValue::Type::type_string
					|| (type == ConfigFileValue::Type::type_float)
					&& String::CanConvertStringToFloat(newValue)
					|| (type == ConfigFileValue::Type::type_int)
					&& String::CanConvertStringToFloat(newValue))
				{
					if (ConfigFileManager::IsValueInRange(name, newValue))
					{
						configFileValue.SetValue(newValue);
					}
					else cout << "this idiot cant set config file values in range\n";
					break;
				}
				else
				{
					cout << "this idiot cant convert config file values\n";
					break;
				}
			}
		}
	}

	bool ConfigFileManager::IsValueInRange(
		const string& name,
		const string& value)
	{
		ConfigFileValue::Type type{};

		float currentFloatValue{}, minFloatValue{}, maxFloatValue{};
		int currentIntValue{}, minIntValue{}, maxIntValue{};
		vec2 currentVec2Value{}, minVec2Value{}, maxVec2Value{};
		vec3 currentVec3Value{}, minVec3Value{}, maxVec3Value{};

		for (auto& configFileValue : values)
		{
			if (configFileValue.GetName() == name)
			{
				type = configFileValue.GetType();

				if (type == ConfigFileValue::Type::type_float)
				{
					currentFloatValue = stof(configFileValue.GetValue());
					minFloatValue = stof(configFileValue.GetMinValue());
					maxFloatValue = stof(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_int)
				{
					currentIntValue = stoi(configFileValue.GetValue());
					minIntValue = stoi(configFileValue.GetMinValue());
					maxIntValue = stoi(configFileValue.GetMaxValue());
				}
				else if (type == ConfigFileValue::Type::type_vec2)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec2Value = vec2(stof(currentSplitValue[0]), stof(currentSplitValue[1]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec2Value = vec2(stof(minSplitValue[0]), stof(minSplitValue[1]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec2Value = vec2(stof(maxSplitValue[0]), stof(maxSplitValue[1]));
				}
				else if (type == ConfigFileValue::Type::type_vec3)
				{
					vector<string> currentSplitValue = String::Split(configFileValue.GetValue(), ',');
					currentVec3Value = vec3(stof(currentSplitValue[0]), stof(currentSplitValue[1]), stof(currentSplitValue[2]));

					vector<string> minSplitValue = String::Split(configFileValue.GetMinValue(), ',');
					minVec3Value = vec3(stof(minSplitValue[0]), stof(minSplitValue[1]), stof(minSplitValue[2]));

					vector<string> maxSplitValue = String::Split(configFileValue.GetMaxValue(), ',');
					maxVec3Value = vec3(stof(maxSplitValue[0]), stof(maxSplitValue[1]), stof(maxSplitValue[2]));
				}

				break;
			}
		}

		bool isCorrectType = false;
		vec2 vec2Value{};
		vec3 vec3Value{};
		switch (type)
		{
		case ConfigFileValue::Type::type_string:
			return true;
		case ConfigFileValue::Type::type_float:
			isCorrectType = String::CanConvertStringToFloat(value);
			break;
		case ConfigFileValue::Type::type_int:
			isCorrectType = String::CanConvertStringToInt(value);
			break;
		case ConfigFileValue::Type::type_vec2:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec2 = String::Split(newValue, ',');
				vec2Value = vec2(stof(splitVec2[0]), stof(splitVec2[1]));
			}
			else return false;
			break;
		case ConfigFileValue::Type::type_vec3:
			if (String::ContainsString(value, ", "))
			{
				string newValue = String::StringReplace(value, ", ", ",");
				vector<string> splitVec3 = String::Split(newValue, ',');
				vec3Value = vec3(stof(splitVec3[0]), stof(splitVec3[1]), stof(splitVec3[2]));
			}
			break;
		}

		if (!isCorrectType) return false;

		float floatValue = stof(value);
		int intValue = stoi(value);

		if (type == ConfigFileValue::Type::type_float)
		{
			return floatValue >= minFloatValue && floatValue <= maxFloatValue;
		}
		else if (type == ConfigFileValue::Type::type_int)
		{
			return intValue >= minIntValue && intValue <= maxIntValue;
		}
		else if (type == ConfigFileValue::Type::type_vec2)
		{
			return vec2Value.x >= minVec2Value.x
				&& vec2Value.y >= minVec2Value.y
				&& vec2Value.x <= maxVec2Value.x
				&& vec2Value.y <= maxVec2Value.y;
		}
		else if (type == ConfigFileValue::Type::type_vec3)
		{
			return vec3Value.x >= minVec3Value.x
				&& vec3Value.y >= minVec3Value.y
				&& vec3Value.z >= minVec3Value.z
				&& vec3Value.x <= maxVec3Value.x
				&& vec3Value.y <= maxVec3Value.y
				&& vec3Value.z <= maxVec3Value.z;
		}

		return false;
	}
}