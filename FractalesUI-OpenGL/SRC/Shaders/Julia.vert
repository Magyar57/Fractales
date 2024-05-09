#version 330 core

layout (location = 0) in vec3 pos;

out vec3 vert_pos;

void main(){
	vert_pos = pos;
	
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
