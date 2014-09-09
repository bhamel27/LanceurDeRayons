#include "box.h"

Box::Box(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
:Geometry(position, orientation, scaling, mtl){
	_center = vec3(0.0f);
	_min = vec3(-1.0f);
	_max = vec3(1.0f);

	vec3 corner_ftr{ 1.0f, 1.0f, 1.0f };//0
	vec3 corner_rbl{ -1.0f, -1.0f, -1.0f };//1
	vec3 corner_fbr{ corner_ftr.x, corner_rbl.y, corner_ftr.z };//2
	vec3 corner_ftl{ corner_ftr.x, corner_ftr.y, corner_rbl.z };//3
	vec3 corner_fbl{ corner_ftr.x, corner_rbl.y, corner_rbl.z };//4
	vec3 corner_rtr{ corner_rbl.x, corner_ftr.y, corner_ftr.z };//5
	vec3 corner_rtl{ corner_rbl.x, corner_ftr.y, corner_rbl.z };//6
	vec3 corner_rbr{ corner_rbl.x, corner_rbl.y, corner_ftr.z };//7

	points = { { corner_ftr, corner_rbl, corner_fbr, corner_ftl, corner_fbl, corner_rtr, corner_rtl, corner_rbr } };

	_faces_points.push_back(vec3(1.0f, 0.0f, 0.0f));
	_faces_points.push_back(vec3(0.0f, 1.0f, 0.0f));
	_faces_points.push_back(vec3(0.0f, 0.0f, 1.0f));
	_faces_points.push_back(vec3(-1.0f, 0.0f, 0.0f));
	_faces_points.push_back(vec3(0.0f, -1.0f, 0.0f));
	_faces_points.push_back(vec3(0.0f, 0.0f, -1.0f));
}


std::unique_ptr<struct Intersection> Box::intersect(const struct Ray& ray, decimal &currentdepth) const{
	vec3 ray_origin = vec3(_inv_modelTransform * vec4(ray.origin, 1.0f));
	vec3 ray_direction = vec3(_inv_modelTransform * vec4(ray.direction, 0.0f));

	float t1 = (_min.x - ray_origin.x)*(1.0 / ray_direction.x);
	float t2 = (_max.x - ray_origin.x)*(1.0 / ray_direction.x);
	float t3 = (_min.y - ray_origin.y)*(1.0 / ray_direction.y);
	float t4 = (_max.y - ray_origin.y)*(1.0 / ray_direction.y);
	float t5 = (_min.z - ray_origin.z)*(1.0 / ray_direction.z);
	float t6 = (_max.z - ray_origin.z)*(1.0 / ray_direction.z);
	decimal t = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	if (tmax < 0)
		return nullptr;
	if (t > tmax)
		return nullptr;

	vec3 ray_isect = ray_origin + t * ray_direction;

	//Calcul des normales
	decimal min_face_dist = INFINITY;
	uint index=0;
	for (uint i = 0; i < _faces_points.size(); i++){
		decimal curr_face_dist = length(_faces_points[i] - ray_isect);
		if (curr_face_dist < min_face_dist){
			min_face_dist = curr_face_dist;
			index = i;
		}
	}

	vec3 normal = glm::normalize(_faces_points[index]);
	normal = glm::normalize((vec3(glm::transpose(_inv_modelTransform) * vec4(normal, 0.0f))));

	// Calcul des coordonees uv		
	vec3 uv_coord_0_0;
	vec3 uv_coord_1_0;
	vec3 uv_coord_0_1;

	// test pour savoir sur quel plan se trouve l'intersection
	if (index == 0) {
		uv_coord_0_0 = points[4];
		uv_coord_1_0 = points[2];
		uv_coord_0_1 = points[3];
	}

	if (index == 1) {
		uv_coord_0_0 = points[3];
		uv_coord_1_0 = points[0];
		uv_coord_0_1 = points[6];
	}

	if (index == 2) {
		uv_coord_0_0 = points[2];
		uv_coord_1_0 = points[7];
		uv_coord_0_1 = points[0];
	}

	if (index == 3) {
		uv_coord_0_0 = points[1];
		uv_coord_1_0 = points[7];
		uv_coord_0_1 = points[6];
	}

	if (index == 4) {
		uv_coord_0_0 = points[1];
		uv_coord_1_0 = points[7];
		uv_coord_0_1 = points[4];
	}

	if (index == 5) {
		uv_coord_0_0 = points[1];
		uv_coord_1_0 = points[4];
		uv_coord_0_1 = points[6];
	}

	vec3 u_vec = glm::cross((ray_isect - uv_coord_0_0), (ray_isect - uv_coord_1_0));
	float u = glm::length(u_vec) / (2 * glm::length(uv_coord_1_0 - uv_coord_0_0));
	vec3 v_vec = glm::cross((ray_isect - uv_coord_0_0), (ray_isect - uv_coord_0_1));
	float v = glm::length(v_vec) / (2 * glm::length(uv_coord_0_1 - uv_coord_0_0));

	ray_isect = vec3(_modelTransform * vec4(ray_isect, 1.0f));
	vec2 uv = glm::vec2(u, v);
	std::unique_ptr<struct Intersection> isect(new Intersection{ ray, ray_isect, normal, uv, _material });
	return std::move(isect);
}