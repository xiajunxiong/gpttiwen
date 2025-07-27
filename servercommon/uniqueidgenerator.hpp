#ifndef __UNIQUE_ID_GENERATOR__
#define __UNIQUE_ID_GENERATOR__

#include "servercommon/servercommon.h"
#include <set>

enum SERVER_TYPE
{
	SERVER_TYPE_INVALID = 0,
	LOGINSERVER,
	GATEWAY,
	GAMEWORLD,
	DATAACCESS,
	CROSSSERVER,
	BATTLESERVER,
	TELNETCLIENT,
	BIGCROSSSERVER,
	SERVER_TYPE_MAX,
};
UNSTD_STATIC_CHECK(SERVER_TYPE_MAX < 16);


//!< ��ʼʱ��� (2020-01-01)
static const long long ZERO_TIMESTAMP = 1577808000LL;

//!< ����������λ����֧������[0,16)
static const long long SERVER_TYPE_BIT = 4LL;

//!< ������IDλ��, ֧�ַ�����ID[0,4096)
static const long long SERVER_ID_BIT = 12LL;

//!< ʵ��indexλ����֧��index[0,16)
static const long long SERVER_INDEX_BIT = 4LL;

//!< ʱ���λ��,����Ϊ��λ��֧�ִ�[2020-01-01, 2037-01-04)  
static const long long TIMESTAMP_BIT = 29LL;

//!< ����IDλ����֧������ID��Χ[0, 8192) ��ÿ�����16384��ID
static const long long INC_ID_BIT = 14LL;

static const long long SIGN_BIT = 1LL;

union UniqueIDUnion
{
	struct 
	{
		long long inc_id : INC_ID_BIT;
		long long timestamp_diff : TIMESTAMP_BIT;
		long long server_index : SERVER_INDEX_BIT;
		long long server_id : SERVER_ID_BIT;
		long long server_type : SERVER_TYPE_BIT;
		long long sign_bit : SIGN_BIT;
	};

	long long unique_id;
};
UNSTD_STATIC_CHECK(8 == sizeof(UniqueIDUnion));

// ���뱣��1λ��Ϊ����λ
UNSTD_STATIC_CHECK(64 == INC_ID_BIT + TIMESTAMP_BIT + SERVER_INDEX_BIT + SERVER_ID_BIT + SERVER_TYPE_BIT + SIGN_BIT);

class IClockModule;
class UniqueIDGenerator
{
public:
	//!< ����ID����0λ
	static const long long INC_ID_SHIFT = 0LL;

	//!< ʱ�������14λ
	static const long long TIMESTAMP_SHIFT = INC_ID_SHIFT + INC_ID_BIT;

	//!< ʵ��index����14+30λ
	static const long long SERVER_INDEX_SHIFT = TIMESTAMP_SHIFT + TIMESTAMP_BIT;

	//!< ������ID����14+30+4λ
	static const long long SERVER_ID_SHIFT = SERVER_INDEX_SHIFT + SERVER_INDEX_BIT;

	//!< ��������������14+30+4+11λ
	static const long long SERVER_TYPE_SHIFT = SERVER_ID_SHIFT + SERVER_ID_BIT;

	UNSTD_STATIC_CHECK(SERVER_TYPE_SHIFT + SERVER_TYPE_BIT == 63);
public:
	UniqueIDGenerator();
	~UniqueIDGenerator();

	static inline bool CheckUniqueIDValid(long long unique_id) { return 0 != unique_id; }
	static UniqueIDGenerator* Instance();
	bool Init(SERVER_TYPE server_type, int server_id, int server_index, IClockModule* clock);

	bool GenUniqueID(ARG_OUT long long* unique_id);
private:
	
	bool m_has_init;					//!< �Ƿ��ѳ�ʼ��
	SERVER_TYPE m_server_type;			//!< ����������
	int m_server_id;					//!< ������id
	int m_server_index;					//!< ������index ��ͬһ��������id���ܴ��ڶ��ʵ����
	long long m_last_gen_time;			//!< �ϴ�����uuid��ʱ���
	int m_inc_id;						//!< ����id
	IClockModule* m_clock;				//!< ���ڻ�ȡʱ���

#ifdef _DEBUG
	std::set<long long> m_history_unique_id_set;
#endif
};

#endif