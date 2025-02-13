//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
//external
#include "glm.hpp"

//engine
#include "shader.hpp"

namespace Graphics
{
	using glm::vec3;
	using glm::mat4;

	using Graphics::Shader;

	class Grid
	{
	public:
		static void InitializeGrid();
		static void RenderGrid(const mat4& view, const mat4& projection);
	private:
		static inline GLuint VAO, VBO;
		static inline Shader shader;

		static inline float fadeDistance;
		static constexpr int lineCount = 10000; //BREAKS IF YOU PUT IT ABOVE 100000
		static inline float lineDistance = 1.0f;
		static inline float vertices[lineCount * 4 * 3];
	};
}
#endif