#include "debug/pretty_print.hpp"

std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceProperties &props) {
	os << "\tID:" << props.deviceID << std::endl;
	os << "\tName:" << props.deviceName << std::endl;
	return os;
}
