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

   float height = (-texture(normalmap,texcoord).a)*2+1;
   float x;

   vec4 blanc = vec4(1,1,1,1);
   vec4 rouge = vec4(0.8,0.2,0.2,1);
   vec4 vert = vec4(0.2,0.9,0.2,1);
   vec4 bleu = vec4(0,0.1,0.9,1);

   //outTexBuffer = texture(texroche,texcoord);
   if(height > 1){
      outTexBuffer = blanc;
   }
   else if(height > 0.5){
      x = (height-0.5)*2;
      outTexBuffer = x*blanc + (1-x)*rouge;
   }
   else if(height > -0.5){
      x = height+0.5;
      outTexBuffer = x*rouge + (1-x)*vert;
   }
   else if(height > -1){
      x = (height+1)*2;
      outTexBuffer = x*vert + (1-x)*bleu;
   }
   else{ //height < -1
      outTexBuffer = bleu;
   }
   outDepthBuffer = vec4(n,depth);
}
