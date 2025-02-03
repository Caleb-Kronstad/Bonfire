#include "bonfire_pch.h"
#include "Shader.h"

namespace Bonfire
{
	Shader::Shader(const std::string& shaderName,
		std::string vertexPath, std::string fragmentPath, std::string geometryPath)
	{
		Load(shaderName, vertexPath, fragmentPath, geometryPath);
	}

	void Shader::Load(const std::string& shaderName,
		std::string vertexPath, std::string fragmentPath, std::string geometryPath)
	{
		if (vertexPath != "None") vPath = std::string(vertexPath);
		if (fragmentPath != "None") fPath = std::string(fragmentPath);
		if (geometryPath != "None") gPath = std::string(geometryPath);

		if (vertexPath == "None" && fragmentPath == "None" && geometryPath == "None") return;

		std::string vertexSource;
		std::string fragmentSource;
		std::string geometrySource;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// Open files
			if (vertexPath != "None")
			{
				vShaderFile.open(vertexPath);
				std::stringstream vShaderStream;
				vShaderStream << vShaderFile.rdbuf();
				vShaderFile.close();
				vertexSource = vShaderStream.str();
			}

			if (fragmentPath != "None")
			{
				fShaderFile.open(fragmentPath);
				std::stringstream fShaderStream;
				fShaderStream << fShaderFile.rdbuf();
				fShaderFile.close();
				fragmentSource = fShaderStream.str();
			}

			if (geometryPath != "None")
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometrySource = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}

		// Compile shaders
		unsigned int vertex = 0;
		if (vertexPath != "None")
		{
			const char* vShaderSource = vertexSource.c_str();
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderSource, NULL);
			glCompileShader(vertex);
			CheckCompileErrors(vertex, "VERTEX"); // Check for shader compile errors
		}

		unsigned int fragment = 0;
		if (fragmentPath != "None")
		{
			const char* fShaderSource = fragmentSource.c_str();
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderSource, NULL);
			glCompileShader(fragment);
			CheckCompileErrors(fragment, "FRAGMENT"); // Check for shader compile errors
		}

		unsigned int geometry = 0;
		if (geometryPath != "None")
		{
			const char* gShaderSource = geometrySource.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderSource, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}

		// Shader program
		m_ShaderID = glCreateProgram();
		if (vertexPath != "None") glAttachShader(m_ShaderID, vertex);
		if (fragmentPath != "None") glAttachShader(m_ShaderID, fragment);
		if (geometryPath != "None") glAttachShader(m_ShaderID, geometry);
		glLinkProgram(m_ShaderID);
		CheckCompileErrors(m_ShaderID, "PROGRAM");

		// Delete shaders as they have now been linked
		if (vertex) glDeleteShader(vertex);
		if (fragment) glDeleteShader(fragment);
		if (geometry) glDeleteShader(geometry);
	}

	void Shader::Use()
	{
		glUseProgram(m_ShaderID);
	}

	void Shader::SetBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), (int)value);
	}
	void Shader::SetInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value);
	}
	void Shader::SetFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
	}
	void Shader::SetMat2(const std::string& name, glm::mat2 value) const
	{
		glUniformMatrix2fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetMat3(const std::string& name, glm::mat3 value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetMat4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y);
	}
	void Shader::SetVec2(const std::string& name, glm::vec2 value) const
	{
		glUniform2fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, glm::value_ptr(value));
	}
	void Shader::SetVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y, z);
	}
	void Shader::SetVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, glm::value_ptr(value));
	}
	void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(m_ShaderID, name.c_str()), x, y, z, w);
	}
	void Shader::SetVec4(const std::string& name, glm::vec4 value) const
	{
		glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, glm::value_ptr(value));
	}

	void Shader::CheckCompileErrors(GLuint shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				std::cout << vPath << "\n" << fPath << "\n" << gPath << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				std::cout << vPath << "\n" << fPath << "\n" << gPath << std::endl;
			}
		}
	}
}