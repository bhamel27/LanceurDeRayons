#pragma once
#include <main.h>
#include <unordered_map>

#include <basic_structs.h>
//#include <geom.h>
#include "../vs12/rtbase/sphere.h"
#include "../vs12/rtbase/cone.h"
#include "../vs12/rtbase/box.h"
#include "../vs12/rtbase/cylinder.h"

class Scene
{
public:
	Scene(const char* file);

	//TRACE FUNCTION!!!!! Premiere chose a faire
	std::unique_ptr<Intersection> trace(const Ray& ray, uint8_t depth, decimal maxdist = 1e20, decimal mindist = 1e-5) const;
	

	const mat4& cameraMatrix() const { return _cameraMatrix; }
	decimal fov() const { return _fov; }
	uint8_t maxDepth() const { return _maxDepth; }
	vec3 background() const { return _background; }
	decimal discretization() const { return _discretization; }

	const std::vector<std::unique_ptr<Light>>& lights() const { return _lights; }

protected:
	//Le vecteur contenant les formes a parcourir pour determiner si il y a intersection
	std::vector<std::unique_ptr<Geometry>> _geometry;
	//Le vecteur contenant les lumieres
	std::vector<std::unique_ptr<Light>> _lights;
	//Un mapping entre les noms des materiaux et les materiaux
	std::unordered_map<std::string, std::unique_ptr<Material>> _materials;
	//background color
	vec3 _background;
	//La camera matrix est linverse du glm::lookAt(eye,center,up), definie par le fichier de scene
	mat4 _cameraMatrix;
	decimal _fov;

	decimal _discretization;

	uint _maxDepth;

};