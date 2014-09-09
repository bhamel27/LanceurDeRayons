#pragma once
#include "../../include/geom.h"

class Cylinder : public Geometry
{
public:
	Cylinder(vec3 position, vec3 orientation, vec3 scaling, Material* mtl = new Material());

	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
	vec3 calculateNormal(vec3& hitPoint, bool sides) const;
protected:
	vec3 _center;
	vec3 _p;
	vec3 _q;
	decimal _radius;
	decimal _height;

};