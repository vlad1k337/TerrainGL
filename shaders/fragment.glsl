#version 410 core

uniform sampler2D uHeightMap;
uniform float redComponent;
uniform float greenComponent;
uniform float blueComponent;

uniform float brightness;

uniform float heightScale;
uniform float heightOffset;

uniform vec2 uTexelSize;

in float Height;
out vec4 FragColor;

void main()
{	
	float h = (Height + heightOffset)/heightScale;
	FragColor = h * vec4(1.0f + brightness);
}
