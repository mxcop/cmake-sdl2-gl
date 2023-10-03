#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <filesystem>
#include <stdlib.h>

#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/sprite.h"

constexpr GLuint WIDTH = 800, HEIGHT = 600;

std::string relative_path(const char* p)
{
	return std::filesystem::absolute(p).string();
}

int main( int argc, char** argv ) {
    // code without checking for errors
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "[glad] GL with SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    SDL_GLContext context = SDL_GL_CreateContext(window);

    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    const std::string vert_src =
	#include "engine/shaders/sprite.vert"
	;

	const std::string frag_src =
	#include "engine/shaders/sprite.frag"
	;

    Shader* shader = new Shader(vert_src.c_str(), frag_src.c_str());

    int width, height, nr_channels;
    std::string path = relative_path("./assets/player.png");
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);

    const char* error = stbi_failure_reason();

    if (error != nullptr)
    {
        printf("Error loading texture :\n%s", error);

        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    Texture* texture = new Texture(width, height, data, GL_RGBA);

    stbi_image_free(data);

    Sprite* sprite = new Sprite(shader, texture);

    int exit = 0;
    while(!exit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    exit = 1;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        exit = 1;
                    }
                    break;
                default:
                    break;
            }
        }

        glClearColor(0.105f, 0.105f, 0.105f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        sprite->draw(glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f);

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }

    delete sprite;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}