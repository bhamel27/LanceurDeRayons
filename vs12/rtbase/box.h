#pragma once
#include "../../include/geom.h"
class Box : public Geometry
{
public:
	Box(vec3 position, vec3 orientation, vec3 scaling, Material* mtl = new Material());
	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
protected:
	vec3 _min;
	vec3 _max;
	vec3 _center;
	std::array<vec3, 8> points;
	std::vector<vec3>_faces_points;
};
