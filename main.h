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

uint32_t imgCt;

vk::Device device;
vk::Extent2D ext;
vk::RenderPass renderPass;
std::vector<VkFramebuffer> swapChainFramebuffers;
vk::Pipeline graphicsPipeline;
vk::Viewport viewport{};
vk::Rect2D scissor{};
vk::PhysicalDevice cur_pd;
vk::Semaphore imageAvailableSemaphore;
vk::Semaphore renderFinishedSemaphore;
vk::Fence inFlightFence;
vk::SwapchainKHR sc{};
vk::CommandBuffer commandBuffer;
vk::CommandPool commandPool;
vk::SurfaceCapabilitiesKHR cap;
vk::SurfaceFormatKHR fmt;
vk::SurfaceKHR surface;
vk::PresentModeKHR pmd;
vk::Instance instance;
vk::PipelineLayout layout;


std::vector<vk::ImageView> sciv;

vk::PipelineShaderStageCreateInfo shaderStages[2];

vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
vk::PipelineViewportStateCreateInfo viewportState{};
vk::PipelineRasterizationStateCreateInfo rasterizer{};
vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
vk::PipelineColorBlendStateCreateInfo colorBlending{};

SDL_Window* window;

vk::Queue gq;
vk::Queue pq;

int graphics_index = -1;
int compute_index = -1;
int transfer_index = -1;
int sbinding_index = -1;
int protected_index = -1;
int vdecode_index = -1;
int vencode_index = -1;
int optflow_index = -1;
int present_index = -1;