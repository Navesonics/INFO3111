#version 110

varying vec3 color;

uniform vec3 colorOverrideRGB;

void main()
{
//	gl_FragColor = vec4(color, 1.0);
	gl_FragColor = vec4(colorOverrideRGB, 1.0);
}
