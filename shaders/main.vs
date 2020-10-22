#version 460 core

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uPerspect;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = uPerspect * uView * uModel * vec4(aPosition, 1.0);
}