//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "pointlight.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "billboard.hpp"
#include "console.hpp"
#include "core.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::translate;
using glm::quat;
using std::filesystem::exists;

using Core::Engine;
using Graphics::Shader;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using MeshType = Graphics::Components::MeshComponent::MeshType;
using Graphics::Render;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> PointLight::InitializePointLight(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		const vec3& diffuse,
		const float& intensity,
		const float& distance,
		string& name,
		unsigned int& id,
		const bool& isEnabled,
		const bool& isMeshEnabled,

		unsigned int& billboardID,
		const bool& isBillboardEnabled)
	{
		auto obj = make_shared<GameObject>(name, txtFilePath);
		obj->SetEnableState(isEnabled);
		obj->GetTransform()->SetPosition(pos);
		obj->GetTransform()->SetRotation(rot);
		obj->GetTransform()->SetScale(scale);

		float vertices[] =
		{
			//edges of the cube
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,

			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			//connecting edges
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
		};

		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		auto mesh = obj->AddComponent<MeshComponent>(
			isMeshEnabled, 
			MeshType::point_light, 
			vao, 
			vbo, 
			ebo);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for point light is invalid!");
		}

		Shader pointLightShader = Shader::LoadShader(vert, frag);

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->AddShader(vert, frag, pointLightShader);

		auto pointLight = obj->AddComponent<LightComponent>(diffuse, intensity, distance);

		string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "pointLight.png").string();
		auto billboard = Billboard::InitializeBillboard(
			pos,
			rot,
			scale,
			billboardDiffTexture,
			billboardID,
			isBillboardEnabled);

		billboard->SetParentBillboardHolder(obj);
		obj->SetChildBillboard(billboard);

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);
		GameObjectManager::AddPointLight(obj);

#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif
		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void PointLight::RenderPointLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		if (!obj) Engine::CreateErrorPopup("Point light gameobject is invalid.");

		if (obj->IsEnabled())
		{
			auto material = obj->GetComponent<MaterialComponent>();

			Shader shader = material->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			float transparency =
				Select::selectedObj == obj
				&& Select::isObjectSelected ? 1.0f : 0.5f;
			shader.SetFloat("transparency", transparency);

			auto light = obj->GetComponent<LightComponent>();
			shader.SetVec3("color", light->GetDiffuse());

			auto mesh = obj->GetComponent<MeshComponent>();
			if (GameObjectManager::renderLightBorders
				&& mesh
				&& mesh->IsEnabled())
			{
				mat4 model = mat4(1.0f);
				model = translate(model, obj->GetTransform()->GetPosition());
				quat newRot = quat(radians(obj->GetTransform()->GetRotation()));
				model *= mat4_cast(newRot);
				model = scale(model, obj->GetTransform()->GetScale());

				shader.SetMat4("model", model);
				GLuint VAO = mesh->GetVAO();
				glBindVertexArray(VAO);
				glDrawArrays(GL_LINES, 0, 24);
			}
		}
	}
}