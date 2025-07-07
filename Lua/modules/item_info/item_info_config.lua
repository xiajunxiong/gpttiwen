ItemInfoConfig = {
	ShowType = {
		Main = 1,
		Left = 2,
	},

	ButtonConfig = {
		{ SpName = "LvAnNiu_PuTong", LbColor = COLORS.Green1 },
		{ SpName = "HuangAnNiu_PuTong", LbColor = COLORS.Red1 },
		{ SpName = "LvAnNiu_PuTong", LbColor = COLORS.Red7 },
	},

	ItemInfoShow = {
		"QuaName", "TypeDesc", "LimitProfDesc", "LevelDesc", "Cap", "Bind", "GetWay","PreRange",
	},

	QualitySp = {
		[ItemColor.White] = "",
		[ItemColor.Green] = "QualityGreen",
		[ItemColor.Blue] = "QualityBlue",
		[ItemColor.Purple] = "QualityPurple",
		[ItemColor.Orange] = "QualityOrange",
		[ItemColor.Red] = "QualityRed",
		[ItemColor.Star] = "QualityOrange",
		[ItemColor.God] = "QualityRed",
		[ItemColor.Gold] = "QualityGold",
		[ItemColor.Black] = "QualityBlack",
	},

	BagOperType = {
		store = 2,			--存入仓库
		fetch = 3,			--取出仓库
		sell = 6,			--丢弃(出售)
	},

	RoleInfoButton = {
		AddBlackList = 2,
		Report = 2,
		Unblock = 2,
		ApplyPost = 2,
		KickOutG = 2,
		Accuse = 2,
		GroupExit = 2,
		GroupDissmiss = 2,
		GroupNoDisturb = 2,
		GroupNoDisturbClose = 2,
		GroupKickOut = 2,
		QuickChat = 2,
		QuickChatDel = 2,
		Duel = 3,
		Sanction = 3,
		Escort = 3,
	},
	AutoPosSetting = {
		half_info_pos_width = 435 / 2,		-- info和info2的宽,自适应计算时要用到
		max_plus_width = 450,			-- 存在按钮列表时增加的宽,因为打开出售窗口等info位置不移动,所以是那些窗口宽度取一个合适的最大值,保证右边自适应不超出边界
		max_plus_height = 400,			-- 存在按钮列表时增加的高,因为打开出售窗口等info位置不移动,所以是那些窗口高度取一个合适的最大值,保证下边自适应不超出边界
		button_per = 68,
	},
	AutoPosConfig = {
		Offset = {
			[21] = Vector2.New(245,-40),
			[27] = Vector2.New(65,-40),
			[28] = Vector2.New(245,-40),
			[53] = Vector2.New(296,-21.5),
			[862] = 200,
			[635] = 85,
			Height = -50,
			MinHeight = 180,
			SellAutoSize = Vector2.New(427,362),
			PreRangeSize = Vector2.New(414,490),
			GetWaySize = Vector2.New(200,180),
			ItemList = Vector2.New(-60,0),
			Model = {
				[21] = Vector2.New(77,0),
				[27] = Vector2.New(-101,0),
				[28] = Vector2.New(77,0),
				[53] = Vector2.New(128,-40),
			}
		}
	},
	BindSpriteName = {
		[0] = "_Loc_isnt_bind",
		[1] = "_Loc_is_bind",
		[2] = "_Loc_sell_bind",
	},
	QuickBuyKey = {
		FuyuQiangHua = "FuyuQiangHua"
	}
}