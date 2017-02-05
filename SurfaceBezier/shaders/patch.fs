#version 420 core

layout(binding=0) uniform sampler2D mainTexture;
out vec4 color;


in vec3 normal;
in vec4 positionOut;
in vec2 texCoords;

uniform vec3 light_direction;
uniform vec3 pos_lights[2];
uniform int display_normal;
uniform int display_texture;
uniform mat4 model_matrix;
uniform vec3 cam_pos;

vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
vec3 specular_albedo = vec3(1.0);
float specular_power = 32.0;

vec3 calcPointLight(int i)
{
	vec3 colorOut;

	vec3 light_dir = vec3(pos_lights[i] - vec3(positionOut));
	light_dir = normalize(light_dir);

	//diffuse
	vec3 diffuse = max(dot(normal, light_dir), 0.0) * diffuse_albedo;

	//specular
	vec3 specular;
    float intensity = max(dot(normal,light_dir), 0.0);
	vec3 camToPos = normalize(vec3(positionOut) - cam_pos);
	vec3 reflectDir = normalize(reflect(-light_dir, normal));
	if (intensity > 0.0) 
		specular = specular_albedo * pow(max(dot(reflectDir,camToPos), 0.0), specular_power);

	float dist = length(light_dir);
	float attenuation = 1.0 / (1.0 + 1.0 * pow(dist, 2));

	colorOut = diffuse * attenuation + specular * attenuation;
	return colorOut;
}

void main(void)
{
    vec3 normal = normalize(normal);
	normal = vec3(model_matrix * vec4(normal, 1.0));
	
	vec3 colorAcc = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i <2 ; i++)
	{
		colorAcc += calcPointLight(i);
	}

	if(display_normal == 1)
		color = vec4(normal, 1.0);
	else if(display_texture == 1)
		color = texture(mainTexture, texCoords);
	else
		color = vec4(colorAcc, 1.0);
}
