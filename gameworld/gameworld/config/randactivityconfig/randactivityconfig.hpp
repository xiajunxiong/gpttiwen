#ifndef __RAND_ACTIVITY_CONFIG_HPP__
#define __RAND_ACTIVITY_CONFIG_HPP__

#include "config/logicconfigmanager.hpp"
#include <map>
#include "servercommon/servercommon.h"

#define PRE_LOAD_RA_CONFIG \
	if (NULL == RootElement || NULL == err) return false;			\
	int iRet = 0;													\
	char errinfo[1024] = { 0 };										\


#define LOAD_RA_CONFIG(element_name, init_func)\
	{\
		TiXmlElement *root_element = RootElement->FirstChildElement(element_name);	\
		if (NULL == root_element)													\
		{																			\
			sprintf(errinfo, "\n%s :  no [ %s ]\n", __FUNCTION__, #element_name);			\
			*err = errinfo;															\
			return false;															\
		}																			\
		iRet = this->init_func(root_element);											\
		if (iRet)																	\
		{																			\
			sprintf(errinfo, "\n\n%s : %s failed %d.\n", __FUNCTION__, #element_name, iRet);	\
			*err = errinfo;															\
			return false;															\
		}																			\
	}

#define LOAD_RA_CONFIG_2(element_name, init_func)\
	{\
		TiXmlElement *root_element = RootElement->FirstChildElement(element_name);	\
		if (NULL == root_element)													\
		{																			\
			sprintf(errinfo, "\n%s :  no [ %s ]\n", __FUNCTION__, #element_name);	\
			*err = errinfo;															\
			return false;															\
		}																			\
		std::string error_msg;														\
		iRet = this->init_func(root_element, error_msg);							\
		if (iRet)																	\
		{																			\
			SNPRINTF(errinfo, sizeof(errinfo), "%s: %s failed %d, errormsg[%s]", #element_name, #init_func, iRet, error_msg.c_str());\
			*err = errinfo;															\
			return false;															\
		}																			\
	}

struct  RAOpenDay
{
	RAOpenDay(int min,int max)
	{
		min_day = min;
		max_day = max;
	}

	bool operator==(const RAOpenDay & other)const
	{
		if (min_day == other.min_day && max_day == other.max_day)
		{
			return true;
		}

		return false;
	}

	int min_day;
	int max_day;
};

struct  RAOpenDayList
{
	bool operator !=(const RAOpenDayList & other)const
	{
		return open_day_vec != other.open_day_vec;
	}

	bool HasSame(RAOpenDay other)
	{
		for (int i = 0; i < (int)open_day_vec.size(); ++i)
		{
			if (open_day_vec[i] == other)
			{
				return true;
			}
		}

		return false;
	}

	bool Add(RAOpenDay other)
	{
		if (HasSame(other))
		{
			return true;
		}

		if (!open_day_vec.empty())
		{
			RAOpenDay b = open_day_vec.back();

			if (other.min_day < b.max_day)
			{
				return false;
			}
		}

		open_day_vec.push_back(other);

		return true;
	}


	std::vector<RAOpenDay> open_day_vec;
};


struct RoleCommonSaveGameItem;

class RandActivityManager;

////////////////////////////////////////////////////////////////////////////
class RandActivityConfig
{
public:
	RandActivityConfig();
	virtual ~RandActivityConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err) = 0;

	int GetActOpenDay(RandActivityManager * ramgr, int activity_type, bool can_negative = false)const;		//当前时间	    到	活动开启时间天数
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type)const;								//活动开启时间	到	开服时间天数
	int GetCurRealOpenDay(RandActivityManager* ramgr)const;								//当前时间	    到	开服时间天数
	int GetOpenDayByTimestamp(RandActivityManager* ramgr, unsigned int timestamp)const;	//timestamp	    到	开服时间天数
	int GetCurRealEndDay(RandActivityManager* ramgr, int activity_type) const;								//当前时间       到  活动结束时间天数
	virtual int GetConfigVer() const;
protected:

};

#endif	//__RAND_ACTIVITY_CONFIG_HPP__


