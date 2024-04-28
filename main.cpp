#include "main.h"


std::string readFile(fs::path path)
{
	// Open the stream to 'lock' the file.
	std::ifstream f(path, std::ios::in | std::ios::binary);

	// Obtain the size of the file.
	const auto sz = fs::file_size(path);

	// Create a buffer.
	std::string result(sz, '\0');

	// Read the whole file into the buffer.
	f.read(result.data(), sz);

	return result;

}

vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code) {
	vk::ShaderModuleCreateInfo smci;
	smci.codeSize = code.size()*4;
	smci.pCode = code.data();

	vk::ShaderModule sm;
	syn::device.createShaderModule(&smci, nullptr, &sm);
	return sm;
}

static EShLanguage FindLanguage(const vk::ShaderStageFlagBits shader_type) {
	switch (shader_type) {
	case vk::ShaderStageFlagBits::eVertex:
		return EShLangVertex;
	case vk::ShaderStageFlagBits::eTessellationControl:
		return EShLangTessControl;
	case vk::ShaderStageFlagBits::eTessellationEvaluation:
		return EShLangTessEvaluation;
	case vk::ShaderStageFlagBits::eGeometry:
		return EShLangGeometry;
	case vk::ShaderStageFlagBits::eFragment:
		return EShLangFragment;
	case vk::ShaderStageFlagBits::eCompute:
		return EShLangCompute;
	default:
		return EShLangVertex;
	}
}

