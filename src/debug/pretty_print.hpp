#pragma once

#include <vulkan/vulkan.h>
#include <ostream>

std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceProperties &props);
std::ostream &operator<<(std::ostream &os, const VkPhysicalDeviceType &type);
