#ifndef __COLLECTION_CONFIG_HPP__
#define __COLLECTION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <map>
#include <vector>
static const int COLLECTION__FEE_MAX_NUM = 20;

//Í¼¼ø×éÆ·ÖÊÀàÐÍ
enum COLLECTION_COLOR_TYPE
{
	COLLECTION_COLOR_TYPE_INVALID = 0,

	COLLECTION_COLOR_TYPE_WHITE = 1,		//°×
	COLLECTION_COLOR_TYPE_GREEN = 2,		//ÂÌ
	COLLECTION_COLOR_TYPE_BLUE = 3,			//À¶
	COLLECTION_COLOR_TYPE_PURPLE = 4,		//×Ï
	COLLECTION_COLOR_TYPE_ORANGE = 5,		//³È
	COLLECTION_COLOR_TYPE_RED = 6,			//ºì

	COLLECTION_COLOR_TYPE_MAX
};

struct CollectionCfg
{
	CollectionCfg()
	{
		seq = 0;
		page = 0;
		color = 0;
		limit_prof = 0;
		is_back_card = false;
		maxhp = 0;
		maxmp = 0;
		gongji = 0;
		fangyu = 0;
		minjie = 0;
		jingshen = 0;
		hueifu = 0;
		fee_num = 0;
		memset(collection_itemfee_id, 0, sizeof(collection_itemfee_id));
		memset(collection_itemfee_num, 0, sizeof(collection_itemfee_num));
	}

	int seq;
	int page;
	int color;
	int limit_prof;
	// ×ªÖ°ºóÊÇ·ñ·µ»¹Í¼¼ø
	bool is_back_card;

	int maxhp;
	int maxmp;
	int gongji;
	int fangyu;
	int minjie;
	int jingshen;
	int hueifu;
	int fee_num;
	int collection_itemfee_id[COLLECTION__FEE_MAX_NUM];
	int collection_itemfee_num[COLLECTION__FEE_MAX_NUM];

};

struct CollectionConvertCfg
{
	CollectionConvertCfg()
	{}

	int seq;
	int convert_page;
	int convert_itemfee;
	int convert_itemnum;
	ItemConfigData get_item;
};

struct CollectionActivationCfg
{
	CollectionActivationCfg() : seq(0),page(0), id(0), maxhp(0), maxmp(0), gongji(0), fangyu(0), minjie(0), jingshen(0), huifu(0), bisha(0), jianren(0), fanji(0),
		mingzhong(0), shanduo(0), def_zhongdu(0), def_shihua(0), def_hunshui(0), def_jiuzui(0), def_hunluan(0), mogong(0), mokang(0), bisha_dmg(0) {}
	int seq;
	int page;
	int	id;
	int	maxhp;
	int	maxmp;
	int	gongji;
	int	fangyu;
	int	minjie;
	int	jingshen;
	int	huifu;
	int	bisha;
	int	jianren;
	int	fanji;
	int	mingzhong;
	int	shanduo;
	int	def_zhongdu;
	int	def_shihua;
	int	def_hunshui;
	int	def_jiuzui;
	int	def_hunluan;
	int	mogong;
	int	mokang;
	int	bisha_dmg;
};

struct CollectionTreasuresCfg
{
	CollectionTreasuresCfg() : seq(0), grade(0), id(0), attr_type_1(0), attr_value_1(0), attr_type_2(0), attr_value_2(0),
		attr_type_3(0), attr_value_3(0), attr_type_4(0), attr_value_4(0), consume_quantity(0) {}
	int seq;
	int	grade;
	int	id;
	int attr_type_1;
	int	attr_value_1;
	int	attr_type_2;
	int	attr_value_2;
	int	attr_type_3;
	int	attr_value_3;
	int	attr_type_4;
	int	attr_value_4;
	int	consume_quantity;
};

class CollectionConfig : public ILogicConfig
{
public:
	CollectionConfig();
	enum COLLECTION_TYPE
	{
		COLLECTION_TYPE_PET = 1,				//³èÎï
		COLLECTION_TYPE_PARTNER = 2,			//»ï°é
		COLLECTION_TYPE_FABAO = 3,				//·¨±¦
		COLLECTION_TYPE_HEART_SKILL = 4,		//ÐÄ·¨
		COLLECTION_TYPE_PET_SOUL = 5,			//³èÎï»êÆ÷  id: ³èÎï»êÆ÷id
		COLLECTION_TYPE_HOLY_EQUIP = 6,			//Ê¥Æ÷

		COLLECTION_TYPE_MAX,
	};
	virtual ~CollectionConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const CollectionCfg* GetCollectionCfg(int seq);
	const CollectionConvertCfg* GetConvertCfg(int seq);
	int GetMaxSeq() { return m_max_seq; }
	int GetIncludedMaxSeq() { return m_max_included_seq; }
	const CollectionActivationCfg * GetPetCollectBySeq(int seq);
	const CollectionActivationCfg * GetPetCollectByPetId(int pet_id);
	const CollectionActivationCfg * GetPartnerCollectByPartnerId(int partner_id);
	const CollectionActivationCfg * GetFabaoCollectByFabaoId(int fabao_id);
	
	const CollectionActivationCfg * GetPetSoulCollectByPetSoulId(int pet_soul_id);		//³èÎï»êÆ÷
	const CollectionActivationCfg * GetHolyEquipCollectByHolyEquipId(int holy_equip_id);	// Ê¥Æ÷

	const CollectionActivationCfg * GetCollectionActivationCfg(int type, int id);
	const std::vector<CollectionActivationCfg> * GetCollectionByType(int type);

	const CollectionTreasuresCfg * GetTreasuresCfg(int seq, int grade);
private:
	int InitCollectionCfg(TiXmlElement *RootElement);
	int InitConvertCfg(TiXmlElement *RootElement);
	int InitCollectCfg(TiXmlElement *RootElement);
	int InitTreasuresCfg(TiXmlElement *RootElement);

	int m_max_seq;
	std::map<int, CollectionCfg> m_collection_map;
	std::map<int, CollectionConvertCfg> m_convert_map;
	int m_max_included_seq;
	std::map<int, CollectionActivationCfg> m_collection_activation_seq_map;		//¼ü - seq
	std::map<int, std::vector<CollectionActivationCfg> > m_collection_activation_map;			//¼ü - ·ÖÒ³£¨1³èÎï£¬2»ï°é£¬3·¨±¦£©


	std::map<int, std::map<int, CollectionTreasuresCfg> > treasures_map;
};

#endif // !__COLLECTION_CONFIG_HPP__
