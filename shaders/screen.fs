#version 460 core

in vec2 vTexCoord;

uniform sampler2D uScreen;

const float offset = 1.0 / 1000.0;

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset,  offset), vec2(0.0f,  offset), vec2(offset,  offset),
		vec2(-offset,    0.0f), vec2(0.0f,    0.0f), vec2(offset,    0.0f),
		vec2(-offset, -offset), vec2(0.0f, -offset), vec2(offset, -offset)
	);

	float kernel[9] = float[](
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
		2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	);
	vec3 color = vec3(0.0);

	for(int i = 0; i < 9; i++) { color += vec3(texture(uScreen, vTexCoord.xy + offsets[i])) * kernel[i]; }

	gl_FragColor = vec4(texture(uScreen, vTexCoord).rgb, 1.0);
}