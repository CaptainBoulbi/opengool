#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main(){
	gl_Position = vec4(aPos, 0.0, 1.0);
	ourColor = vec3(0.2, 0.3, 0.4);
	TexCoord = aTexCoord;
}

//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTexCoord;
//
//out vec3 ourColor;
//out vec2 TexCoord;
//
//void main()
//{
//    gl_Position = vec4(aPos, 1.0);
//    ourColor = aColor;
//    TexCoord = aTexCoord;
//}
