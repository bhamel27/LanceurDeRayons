#include <geom.h>
#include <basic_structs.h>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <array>


//https://code.google.com/p/pwsraytracer/source/browse/trunk/raytracer/
Geometry::Geometry(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
:_position(position),
_orientation(orientation),
_scaling(scaling),
_material(mtl)
{	
	mat4 translation_mat = glm::translate(mat4(), position);
	mat4 scaling_mat = glm::scale(mat4(), scaling);
	mat4 rotateX = glm::rotate(mat4(), orientation.x, vec3(1.0,0,0));
	mat4 rotateY = glm::rotate(mat4(), orientation.y, vec3(0.0, 1, 0));
	mat4 rotateZ = glm::rotate(mat4(), orientation.z, vec3(0.0, 0, 1));
	_modelTransform ={
		translation_mat*
		
		
		rotateX*rotateY*rotateZ*
		scaling_mat
	};

	_inv_modelTransform = glm::inverse(_modelTransform);
}

int plane_Intersection(vec3 ray_origin, vec3 ray_direction, vec3 normal, vec3 point, double &t){
	double d = 0 - (dot(normal, point));
	double temp = (dot(normal, ray_direction));
	if (temp == 0)
		return 0;
	t = (-d - (dot(normal, ray_origin))) / temp;
	if (t < 0.0000001)
		return 0;
	return 1;
}

vec2 calculateUVSphere(const vec3& point){
	//decimal u = 0.5 + atan2(point.z, point.x) / (2 * glm::pi<decimal>());
	
	float u = acos(point.y / 1.0f);
	float v = atan2(point.x, point.z);
	
	v += pi<decimal>();
	u = u / pi<decimal>();
	v = v / (2 * pi<decimal>());
	return vec2(u,v);
}

vec2 calculateUVCylinder(const vec3& point){
	vec2 uv;
	double phi = atan2(point.x, point.z);
	phi += pi<decimal>();

	uv.y = phi * (1 / (2 * pi<decimal>()));
	uv.x = (point.y + 1) / 2;

	return uv;
}


vec2 calculateUVCircle(const vec3& point){
	vec2 uv;
	uv.x = sqrt((pow(point.x, 2) + pow(point.z, 2)));
	double phi = (atan2(point.x, point.z));

	phi += pi<decimal>();

	uv.y = phi * (1 / (2 * pi<decimal>()));
	return uv;
}