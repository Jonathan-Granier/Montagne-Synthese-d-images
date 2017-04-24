#version 330

out vec4 outBuffer;
uniform sampler2D height_and_normal_map;

in vec2 texcoord;


float value(in vec4 c) {

	// gradient of what:

	return c.x;// the height is stored in all channels (take the first one)

}

vec3 rainbow_color(float p)
{
    float step_number = 6.;
    float step = 1./step_number;
    float linear_factor;
    int color_step =0;

    vec3 Rouge = vec3(1,0,0);
    vec3 Jaune = vec3(1,1,0);
    vec3 Vert = vec3(0,1,0);
    vec3 Cyan = vec3(0,1,1);
    vec3 Bleu = vec3(0,0,1);
    vec3 Violet = vec3(1,0,1);


    if(p<0)
        p = -p;

    while(p>step){
        p -= step;
        color_step ++;
    }
    linear_factor = p / step;

    switch(color_step){
    case 0:
        return Rouge * (1-linear_factor) + Jaune * linear_factor;
        break;
    case 1:
        return Jaune * (1-linear_factor) + Vert * linear_factor;
        break;
    case 2:
        return Vert * (1-linear_factor) + Cyan * linear_factor;
        break;
    case 3:
        return Cyan * (1-linear_factor) + Bleu * linear_factor;
        break;
    case 4:
        return Bleu * (1-linear_factor) + Violet * linear_factor;
        break;
    case 5:
        return Violet * (1-linear_factor) + Rouge * linear_factor;
        break;
    }


}

vec3 disco_sunlight(vec2 coord)
{
    vec2 focal_center = vec2(0,0);
    float seuil_uniform_light = 0.2;
    float transition_uniform_to_rainbow = 0.1;
    float seuil_rainbow = seuil_uniform_light + transition_uniform_to_rainbow;
    float rainbow_length = 0.2;
    float seuil_grid = seuil_rainbow + rainbow_length;
    float grid_amplitude = 0.3;

    float X = coord.x - focal_center.x;
    float Y = coord.y - focal_center.y;
    float distance_to_focal = sqrt(X*X + Y*Y);

    if(distance_to_focal < seuil_uniform_light)
        return vec3(1,1,1);

    if(distance_to_focal < seuil_rainbow)
        float linear_factor = (distance_to_focal - seuil_uniform_light)/transition_uniform_to_rainbow;
        return vec3(1,1,1)*(1-linear_factor) + vec3(1,0,0) * linear_factor;

    if(distance_to_focal < seuil_grid)
        float linear_factor = (distance_to_focal - seuil_rainbow)/rainbow_length;
        return rainbow_color(linear_factor);

    else
        return rainbow_color(distance_to_focal);
}

void main() {
        vec3 color = disco_sunlight(texcoord);

        outBuffer = vec4(color,1);

}

