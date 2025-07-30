#ifndef __RAND_ACTIVITY_COLLECT_WORD_HPP__
#define __RAND_ACTIVITY_COLLECT_WORD_HPP__

#include "global/randactivity/randactivity.hpp"

class RandAcitivityCollectWord : public RandActivity
{
public:
	RandAcitivityCollectWord(RandActivityManager *activity_manager);
	~RandAcitivityCollectWord();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};


#endif