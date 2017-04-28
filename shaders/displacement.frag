#version 330

layout(location = 0) out vec4 outTexBuffer;
layout(location = 1) out vec4 outDepthBuffer;

in vec2 texcoord;
in vec3 normalView;
in vec3 eyeView;
in float depth;
in vec4 pos_l;

uniform vec3 light;
uniform sampler2D normalmap;
uniform sampler2D texlave;
uniform sampler2D texeau;
uniform sampler2DShadow shadowmap;
uniform float position_x;
uniform float position_y;
uniform float anim_x;
uniform float anim_y;

// used for the last question
vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

void main() {

   vec3 n = normalize(normalView);

   vec4 normal = texture(normalmap,texcoord);
   float height = (-normal.a)*2+1;
   float x;

   vec3 neige = vec3(0.5,0.5,0.5);
   vec3 caillou = vec3(0.3,0.3,0.3);
   vec3 foret = vec3(0.05,0.3,0.05);
   vec3 sable = vec3(0.3,0.25,0.05);

   vec3 surface_color;

   //outTexBuffer = texture(texroche,texcoord);
   if(height > 1){
      surface_color = neige;
   }
   else if(height > 0.5){
      x = (height-0.5)*2;
      surface_color = x*neige + (1-x)*caillou;
   }
   else if(height > -0.5){
      x = height+0.5;
      surface_color = x*caillou + (1-x)*foret;
   }
   else if(height > -1){
      x = (height+1)*2;
      surface_color = x*foret + (1-x)*sable;
   }
   else{ //height < -1
      surface_color = sable;
   }

   // gestion des lacs
   if(normal.r == 0 && normal.g == 0 && normal.b==1 && height<-0.4)
       surface_color = 0.3*texture(texeau,texcoord+vec2(position_x+anim_x,position_y+anim_y)).xyz;


   // Phong parameters (colors and roughness)
   const vec3 diffuse  = vec3(0.8,0.8,0.6);
   const vec3 specular = vec3(0.8,0.6,0.6);
   const float et = 50.0;

   // normal / view and light directions (in camera space)
   vec3 e = -normalize(eyeView);
   vec3 l = normalize(light);

   // diffuse and specular components of the phong shading model
   float diff = max(dot(l,n),0.0);
   float spec = pow(max(dot(reflect(l,n),e),0.0),et);

   // final color
   vec3 color = surface_color + diff*diffuse + spec*specular;
   float v=0;

   // *** TODO: compute visibility by comparing current depth and the depth in the shadow map ***
   //float depth = texture(shadowmap,pos_l.xy).x;
   /*float depth_shadowmap = texture(shadowmap,pos_l.xyz);
   float depth_l = pos_l.x;
   if(depth_shadowmap < depth_l)
       v=0.5;
   else*/
       v=1.0;
/*
   int i;
   vec4 shadcoord = pos_l;
   for(i=0;i<16;i++){
       v += texture(shadowmap,vec3(shadcoord.xy + poissonDisk[i]/300.0,(shadcoord.z-0.005)/shadcoord.w));
       //shadcoord.xy += poissonDisk[i]/300.0;
   }
   v/=16.0;
*/
   //v=1;
   // gestion de la lave
   if(normal.r == 0 && normal.g == 0 && normal.b==1 && height>0.6)
       outTexBuffer = texture(texlave,texcoord+vec2(position_x+anim_x,position_y+anim_y));
   else
       outTexBuffer = vec4(color*v,1.0);
   outDepthBuffer = vec4(n,depth);
}
