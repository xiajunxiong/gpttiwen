TombEffectConfig = {
	[1] = {path = "effect/2015000.prefab", auto_release = true, resident = false},
	[2] = {path = "effect/2015001.prefab", auto_release = true, resident = false},
	[3] = {path = "effect/2015002.prefab", auto_release = true, resident = false},
	[4] = {path = "effect/2015004.prefab", auto_release = true, resident = false},
	[5] = {path = "effect/2015005.prefab", auto_release = true, resident = false},
	[6] = {path = "effect/2015006.prefab", auto_release = true, resident = false},
	[7] = {path = "effect/2015007.prefab", auto_release = true, resident = false},
	[8] = {path = "effect/2015009.prefab", auto_release = true, resident = false},
	[9] = {path = "effect/2015010.prefab", auto_release = true, resident = false},
	[10] = {path = "effect/2015011.prefab", auto_release = true, resident = false},
	[11] = {path = "effect/2015012.prefab", auto_release = true, resident = false},
	[12] = {path = "effect/2015013.prefab", auto_release = true, resident = false},
	[13] = {path = "effect/2015014.prefab", auto_release = true, resident = false},
}

TombRaiderEnum = {
	Catch = 1,           -- 抓捕
	Boost = 2,           -- 疾走
	TransMits = 3,       -- 传送
	UnLock = 4,          -- 开锁
	Rescue = 5,          -- 救援
	Alpha = 6,           -- 神隐
	BurstBoost = 7,      -- 轻身符
	TransMitsItem = 9,   -- 天行梭
	Scarecrow = 10,      -- 稻草人
	AlphaItem = 13,      -- 隐身衣
	HyperMove = 14,      -- 挪移尺
}

TombRaiderConfig = {
	SceneId = 313,
	
    RankSp = {
		[1] = "DiYiMing",
		[2] = "DiErMing",
		[3] = "DiSanMing",
	},

    SkillGroup = {
		[0] = {
			[1] = 1,
			[2] = 2,
			[3] = 3,
		},
		[1] = {
			[1] = 4,
			[2] = 5,
			[3] = 6,
		}
	},
	
	OperType = {
		SkillDrop = 1, -- 未启用 丢弃物品技能
		CatchItem = 2, -- 获取物品技能 param1 技能id
		UseSkill = 3, -- 施展技能 param1 技能id, param2 目标objid
		AccpetTransmist = 4, --接受传送邀请 param1 邀请uid
		MarkMap = 5, --标记地图
		StopSkill = 6, -- 中断技能
	},


	-- 释放特效
	-- is_follow 是否生成在对象上， is_special 是否特殊处理
	SkillEffectConfig = {
		[1] = {eff = TombEffectConfig[4],is_follow = true},
		[2] = {eff = TombEffectConfig[8],is_follow = true,is_special = true},
		[3] = {eff = TombEffectConfig[2],is_follow = false},
		-- 开宝箱，生成在人的开箱人的手上，播采集动画
		[4] = {eff = TombEffectConfig[11],is_follow = true,is_special = true}, 
		[5] = {eff = TombEffectConfig[7],is_follow = false},
		[6] = {eff = TombEffectConfig[10],is_follow = true},
		[7] = {eff = TombEffectConfig[12],is_follow = true},
		[8] = {eff = TombEffectConfig[13],is_follow = false},
		[9] = {eff = TombEffectConfig[2],is_follow = false},
		[12] = {eff = TombEffectConfig[13],is_follow = false}, 
		[13] = {eff = TombEffectConfig[10],is_follow = true}, 
		[14] = {eff = TombEffectConfig[1],is_follow = true}, 
	},

	-- 观察特效
	-- is_follow 是否生成在对象上， is_special 是否特殊处理
	SkillObserveEffectConfig = {
		[1] = {eff = TombEffectConfig[4],is_follow = true},
		[2] = {eff = TombEffectConfig[5],is_follow = true,is_special = true},
		[3] = {eff = TombEffectConfig[8],is_follow = true,self_complete = true},
		[4] = {eff = TombEffectConfig[2],is_follow = false}, 
		[5] = {eff = TombEffectConfig[3],is_follow = false},
		[6] = {eff = TombEffectConfig[11],is_follow = true,is_special = true},
		[7] = {eff = TombEffectConfig[7],is_follow = false},
		[8] = {eff = TombEffectConfig[6],is_follow = false}, 
		[9] = {eff = TombEffectConfig[10],is_follow = true,self_complete = true}, 
		[10] = {eff = TombEffectConfig[13],is_follow = false}, 
		[11] = {eff = TombEffectConfig[12],is_follow = true}, 
		[12] = {eff = TombEffectConfig[9],is_follow = true}, 
		[13] = {eff = TombEffectConfig[5],is_follow = true}, 
		[14] = {eff = TombEffectConfig[13],is_follow = false}, 
		[15] = {eff = TombEffectConfig[1],is_follow = true}, 
	},
	
	BattleMapIconType = {
		Flag = 1, -- 旗帜
		Death = 2, -- 死亡
		Mine = 3, -- 我
		Box = 4, -- 宝箱
		Ruins = 5, -- 已开宝箱
		Mark = 6, -- 标记
		Guard = 7, -- 守卫
		Member= 8, -- 队友
	},

	Buff = {
		Speedup = 1,
		SpeedDown = 2,
		Alpha =3,
	},

	RoleSpecialEffectType = {
		Type_Nomal = 0,                    -- 正常
		Type_Catch = 1,                    -- 技能 抓捕引导
		Type_Catched = 2,                  -- 技能 被抓捕的人（抓捕状态中）
		Type_Boost = 3,                    -- 技能 疾走引导
		Type_TransMits = 4,                -- 技能 传送 天行梭引导
		Type_Trans_Complete = 5,           -- 技能 传送 天行梭 传送法阵 的传送技能结束
		Type_UnLock = 6,                   -- 技能 开锁引导
		Type_Rescue = 7,                   -- 技能 救援引导
		Type_Rescued = 8,                  -- 技能 救援 被救援的人（救援生效）
		Type_Alpha = 9,                    -- 技能 隐身符 神隐引导
		Type_Scarecrow = 10,               -- 技能 稻草人
		-- （备注，稻草人的生成，施法者技能引导，技能结束施法者传送回来都用这个）
		Type_Burst_Boost = 11,             -- 技能 轻身符技能引导（加速比疾走大）
		Type_Burst_Slow = 12,              -- 技能 轻身符 的加速结束的减速效果
		Type_Hit_Trap = 13,                -- 技能 陷阱 踩陷阱的人
		Type_Puppet = 14,                  -- 技能 傀儡 释放特效
		Type_Flash = 15,                   -- 技能 挪移尺 的引导特效
	}
}