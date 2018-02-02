std::string voronoiVertex = R"(#version 400 core
layout(location = 0) in vec3 VertPosition;
layout(location = 1) in vec2 ConePosition;
layout(location = 2) in vec3 ConeColor;

out vec3 VertColor;


const float height = 1.99f;

const mat4 projection = mat4(2.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, -2.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, -1.0f, 0.0f,
                             -1.0f, 1.0f, 0.0f, 1.0f);

void main()
{
	VertColor = ConeColor;
	gl_Position = projection * vec4(VertPosition.xy + ConePosition, VertPosition.z + (1.0 - height), 1.0f);
})";
