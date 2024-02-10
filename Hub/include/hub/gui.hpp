//<Elypso hub>
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
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine 

#pragma once

#include <string>
#include <Windows.h>
#include <vector>

using std::string;
using std::vector;

class GUI
{
public:
	static inline int framebufferWidth, framebufferHeight;
	static inline float panelSpacing = 10.0f;
	static inline float panelHeight = 200.0f;

	static void Initialize();

	static void Render();
	static void RenderPanels(const vector<string>& files);
	static void RenderButtons();

	static void NewProject();

	static void AddProject();

	static string SetNewProjectFolderPath(HWND hwndOwner);

	static vector<string> GetFiles(const string& path);

	static void Shutdown();
};