#pragma once

// --- General ------------------
#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <fstream>

#include <filesystem>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <cmath>
// ------------------------------

// --- Additional Libraries -----
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <Interface/ImGui/imgui_stdlib.h>

#include <ImGuizmo.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// ------------------------------

// --- Windows Specific ---------
#ifdef BONFIRE_PLATFORM_WINDOWS
#include <Windows.h>
#include <WinBase.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shtypes.h>
#include <ShlObj_core.h>
#include <ShlObj.h>
#endif