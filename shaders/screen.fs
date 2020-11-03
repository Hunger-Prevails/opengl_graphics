#version 460 core

in vec2 vTexCoord;

uniform sampler2D uScreen;

void main()
{
    vec3 color = texture(uScreen, vTexCoord).rgb;
    gl_FragColor = vec4(color, 1.0);
}