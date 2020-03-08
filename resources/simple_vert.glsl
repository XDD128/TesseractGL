#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 fragNor;
out vec3 lightDir;
out vec3 viewDir;
void main()
{
	gl_Position = P * V * M * vertPos;
	fragNor = (M * vec4(vertNor, 0.0)).xyz;
	lightDir = lightPos - (M*vertPos).xyz;
	viewDir = viewPos - (M*vertPos).xyz;
}
