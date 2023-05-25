#pragma once

struct MassComponent
{
	float mass;
	float inv_mass;

	glm::vec3 inertia;
	glm::vec3 inv_inertia;

	void SetMass(float mass)
	{
		if (mass == INFINITY)
			SetInverseMass(0);

		this->mass = mass;
		this->inv_mass = 1 / mass;

		inertia = glm::vec3(1 / 24.0f);
		inv_inertia = glm::vec3(24.0f);
	}

	void SetInverseMass(float inv_mass)
	{
		this->inv_mass = inv_mass;

		if (inv_mass != 0)
		{
			this->mass = 1 / inv_mass;
		}
		else
		{
			this->mass = INFINITY;
		}
	}
};