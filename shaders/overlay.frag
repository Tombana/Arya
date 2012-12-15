#version 140

uniform sampler2D texture1;

in vec2 texCoo;
layout (location = 0) out vec4 fragColor;


vec4 overlayColor(vec2 tex)
{
    return texture(texture1,texCoo);
}


void main()
{
    fragColor = overlayColor(texCoo);
}
