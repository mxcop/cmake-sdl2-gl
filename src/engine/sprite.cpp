#include "sprite.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

void Sprite::init()
{
    // Init objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);

	float vertices[] =
	{
		// x   y      u     v
		0.0f, 1.0f,  0.0f, 0.0f,
		1.0f, 1.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 1.0f
	};

	// Upload the vertices to the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Init more objects
	glGenBuffers(1, &ebo);

	GLuint indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	// Upload the indices (elements) to the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Pos atrribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// Tex attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(GLfloat)));

	// Unbind the GL buffers:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Sprite::Sprite(Shader *shader, Texture *texture)
{
    this->shader = shader;
    this->tex = texture;
}

Sprite::~Sprite()
{
    delete shader;
    delete tex;
}

void Sprite::draw(glm::vec2 position, glm::vec2 size, float rotate)
{
    // Bind the GL buffers:
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Compute the model matrix:
	glm::mat4 model = glm::mat4(1.0f/* Identity matrix */);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	// model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));
	model = glm::translate(model, -glm::vec3(0.5f, 0.5f, 0.0f));

	// Set the uniforms within the shader:
	shader->use();
	shader->set_mat4("model", model);
	shader->set_int("frame", 0);
	shader->set_int("frames", 1);
	shader->set_int("flip_x", false);
	tex->bind();

	// Draw the quad with texture.
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Unbind the GL buffers:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
