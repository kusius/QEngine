#version 330 core
//Struct for the material properties
struct Material 
{
	//3 types of colors
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	//a shininess value
	float shininess;
};

struct Light
{
	vec3 position;

	//same color intensities for the light probe whi illuminates our object
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 lightColor; //not used rightno
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;





void main()
{
	//ambient light calculation
	
	vec3 ambient =  light.ambient * material.ambient;

	//diffuse light calculation
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - FragPos);

	//scalar on what to scale the lightColor base on angle of impact
	//use max function to avoid values greater than 90 degrees
	float diff = max(dot(norm, lightDirection), 0.0); 

	vec3 diffuse = light.diffuse *  (diff * material.diffuse) ;


	//specular
	float specularStrength = 0.1;
	vec3 viewDirection = normalize(viewPos - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);//the power raised is the shininess
	vec3 specular = light.specular * (spec * material.specular);  


	vec3 result =  ambient + diffuse + specular   ;




	color =  vec4(result, 1.0) ;
}