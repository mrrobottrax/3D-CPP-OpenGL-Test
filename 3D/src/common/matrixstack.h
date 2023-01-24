#pragma once

class MatrixStack
{
public:
	void push(glm::mat4&);
	void push();
	void pushCpy();
	void pop();
	glm::mat4& top();

	void translate(glm::vec3);
	void applyMatrix(glm::mat4);

private:
	std::stack<glm::mat4> mStack;
};