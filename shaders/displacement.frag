#version 330

// the 2 output locations: refers to glDrawBuffers in the cpp file
out vec4 outBuffer;

in vec2 texcoord;
in vec3 normalView;
in vec3 eyeView;

uniform vec3 light;
uniform sampler2D normalmap;
uniform sampler2D texroche;

void main() {
  vec4 normal = texture(normalmap,texcoord);
  vec4 texColor = texture(texroche,texcoord);

  // Phong parameters (colors and roughness)
  const vec3 ambient  = vec3(0.2,0.3,0.3);
  const vec3 diffuse  = vec3(0.4,0.4,0.4);
  const vec3 specular = vec3(0.2,0.2,0.2);
  const float et = 5.0;

  // normal / view and light directions (in camera space)
  vec3 n = normalize(normalView);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  // diffuse and specular components of the phong shading model
  float diff = max(dot(l,n),0.0);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  // final color
  vec3 color = ambient + diff*diffuse + spec*specular;
  outBuffer = texColor*(diff+spec)*2.0;
}
