#include <SDL.h>
#include <SDL_opengl.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "engine/shader.h"
#include "engine/sprite.h"
#include "engine/texture.h"

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetProcessDPIAware(); /* <- handle high DPI screens on Windows */
#endif

    // code without checking for errors
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "[glad] GL with SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_GLContext context = SDL_GL_CreateContext(window);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version),
           GLAD_VERSION_MINOR(version));

    /* Enable OpenGL alpha blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const std::string vert_src =
#include "engine/shaders/sprite.vert"
        ;

    const std::string frag_src =
#include "engine/shaders/sprite.frag"
        ;

    Shader shader = {};
    if (shader_load(&shader, vert_src.c_str(), frag_src.c_str()) == false) {
        printf("Error loading shader!\n");
        goto prog_close;
    }

    Texture texture = {};
    if (texture_load(&texture, "./assets/player.png") == false) {
        printf("Error loading texture!\n");
        goto prog_close;
    }

    Sprite sprite = sprite_from(shader, texture);

    int exit = 0;
    while (!exit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: exit = 1; break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) { exit = 1; }
                    break;
                default: break;
            }
        }

        glClearColor(0.105f, 0.105f, 0.105f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw some sprites */
        sprite.draw(glm::vec2(6.0f, 6.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        sprite.draw(glm::vec2(8.0f, 8.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }

prog_close:
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
