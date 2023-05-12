#pragma once

class MatrixStack
{
public:
	void Push(glm::mat4&);
	void Push();
	void PushCpy();
	void Pop();
	glm::mat4& Top();

	void Translate(glm::vec3);
	void Scale(glm::vec3);
	void ApplyMatrix(glm::mat4);
	void Invert();

private:
	std::stack<glm::mat4> mStack;
};