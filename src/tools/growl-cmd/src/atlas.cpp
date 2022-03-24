#include "../../../contrib/fpng/fpng.h"
#include "../../../contrib/stb_image/stb_image.h"
#include "../contrib/rang.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <growl/util/assets/atlas.h>
#include <growl/util/assets/bundle.h>
#include <growl/util/assets/error.h>
#include <growl/util/error.h>
#include <memory>
#include <ostream>

using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::AtlasImagePackInfo;
using Growl::Error;
using rang::style;

Error includeAtlas(
	std::filesystem::path path, std::filesystem::path& resolved_path,
	AssetsMap& assets_map, std::ofstream& outfile) noexcept {
	std::vector<Growl::AtlasImagePackInfo> images;
	for (auto entry : std::filesystem::directory_iterator(path)) {
		int width, height, channels;
		if (!stbi_info(entry.path().c_str(), &width, &height, &channels)) {
			// Not an image
			continue;
		}
		images.push_back(AtlasImagePackInfo(entry.path(), width, height));
	}

	auto result = packAtlasFromFiles(images);
	if (result.hasError()) {
		return std::move(result.error());
	}
	auto atlas = std::move(result.get());
	for (auto& [name, _] : atlas.getMappings()) {
		std::cout << "=> [" << style::bold << resolved_path.string()
				  << style::reset << "] Included image " << style::bold << name
				  << style::reset << "." << std::endl;
	}

	std::vector<uint8_t> out_buf;
	if (!fpng::fpng_encode_image_to_memory(
			atlas.getImage().getRaw(), atlas.getImage().getWidth(),
			atlas.getImage().getHeight(), 4, out_buf,
			fpng::FPNG_ENCODE_SLOWER)) {
		return std::make_unique<AssetsError>("Failed to encode image.");
	}
	unsigned int ptr = outfile.tellp();
	assets_map[resolved_path] = {
		ptr, out_buf.size(), AssetType::Atlas, atlas.getMappings()};
	outfile.write(
		reinterpret_cast<const char*>(out_buf.data()), out_buf.size());

	return nullptr;
}
