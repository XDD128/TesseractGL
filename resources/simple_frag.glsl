#version 330 core 
in vec3 fragNor;
out vec4 color;


uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

vec3 lightCol = vec3(1, 1, 1);
in vec3 viewDir;
in vec3 lightDir;


void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 v = normalize(viewDir);
	vec3 hf = normalize(light+v);

	color = vec4(0.1*MatAmb + MatDif*(max(0, dot(normal, light)))*lightCol + MatSpec*(pow(dot(normal,hf),shine))*lightCol, 1.0);
}
