#pragma once

class Texture
{
    /* Dimensions in pixels */
    uint32_t width = 0, height = 0;

    /* Texture config */
    uint32_t wrap_s = 0; /* Wrapping mode on S axis */
    uint32_t wrap_t = 0; /* Wrapping mode on T axis */
    uint32_t filter_min = 0; /* Filtering mode if texture pixels < screen pixels */
    uint32_t filter_max = 0; /* Filtering mode if texture pixels > screen pixels */

    void gen(uint32_t width, uint32_t height, uint8_t* data);

public:
    uint32_t id = 0;

    uint32_t int_fmt = 0; /* Internal format */
    uint32_t img_fmt = 0; /* Image format */

    Texture() {}
    Texture(uint32_t width, uint32_t height, uint8_t* data, uint32_t img_fmt);

    void bind(int tex_idx = 0) const;
};