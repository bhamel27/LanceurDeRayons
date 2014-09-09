#pragma once
#include <main.h>
#include <material.h>
#include <array>


int plane_Intersection(vec3 ray_origin, vec3 ray_direction, vec3 normal, vec3 point, double &t);
vec2 calculateUVSphere(const vec3& point);
vec2 calculateUVCylinder(const vec3& point);
vec2 calculateUVCircle(const vec3& point);

class Geometry
{
public:
	Geometry(vec3 position, vec3 orientation, vec3 scaling, Material* mtl = new Material());

	// An interesting use of this function definition is to only create an intersection
	// object when we are certain that this new intersection would be the closest.
	// Passing currentdepth (the linear distance between the ray's origin and the previous closest object)
	// lets us do just that. Moreover, the & lets us update that value straight in the intersect method.
	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const abstract;
	
protected:
	vec3 _position;
	vec3 _orientation;
	vec3 _scaling;
	mat4 _modelTransform;
	mat4 _inv_modelTransform;
	Material* _material;

	// Transform order: scaling, then rotation, then translation (use glm methods)
	// Hint: store the transforms in this class as a single matrix
	// Hint: preprocess any modified matrices you might need (like the inverse)
};




