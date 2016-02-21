#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "debug/pretty_print.hpp"
#include "utils/demangle.hpp"

constexpr std::size_t QUEUE_COUNT = 8;

int main()
{
	if ( ! glfwInit()) {
		std::cout << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	glfwSetErrorCallback([](int, const char* msg){std::cout << msg << std::endl;});

	if ( ! glfwVulkanSupported()) {
		std::cout << "Vulkan is not supported on this system!" << std::endl;
		return -2;
	}

	// Initialize Vulkan

	VkApplicationInfo app_info = {};
	{
		app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext              = nullptr;
		app_info.pApplicationName   = "VkVoxels";
		app_info.applicationVersion = create_version(0, 0, 1);
		app_info.pEngineName        = "VkVoxels Engine";
		app_info.engineVersion      = create_version(0, 0, 1);
		app_info.apiVersion         = create_version(1, 0, 0);
	}

	VkInstanceCreateInfo instance_create_info = {};
	{
		int required_extension_count = 0;
		auto required_extension_names = glfwGetRequiredInstanceExtensions(&required_extension_count);
		if (required_extension_names != nullptr) {
			instance_create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instance_create_info.pNext                   = nullptr;
			instance_create_info.flags                   = 0;
			instance_create_info.pApplicationInfo        = &app_info;
			instance_create_info.enabledLayerCount       = 0;
			instance_create_info.ppEnabledLayerNames     = nullptr;
			instance_create_info.enabledExtensionCount   = required_extension_count;
			instance_create_info.ppEnabledExtensionNames = required_extension_names;
		} else {
			std::cout << "Error getting Vulkan extensions from GLFW!" << std::endl;
			return -3;
		}
	}

	VkInstance instance;
	if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS) {
		std::cout << "Error creating Vulkan instance!" << std::endl;
		return -4;
	}

	std::vector<VkPhysicalDevice> phys_devices;
	// List available devices
	{
		// Get device count
		uint32_t phys_device_count = 0;
		vkEnumeratePhysicalDevices(instance, &phys_device_count, nullptr);

		// Get devices
		phys_devices.resize(phys_device_count);
		vkEnumeratePhysicalDevices(instance, &phys_device_count, phys_devices.data());

		// Iterate over devices
		for (const auto &phys_device : phys_devices) {
			std::cout << "Found physical device!" << std::endl;
			VkPhysicalDeviceProperties phys_device_props;
			vkGetPhysicalDeviceProperties(phys_device, &phys_device_props);
			std::cout << phys_device_props << std::endl;


			std::cout << "Device Queue Families:" << std::endl;

			// Get Queue count
			uint32_t queue_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &queue_count, nullptr);

			// Get Queues
			std::vector<VkQueueFamilyProperties> queue_props(queue_count);
			vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &queue_count, queue_props.data());

			auto queue_index = 0u;
			for (const auto &queue_prop : queue_props) {
				std::cout << "Queue Number " << queue_index++ << std::endl;
				std::cout << queue_prop << std::endl;
			}
		}
	}

	if (phys_devices.empty()) {
		std::cout << "Could not find any physical devices!" << std::endl;
		return -5;
	}

	VkPhysicalDevice *phys_dev = nullptr;
	for (auto &physical_device : phys_devices) {
		if (glfwGetPhysicalDevicePresentationSupport(instance, physical_device, 0) == GLFW_TRUE) {
			phys_dev = &physical_device;
		}
	}
	if (phys_dev == nullptr) {
		std::cout << "Could not find device capable of presenting images!" << std::endl;
		return -6;
	}

	// Create the logical Vulkan device
	VkDevice device;
	{
		const float queue_priorities[] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
		VkDeviceQueueCreateInfo queue_create_info = {};
		{
			queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.pNext            = nullptr;
			queue_create_info.flags            = 0;
			queue_create_info.queueFamilyIndex = 0;
			queue_create_info.queueCount       = QUEUE_COUNT;
			queue_create_info.pQueuePriorities = queue_priorities;
		}

		VkPhysicalDeviceFeatures phys_device_features;
		vkGetPhysicalDeviceFeatures(phys_devices[0], &phys_device_features);

		VkDeviceCreateInfo device_create_info = {};
		{
			device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_create_info.pNext                   = nullptr;
			device_create_info.flags                   = 0;
			device_create_info.queueCreateInfoCount    = 1;
			device_create_info.pQueueCreateInfos       = &queue_create_info;
			device_create_info.enabledLayerCount       = 0;
			device_create_info.ppEnabledLayerNames     = nullptr;
			device_create_info.enabledExtensionCount   = 0;
			device_create_info.ppEnabledExtensionNames = nullptr;
			device_create_info.pEnabledFeatures        = &phys_device_features;
		}

		if (vkCreateDevice(*phys_dev, &device_create_info, nullptr, &device) != VK_SUCCESS) {
			std::cout << "Failed to create logical device!" << std::endl;
			return -7;
		}
	}


	// Get logical device queues
	std::vector<VkQueue> queues(QUEUE_COUNT);
	{
		uint32_t queue_index = 0;
		for (auto &queue : queues) {
			vkGetDeviceQueue(device, 0, queue_index++, &queue);
		}
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(640, 480, "VkVoxels", nullptr, nullptr);
	if ( ! window) {
		std::cout << "Failed to create window!" << std::endl;
		return -8;
	}

	// Create command pool
	VkCommandPool command_pool;
	{
		VkCommandPoolCreateInfo pool_create_info = {};
		pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_create_info.pNext            = nullptr;
		pool_create_info.flags            = 0;
		pool_create_info.queueFamilyIndex = 0;

		if (vkCreateCommandPool(device, &pool_create_info, nullptr, &command_pool) != VK_SUCCESS) {
			std::cout << "Failed to create command pool!" << std::endl;
			return -9;
		}
	}

	// Allocate command buffers from command pool
	VkCommandBuffer command_buffer;
	{
		VkCommandBufferAllocateInfo buffer_alloc_info = {};
		buffer_alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		buffer_alloc_info.pNext              = nullptr;
		buffer_alloc_info.commandPool        = command_pool;
		buffer_alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		buffer_alloc_info.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(device, &buffer_alloc_info, &command_buffer) != VK_SUCCESS) {
			std::cout << "Failed to create command buffers!" << std::endl;
			return -10;
		}
	}

	// Record commands into command buffer
	{
		VkCommandBufferBeginInfo buffer_begin_info = {};
		{
			buffer_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			buffer_begin_info.pNext            = nullptr;
			buffer_begin_info.flags            = 0;
			buffer_begin_info.pInheritanceInfo = nullptr;
		}
		if (vkBeginCommandBuffer(command_buffer, &buffer_begin_info) != VK_SUCCESS) {
			std::cout << "Failed to begin command buffer recording!" << std::endl;
			return -11;
		}

		{

		}

		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
			std::cout << "Error recording command buffer!" << std::endl;
			return -12;
		}
	}

	// Submit command buffer for execution
	{
		VkSubmitInfo submit_info = {};
		{
			submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.pNext                = nullptr;
			submit_info.waitSemaphoreCount   = 0;
			submit_info.pWaitSemaphores      = nullptr;
			submit_info.pWaitDstStageMask    = nullptr;
			submit_info.commandBufferCount   = 1;
			submit_info.pCommandBuffers      = &command_buffer;
			submit_info.signalSemaphoreCount = 0;
			submit_info.pSignalSemaphores    = nullptr;
		}
		
		if (vkQueueSubmit(queues[0], 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
			std::cout << "Error submitting work to queue!" << std::endl;
			return -13;
		}
	}

	// Wait for queue to finish execution
	vkQueueWaitIdle(queues[0]);

	// Create window surface
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		std::cout << "Failed to create window surface!" << std::endl;
		return -14;
	}

	while ( ! glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
	vkDestroyCommandPool(device, command_pool, nullptr);
	glfwDestroyWindow(window);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwTerminate();
	return 0;
}
