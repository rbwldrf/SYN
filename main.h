#pragma once


// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>


#include <iostream>
#include <vector>
#include <chrono>

#include <limits>

#include <set>

#include <Windows.h>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <string>

#include "magic_enum/magic_enum_all.hpp"

#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/spirv.hpp"

#include "fuck.h"

#define MAGIC_ENUM_SUPPORTED_ALIASES 1

namespace fs = std::filesystem;

#undef max
#undef min

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}


struct AllocatedBuffer {
	vk::Buffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;
};

struct AllocatedImage {
	vk::Image image;
	vk::ImageView imageView;
	VmaAllocation allocation;
	vk::Extent3D imageExtent;
	vk::Format imageFormat;
};

#include "layout.h"
#include "renderer.h"

