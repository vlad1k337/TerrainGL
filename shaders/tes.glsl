#version 410 core
 
layout (quads, fractional_odd_spacing, ccw) in;
 
uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
uniform float heightScale;
uniform float heightOffset;

uniform vec2 uTexelSize; 

in vec2 TextureCoord[];
 
out float Height;

 
vec4 p00 = gl_in[0].gl_Position;
vec4 p01 = gl_in[1].gl_Position;
vec4 p10 = gl_in[2].gl_Position;
vec4 p11 = gl_in[3].gl_Position;
 
vec2 t00 = TextureCoord[0];
vec2 t01 = TextureCoord[1];
vec2 t10 = TextureCoord[2];
vec2 t11 = TextureCoord[3];
 
vec2 getTex(float u, float v)
{
	vec2 t0 = mix(t00, t01, u);
	vec2 t1 = mix(t10, t11, u);
	vec2 t  = mix(t0, t1, v);
	return t;
}
 
float getHeight(float u, float v)
{
	vec2 t0 = mix(t00, t01, u);
	vec2 t1 = mix(t10, t11, u);
	vec2 t  = mix(t0, t1, v);
	return texture(heightMap, t).y * heightScale - heightOffset;
}
 
vec4 getPos(float u, float v)
{
	vec4 n0 = mix(p00, p01, u);
	vec4 n1 = mix(p10, p11, u);
	vec4 n  = mix(n0, n1, v);
 
	return vec4(n.x, getHeight(u, v), n.zw);
}
 
void main()
{
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	vec2 TexCoord = getTex(u, v);
	
    vec4 p = getPos(u, v);
	Height = p.y;

	gl_Position = projection * view * model * p;
}
