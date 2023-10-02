#pragma once

class Shader
{
public:
    uint32_t id = 0u;
    Shader() = default;
    Shader(const char* vert_src, const char* frag_src, const char* geo_src = nullptr);
    
    /**
     * @brief Start using the shader program.
     */
    Shader& use();

    /* Uniform utility functions */
    void set_float(const char* name, float value);
    void set_int(const char* name, int value);
    void set_vec2f(const char* name, float x, float y);
    void set_vec2f(const char* name, const glm::vec2& value);
    void set_vec3f(const char* name, float x, float y, float z);
    void set_vec3f(const char* name, const glm::vec3& value);
    void set_vec4f(const char* name, float x, float y, float z, float w);
    void set_vec4f(const char* name, const glm::vec4& value);
    void set_mat4(const char* name, const glm::mat4& matrix);

private:
    bool compile(const char* vert_src, const char* frag_src, const char* geo_src = nullptr);
};
