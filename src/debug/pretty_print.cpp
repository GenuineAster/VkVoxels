#include "debug/pretty_print.hpp"
#include "utils/demangle.hpp"
#include <iomanip>
#include <sstream>

std::string get_version_string(const uint32_t version) {
	std::stringstream verstream;
	verstream << get_major_version(version)
	          << '.'
	          << get_minor_version(version)
	          << '.'
	          << get_patch_version(version);
	return verstream.str();
}

std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceProperties &props) {
	os << std::setiosflags( std::ios_base::showbase );
	os << "\tDevice ID:               " << std::hex << props.deviceID              << std::endl;
	os << "\tVendor ID:               " << std::hex << props.vendorID              << std::endl;
	os << "\tDevice Name:             " << props.deviceName                        << std::endl;
	os << "\tDevice Type:             " << props.deviceType                        << std::endl;
	os << "\tAPI Version:             " << get_version_string(props.apiVersion)    << std::endl;
	os << "\tDriver Version:          " << get_version_string(props.driverVersion) << std::endl;
	os << props.limits;
	return os;
}

std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceLimits &limits) {
	os << std::dec;
	os << "\tMax framebuffer size:    " << limits.maxFramebufferWidth << 'x' << limits.maxFramebufferHeight << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceType &type) {
	switch (type) {
		case VK_PHYSICAL_DEVICE_TYPE_CPU: {
			os << "CPU";
		} break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER: {
			os << "OTHER";
		} break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
			os << "DISCRETE GPU";
		} break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
			os << "INTEGRATED GPU";
		} break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
			os << "VIRTUAL GPU";
		} break;
		default: {
			os << "Unknown";
		} break;
	}
	return os;
}
