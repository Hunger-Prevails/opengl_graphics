#version 460 core

in vec3 vColor;
in vec2 vTexCoord;

uniform float uVolume;
uniform sampler2D uTexture;

void main() 
{
    gl_FragColor = texture(uTexture, vTexCoord) * (1 - uVolume) + vec4(vColor, 1.0f) * uVolume;
}