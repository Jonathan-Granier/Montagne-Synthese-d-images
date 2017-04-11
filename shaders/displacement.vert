#version 330

// input attributes
layout(location = 0) in vec3 position;// position of the vertex in world space


uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)
uniform mat3 normalMat; // normal matrix (constant for all the vertices)
uniform sampler2D normalmap;

out vec2 texcoord;


void main() {
  texcoord = position.xy*0.5+0.5;
  vec3 pos = position + vec3(0,0,texture(normalmap,texcoord).a);

  gl_Position = projMat*mdvMat*vec4(pos,1.0);
}

