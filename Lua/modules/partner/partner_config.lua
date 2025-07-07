
PartnerConfig = {
	ResPath = "partner/",
	PngPath = "partner/skin_%s.png",
	AuditPath = "partner/AuditImg_%s_%s.png",
	PngPathSoarWeaponS = "partner_soar_weapon/s%s.png",
	PngPathSoarWeaponB = "partner_soar_weapon/b%s.png",
	StrLevelSp = "PartnerNum%s",

	PARTNER_FETTER_MAX = 2,
	PARTNER_PE_MAX = 3,

	PARTNER_NUM_MAX = 40,

	PARTNER_ATTR_MAX = 7,
	PARTNER_RESI_MAX = 5,
	PARTNER_AMEN_MAX = 6,
	PARTNER_SKILL_AN_MAX = 3,		--AN = ALL_NUM
	PARTNER_SKILL_EN_MAX = 3,		--EN = EUIP_NUM
	UP_STAGE_MAX = 7,
	UP_LEVEL_MAX = 20,
	-- SKILLF_LEVEL_MAX = 7,
	PARTNER_EQUIP_VOC_NUM = 4,
	PARTNER_FIGHT_NUM_MAX = 4,

	Attr_Ele_MAX = 2,
	UPPreView_Skill_Max = 3,
	UPPreView_Condition_MAX = 3,

	HOLE_LOCK_LEVEL = 120,

	BIOGRAPHY_PAGE_HEIGHT = 450,
	BIOGRAPHY_PAGE_DISTANCE = 100,

	MAX_PARTNER_GIFT_TREE_SKILL_NUM = 15,

	PARTNER_CONTRACT_MAX = 3,
	PARTNER_CONTRACT_ATTR_MAX = 7,
	PARTNER_CONTRACT_ATTR_UP_MAX = 18,
	PARTNER_CONTRACT_ATTR_UP_PER = 3,

	PARTNER_STRENGTHEN_LEVEL_MAX = 15,

	PARTNER_RANK_MAX_SHOW = 10,

	FUYU_MAX_NUM = 3,

	PartnerViewTabs = {
		None = 0,
		Attr = 1,
		Up = 2,
		Life = 5,
		Quality = 4,
		Gift = 6,
		Contract = 3,
		Strengthen = 7,
	},

	OperType = {
		info = 0,				--请求所有伙伴信息
		fight = 1,				--伙伴出战状态改变
		up = 2,					--属性突破/能力进阶
		wash = 3,				--情缘洗练
		wash_save = 4,			--保存情缘洗练结果
		acti_partner = 5,		--道具激活伙伴
		signle_info = 6,		--请求单个伙伴信息
		up_level = 7,			--请求伙伴升级 p1:次数
		reset_level = 8,		--请求伙伴重置等级,
		open_biography = 9,		--打开传记 p1:biography_id
		fetch_biography = 10,	--领取传记奖励 p1:biography_id
		page_compose = 11,		--请求伙伴残页合成 p1:item_id
		uplevel_gift = 12,      --请求伙伴天赋点升级 p1:天赋孔位
		reset_gift = 13,        --请求伙伴天赋点重置 p1:天赋孔位
		contract_put_on = 14,	--元神契约 装备 p1:装备位置 p2:装备save_index
		contract_put_off = 15,	--元神契约 卸下 p1:装备位置
		contract_up_star = 16,	--元神契约 升星 p1:装备位置
		contract_replace = 17,	--元神契约 替换 p1:装备位置 p2:伙伴id p3:伙伴装备位置
		soar = 18,				--请求飞升
		soar_weapon = 19,		--请求飞升武器升级
	},

	ElementCrystalOperType = {
		use = 1,		--为某个伙伴装备元素水晶 p1:伙伴id p2:元素水晶index
		remove = 2,		--卸下某个伙伴的元素水晶 p1:伙伴id
		use_quick = 3,	--快速装备元素水晶到出战伙伴 p1~p4:元素水晶index1~index4
		remove_all = 4,	--卸下所有激活伙伴的元素水晶
	},

	PEOperType = {
		equip = 0,			--穿装备
		remove = 1,			--脱装备,
		unlock = 2,			--解锁,
		equip_break = 3,    --分解
	},

	SkinOperType = {
		active = 0,			--激活皮肤
		equip = 1,			--穿戴皮肤
		remove = 2,			--卸下皮肤
	},

	LockState = {
		lock = 0,
		unlock = 1,
	},

	FightState = {
		rest = 0,
		fight = 1,
	},

	SoarNoticeType = {
		soar = 0,				--伙伴飞升
		soar_weapon = 1,		--伙伴飞升武器强化
	},

	ChangeServerQua = {
		[3] = 1,
		[4] = 2,
		[5] = 3,
		[6] = 4,
		[7] = 5,
	},

	PartnerAttrConvertCommon = {
		BattleAttriType.MaxHp,
		BattleAttriType.MaxMp,
		BattleAttriType.Attack,
		BattleAttriType.Defense,
		BattleAttriType.Agility,
		BattleAttriType.Mental,
		BattleAttriType.Recovery,
	},

	PartnerAttrConvertCfg = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "jingshen", "huifu" },

	LoopSetting = {
		BgS = {
			inactive = "WeiTuPoTuBiao",
			actived = "YiTuPoTuBiao",
			cur = "DangQianXuanZhongTuBiao",
		},
		NameC = {
			inactive = COLORSTR.Yellow13,
			actived = COLORSTR.Green1,
			cur = COLORSTR.Red1,
		},
		LineS = {
			inactive = "BaiSeXiaoDian",
			actived = "LvSeXiaoDian",
			cur = "BaiSeXiaoDian",
		},
	},

	PreviewCondition = {
		Attr = 1,
		Level = 2,
		Task = 3,
	},

	FightSp = {
		Zhan = "_LocZhan",
		Lei = "_LocLei",
	},

	NoticeType = {
		no = 0,				--无标记
		hp_mp = 1,			--hp,mp变化
		break_though = 2,	--伙伴突破
		up_grade = 3,		--伙伴进阶
		equip = 4,			--伙伴装备
		role_level_up = 5,	--人物升级
		role_train_up = 6,	--人物修炼
		role_quality_up = 7,--伙伴升品
		skin = 8,			--伙伴皮肤(伙伴激活时)
		skin_no_ac = 9,		--伙伴皮肤(伙伴未激活)
		guild_inborn = 10,	--家族灵脉
		attr_pellet = 11,	--属性丹
		pact = 12,			--契约属性
		pact_change = 13,	--契约属性变更
		soar = 14,			--伙伴飞升
		soar_weapon = 15,	--飞升武器强化
		xiu_wei = 16,		--修为
		intensify = 17,		--伙伴强化
		fu_yu = 18,--符玉强化
		star_map = 19,		--星图
	},

	QuaSp = {
		[3] = "HuoBanPinZhiKuangLan",
		[4] = "HuoBanPinZhiKuangZi",
		[5] = "HuoBanPinZhiKuangCheng",
		[6] = "HuoBanPinZhiKuangHong",
		[7] = "HuoBanPinZhiKuangJin",
	},

	QuaPicName = {
		[BattleAttriType.Attack] = "gj",
		[BattleAttriType.Defense] = "fy",
		[BattleAttriType.Agility] = "sd",
		[BattleAttriType.Recovery] = "hf",
		[BattleAttriType.Mental] = "js",
		[BattleAttriType.MaxHp] = "sm",
		[BattleAttriType.MaxMp] = "fl",
	},

	QuaQuaPicName = {
		[1] = "blue",
		[2] = "purple",
		[3] = "orange",
		[4] = "red",
		[5] = "gold",
	},
	
	-- 协议1772专用
	QuaAttrType = {
		[BattleAttriType.MaxHp] = 0,
		[BattleAttriType.MaxMp] = 1,
		[BattleAttriType.Attack] = 2,
		[BattleAttriType.Defense] = 3,
		[BattleAttriType.Agility] = 4,
		[BattleAttriType.Recovery] = 5,
		[BattleAttriType.Mental] = 6,
	},

	QuaBgSp = {
		[3] = "lanse",
		[4] = "zise",
		[5] = "chengse",
		[6] = "hongse",
		[7] = "jinse",
	},

	QuaFetter = {
		White = 1,
		Grassy = 2,
		Blue = 3,
		Purple = 4,
		Orange = 5,
		Red = 6,
		Gold = 7,
	},

	CrystalGet = {
		[0] = {true, true, 19, 144},
		[40000] = {true, false, 144, 19},
		[40001] = {true, false, 144, 19},
		[40002] = {true, false, 144, 19},
		[40003] = {true, false, 144, 19},
		[40004] = {false, true, 19, 144},
		[40005] = {false, true, 19, 144},
		[40006] = {false, true, 19, 144},
		[40007] = {false, true, 19, 144},
	},

	PESuitId2Prof = {2,1,3,6,4,7,5,8},

	SkillType = {
		active = 1,
		passive = 2,
		friendly = 3,
	},

	UpAttrType = {
		[1] = BattleAttriType.MaxHp,
		[2] = BattleAttriType.MaxMp,
		[3] = BattleAttriType.Attack,
		[4] = BattleAttriType.Defense,
		[5] = BattleAttriType.Agility,
		[6] = BattleAttriType.Recovery,
		[7] = BattleAttriType.Mental,
		[8] = BattleAttriType.MagicAttack,
	},

	LifeColor = {
		[3] = "Blue10",
		[4] = "Purple8",
		[5] = "Yellow13",
	},
	LifeId2ProfType = {
		[1] = 2,
		[2] = 1,
		[3] = 3,
		[4] = 6,
		[5] = 4,
		[6] = 7,
		[7] = 5,
		[8] = 8,
	},

	LifeHandbookDecoSp = {
		[3] = "ZhuangShiLan",
		[4] = "ZhuangShiZi",
		[5] = "ZhuangShiCheng",
		[6] = "ZhuangShiHong",
	},

	LifeHandbookDiSp = {
		[3] = "LanDi",
		[4] = "ZiDi",
		[5] = "ChengDi",
		[6] = "HongDi",
	},

	LifeHandbookBTSp = {
		[3] = "LanBiaoTi",
		[4] = "ZiBiaoTi",
		[5] = "ChengBiaoTi",
		[6] = "HongBiaoTi",
	},

	LifeHandbookCardColorName = {
		[3] = COLORSTR.Blue1,
		[4] = COLORSTR.Purple3,
		[5] = COLORSTR.Red2,
		[6] = COLORSTR.Red7,
	},
	-- 伙伴属性配置 -- name 去Language.PetOtherAttriNames找
	OtherAttriCfg = {
		{attr_type = 19, is_percent = false},      -- name = "法攻:"
		{attr_type = 20, is_percent = false},      -- name = "法防:"
		{attr_type = 15, is_percent = true},       -- name = "必杀:"
		{attr_type = 16, is_percent = true},       -- name = "命中:"
		{attr_type = 17, is_percent = true},       -- name = "闪避:"
		{attr_type = 18, is_percent = true},       -- name = "反击:"
		{attr_type = 22, is_percent = true},       -- name = "坚韧:"
		{attr_type = 27, is_percent = true},       -- name = "法爆:"
		{attr_type = 28, is_percent = true},       -- name = "法爆伤害:"
		{attr_type = 21, is_percent = true},       -- name = "必杀伤害:"
		{attr_type = 30, is_percent = true},       -- name = "中毒抗性:"
		{attr_type = 31, is_percent = true},      -- name = "石化抗性:"
		{attr_type = 32, is_percent = true},      -- name = "昏睡抗性:"
		{attr_type = 33, is_percent = true},      -- name = "混乱抗性:"
		{attr_type = 34, is_percent = true},      -- name = "酒醉抗性:"
		{attr_type = 35, is_percent = true},      -- name = "沉默抗性:"
		{attr_type = 38, is_percent = true},      -- name = "重伤抗性:"
		{attr_type = 11, is_percent = true},      -- name = "物理增伤:"
		{attr_type = 12, is_percent = true},      -- name = "法术增伤":"
		{attr_type = 9, is_percent = true},		  -- name = "物伤减免:"
		{attr_type = 10, is_percent = true},      -- name = "法伤减免":"
		{attr_type = 23, is_percent = true},      -- name = "效果命中:"
		{attr_type = 26, is_percent = true},      -- name = "效果抵抗:"
		{attr_type = 24, is_percent = false},      -- name = "真实伤害:"
	},

	ContractI2Type = {
		[0] = 6,	--生命
		[1] = 8,	--魔力
		[2] = 0,	--攻击
		[3] = 1,	--防御
		[4] = 2,	--敏捷
		[5] = 3,	--回复
		[6] = 4,	--精神
	},
	fuyu_decom_quick_select = {
        { name = Language.FuYu.Medal_Qua[2], qua = 2,},
        { name = Language.FuYu.Medal_Qua[3], qua = 3,},
        { name = Language.FuYu.Medal_Qua[4], qua = 4,},
        { name = Language.FuYu.Medal_Qua[5], qua = 5,},
        { name = Language.FuYu.Medal_Qua[6], qua = 6,},
        { name = Language.FuYu.Medal_Qua[0], qua = -1,},
    },
	FuYuType = {
		Normal = 1,
		Exclusive = 2,
	},
	FuYuQuaPic = {
		[2] = "Lv",
		[3] = "Lan",
		[4] = "Zi",
		[5] = "Cheng",
		[6] = "Hong"
	},
	FuYuProfSp = {
		[100]  = "_LocQuan",
		[200] = "_LocJian",
		[300] = "_LocFu",
		[400] = "_LocQiang",
		[500] = "_LocGong",
		[600] ="_LocShu",
		[700] = "_LocYi",
		[800] = "_LocWu",
	}
}