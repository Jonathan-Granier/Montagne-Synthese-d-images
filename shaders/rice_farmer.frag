#version 330

out vec4 outBuffer;
uniform sampler2D height_and_normal_map;

in vec2 texcoord;


float value(in vec4 c) {

	// gradient of what:

	return c.x;// the height is stored in all channels (take the first one)

}


float rice_banquet(float p, vec3 n)
{
    float seuil_orientation=-0.15;
    float seuil_m = -0.15;
    float seuil_M = 0.5;
    float amplitude = 0.15;

    vec3 orientation = vec3(-1,0,0);
    float angle = orientation * n;

    if(angle > seuil_orientation && seuil_m <p && p< seuil_M)
        p = floor(p/amplitude)*amplitude;

    return p;
}

void main() {

        vec4 R = texture(height_and_normal_map, texcoord);

        vec3 n = R.xyz;
        float h = R.a;

        h = rice_banquet(h, n);


        outBuffer = vec4(h*0.5+0.5);

}

