#version 460 core

uniform sampler2DMS uScreen;

const float offset = 1.0 / 10.0;


vec4 average(ivec2 coord) {

	vec4 color_a = texelFetch(uScreen, coord, 0);
	vec4 color_b = texelFetch(uScreen, coord, 1);
	vec4 color_c = texelFetch(uScreen, coord, 2);
	vec4 color_d = texelFetch(uScreen, coord, 3);

	return (color_a + color_b + color_c + color_d) / 4.0;
}


vec4 bilinear(vec2 coord) {

	vec2 weight = fract(coord);

	vec4 left = mix(average(ivec2(floor(coord.x), floor(coord.y))), average(ivec2(floor(coord.x), ceil(coord.y))), weight.y);

	vec4 right = mix(average(ivec2(ceil(coord.x), floor(coord.y))), average(ivec2(ceil(coord.x), ceil(coord.y))), weight.y);

	return mix(left, right, weight.x);
}


void main() {

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

	for(int i = 0; i < 9; i++) { color += bilinear(gl_FragCoord.xy + offsets[i]).xyz * kernel[i]; }

	gl_FragColor = vec4(color, 1.0);
}
