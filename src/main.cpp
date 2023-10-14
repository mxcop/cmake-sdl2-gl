#include <SDL.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <imgui_impl_win32.h>
#endif

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "engine/shader.h"
#include "engine/sockets/common.h"
#include "engine/sprite.h"
#include "engine/texture.h"
#include "game/net.h"

int exit(int code, SDL_GLContext context, SDL_Window *window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return code;
}

int main(int argc, char **argv)
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window =
        SDL_CreateWindow("[glad] GL with SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    SDL_GLContext context = SDL_GL_CreateContext(window);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    /* Enable OpenGL alpha blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* ImGui setup */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

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
    if (shader_load(&shader, vert_src.c_str(), frag_src.c_str()) == false)
    {
        perror("Error loading shader!\n");
        return exit(-1, context, window);
    }

    Texture texture = {};
    if (texture_load(&texture, "./assets/player.png") == false)
    {
        perror("Error loading texture!\n");
        return exit(-1, context, window);
    }

    Sprite sprite = sprite_from(shader, texture);

    char in_address[128] = "127.0.0.1";
    char in_port[16] = "1440";
    char in_msg[128] = "";

    bool selected = false;
    char msg[64] = "";

    std::thread *net_th = nullptr;
    std::atomic_bool should_exit = false;
    std::atomic_bool msg_queued = false;
    std::vector<std::string> msg_log;

    int alive = 1;
    while (alive)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
            case SDL_QUIT:
                alive = 0;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    alive = 0;
                }
                // if (event.key.keysym.sym == SDLK_SPACE) {
                //     client.send("Jump!", 6);
                //     char buf[128] = {};
                //     client.recv(buf, 128);
                //     SDL_Delay(5);
                //     std::cout << "Server says: " << buf << std::endl;
                // }
                break;
            default:
                break;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        /* Socket Debug Window */
        ImGui::Begin("Multiplayer [Debugger]");
        ImGui::SetWindowFontScale(1.5f);
        if (selected == false)
        {
            ImGui::Text("Enter address & port below to start or connect to a server.");
            ImGui::InputText("Address", in_address, IM_ARRAYSIZE(in_address));
            ImGui::InputText("Port", in_port, IM_ARRAYSIZE(in_port));

            if (ImGui::Button("Host"))
            {
                selected = true;
                strcpy(msg, "Hosting server.");
                net_th = new std::thread(net_thread, &should_exit, &msg_queued, &msg_log, in_msg, NetType::SERVER,
                                         "127.0.0.1", in_port);
            }
            if (ImGui::Button("Connect"))
            {
                selected = true;
                strcpy(msg, "Connected to server.");
                net_th = new std::thread(net_thread, &should_exit, &msg_queued, &msg_log, in_msg, NetType::CLIENT,
                                         in_address, in_port);
            }
        }
        else
        {
            ImGui::Text("%s", msg);
            ImGui::InputText("Message", in_msg, IM_ARRAYSIZE(in_msg));
            if (ImGui::Button("Send"))
            {
                msg_queued = true;
            }
            ImGui::BeginChild("Msg Log");
            ImGui::Text("This is the log:");
            for (std::string str : msg_log)
                ImGui::Text("%s", str.c_str());
            ImGui::EndChild();
        }
        ImGui::End();

        /* Render ImGui */
        ImGui::Render();
        glClearColor(0.105f, 0.105f, 0.105f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw some sprites */
        sprite.draw(glm::vec2(6.0f, 6.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        sprite.draw(glm::vec2(8.0f, 8.0f), glm::vec2(1.0f, 1.0f), 0.0f);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        SDL_Delay(10);

#ifdef _DEBUG
        flushall();
#endif
    };

    should_exit = true;

#ifdef _DEBUG
    flushall();
#endif

    if (net_th)
        net_th->join();
    // client.close();
    // th1.join();

    return exit(0, context, window);
}
