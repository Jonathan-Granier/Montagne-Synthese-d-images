#version 330

out vec4 outBuffer;

uniform sampler2D colormap;
uniform sampler2D normalmap;
uniform sampler2D shadowmap;

uniform vec3      light;
uniform int       anim;

in vec2 texcoord;

vec4 shade(in vec2 coord) {
  vec4  nd = texture(normalmap,coord);
  vec4  c  = texture(colormap ,coord);

  vec3  n = nd.xyz;
  float d = nd.w;

  vec3 e = vec3(0,0,1);
  vec3 l;
  l = normalize(light);

  float diff = max(dot(l,n),0.0);
  float spec = pow(max(dot(reflect(l,n),e),0.0),d*100.0);

  return vec4(c.xyz*(diff + spec),1);
}

void main() {
  //vec2 coord = floor(texcoord*250)/250;
  vec4 color = shade(texcoord);

  vec4 fog_color = vec4 (0.8,0.8,0.8,1);
  float depth = min(1,max(0,texture(normalmap,texcoord).a));

  outBuffer = (1-depth)*color + depth*fog_color;

}
