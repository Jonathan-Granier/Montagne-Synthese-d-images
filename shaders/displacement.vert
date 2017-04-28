#version 330

// input attributes
layout(location = 0) in vec3 position;// position of the vertex in world space


uniform mat4 mdvMat; // modelview matrix (constant for all the vertices)
uniform mat4 projMat; // projection matrix (constant for all the vertices)
uniform mat3 normalMat; // normal matrix (constant for all the vertices)
uniform mat4 mvpLightMat;
uniform sampler2D normalmap;

out vec2 texcoord;
out vec3 normalView;
out vec3 eyeView;
out float depth;

out vec4 pos_l;

void main() {
  texcoord = position.xy*0.5+0.5;
  float height = texture(normalmap,texcoord).a;
  vec3 pos = position+vec3(0,0,height-0.5);
  gl_Position = projMat*mdvMat*vec4(pos,1.0);
  vec3 normal = texture(normalmap,texcoord).xyz;
  normalView  = normalMat*normal;
  eyeView     = normalize(vec4(mdvMat*vec4(position,1.0)).xyz);
  depth       = - (vec4(mdvMat*vec4(pos,1.0)).z) / 2.5;

  // project position in light space to compute shadows
  pos_l = (mvpLightMat*vec4(pos,1.0))*0.5 + vec4(0.5,0.5,0.5,0.5);
}

