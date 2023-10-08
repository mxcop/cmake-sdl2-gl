#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>
#include <glad/gl.h>

/**
 * @brief Convert a relative path to an absolute one.
 */
std::string relative_path(const char* p)
{
	return std::filesystem::absolute(p).string();
}

bool texture_load(Texture* out_tex, const char* file_path)
{
    /* Assert if the output texture pointer is a nullptr */
    assert(out_tex != nullptr);

    /* Get the relative file path */
    std::string path = relative_path(file_path); /* ./assets/.. */

    /* Load the image file */
    int nr_channels = 0;
    uint8_t* data = stbi_load(path.c_str(), &out_tex->width, &out_tex->height, &nr_channels, 0);

    /* Check if loading the file caused an error */
    if (stbi_failure_reason() != nullptr)
    {
        return false;
    }

    /* Image format */
    uint32_t img_fmt;
    switch (nr_channels)
    {
    case 3:
        img_fmt = GL_RGB;
        break;
    case 4:
        img_fmt = GL_RGBA;
        break;
    default:
        return false;
    }

    /* Create a new texture */
    glGenTextures(1, &out_tex->id);
    glBindTexture(GL_TEXTURE_2D, out_tex->id);

    /* Set wrap and filter modes */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Generate the image */
    glTexImage2D(GL_TEXTURE_2D, 0, img_fmt, out_tex->width, out_tex->height, 0, img_fmt, GL_UNSIGNED_BYTE, data);

    /* Cleanup */
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return true;
}

void texture_bind(const Texture *tex, int bind_id)
{
    /* Bind our texture id */
    glActiveTexture(GL_TEXTURE0 + bind_id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}
