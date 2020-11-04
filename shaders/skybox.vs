#version 460 core

in vec3 aPosition;

out vec3 vTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uPerspect;

void main()
{
    vTexCoord = aPosition;
    vec4 pos = uPerspect * uView * uModel * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
}