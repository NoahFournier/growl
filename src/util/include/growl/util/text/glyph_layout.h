#pragma once

#include "growl/util/assets/font_atlas.h"
#include <string>

namespace Growl {

struct HBData;

struct LayoutInfo {
	int glyph_id;
	int x;
	int y;
	int w;
	int h;
};

class GlyphLayout {
public:
	GlyphLayout(Font& font, std::string text, int width) noexcept;
	~GlyphLayout() noexcept;

	void layout() noexcept;

	void setText(std::string text);

	const int getWidth() const {
		return width;
	}

	const int getHeight() const {
		return height;
	}

	const int getOriginOffset() const {
		return origin_offset;
	}

	const std::vector<LayoutInfo>& getLayout() const {
		return layout_info;
	}

private:
	std::string text;
	int width;
	int height;
	int origin_offset;
	std::unique_ptr<HBData> hb_data;
	std::vector<LayoutInfo> layout_info;
};

} // namespace Growl
