#version 400 core
layout(location = 0) in vec3 VertPosition;
layout(location = 1) in vec2 ConePosition;
layout(location = 2) in vec3 ConeColor;

out vec3 VertColor;

uniform mat4 view;
uniform mat4 projection;

const float height = 1.99f;

void main()
{
	VertColor = ConeColor;
	gl_Position = projection * view * vec4(VertPosition.xy + ConePosition, VertPosition.z + (1.0 - height), 1.0f);
}
