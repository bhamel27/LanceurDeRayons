#pragma once
#include "../../include/geom.h"

class Cone : public Geometry
{
public:
	Cone(vec3 position, vec3 orientation, vec3 scaling, Material* mtl = new Material());
	vec3 Cone::calculateNormal(vec3& hitPoint, bool sides)const;
	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
protected:
	vec3 _base_center;
	vec3 _apex;
	vec3 _direction;
	vec3 _center;
	decimal _radius;
	decimal _height;
};


