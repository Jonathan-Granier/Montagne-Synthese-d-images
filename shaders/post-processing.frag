#version 330

out vec4 outBuffer;

uniform sampler2D colormap;
uniform sampler2D normalmap;
uniform sampler2D shadowmap;

uniform vec3      light;
uniform int       anim;

in vec2 texcoord;

void main() {
  vec4 color;
  color = texture(colormap,texcoord);

  vec4 fog_color = vec4(0.8,0.8,0.8,1);
  float depth = min(1,max(0,texture(normalmap,texcoord).a));

  outBuffer = (1-depth)*color + depth*fog_color;
}
