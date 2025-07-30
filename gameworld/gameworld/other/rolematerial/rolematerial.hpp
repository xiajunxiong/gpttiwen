#ifndef __ROLE_MATERIAL_HPP__
#define __ROLE_MATERIAL_HPP__

#include "servercommon/rolematerialdef.hpp"

struct MaterialKey
{
	MaterialKey() : chapter_id(0), level_seq(0) {}
	MaterialKey(char _chapter_id, char _level_seq) : chapter_id(_chapter_id), level_seq(_level_seq) {}

	bool operator< (const MaterialKey& index) const
	{
		if (chapter_id != index.chapter_id)
		{
			return chapter_id < index.chapter_id;
		}
		else
		{
			return level_seq < index.level_seq;
		}
	}

	char chapter_id;		// �ֲ�ȼ���
	char level_seq;			// �÷ֲ�ȼ����е�seq
};

class RoleModuleManager;
class RoleMaterial
{
public:
	RoleMaterial();
	~RoleMaterial();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const MaterialParam & param);
	void GetInitParam(MaterialParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	bool IsCanEnterFB(int fb_seq);
	bool IsCanEnterFB(int chapter_id, int level_seq);
	bool IsCanFastFb(int chapter_id, int level_seq);
	bool IsCanEnterFBByTimes(int chapter_id, int level_seq);
	int GetPassLevelByChapter(int chapter_id);						//��ȡ���½�ͨ������߹ؿ�(����level_seq+1)
	int GetPassTimesCount() { return m_param.day_pass_times_count; }

	bool IsCanTips();

	void OnBuyTimes(int fb_seq);
	void SendPassReward(int chapter_id, int level_seq, unsigned char condition_flag);
	void OnFastPassFb(int fb_seq);
	void OnFetchBoxReward(int seq);
		
	void SendAllInfo();
	void SendSingleInfo(int level_data_index);
	void SendOtherInfo();

private:
	bool IsInValid(int level_data_seq);
	bool IsInValid(int chapter_id, int level_seq);
	int GetLevelIntegralNum(int level_data_seq);

	RoleModuleManager * m_module_mgr;

	MaterialParam  m_param;
	std::map<MaterialKey, short> m_level_data_map;			// second��Ӧ��ʱm_param��level_data_list�����±�
	std::map<int, int> m_chapter_map;						// key:�½�ID second:����

	int max_pass_level_seq[MAX_MATERIAL_CHAPTER_NUM];		//�����½ڵ����ͨ�عؿ�seq
	
};

#endif