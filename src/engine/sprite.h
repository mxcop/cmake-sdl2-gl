#pragma once

class Shader;
class Texture;

class Sprite 
{
    Shader* shader;
    Texture* tex;

    uint32_t vao, vbo, ebo;

    void init();

public:
    /**
     * @warning The Sprite takes ownership of both the [shader] and the [texture]!
     */
    Sprite(Shader* shader, Texture* texture);
	~Sprite();

    /**
     * @brief Draw sprite to the screen.
     */
    void draw(glm::vec2 position, glm::vec2 size, float rotate = 0.0f);
};
