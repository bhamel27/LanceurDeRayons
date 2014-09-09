#pragma once

#include <memory>
#include <vector>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_PRECISION_HIGHP_FLOAT
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <log.h>

#ifndef _MSC_VER
#if __GNUC__ != 4 || __GNUC_MINOR__ < 7
#define override
#define nullptr NULL
#endif
#include <strings.h>
#define _stricmp strcasecmp
#endif

#define abstract = 0

using namespace glm;

typedef glm::float_t decimal;

extern bool use_fresnel;

inline decimal pi() { return glm::pi<decimal>(); }
inline decimal epsilon() { return glm::epsilon<decimal>(); }
inline decimal one_over_pi() { return glm::one_over_pi<decimal>(); }
#include "../vs12/rtbase/utils.h"

void decrementCurrentDepth();
uint8_t currentDepth();