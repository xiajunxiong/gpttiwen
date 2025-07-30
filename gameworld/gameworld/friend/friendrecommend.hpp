#ifndef __FRIENDRECOMMEND_HPP__
#define __FRIENDRECOMMEND_HPP__

//#include "gameworld/global/usercache/usercache.hpp"		// �����ã�Ϊʲô

#include <map>
#include <set>
#include "servercommon/frienddef.hpp"
#include "other/rolemodulemanager.hpp"


enum LAST_USE_TYPE_FLAG
{
	LAST_USE_TYPE_FLAG_BEGIN,		
	LAST_USE_TYPE_FLAG_REFRESH,		// ��һ��
	LAST_USE_TYPE_FLAG_IS_ADD_ALL,	// һ������(���ȼ����ڻ�һ��)

	LAST_USE_TYPE_FLAG_MAX
};

class Role;
struct UserCacheNode;

class FriendRecommend
{
public:	
	
	static const int TEN_SECOND = 10;
public:
	FriendRecommend();
	~FriendRecommend();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager);							// ��ʼ��
	void GetInitParam(FriendRecommendParam* param);
	bool RefreshFriendRecommendList(bool is_notice = false);		//ɸѡ���ѣ��Ƿ�������ߺ���
	void RefreshOneFriendRecommend(int index);						//ˢ�º����Ƽ���
	void SendFriendRecommend();										//�������ݵ��ͻ���
	bool IsCanRefresh(int type_flag);								//�Ƿ����ˢ�º���
	void SetNextRefreshTimestamp(int type_flag);					//�����´�ˢ�º���ʱ���
	bool IsLastTimeRefreshUid(int uid);								//�ж��Ƿ����ϴ�ˢ�µ�uid

	void AddAllRecommendFriend();					
	void AddOneRecommendFriend(int index);
	void ClearData();
private:

	RoleModuleManager *	m_role_module_mgr;
	bool m_is_get_recommend_list;

	int m_last_use_type_flag;											// ��һ�ε����ʶ  LAST_USE_TYPE_FLAG_MAX
	int m_recommend_uid_count;											// ��������
	int m_recommend_uid_list[FriendRecommendParam::RECOMMEND_LIST_MAX];	// ��¼����ϴ�ˢ�µ�uid_list
	FriendRecommendParam 	m_param;									// �Ƽ��б�
	unsigned int m_next_refresh_friend_timestamp;						// �´�ˢ���Ƽ�����ʱ���
};


#endif	// FRIENDRECOMMEND_HPP