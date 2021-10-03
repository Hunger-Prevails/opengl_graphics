#version 460 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;
in mat4 iModel;

out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    gl_Position = iModel * vec4(aPos, 1.0);
    vNormal = mat3(transpose(inverse(iModel))) * aNormal;
    vTexCoord = aTexCoord;
}
