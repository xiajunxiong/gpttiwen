#ifndef __RAND_ACTIVITY_ADAPTER_HPP__
#define __RAND_ACTIVITY_ADAPTER_HPP__

#include "servercommon/servercommon.h"

class RandActivityManager;
class RandActivityAdapter
{
public:
	static RandActivityAdapter* Instance();

	RandActivityManager* GetRAManager(const UniqueServerID& usid);
private:

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(RandActivityAdapter);
};

#endif