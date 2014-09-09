#include <material.h>
#include <scene.h>
#include <iostream>

vec3 Material::shade(const Intersection* isect, uint8_t depth) const {
	depth++;
	decimal offset = 1e-4;
	const std::vector<std::unique_ptr<Light>>& lights = isect->scene->lights();
	vec3 total_light(0.0f);
	vec3 shadow_ray_origin = isect->position + offset * isect->normal;
	uint nb_lights = lights.size();
	
	for (uint i = 0; i < nb_lights; i++){
		bool inShadow = false;
		bool checkForDropShadows = true;
		vec3 shadow_ray_direction;

		//Light type check
		if (lights[i]->type >= lights[i]->NO_SHADOWS)
			inShadow = false;
		else{
			if (lights[i]->directional())
				shadow_ray_direction = glm::normalize(-lights.at(i)->positionOrDirection);
			else
				shadow_ray_direction = glm::normalize(lights.at(i)->positionOrDirection - shadow_ray_origin);

			//Shadow ray
			Ray shadow_ray = Ray{ shadow_ray_origin, shadow_ray_direction };
			if (isect->scene->trace(shadow_ray, 1) == nullptr)
				inShadow = false;
			else
				inShadow = true;
		}

		if (!inShadow)
			total_light += this->shadeLight(isect, lights[i].get(), depth);
		
	}

	return glm::min(total_light, vec3(255.0f, 255.0f, 255.0f));
}

//NE PAS OUBLIER QUE LES LUMIERES DIRECTIONNELLES SONT NORMALISEES DANS SCENE.cpp

vec3 Material::shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const {
	float scale = 10.0f;
	float u = isect->uv.x;
	float v = isect->uv.y;
    //coefficient de reflexion de la lumiere
	vec3 color;
	if (_texture != nullptr){
		color = _texture->sample(vec2(_texture->width() * _tiling.x * u, _texture->height() * _tiling.y * v));
	}
	else{
		if ((int)(floorf(scale * _tiling.x * u) + floorf(scale * _tiling.y * v)) % 2 == 1){
			color = vec3(1.0f);
		}
		else
			color = vec3(0.0f);
	}

	if (l->directional()){
		color = l->color * color / glm::pi<double>();
	}
	else {
		double dist = glm::length(l->positionOrDirection - isect->position);
		color = ((l->color*pi()) / pow(dist, 2.0)) * color / glm::pi<double>();
	}

	return color;
}


vec3 MaterialLambert::shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const {
	vec3 color;

	if (l->directional()){
		double lambert = max(glm::dot(-l->positionOrDirection, isect->normal), 0.0);
		color = lambert * (_color / glm::pi<double>()) * l->color;
	}
	else {
		double lambert = max(glm::dot(-normalize(isect->position - l->positionOrDirection), isect->normal), 0.0);
		double dist = glm::length(l->positionOrDirection - isect->position);
		color = lambert * (_color / glm::pi<double>()) * l->color * glm::pi<double>() / pow(dist, 2.0);
	}

	return color;
}


vec3 MaterialBlinnPhong::shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const {
	vec3 color;

	if (l->directional()){
		vec3 l_vec = -l->positionOrDirection;
		double l_angle = max(glm::dot(l_vec, isect->normal), 0.0);
		vec3 h_vec = (l_vec - isect->ray.direction) / glm::length(l_vec - isect->ray.direction);
		double h_angle = max(glm::dot(h_vec, isect->normal), 0.0);
		color = l_angle * ((_shininess + 2.0) / (8.0* glm::pi<double>())) * pow(h_angle, _shininess) * _color * l->color;
	}
	else {
		vec3 l_vec = -glm::normalize(isect->position - l->positionOrDirection);
		double l_angle = max(glm::dot(l_vec, isect->normal), 0.0);
		vec3 h_vec = (l_vec - isect->ray.direction) / glm::length(l_vec - isect->ray.direction);
		double h_angle = max(glm::dot(h_vec, isect->normal), 0.0);
		double dist = glm::length(l->positionOrDirection - isect->position);
		color = l_angle * ((_shininess + 2.0) / (8.0 * glm::pi<double>())) * pow(h_angle, _shininess) * _color * l->color * glm::pi<double>() / pow(dist, 2.0);
	}

	return color;
}


vec3 MaterialCombined::shade(const Intersection* isect, uint8_t depth) const {
	depth++;
	decimal bias = 1e-4;
	const std::vector<std::unique_ptr<Light>>& lights = isect->scene->lights();
	vec3 total_light = _ambient;
	vec3 shadow_ray_origin = isect->position + bias * isect->normal;
	uint nb_lights = lights.size();

	for (uint i = 0; i < nb_lights; i++){
		bool inShadow = false;
		bool checkForDropShadows = true;
		vec3 shadow_ray_direction;

		//Light type check
		if (lights[i]->type >= lights[i]->NO_SHADOWS)
			inShadow = false;
		else{
			if (lights[i]->directional())
				shadow_ray_direction = glm::normalize(-lights.at(i)->positionOrDirection);
			else
				shadow_ray_direction = glm::normalize(lights.at(i)->positionOrDirection - shadow_ray_origin);

			//Shadow ray
			Ray shadow_ray = Ray{ shadow_ray_origin, shadow_ray_direction };
			if (isect->scene->trace(shadow_ray, 1) == nullptr)
				inShadow = false;
			else
				inShadow = true;
		}

		if (!inShadow)
			total_light += this->shadeLight(isect, lights[i].get(), depth);

	}

	return glm::min(total_light, vec3(255.0f, 255.0f, 255.0f));
}

