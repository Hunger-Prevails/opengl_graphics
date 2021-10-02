#version 460 core

in vec3 aPos;

out vec3 vTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uPerspect;

void main()
{
    vTexCoord = aPos;
    vec4 pos = uPerspect * uView * uModel * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
