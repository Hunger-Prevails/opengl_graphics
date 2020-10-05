#version 460 core

in vec3 vColor;

uniform float uVolume;

void main() 
{
    gl_FragColor = vec4(vColor, 1.0f) * uVolume;
}