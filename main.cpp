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
