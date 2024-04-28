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

namespace syn {

	namespace window {
		SDL_Window* self;
		vk::Extent2D extent;
		vk::Viewport viewport{};
		vk::Rect2D scissor{};
	}

	vk::Instance instance;
	vk::Device device;

	namespace ren {

		vk::PhysicalDevice physicalDevice;

		vk::CommandBuffer commandBuffer;
		vk::CommandPool commandPool;

		vk::RenderPass renderPass;

		namespace swapchain {
			vk::SwapchainKHR self{};
			std::vector<VkFramebuffer> framebuffers;
			std::vector<vk::ImageView> imageViews;
			vk::PresentModeKHR presentMode;
			uint32_t imageCount;
		}

		namespace semaphores {
			vk::Semaphore imageAvailable;
			vk::Semaphore renderFinished;
		}

		namespace fences {
			vk::Fence inFlight;
		}

		namespace pipeline {
			vk::Pipeline self;
			vk::PipelineLayout layout;
		}

		namespace queues {
			vk::Queue graphics;
			vk::Queue present;
		}

		namespace surface {
			vk::SurfaceKHR self{};
			vk::SurfaceFormatKHR format{};
			vk::SurfaceCapabilitiesKHR capabilities{};
		}

		namespace meta {
			vk::PipelineShaderStageCreateInfo shaderStages[2];
			vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
			vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
			vk::PipelineViewportStateCreateInfo viewportState{};
			vk::PipelineRasterizationStateCreateInfo rasterizer{};
			vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
			vk::PipelineColorBlendStateCreateInfo colorBlending{};
		}
	}


};


int graphics_index = -1;
int compute_index = -1;
int transfer_index = -1;
int sbinding_index = -1;
int protected_index = -1;
int vdecode_index = -1;
int vencode_index = -1;
int optflow_index = -1;
int present_index = -1;