
AnimalRaceConfig = {
	ResPath = "animal_race/",

	MAX_ANIMAL_RACE_NUM = 4,
	AnimalCoin = 27396,
	SceneId = 998,
	RaceMaxTime = 50,
	RaceAllTime = 180,

	NONE = -1,

	OperType = {
		role_info = 0,			-- 神兽径赛玩家信息下发
		bet = 1,				-- 玩家下注
		activity_info = 2,		-- 神兽径赛场景活动信息下发
		enter_scene = 3,		-- 玩家进入场景
		exit_scene = 4,			-- 玩家退出场景
		anim_info = 5,			-- 下注时间结束 准备开始播放动画
	},

	InfoType = {
		RoleReady = 0,		-- 玩家信息下发	timestamp - 准备时间:押注时间结束的时间戳 不需要获胜神兽序号
		RoleDoing = 1,		-- 玩家信息下发	timestamp - 比赛中:距离下一次押注时间的时间戳 需要获胜神兽序号
		NotTime = 2,		-- 玩家信息下发 timestamp - 空闲状态:下次押注时间开始的时候 不需要获胜神兽序号
	},

	AnimalPoints = {
		StartX = 120,
		EndX = 170,
		StartY = {
			[1] = 159,
			[2] = 157.6,
			[3] = 156.3,
			[4] = 155,
		}
	}
}