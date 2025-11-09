#pragma once

#include "bonfire_pch.hpp"
#include "Renderer/Model.hpp"

namespace Bonfire
{
    namespace Debug
    {
        static std::vector<glm::vec3> GetBoxVertices(const glm::vec3& half_extents)
        {
            return {
                // Bottom face
                {-half_extents.x, -half_extents.y, -half_extents.z}, {half_extents.x, -half_extents.y, -half_extents.z},
                {half_extents.x, -half_extents.y, -half_extents.z}, {half_extents.x, -half_extents.y, half_extents.z},
                {half_extents.x, -half_extents.y, half_extents.z}, {-half_extents.x, -half_extents.y, half_extents.z},
                {-half_extents.x, -half_extents.y, half_extents.z}, {-half_extents.x, -half_extents.y, -half_extents.z},
                // Top face
                {-half_extents.x, half_extents.y, -half_extents.z}, {half_extents.x, half_extents.y, -half_extents.z},
                {half_extents.x, half_extents.y, -half_extents.z}, {half_extents.x, half_extents.y, half_extents.z},
                {half_extents.x, half_extents.y, half_extents.z}, {-half_extents.x, half_extents.y, half_extents.z},
                {-half_extents.x, half_extents.y, half_extents.z}, {-half_extents.x, half_extents.y, -half_extents.z},
                // Vertical edges
                {-half_extents.x, -half_extents.y, -half_extents.z}, {-half_extents.x, half_extents.y, -half_extents.z},
                {half_extents.x, -half_extents.y, -half_extents.z}, {half_extents.x, half_extents.y, -half_extents.z},
                {half_extents.x, -half_extents.y, half_extents.z}, {half_extents.x, half_extents.y, half_extents.z},
                {-half_extents.x, -half_extents.y, half_extents.z}, {-half_extents.x, half_extents.y, half_extents.z}
            };
        }

        static std::vector<glm::vec3> GetSphereVertices(float radius, int segments = 16)
        {
            std::vector<glm::vec3> vertices;
            float angleStep = glm::two_pi<float>() / segments;

            // 3 circles: XY, XZ, YZ planes
            for (int i = 0; i < segments; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;

                // XY circle
                vertices.push_back({radius * cos(angle1), radius * sin(angle1), 0});
                vertices.push_back({radius * cos(angle2), radius * sin(angle2), 0});

                // XZ circle
                vertices.push_back({radius * cos(angle1), 0, radius * sin(angle1)});
                vertices.push_back({radius * cos(angle2), 0, radius * sin(angle2)});

                // YZ circle
                vertices.push_back({0, radius * cos(angle1), radius * sin(angle1)});
                vertices.push_back({0, radius * cos(angle2), radius * sin(angle2)});
            }
            return vertices;
        }

        static std::vector<glm::vec3> GetCapsuleVertices(float radius, float half_height, int segments = 16)
        {
            std::vector<glm::vec3> vertices;
            float angleStep = glm::two_pi<float>() / segments;

            // Cylinder body circles
            for (int i = 0; i < segments; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;

                // Top circle
                vertices.push_back({radius * cos(angle1), half_height, radius * sin(angle1)});
                vertices.push_back({radius * cos(angle2), half_height, radius * sin(angle2)});

                // Bottom circle
                vertices.push_back({radius * cos(angle1), -half_height, radius * sin(angle1)});
                vertices.push_back({radius * cos(angle2), -half_height, radius * sin(angle2)});

                // Vertical lines
                if (i % 4 == 0)
                {
                    vertices.push_back({radius * cos(angle1), -half_height, radius * sin(angle1)});
                    vertices.push_back({radius * cos(angle1), half_height, radius * sin(angle1)});
                }
            }

            // Hemisphere arcs (simplified)
            for (int i = 0; i < segments / 2; i++)
            {
                float angle1 = i * angleStep;
                float angle2 = (i + 1) * angleStep;

                // Top hemisphere arc
                vertices.push_back({radius * cos(angle1), half_height + radius * sin(angle1), 0});
                vertices.push_back({radius * cos(angle2), half_height + radius * sin(angle2), 0});

                // Bottom hemisphere arc
                vertices.push_back({radius * cos(angle1), -half_height - radius * sin(angle1), 0});
                vertices.push_back({radius * cos(angle2), -half_height - radius * sin(angle2), 0});
            }

            return vertices;
        }

        static std::vector<glm::vec3> GetMeshVertices(std::shared_ptr<Model> model)
        {
            std::vector<glm::vec3> vertices;

            if (!model)
                return vertices;

            for (const Mesh& mesh : model->meshes)
            {
                for (size_t i = 0; i < mesh.indices.size(); i += 3)
                {
                    if (i + 2 < mesh.indices.size())
                    {
                        GLuint idx0 = mesh.indices[i];
                        GLuint idx1 = mesh.indices[i + 1];
                        GLuint idx2 = mesh.indices[i + 2];
                        
                        if (idx0 < mesh.vertices.size() &&
                            idx1 < mesh.vertices.size() &&
                            idx2 < mesh.vertices.size())
                        {
                            glm::vec3 v0 = mesh.vertices[idx0].position;
                            glm::vec3 v1 = mesh.vertices[idx1].position;
                            glm::vec3 v2 = mesh.vertices[idx2].position;

                            vertices.push_back(v0); vertices.push_back(v1);  // Edge 0-1
                            vertices.push_back(v1); vertices.push_back(v2);  // Edge 1-2
                            vertices.push_back(v2); vertices.push_back(v0);  // Edge 2-0
                        }
                    }
                }
            }

            return vertices;
        }
    }
}