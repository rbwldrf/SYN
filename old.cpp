#include "renderer.h"

extern struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static vk::VertexInputBindingDescription getBindingDesc() {
		vk::VertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.stride = sizeof(Vertex);
		bindingDesc.inputRate = vk::VertexInputRate::eVertex;
		std::cout << "Binding description created." << std::endl;

		return bindingDesc;
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		std::cout << "Attribute description created." << std::endl;

		return attributeDescriptions;
	}

};
const std::vector<Vertex> vertices = std::vector<Vertex>({
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	});

struct GPUMeshBuffers {

	AllocatedBuffer indexBuffer;
	AllocatedBuffer vertexBuffer;
	vk::DeviceAddress vertexBufferAddress;
};

vk::Extent2D getSwapExtent(const vk::SurfaceCapabilitiesKHR& cap) {
	/*if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		std::cout << "Window size in pixels: (" << cap.currentExtent.width << "x" << cap.currentExtent.height << ")" << std::endl;
		return cap.currentExtent;
	} else {*/
	int w, h;
	SDL_GetWindowSizeInPixels(syn::window::self, &w, &h);

	std::cout << "Window size in pixels: (" << w << "x" << h << ")" << std::endl;

	vk::Extent2D actualExtent = {
		static_cast<uint32_t>(w),
		static_cast<uint32_t>(h)
	};

	actualExtent.width = std::clamp(actualExtent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

	return actualExtent;
	//}
}

uint32_t findMemoryType(uint32_t typeFilter, vk::Flags<vk::MemoryPropertyFlagBits> properties) {
	vk::PhysicalDeviceMemoryProperties memProperties;
	syn::ren::physicalDevice.getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			std::cout << "Memory type found" << std::endl;
			return i;
		}
	}

	std::cout << "Memory type NOT found" << std::endl;
}

//INITIALIZATION AND CREATION

void updateViewportAndScissor() {
	syn::ren::physicalDevice.getSurfaceCapabilitiesKHR(syn::ren::surface::self, &syn::ren::surface::capabilities);
	syn::window::extent = getSwapExtent(syn::ren::surface::capabilities);

	syn::window::viewport.x = 0.0f;
	syn::window::viewport.y = 0.0f;
	syn::window::viewport.width = (float)syn::window::extent.width;
	syn::window::viewport.height = (float)syn::window::extent.height;
	syn::window::viewport.minDepth = 0.0f;
	syn::window::viewport.maxDepth = 1.0f;

	syn::window::scissor.offset = vk::Offset2D(0, 0);
	syn::window::scissor.extent = syn::window::extent;
}

bool vulkanSDLInit() {

	// Create an SDL window that supports Vulkan rendering.
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL." << std::endl;
		return false;
	}

	syn::window::self = SDL_CreateWindow("SYN Engine | NOT REAL GAMES", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if (syn::window::self == NULL) {
		std::cout << "Could not create SDL window." << std::endl;
		return false;
	}

	// Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
	unsigned extension_count;
	const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

	// Use validation layers if this is a debug build
	std::vector<const char*> layers;
#if defined(_DEBUG)
	layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// vk::ApplicationInfo allows the programmer to specifiy some basic information about the
	// program, which can be useful for layers and tools to provide more debug information.
	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
		.setPApplicationName("SYN")
		.setApplicationVersion(1)
		.setPEngineName("SYN")
		.setEngineVersion(1)
		.setApiVersion(VK_API_VERSION_1_3);

	// vk::InstanceCreateInfo is where the programmer specifies the layers and/or extensions that
	// are needed.
	vk::InstanceCreateInfo instInfo = vk::InstanceCreateInfo()
		.setFlags(vk::InstanceCreateFlags())
		.setPApplicationInfo(&appInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(extension_count))
		.setPpEnabledExtensionNames(extensions)
		.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
		.setPpEnabledLayerNames(layers.data());

	// Create the Vulkan instance.
	try {
		syn::instance = vk::createInstance(instInfo);
	}
	catch (const std::exception& e) {
		std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
		return false;
	}

	// Create a Vulkan surface for rendering
	VkSurfaceKHR c_surface;
	if (!SDL_Vulkan_CreateSurface(syn::window::self, static_cast<VkInstance>(syn::instance), nullptr, &c_surface)) {
		std::cout << "Could not create a Vulkan surface." << std::endl;
		return false;
	}
	syn::ren::surface::self = (c_surface);

	return true;
}

