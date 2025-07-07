
TeamConfig = {
	ResPath = "team/",
	PngPath = "team/%s.png",

	RobotUid = 1048575,

	TEAM_MEMBER_MAX = 5,

	TEAM_MEMBER_MAX_T = {
		[243] = 2,
		[244] = 2,
		[246] = 2,
		[247] = 2,
		[248] = 2,
		[240] = 3,
		[161] = 3,
		[180] = 3,
	},

	TeamFunctions1 = {
		[1] = { "ViewEquip", "Chat", "KickOut", "TeamSort" },
		[2] = { "ViewEquip", "Chat" },
		[3] = { "PartnerRest", "TeamSort" },
	},

	TeamFunctions2 = {
		[1] = { "ViewEquip", "Chat", "KickOut", "TeamSort", "UseHeal" },
		[2] = { "ViewEquip", "Chat", "UseHeal" },
		[3] = { "PartnerRest", "TeamSort", "UseHeal" },
	},

	TeamDetectType = {
		succ = 1,		-- 检测通过
		fail_no = 2,	-- 检测失败并且不在队伍中
		fail_in = 3,	-- 检测失败并且在队伍中
	},

	MatchType = {
		start = 1,
		cancel = 0,
	},

	CrossType = {
		is_cross = 1,
		not_cross = 0,
	},

	InviteType = {
		friend = 1,
		family = 2,
		partner = 3,
		shitu = 10,
	},

	FbInfoTeamType = {
		refuse = 0,
		confirm = 1,
	},

	TeamCheckType = {
		refuse_all = 0,			-- 拒绝所有申请
		need_check = 1,			-- 需要审批
		auto_join = 2,			-- 自动加入
	},

	TeamIncompatibleType = {
		world_boss1 = 0,		-- 世界boss1次数不足
		world_boss2 = 1,		-- 世界boss2次数不足
		level = 2,				-- 队员等级不足 p1目标等级
		item = 3,				-- 队员物品数量不足 p1:item_id p2:num
		times = 4,				-- 队员已经超过参加次数
		max_num = 5,			-- 队伍人数太多
		not_num = 6,      		-- 队伍人数不足
		not_apprentice = 7,     -- 队友不是你的徒弟 
		beave_ground_cant_next = 8, --暂时无法进入，将在世界等级达到p1级后开服
		has_jie_yi_zu = 9,		-- 拥有结义组
		world_boss3 = 10,		-- 世界boss3（星官降临）次数不足
		team_member_incompatible_no_task_record = 11,		-- 队员x param1任务未达成
	},

	-- 组队平台大类型
	TeamTargetType = {
		DailyAct = 1,		--日常活动
		LimitAct = 2,		--限时活动
		BranchTask = 3,		--支线任务
		ChallengeTask = 4,	--挑战任务
		UpgradeTask = 5,	--进阶任务
		MedalTask = 6,		--勋章任务
		EliteFB = 7,		--精英副本
		RareFB = 8,			--稀有副本
		YYAct = 9,			--运营活动
		Conspiracy = 14,	--远征魔冢
		BigDipper = 15,		--北斗七星
		Storybook = 17,		--万灵话本
		EliteFBFly = 19,	--化圣秘境
		HuanShiChallenge=23,--幻世挑战
	},

	SalaryType = {
		salary = 0,				-- 队长工资
		knightly = 1,			-- 侠义值
	},

	SalaryReasonType = {
		init = 0,				-- 下发整个列表
		update = 0,				-- 更新列表
	},

	BgPng = {
		no_team = "WeiZuDuiDi",
		in_team = "ZuDuiChengGongDi",
	},

	Dians = {
		BaiDian = "BaiDian",
		HongDian = "HongDian",
	},

	TeamPlatformPorfBg = {
		[0] = "QiGong",
		[1] = "QiGong",
		[2] = "JianKe",
		[3] = "KuangZhan",
		[4] = "TieWei",
		[5] = "SheShou",
		[6] = "XingShu",
		[7] = "YiShi",
		[8] = "FuZhou",
	},

	TeamMemberBgSp = {
		DuiYou = "DuiYouXinXiDi",
		Wo = "WoXinXiDi",
		HuoBan = "HuoBanXinXiDi",
	},

	TeamLeaderNpcOperType = {
		init = 0,		--	初始化
		add = 1,		--	添加
		delete = 2,		--	删除
	},

	HealMenuIconSp = {
		[1] = "Xue",
		[2] = "Lan"
	},

	HealMenuObjSp = {
		[0] = "ZhuanHuanJueSe",
		[1] = "ZhuanHuanChongWu"
	},

	TeamStateSp = {
		leave = "ZanLiDi",
		outline = "LiXianDi",
	},

	TeamInvitedNotice = {
		default = 0,
		update = 1,
		remove = 2,
	},

	TARGET_TIPS_FUNC = {
		[1] = function(do_func, target_id)
			if do_func then
				ResearchTaskCtrl.Instance:GotoResearchTask()
			end
			local co = ActiveData.Instance:GetActiveCfg(ActType.ResearchTask)
			local num = ActivityData.Instance:GetProgressNum(co.type)
			local need_num = ActiveData.Instance:GetDailyNeedCount(co)
			return num < need_num
        end,
		[2] = function(do_func, target_id)
			if do_func then
				local co = Cfg.CrystalSceneNameByTeamTarget(target_id)
				if co then
					local info = FightData.Instance.dungeon_seq_list[co.scene_seq]
					info.click_handle()
				end
			end
            local co = ActiveData.Instance:GetActiveCfg(ActType.FightElite)
			local num = ActivityData.Instance:GetProgressNum(co.type)
			local need_num = ActiveData.Instance:GetDailyNeedCount(co)
			return num < need_num
        end,
		[3] = function(do_func, target_id)
			if do_func then
				local co = Cfg.WanlingHuabenFbInfoByTeamTarget(target_id)
				if co then
					SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ALL_SOULS_FB, co.fb_index)
				end
			end
            local co = ActiveData.Instance:GetActiveCfg(ActType.AllSoulsFB)
			local num = ActivityData.Instance:GetProgressNum(co.type)
			local need_num = ActiveData.Instance:GetDailyNeedCount(co)
			return num < need_num
        end,
		[22] = function(do_func, target_id)
			local co = Cfg.PromevalParadiseBossFbTargetId(target_id, false)
			if do_func then
				if co then
					LogDG("发送进入鸿蒙副本的协议,服务端会帮客户端进跨服")
					SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HARMONY_HEAVEN, co.fb_type, co.seq)
					-- if not LoginData.Instance:IsOnCrossSever() then
					-- 	if TeamData.Instance.care_handle_crossserver ~= nil then
					-- 		LoginData.Instance:BaseData():Uncare(TeamData.Instance.care_handle_crossserver)
					-- 	end
					-- 		TeamData.Instance.care_handle_crossserver = LoginData.Instance:BaseData():KeysCare({"real_is_on_crossserver"},function ()
					-- 		if TeamData.Instance.care_handle_crossserver ~= nil then
					-- 			LoginData.Instance:BaseData():Uncare(TeamData.Instance.care_handle_crossserver)
					-- 		end
					-- 		if LoginData.Instance:IsOnCrossSever() then
					-- 			LogDG("111111111")
					-- 			TimeHelper:AddDelayTimer(function ()
									
					-- 			end)
					-- 			TeamData.Instance.is_crossserver_ing = false
					-- 		end
					-- 		LogDG("22222222222222")
					-- 	end)
					-- 	LogDG("00000000000")
					-- 	TeamData.Instance.is_crossserver_ing = true
					-- 	LoginCtrl:SendCrossStartReq()
					-- else
					-- 	SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HARMONY_HEAVEN, co.fb_type, co.seq)
					-- end
					return false, nil
				else
					LogDG("找不到副本配置")
				end
			end
			if co then
				local desc = string.format(Language.HarmonyHeaven.FightTeam.Tip[co.fb_type], co.fb_name, Language.HarmonyHeaven.FightTeam.TypeShow[co.fb_type])
				return false, {desc = desc, tnr = DTTodayNotRemind.HarmonyFight, trn_str = Language.Common.LoginNotRemind}
			end
			return false, nil
		end
	}
}

-- npc战斗需要发额外的参数 task_id
NPC_FIGHT_EXA_PARAM_TASKID = {[TeamConfig.TeamTargetType.HuanShiChallenge] = true}
