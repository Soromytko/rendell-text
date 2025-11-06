#pragma once
#include <rendell_text/IGlyphShaper.h>

#include <HB_Resource.h>
#include <rendell_text/ShaperConfig.h>

#include <vector>

namespace rendell_text {
class HarfBuzzShaper final : public IGlyphShaper {
public:
    HarfBuzzShaper(ShaperConfig config);
    ~HarfBuzzShaper() = default;

    ShapeResult shape(const TextRun &textRun) override;
    void shape(const TextRun &textRun, ShapeResult &result) override;

private:
    bool doShape(const HB_Resource &hb, const TextRun &textRun, ShapeResult &result) const;

    const std::vector<hb_feature_t> _features{};
};
} // namespace rendell_text