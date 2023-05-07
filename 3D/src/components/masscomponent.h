#pragma once

struct MassComponent
{
	float mass;
	float inv_mass;

	void SetMass(float mass)
	{
		this->mass = mass;
		this->inv_mass = 1 / mass;
	}

	void SetInverseMass(float inv_mass)
	{
		this->inv_mass = inv_mass;

		if (inv_mass != 0)
			this->mass = 1 / inv_mass;
		else
			this->mass = INFINITY;
	}
};