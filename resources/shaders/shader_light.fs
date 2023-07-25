#version 450 core
out vec4 FragColor;

struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;


//Material
in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;
in float Shininess;
in float Refracti;

uniform vec3 viewPos;
uniform Light light;
uniform bool select;
uniform vec3 color;
uniform bool isColorMode;

uniform sampler2D texture_diffuse1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
    // ambient
    vec3 ambient = light.ambient * Diffuse.rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =light.diffuse * diff *Diffuse.rgb;  
      
    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = light.specular * spec *   Specular.rgb;  
	
    //ambient  *= attenuation;  
    diffuse   *= attenuation; 
    specular *= attenuation; 
	  
    vec3 result = ambient + diffuse +specular;

    if (select) {
    	result = vec3(0.8, 0.0, 0.0);
    }

    if (isColorMode) {
        result = color;
    }
    
    FragColor = vec4(result ,1.0);
}