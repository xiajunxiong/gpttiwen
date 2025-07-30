/***************************************************************************
FileName: msgcsrank.h
Author: fyh
Description: ���а� �ͻ���-������ ���Э�� 
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
fyh		   2011/09/06/ 17:48:05     1.0           ����
**************************************************************************/

#ifndef __MSG_CS_RANK_HPP__
#define __MSG_CS_RANK_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSGetPersonRankList				// 10050 �ͻ��������������а���Ϣ
	{
	public:
		CSGetPersonRankList();
		enum SPECIAL_TYPE
		{
			SPECIAL_TYPE_0 = 0,//��
			SPECIAL_TYPE_1 = 1,//����
			SPECIAL_TYPE_2 = 2,//����
			SPECIAL_TYPE_MAX,
		};
		MessageHeader header;

		int rank_type;    
		int special_type;					// ��������
		short rank_start;					// ���а����д�0��ʼ  ����ȡ1-100  ��  start :0 end :99
		short rank_end;
	};

	class CSGetPetRankList					//10056
	{
	public:
		CSGetPetRankList();

		MessageHeader header;

		int rank_type;
		int from_index;
		int to_index;
	};

	class CSGetPersonRankTopUser
	{
	public:
		CSGetPersonRankTopUser();

		MessageHeader header;

		int rank_type;
	};

	class CSGetWorldLevel
	{
	public:
		CSGetWorldLevel();

		MessageHeader header;
	};

	class CSGetGuildRankList
	{
	public:
		CSGetGuildRankList();

		MessageHeader header;

		int rank_type;
		short rank_start;					// ���а����д�0��ʼ  ����ȡ1-100  ��  start :0 end :99
		short rank_end;
	};

	class CSGetTeamRankList
	{
	public:
		CSGetTeamRankList();

		MessageHeader header;

		int rank_type;    
	};

	class CSGetRankRefreshTime				//!< 10057 �������а�ˢ��ʱ��
	{
	public:
		CSGetRankRefreshTime();
		MessageHeader header;
	};

	class CSGetRankPetDetailedInfo			//!< 10059 ����������а�ĳһ��������Ϣ
	{
	public:
		CSGetRankPetDetailedInfo();
		MessageHeader header;

		int target_uid;
		int rank_type;
		unsigned long long pet_unique_item_id;
	};

}

#pragma pack(pop)

#endif // __MSG_CS_RANK_HPP__


