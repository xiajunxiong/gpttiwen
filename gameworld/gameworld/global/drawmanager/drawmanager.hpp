#ifndef __DRAW_MANAGER_HPP__
#define __DRAW_MANAGER_HPP__

#include "servercommon/struct/global/drawdef.hpp"

/*
翻牌管理器  只负责 奖励发放和选择index的通知 (手动领取, 计时自动下发)

	数据结构:
		奖励组id   开服后从1000 开始递增
		储存map<奖励组id, 奖励组信息> (不存库)

	翻牌管理器:
		负责向 全队 广播 队员 的翻牌选择index(客户端展示用  实际奖励在在创建奖励组时就已经确定)
		在手动翻牌时 可以下发对应的奖励 并 广播翻牌选择   TODO:传入一个静态函数指针用来支持一些特殊处理
		在超时时	 下发对应奖励后广播		之后会 删除 奖励组 信息

*/
/*使用
	创建
	DrawGroupInfo group_info;
	for(int i = 0; i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		group_info.draw_count++;
		group_info.draw_info_list[i].role_uid = 拥有者id;
		group_info.draw_info_list[uid].draw_index = 奖励领取状态;

		group_info.draw_info_list[uid].money_type[奖励货币类型] = 奖励数量;

		//奖励物品
		group_info.draw_info_list[uid].item_list.inster(奖励物品);
		//或者 自动产生物品参数并插入 DrawManager::Instance().PutRewardItem(group_info.draw_info_list[uid], 物品数量, 物品列表)

		//信息通知协议并没有整合到一起 可能需要取一些信息到通知协议里
		if (角色在线)
		{
			role.getname(协议要用的角色名);
		}
		else
		{
			 DrawManager::Instance().GetRoleOrRobotName(拥有者id,协议要用的角色名);
		}
	}
	//设置失效时间(秒) 失效会后自动发奖励
	group_info.out_time = now_time + xxx;

	//打乱奖励(一般不需要, 奖励本身就是随机的)
	//DrawManager::Instance().ShuffDrawGroupByUid(group_info);

	//加入管理器  
	DrawManager::Instance().AddDrawGroup(group_info);
	
	//调用后才会传出自动分配的奖励组id
	group_info.draw_group_id;

	//目前只要失效时间没到 就允许对奖励组进行任何操作
	//可以在 其他系统中 获取奖励组进行特殊处理
	//TODO:通过功能分类 将特殊操作整合进来 如奖励翻倍 重随.....
*/

//奖励组 储存map<奖励组id, 奖励组信息>
typedef std::map<int, DrawGroupInfo> DrawGroupMap;

//奖励组 超时map<失效时间戳, 奖励组id>
typedef std::multimap<unsigned int, int> DrawGroupOutTimeMap;

//uidset<uid, 奖励组id>
typedef std::map<int, int> DrawUIDMap;

class Role;

class DrawManager
{
public:

	static DrawManager & Instance();

	void OnServerStart();
	void OnServerStop();

	void OnRoleLogOut(Role * role);

	void Update(unsigned long interval, time_t now_second);

	void OnReq(Role *role, int type, int param_1, int param_2);

	/*
	添加奖励组
	deaw_group_info 传入时设置	draw_group_type 奖励组 类型
								draw_count		翻牌奖励数
								draw_info		各奖励的所有者及奖励
	need_rand		是否打乱 奖励的所有者 与 奖励
	*/
	void AddDrawGroup(DrawGroupInfo &deaw_group_info, int draw_group_type);

	//获取奖励组信息
	const DrawGroupInfo *GetDrawGroup(int draw_group_id) const;

	//领取请求(手动翻牌)
	void OnRoleDraw(Role *role, int draw_group_id, int draw_index);

	//发放奖励组全部奖励
	void GiveAllDraw(DrawGroupInfo &draw_group_info);

	//发放随机奖励(单角色)
	void GiveRandDraw(int uid, DrawGroupInfo &draw_group_info, Role *role = NULL);

	bool IsDrawing(int uid);

	//放入奖励物品(随机物品参数)
	static void PutRewardItem(DrawGroupInfo::DrawItem &reward_info, int num, const ItemConfigData *item_cfg_list, int prof = 0);
	//洗牌  打乱奖励 (只对uid洗牌,,但按传入的uid排列)
	static void ShuffDrawGroupByUid(DrawGroupInfo &draw_group_info);

	//向role发放reward_info里的奖励  NULL == *role 时 根据reward_info.uid 发邮件
	static void SendGroupReward(DrawGroupInfo::DrawItem reward_info, int group_type, Role *role = NULL);

	//获得角色或机器人名
	static void GetRoleOrRobotName(int role_uid, GameName &name);

	static void GetItemInfoType(ItemDrawInfo &info_type, const ItemDataWrapper & item_Data);

private:
	DrawManager();
	~DrawManager();

	//检测超时(迭代器可能失效)
	void CheckOutTime(unsigned int now_second, bool clear_all = false);

	void GetNewDrawGroupId(int &new_draw_group_id);
	bool IsAllUserFetch(DrawGroupInfo & draw_group_info);		//检测该翻牌中玩家是否全都领取了

	//超时检测
	unsigned int m_next_check_timeout;
	DrawGroupOutTimeMap m_draw_group_out_time;

	
	int m_draw_group_id_auto;		//奖励组 自增id
	//奖励组map
	DrawGroupMap m_draw_group;

	//正在翻牌的角色记录 一般一个角色只会有1个未领取的奖励组 所以不考虑多个的情况
	DrawUIDMap m_draw_uid_map;
};

//修改后需通知客户端修改协议解析函数		//todo:gfp
UNSTD_STATIC_CHECK(sizeof(ItemDataWrapper) == 1052);
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) == 1024);

#endif