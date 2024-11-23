#version 410 core

uniform float redComponent;
uniform float greenComponent;
uniform float blueComponent;

uniform float brightness;

uniform float heightScale;
uniform float colorOffset;

in float Height;
out vec4 FragColor;

void main()
{
	float h = (Height + colorOffset)/heightScale;
	h *= brightness;
	FragColor = vec4(h*redComponent, h*greenComponent, h*blueComponent, 1.0f);
}
