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

vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
vec3 specular_albedo = vec3(0.7);
float specular_power = 200.0;

void main(void)
{
    vec3 normal = normalize(fs_in.normal);

    /*vec4 c = vec4(1.0, -1.0, 0.0, 0.0) * normal.z +
             vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < 1; i++)
	{
		vec3 light_dir = vec3(posLights[i] - vec3(positionOut));
		vec3 eye = vec3(-positionOut);
		float intensity = max(dot(normal,light_dir), 0.0);
		if (intensity > 0.0) 
		{
			vec3 h = normalize(light_dir + eye);
			float intSpec = max(dot(h,normal), 0.0);
			spec = specular * pow(intSpec, shininess);
		}

		if(display_normal == 1)
			color = vec4(normal, 1.0);
		else
			color += (ambient + spec) * diffuse;// clamp(c, vec4(0.1), vec4(1.0));
	}*/
	
	vec3 light_dir = normalize(vec3(posLights[0] - vec3(positionOut)));
	vec3 view = normalize(-positionOut.xyz);

	vec3 H = normalize(light_direction + view);
	vec3 diffuse = max(dot(normal, light_direction), 0.0) * diffuse_albedo;
    vec3 specular = pow(max(dot(normal, H), 0.0), specular_power) * specular_albedo;

    // Write final color to the framebuffer
    color = vec4(diffuse + specular, 1.0);
}
