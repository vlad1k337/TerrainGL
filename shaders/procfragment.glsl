#version 410 core

out vec4 FragColor;
in vec2 TexCoords;
 
uniform sampler2D screenTexture;

const vec2 offset = vec2(1.0/800.0, 1.0/600.0);

uniform float kernel[9];


void main()
{
	if(TexCoords.x == 0 || TexCoords.y == 0)
	{
		FragColor = vec4(0.0);
	}

	vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y), // top-left
        vec2( 0.0f,    offset.y), // top-center
        vec2( offset.x,  offset.y), // top-right
        vec2(-offset.x,  0.0f),   // center-left
		vec2(0.0, 0.0),
        vec2( offset.x,  0.0f),   // center-right
        vec2(-offset.x, -offset.y), // bottom-left
        vec2( 0.0f,   -offset.y), // bottom-center
        vec2( offset.y, -offset.y)  // bottom-right    
    );	
	
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
	vec4 color = vec4(col, 0.0);

	vec4 leftX = texture(screenTexture, TexCoords.st + vec2(-offset.x, 0));
	vec4 rightX = texture(screenTexture, TexCoords.st + vec2(+offset.x, 0));
	vec4 downY = texture(screenTexture, TexCoords.st + vec2(0, +offset.y));
	vec4 upY = texture(screenTexture, TexCoords.st + vec2(0, -offset.y));

	vec4 Lx = -0.5*leftX + 0.5*rightX;
	vec4 Ly = -0.5*downY + 0.5*upY;

	vec4 gradient = sqrt(Lx*Lx + Ly*Ly);

	//FragColor = mix(gradient, color, 0.1) + 0.1;
	//FragColor = atan(Ly, Lx) - 0.2;
	FragColor = color;
/*
	if(gradient.r < 0.1)
		FragColor = color;
	else
		FragColor = vec4(0.0);
*/
}  
