#include "cylinder.h"


Cylinder::Cylinder(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
:Geometry(position, orientation, scaling, mtl){
	_center = vec3(0.0f);
	_p = vec3(0.0f, 1.0f, 0.0f);
	_q = vec3(0.0f, -1.0f, 0.0f);
	_radius = 1;
	_height = 2;
}

std::unique_ptr<struct Intersection> Cylinder::intersect(const struct Ray& ray, decimal &currentdepth) const{
	vec3 ray_origin = vec3(_inv_modelTransform * vec4(ray.origin, 1.0f));
	vec3 ray_direction = vec3(_inv_modelTransform * vec4(ray.direction, 0.0f));

https://code.google.com/p/pwsraytracer/source/browse/trunk/raytracer/cylinder.cpp?r=160

	double tmin = INFINITY;
	double t = DBL_MAX, t1 = DBL_MAX, t2 = DBL_MAX, t3 = DBL_MAX, t4 = DBL_MAX;
	bool sides = false;
	double a = pow(ray_direction.x, 2) + pow(ray_direction.z, 2);
	double b = 2.0 * ((ray_origin.x - _center.x) * ray_direction.x + (ray_origin.z - _center.z) * ray_direction.z);
	double c = pow(ray_origin.x - _center.x, 2) + pow(ray_origin.z - _center.z, 2) - pow(_radius, 2);
	double D = b * b - 4.0 * a * c;
	if (D < 0.0) {
		return nullptr;
	}
	else{
		double sqrtD = sqrt(D);
		double aa = a * 2;
		double t1 = (-b - sqrtD) / aa;
		if (t1 > epsilon<double>()) {
			double y = (ray_origin.y - _center.y) + t1 * ray_direction.y;
			if (y > _q.y && y < _p.y) {
				if (t1 < t) {
					t = t1;
					sides = true;
				}
			}
		}
		t2 = (-b + sqrtD) / aa;
		if (t2 >  epsilon<double>()) {
			double y = (ray_origin.y - _center.y) + t2 * ray_direction.y;
			if (y > _q.y && y < _p.y) {
				if (t2 < t){
					t = t2;
					sides = true;
				}
			}
		}
	}
	if (plane_Intersection(ray_origin, ray_direction, vec3(0, -1.0, 0), _p, t3)) {
		vec3 intersection = ray_origin + t3 * ray_direction;
		if (pow(intersection.x - _p.x, 2) + pow(intersection.z - _p.z, 2) <= _radius * _radius) {
			if (t3 < t) {
				t = t3;
				sides = false;
			}
		}
	}
	if (plane_Intersection(ray_origin, ray_direction, vec3(0, 1.0, 0), _q, t4)) {
		vec3 intersection = ray_origin + t4 * ray_direction;
		if (pow(intersection.x - _q.x, 2) + pow(intersection.z - _q.z, 2) <= _radius * _radius) {
			if (t4 < t){
				t = t4;
				sides = false;
			}
		}
	}
	if (t != DBL_MAX && t > 0.00000001) {
		vec3 ray_isect = ray_origin + t * ray_direction;
		vec3 normal = calculateNormal(ray_isect, sides);
		normal = glm::normalize((vec3(glm::transpose(_inv_modelTransform) * vec4(normal, 0.0f))));
		vec2 uv;
		if (sides)
			uv = calculateUVCylinder(ray_isect);
		else
			uv = calculateUVCircle(ray_isect - _p);
		ray_isect = vec3(_modelTransform * vec4(ray_isect, 1.0f));
		std::unique_ptr<struct Intersection> isect(new Intersection{ ray, ray_isect, normal, uv, _material });
		return std::move(isect);
	}
	return nullptr;
}

vec3 Cylinder::calculateNormal(vec3& hitPoint, bool sides)const{
	vec3 normal;
	if (!sides && hitPoint.y> _center.y)
		normal = normalize(_p);
	else if (!sides && hitPoint.y<_center.y)
		normal = normalize(_q);
	else {
		normal = normalize(vec3(hitPoint.x, 0.0, hitPoint.z));
	}
	return normal;
}