void createImageViews() {
	std::vector<vk::Image> sci = syn::device.getSwapchainImagesKHR(syn::ren::swapchain::self);

	syn::ren::swapchain::imageViews.resize(sci.size());

	for (int i = 0; i < sci.size(); ++i) {
		vk::ImageViewCreateInfo ivci;
		ivci.image = sci[i];
		ivci.viewType = vk::ImageViewType::e2D;
		ivci.format = syn::ren::surface::format.format;
		ivci.components.r = vk::ComponentSwizzle::eIdentity;
		ivci.components.g = vk::ComponentSwizzle::eIdentity;
		ivci.components.b = vk::ComponentSwizzle::eIdentity;
		ivci.components.a = vk::ComponentSwizzle::eIdentity;
		ivci.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;

		syn::device.createImageView(&ivci, nullptr, &syn::ren::swapchain::imageViews[i]);
	}
}

void createSwapChain() {

	syn::ren::swapchain::imageCount = syn::ren::surface::capabilities.minImageCount + 1;

	if (syn::ren::surface::capabilities.maxImageCount > 0 && syn::ren::swapchain::imageCount > syn::ren::surface::capabilities.maxImageCount) {
		syn::ren::swapchain::imageCount = syn::ren::surface::capabilities.maxImageCount;
	}

	updateViewportAndScissor();

	// SWAPCHAIN CREATION
	vk::SwapchainCreateInfoKHR scci{};
	scci.surface = syn::ren::surface::self;
	scci.minImageCount = syn::ren::swapchain::imageCount;
	scci.imageFormat = syn::ren::surface::format.format;
	scci.imageColorSpace = syn::ren::surface::format.colorSpace;
	scci.imageExtent = syn::window::extent;
	scci.imageArrayLayers = 1;
	scci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

	uint32_t qfi[] = { graphics_index,present_index };

	std::cout << "Image sharing mode: " << (graphics_index != present_index ? "Concurrent" : "Exclusive") << std::endl;

	if (graphics_index != present_index) {
		scci.imageSharingMode = vk::SharingMode::eConcurrent;
		scci.queueFamilyIndexCount = 2;
		scci.pQueueFamilyIndices = qfi;
	}
	else {
		scci.imageSharingMode = vk::SharingMode::eExclusive;
	}

	scci.preTransform = syn::ren::surface::capabilities.currentTransform;
	scci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	scci.presentMode = syn::ren::swapchain::presentMode;
	scci.clipped = vk::True;
	scci.oldSwapchain = nullptr;

	syn::device.createSwapchainKHR(&scci, nullptr, &syn::ren::swapchain::self);
}

void createPipelineLayout() {

	auto bind = Vertex::getBindingDesc();
	auto attr = Vertex::getAttributeDescriptions();

	vk::VertexInputBindingDescription* bla = &bind;

	syn::ren::meta::vertexInputInfo.vertexBindingDescriptionCount = 1;
	syn::ren::meta::vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attr.size());
	syn::ren::meta::vertexInputInfo.pVertexBindingDescriptions = bla;
	syn::ren::meta::vertexInputInfo.pVertexAttributeDescriptions = attr.data();

	syn::ren::meta::inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	syn::ren::meta::inputAssembly.primitiveRestartEnable = false;

	syn::ren::meta::viewportState.viewportCount = 1;
	syn::ren::meta::viewportState.pViewports = &syn::window::viewport;
	syn::ren::meta::viewportState.scissorCount = 1;
	syn::ren::meta::viewportState.pScissors = &syn::window::scissor;

	syn::ren::meta::rasterizer.depthClampEnable = false;
	syn::ren::meta::rasterizer.rasterizerDiscardEnable = false;
	syn::ren::meta::rasterizer.polygonMode = vk::PolygonMode::eFill;
	syn::ren::meta::rasterizer.lineWidth = 1.0f;
	syn::ren::meta::rasterizer.depthBiasEnable = false;
	syn::ren::meta::rasterizer.depthBiasConstantFactor = 0.0f;
	syn::ren::meta::rasterizer.depthBiasClamp = 0.0f;
	syn::ren::meta::rasterizer.depthBiasSlopeFactor = 0.0f;

	syn::ren::meta::colorBlendAttachment.colorWriteMask =
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;
	syn::ren::meta::colorBlendAttachment.blendEnable = true;
	syn::ren::meta::colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	syn::ren::meta::colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	syn::ren::meta::colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	syn::ren::meta::colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	syn::ren::meta::colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	syn::ren::meta::colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

	syn::ren::meta::colorBlending.logicOpEnable = false;
	syn::ren::meta::colorBlending.logicOp = vk::LogicOp::eCopy;
	syn::ren::meta::colorBlending.attachmentCount = 1;
	syn::ren::meta::colorBlending.pAttachments = &syn::ren::meta::colorBlendAttachment;

	vk::PipelineLayoutCreateInfo layoutInfo{};

	syn::device.createPipelineLayout(&layoutInfo, nullptr, &syn::ren::pipeline::layout);

	std::cout << "Pipeline layout successfully created." << std::endl;
}

