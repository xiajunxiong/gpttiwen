#ifndef __PLATFORMASQJH_H__
#define __PLATFORMASQJH_H__

#include "platform.h"

class PlatformSqjh: public Platform
{
public:
	PlatformSqjh( )
	{
	}
	virtual ~PlatformSqjh( )
	{
	}

protected:
	virtual bool Check(NetID netid,PlatName pname,unsigned int login_time,LoginStr str,short server);
};


#endif


