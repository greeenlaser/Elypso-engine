//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
namespace Graphics::GUI
{
	class GUIFirstTime
	{
	public:
		static void RenderFirstTime();
	private:
		static void RenderFirstTimeContent();
	};
}
#endif