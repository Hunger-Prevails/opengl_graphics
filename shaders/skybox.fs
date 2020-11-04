#version 460 core

in vec3 vTexCoord;

uniform samplerCube uSkybox;

void main()
{
    gl_FragColor = texture(uSkybox, vTexCoord);
}