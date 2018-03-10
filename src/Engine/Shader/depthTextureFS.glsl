#version 430 core

// Ouput data
out float depth;
in vec3 Position_worldspace;

uniform vec3 LightPosition_worldspace;
uniform int isPointLight;

void main(){
	if(isPointLight > 0)
	{
		depth = length(LightPosition_worldspace - Position_worldspace);
	}
	else{
		depth = gl_FragCoord.z;
	}
}