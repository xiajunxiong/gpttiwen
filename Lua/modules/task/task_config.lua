CUSTOM_OBJID = {
    TREASURE_MAP = 1000000,
    RESEARCH_TASK = 1000001,
    TASK_CHAIN_START = 1000002,
    FantasyFB = 1000003,
    FightDen = 1000004,
    PRESTIGE_TASK_START = 1000005,-- 门派任务特殊处理，2000000以后1000个预留为门派任务子任务
    GUILD_TASK_BEGIN = 2000000,
    GUILD_TASK_END = 2001000,
    TASK_CHAIN = 2001001,
    CHIEF_STATUE = 2001002,--跨服巅峰战雕像
    HELP_OTHERS = 2002001,  -- 义胆助人
    PEAK_TASK_ID = 2003001, --巅峰之路任务
    SHAN_HAI_ORIGIN = 2004001,--山海缘起
    LIVE_PROP = 2005001,    -- 生活系统的道具
    TREASURE_TASK = 2006001,    -- 九州秘宝
    PARADE_FLOAT_1 = 2007001,
	PARADE_FLOAT_2 = 2007002,
    FAIRY_WHERE = 2008001, -- 仙子去哪儿
	MS_LANTERN_NPC = 2009001,--山海灯会大使
	MS_LANTERN = 2009002,--山海灯会灯笼
	IMPERIAL_EXAM_NPC = 2009011,--科举考试文曲星君
	IMPERIAL_EXAM = 2009012,--科举考试考官
	UTA_STATUE = 2011001,--天下第一雕像
	GOTO_SCHOOL = 2012001,	--我要上学
	DAY_ACT = 	 2100000,--日常任务的任务ID，DAY_ACT+act_type
	PALACE_STATUS = 2013001,--殿堂雕像
}
-- 注意!!
-- task_id是primary_key(每个任务唯一的id,策划配的id不一定是唯一的,有需要请加偏移值)
-- 这个偏移值不要弄太大，因为跟服务器有协定，需要让task_id<2的（16-1）次方

