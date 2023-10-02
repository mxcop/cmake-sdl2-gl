R"(#version 460 core
in vec2 f_texcoord;
out vec4 color;

/* Animation uniforms */
uniform int frames = 1;
uniform int frame = 0;
uniform int flip_x = 0; 

uniform sampler2D sprite;

void main()
{
	color = texture(sprite, vec2((frame + abs(flip_x - f_texcoord.x)) / frames, f_texcoord.y));
})"
