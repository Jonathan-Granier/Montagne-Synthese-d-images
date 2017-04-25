#version 330

layout(location = 0) out vec4 outTexBuffer;
layout(location = 1) out vec4 outDepthBuffer;

in vec2 texcoord;
in vec3 normalView;
in float depth;

uniform vec3 light;
uniform sampler2D normalmap;
uniform sampler2D texlave;
uniform sampler2D texeau;

void main() {
   vec3 n = normalize(normalView);

   vec4 normal = texture(normalmap,texcoord);
   float height = (-normal.a)*2+1;
   float x;

   vec4 blanc = vec4(1,1,1,1);
   vec4 gris = vec4(0.6,0.6,0.6,1);
   vec4 vert = vec4(0.2,0.9,0.2,1);
   vec4 bleu = vec4(0,0.1,0.9,1);

   //outTexBuffer = texture(texroche,texcoord);
   if(height > 1){
      outTexBuffer = blanc;
   }
   else if(height > 0.5){
      x = (height-0.5)*2;
      outTexBuffer = x*blanc + (1-x)*gris;
   }
   else if(height > -0.5){
      x = height+0.5;
      outTexBuffer = x*gris + (1-x)*vert;
   }
   else if(height > -1){
      x = (height+1)*2;
      outTexBuffer = x*vert + (1-x)*bleu;
   }
   else{ //height < -1
      outTexBuffer = bleu;
   }

   // gestion de la lave
   if(normal.r == 0 && normal.g == 0 && normal.b==1 && height>0)
       outTexBuffer = texture(texlave,texcoord);
   // gestion des lacs
   if(normal.r == 0 && normal.g == 0 && normal.b==1 && height<0)
       outTexBuffer = texture(texeau,texcoord);

   outDepthBuffer = vec4(n,depth);
}
