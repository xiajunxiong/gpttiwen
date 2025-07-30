#ifndef __MAIL_ITEM_HPP__
#define __MAIL_ITEM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/maildef.hpp"

class MailItem
{
public:
	MailItem();
	~MailItem();

	bool OperTimeIsValid();

	bool HasAttachment() const;
	bool HasFetched() const;
	void ClearAttachment();
	bool Invalid();

	void * operator new(size_t c);
	void operator delete(void *m);

public:
	MailParam mail_param;
	unsigned int last_fetch_time;
};

#endif

