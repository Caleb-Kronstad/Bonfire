#include "bonfire_pch.hpp"
#include "ShadowMap.hpp"

namespace Bonfire
{
	ShadowMap::ShadowMap(std::shared_ptr<Shader> point_shadow_map_shader, std::shared_ptr<Shader> shadow_map_shader, std::shared_ptr<Shader> lit_shader, const std::string& path)
	{
		this->point_shadow_map_shader = point_shadow_map_shader;
		this->shadow_map_shader = shadow_map_shader;
		this->lit_shader = lit_shader;
		map_texture = LoadTexture(path.c_str());

		glGenFramebuffers(1, &frame_buffer);

		glGenTextures(1, &shadow_cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_cubemap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_cubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(1, &directional_frame_buffer);
		glGenTextures(1, &directional_shadow_map);

		glBindTexture(GL_TEXTURE_2D, directional_shadow_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, directional_frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directional_shadow_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->lit_shader->Use();
		this->lit_shader->SetInt("shadow_map", 0);
		this->lit_shader->SetInt("point_shadow_map", 1);
	}

	ShadowMap::~ShadowMap()
	{
		if (frame_buffer != 0)
			glDeleteFramebuffers(1, &frame_buffer);
		if (directional_frame_buffer != 0)
			glDeleteFramebuffers(1, &directional_frame_buffer);
		if (shadow_cubemap != 0)
			glDeleteTextures(1, &shadow_cubemap);
		if (directional_shadow_map != 0)
			glDeleteTextures(1, &directional_shadow_map);
		if (map_texture != 0)
			glDeleteTextures(1, &map_texture);
	}

	void ShadowMap::Load(glm::vec3& light_pos)
	{
		aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		near_plane = 1.0f;
		far_plane = 25.0f;

		glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);

		shadow_transforms.clear();

		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	}

	void ShadowMap::LoadDirectional(glm::vec3& light_dir)
	{
		float near_plane = 1.0f, far_plane = 100.0f;
		glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::vec3 light_pos = -light_dir * 10.0f;
		glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		light_space_matrix = light_projection * light_view;
	}

	void ShadowMap::SetDirectional()
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, directional_frame_buffer);
		glClear(GL_DEPTH_BUFFER_BIT);
	}


	void ShadowMap::Set(glm::vec3& light_pos)
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glClear(GL_DEPTH_BUFFER_BIT);

		point_shadow_map_shader->Use();
		for (unsigned int i = 0; i < 6; ++i)
			point_shadow_map_shader->SetMat4("shadow_matrices[" + std::to_string(i) + "]", shadow_transforms[i]);
		point_shadow_map_shader->SetFloat("far_plane", far_plane);
		point_shadow_map_shader->SetVec3("light_pos", light_pos);
	}

	void ShadowMap::Draw()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directional_shadow_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_cubemap);
	}

	void ShadowMap::Reset(bool cull) // unbinds the frame buffer and switches the cull setting to back faces (if cull is set to true)
	{
		if (cull) glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint ShadowMap::LoadTexture(char const* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			Log::Error("Texture failed to load at path: " + std::string(path) + " [ShadowMap.cpp]");
			stbi_image_free(data);
		}

		return textureID;
	}
}