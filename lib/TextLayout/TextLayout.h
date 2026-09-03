#ifndef TEXT_LAYOUT_H
#define TEXT_LAYOUT_H

#include <cstdint>
#include <string>
#include <vector>

class TextLayout {
public:
    TextLayout() = default;
    explicit TextLayout(const std::string& text);

    void setText(const std::string& text);
    void setMetrics(uint16_t charW, uint16_t charH, uint16_t areaW, uint16_t areaH);

    std::vector<std::string> pages() const;

private:
    std::string text_;
    uint16_t charW_ = 12;   // default text-size-2 mono width
    uint16_t charH_ = 14;   // default text-size-2 mono height
    uint16_t areaW_ = 1240;
    uint16_t areaH_ = 600;

    std::vector<std::string> wrapWords(uint16_t charsPerLine) const;
    static std::string trim(const std::string& s);
};

#endif
