#include "shader.h"

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Print any shader compilation errors to the console.
 */
bool log_err(unsigned int shader_id, std::string type);

bool shader_load(Shader *out_shader, const char *vert_src, const char *frag_src, const char *geo_src)
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

    if (geo_src != nullptr) 
    {
        /* Compile geometry shader */
        geo_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geo_shader, 1, &geo_src, NULL);
        glCompileShader(geo_shader);
        if (!log_err(geo_shader, "GEOMETRY")) return false;
    }

    /* Create the shader program */
    out_shader->id = glCreateProgram();

    /* Attach the compiled shaders */
    glAttachShader(out_shader->id, vert_shader);
    glAttachShader(out_shader->id, frag_shader);
    if (geo_src != nullptr) 
    {
        glAttachShader(out_shader->id, geo_shader);
    }

    /* Link the shader program */
    glLinkProgram(out_shader->id);
    if (!log_err(out_shader->id, "PROGRAM")) return false;

    /*
        Finally, delete the shaders once linked.
        Now they're stored on the GPU.
    */
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geo_src != nullptr) 
    {
        glDeleteShader(geo_shader);
    }

    return true;
}

void shader_use(Shader shader)
{
    glUseProgram(shader.id);
}

void shader_uniform_float(Shader shader, const char *name, float value)
{
    glUniform1f(glGetUniformLocation(shader.id, name), value);
}

void shader_uniform_int(Shader shader, const char *name, int value)
{
    glUniform1i(glGetUniformLocation(shader.id, name), value);
}

void shader_uniform_mat4(Shader shader, const char *name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, false, glm::value_ptr(matrix));
}

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
