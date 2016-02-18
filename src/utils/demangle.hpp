#pragma once

#include <tuple>

constexpr int create_version(const uint32_t major, const uint32_t minor, const uint32_t patch) noexcept {
	return (major << 22) | (minor << 12) | (patch);
}

constexpr std::tuple<int, int, int> get_version(const uint32_t version) noexcept {
	return std::make_tuple<int, int, int>(
		version>>22 & 0x3FF,
		version>>12 & 0x3FF,
		version>>00 & 0xFFF
	);
}

constexpr int get_major_version(const uint32_t version) noexcept {
	return std::get<0>(get_version(version));
}
constexpr int get_minor_version(const uint32_t version) noexcept {
	return std::get<1>(get_version(version));
}
constexpr int get_patch_version(const uint32_t version) noexcept {
	return std::get<2>(get_version(version));
}
