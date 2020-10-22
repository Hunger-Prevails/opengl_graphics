#version 460 core

in vec2 vTexCoord;

uniform sampler2D uDiffuse;

void main()
{    
    gl_FragColor = texture(uDiffuse, vTexCoord);
}