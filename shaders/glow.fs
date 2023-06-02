#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform vec3 spriteColor;
uniform float alpha;

uniform vec2 point1, point2;
uniform int glowMode; // 0 = none, 1 = circle, 2 = line, 3 = rectangle
uniform float radius;
uniform float aspectRatio;

float dist_pt_line(vec2 point, vec2 pt1, vec2 pt2)
{
   vec2 v1 = pt2 - pt1;
   vec2 v2 = pt1 - point;
   vec2 v3 = vec2(v1.y,-v1.x);
   return abs(dot(v2,normalize(v3)));
}

void main()
{
  float dist;
  if(glowMode == 0){
    discard;
  }else if(glowMode == 1){
    // distance from point
    dist = distance(TexCoords, point1);
  }else if(glowMode == 2){
    // distance from line
    dist = dist_pt_line(TexCoords, point1, point2);
  }else if(glowMode == 3){
    // distance from rectangle
    if(TexCoords.x < point1.x)
      dist = distance(TexCoords, point1);
    else if(TexCoords.x > point2.x)
      dist = distance(TexCoords, point2);
    else 
      dist = dist_pt_line(TexCoords, point1, point2);
  }
    color = vec4(spriteColor,alpha) * smoothstep(radius,0.0, dist);
}  
