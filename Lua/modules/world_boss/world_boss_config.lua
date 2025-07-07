
WorldBossConfig = {
  ResPath = "world_boss/",
	
	RANK_NUM_MAX = 20,

  OperType = {
		info = 0,						--请求信息
		cancel_info = 1,    			--取消信息
	},

	RankType = {
		hurt = 1,
		heal = 2,
		luck = 3,
	},

	RewardNameSp = {
		[1] = "_LocShuChuBang",
		[2] = "_LocZhiLiaoBang",
		[3] = "_LocXingYunBang",
	},

	WildBossNotice = {
		monster_battle = 0,			--怪物正在战斗中
		not_enough_member = 1,	--队伍人数不足
	}
}