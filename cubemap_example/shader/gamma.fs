#version 330 core

in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float gamma;

void main() {
  vec4 pixel = texture(tex, texCoord);
  if (pixel.a < 0.01) discard;
  fragColor = vec4(pow(pixel.rgb, vec3(gamma)), 1.0);
}