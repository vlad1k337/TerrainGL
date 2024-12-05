#version 410 core

uniform sampler2D uHeightMap;

uniform float redComponent;
uniform float greenComponent;
uniform float blueComponent;
uniform float brightness;
uniform float shininess;

uniform vec3 cameraPos;
uniform mat4 model;

in vec3 FragPos;

out vec4 FragColor;

// just in case i would need to debug normals. again.
vec3 normal;

vec3 calculateLight()
{
	vec3 x = dFdx(FragPos);
	vec3 y = dFdy(FragPos);
	normal = inverse(mat3(model)) * normalize(cross(x, y));

	vec3 lightColor = vec3(redComponent, greenComponent, blueComponent) + brightness;
	vec3 lightPos   = vec3(0.0, 100.0, -250.0);

	vec3 lightDir   = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float diff = max(dot(normalize(normal), lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	float specularStrength = 0.5;
	vec3 specular = specularStrength * spec * lightColor;

	return ambient + diffuse + specular;
}

void main()
{	
	vec3 surfaceColor = vec3(1.0) * calculateLight();
	FragColor = vec4(surfaceColor, 1.0);
	
	//FragColor = vec4(normal.xyz, 1.0); 
}
