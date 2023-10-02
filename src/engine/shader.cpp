#include "shader.h"

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vert_src, const char *frag_src, const char *geo_src)
{
    compile(vert_src, frag_src, geo_src);
}

Shader &Shader::use()
{
    glUseProgram(this->id);
    return *this;
}

/**
 * @brief Print any shader compilation errors to the console.
 */
bool log_err(unsigned int shader_id, std::string type)
{
    int success;
    char info_log[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader_id, 1024, NULL, info_log);
            printf("ERROR::SHADER: Compile-time error: Type: %s\n%s", type.c_str(), info_log);
        }
    }
    else
    {
        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_id, 1024, NULL, info_log);
            printf("ERROR::Shader: Link-time error: Type: %s\n%s", type.c_str(), info_log);
        }
    }
    return success;
}

bool Shader::compile(const char *vert_src, const char *frag_src, const char *geo_src)
{
    /* Don't compile if either the vertex or fragment shader is null */
    if (vert_src == nullptr || frag_src == nullptr) return false;

    GLuint vert_shader, frag_shader, geo_shader;

    /* Compile vertex shader */
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_src, NULL);
    glCompileShader(vert_shader);
    if (!log_err(vert_shader, "VERTEX")) return false;

    /* Compile fragment shader */
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_src, NULL);
    glCompileShader(frag_shader);
    if (!log_err(frag_shader, "FRAGMENT")) return false;

    if (geo_src != nullptr) {
        /* Compile geometry shader */
        geo_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geo_shader, 1, &geo_src, NULL);
        glCompileShader(geo_shader);
        if (!log_err(geo_shader, "GEOMETRY")) return false;
    }

    /* Create the shader program */
    this->id = glCreateProgram();

    /* Attach the compiled shaders */
    glAttachShader(this->id, vert_shader);
    glAttachShader(this->id, frag_shader);
    if (geo_src != nullptr) {
        glAttachShader(this->id, geo_shader);
    }

    /* Link the shader program */
    glLinkProgram(this->id);
    if (!log_err(this->id, "PROGRAM")) return false;

    /*
        Finally, delete the shaders once linked.
        Now they're stored on the GPU.
    */
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geo_src != nullptr) {
        glDeleteShader(geo_shader);
    }

    return true;
}

#pragma region UNIFORMS

void Shader::set_float(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(this->id, name), value);
}

void Shader::set_int(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(this->id, name), value);
}

void Shader::set_vec2f(const char* name, float x, float y)
{
    glUniform2f(glGetUniformLocation(this->id, name), x, y);
}

void Shader::set_vec2f(const char* name, const glm::vec2& value)
{
    glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y);
}

void Shader::set_vec3f(const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
}

void Shader::set_vec3f(const char* name, const glm::vec3& value)
{
    glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z);
}

void Shader::set_vec4f(const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
}

void Shader::set_vec4f(const char* name, const glm::vec4& value)
{
    glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w);
}

void Shader::set_mat4(const char* name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}

#pragma endregion
