std::string voronoiFragment = R"(#version 400 core

in vec3 VertColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(VertColor, 1);
})";
