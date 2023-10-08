#pragma once

/* Includes because of Data members */
#include "shader.h"
#include "texture.h"

struct Sprite {
    Shader shader;
    Texture tex;

    /* Buffer indices for drawing */
    uint32_t vao, vbo, ebo;

    /**
     * @brief Draw sprite to the screen.
     */
    void draw(glm::vec2 position, glm::vec2 size, float rotate = 0.0f);
};

/**
 * @brief Create a new sprite from a shader and texture.
 */
Sprite sprite_from(Shader shader, Texture texture);
