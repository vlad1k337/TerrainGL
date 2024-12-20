#version 410 core
precision highp float;

in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D uHeightMap;

uniform float redComponent;
uniform float greenComponent;
uniform float blueComponent;
uniform float brightness;
uniform float shininess;

uniform vec3 cameraPos;
uniform mat4 model;

uniform bool gamma;
uniform bool gooch;

vec3 albedo = vec3(redComponent, greenComponent, blueComponent) + brightness;
uniform float metallic;
uniform float roughness;
uniform float ambientOcclusion;

const float PI = 3.141592;

vec3 goochDiffuse(vec3 normal, vec3 objectColor, vec3 cool, vec3 warm)
{
	vec3 gcool = (1 + dot(objectColor, normal))/2 * cool;
	vec3 gwarm = (1 - (1 + dot(objectColor, normal))/2) * warm;

	return gcool + gwarm;
}

vec3 fresnel(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 calculateLight()
{
	vec3 x = dFdx(FragPos);
	vec3 y = dFdy(FragPos);
	vec3 normal   = inverse(mat3(model)) * normalize(cross(x, y));

	vec3 lightPos   = vec3(0.0, 150.0, 0.0);
	vec3 lightColor = vec3(23.47, 21.31, 20.79);	

	vec3 view     = normalize(cameraPos - FragPos);
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 halfway  = normalize(view + lightDir);

	float incAngle  = max(dot(normal, lightDir), 0.0);
	vec3 radiance   = lightColor * incAngle;

	// basic reflectance
	// https://refractiveindex.info/
	vec3 F0 = vec3(0.4);
	F0      = mix(F0, albedo, metallic);
	vec3 F  = fresnel(max(dot(halfway, view), 0.0), F0);

	vec3 reflectedRatio = F;
	vec3 refractedRatio = vec3(1.0) - F;

	// metallic surfaces do not refract
	refractedRatio *= 1.0 - metallic; 

	// normal distribution function 
	float NDF = DistributionGGX(normal, halfway, roughness);

	// microfacets overshadowing
	float G = GeometrySmith(normal, view, lightDir, roughness);

	vec3  num   = NDF * G * F;
	float denom = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, lightDir), 0.0f);

	vec3 specular = num / denom;
	vec3 ambient = vec3(0.03) * albedo * ambientOcclusion;  

	if(gooch)
		albedo = goochDiffuse(normalize(normal), vec3(1.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));

	vec3 irradiance = (refractedRatio * albedo / PI + specular) * radiance * incAngle;
	return ambient + irradiance;
}

void main()
{	
	vec3 surfaceColor = calculateLight();
	surfaceColor = surfaceColor / (surfaceColor + vec3(1.0));	

	if(gamma)
		surfaceColor = pow(surfaceColor, vec3(1.0/2.2));

	FragColor = vec4(surfaceColor, 1.0); 
}