TaskConfig = {
	ResPath = "task/",
	ResPathBT = "task/bounty_task/",
	ArrowLevel = 30,
	BountyTaskMax = 10,
	NORMAL_TASK_NOT_ENOUGH_LEVEL = 31,
	OperType = {
		accept = 0,				--接受任务
		give_up = 1,			--放弃任务
		commit = 2,				--提交任务
		demand = 3,				--查询任务状态
		task_patrol = 4,		--巡逻任务	0-请求下发信息 1-接取任务 2-领取奖励
		skip_battle = 5,		--任务跳过战斗 只有任务条件为15  击败怪物组的任务才可以跳过战斗
	},
	ListType = {
		accepted = 0,		--已接任务列表
		acceptable = 1,		--可接任务列表
	},
	-- 任务类型
	-- 任务排序、颜色、底图颜色在最下面
	TaskType = {
		mainline = 0,		--主线任务
		branch = 1,			--支线任务
		challenge = 2,		--挑战任务
		upgrade = 3,		--进阶任务
		medal = 4,			--勋章任务
		treasure_map = 5,	--宝图任务
		guild = 6,			--门派任务
		task_chain = 7,		--任务链
		family_week = 8,	--家族周任务
		dragon = 9,			--一条龙(三界除魔)
		adventure = 10,		--奇遇任务
		reco = 11,			--推荐任务
		bounty = 12,		--赏金任务(现门派任务)
		silk_road = 13,     --远洋贸易
		anecdote = 14,      --奇闻任务
		prestige = 15,		--声望任务
		fly_up = 16,		--角色飞升任务
		partner_soar = 17,	--伙伴飞升任务
		partner_soar_pre = 18,--伙伴飞升公用前置任务
		pet_soar = 19,	    --宠物飞升任务
		fly_up_pre = 20,    --角色飞升前置任务
		dianhua_task = 21,	--装备点化任务
		upgrade_item = 22,  --晋阶任务
		branch_serial = 23,	--连环式支线任务
		branch_marbas = 24,	--造化装备前置环式任务
		branch_relic = 25,	--圣器环式任务
		team_challenges = 26,	--组队挑战任务
		family_daily = 27,    --家族日常任务

		help_others = 100,	--义胆助人
		peak_road = 101,	--巅峰之路
		live_prop = 102,	--生活道具
		treasure_task = 103,--九州秘宝
		FairyWhere = 104,   --仙子去哪儿
		ms_lantern = 105,	--山海灯会
		day_act = 106,		--日常任务
		imperial_exam = 107,--科举考试
	},
	ListChangeReason = {
		init = 0,			--初始化
		change = 1,			--信息改变
		insert = 2,			--插入任务
		remove = 3,			--移除任务
	},
	TaskState = {
		undo = 0,			-- 未进行
		done = 1,			-- 已完成
		doing = 2			-- 进行中
	},
	MainViewFuncImgConvert = {
		list_type = {
			[0] = "Jiao",
			[1] = "Jie"
		},
		list_type_new = {
			[0] = "JianTou",
			[1] = "JuanZhou"
		},
		task_type = {
			[0] = "Huang",
			[1] = "Hong",
			[2] = "Lv",
			[3] = "Hong",
			[4] = "Hong",
			[5] = "Hong",
			[6] = "Lv",
			[7] = "Lan",
			[8] = "Zi",
			[9] = "Lv",
		}
	},
	
	ConditionType = {
		gather = 5,
	},

	TaskGuideAutoType = {
		mod_key = 1,
		scene_mode = 2,
	},

	HelpOthersStatus = {
		accept = 0,		-- 未接取
		commit = 1,		-- 已接取 进行中
		finish = 2,		-- 完成
	},

	HelpOthersOperType = {
		start = 0,		--完成客户端伪任务
		accept = 1,		--接取
		item = 2,		--交货 p1:column p2:index
		talk = 3,		--交谈
		pet = 4,		--上交宠物 p1:pet_index
		battle = 5,		--打怪完成
	},

	TaskChainItemSelFrom = {
		task_chain = 1,
		help_others = 2,
		shan_hai = 3,
	},
	
	TASK_CHAIN_COLOR_FRAME_SPRITE = {
		[ItemColor.Green] = "LvSePinZhiYuan",
		[ItemColor.Blue] = "LanSePinZhiYuan",
		[ItemColor.Purple] = "ZiSePinZhiYuan",
		[ItemColor.Orange] = "ChengSePinZhiYuan",
		[ItemColor.Red] = "HongSePinZhiYuan",
	},
	
	-- 任务小游戏
	TaskGameList = {
		ElementGuide = 0,	-- 元素克制
		BreakAltar = 1,		-- 破坏祭坛
		RepairArray = 2,	-- 修补法阵
		Practice = 3,		-- 修行
	},
}

