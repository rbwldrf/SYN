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

void recreateSwapChain();

void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

void draw();