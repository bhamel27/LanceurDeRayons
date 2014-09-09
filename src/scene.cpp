#include <scene.h>
#include <fstream>
#include <iostream>

std::istream& operator>> (std::istream &in, vec2 &v)
{
	std::string word;

	// (##, or (##)
	in >> word;
	v.x = atof(word.substr(1, word.length() - 2).c_str());

	if (word[word.length() - 1] == ')')
	{
		v.y = v.x;
		return in;
	}

	// ##)
	in >> word;
	v.y = atof(word.substr(0, word.length() - 1).c_str());

	return in;
}

std::istream& operator>> (std::istream &in, vec3 &v)
{
	std::string word;

	// (##, or (##)
	in >> word;
	v.x = atof(word.substr(1, word.length() - 2).c_str());

	if (word[word.length() - 1] == ')')
	{
		v.y = v.z = v.x;
		return in;
	}

	// ##,
	in >> word;
	v.y = atof(word.substr(0, word.length() - 1).c_str());

	// ##)
	in >> word;
	v.z = atof(word.substr(0, word.length() - 1).c_str());

	return in;
}

Scene::Scene(const char* file)
{
	std::ifstream f(file);
	if (!f)
		_LOG_CRIT() << "Could not open scene file '" << file << "'!" << std::endl;

	_discretization = 100;
	_maxDepth = 10;

	for (std::string line; getline(f, line);)
	{
		std::string type;
		std::istringstream iss(line);
		iss >> type;
		if (type[0] == '#')
			continue;

		if (type == "Material")
		{
			std::string mattype, matname;
			iss >> mattype >> matname;

			std::string texture;
			vec2 tiling(1);

			vec3 color(1), specular(1), ambient(0);
			decimal shininess = 1, ior = 1;

			std::string mat_data;
			while (iss >> mat_data)
			{
				if (mat_data == "Texture=")
					iss >> texture;
				else if (mat_data == "Tiling=")
					iss >> tiling;
				else if (mat_data == "Color=")
					iss >> color;
				else if (mat_data == "Shininess=")
					iss >> shininess;
				else if (mat_data == "Specular=")
					iss >> specular;
				else if (mat_data == "Ambient=")
					iss >> ambient;
				else if (mat_data == "Index=")
					iss >> ior;
			}
			//Il est important de remarquer que _materiels est une map qui map les noms des textures a un materiel
			//Si il n'y a aucune texture, alors la texture sera egale a nullptr (voir material.cpp)
			if (mattype == "Lambert")
				_materials[matname] = std::unique_ptr<Material>(new MaterialLambert(color, texture, tiling));
			else if (mattype == "BlinnPhong")
				_materials[matname] = std::unique_ptr<Material>(new MaterialBlinnPhong(color, shininess, texture, tiling));
			else if (mattype == "Combined")
				_materials[matname] = std::unique_ptr<Material>(new MaterialCombined(color, specular, ambient, shininess, texture, tiling));
			else if (mattype == "Reflective")
				_materials[matname] = std::unique_ptr<Material>(new MaterialReflective(color, texture, tiling));
			else if (mattype == "Refractive")
				_materials[matname] = std::unique_ptr<Material>(new MaterialRefractive(color, ior, texture, tiling));
			else if (mattype == "Default")
				_materials[matname] = std::unique_ptr<Material>(new Material(texture, tiling));
		}
		else if (type == "Light")
		{
			std::string lighttype, shadows;
			iss >> lighttype >> shadows;

			Light* l = new Light();
			l->type = lighttype == "Directional" ? Light::DIRECTIONAL : Light::POINT;

			if (shadows == "NoShadows")
				l->type |= Light::NO_SHADOWS;

			iss >> l->color >> l->positionOrDirection;
			if (l->directional())
				l->positionOrDirection = normalize(l->positionOrDirection);

			_lights.push_back(std::unique_ptr<Light>(l));
		}
		else if (type == "Background")
		{
			iss >> _background;
		}
		else if (type == "Camera")
		{
			vec3 eye, center, up;
			iss >> eye >> center >> up >> _fov;
			//converti en rad
			_fov *= pi() / 180.0;

			_cameraMatrix = inverse(glm::lookAt(eye, center, up));
		}
		else if (type == "Globals")
		{
			iss >> _discretization >> _maxDepth;
		}
		else if (type.length() > 0)
		{
			vec3 position, rotation, scaling;
			std::string mat;
			iss >> position >> rotation >> scaling >> mat;

			rotation *= pi() / 180.0;

			auto matit = _materials.find(mat);
			Material* m = matit != _materials.end() ? matit->second.get() : new Material();

			if (type == "Sphere")
				_geometry.push_back(std::unique_ptr<Geometry>(new Sphere(position, rotation, scaling, m)));
			else if (type == "Box")
				_geometry.push_back(std::unique_ptr<Geometry>(new Box(position, rotation, scaling, m)));
			else if (type == "Cylinder")
				_geometry.push_back(std::unique_ptr<Geometry>(new Cylinder(position, rotation, scaling, m)));
			else if (type == "Cone")
				_geometry.push_back(std::unique_ptr<Geometry>(new Cone(position, rotation, scaling, m)));
		}
	}
}

std::unique_ptr<Intersection> Scene::trace(const Ray& ray, uint8_t depth, decimal maxdist, decimal mindist) const
{
	if (depth == maxDepth()) return nullptr;
	decimal min_dist = maxdist;
	std::unique_ptr<Intersection> nearest_isect;
	for (uint i = 0; i < _geometry.size(); i++){
		std::unique_ptr<Intersection> current_isect = _geometry.at(i)->intersect(ray, mindist);
		if (current_isect != nullptr){
			decimal current_dist = glm::length(current_isect->position - current_isect->ray.origin);
			if (current_dist < min_dist) {
				min_dist = current_dist;
				nearest_isect = std::move(current_isect);
				nearest_isect->scene = this;
			}
		}
	}
	return std::move(nearest_isect);
}