--任务菜单排序
TaskConfig.TaskTypeSort = {
	[TaskConfig.TaskType.peak_road] = -1,		--巅峰之路
	[TaskConfig.TaskType.fly_up_pre] = 0,		--角色飞升前置任务
	[TaskConfig.TaskType.fly_up] = 1,			--角色飞升任务
	[TaskConfig.TaskType.pet_soar] = 2,			--宠物飞升任务
	[TaskConfig.TaskType.partner_soar_pre] = 3,	--伙伴飞升公用前置任务
	[TaskConfig.TaskType.partner_soar] = 4,		--伙伴飞升任务
	[TaskConfig.TaskType.mainline] = 5,			--主线任务
	[TaskConfig.TaskType.branch_serial] = 6,	--连环式支线任务
	[TaskConfig.TaskType.branch_marbas] = 7,	--造化装备前置环式任务
	[TaskConfig.TaskType.branch_relic] = 8,		--圣器环式任务
	[TaskConfig.TaskType.live_prop] = 9,		--生活道具
	[TaskConfig.TaskType.upgrade_item] = 10,	--晋阶任务
	[TaskConfig.TaskType.upgrade] = 11,			--进阶任务
	[TaskConfig.TaskType.silk_road] = 12,      	--远洋贸易
	[TaskConfig.TaskType.adventure] = 13,		--奇遇任务
	[TaskConfig.TaskType.treasure_map] = 14,	--宝图任务
	[TaskConfig.TaskType.bounty] = 15,			--赏金任务
	[TaskConfig.TaskType.task_chain] = 16,		--任务链
	[TaskConfig.TaskType.prestige] = 17,		--声望任务
	[TaskConfig.TaskType.dianhua_task] = 18,	--点化任务
	[TaskConfig.TaskType.branch] = 19,			--支线任务
	[TaskConfig.TaskType.FairyWhere] = 20,		--仙子去哪儿
	[TaskConfig.TaskType.ms_lantern] = 21,		--山海灯会
	[TaskConfig.TaskType.imperial_exam] = 22,	--科举考试
	[TaskConfig.TaskType.day_act] = 23,			--日常任务
	[TaskConfig.TaskType.guild] = 24,			--门派任务
	[TaskConfig.TaskType.treasure_task] = 26,	--九州秘宝
	[TaskConfig.TaskType.dragon] = 27,			--一条龙
	[TaskConfig.TaskType.medal] = 28,			--勋章任务
	[TaskConfig.TaskType.challenge] = 29,		--挑战任务
	[TaskConfig.TaskType.reco] = 31,			--推荐任务
	[TaskConfig.TaskType.help_others] = 32,		--义胆助人
	[TaskConfig.TaskType.team_challenges] = 30,		--组队挑战
	[TaskConfig.TaskType.family_daily] = 25, --家族日常任务
	
}

--任务菜单颜色
TaskConfig.TaskMenuTypeColor = {
	[TaskConfig.TaskType.mainline] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.upgrade] = COLORSTR.Red3,
	[TaskConfig.TaskType.upgrade_item] = COLORSTR.Red3,
	[TaskConfig.TaskType.branch] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.branch_serial] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.branch_marbas] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.branch_relic] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.dianhua_task] = COLORSTR.Red3,
	[TaskConfig.TaskType.reco] = COLORSTR.Red3,
	[TaskConfig.TaskType.treasure_map] = COLORSTR.Blue7,
	[TaskConfig.TaskType.guild] = COLORSTR.Blue7,
	[TaskConfig.TaskType.dragon] = COLORSTR.Blue7,
	[TaskConfig.TaskType.ms_lantern] = COLORSTR.Green5,
	[TaskConfig.TaskType.imperial_exam] = COLORSTR.Green5,
	[TaskConfig.TaskType.bounty] = COLORSTR.Blue7,
	[TaskConfig.TaskType.task_chain] = COLORSTR.Green5,
	[TaskConfig.TaskType.help_others] = COLORSTR.Purple2,
	[TaskConfig.TaskType.challenge] = COLORSTR.Purple2,
	[TaskConfig.TaskType.medal] = COLORSTR.Purple6,
	[TaskConfig.TaskType.adventure] = COLORSTR.Purple2,
	[TaskConfig.TaskType.silk_road] = COLORSTR.Green5,
	[TaskConfig.TaskType.prestige] = COLORSTR.Blue7,
	[TaskConfig.TaskType.fly_up] = COLORSTR.Green4,
	[TaskConfig.TaskType.partner_soar] = COLORSTR.Green4,
	[TaskConfig.TaskType.partner_soar_pre] = COLORSTR.Green4,
	[TaskConfig.TaskType.pet_soar] = COLORSTR.Green4,
	[TaskConfig.TaskType.fly_up_pre] = COLORSTR.Green4,
	[TaskConfig.TaskType.peak_road] = COLORSTR.Yellow1,
	[TaskConfig.TaskType.live_prop] = COLORSTR.Blue7,
	[TaskConfig.TaskType.treasure_task] = COLORSTR.Green5,
	[TaskConfig.TaskType.day_act] = COLORSTR.Green5,			--日常任务
	[TaskConfig.TaskType.team_challenges] = COLORSTR.Red8,		 
	[TaskConfig.TaskType.family_daily] = COLORSTR.Blue7,
}

