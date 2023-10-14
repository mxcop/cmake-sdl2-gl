#pragma once

struct Shader
{
    uint32_t id = 0;
};

/**
 * @brief Load a shader and store it in VRAM.
 *
 * @param out_shader The output shader.
 * @param vert_src The vertex shader source code.
 * @param frag_src The fragment shader source code.
 * @param geo_src The geometry shader source code. (default nullptr)
 * @return True if the shader was compiled succesfully.
 */
bool shader_load(Shader *out_shader, const char *vert_src, const char *frag_src, const char *geo_src = nullptr);

/**
 * @brief Use shader program stored in VRAM.
 */
void shader_use(Shader shader);

void shader_uniform_float(Shader shader, const char *name, float value);
void shader_uniform_int(Shader shader, const char *name, int value);
void shader_uniform_mat4(Shader shader, const char *name, const glm::mat4 &matrix);
