#version 330

uniform float amplitude1;
uniform float amplitude2;
uniform float position_x;
uniform float position_y;

in vec2 pos;

out vec4 outBuffer;

float mathpi = 3.14159265358979323846;

vec2 hash(vec2 p) {
  p = vec2( dot(p,vec2(127.1,311.7)),
        dot(p,vec2(269.5,183.3)) );
 
  return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float gnoise(in vec2 p) {
  vec2 i = floor( p );
  vec2 f = fract( p );
   
  vec2 u = f*f*(3.0-2.0*f);
 
  return mix( mix( dot( hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
           dot( hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
          mix( dot( hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
           dot( hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

float pnoise(in vec2 p,in float amplitude,in float frequency,in float persistence, in int nboctaves) {
  float a = amplitude;
  float f = frequency;
  float n = 0.0;
 
  for(int i=0;i<nboctaves;++i) {
    n = n+a*gnoise(p*f);
    f = f*2.;
    a = a*persistence;
  }
 
  return n;
}

vec3 perlinHeightandAngle(in vec3 motion,in float persistence, in int nboctaves){
    float dist=1.0;

    float centerp = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
    motion.x += dist;
    float xPoints1 = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
    motion.x -= 2*dist;
    float xPoints2 = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
    motion.x += dist;

    motion.y += dist;
    float yPoints1 = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
    motion.y -= 2*dist;
    float yPoints2 = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
    motion.y += dist;

    float Xangle = (xPoints1 - xPoints2);
    float Yangle = (yPoints1 - yPoints2);
    return vec3(centerp,Xangle,Yangle);
}

float oneStep(float position_in_step){//[0,1]
    float bump_length = 0.4;
    float tallus_length = 0.2;
    float bump_height=0.05;

    //return 0.;
    if(position_in_step <0 || 1< position_in_step)//Not in expected space?
        return 0.;

    if(position_in_step < tallus_length)//Early tallus [0,0.3]
    {
        float linear_coef = (position_in_step)/tallus_length;
        return linear_coef;//2.0;//[-1;0]
    }
    else if(position_in_step > 1-bump_length){//end bump [0.3,0.4]

        float linear_coef = (position_in_step- (1-bump_length))/bump_length;
        return 1-sin(linear_coef * mathpi)*bump_height;//[0,+bump_height,0]

    }//middle: flat surface [0.4,1]
    return 1.;//[0]
}

float rice_banquet(float p, vec2 pente)
{
    //TODO REDO //DONE
    float seuil_m = -0.1;//Doit être > aux volcan == -0.4
    float seuil_M = 0.7;//0.5; // doit être < aux lac == 0.5
    float amplitude = 0.05;
    float angleMax = 0.2;
    float angleNorthMax = 0;
    float initialp= p;




    if((-angleMax < pente.x && pente.x < angleMax) && (-angleMax < pente.y && pente.y < angleNorthMax )){// Si la pente est bonne
        if((seuil_m < p) && (p < seuil_M)){
            //return p;//0.;

            float altitude_plateau = seuil_m-amplitude;
            int i=0;
            while( seuil_m < p ){ // COMPUTE plateau_number
                p -= amplitude; // - amplitude
                altitude_plateau += amplitude; // +1*amplitude >> same height?
                //i++;
            }
            p = p + amplitude - seuil_m;// p + amplitude - seuil_m [0, amplitude[
            float linear_coef = p/amplitude;// Appartient à [0,1[
            //if(altitude_plateau < seuil_M- amplitude)
                return altitude_plateau + oneStep(linear_coef)*amplitude;
            //return initialp;
        }
        //return pente.y;
    }
    return p;
}

void main() {
  vec3 motion = vec3(position_x,position_y,0.); // could be controlled via a global uniform variable
  float p = pnoise(pos+motion.xy,amplitude1,amplitude2,0.5,10)+motion.z;
  //vec3 pDxDy = perlinHeightandAngle(motion,0.5,10);
  //p += motion.z;
  //p = rice_banquet(p, pDxDy.yz); // ajout des rizières

  float val = p*0.5+0.5;
  val = min(val,0.75); // ajout de lacs

  if(val < 0.1){ // ajout des volcans
    val = 0.2-val;
    if(val > 0.15)
      val = 0.15;
  }

  outBuffer =vec4(val);
}

