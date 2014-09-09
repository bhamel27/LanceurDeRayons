#include "sphere.h"

Sphere::Sphere(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
:Geometry(position, orientation, scaling, mtl){
	_radius = 1;
	_center = vec3(0.0f);
}

std::unique_ptr<struct Intersection> Sphere::intersect(const struct Ray& ray, decimal &currentdepth) const{
	vec3 ray_origin = vec3(_inv_modelTransform * vec4(ray.origin, 1.0f));
	vec3 ray_direction = vec3(_inv_modelTransform * vec4(ray.direction, 0.0f));

	float a = glm::dot(ray_direction, ray_direction);
	float b = 2 * glm::dot(ray_origin, ray_direction);
	float c = glm::dot(ray_origin, ray_origin) - _radius * _radius;
	float t0, t1;
	if (!solveQuadratic(a, b, c, t0, t1) || t1 < 0)
		return nullptr;
	if (t1 < t0)
		std::swap(t0, t1);
	double t = (t0 < 0) ? (double)t1 : (double)t0;

	vec3 ray_isect = ray_origin + t * ray_direction;
	vec3 normal = glm::normalize(ray_isect);
	vec2 uv = calculateUVSphere(normal);
	normal = glm::normalize((vec3(glm::transpose(_inv_modelTransform) * vec4(normal, 0.0f))));
	//vec2 uv = calculateUVSphere(normal);
	ray_isect = vec3(_modelTransform * vec4(ray_isect, 1.0f));

	std::unique_ptr<struct Intersection> isect(new Intersection{ ray, ray_isect, normal, uv, _material });
	return std::move(isect);
}