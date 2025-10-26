#pragma once

// --- Linux Specific ---
#ifdef BONFIRE_PLATFORM_LINUX
#include <cstdint>
#endif
// ------------------------------

// --- Engine ------------------
#include "BonfireEnums.hpp"
#include "BonfireLog.hpp"
// ------------------------------

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
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include <atomic>

#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <cmath>
#include <cfloat>
// ------------------------------

// --- Additional Libraries -----
#include <json.hpp>
#include <json_fwd.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <Interface/ImGui/imgui_stdlib.h>

#include <ImGuizmo.h>
//#include <boost/asio.hpp>

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

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Body/MassProperties.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/RegisterTypes.h>
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
