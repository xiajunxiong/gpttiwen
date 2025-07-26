#ifndef __PLATFORMAML_H__
#define __PLATFORMAML_H__

#include "platform.h"

class PlatformMIX : public Platform
{
public:
	PlatformMIX(){}
	virtual ~PlatformMIX(){}

protected:
	virtual bool Check(NetID netid, PlatName pname, unsigned int login_time, LoginStr str, short server);
};


#endif


