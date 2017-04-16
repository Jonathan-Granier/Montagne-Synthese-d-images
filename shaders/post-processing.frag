#version 330

out vec4 outBuffer;

uniform sampler2D colormap;
uniform sampler2D depthmap;

in vec2 texcoord;

void main() {
  //vec2 coord = floor(texcoord*250)/250;
  vec4 color = texture(colormap,texcoord);

  vec4 fog_color = vec4 (1,0,0,1);

  float depth = texture(depthmap,texcoord).x;

  outBuffer = (100-depth)*color + depth*fog_color;

}
