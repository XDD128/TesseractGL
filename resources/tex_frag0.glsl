#version 330 core
uniform sampler2D Texture0;
uniform vec3 MatDif;

in vec2 vTexCoord;
in float dCo;
out vec4 Outcolor;

void main() {
  vec4 texColor0 = texture(Texture0, vTexCoord);

  vec4 color = dCo*texColor0 + 0.2*texColor0;
Outcolor = color;
  
  

	//uncomment later on: Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

