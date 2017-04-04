#version 330

layout(location = 0) in vec3 position;

// A débattre
uniform vec2 vertex;
out vec2 texcoord;


void main() {

gl_Position = vec4(vertex,0,1);

texcoord = position.xy*0.5+0.5;

}



