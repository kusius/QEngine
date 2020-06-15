#version 330 core
//Light Structures
struct DirLight 
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	//same color intensities for the light probe whi illuminates our object
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutoff;
	float outercutoff;
};

//Material Struture
struct Material 
{
	//3 types of colors
	sampler2D texture_diffuse1;//diffuse map (texture) 
	sampler2D texture_diffuse2;//diffuse map (texture) 
	sampler2D texture_specular1;//specular map (texture)
	sampler2D texture_specular2;//specular map (texture)
	sampler2D texture_emission1; //emission map (texture)
	//a shininess value
	float shininess;
};

//Inputs from vertex shader
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

//Output (fragment's color)
out vec4 color;


//Input from program (uniforms)
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform Material material;


//function protos
vec3 CalcDirLight (DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);


void main()
{
	//we dont use this now because Calc*Light functions are using the 
	//diffuse and specular maps internally
	//color = texture(material.texture_diffuse1, TexCoords);
	
	
	//properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result  = vec3(0);
	// phase 1: Directional lighting
	result += CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
       result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
	//phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
	//phase4: emission does not need lighting calculation. Add to result as is
	vec3 emission = texture(material.texture_emission1, TexCoords).rgb;
	result += emission;




    color = vec4(result, 1.0f);
	
}

vec3 CalcDirLight (DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

	//ambient += light.ambient  * vec3(texture(material.texture_diffuse2, TexCoords));
	//diffuse += light.diffuse  * diff * vec3(texture(material.texture_diffuse2, TexCoords));
	//specular = light.specular * spec * vec3(texture(material.texture_specular2, TexCoords));
    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	//ambient += light.ambient  * vec3(texture(material.texture_diffuse2, TexCoords));
	//diffuse += light.diffuse  * diff * vec3(texture(material.texture_diffuse2, TexCoords));
	//specular = light.specular * spec * vec3(texture(material.texture_specular2, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	//angles
	vec3 lightDirection = normalize(light.position - fragPos);

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = clamp((theta - light.outercutoff)/epsilon, 0.0, 1.0);

	//ambient light calculation
	
	vec3 ambient =  light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

	//diffuse light calculation
	//scalar on what to scale the lightColor base on angle of impact
	//use max function to avoid values greater than 90 degrees
	float diff = max(dot(normal, lightDirection), 0.0); 

	vec3 diffuse = light.diffuse *  (diff * vec3(texture(material.texture_diffuse1, TexCoords))) ;
	diffuse *= intensity;

	//specular
	float specularStrength = 0.1;
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);//the power raised is the shininess
	vec3 specular = light.specular * (spec * vec3(texture(material.texture_specular1, TexCoords)));  
	specular *= intensity;

	return (ambient + diffuse + specular) ;
}
