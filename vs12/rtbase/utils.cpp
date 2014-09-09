#include "utils.h"


vec3 averageVec3s(const std::vector<vec3>& vecs)
{
	decimal R = 0;
	decimal G = 0;
	decimal B = 0;
	for (uint i = 0; i < vecs.size(); i++){
		R += vecs[i].x;
		G += vecs[i].y;
		B += vecs[i].z;
	}
	R / vecs.size();
	G / vecs.size();
	B / vecs.size();

	return vec3(R, G, B);

}