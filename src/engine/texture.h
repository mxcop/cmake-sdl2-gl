#pragma once

struct Texture {
    uint32_t id = 0;

    /* Dimensions in pixels */
    int width = 0, height = 0;
};

/**
 * @brief Load an image as OpenGL texture.
 *
 * @param out_tex The output texture.
 * @param file_path The relative file path to the image.
 * @return True if the texture was loaded succesfully.
 */
bool texture_load(Texture* out_tex, const char* file_path);

/**
 * @brief Bind an OpenGL texture.
 *
 * @param tex The texture to bind.
 * @param bind_id The ID to bind the texture to. (default 0)
 */
void texture_bind(const Texture* tex, int bind_id = 0);