--任务菜单底图颜色
TaskConfig.TaskMenuTypeSp = {
	[TaskConfig.TaskType.mainline] = "TYellow",
	[TaskConfig.TaskType.upgrade] = "TRed",
	[TaskConfig.TaskType.upgrade_item] = "TRed",
	[TaskConfig.TaskType.branch] = "TYellow",
	[TaskConfig.TaskType.branch_serial] = "TYellow",
	[TaskConfig.TaskType.branch_marbas] = "TYellow",
	[TaskConfig.TaskType.branch_relic] = "TYellow",
	[TaskConfig.TaskType.dianhua_task] = "TRed",
	[TaskConfig.TaskType.reco] = "TRed",
	[TaskConfig.TaskType.treasure_map] = "TBlue",
	[TaskConfig.TaskType.guild] = "TBlue",
	[TaskConfig.TaskType.dragon] = "TBlue",
	[TaskConfig.TaskType.ms_lantern] = "TGreen",
	[TaskConfig.TaskType.imperial_exam] = "TGreen",
	[TaskConfig.TaskType.bounty] = "TBlue",
	[TaskConfig.TaskType.task_chain] = "TGreen",
	[TaskConfig.TaskType.help_others] = "TPurple",
	[TaskConfig.TaskType.challenge] = "TPurple",
	[TaskConfig.TaskType.medal] =  "TPurple6",
	[TaskConfig.TaskType.adventure] = "TPurple",
	[TaskConfig.TaskType.silk_road] = "TGreen",
	[TaskConfig.TaskType.prestige] = "TBlue",
	[TaskConfig.TaskType.fly_up] = "TGreen2",
	[TaskConfig.TaskType.partner_soar] = "TGreen2",
	[TaskConfig.TaskType.partner_soar_pre] = "TGreen2",
	[TaskConfig.TaskType.pet_soar] = "TGreen2",
	[TaskConfig.TaskType.fly_up_pre] ="TGreen2",
	[TaskConfig.TaskType.peak_road] = "TRed8",
	[TaskConfig.TaskType.live_prop] = "TBlue",
	[TaskConfig.TaskType.treasure_task] = "TGreen",
	[TaskConfig.TaskType.day_act] = "TGreen",			--日常任务
	[TaskConfig.TaskType.team_challenges] = "TRed8",			
	[TaskConfig.TaskType.family_daily] = "TBlue",
}
--声望任务请求
PRESTIGE_TASK_REQ_TYPE = {
	TYPE_FLUSH_STAR = 0,	--刷新星级
	TYPE_BEGIN = 1,			--开始任务
	TYPE_ALL_INFO = 2,		--请求所有信息
}

--任务类型对应的任务类
TaskConfig.NewType = {
    [TaskConfig.TaskType.FairyWhere] = "TaskFairyWhere",
	[TaskConfig.TaskType.day_act] = "DailyActGuideTask",
}
--self.skin_type
TaskConfig.TaskMenuSkin = {
	[COLORSTR.Yellow1] = "TYellow",
	[COLORSTR.Red3] = "TRed",
	[COLORSTR.Blue7] = "TBlue",
	[COLORSTR.Green5] = "TGreen",
	[COLORSTR.Purple2] = "TPurple",
	[COLORSTR.Purple6] =  "TPurple6",
	[COLORSTR.Green4] = "TGreen2",
}

-- 不重写npc行为的任务类型
UN_OVERWRITE_TASK = {
	[TaskConfig.TaskType.team_challenges] = true
}

-- npc对话界面不显示任务按钮的任务类型
UN_SHOW_TASK_BUTTON={
	[TaskConfig.TaskType.team_challenges] = true
}