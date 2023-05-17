#pragma once

struct MassComponent
{
	float mass;
	float inv_mass;

	float inertia;
	float inv_inertia;

	void SetMassAndInertia(float mass)
	{
		SetMass(mass);
		SetInertia(mass);
	}

	void SetMass(float mass)
	{
		if (mass == INFINITY)
			SetInverseMass(0);

		this->mass = mass;
		this->inv_mass = 1 / mass;
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

	void SetInertia(float inertia)
	{
		if (inertia == INFINITY)
			SetInverseInertia(0);

		this->inertia = inertia;
		this->inv_inertia = 1 / inertia;
	}

	void SetInverseInertia(float inv_inertia)
	{
		this->inv_inertia = inv_inertia;

		if (inv_inertia != 0)
		{
			this->inertia = 1 / inv_inertia;
		}
		else
		{
			this->inertia = INFINITY;
		}
	}
};