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
};