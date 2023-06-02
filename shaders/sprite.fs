#version 330 core
// in vec2 TexCoords;
in vec4 vertex2;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float alpha;

void main()
{    
  vec2 TexCoords = vertex2.zw;
  color = vec4(spriteColor, alpha) * texture(image, TexCoords);
  // if (color.a < 0.1)
  //   discard;
}  