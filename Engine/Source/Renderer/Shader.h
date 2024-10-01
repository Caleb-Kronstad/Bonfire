#pragma once

namespace Bonfire
{
	class Shader
	{
	public:
		Shader(const std::string& shaderName = "NewShader", std::string vertexPath = "None", std::string fragmentPath = "None", std::string geometryPath = "None");

		void Load(const std::string& shaderName, std::string vertexPath = "None", std::string fragmentPath = "None", std::string geometryPath = "None");
		void Use();

		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value) const;

		void SetMat2(const std::string& name, glm::mat2 value) const;
		void SetMat3(const std::string& name, glm::mat3 value) const;
		void SetMat4(const std::string& name, glm::mat4 value) const;

		void SetVec2(const std::string& name, float x, float y) const;
		void SetVec2(const std::string& name, glm::vec2 value) const;
		void SetVec3(const std::string& name, float x, float y, float z) const;
		void SetVec3(const std::string& name, glm::vec3 value) const;
		void SetVec4(const std::string& name, float x, float y, float z, float w) const;
		void SetVec4(const std::string& name, glm::vec4 value) const;

		std::string& GetVertexPath() { return vPath; }
		std::string& GetFragmentPath() { return fPath; }
		std::string& GetGeometryPath() { return gPath; }
		GLuint GetShaderID() const { return m_ShaderID; }

	private:
		void CheckCompileErrors(GLuint shader, std::string type);

	private:
		GLuint m_ShaderID;

		std::string vPath = "None";
		std::string fPath = "None";
		std::string gPath = "None";
	};
}