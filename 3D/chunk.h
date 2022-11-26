#pragma once
struct Chunk
{
	Chunk* next;
	unsigned short numberOfEntities;
	bool isFull = false;
};