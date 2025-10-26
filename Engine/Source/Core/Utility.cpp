#include "bonfire_pch.hpp"
#include "Utility.hpp"

#define GLM_ENABLE_EXPERIMENTAL

namespace Bonfire
{
	std::pair<ImVec4, std::string> ParseAnsiLine(const std::string& line)
	{
		ImVec4 color = ImVec4(1, 1, 1, 1);
    
		if (line.find("\x1b[31m") != std::string::npos)
			color = ImVec4(1, 0, 0, 1); // red (ERROR)
		else if (line.find("\x1b[33m") != std::string::npos)
			color = ImVec4(1, 1, 0, 1); // yellow (WARNING)
		else if (line.find("\x1b[37m") != std::string::npos)
			color = ImVec4(1, 1, 1, 1); // white (INFO)
    
		std::string clean_text = line;
		size_t pos = 0;
		while ((pos = clean_text.find("\x1b[", pos)) != std::string::npos)
		{
			size_t end = clean_text.find('m', pos);
			if (end != std::string::npos)
			{
				clean_text.erase(pos, end - pos + 1);
			}
			else
			{
				break;
			}
		}
    
		return {color, clean_text};
	}
	
	bool ContainsCharacter(const std::string& str, const char& chr)
	{
		for (unsigned int i = 0; i < str.length(); ++i)
			if (str[i] == chr) return true;
		return false;
	}

	void RemoveCharacter(std::string& str, const char& chr)
	{
		std::string clean_str;
		for (unsigned int i = 0; i < str.length(); ++i)
		{
			if (str[i] != chr) clean_str += str[i];
		}
		str = std::string(clean_str);
	}

	ImVec4 HexToImVec4(unsigned int hex_value, float alpha)
	{
		float r = ((hex_value >> 16) & 0xFF) / 255.0f;
		float g = ((hex_value >> 8) & 0xFF) / 255.0f;
		float b = ((hex_value) & 0xFF) / 255.0f;
		return ImVec4(r, g, b, alpha);
	}
	ImVec4 RgbaToImVec4(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
	{
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
	glm::vec4 HexToGlmVec4(unsigned int hex_value, float alpha)
	{
		float r = ((hex_value >> 16) & 0xFF) / 255.0f;
		float g = ((hex_value >> 8) & 0xFF) / 255.0f;
		float b = ((hex_value) & 0xFF) / 255.0f;
		return glm::vec4(r, g, b, alpha);
	}
	glm::vec4 RgbaToGlmVec4(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
	{
		return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
	glm::vec3 RgbToGlmVec3(unsigned int r, unsigned int g, unsigned int b)
	{
		return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
	}

	std::vector<std::filesystem::path> GetFilesInDirectory(const std::string& path)
	{
		std::vector<std::filesystem::path> files_in_directory;
		for (const auto& file : std::filesystem::directory_iterator(path))
			files_in_directory.push_back(file.path());
		return files_in_directory;
	}
	
	bool FloatEquals(float a, float b, float epsilon) { return std::abs(a - b) < epsilon; }
	bool DoubleEquals(double a, double b, double epsilon) { return std::abs(a - b) < epsilon; }

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		using T = float;

		glm::mat4 LocalMatrix(transform);

		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
			return false;

		if (
			glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}
}