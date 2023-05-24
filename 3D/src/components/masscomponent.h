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
		SetInertiaFromMass(mass);
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

	// TODO: Inertia tensor is in reality much more complicated
	void SetInertiaFromMass(float mass)
	{
		float inertia = mass / 24.0f;

		if (mass == INFINITY)
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