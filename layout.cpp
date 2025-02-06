#include "layout.h"

int graphics_index = -1;
int compute_index = -1;
int transfer_index = -1;
int sbinding_index = -1;
int protected_index = -1;
int vdecode_index = -1;
int vencode_index = -1;
int optflow_index = -1;
int present_index = -1;

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

		std::vector<vk::DescriptorSetLayout> desc_layouts;

		vk::RenderingAttachmentInfo colorAttachment{};
		vk::AttachmentReference colorAttachmentRef{};

		AllocatedImage drawImage{};

		namespace swapchain {
			vk::SwapchainKHR self{};
			std::vector<VkFramebuffer> framebuffers;
			std::vector<vk::ImageView> imageViews;
			vk::PresentModeKHR presentMode;
			uint32_t imageCount;
			std::vector<vk::Image, std::allocator<vk::Image>> images{};
		}

		namespace semaphores {
			vk::Semaphore wait;
			vk::Semaphore signal;
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

		namespace buffers {
			vk::Buffer vertex{};
		}

		namespace memory {
			VmaAllocator allocator{};
			AllocatedBuffer vertexBuffer{};
		}

		namespace meta {
			vk::MemoryRequirements memoryReq{};

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