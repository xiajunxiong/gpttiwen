#include "skinconfig.hpp"
#include "skindef.h"
#include "servercommon/partnerdef.h"
#include "item/itembase.h"

SkinConfig::SkinConfig()
{
}

SkinConfig::~SkinConfig()
{
}

bool SkinConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("partner_skin", InitPartnerSkin);
	LOAD_CONFIG("pet_skin", InitSkinPetCfg);

	return true;
}


const PartnerSkinCfgInfo * SkinConfig::GetPartnerSkinCfgInfo(SkinKey key)
{
	std::map<SkinKey, PartnerSkinCfgInfo>::const_iterator it = m_key_to_pscfg.find(key);
	if (it != m_key_to_pscfg.end()) return &it->second;

	return NULL;
}

const PartnerSkinCfgInfo * SkinConfig::GetPartnerSkinCfgInfoByActiveId(ItemID active_id)
{
	std::map<ItemID, PartnerSkinCfgInfo>::const_iterator it = m_active_id_to_pscfg.find(active_id);
	if (it != m_active_id_to_pscfg.end()) return &it->second;

	return NULL;
}

const PartnerSkinCfgInfo * SkinConfig::GetPartnerSkinCfgInfoBySkinId(int skin_id)
{
	std::map<int, PartnerSkinCfgInfo>::const_iterator it = m_skin_id_to_pscfg.find(skin_id);
	if(it != m_skin_id_to_pscfg.end()) return &it->second;

	return NULL;
}

