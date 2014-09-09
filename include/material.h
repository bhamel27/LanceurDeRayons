#pragma once
#include <main.h>
#include <basic_structs.h>
#include <texture.h>

//FORTEMENT RECOMMANDE DE CREER UN MATERIAU EN DAMIER POUR COMMENCER (VOIR ENONCE)
//PAR DEFAUT, LA TEXTURE EST INSTANCIEE A NULL PTR SI ON NE LUI PASSE PAS DE FICHIER TGA

class Material
{
public:
	Material(std::string texfile = "", vec2 tiling = vec2(1, 1)) : _texture(texfile.length() == 0 ? nullptr : new Texture(texfile.c_str())), _tiling(tiling) {}

	// shade takes care of the broad rendering process.
	// It should call shadeLight (if the light is visible!) for each light for better modularity.
	//DONC SHADE EST LA FONCTION QUI APPELLE TOUS LES SHADELIGHT
	virtual vec3 shade(const Intersection* isect, uint8_t depth) const;
protected:
	virtual vec3 shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const;
	std::unique_ptr<Texture> _texture;
	vec2 _tiling;
	
};

class MaterialLambert : public Material
{
public:
	MaterialLambert(vec3 color, std::string texfile = "", vec2 tiling = vec2(1, 1)) : Material(texfile, tiling), _color(color) { }
protected:
	virtual vec3 shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const override;
private:
	vec3 _color;
};

class MaterialBlinnPhong : public Material
{
public:
	MaterialBlinnPhong(vec3 color, decimal shininess, std::string texfile = "", vec2 tiling = vec2(1, 1)) : Material(texfile, tiling), _color(color), _shininess(shininess) { }
protected:
	virtual vec3 shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const override;
private:
	vec3 _color;
	decimal _shininess;
};

class MaterialCombined : public Material
{
public:
	MaterialCombined(vec3 diffuse, vec3 specular, vec3 ambient, decimal shininess, std::string texfile = "", vec2 tiling = vec2(1, 1)) :
		Material(texfile, tiling), _diffuse(diffuse), _specular(specular), _ambient(ambient), _shininess(shininess) { }

	virtual vec3 shade(const Intersection* isect, uint8_t depth) const override;
protected:
	virtual vec3 shadeLight(const Intersection* isect, const Light* l, uint8_t depth) const override;
private:
	vec3 _diffuse, _specular, _ambient;
	decimal _shininess;
};

class MaterialReflective : public Material
{
public:
	MaterialReflective(vec3 color, std::string texfile = "", vec2 tiling = vec2(1, 1)) : Material(texfile, tiling), _color(color) { }

	// Hint: reflective materials don't directly sample lights!
	virtual vec3 shade(const Intersection* isect, uint8_t depth) const override;
private:
	vec3 _color;
};

class MaterialRefractive : public Material
{
public:
	MaterialRefractive(vec3 color, decimal ior, std::string texfile = "", vec2 tiling = vec2(1, 1)) : Material(texfile, tiling), _color(color), _index(ior) { }

	// Hint: refractive materials don't directly sample lights!
	virtual vec3 shade(const Intersection* isect, uint8_t depth) const override;
private:
	vec3 _color;
	decimal _index;
};