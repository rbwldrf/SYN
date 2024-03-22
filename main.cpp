#include "main.h"


SDL_Window* window;

vk::Extent2D getSwapExtent(const vk::SurfaceCapabilitiesKHR& cap) {
	if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return cap.currentExtent;
	} else {
		int w, h;
		SDL_GetWindowSizeInPixels(window,&w,&h);

		vk::Extent2D actualExtent = {
			static_cast<uint32_t>(w),
			static_cast<uint32_t>(h)
		};

		actualExtent.width = std::clamp(actualExtent.width, cap.minImageExtent.width, cap.minImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, cap.minImageExtent.height, cap.minImageExtent.height);
	
		return actualExtent;
	}
}


int main()
{
	std::string bla;

	std::cout << "Hello." << std::endl;


	// Create an SDL window that supports Vulkan rendering.
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "Could not initialize SDL." << std::endl;
		return 1;
	}

	window = SDL_CreateWindow("SYN Engine | NOT REAL GAMES", 1280, 720, SDL_WINDOW_VULKAN);
	if(window == NULL) {
		std::cout << "Could not create SDL window." << std::endl;
		return 1;
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
	vk::Instance instance;
	try {
		instance = vk::createInstance(instInfo);
	} catch(const std::exception& e) {
		std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
		return 1;
	}

	// Create a Vulkan surface for rendering
	VkSurfaceKHR c_surface;
	if(!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance), nullptr, &c_surface)) {
		std::cout << "Could not create a Vulkan surface." << std::endl;
		return 1;
	}
	vk::SurfaceKHR surface(c_surface);

	// This is where most initializtion for a program should be performed
	
	auto pds = instance.enumeratePhysicalDevices();

	std::cout << ("PHYSICAL DEVICES FOUND: " + std::to_string(pds.size())) << std::endl;

	if (pds.size() <= 0) { exit(-1); }

	auto cur_pd = pds[0]; //default device

	if (pds.size() > 1) {
		//shouldn't run into problems
		//unless you're running this
		//on a FUCKING BITCOIN FARM???
		const char* pdsel = "1000"; 
		long int sel = strtol(pdsel, NULL, 10);
		while (sel > pds.size() || sel <= 0) {

			for (int i = 0; i < pds.size(); ++i) {
				std::cout << ("  -Device " + std::to_string(i+1) + ": ") << (pds[i].getProperties().deviceName) << std::endl; 
			}
			char wtf[4]{}; std::cin.get(wtf,4);
			pdsel = wtf; sel = strtol(pdsel, NULL, 10);

			if (sel > pds.size() || sel <= 0) {
				std::cout << "Invalid selection." << std::endl; std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		--sel; cur_pd = pds[sel];
		std::cout << cur_pd.getProperties().deviceName << " selected." << std::endl;
	}
	
	int graphics_index = -1;
	int compute_index = -1;
	int transfer_index = -1;
	int sbinding_index = -1;
	int protected_index = -1;
	int vdecode_index = -1;
	int vencode_index = -1;
	int optflow_index = -1;

	int present_index = -1;

	auto qfps = cur_pd.getQueueFamilyProperties();
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
			auto presentSupport = cur_pd.getSurfaceSupportKHR(qfpi, surface);
			if (presentSupport) {			
				std::cout << "  -Queue family found for presentation at index " << qfpi << "." << std::endl;

				present_index = qfpi;
			}
		}
		
		++qfpi;
	}

	float qp = 1.0f;
	
	bool supportsSwapchain{false};

	std::vector<vk::ExtensionProperties> dx = cur_pd.enumerateDeviceExtensionProperties();

	std::vector<const char*> the_extensions;

	for (const auto& x : dx) {
		if (x.extensionName == std::string("VK_KHR_swapchain")) {
			the_extensions.push_back(x.extensionName);
			supportsSwapchain = true;
		}
	}

	std::cout << "Device selected " << (supportsSwapchain?"supports":"does not support") << " swapchains." << std::endl;


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

	vk::Device device = cur_pd.createDevice(ci); 
	std::cout << "Device created successfully." << std::endl;

	vk::Queue gq = device.getQueue(graphics_index, 0);
	std::cout << "Graphics queue established at index " << graphics_index << "." << std::endl;
	vk::Queue pq = device.getQueue(present_index, 0);
	std::cout << "Presentation queue established at index " << present_index << "." << std::endl;

	vk::SurfaceCapabilitiesKHR cap; 
	
	if (cur_pd.getSurfaceCapabilitiesKHR(surface, &cap)!=vk::Result::eSuccess) {
		std::cout << "Getting surface capabilities unsuccessful." << std::endl;
		return -1;
	}

	auto surf_fmt = cur_pd.getSurfaceFormatsKHR(surface);
	auto pst_modes = cur_pd.getSurfacePresentModesKHR(surface);

	std::cout << "Surface formats: " << (!surf_fmt.empty()?"":"NOT ") << "FOUND" << std::endl;
	std::cout << "Present modes: " << (!pst_modes.empty()?"":"NOT ") << "FOUND" << std::endl << std::endl;


	// SURFACE FORMATS

	vk::SurfaceFormatKHR fmt{};

	std::cout << "Available formats: " << std::endl;

	for (const auto& f : surf_fmt) {
		std::cout << "  -" << magic_enum::enum_name(f.format) << "(" <<
			magic_enum::enum_integer(f.format) << "), " <<
			magic_enum::enum_name(f.colorSpace) << "(" <<
			magic_enum::enum_integer(f.colorSpace) << ")" 
			<< std::endl;
		if (f.format == vk::Format::eB8G8R8A8Srgb && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			fmt = f;
		}
	}

	std::cout << "Desired format (HDR10) " << (fmt == vk::SurfaceFormatKHR() ? "not " : "") << "found." << std::endl;

	if (fmt == vk::SurfaceFormatKHR()) { fmt = surf_fmt[0]; }


	// PRESENT MODES

	vk::PresentModeKHR pmd{};

	std::cout << "Available present modes: " << std::endl;

	for (const auto& p : pst_modes) {
		std::cout << "  -" << magic_enum::enum_name(p) << std::endl;
		if (p == vk::PresentModeKHR::eMailbox) { pmd = p; }
	}

	std::cout << "Desired present mode (mailbox) " << (pmd == vk::PresentModeKHR() ? "not " : "") << "found." << std::endl;

	if (pmd == vk::PresentModeKHR()) { pmd = pst_modes[0]; }

	
	// EXTENT

	auto ext = getSwapExtent(cap);


	// IMAGE COUNT

	uint32_t imgCt = cap.minImageCount + 1;

	if (cap.maxImageCount > 0 && imgCt > cap.maxImageCount) {
		imgCt = cap.maxImageCount;
	}


	// SWAPCHAIN CREATION
	{
		vk::SwapchainCreateInfoKHR scci{};
		scci.surface = surface;
		scci.minImageCount = imgCt;
		scci.imageFormat = fmt.format;
		scci.imageColorSpace = fmt.colorSpace;
		scci.imageExtent = ext;
		scci.imageArrayLayers = 1;
		scci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		uint32_t qfi[] = { graphics_index,present_index };

		if (graphics_index != present_index) {
			scci.imageSharingMode = vk::SharingMode::eConcurrent;
			scci.queueFamilyIndexCount = 2;
			scci.pQueueFamilyIndices = qfi;
		}
		else {
			scci.imageSharingMode = vk::SharingMode::eExclusive;
		}

		scci.preTransform = cap.currentTransform;
		scci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		scci.presentMode = pmd;
		scci.clipped = vk::True;
		scci.oldSwapchain = nullptr;

		vk::SwapchainKHR sc;
		device.createSwapchainKHR(&scci, nullptr, &sc);

		auto sci = device.getSwapchainImagesKHR(sc).data();
	}

	// Poll for user input.
	bool stillRunning = true;
	while(stillRunning) {

		SDL_Event event;
		while(SDL_PollEvent(&event)) {

			switch(event.type) {

			case SDL_EVENT_QUIT:
				stillRunning = false;
				break;

			default:
				// Do nothing.
				break;
			}
		}

		SDL_Delay(10);
	}

	// Clean up.
	instance.destroySurfaceKHR(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();
	instance.destroy();

	return 0;
}
