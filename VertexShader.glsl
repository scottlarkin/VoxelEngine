#version 330

uniform mat4 projection_view_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 texCoords;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_texCoords;

void main()
{ 
	varying_position = vec3(model_xform * vec4(vertex_position,1));
	varying_texCoords = texCoords;
	varying_normal = vec3(model_xform * vec4(vertex_normal,0));

	gl_Position =  projection_view_xform  * model_xform * vec4(vertex_position, 1 );
}	 