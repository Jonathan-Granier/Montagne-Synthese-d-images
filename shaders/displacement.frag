#version 330

// the 2 output locations: refers to glDrawBuffers in the cpp file
out vec4 outBuffer;

in vec2 texcoord;

uniform sampler2D normalmap;

void main() {
  outBuffer = texture(normalmap,texcoord);

  //outBuffer = vec4(1,0,0,1);
}
