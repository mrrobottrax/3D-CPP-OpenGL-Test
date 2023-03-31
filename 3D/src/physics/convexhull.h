#pragma once

struct Face
{
	float normal[3];
	float dist;
};

class ConvexHull
{
public:
	ConvexHull() : faceCount(0), faces(nullptr)
	{
	};
	ConvexHull(unsigned short faceCount, Face* faces)
	{
		this->faceCount = faceCount;
		this->faces = faces;
	};

	~ConvexHull();

public:
	unsigned short faceCount;
	Face* faces;
};