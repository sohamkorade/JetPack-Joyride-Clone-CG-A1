#version 330 core
// in vec2 TexCoords;
in vec4 vertex2;
out vec4 color;

uniform sampler2DArray image;
uniform vec3 spriteColor;
uniform int spriteIndex;
uniform float alpha;


void main()
{    
  vec2 TexCoords = vertex2.zw;
  color = vec4(spriteColor, alpha) * texture(image, vec3(TexCoords, spriteIndex));
  if (color.a < 0.1)
    discard;
}  