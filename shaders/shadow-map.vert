#version 330

// input attributes 
layout(location = 0) in vec3 position; 

// input uniforms
uniform mat4 mvpMat;

uniform sampler2D heightmap;

void main() {
  vec2 texcoord = position.xy*0.5+0.5;
  float height = texture(heightmap,texcoord).x;
  vec3 pos = position-vec3(0,0,height-0.5);
  gl_Position = mvpMat*vec4(pos,1.0);
}
