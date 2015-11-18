#version 330




//uniform sampler2D sampler;

out vec4 fragment_colour;

//in vec2 varying_texCoords;

void main()
{ 	
	
	//vec3 tex_colour  = texture(sampler, varying_texCoords).rgb;
	vec3 tex_colour = vec3(1,1,1);
		
	fragment_colour = vec4(  tex_colour ,1.0);

}