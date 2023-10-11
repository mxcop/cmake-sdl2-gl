#include <SDL.h>
// #include <SDL_opengl.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <string>
#include <thread>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include <imgui_impl_win32.h>
#endif

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include "engine/shader.h"
#include "engine/sprite.h"
#include "engine/texture.h"
#include "engine/sockets/common.h"

int exit(int code, SDL_GLContext context, SDL_Window *window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return code;
}

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetProcessDPIAware(); /* <- handle high DPI screens on Windows */
    ImGui_ImplWin32_EnableDpiAwareness();
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

    /* ImGui setup */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 130");

    const std::string vert_src =
#include "engine/shaders/sprite.vert"
        ;

    const std::string frag_src =
#include "engine/shaders/sprite.frag"
        ;

    Shader shader = {};
    if (shader_load(&shader, vert_src.c_str(), frag_src.c_str()) == false) {
        perror("Error loading shader!\n");
        return exit(-1, context, window);
    }

    Texture texture = {};
    if (texture_load(&texture, "./assets/player.png") == false) {
        perror("Error loading texture!\n");
        return exit(-1, context, window);
    }

    Sprite sprite = sprite_from(shader, texture);

    /* Socket server */
    ServerSocket server = ServerSocket();
    if (server.bind("1440") == false) {
        perror("Error binding server!\n");
        return exit(-1, context, window);
    }

    auto f = [&]() {
        ClientSocket* client = server.accept();
        char buf[128] = {};
        if (client != nullptr) {
            while(client->recv(buf, 128) > 0) {
                std::cout << "Client says: " << buf << std::endl;
                client->send("Copy that!", 11);
                memset(buf, 0, 128);
            }
            client->close();
        }
        server.close();
    };

    std::thread th1(f);

    /* Socket client */
    ClientSocket client = ClientSocket();
    if (client.connect("127.0.0.1", "1440") == false) {
        perror("Error connecting to server!\n");
        return exit(-1, context, window);
    }
    client.send("Hello, world!", 14);

    int alive = 1;
    while (alive) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT: alive = 0; break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) { alive = 0; }
                    if (event.key.keysym.sym == SDLK_SPACE) { 
                        client.send("Jump!", 6); 
                        char buf[128] = {};
                        client.recv(buf, 128);
                        SDL_Delay(5);
                        std::cout << "Server says: " << buf << std::endl;
                    }
                    break;
                default: break;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello, world!");
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("This is some useful text.");
        ImGui::End();

        ImGui::Render();
        glClearColor(0.105f, 0.105f, 0.105f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw some sprites */
        sprite.draw(glm::vec2(6.0f, 6.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        sprite.draw(glm::vec2(8.0f, 8.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    };

    client.close();
    th1.join();

    return exit(0, context, window);
}
