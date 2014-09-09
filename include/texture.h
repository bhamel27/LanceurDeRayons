#pragma once
#include <main.h>

class Texture
{
public:
	Texture(const char* filepath);
	uint width() const { return _width; }
	uint height() const { return _height; }
	vec3 sample(const vec2& uv) const;
	const vec3& get(uint x, uint y) const { return _data[x % _width + (y % _height) * _width]; }
protected:
	uint _width, _height;

	std::vector<vec3> _data;
};