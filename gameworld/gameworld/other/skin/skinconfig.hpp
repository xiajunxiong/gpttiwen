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
		con = 0;		// 体力
		str = 0;		// 力量
		def = 0;		// 强度
		agi = 0;		// 速度
		magic = 0;		// 魔法
		extra_point = 0;
	}

	int skin_id;
	std::set<int> can_wear_pet_list;
	int active_stuff_id; // 激活物品id

	// 宠物属性
	int con;		// 体力
	int str;		// 力量
	int def;		// 强度
	int agi;		// 速度
	int magic;		// 魔法

	//额外加点
	int extra_point;
	int decompose_item_id;	// 分解后获得的道具ID
	int decompose_item_num;	// 单个分解后获得的道具数量
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
	int skin_seq;			// 伙伴皮肤seq (每个伙伴0~11)
	short skin_id;			// 皮肤编号
	ItemID active_stuff_id; // 激活物品id

							// 伙伴属性
	int maxhp;				// 生命
	int maxmp;				// 魔力
	int gongji;				// 攻击
	int fangyu;				// 防御
	int minjie;				// 敏捷
	int jingshen;			// 精神
	int huifu;				// 回复
	int bisha;				// 必杀
	int shanduo;			// 闪躲
	int mingzhong;			// 命中
	int fanji;				// 反击
	int mokang;				// 法抗
	int mogong;				// 魔攻
	int jianren;			// 坚韧
	int bisha_dmg;			// 必杀伤害
	int def_zhongdu;		// 抗中毒
	int def_shihua;			// 抗石化
	int def_hunshui;		// 抗昏睡
	int def_jiuzui;			// 抗酒醉
	int def_hunluan;		// 抗混乱
	int decompose_item_id;	// 分解后获得的道具ID
	int decompose_item_num;	// 单个分解后获得的道具数量
};

struct SkinKey			// 这个结构为key获得PartnerSkinCfgInfo
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
	std::map<int, PartnerSkinCfgInfo> m_skin_id_to_pscfg;		// 伙伴皮肤配置 key:skin_id
};


#endif // !__PARTNER_SKIN_CONFIG_HPP__

