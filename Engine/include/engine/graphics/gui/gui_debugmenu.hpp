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

#pragma once

namespace Graphics::GUI
{
	class GUIDebugMenu
	{
	public:
		static inline bool renderDebugMenu;
		static void RenderDebugMenu();

		static void RD_DebugMenuInfo();
		static void RD_Interactions();
	private:
		static inline const int bufferSize = 32;

		static inline char inputTextBuffer_camNearClip[bufferSize];
		static inline char inputTextBuffer_camFarClip[bufferSize];
		static inline char inputTextBuffer_camMoveSpeedMult[bufferSize];

		static inline float camNearClip;
		static inline float camFarClip;
		static inline float camMovespeed;
	};
}