const SkinPetCfg * SkinConfig::GetSkinPetCfgBySkinId(short skin_id)
{
	std::map<int, SkinPetCfg>::iterator iter = m_skin_pet_cfg.find(skin_id);
	if (iter != m_skin_pet_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

int SkinConfig::InitPartnerSkin(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		PartnerSkinCfgInfo ps_cfg;

		if (!GetSubNodeValue(root_element, "partner_id", ps_cfg.partner_id) || ps_cfg.partner_id <= 0 || ps_cfg.partner_id > PARTNER_NUM_MAX) return -1;
		if (!GetSubNodeValue(root_element, "seq", ps_cfg.skin_seq) || ps_cfg.skin_seq < 0 || ps_cfg.skin_seq >= PARTNER_SKIN_ONCE) return -2;
		if (!GetSubNodeValue(root_element, "partner_skin_id", ps_cfg.skin_id) || ps_cfg.skin_id <= 0 || ps_cfg.skin_id >= PARTNER_SKIN_MAX) return -3;
		if (!GetSubNodeValue(root_element, "active_stuff_id", ps_cfg.active_stuff_id) || NULL == ITEMPOOL->GetItem(ps_cfg.active_stuff_id)) return -4;

		if (!GetSubNodeValue(root_element, "maxhp", ps_cfg.maxhp) || ps_cfg.maxhp < 0) return -7;
		if (!GetSubNodeValue(root_element, "maxmp", ps_cfg.maxmp) || ps_cfg.maxmp < 0) return -8;
		if (!GetSubNodeValue(root_element, "gongji", ps_cfg.gongji) || ps_cfg.gongji < 0) return -9;
		if (!GetSubNodeValue(root_element, "fangyu", ps_cfg.fangyu) || ps_cfg.fangyu < 0) return -10;
		if (!GetSubNodeValue(root_element, "minjie", ps_cfg.minjie) || ps_cfg.minjie < 0) return -11;
		if (!GetSubNodeValue(root_element, "jingshen", ps_cfg.jingshen) || ps_cfg.jingshen < 0) return -12;
		if (!GetSubNodeValue(root_element, "huifu", ps_cfg.huifu) || ps_cfg.huifu < 0) return -13;
		if (!GetSubNodeValue(root_element, "bisha", ps_cfg.bisha) || ps_cfg.bisha < 0) return -14;
		if (!GetSubNodeValue(root_element, "shanduo", ps_cfg.shanduo) || ps_cfg.shanduo < 0) return -15;
		if (!GetSubNodeValue(root_element, "mingzhong", ps_cfg.mingzhong) || ps_cfg.mingzhong < 0) return -16;
		if (!GetSubNodeValue(root_element, "fanji", ps_cfg.fanji) || ps_cfg.fanji < 0) return -17;
		if (!GetSubNodeValue(root_element, "mokang", ps_cfg.mokang) || ps_cfg.mokang < 0) return -18;
		if (!GetSubNodeValue(root_element, "mogong", ps_cfg.mogong) || ps_cfg.mogong < 0) return -19;
		if (!GetSubNodeValue(root_element, "jianren", ps_cfg.jianren) || ps_cfg.jianren < 0) return -20;
		if (!GetSubNodeValue(root_element, "bisha_dmg", ps_cfg.bisha_dmg) || ps_cfg.bisha_dmg < 0) return -21;
		if (!GetSubNodeValue(root_element, "def_zhongdu", ps_cfg.def_zhongdu) || ps_cfg.def_zhongdu < 0) return -22;
		if (!GetSubNodeValue(root_element, "def_shihua", ps_cfg.def_shihua) || ps_cfg.def_shihua < 0) return -23;
		if (!GetSubNodeValue(root_element, "def_hunshui", ps_cfg.def_hunshui) || ps_cfg.def_hunshui < 0) return -24;
		if (!GetSubNodeValue(root_element, "def_jiuzui", ps_cfg.def_jiuzui) || ps_cfg.def_jiuzui < 0) return -25;
		if (!GetSubNodeValue(root_element, "def_hunluan", ps_cfg.def_hunluan) || ps_cfg.def_hunluan < 0) return -26;
		if (!GetSubNodeValue(root_element, "decompose_item", ps_cfg.decompose_item_id) || NULL == ITEMPOOL->GetItem(ps_cfg.decompose_item_id)) return -27;
		if (!GetSubNodeValue(root_element, "decompose_num", ps_cfg.decompose_item_num) || ps_cfg.decompose_item_num <= 0) return -28;

		SkinKey key(ps_cfg.partner_id, ps_cfg.skin_seq);
		m_key_to_pscfg[key] = ps_cfg;

		m_active_id_to_pscfg[ps_cfg.active_stuff_id] = ps_cfg;
		m_skin_id_to_pscfg[ps_cfg.skin_id] = ps_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SkinConfig::InitSkinPetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int read_index = 0;
	while (root_element != NULL)
	{
		SkinPetCfg cfg;

		if (!GetSubNodeValue(root_element, "pet_skin_id", cfg.skin_id) || cfg.skin_id < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "active_stuff_id", cfg.active_stuff_id) || cfg.active_stuff_id < 0) return -__LINE__;

		// ³èÎï
		if (!GetSubNodeValue(root_element, "con", cfg.con) || cfg.con < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "str", cfg.str) || cfg.str < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "def", cfg.def) || cfg.def < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "agi", cfg.agi) || cfg.agi < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "magic", cfg.magic) || cfg.magic < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "potential", cfg.extra_point) || cfg.extra_point < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "decompose_item", cfg.decompose_item_id) || cfg.decompose_item_id < 0) return -__LINE__;
		if (!GetSubNodeValue(root_element, "decompose_num", cfg.decompose_item_num) || cfg.decompose_item_num < 0) return -__LINE__;


		TiXmlElement * can_use_pet_id_list = root_element->FirstChildElement("pet_id");
		if (NULL == can_use_pet_id_list) return -__LINE__;

		TiXmlElement * reward_group_id = can_use_pet_id_list->FirstChildElement("node");
		while (NULL != reward_group_id)
		{
			int tmp = 0;
			if (!GetNodeValue(reward_group_id, tmp))
			{
				return -__LINE__;
			}
			cfg.can_wear_pet_list.insert(tmp);
			reward_group_id = reward_group_id->NextSiblingElement();
		}

		m_skin_pet_cfg[cfg.skin_id] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
