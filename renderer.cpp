#include "renderer.h"

struct pConst {
	vk::DeviceAddress vBuf;
};

extern struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;

	static std::vector<vk::VertexInputBindingDescription> getBindingDesc() {
		vk::VertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.stride = sizeof(Vertex);
		bindingDesc.inputRate = vk::VertexInputRate::eVertex;
		std::cout << "Binding description created." << std::endl;

		std::vector<vk::VertexInputBindingDescription> a = {};
		a.push_back(bindingDesc);

		return a;
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

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout currentLayout, vk::ImageLayout newLayout)
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
	imageBarrier.pNext = nullptr;

	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = (VkImageLayout)currentLayout;
	imageBarrier.newLayout = (VkImageLayout)newLayout;

	VkImageAspectFlags aspectMask = (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageSubresourceRange subImage{};
	subImage.aspectMask = aspectMask;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;


	imageBarrier.subresourceRange = subImage;
	imageBarrier.image = image;

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.pNext = nullptr;

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cmd, &depInfo);
}


std::vector<Vertex> vertices = std::vector<Vertex>({
	{{0.0f, -0.5f,0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f,0.0f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f,0.0f}, {0.0f, 0.0f, 1.0f}},
	{{0.0f, -0.5f,0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f,0.0f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f,0.0f}, {0.0f, 0.0f, 1.0f}}

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
	scci.imageUsage = vk::ImageUsageFlagBits::eTransferDst;

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

	vk::Extent3D drawImageExtent = {
	syn::window::extent.width,
	syn::window::extent.height,
	1 };

	syn::ren::drawImage.imageFormat = vk::Format::eR16G16B16A16Sfloat;
	syn::ren::drawImage.imageExtent = drawImageExtent;

	vk::ImageUsageFlags drawImageUsages{};
	drawImageUsages |= vk::ImageUsageFlagBits::eTransferSrc;
	drawImageUsages |= vk::ImageUsageFlagBits::eStorage;
	drawImageUsages |= vk::ImageUsageFlagBits::eColorAttachment;

	auto img_info = create_image_info(syn::ren::drawImage.imageFormat, drawImageUsages, drawImageExtent, vk::ImageLayout::eUndefined);

	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vmaCreateImage(syn::ren::memory::allocator, (VkImageCreateInfo*)&img_info, &rimg_allocinfo, (VkImage*)&syn::ren::drawImage.image, &syn::ren::drawImage.allocation, nullptr);


	auto img_view_info = imageview_create_info(syn::ren::drawImage.imageFormat, syn::ren::drawImage.image, vk::ImageAspectFlagBits::eColor);

	syn::device.createImageView(&img_view_info, nullptr, &syn::ren::drawImage.imageView);

	syn::ren::colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	syn::ren::colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	syn::ren::colorAttachment.imageView = syn::ren::drawImage.imageView;
	syn::ren::colorAttachment.imageLayout = vk::ImageLayout::eGeneral;

	syn::ren::swapchain::images = syn::device.getSwapchainImagesKHR(syn::ren::swapchain::self);

}

auto bd = Vertex::getBindingDesc();
auto attr = Vertex::getAttributeDescriptions();

void createPipelineLayout() {


	syn::ren::meta::vertexInputInfo.vertexBindingDescriptionCount = bd.size();
	syn::ren::meta::vertexInputInfo.vertexAttributeDescriptionCount = attr.size();
	syn::ren::meta::vertexInputInfo.pVertexBindingDescriptions = bd.data();
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
	syn::ren::meta::rasterizer.cullMode = vk::CullModeFlagBits::eBack;
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
	
	vk::PushConstantRange range;
	range.size = sizeof(pConst);
	range.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::DescriptorSetLayoutBinding dslBind;
	dslBind.binding = 0;
	dslBind.descriptorType = vk::DescriptorType::eUniformBuffer;
	dslBind.stageFlags = vk::ShaderStageFlagBits::eVertex;

	vk::DescriptorSetLayoutCreateInfo dslLayout;
	dslLayout.bindingCount = 1;
	dslLayout.pBindings = &dslBind;

	syn::ren::desc_layouts.resize(1);
	syn::device.createDescriptorSetLayout(&dslLayout, 0, syn::ren::desc_layouts.data());

	vk::PipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &range;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = syn::ren::desc_layouts.data();


	syn::device.createPipelineLayout(&layoutInfo, nullptr, &syn::ren::pipeline::layout);

	std::cout << "Pipeline layout successfully created." << std::endl;
}


vk::ImageCreateInfo create_image_info(vk::Format fmt, vk::ImageUsageFlags usage, vk::Extent3D extent, vk::ImageLayout layout) {
	VkImageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = nullptr;

	info.imageType = VK_IMAGE_TYPE_2D;

	info.format = (VkFormat)fmt;
	info.extent = extent;
	info.initialLayout = (VkImageLayout)layout;

	info.mipLevels = 1;
	info.arrayLayers = 1;

	//for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
	info.samples = VK_SAMPLE_COUNT_1_BIT;

	//optimal tiling, which means the image is stored on the best gpu format
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = (VkImageUsageFlags)usage;

	return info;
}

vk::ImageViewCreateInfo imageview_create_info(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags)
{
	// build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = (VkFormat)format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = (VkImageAspectFlags)aspectFlags;

	return info;
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
	pipelineInfo.subpass = 0;


	vk::PipelineCache pipelineCache;

	syn::device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &syn::ren::pipeline::self);

	std::cout << "Pipeline successfully created." << std::endl;
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
	syn::device.createSemaphore(&sem, nullptr, &syn::ren::semaphores::wait);
	syn::device.createSemaphore(&sem, nullptr, &syn::ren::semaphores::signal);
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
	newSurface.vertexBuffer = createBuffer(vertexBufferSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
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

GPUMeshBuffers bla;

void createVertexBuffer() {

	bla = uploadMesh({ 0,1,2, 3, 4, 5 }, vertices);

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
}

// RENDERING

void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(&beginInfo);

	/*vk::RenderPassBeginInfo renderpassInfo{};
	renderpassInfo.renderPass = syn::ren::renderPass;
	renderpassInfo.framebuffer = syn::ren::swapchain::framebuffers[imageIndex];
	renderpassInfo.renderArea.offset = vk::Offset2D(0, 0);
	renderpassInfo.renderArea.extent = syn::window::extent;

	vk::ClearValue clear{};
	clear.color = vk::ClearColorValue(0u, 0u, 0u, std::numeric_limits<uint32_t>::max());
	renderpassInfo.clearValueCount = 1;
	renderpassInfo.pClearValues = &clear;

	commandBuffer.beginRenderPass(&renderpassInfo, vk::SubpassContents::eInline);*/
	transition_image(commandBuffer, syn::ren::swapchain::images[imageIndex], vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	transition_image(commandBuffer, syn::ren::drawImage.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

	vk::RenderingInfo ren{};
	ren.colorAttachmentCount = 1;
	ren.pColorAttachments = &syn::ren::colorAttachment;
	ren.renderArea.extent = vk::Extent2D{ syn::ren::drawImage.imageExtent.width, syn::ren::drawImage.imageExtent.height };
	ren.layerCount = 1;

	const vk::ArrayProxy<const vk::DeviceSize> offsets{ 0 };

	pConst push;
	push.vBuf = bla.vertexBufferAddress;

	commandBuffer.bindIndexBuffer(bla.indexBuffer.buffer, 0, vk::IndexType::eUint32);
	commandBuffer.bindVertexBuffers(0, bla.vertexBuffer.buffer, offsets);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, syn::ren::pipeline::self);
	commandBuffer.beginRendering(&ren);

	commandBuffer.pushConstants(syn::ren::pipeline::layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(pConst), &push);

	commandBuffer.setViewport(0, 1, &syn::window::viewport);
	commandBuffer.setScissor(0, 1, &syn::window::scissor);
	commandBuffer.drawIndexed(6, 1, 0, 0, 0);
	commandBuffer.endRendering();

	transition_image(commandBuffer, syn::ren::swapchain::images[imageIndex], vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferDstOptimal);
	transition_image(commandBuffer, syn::ren::drawImage.image, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal);


	copy_image_to_image(commandBuffer, syn::ren::drawImage.image, syn::ren::swapchain::images[imageIndex], vk::Extent2D{ syn::ren::drawImage.imageExtent.width, syn::ren::drawImage.imageExtent.height }, syn::window::extent);

	transition_image(commandBuffer, syn::ren::drawImage.image, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral);

	transition_image(commandBuffer, syn::ren::swapchain::images[imageIndex], vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);


	commandBuffer.end();


}

void copy_image_to_image(vk::CommandBuffer cmd, vk::Image source, vk::Image destination, vk::Extent2D srcSize, vk::Extent2D dstSize)
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[1].x = srcSize.width;
	blitRegion.srcOffsets[1].y = srcSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.width;
	blitRegion.dstOffsets[1].y = dstSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blitInfo.dstImage = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(cmd, &blitInfo);
}

vk::SemaphoreSubmitInfo semaphore_submit_info(vk::PipelineStageFlags2 stageMask, vk::Semaphore semaphore)
{
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.semaphore = (VkSemaphore)semaphore;
	submitInfo.stageMask = (VkPipelineStageFlags2)stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

vk::CommandBufferSubmitInfo command_buffer_submit_info(vk::CommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

vk::SubmitInfo2 submit_info(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signalSemaphoreInfo,
	vk::SemaphoreSubmitInfo* waitSemaphoreInfo)
{
	VkSubmitInfo2 info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.pNext = nullptr;

	info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos = (VkSemaphoreSubmitInfo*)waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos = (VkSemaphoreSubmitInfo*)signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = (VkCommandBufferSubmitInfo*)cmd;

	return info;
}

uint32_t imageIndex;

void draw() {
	syn::device.waitForFences(1, &syn::ren::fences::inFlight, true, UINT64_MAX);

	vk::Result res = syn::device.acquireNextImageKHR(syn::ren::swapchain::self, UINT64_MAX, syn::ren::semaphores::wait, nullptr, &imageIndex);

	if (res == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		return;
	}

	syn::device.resetFences(1, &syn::ren::fences::inFlight);

	syn::ren::commandBuffer.reset();
	recordCommandBuffer(syn::ren::commandBuffer, imageIndex);

	vk::SemaphoreSubmitInfo wait = semaphore_submit_info(vk::PipelineStageFlagBits2::eColorAttachmentOutput, syn::ren::semaphores::wait);


	vk::SemaphoreSubmitInfo signal = semaphore_submit_info(vk::PipelineStageFlagBits2::eAllGraphics, syn::ren::semaphores::signal);

	auto cmd = command_buffer_submit_info(syn::ren::commandBuffer);

	vk::SubmitInfo2 submitInfo{};

	submitInfo.waitSemaphoreInfoCount = 1;
	submitInfo.pWaitSemaphoreInfos = &wait;

	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmd;

	vk::Semaphore signalSemaphores[] = { syn::ren::semaphores::signal };
	vk::Semaphore waitSemaphores[] = { syn::ren::semaphores::wait };
	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.pSignalSemaphoreInfos = &signal;
	submitInfo.waitSemaphoreInfoCount = 1;
	submitInfo.pWaitSemaphoreInfos = &wait;

	syn::ren::queues::graphics.submit2(1, &submitInfo, syn::ren::fences::inFlight);

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