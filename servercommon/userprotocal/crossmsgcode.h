#ifndef __CROSS_MSG_CODE_H__
#define __CROSS_MSG_CODE_H__

namespace Protocol
{
	// ע�� ÿ�ֿ���淨Э��ŵ���һ���� global����Э���ת������Ӧcross

	enum
	{
		CROSS_SERVER_MSG_BEGIN = 14000,
		CROSS_SERVER_MSG_END = 15000,
	};

	enum 
	{
		MT_RETURN_ORIGINAL_SERVER_SC = 14001,								// ֪ͨ��һص�ԭ��
		MT_CROSS_GET_PERSON_RANK_LIST_ACK_SC = 14002,						// �·�������а��б�
		MT_CROSS_PERSON_RANK_BASE_INFO_SC = 14003,							// �·�������а������Ϣ��ÿ���񵥵Ŀ����һ����������ӵķ�����id�б�
		MT_CROSS_GET_PERSON_RANK_ALL_SERVER_LIST_ACK_SC = 14004,			// ����ܰ�
		MT_CROSS_GET_PET_RANK_LIST_ACK_SC = 14005,							// �·�������а��б�
		MT_CROSS_GET_PET_RANK_ALL_SERVER_LIST_ACK_SC = 14006,				// ����ܰ�

		MT_CROSS_GET_PERSON_RANK_LIST_CS = 14050,							// ��ȡ������а��б�
		MT_CROSS_GET_PERSON_RANK_BASE_INFO_CS = 14051,						// ��ȡ������а������Ϣ
		MT_CROSS_GET_PERSON_RANK_ALL_SERER_LIST_CS = 14052,					// �����ܰ�
		MT_CROSS_GET_PET_RANK_LIST_CS = 14053,								// ��ȡ������а��б�
		MT_CROSS_GET_PET_RANK_ALL_SERER_LIST_CS = 14054,					// �����ܰ�
		MT_CROSS_GET_PET_RANK_DETAILED_INFO_CS = 14055,						// �������������а�ĳһ��������Ϣ
		MT_CROSS_GET_PET_RANK_ALL_DETAILED_INFO_CS = 14056,					// �����������ܰ�ĳһ��������Ϣ

		MT_CROSS_ROLE_QUERY_FOLLOW_UP_INFO_SC = 14060,						// �����ѯ������Ϣ׷���·�		

		MT_CROSS_GUILD_RANK_LIST_REQ_SC = 14070,							// �������������а���Ϣ
		MT_CROSS_GUILD_RANK_LIST_ACK_SC = 14071,							// ���Ϳ���������а���Ϣ[����]
		MT_CROSS_GUILD_RANK_LIST_ALL_SERVER_SC = 14072,						// ���Ϳ���������а���Ϣ[�ܰ�]
	};
}

#endif	// __CROSS_MSG_CODE_H__

