#version 330

layout(location = 0) out vec4 outTexBuffer;
layout(location = 1) out vec4 outDepthBuffer;

in vec2 texcoord;
in vec3 normalView;
in float depth;

uniform vec3 light;
uniform sampler2D normalmap;
uniform sampler2D texroche;

void main() {
   vec3 n = normalize(normalView);

   outTexBuffer = texture(texroche,texcoord);
   outDepthBuffer = vec4(n,depth);
}
