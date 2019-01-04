#version 430 core

// Ouput data
out float depth;

void main(){

	depth = gl_FragCoord.z;
}