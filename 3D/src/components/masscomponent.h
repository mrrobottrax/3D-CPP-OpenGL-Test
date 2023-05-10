#pragma once

struct MassComponent
{
	float mass;
	float inv_mass;

	float inertia;
	float inv_inertia;

	void SetMass(float mass)
	{
		if (mass == INFINITY)
			SetInverseMass(0);

		this->mass = mass;
		this->inv_mass = 1 / mass;

		this->inertia = mass;
		this->inv_inertia = 1 / mass;
	}

	void SetInverseMass(float inv_mass)
	{
		this->inv_mass = inv_mass;
		this->inv_inertia = inv_mass;

		if (inv_mass != 0)
		{
			this->mass = 1 / inv_mass;
			this->inertia = 1 / inv_inertia;
		}
		else
		{
			this->mass = INFINITY;
			this->inertia = INFINITY;
		}
	}
};