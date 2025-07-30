#pragma once
#ifndef __PARTNER_SKIN_CONFIG_HPP__
#define __PARTNER_SKIN_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"

#include <map>
#include <set>

struct SkinPetCfg
{
	SkinPetCfg()
	{
		skin_id = 0;
		active_stuff_id = 0;
		con = 0;		// ����
		str = 0;		// ����
		def = 0;		// ǿ��
		agi = 0;		// �ٶ�
		magic = 0;		// ħ��
		extra_point = 0;
	}

	int skin_id;
	std::set<int> can_wear_pet_list;
	int active_stuff_id; // ������Ʒid

	// ��������
	int con;		// ����
	int str;		// ����
	int def;		// ǿ��
	int agi;		// �ٶ�
	int magic;		// ħ��

	//����ӵ�
	int extra_point;
	int decompose_item_id;	// �ֽ���õĵ���ID
	int decompose_item_num;	// �����ֽ���õĵ�������
};

struct PartnerSkinCfgInfo
{
	PartnerSkinCfgInfo()
	{
		partner_id = 0;
		skin_seq = 0;
		skin_id = 0;
		active_stuff_id = 0;

		maxhp = 0;
		maxmp = 0;
		gongji = 0;
		fangyu = 0;
		minjie = 0;
		jingshen = 0;
		huifu = 0;
		bisha = 0;
		shanduo = 0;
		mingzhong = 0;
		fanji = 0;
		mokang = 0;
		mogong = 0;
		jianren = 0;
		bisha_dmg = 0;
		def_zhongdu = 0;
		def_shihua = 0;
		def_hunshui = 0;
		def_jiuzui = 0;
		def_hunluan = 0;
		decompose_item_id = 0;
		decompose_item_num = 0;
	}
	int partner_id;
	int skin_seq;			// ���Ƥ��seq (ÿ�����0~11)
	short skin_id;			// Ƥ�����
	ItemID active_stuff_id; // ������Ʒid

							// �������
	int maxhp;				// ����
	int maxmp;				// ħ��
	int gongji;				// ����
	int fangyu;				// ����
	int minjie;				// ����
	int jingshen;			// ����
	int huifu;				// �ظ�
	int bisha;				// ��ɱ
	int shanduo;			// ����
	int mingzhong;			// ����
	int fanji;				// ����
	int mokang;				// ����
	int mogong;				// ħ��
	int jianren;			// ����
	int bisha_dmg;			// ��ɱ�˺�
	int def_zhongdu;		// ���ж�
	int def_shihua;			// ��ʯ��
	int def_hunshui;		// ����˯
	int def_jiuzui;			// ������
	int def_hunluan;		// ������
	int decompose_item_id;	// �ֽ���õĵ���ID
	int decompose_item_num;	// �����ֽ���õĵ�������
};

struct SkinKey			// ����ṹΪkey���PartnerSkinCfgInfo
{
	enum KeyBaseNum			
	{
		KEY_BASE = 100,		
	};
	SkinKey(int partner_id_, short skin_seq_)
	{
		partner_id = partner_id_;
		skin_seq = skin_seq_;
	}

	bool operator<(const SkinKey &key_)const
	{
		return (partner_id * KEY_BASE + skin_seq) < (key_.partner_id * KEY_BASE + key_.skin_seq);
	}

	int partner_id;
	short skin_seq;
};

class SkinConfig : public ILogicConfig
{
public:
	SkinConfig();
	virtual ~SkinConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PartnerSkinCfgInfo* GetPartnerSkinCfgInfo(SkinKey key);
	const PartnerSkinCfgInfo* GetPartnerSkinCfgInfoByActiveId(ItemID active_id);
	const PartnerSkinCfgInfo* GetPartnerSkinCfgInfoBySkinId(int skin_id);

	const SkinPetCfg * GetSkinPetCfgBySkinId(short skin_id);

private:
	int InitPartnerSkin(TiXmlElement *RootElement);
	int InitSkinPetCfg(TiXmlElement * RootElement);
private:
	
	std::map<int, SkinPetCfg> m_skin_pet_cfg;					// skin_id  -- >  SkinPetCfg

	std::map<SkinKey, PartnerSkinCfgInfo> m_key_to_pscfg;		// partner_skin_key  == > PartnerSkinCfgInfo
	std::map<ItemID, PartnerSkinCfgInfo>  m_active_id_to_pscfg;	//partner_skin_active_id   -- > PartnerSkinCfgInfo
	std::map<int, PartnerSkinCfgInfo> m_skin_id_to_pscfg;		// ���Ƥ������ key:skin_id
};


#endif // !__PARTNER_SKIN_CONFIG_HPP__

