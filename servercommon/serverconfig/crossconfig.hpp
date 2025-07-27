#ifndef __CROSS_CONFIG_H__
#define __CROSS_CONFIG_H__

#include <string>

#include "servercommon/crossdef.hpp"
#include "servercommon/servercommon.h"

struct CrossConfigItem							// cross����
{
	NetAddress cross_server_addr;				// cross��ַ
};

class CrossConfig
{
public:
	static CrossConfig & Instance();

	bool Init(const std::string &path, std::string *err);
	bool Reload(const std::string &path, std::string *err, bool *is_cross_addr_change, bool *is_open_cross_change);

	inline const CrossConfigItem & GetCrossConfig() { return m_cross_config; }
	inline bool IsOpenCross() { return m_is_open_cross; }
	inline bool IsHiddenServer() { return m_is_hidden_server; }
	
	inline int GetHiddenServerIDX() const { return m_hidden_server_idx; }

private:
	CrossConfig();
	~CrossConfig();
	
	CrossConfigItem		m_cross_config;;			// cross����

	bool m_is_open_cross;							// �Ƿ񿪷�cross
	bool m_is_hidden_server;						// �Ƿ����ط�
	int m_hidden_server_idx;						// ���ط������
};

#endif	// __CROSS_CONFIG_H__

