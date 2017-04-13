#version 330

// input attributes
layout(location = 0) in vec3 position;// position of the vertex in world space


uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)
uniform mat3 normalMat; // normal matrix (constant for all the vertices)
uniform sampler2D normalmap;

out vec2 texcoord;
out vec3 normalView;
out vec3 eyeView;

void main() {
  texcoord = position.xy*0.5+0.5;
  vec4 normal = texture(normalmap,texcoord);
  vec3 pos = position + vec3(0,0,normal.a-0.5);
  normalView  = normalize(normalMat*normal.xyz);
  eyeView     = normalize(vec4(mdvMat*vec4(position,1.0)).xyz);
  gl_Position = projMat*mdvMat*vec4(pos,1.0);
}

