#version 420 core

out vec4 color;

in TES_OUT
{
    vec3 normal;
} fs_in;

uniform vec3 posLights[2];
uniform vec3 colorLights[2];
uniform int display_normal;

vec3 posLightTransformed[2];

uniform mat4 view;
in vec4 positionOut;

vec4 spec = vec4(0.0);
vec4 ambient = vec4(0.5, 0.5, 0.5, 1.0);
vec4 diffuse = vec4(0.8, 0.8, 0.8, 1.0);
vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);
float shininess = 32;

void main(void)
{
    vec3 normal = normalize(fs_in.normal);

    vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * normal.z +
             vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < 1; i++)
	{
		posLightTransformed[i] = vec3(view * vec4(posLights[i], 1.0));
		vec3 light_dir = vec3(posLightTransformed[i] - vec3(positionOut));
		vec3 eye = vec3(-positionOut);
		float intensity = max(dot(normal,light_dir), 0.0);
		if (intensity > 0.0) 
		{
			vec3 h = normalize(light_dir + eye);
			float intSpec = max(dot(h,normal), 0.0);
			spec = specular * pow(intSpec, shininess);
		}
	}

	if(display_normal == 1)
		color = vec4(normal, 1.0);
	else
		color = (ambient + spec) * clamp(c, vec4(0.1), vec4(1.0));
}
