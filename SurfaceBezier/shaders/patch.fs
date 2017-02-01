#version 420 core

out vec4 color;

in TES_OUT
{
    vec3 normal;
} fs_in;

in vec4 positionOut;

uniform vec3 light_direction;
uniform vec3 posLights[2];
uniform vec3 colorLights[2];
uniform int display_normal;
uniform mat4 view;
uniform mat4 model_matrix;

vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
vec3 specular_albedo = vec3(1.0);
float specular_power = 200.0;

vec3 calcPointLight()
{
	vec3 color;
	return color;
}

void main(void)
{
    vec3 normal = normalize(fs_in.normal);
	normal = vec3(model_matrix * vec4(normal, 1.0));
    /*vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * normal.z +
             vec4(0.0, 0.0, 0.0, 1.0);
	for(int i = 0; i < 1; i++)
	{
		vec3 light_dir = vec3(posLights[i] - vec3(positionOut));
	}*/

	//vec4 pos = view * positionOut;
	vec3 light_dir = vec3(posLights[0] - vec3(positionOut));

	//diffuse
	vec3 diffuse = max(dot(normal, light_dir), 0.0) * diffuse_albedo;

	//specular
	vec3 specular;
	vec3 eye = vec3(-positionOut);
    float intensity = max(dot(normal,light_dir), 0.0);
	if (intensity > 0.0) 
	{
		vec3 h = normalize(light_dir + eye);
		specular = specular_albedo * pow(max(dot(h,normal), 0.0), specular_power);
	}

	float dist = length(light_dir);
	float attenuation = 1.0 / (1.0 + 1.0 * pow(dist, 2));

	if(display_normal == 1)
		color = vec4(normal, 1.0);
	else
		color = vec4(diffuse * attenuation + specular * attenuation, 1.0);
}