static void InitResources(TBuiltInResource& Resources) {
	Resources.maxLights = 32;
	Resources.maxClipPlanes = 6;
	Resources.maxTextureUnits = 32;
	Resources.maxTextureCoords = 32;
	Resources.maxVertexAttribs = 64;
	Resources.maxVertexUniformComponents = 4096;
	Resources.maxVaryingFloats = 64;
	Resources.maxVertexTextureImageUnits = 32;
	Resources.maxCombinedTextureImageUnits = 80;
	Resources.maxTextureImageUnits = 32;
	Resources.maxFragmentUniformComponents = 4096;
	Resources.maxDrawBuffers = 32;
	Resources.maxVertexUniformVectors = 128;
	Resources.maxVaryingVectors = 8;
	Resources.maxFragmentUniformVectors = 16;
	Resources.maxVertexOutputVectors = 16;
	Resources.maxFragmentInputVectors = 15;
	Resources.minProgramTexelOffset = -8;
	Resources.maxProgramTexelOffset = 7;
	Resources.maxClipDistances = 8;
	Resources.maxComputeWorkGroupCountX = 65535;
	Resources.maxComputeWorkGroupCountY = 65535;
	Resources.maxComputeWorkGroupCountZ = 65535;
	Resources.maxComputeWorkGroupSizeX = 1024;
	Resources.maxComputeWorkGroupSizeY = 1024;
	Resources.maxComputeWorkGroupSizeZ = 64;
	Resources.maxComputeUniformComponents = 1024;
	Resources.maxComputeTextureImageUnits = 16;
	Resources.maxComputeImageUniforms = 8;
	Resources.maxComputeAtomicCounters = 8;
	Resources.maxComputeAtomicCounterBuffers = 1;
	Resources.maxVaryingComponents = 60;
	Resources.maxVertexOutputComponents = 64;
	Resources.maxGeometryInputComponents = 64;
	Resources.maxGeometryOutputComponents = 128;
	Resources.maxFragmentInputComponents = 128;
	Resources.maxImageUnits = 8;
	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	Resources.maxCombinedShaderOutputResources = 8;
	Resources.maxImageSamples = 0;
	Resources.maxVertexImageUniforms = 0;
	Resources.maxTessControlImageUniforms = 0;
	Resources.maxTessEvaluationImageUniforms = 0;
	Resources.maxGeometryImageUniforms = 0;
	Resources.maxFragmentImageUniforms = 8;
	Resources.maxCombinedImageUniforms = 8;
	Resources.maxGeometryTextureImageUnits = 16;
	Resources.maxGeometryOutputVertices = 256;
	Resources.maxGeometryTotalOutputComponents = 1024;
	Resources.maxGeometryUniformComponents = 1024;
	Resources.maxGeometryVaryingComponents = 64;
	Resources.maxTessControlInputComponents = 128;
	Resources.maxTessControlOutputComponents = 128;
	Resources.maxTessControlTextureImageUnits = 16;
	Resources.maxTessControlUniformComponents = 1024;
	Resources.maxTessControlTotalOutputComponents = 4096;
	Resources.maxTessEvaluationInputComponents = 128;
	Resources.maxTessEvaluationOutputComponents = 128;
	Resources.maxTessEvaluationTextureImageUnits = 16;
	Resources.maxTessEvaluationUniformComponents = 1024;
	Resources.maxTessPatchComponents = 120;
	Resources.maxPatchVertices = 32;
	Resources.maxTessGenLevel = 64;
	Resources.maxViewports = 16;
	Resources.maxVertexAtomicCounters = 0;
	Resources.maxTessControlAtomicCounters = 0;
	Resources.maxTessEvaluationAtomicCounters = 0;
	Resources.maxGeometryAtomicCounters = 0;
	Resources.maxFragmentAtomicCounters = 8;
	Resources.maxCombinedAtomicCounters = 8;
	Resources.maxAtomicCounterBindings = 1;
	Resources.maxVertexAtomicCounterBuffers = 0;
	Resources.maxTessControlAtomicCounterBuffers = 0;
	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
	Resources.maxGeometryAtomicCounterBuffers = 0;
	Resources.maxFragmentAtomicCounterBuffers = 1;
	Resources.maxCombinedAtomicCounterBuffers = 1;
	Resources.maxAtomicCounterBufferSize = 16384;
	Resources.maxTransformFeedbackBuffers = 4;
	Resources.maxTransformFeedbackInterleavedComponents = 64;
	Resources.maxCullDistances = 8;
	Resources.maxCombinedClipAndCullDistances = 8;
	Resources.maxSamples = 4;
	Resources.maxMeshOutputVerticesNV = 256;
	Resources.maxMeshOutputPrimitivesNV = 512;
	Resources.maxMeshWorkGroupSizeX_NV = 32;
	Resources.maxMeshWorkGroupSizeY_NV = 1;
	Resources.maxMeshWorkGroupSizeZ_NV = 1;
	Resources.maxTaskWorkGroupSizeX_NV = 32;
	Resources.maxTaskWorkGroupSizeY_NV = 1;
	Resources.maxTaskWorkGroupSizeZ_NV = 1;
	Resources.maxMeshViewCountNV = 4;
	Resources.limits.nonInductiveForLoops = 1;
	Resources.limits.whileLoops = 1;
	Resources.limits.doWhileLoops = 1;
	Resources.limits.generalUniformIndexing = 1;
	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
	Resources.limits.generalVaryingIndexing = 1;
	Resources.limits.generalSamplerIndexing = 1;
	Resources.limits.generalVariableIndexing = 1;
	Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

void CompileShader(const char* source,vk::ShaderStageFlagBits type, std::vector<uint32_t>& spirv)
{
	glslang::InitializeProcess();

	EShLanguage stage = FindLanguage(type);

	glslang::TShader shader(stage);
	glslang::TProgram program;

	TBuiltInResource Resources = {};
	InitResources(Resources);

	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

	std::string source_str = source;
	const char* source_str_ptr = source_str.c_str();
	const char* const* source_ptr = &source_str_ptr;
	int length = source_str.size();

	shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_6);
	shader.setStringsWithLengths(source_ptr, &length, 1);
	shader.setSourceEntryPoint("main");
	shader.setEntryPoint("main");

	std::cout << "Parsing shader..." << std::endl;
	shader.parse(&Resources, 110, false, messages);
	
	program.addShader(&shader);
	program.link(messages);

	std::cout << "Converting GLSL to SPIR-V..." << std::endl;
	glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

	std::cout << "Shader successfully compiled." << std::endl;

}

