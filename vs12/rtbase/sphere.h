#pragma once
#include "../../include/geom.h"
class Sphere : public Geometry
{
public:
	Sphere(vec3 position, vec3 orientation, vec3 scaling, Material* mtl = new Material());

	//const after a function declaration means that the function is not allowed to 
	//change any class members(except ones that are marked mutable)
	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
protected:
	decimal _radius;
	vec3 _center;

};
