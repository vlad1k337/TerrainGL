#version 410 core
 
layout (quads, fractional_odd_spacing, ccw) in;
 
uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

uniform float frequency;
uniform float amplitude;
uniform int octaves;

in vec2 TextureCoord[];
out vec3 FragPos;
out vec3 normal;
 
vec4 p00 = gl_in[0].gl_Position;
vec4 p01 = gl_in[1].gl_Position;
vec4 p10 = gl_in[2].gl_Position;
vec4 p11 = gl_in[3].gl_Position;
 
vec2 t00 = TextureCoord[0];
vec2 t01 = TextureCoord[1];
vec2 t10 = TextureCoord[2];
vec2 t11 = TextureCoord[3];

uint hash(uint x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    uint k = x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

uint hash(uvec2 x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    uint k = x.x; 
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    k = x.y; 
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

vec2 gradientDirection(uint hash)
{
    switch (int(hash) & 3) 
    { 
    case 0:
        return vec2(1.0, 1.0);
    case 1:
        return vec2(-1.0, 1.0);
    case 2:
        return vec2(1.0, -1.0);
    case 3:
        return vec2(-1.0, -1.0);
     
    }
}

float fade(float t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float dfade(float t)
{
	return 30.0 * t * t * (t * (t - 2.0) + 1.0);
}

vec3 noise(vec2 uv, uint seed)
{
	vec2 floorCorner = floor(uv);
	uvec2 boxCorner = uvec2(floorCorner);
	vec2 inCoord = fract(uv);

	float u = fade(inCoord.x);
	float v = fade(inCoord.y);

	vec2 ga = gradientDirection(hash(boxCorner, seed));
	vec2 gb = gradientDirection(hash(boxCorner + uvec2(1.0, 0.0), seed));
	vec2 gc = gradientDirection(hash(boxCorner + uvec2(0.0, 1.0), seed));
	vec2 gd = gradientDirection(hash(boxCorner + uvec2(1.0, 1.0), seed));

	float a = dot(ga, inCoord);	
	float b = dot(gb, inCoord - uvec2(1.0, 0.0));	
	float c = dot(gc, inCoord - uvec2(0.0, 1.0));	
	float d = dot(gd, inCoord - uvec2(1.0, 1.0));	

	float x1 = mix(a, b, u);
	float x2 = mix(c, d, u);

	vec2 fu = vec2(fade(inCoord.x), fade(inCoord.y));
	vec2 du = vec2(dfade(inCoord.x), dfade(inCoord.y));
	
	float dx = (mix(c, d, fu.x) - mix(a, b, fu.x)) * du.y;
	float dy = mix((b - a) * du.x, (d - c) * du.x, fu.y);

	return vec3(mix(x1, x2, v),
		ga + fu.x * (gb - ga) + fu.y * (gc - ga) + fu.x * fu.y * (ga - gb - gc + gd) +  
                 du * (fu.yx*(a - b - c + d) + vec2(b, c) - a));
}

float summedNoise(vec2 uv, uint seed, float frequency, float amplitude, uint octaves)
{
	vec2 d = vec2(0.0, 0.0);
	float total = 0.0;
	for(uint i = 0; i < octaves; ++i)
	{
		vec3 n = noise(uv * frequency, seed) * amplitude;
		d += vec2(n.y, n.z);
		total += n.x;

		frequency *= 2.0;
		amplitude *= 0.5;
	}
	// analytical normals doesn't work and i have no idea how to fix them..
	normal = vec3(d.x, 1.0, d.y);
	return total;
}

vec4 getPos(float u, float v)
{
	vec4 p0 = mix(p00, p01, u);
	vec4 p1 = mix(p10, p11, u);
	return mix(p0, p1, v);
}

vec2 getTex(float u, float v)
{	
	vec2 t0 = mix(t00, t01, u);
	vec2 t1 = mix(t10, t11, u);
	return mix(t0, t1, v);
}

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p  = getPos(u, v);
	vec2 t  = getTex(u, v);

	p.y = summedNoise(t, 0xDEFECA7E, frequency, amplitude, octaves) * 20.0;
	
	gl_Position = projection * view * model * p;

	FragPos = vec3(model * p);
}