vk::Extent2D getSwapExtent(const vk::SurfaceCapabilitiesKHR& cap) {
	/*if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		std::cout << "Window size in pixels: (" << cap.currentExtent.width << "x" << cap.currentExtent.height << ")" << std::endl;
		return cap.currentExtent;
	} else {*/
		int w, h;
		SDL_GetWindowSizeInPixels(syn::window::self,&w,&h);

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



void updateViewportAndScissor() {
	syn::ren::physicalDevice.getSurfaceCapabilitiesKHR(syn::ren::surface::self, &syn::ren::surface::capabilities);
	syn::window::extent= getSwapExtent(syn::ren::surface::capabilities);

	syn::window::viewport.x = 0.0f;
	syn::window::viewport.y = 0.0f;
	syn::window::viewport.width = (float)syn::window::extent.width;
	syn::window::viewport.height = (float)syn::window::extent.height;
	syn::window::viewport.minDepth = 0.0f;
	syn::window::viewport.maxDepth = 1.0f;

	syn::window::scissor.offset = vk::Offset2D(0, 0);
	syn::window::scissor.extent = syn::window::extent;
}

void selectDevice() {
	auto pds = syn::instance.enumeratePhysicalDevices();

	std::cout << ("PHYSICAL DEVICES FOUND: " + std::to_string(pds.size())) << std::endl;

	if (pds.size() <= 0) { exit(-1); }

	syn::ren::physicalDevice = pds[0]; //default device

	if (pds.size() > 1) {
		//shouldn't run into problems
		//unless you're running this
		//on a FUCKING BITCOIN FARM???
		const char* pdsel = "1000";
		long int sel = strtol(pdsel, NULL, 10);
		while (sel > pds.size() || sel <= 0) {

			for (int i = 0; i < pds.size(); ++i) {
				std::cout << ("  -Device " + std::to_string(i + 1) + ": ") << (pds[i].getProperties().deviceName) << std::endl;
			}
			char wtf[4]{}; std::cin.get(wtf, 4);
			pdsel = wtf; sel = strtol(pdsel, NULL, 10);

			if (sel > pds.size() || sel <= 0) {
				std::cout << "Invalid selection." << std::endl; std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		--sel; syn::ren::physicalDevice = pds[sel];
		std::cout << syn::ren::physicalDevice.getProperties().deviceName << " selected." << std::endl;
	}

	// FINDING INDEXES

	{
		auto qfps = syn::ren::physicalDevice.getQueueFamilyProperties();
		int qfpi = 0;
		for (const auto& qf : qfps) {

			if (qf.queueFlags & vk::QueueFlagBits::eGraphics && graphics_index < 0) {
				std::cout << "  -Queue family found for graphics at index " << qfpi << '.' << std::endl;
				graphics_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eCompute && compute_index < 0) {
				std::cout << "  -Queue family found for compute at index " << qfpi << '.' << std::endl;
				compute_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eTransfer && transfer_index < 0) {
				std::cout << "  -Queue family found for transfer at index " << qfpi << '.' << std::endl;
				transfer_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eSparseBinding && sbinding_index < 0) {
				std::cout << "  -Queue family found for sparse binding at index " << qfpi << '.' << std::endl;
				sbinding_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eProtected && protected_index < 0) {
				std::cout << "  -Queue family found for protected at index " << qfpi << '.' << std::endl;
				protected_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eVideoDecodeKHR && vdecode_index < 0) {
				std::cout << "  -Queue family found for video decode at index " << qfpi << '.' << std::endl;
				vdecode_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eVideoEncodeKHR && vencode_index < 0) {
				std::cout << "  -Queue family found for video encode at index " << qfpi << '.' << std::endl;
				vencode_index = qfpi;
			}

			if (qf.queueFlags & vk::QueueFlagBits::eOpticalFlowNV && optflow_index < 0) {
				std::cout << "  -Queue family found for optical flow at index " << qfpi << '.' << std::endl;
				optflow_index = qfpi;
			}

			if (present_index < 0 && qfpi != graphics_index) {
				auto presentSupport = syn::ren::physicalDevice.getSurfaceSupportKHR(qfpi, syn::ren::surface::self);
				if (presentSupport) {
					std::cout << "  -Queue family found for presentation at index " << qfpi << "." << std::endl;

					present_index = qfpi;
				}
			}

			++qfpi;
		}
	}

	float qp = 1.0f;

	bool supportsSwapchain{ false };

	std::vector<vk::ExtensionProperties> dx = syn::ren::physicalDevice.enumerateDeviceExtensionProperties();

	std::vector<const char*> the_extensions;

	for (const auto& x : dx) {
		if (x.extensionName == std::string("VK_KHR_swapchain")) {
			the_extensions.push_back(x.extensionName);
			supportsSwapchain = true;
		}
	}

	std::cout << "Device selected " << (supportsSwapchain ? "supports" : "does not support") << " swapchains." << std::endl;


	std::vector<vk::DeviceQueueCreateInfo> qci;

	vk::DeviceQueueCreateInfo gqci{}; gqci
		.setQueueCount(1)
		.setQueueFamilyIndex(graphics_index)
		.setQueuePriorities(qp);

	qci.push_back(gqci);

	vk::DeviceQueueCreateInfo pqci{}; pqci
		.setQueueCount(1)
		.setQueueFamilyIndex(present_index)
		.setQueuePriorities(qp);

	qci.push_back(pqci);

	vk::PhysicalDeviceFeatures pdf{};

	vk::DeviceCreateInfo ci{}; ci
		.setQueueCreateInfos(qci)
		.setQueueCreateInfoCount(qci.size())
		.setPEnabledFeatures(&pdf)
		.setEnabledExtensionCount(the_extensions.size())
		.setPpEnabledExtensionNames(the_extensions.data())
		.setEnabledLayerCount(0);

	syn::device = syn::ren::physicalDevice.createDevice(ci);
	std::cout << "Device created successfully." << std::endl;
}

void selectFormatAndPresentMode() {

	//TODO: make formats and present modes selectable

	syn::ren::queues::graphics = syn::device.getQueue(graphics_index, 0);
	std::cout << "Graphics queue established at index " << graphics_index << "." << std::endl;
	syn::ren::queues::present = syn::device.getQueue(present_index, 0);
	std::cout << "Presentation queue established at index " << present_index << "." << std::endl;

	if (syn::ren::physicalDevice.getSurfaceCapabilitiesKHR(syn::ren::surface::self, &syn::ren::surface::capabilities) != vk::Result::eSuccess) {
		std::cout << "Getting surface capabilities unsuccessful." << std::endl;
	}

	auto surf_fmt = syn::ren::physicalDevice.getSurfaceFormatsKHR(syn::ren::surface::self);
	auto pst_modes = syn::ren::physicalDevice.getSurfacePresentModesKHR(syn::ren::surface::self);

	std::cout << "Surface formats: " << (!surf_fmt.empty() ? "" : "NOT ") << "FOUND" << std::endl;
	std::cout << "Present modes: " << (!pst_modes.empty() ? "" : "NOT ") << "FOUND" << std::endl << std::endl;


	std::cout << "Available formats: " << std::endl;

	for (const auto& f : surf_fmt) {
		std::cout << "  -" << magic_enum::enum_name(f.format) << "(" <<
			magic_enum::enum_integer(f.format) << "), " <<
			magic_enum::enum_name(f.colorSpace) << "(" <<
			magic_enum::enum_integer(f.colorSpace) << ")"
			<< std::endl;
		if (f.format == vk::Format::eB8G8R8A8Srgb && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			syn::ren::surface::format = f;
		}
	}

	std::cout << "Desired format (SRGB) " << (syn::ren::surface::format == vk::SurfaceFormatKHR() ? "not " : "") << "found." << std::endl;

	if (syn::ren::surface::format == vk::SurfaceFormatKHR()) { syn::ren::surface::format = surf_fmt[0]; }

	std::cout << "Available present modes: " << std::endl;

	for (const auto& p : pst_modes) {
		std::cout << "  -" << magic_enum::enum_name(p) << std::endl;
		if (p == vk::PresentModeKHR::eMailbox) { syn::ren::swapchain::presentMode = p; }
	}

	std::cout << "Desired present mode (mailbox) " << (syn::ren::swapchain::presentMode == vk::PresentModeKHR() ? "not " : "") << "found." << std::endl;

	if (syn::ren::swapchain::presentMode == vk::PresentModeKHR()) { syn::ren::swapchain::presentMode = pst_modes[0]; }

}

void precomputeShaders() {

	//TODO: MAKE THIS MODULAR AND INTEGRATE IT WITH THE MATERIAL SYSTEM IDK

	std::vector<uint32_t> vert;
	std::vector<uint32_t> frag;

	CompileShader(readFile("./vert.glsl").c_str(), vk::ShaderStageFlagBits::eVertex, vert);
	CompileShader(readFile("./frag.glsl").c_str(), vk::ShaderStageFlagBits::eFragment, frag);

	vk::ShaderModule vertShaderModule = CreateShaderModule(vert);
	vk::ShaderModule fragShaderModule = CreateShaderModule(frag);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	syn::ren::meta::shaderStages[0] = vertShaderStageInfo;
	syn::ren::meta::shaderStages[1] = fragShaderStageInfo;
}

//INITIALIZATION AND CREATION

bool vulkanSDLInit() {

	

	// Create an SDL window that supports Vulkan rendering.
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL." << std::endl;
		return false;
	}

	syn::window::self = SDL_CreateWindow("SYN Engine | NOT REAL GAMES", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if(syn::window::self == NULL) {
		std::cout << "Could not create SDL window." << std::endl;
		return false;
	}

	// Get WSI extensions from SDL (we can add more if we like - we just can't remove these)
	unsigned extension_count;
	const char* const *extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

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
	} catch(const std::exception& e) {
		std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
		return false;
	}

	// Create a Vulkan surface for rendering
	VkSurfaceKHR c_surface;
	if(!SDL_Vulkan_CreateSurface(syn::window::self, static_cast<VkInstance>(syn::instance), nullptr, &c_surface)) {
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
	
	syn::ren::meta::vertexInputInfo.vertexBindingDescriptionCount = 0;
	syn::ren::meta::vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	syn::ren::meta::vertexInputInfo.vertexAttributeDescriptionCount = 0;
	syn::ren::meta::vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

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


	commandBuffer.setViewport(0, 1, &syn::window::viewport);
	commandBuffer.setScissor(0, 1, &syn::window::scissor);
	commandBuffer.draw(3, 1, 0, 0);
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

	if (res == vk::Result::eErrorOutOfDateKHR||res==vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
	}



}



int main()
{
	std::cout << "Hello." << std::endl;

	if (!vulkanSDLInit()) { exit(-1); }

	selectDevice();
	selectFormatAndPresentMode();

	syn::window::extent = getSwapExtent(syn::ren::surface::capabilities);

	createSwapChain();
	createImageViews();

	precomputeShaders();

	createPipelineLayout();
	createRenderPass();
	createRenderPipeline();
	createFramebuffers();
	createCommandPool();

	allocateCommandBuffers();

	createSemaphores();
	createFence();

	// Poll for user input.
	bool stillRunning = true;
	while(stillRunning) {

		SDL_Event event;
		while(SDL_PollEvent(&event)) {

			switch(event.type) {

			case SDL_EVENT_QUIT:
				stillRunning = false;
				break;

			case SDL_EVENT_WINDOW_RESIZED:
				recreateSwapChain();
				break;

			default:
				draw();
				break;
			}
		}

		SDL_Delay(10);
	}

	// Clean up.
	syn::instance.destroySurfaceKHR(syn::ren::surface::self);
	SDL_DestroyWindow(syn::window::self);
	SDL_Quit();
	syn::instance.destroy();

	return 0;
}
