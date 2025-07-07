
ArenaMatchConfig = {
	ResPath = "arena_match/",

	PET_NUM_MAX = 5,
	PARTNER_NUM_MAX = 5,

	OperType = {
		role_info = 0,			--角色信息
		rank_info = 1,			--擂台信息 p1擂台编号
		challenge = 2,			--战斗 p1擂台编号 p2rank pet_index要上场的宠物 partner_id要上场的伙伴
		adjust = 3,				--调整 p1擂台编号 pet_index要上场的宠物 partner_id要上场的伙伴
		remove = 4,				--撤下某个擂台上的伙伴和宠物 p1擂台编号
		record = 5, 			--请求擂台战报
		set_notice = 6, 		--设置是否打开过擂台界面 p1, 1是isfirst
		set_notice2 = 7,        --查看了战报，p1 等于擂台方位配置的position
		last_rank_info = 8,		--上次排名信息，下发2791协议
	},
	OperType2 = {
		Achieve = 0,			--领取成就奖励 p1 type,p2 seq
	},

	InfoType = {
		own = 1,
		other = 2,
		ready = 3,
	},

	RankSp = {
		[1] = "DiYiMing_2",
		[2] = "DiErMing_2",
		[3] = "DiSanMing_2",
		default = "PaiMingDi_4"
	},

	OptionType = {
		pet = 1,
		partner = 2,
	},

	PosC2S = {
		f = {
			[1] = 3,
			[2] = 8,
			[3] = 1,
			[4] = 6,
			[5] = 0,
			[6] = 5,
			[7] = 2,
			[8] = 7,
			[9] = 4,
			[10] = 9,
		},
		t = {
			[1] = 1,
			[2] = 6,
			[3] = 0,
			[4] = 5,
			[5] = 2,
			[6] = 7,
		}
	},

	PosS2C = {
		f = {
			[0] = 5,
			[1] = 3,
			[2] = 7,
			[3] = 1,
			[4] = 9,
			[5] = 6,
			[6] = 4,
			[7] = 8,
			[8] = 2,
			[9] = 10,
		},

		t = {
			[0] = 3,
			[1] = 1,
			[2] = 5,
			[5] = 4,
			[6] = 2,
			[7] = 6,
		}
	},

	ArenaMatchIds = {
		[49990] = true,
		[49991] = true,
		[48997] = true,
		[48998] = true,
		[48999] = true,
	},

	ArenaShowOffsetY = {
		[0] = 3.5,
		[1] = 3.78,
		[2] = 3.55,
		[3] = 3.74,
		[4] = 2.8,
	},

	MenuPosToIconSp = {
		[0] = "Dong2",
		[1] = "Bei2",
		[2] = "Nan2",
		[3] = "Xi2",
		[4] = "Dong2",
	},

	BgSp = {
		[0] = "Tiao2",
		[1] = "Tiao1",
	},

	-- 方位对应的显示信息
	ShowInfo = {
		[1] = {img = "Tu5", obj_name = "ql", qua = "Tu_5_5"},
		[2] = {img = "Tu4", obj_name = "bh", qua = "Tu_4_4"},
		[3] = {img = "Tu3", obj_name = "zy", qua = "Tu_3_3"},
		[4] = {img = "Tu2", obj_name = "zq", qua = "Tu_2_2"},
		[5] = {img = "Tu1", obj_name = "xw", qua = "Tu_1_1"},
	},

	-- 计算提醒时间
	RemindEndTime = 86400,
}