vec3 fresnel(const vec3& specularColor,double angle){
	decimal Rr = specularColor.r + (1 - specularColor.r)*pow((1 - angle), 5);
	decimal Rg = specularColor.g + (1 - specularColor.g)*pow((1 - angle), 5);
	decimal Rb = specularColor.b + (1 - specularColor.b)*pow((1 - angle), 5);
	vec3 k = specularColor;
	return vec3(specularColor.r * Rr, specularColor.g*Rg,specularColor.b*Rb);

}
vec3 MaterialCombined::shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const {
	vec3 color;

	vec3 fresnel_specular;
	
	if (l->directional()){
		vec3 l_vec = -l->positionOrDirection;
		double l_angle = max(glm::dot(l_vec, isect->normal), 0.0);
		vec3 h_vec = normalize(l_vec - isect->ray.direction);
		double h_angle = max(glm::dot(h_vec, isect->normal), 0.0);
		if(use_fresnel)fresnel_specular = fresnel(_specular, (dot(h_vec,isect->ray.direction)));
		else fresnel_specular = _specular;
		color = (_diffuse / glm::pi<double>() + fresnel_specular * ((_shininess + 2.0) / (8.0 * glm::pi<double>())) * pow(h_angle, _shininess)) * l_angle * l->color;
	}
	else {
		vec3 l_vec = -glm::normalize(isect->position - l->positionOrDirection);
		double l_angle = max(glm::dot(l_vec, isect->normal), 0.0);
		vec3 h_vec = normalize(l_vec - isect->ray.direction);
		double h_angle = max(glm::dot(h_vec, isect->normal), 0.0);
		double dist = glm::length(l->positionOrDirection - isect->position);
		if (use_fresnel)fresnel_specular = fresnel(_specular, (dot(h_vec, isect->ray.direction)));
		else fresnel_specular = _specular;
		color = (_diffuse / glm::pi<double>() + fresnel_specular * ((_shininess + 2.0) / (8.0 * glm::pi<double>())) * pow(h_angle, _shininess)) * l_angle * l->color * glm::pi<double>() / pow(dist, 2.0);
	}

	return color;
}

//
vec3 MaterialReflective::shade(const Intersection* isect, uint8_t depth) const {
	if (depth >= isect->scene->maxDepth()) return _color;
	depth++;
	decimal offset = 1e-4;
	vec3 color(0.0);
	vec3 v = isect->ray.direction;
	vec3 n = isect->normal;
	vec3 r = v - n * 2.0 * dot(v, n);
	Ray ray_reflex = Ray{ isect->position + offset*isect->normal, r };
	std::unique_ptr<Intersection> isect2 = isect->scene->trace(ray_reflex, depth);
	if (isect2 != nullptr){
		color = vec3(isect2->material->shade(isect2.get(), depth)) * _color;
	}
	else color = isect->scene->background() * _color;

	return color;
}

vec3 MaterialRefractive::shade(const Intersection* isect, uint8_t depth) const {
	depth++;

	//early exit
	if (depth >= isect->scene->maxDepth())return _color;

	//initialization
	vec3 color;
	vec3 refrdir;
	decimal offset = 1e-4;
	vec3 n = isect->normal;
	vec3 d = isect->ray.direction;
	decimal c;
	bool inside;

	//reflexion ray
	Ray reflectionRay = Ray{ isect->position + n*offset, reflect(d, n) };
	std::unique_ptr<Intersection> reflectt = isect->scene->trace(reflectionRay, depth);
	vec3 reflect_col = (reflectt != nullptr) ? reflectt->material->shade(reflectt.get(), depth) : _color;

	//calculs pour refraction
	(dot(d, n) > 0) ? n = -n, inside = true, c=dot(d,n) : inside = false, c = -dot(d,n);
	decimal eta = inside ? _index : 1.0 / _index;
	refrdir = refract(d, n, eta);
	
	//refraction ray
	Ray refractionRay = Ray{ isect->position - n*offset, refrdir };
	std::unique_ptr<Intersection> refract = isect->scene->trace(refractionRay, depth);
	vec3 refract_col = (refract != nullptr) ? refract->material->shade(refract.get(), depth) : reflect_col;

	//Shlick
	decimal R0 = pow((eta-1.0), 2.0) / pow((eta + 1), 2);
	decimal R = R0 + (1 - R0)*pow((1 - c), 5);

	color = (reflect_col *R+refract_col * (1-R)) * _color;
	if (!use_fresnel){ 
		return _color *refract_col;
	}
	return color;

}

