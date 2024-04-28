#pragma once

#include "main.h"

extern int graphics_index;
extern int compute_index;
extern int transfer_index;
extern int sbinding_index;
extern int protected_index;
extern int vdecode_index;
extern int vencode_index;
extern int optflow_index;
extern int present_index;

namespace syn {

	namespace window {
		extern SDL_Window* self;
		extern vk::Extent2D extent;
		extern vk::Viewport viewport;
		extern vk::Rect2D scissor;
	}

	extern vk::Instance instance;
	extern vk::Device device;

	namespace ren {

		extern vk::PhysicalDevice physicalDevice;

		extern vk::CommandBuffer commandBuffer;
		extern vk::CommandPool commandPool;

		extern vk::RenderPass renderPass;

		namespace swapchain {
			extern vk::SwapchainKHR self;
			extern std::vector<VkFramebuffer> framebuffers;
			extern std::vector<vk::ImageView> imageViews;
			extern vk::PresentModeKHR presentMode;
			extern uint32_t imageCount;
		}

		namespace semaphores {
			extern vk::Semaphore imageAvailable;
			extern vk::Semaphore renderFinished;
		}

		namespace fences {
			extern vk::Fence inFlight;
		}

		namespace pipeline {
			extern vk::Pipeline self;
			extern vk::PipelineLayout layout;
		}

		namespace queues {
			extern vk::Queue graphics;
			extern vk::Queue present;
		}

		namespace surface {
			extern vk::SurfaceKHR self;
			extern vk::SurfaceFormatKHR format;
			extern vk::SurfaceCapabilitiesKHR capabilities;
		}

		namespace meta {
			extern vk::PipelineShaderStageCreateInfo shaderStages[2];
			extern vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
			extern vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
			extern vk::PipelineViewportStateCreateInfo viewportState;
			extern vk::PipelineRasterizationStateCreateInfo rasterizer;
			extern vk::PipelineColorBlendAttachmentState colorBlendAttachment;
			extern vk::PipelineColorBlendStateCreateInfo colorBlending;
		}
	}


};