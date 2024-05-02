#pragma once
#include "main.h"

vk::Extent2D getSwapExtent(const vk::SurfaceCapabilitiesKHR& cap);

void updateViewportAndScissor();

bool vulkanSDLInit();

void createImageViews();

void createSwapChain();

void createPipelineLayout();

void createRenderPass();

void createRenderPipeline();

void createFramebuffers();

void createCommandPool();

void createSemaphores();

void createFence();

void allocateCommandBuffers();

void cleanupSwapChain();

void createVertexBuffer();

void recreateSwapChain();

void createAllocator();

void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlagBits properties);

vk::ImageCreateInfo create_image_info(vk::Format fmt, vk::ImageUsageFlags usage, vk::Extent3D extent, vk::ImageLayout layout);

vk::ImageViewCreateInfo imageview_create_info(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags);

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout currentLayout, vk::ImageLayout newLayout);

void copy_image_to_image(vk::CommandBuffer cmd, vk::Image source, vk::Image destination, vk::Extent2D srcSize, vk::Extent2D dstSize);

void draw();