#version 330


struct Light
{
    vec3 position;
    vec3 direction;
    float field_of_view;
    float range;
};

uniform Light light_source;
uniform vec3 camera_position;
uniform sampler2D sampler;

uniform vec3 skyColour;

out vec4 fragment_colour;

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texCoords;

void main()
{ 	
	vec3 diffuse= vec3(0,0,0);
	vec3 specular= vec3(0,0,0);

	//vec3 tex_colour  = texture(sampler, varying_texCoords).rgb;
	vec3 tex_colour = vec3(	1,0,0);
	vec3 ambient = vec3(0.29,0.59,0.f);

	vec3 LightMinusPos = light_source.position - varying_position;

	vec3 L = normalize(LightMinusPos);
	float spotEffect = dot(L, normalize(-light_source.direction));
	float LdotN = dot(varying_normal,L);
	float distance = length(LightMinusPos);
	float distance_attenuation = smoothstep(light_source.range, 1, distance );
	float ConeFovInRads = light_source.field_of_view * 0.0174532925; 
	float cone_attenuation = smoothstep(cos(ConeFovInRads ), 1 ,spotEffect);
	float attenuation = distance_attenuation * cone_attenuation;


	if(spotEffect > cos(ConeFovInRads))
	{
		diffuse += (clamp(LdotN,0,1) * (vec3(0.8, 0.8,0.8) * attenuation));
	}


    float distanceFromPoint = length(camera_position - varying_position);
	

	vec3 fog = skyColour;


	//fragment_colour = vec4( mix(ambient, fog,smoothstep(20,200,distanceFromPoint)) ,1.0);
	fragment_colour = vec4(ambient + diffuse ,1.0);

}