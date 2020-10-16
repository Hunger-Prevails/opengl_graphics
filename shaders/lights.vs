#version 460 core

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uPerspect;

void main()
{
	vPosition = vec3(uModel * vec4(aPosition, 1.0));
    gl_Position = uPerspect * uView * vec4(vPosition, 1.0);
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;
}