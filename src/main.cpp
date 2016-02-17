#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <iostream>

#include "debug/pretty_print.hpp"

int main()
{
	if ( ! glfwInit()) {
		return -1;
	}

	if ( ! glfwVulkanSupported()) {
		return -2;
	}

	// Initialize Vulkan
	VkInstanceCreateInfo create_info = {};
	{
		int required_extension_count = 0;
		auto required_extension_names = glfwGetRequiredInstanceExtensions(&required_extension_count);
		if (required_extension_names != nullptr) {
			create_info.ppEnabledExtensionNames = required_extension_names;
			create_info.enabledExtensionCount = required_extension_count;
		} else {
			return -3;
		}
	}

	VkInstance instance;
	if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
		return -4;
	}

	// List available devices
	{
		// Get device count
		uint32_t phys_device_count;
		vkEnumeratePhysicalDevices(instance, &phys_device_count, NULL);

		// Get devices
		std::vector<VkPhysicalDevice> phys_devices{phys_device_count};
		vkEnumeratePhysicalDevices(instance, &phys_device_count, phys_devices.data());

		// Iterate over devices
		for (const auto &phys_device : phys_devices) {
			VkPhysicalDeviceProperties phys_device_props;
			vkGetPhysicalDeviceProperties(phys_device, &phys_device_props);

			std::cout << "Found physical device!" << std::endl;
			std::cout << phys_device_props << std::endl;
		}
	}

	vkDestroyInstance(instance, nullptr);
	glfwTerminate();
	return 0;
}
