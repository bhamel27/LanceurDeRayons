#pragma once
#include <main.h>

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Intersection
{
	Ray ray;
	vec3 position;
	vec3 normal;
	vec2 uv;
	const class Material* material;
	const class Scene* scene;
};

struct Light
{
	// For compactness, we can reuse the same vec3 as a direction or as a position
	// NB: Make sure to normalize the vector if it's a direction!
	vec3 positionOrDirection;
	vec3 color;

	// type is an enum flag
	// See http://www.codeproject.com/Articles/13740/The-Beginner-s-Guide-to-Using-Enum-Flags
	enum Type
	{
		DIRECTIONAL = 1,
		POINT = 2,
		NO_SHADOWS = 128
	};
	uint8_t type;

	bool directional() const { return (type & DIRECTIONAL) == DIRECTIONAL; }
};