void createRenderPass() {
	vk::AttachmentDescription colorAttachment{};
	colorAttachment.format = syn::ren::surface::format.format;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass{};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	vk::SubpassDependency dependency{};
	dependency.srcSubpass = vk::SubpassExternal;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask = vk::AccessFlagBits::eNone;
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;


	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	syn::device.createRenderPass(&renderPassInfo, nullptr, &syn::ren::renderPass);

	std::cout << "Render pass successfully created." << std::endl;
}

void createRenderPipeline() {
	vk::PipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sampleShadingEnable = false;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

	std::vector<vk::DynamicState> dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.dynamicStateCount = dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.data();


	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = syn::ren::meta::shaderStages;
	pipelineInfo.pVertexInputState = &syn::ren::meta::vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &syn::ren::meta::inputAssembly;
	pipelineInfo.pViewportState = &syn::ren::meta::viewportState;
	pipelineInfo.pRasterizationState = &syn::ren::meta::rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &syn::ren::meta::colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = syn::ren::pipeline::layout;
	pipelineInfo.renderPass = syn::ren::renderPass;
	pipelineInfo.subpass = 0;


	vk::PipelineCache pipelineCache;

	syn::device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &syn::ren::pipeline::self);

	std::cout << "Pipeline successfully created." << std::endl;
}

void createFramebuffers() {
	syn::ren::swapchain::framebuffers.resize(syn::ren::swapchain::imageViews.size());

	for (size_t i = 0; i < syn::ren::swapchain::imageViews.size(); i++) {
		vk::ImageView attachments[] = { syn::ren::swapchain::imageViews[i] };

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.renderPass = syn::ren::renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = syn::window::extent.width;
		framebufferInfo.height = syn::window::extent.height;
		framebufferInfo.layers = 1;

		syn::ren::swapchain::framebuffers[i] = syn::device.createFramebuffer(framebufferInfo);
		std::cout << "Framebuffer " << i + 1 << " successfully created." << std::endl;

	}
}

void createCommandPool() {
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = graphics_index;

	syn::device.createCommandPool(&poolInfo, nullptr, &syn::ren::commandPool);

	std::cout << "Command pool successfully created." << std::endl;
}

void createSemaphores() {
	vk::SemaphoreCreateInfo sem;
	syn::device.createSemaphore(&sem, nullptr, &syn::ren::semaphores::imageAvailable);
	syn::device.createSemaphore(&sem, nullptr, &syn::ren::semaphores::renderFinished);
}

void createFence() {
	vk::FenceCreateInfo fen;
	fen.flags = vk::FenceCreateFlagBits::eSignaled;
	syn::device.createFence(&fen, nullptr, &syn::ren::fences::inFlight);
}

void createAllocator() {

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = syn::ren::physicalDevice;
	allocatorInfo.device = syn::device;
	allocatorInfo.instance = syn::instance;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorInfo, &syn::ren::memory::allocator);

}

AllocatedBuffer createBuffer(size_t allocSize, vk::Flags<vk::BufferUsageFlagBits> usage, VmaMemoryUsage memUsage) {
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	VmaAllocationCreateInfo vmaallocInfo{};
	vmaallocInfo.usage = memUsage;
	vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuf;

	vmaCreateBuffer(syn::ren::memory::allocator, (VkBufferCreateInfo*)&bufferInfo, &vmaallocInfo, (VkBuffer*)&newBuf.buffer, &newBuf.allocation, &newBuf.info);

	return newBuf;
}

