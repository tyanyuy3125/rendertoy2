#include "dotfont.h"
#include "composition.h"

#include <algorithm>

const rendertoy::Image rendertoy::GenerateTextImage(const std::vector<std::string> &textlines, const glm::vec4 &color, const unsigned int font_size)
{
    const int char_width = 5;
    const int char_height = 8;

    int max_text_len = 0;
    for(const std::string &text : textlines)
    {
        max_text_len = std::max(max_text_len, static_cast<int>(text.length()));
    }

    int image_width = max_text_len * (char_width + 1);
    int image_height = (char_height + 1) * static_cast<int>(textlines.size());

    Image ret(image_width, image_height);

    int line_id = 0;
    for (const std::string &text : textlines)
    {
        for (int i = 0; i < text.length(); ++i)
        {
            char c = text[i];
            if (c < 32 || c > 127)
            {
                c = static_cast<char>(128);
            }
            const uint8_t *bitmap = dot_font_library[c - 32];
            for (int y = 0; y < char_height; ++y)
            {
                for (int x = 0; x < char_width; ++x)
                {
                    if (bitmap[x] & (1 << y))
                    {
                        ret(i * (char_width + 1) + x, line_id * (char_height + 1) + y) = color;
                    }
                }
            }
        }
        ++line_id;
    }

    if (font_size > 1)
        return ret.UpScale(static_cast<glm::float32>(font_size));

    return ret;
}