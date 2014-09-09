#include "cone.h"


Cone::Cone(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
:Geometry(position, orientation, scaling, mtl){
	_radius = 1;
	_apex = vec3(0.0f);
	_base_center = vec3(0.0f, -1.0f, 0.0f);
	_direction = vec3(0.0f, 1.0f, 0.0f);
	_height = 1;
	_center = vec3(0.0f);
}

std::unique_ptr<struct Intersection> Cone::intersect(const struct Ray& ray, decimal &currentdepth) const{
	vec3 ray_origin = vec3(_inv_modelTransform * vec4(ray.origin, 1.0f));
	vec3 ray_direction = vec3(_inv_modelTransform * vec4(ray.direction, 0.0f));

	// https://github.com/Penetra/CG-Project/blob/master/Cone.cpp
	double t = 0, t1 = DBL_MAX, t2 = DBL_MAX, t3 = DBL_MAX;
	bool sides = true;
	double rh = -(_radius * _radius) / (_height * _height);
	double a = pow(ray_direction.x, 2) + pow(ray_direction.z, 2) + rh * pow(ray_direction.y, 2);
	double b = 2 * (ray_direction.x * (ray_origin.x - _base_center.x) + ray_direction.z * (ray_origin.z - _base_center.z) + rh * ray_direction.y * (ray_origin.y - _base_center.y - _height));
	double c = pow(ray_origin.x - _base_center.x, 2) + pow(ray_origin.z - _base_center.z, 2) + rh * pow(ray_origin.y - _base_center.y - _height, 2);
	double root = b * b - 4.0 * a * c;
	if (root < 0)
		sides = false;
	if (sides){
		t1 = (-b + sqrtf(root)) / (2.0 * a);
		t2 = (-b - sqrtf(root)) / (2.0 * a);
		vec3 isect = ray_origin + t1 * ray_direction;
		vec3 isect2 = ray_origin + t2 * ray_direction;

		if (isect.y <= _apex.y && isect.y >= _base_center.y) {
			if (isect2.y <= _apex.y && isect2.y >= _base_center.y) {
				if (t1 < t2 && t1 > 0.0 && t2 > 0.0)
					t = t1;
				else if (t2 > 0.0)
					t = t2;
			}
			else if (t1 > 0.0)
				t = t1;
		}
		else if (isect2.y <= _apex.y && isect2.y >= _base_center.y)
		if (t2 > 0.0)
			t = t2;
	}

	if (plane_Intersection(ray_origin, ray_direction, vec3(0, 1.0, 0), _base_center, t3)) {
		vec3 isect = ray_origin + t3 * ray_direction;
		if (pow(isect.x - _base_center.x, 2) + pow(isect.z - _base_center.z, 2) <= _radius * _radius) {
			if (t3 < t) {
				t = t3;
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
		else{
			uv = calculateUVCircle(ray_isect - _base_center);
		}

		ray_isect = vec3(_modelTransform * vec4(ray_isect, 1.0f));
		std::unique_ptr<struct Intersection> isect(new Intersection{ ray, ray_isect, normal, uv, _material });
		return std::move(isect);
	}
	return nullptr;
}


//https://github.com/Penetra/CG-Project/blob/master/Cone.cpp
vec3 Cone::calculateNormal(vec3 &hitPoint, bool sides) const{
	//PAS COMPLETER ENCORE, JAI JUSTE COPY PASTE UNE FONCTION, QUE JAI LEGEREMENT MODIFIE
	vec3 normal;
	if (!sides)
		normal = normalize(_base_center - _apex);
	else {
		double e = -(_radius*_radius) / (_height * _height);
		normal.x = hitPoint.x - _base_center.x;
		normal.y = hitPoint.y - _base_center.y - _height;
		normal.z = hitPoint.z - _base_center.z;
		normal.y = e * normal.y;
		normal = normalize(normal);
	}
	return normal;
}