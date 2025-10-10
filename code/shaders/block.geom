#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 Normal[];
in vec3 FragPos[];
in vec2 TexCoords[];

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 offsetTransform;
uniform vec2 texDim;

float calcTexCoords(float norm) {
	return (0.5 * norm * norm) + (0.5 * norm);
}

void main() {
	vec4 offset = vec4(-(Normal[0].x * Normal[0].x) + 1, -(Normal[0].y * Normal[0].y) + 1, -(Normal[0].z * Normal[0].z) + 1, 0);
	vec4 offsetDiff = offsetTransform * vec4(Normal[0], 0);

	vec2 texCoordsOffset = vec2(calcTexCoords(Normal[0].x) + calcTexCoords(Normal[0].y) + calcTexCoords(-Normal[0].z), calcTexCoords(-Normal[0].x) + calcTexCoords(-Normal[0].y) + calcTexCoords(Normal[0].z));

	// Point 1
	fragPos = FragPos[0];
	normal = mat3(transpose(inverse(model))) * Normal[0];
	//texCoords = vec2(0.0, 0.0) + TexCoords[0];
	texCoords = vec2(1.0 / texDim.x, 1.0 / texDim.y) + TexCoords[0];
	//texCoords = vec2((Normal[0].x + Normal[0].y) / texDim.x, (Normal[0].x + Normal[0].y) / texDim.y) + TexCoords[0];
	vec4 position = gl_in[0].gl_Position - (0.5 * offset);
	gl_Position = projection * view * model * position;
	EmitVertex();

	// Point 2
	fragPos = FragPos[0];
	normal = mat3(transpose(inverse(model))) * Normal[0];
	//texCoords = vec2(0.0, 1.0 / texDim.y) + TexCoords[0];
	//texCoords = vec2(1.0 / texDim.x, 0.0) + TexCoords[0];
	texCoords = vec2(texCoordsOffset.x / texDim.x, texCoordsOffset.y / texDim.y) + TexCoords[0];
	position = gl_in[0].gl_Position + (0.5 * vec4(offsetDiff.xyz, 0));
	gl_Position = projection * view * model * position;
	EmitVertex();

	// Point 3
	fragPos = FragPos[0];
	normal = mat3(transpose(inverse(model))) * Normal[0];
	//texCoords = vec2(1.0 / texDim.x, 0.0) + TexCoords[0];
	//texCoords = vec2(0.0, 1.0 / texDim.y) + TexCoords[0];
	texCoords = vec2((-texCoordsOffset.x + 1) / texDim.x, (-texCoordsOffset.y + 1) / texDim.y) + TexCoords[0];
	position = gl_in[0].gl_Position - (0.5 * vec4(offsetDiff.xyz, 0));
	gl_Position = projection * view * model * position;
	EmitVertex();

	// Point 4
	fragPos = FragPos[0];
	normal = mat3(transpose(inverse(model))) * Normal[0];
	//texCoords = vec2(1.0 / texDim.x, 1.0 / texDim.y) + TexCoords[0];
	texCoords = vec2(0.0, 0.0) + TexCoords[0];
	//texCoords = vec2((Normal[0].z) / texDim.x, (Normal[0].z) / texDim.y) + TexCoords[0];
	position = gl_in[0].gl_Position + (0.5 * offset);
	gl_Position = projection * view * model * position;
	EmitVertex();

	EndPrimitive();
}