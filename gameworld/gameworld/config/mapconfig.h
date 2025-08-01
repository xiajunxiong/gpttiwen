#ifndef __MAPCONFIG_H__
#define __MAPCONFIG_H__

#include <string>
#include <set>
#include "servercommon/serverdef.h"

typedef std::string ConfigMapMask;

struct ConfigMap
{
	ConfigMap():id(0), width(0), height(0){}
	int id;
	int width;
	int height;
	ConfigMapMask mask;

	std::string lasterr;
	bool Load(const std::string& configname);
	std::string GetError(){ return lasterr; }
};

#endif