GPUMeshBuffers uploadMesh(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
{
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	GPUMeshBuffers newSurface;

	//create vertex buffer
	newSurface.vertexBuffer = createBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
		VMA_MEMORY_USAGE_GPU_ONLY);

	//find the adress of the vertex buffer
	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.vertexBuffer.buffer };
	newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(syn::device, &deviceAdressInfo);

	//create index buffer
	newSurface.indexBuffer = createBuffer(indexBufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, VMA_MEMORY_USAGE_GPU_ONLY);

	AllocatedBuffer staging = createBuffer(vertexBufferSize + indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);



	vk::CommandBufferBeginInfo beginInfo{};
	syn::ren::commandBuffer.begin(&beginInfo);

	vk::BufferCopy vertCopy{ 0 };
	vertCopy.dstOffset = 0;
	vertCopy.srcOffset = 0;
	vertCopy.size = vertexBufferSize;

	syn::ren::commandBuffer.copyBuffer(staging.buffer, newSurface.vertexBuffer.buffer, 1, &vertCopy);

	vk::BufferCopy indexCopy{ 0 };
	indexCopy.dstOffset = 0;
	indexCopy.srcOffset = vertexBufferSize;
	indexCopy.size = indexBufferSize;

	syn::ren::commandBuffer.copyBuffer(staging.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);

	syn::ren::commandBuffer.end();

	return newSurface;

}


void createVertexBuffer() {


	auto bla = uploadMesh({ 0,1,2 }, vertices);

}

void allocateCommandBuffers() {
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = syn::ren::commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	syn::device.allocateCommandBuffers(&allocInfo, &syn::ren::commandBuffer);

	std::cout << "Command buffers successfully allocated." << std::endl;
}

//CLEANUP

void cleanupSwapChain() {
	for (int i = 0; i < syn::ren::swapchain::framebuffers.size(); ++i) {
		syn::device.destroyFramebuffer(syn::ren::swapchain::framebuffers[i]);
	}

	for (int i = 0; i < syn::ren::swapchain::imageViews.size(); ++i) {
		syn::device.destroyImageView(syn::ren::swapchain::imageViews[i]);
	}

	syn::device.destroySwapchainKHR(syn::ren::swapchain::self);
}

//RECREATION

void recreateSwapChain() {
	syn::device.waitIdle();

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createFramebuffers();
}

// RENDERING

void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
	vk::CommandBufferBeginInfo beginInfo{};
	commandBuffer.begin(&beginInfo);

	vk::RenderPassBeginInfo renderpassInfo{};
	renderpassInfo.renderPass = syn::ren::renderPass;
	renderpassInfo.framebuffer = syn::ren::swapchain::framebuffers[imageIndex];
	renderpassInfo.renderArea.offset = vk::Offset2D(0, 0);
	renderpassInfo.renderArea.extent = syn::window::extent;

	vk::ClearValue clear{};
	clear.color = vk::ClearColorValue(0u, 0u, 0u, std::numeric_limits<uint32_t>::max());
	renderpassInfo.clearValueCount = 1;
	renderpassInfo.pClearValues = &clear;

	commandBuffer.beginRenderPass(&renderpassInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, syn::ren::pipeline::self);

	vk::Buffer vertBuffers[] = { syn::ren::buffers::vertex };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertBuffers, offsets);

	commandBuffer.setViewport(0, 1, &syn::window::viewport);
	commandBuffer.setScissor(0, 1, &syn::window::scissor);
	commandBuffer.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	commandBuffer.endRenderPass();
	commandBuffer.end();


}

void draw() {
	syn::device.waitForFences(1, &syn::ren::fences::inFlight, true, UINT64_MAX);

	uint32_t imageIndex;
	vk::Result res = syn::device.acquireNextImageKHR(syn::ren::swapchain::self, UINT64_MAX, syn::ren::semaphores::imageAvailable, nullptr, &imageIndex);

	if (res == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		return;
	}

	syn::device.resetFences(1, &syn::ren::fences::inFlight);

	syn::ren::commandBuffer.reset();
	recordCommandBuffer(syn::ren::commandBuffer, imageIndex);

	vk::SubmitInfo submitInfo{};
	vk::Semaphore waitSemaphores[] = { syn::ren::semaphores::imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &syn::ren::commandBuffer;

	vk::Semaphore signalSemaphores[] = { syn::ren::semaphores::renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	syn::ren::queues::graphics.submit(1, &submitInfo, syn::ren::fences::inFlight);

	vk::PresentInfoKHR presentInfo{};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapchains[] = { syn::ren::swapchain::self };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;

	presentInfo.pImageIndices = &imageIndex;

	syn::ren::queues::present.presentKHR(&presentInfo);

	if (res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
	}

}