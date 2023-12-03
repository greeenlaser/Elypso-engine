#pragma once

//external
#include "glad.h"

//engine
#include "console.h"

#include <string>

class ShaderManager
{
public:
	enum class ShaderState : char
	{
		vertex,
		fragment,
		link_shaders,
		vertex_data_and_buffers
	};

	unsigned int ID;

	static inline bool shaderSetupSuccess;

	static inline const char* vertexShader = "C:/Users/sande/Documents/CPP projects/Elypso engine/src/engine/shaders/vertex.shader";
	static inline const char* fragmentShader = "C:/Users/sande/Documents/CPP projects/Elypso engine/src/engine/shaders/fragment.shader";

	static inline unsigned int VAO, VBO, shaderProgram;

	static inline GLenum error;
	static inline GLuint framebuffer = 0;
	static inline GLuint textureColorbuffer = 0;

	static void ShaderSetup();

	ShaderManager(const char* vertexPath, const char* fragmetPath);

	void Use();

	void SetBool(const std::string& name, bool value) const;
	
	void SetInt(const std::string& name, int value) const;

	void SetFloat(const std::string& name, float value) const;

private:
	void CheckCompileErrors(unsigned int shader, std::string type);
};