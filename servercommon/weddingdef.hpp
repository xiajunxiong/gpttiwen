#ifndef __WEDDING_DEF_HPP__
#define __WEDDING_DEF_HPP__

#include "servercommon/servercommon.h"

/////////////////////////////// 情缘---婚宴 /////////////////////////////
//场景
//SCENE_TYPE_WEDDING_FB = 42,
//FB_TYPE_WEDDING_FB = 36,	
//BATTLE_MODE_WEDDING_FB,78	
//婚宴id
typedef int WeddingID;

namespace WeddingFB
{


	//阶段类型
	enum PhaseType
	{
		PHASE_TYPE_INVALID = 0,		//				特殊逻辑
		PHASE_TYPE_BROADCAST = 1,	// 宣传阶段:	副本创建
		PHASE_TYPE_WARM = 2,		// 预热阶段:	无
		PHASE_TYPE_RITE = 3,		// 拜堂阶段:	无
		PHASE_TYPE_MEAL = 4,		// 用餐阶段:	无
		PHASE_TYPE_SUB_ACT = 5,		// 活动阶段:	P_1:活动类型(小游戏)

		PHASE_TYPE_MAX,
	};

	//////活动类型(小游戏)
	enum SubActType
	{//													场景角色参数		操作参数
		SUB_ACT_TYPE_XUN_QING_MI_AI = 1,// 寻情觅爱		p:奖励领取标记		无
		SUB_ACT_TYPE_XIN_XIN_XIANG_YIN,	// 心心相印		p:奖励领取标记		无
		SUB_ACT_TYPE_BAO_WEI_XIU_QIU,	// 保卫绣球		p:奖励领取标记[位]	奖励index
		SUB_ACT_TYPE_ZAO_SHENG_GUI_ZI,	// 枣生桂子		p:奖励领取标记[位]	奖励index

		SUB_ACT_TYPE_MAX,
	};

	//公共操作类型
	enum CommonOperType
	{//												配置意义	场景参数		操作参数
		COMMON_OPER_TYPE_ENTER_SCENE,	//进入场景  cs:可进入	进入次数		无效 用enterfb
		COMMON_OPER_TYPE_CANDIES,		//发喜糖	cs:可领取	领取次数		无
		COMMON_OPER_TYPE_RITE,			//拜堂动画	c:播放		已播放			场景参数赋值
		COMMON_OPER_TYPE_MEAL,			//用餐		cs:可领取	领取次数		无
		COMMON_OPER_TYPE_BLESS,			//祝福		cs:可购买	无				商品index
		COMMON_OPER_TYPE_BARRAGE,		//弹幕		cs:可发送	发出次数		无效 用chat
		COMMON_OPER_TYPE_HEAT_REWARD,	//热度奖励	无配置		领取标志(位)	奖励序号	

		COMMON_OPER_TYPE_MAX,
	};

	enum WeddingRegistStatus
	{
		WEDDING_REGIST_STATUS_NULL = 0,		//未预约
		WEDDING_REGIST_STATUS_NOW,			//已预约
		WEDDING_REGIST_STATUS_DONE,			//已举办
	};

	//婚宴预约储存大小
	static const int WEDDING_INFO_SIZE = 200;

	//主办者列表大小
	static const int HOST_NUM = 2;
	//被邀请者列表大小
	static const int MAX_INVITE_SIZE = 100;
	//申请列表大小
	static const int MAX_APPLY_SIZE = 200;

	//婚宴 小游戏
	//保卫绣球 关卡数
	static const int SUB_ACT_BAO_WEI_XIU_QIU_MAX_CFG_SIZE = 4;

////// 存库
#pragma pack(push, 4)
	//主办者信息
	struct HostInfo
	{	
		HostInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			uid = 0;
			memset(name, 0, sizeof(name));
			memset(hold_list, 0, sizeof(hold_list));
			appearance.Reset();
		}

		int uid;		//uid
		
		//存一份主办者信息  防止 主服未连接时 取不到缓存
		GameName name;	//角色名
		int hold_list[10];
		UNSTD_STATIC_CHECK(52 == sizeof(Appearance));
		Appearance appearance;
	};
	typedef char HostInfoHex[sizeof(HostInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(HostInfoHex) < 512);
	//邀请列表
	struct InviteInfo
	{
		InviteInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			count = 0;
			memset(invite_uid_, 0, sizeof(invite_uid_));
		}
		int count;
		int invite_uid_[WeddingFB::MAX_INVITE_SIZE];
	};
	typedef char InviteInfoHex[sizeof(InviteInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(InviteInfoHex) < 1024);

	//其他信息
	struct CommonInfo
	{
		CommonInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			qing_yuan_id = 0;
			buy_invite_times = 0;
			xun_you_type = 0;
		}
		long long qing_yuan_id;
		int buy_invite_times;	//已拓展次数
		int xun_you_type;		//巡游类型
	};
	typedef char CommonInfoHex[sizeof(CommonInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(CommonInfoHex) < 256);

	//婚宴信息 会被序列化
	struct WeddingInfoDBParam
	{
		WeddingInfoDBParam()
		{
			this->Reset();
		}

		void Reset()
		{
			change_state = 0;
			save_index = 0;

			wedding_id = 0;
			begin_time = 0;
			cfg_ver = 0;

			is_finsih = 0;

			for (int i = 0; i < HOST_NUM; i++)
			{
				host_info_[i].Reset();
			}
			
			invite_info.Reset();

			common_info.Reset();
		}
		bool IsValid() const { return 0 != wedding_id; };

		int change_state;
		int save_index;

		WeddingID wedding_id;		//宴会id
		unsigned int begin_time;	//宴会开启时间
		int cfg_ver;				//配置版本

		int is_finsih;				//完成标记
		//主办者
		HostInfo host_info_[HOST_NUM];

		//被邀请者
		InviteInfo invite_info;

		CommonInfo common_info;
	};

	struct HiddenGlobalParam
	{
		HiddenGlobalParam()
		{
			this->Reset();
		}

		void Reset()
		{
			m_creat_wedding_end = 0;
		}

		WeddingID m_creat_wedding_end;
	};

#pragma pack(pop)

};

class WeddingInfoDBList
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	static const int MAX_SIZE = 30;

	int m_save_index_begin;
	int m_count;
	WeddingFB::WeddingInfoDBParam m_wedding_info_list[MAX_SIZE];
};


#endif