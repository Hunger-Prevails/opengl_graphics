#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vNormal[];
in vec2 vTexCoord[];

out vec3 gPosition;
out vec3 gNormal;
out vec2 gTexCoord;

uniform float uTime;

uniform mat4 uView;
uniform mat4 uPerspect;

void gen_vertex(int index) {

    gPosition = vec3(gl_in[index].gl_Position) + vNormal[index] * 0.1 * uTime;
    gl_Position = uPerspect * uView * vec4(gPosition, 1.0);
    gNormal = vNormal[index];
    gTexCoord = vTexCoord[index];

    EmitVertex();
}

void main() {
    gen_vertex(0);
    gen_vertex(1);
    gen_vertex(2);

    EndPrimitive();
}