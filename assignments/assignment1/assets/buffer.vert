#version 450

out vec2 TexCoord ;

vec4 vertices[3] =
{
	vec4(-1.0, -1.0, 0.0, 0.0),
	vec4(3.0, -1.0, 2.0, 0.0),
	vec4(-1.0, 3.0, 0.0, 2.0)
};	

void main()
{
	gl_Position = vec4(vertices[gl_VertexID].xy,0,1);
	TexCoord = vertices[gl_VertexID].zw;;
}

