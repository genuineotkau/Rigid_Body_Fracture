#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform Mat {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ni;
	float Ns;
};

out vec3 FragPos;
out vec3 Normal;

out vec4 Ambient;
out vec4 Diffuse;
out vec4 Specular;
out float Shininess;
out float Refracti;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    Ambient = Ka;
	Diffuse = Kd;
	Specular = Ks;
	Shininess = Ni;
	Refracti = Ns;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}