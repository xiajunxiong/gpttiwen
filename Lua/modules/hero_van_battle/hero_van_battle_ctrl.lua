FILE_REQ("modules/hero_van_battle/hero_van_battle_data")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_team_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_apply_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_glory_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_neo_rank_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_start")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_menu")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_preteam_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_rule_view")
VIEW_REQ("modules/hero_van_battle/hero_van_battle_pre_show_view")

HeroVanBattleCtrl = HeroVanBattleCtrl or BaseClass(BaseCtrl)
function HeroVanBattleCtrl:__init()
	if HeroVanBattleCtrl.Instance ~= nil then
		Debuger.LogError("[HeroVanBattleCtrl] attempt to create singleton twice!")
		return
	end	
    HeroVanBattleCtrl.Instance = self

	self.data = HeroVanBattleData.New()
    self:RegisterAllProtocols()
end 

function HeroVanBattleCtrl:OnInit()
    ViewMgr:RegisterOpenKeyFunc(Mod.HeroVanBattle.ApplyView, function ()
        self:OpenApplyView()
    end)

	ViewMgr:RegisterOpenKeyFunc(Mod.HeroVanBattle.Rule, function ()
        self:OpenRuleView()
    end)

	BehaviorData.Instance:CustomNpcTalkBtnsId(Config.hero_competition_auto.other[1].npc_id , 
	    BindTool.Bind(self.SetupBehaviourBtn, self),true)

	ActivityData.Instance:RegisterTopCondition(ActType.HeroVanBattle,BindTool.Bind(self.FlushTopCondition, self))

	ActivityData.Instance:RegisterCondition(ActType.HeroVanBattle,function()
        return ""-- self.task_data:GetConditionTip()
    end)

	ActivityData.Instance:CustomStatusButtonName(ActType.HeroVanBattle, ActStatusType.Close, Language.HeroVanBattle.StatusClose)
	ActivityData.Instance:CustomClickHandle(ActType.HeroVanBattle, BindTool.Bind(self.OnClickVanActButton, self))
	ActivityRandData.Instance:Register(Mod.HeroVanBattle.GloryView,function()
		return self.data:IsShowRankMainView() and not ( self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow())
	end)

	ActivityRandData.Instance:Register(Mod.HeroVanBattle.Main,function()
		local role_level = RoleData.Instance:GetRoleLevel()
		return self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow() and Config.hero_competition_auto.other[1].level <= role_level
	end)

	ActiveData.Instance:RegisterActivitySpBtnName(ActType.HeroVanBattle,BindTool.Bind(self.ActiveSpBtn, self))
    ActivityData.Instance:RegisterActivitySpStatus(ActType.HeroVanBattle,BindTool.Bind(self.ActiveSpStatus, self))
	ActiveData.Instance:RegisterActivitySpBtnUnAct(ActType.HeroVanBattle,BindTool.Bind(self.ActiveSpBtnUnAct, self))

	self.handle_login = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))

	-- 额外act_type追加注册
	-- ActivityData.Instance:CustomStatusButtonName(ActType.HeroVanBattleAgain, ActStatusType.Close, Language.HeroVanBattle.StatusClose)
	ActivityData.Instance:CustomClickHandle(ActType.HeroVanBattleAgain, BindTool.Bind(self.OnClickVanActButton, self))
	ActivityData.Instance:RegisterCondition(ActType.HeroVanBattleAgain,function() return "" end)
	ActivityData.Instance:RegisterTopCondition(ActType.HeroVanBattleAgain,BindTool.Bind(self.FlushTopCondition, self))

	ActivityData.Instance:CareEvent(BindTool.Bind(self.OnActEvent, self))
end 

-- 根据活动情况实时切换
function HeroVanBattleCtrl:OnActEvent()
	local name = self:GetStatusName()
	ActivityData.Instance:CustomStatusButtonName(ActType.HeroVanBattleAgain, ActStatusType.Open, name)
	ActivityData.Instance:CustomStatusButtonName(ActType.HeroVanBattleAgain, ActStatusType.Close, name)
	
end

function HeroVanBattleCtrl:OpenApplyView()
	ViewMgr:OpenView(HeroVanBattleApplyView)
end

function HeroVanBattleCtrl:OpenRuleView()
	ViewMgr:OpenView(HeroVanBattleRuleView)
end

function HeroVanBattleCtrl:OnClickVanActButton(param)
	-- 启动界面时打开界面，非启动竞技界面时传送
	if ViewMgr:IsOpen(AthleticsView) then 
		ViewMgr:OpenView(HeroVanBattleView)
	else 
		SceneLogic.Instance:AutoToNpcs(Config.hero_competition_auto.other[1].npc_id)
	end 
end

function HeroVanBattleCtrl:GetStatusName()
	return ActivityData.IsOpen(ActType.HeroVanBattle) and Language.HeroVanBattle.StatusOpen or Language.HeroVanBattle.StatusClose
end

function HeroVanBattleCtrl:FlushTopCondition()
	local can_enroll = self.data:TeamViewNpcCouldShow() -- 可报名
	local can_enter = self.data:EnterSceneNpcCouldShow() -- 可入场
	local battling = self.data:NeoRankNpcCouldShow() -- 战斗中
	local end_battle = self.data:EndRankNpcCouldShow() -- 已结束
	local readying = ActStatusType.Standy == ActivityData.Instance:GetActivityStatus(ActType.HeroVanBattle)

	local str = can_enter and Language.HeroVanBattle.BattleStart 
	or (can_enroll and Language.HeroVanBattle.EnrollTime 
	or (battling and Language.HeroVanBattle.Battling 
	or ( readying and Language.HeroVanBattle.VanReady 
	or Language.HeroVanBattle.EnrollNotTime)))

	local season_str = string.format(Language.HeroVanBattle.Season, DataHelper.GetDaXie(self.data.act_info.season_num))
	if end_battle then
		season_str = string.format(Language.HeroVanBattle.Season, DataHelper.GetDaXie(self.data.act_info.season_num))

		local dates = os.date("*t",self.data.act_info.start_sign_up_timestamp)
		str = string.format(Language.HeroVanBattle.NextEnrollTime,dates.month,dates.day)
	end  

	return season_str.."\n"..str
end

-- 活跃的按钮动态变化
function HeroVanBattleCtrl:ActiveSpBtn()
	local can_enroll = self.data:TeamViewNpcCouldShow() -- 可报名
	local enter_team = self.data:TypeApplyIsCreateSuccess() -- 是否已报名
	local can_enter = self.data:EnterSceneNpcCouldShow() -- 可入场
	local act_flag = ActivityData.IsOpen(ActType.HeroVanBattle) -- 活动状态
	local is_cross = LoginData.Instance:BaseData().is_on_crosssever -- 是否跨服

	--LogDG("?活跃按钮!@是否报名期",can_enroll,"是否已报名",enter_team,"是否入场期",can_enter,"活动是否开启",act_flag)
	-- 活动未开启+报名期+未报名 = 前往报名（亮
	if not act_flag and can_enroll and not enter_team and not can_enter then
		return Language.HeroVanBattle.MainEnrollText
    -- 活动未开启+报名期+报了名 = 已报名（灰
	elseif not act_flag and can_enroll and enter_team and not can_enter then 
		return Language.HeroVanBattle.MainEnrollComplete
	-- 活动开启+报了名+可入场 = 前往（亮
    elseif act_flag and enter_team and can_enter then 
		return Language.HeroVanBattle.MainGoMatch
	-- 活动开启+未报名+可入场 = 未报名（灰
    elseif act_flag and not enter_team and can_enter then 
		return Language.HeroVanBattle.MainEnrollUnComplete
	-- 活动开启+不可入场+未报名 = 未报名（灰
	elseif act_flag and not can_enter and not enter_team then 
		return Language.HeroVanBattle.MainEnrollUnComplete
	-- 活动开启+结束入场+报了名 = 前往（灰
	elseif act_flag and not can_enter and enter_team then 
		return Language.HeroVanBattle.MainGoMatch
	-- 活动未开启+非报名期 = 原逻辑
	elseif not act_flag and not can_enroll then 
		return nil
		-- 其他不管 前往（亮
	else 
		return Language.HeroVanBattle.MainGoMatch
	end 
end

function HeroVanBattleCtrl:ActiveSpBtnUnAct()
	-- 等级判断
	local level_flag = ActivityData.Instance:GetActivityInfo(ActType.HeroVanBattle):LevelRestrict() <= RoleData.Instance:GetRoleLevel()
	if not level_flag then 
		return nil
	end 

	local can_enroll = self.data:TeamViewNpcCouldShow() -- 可报名
	local enter_team = self.data:TypeApplyIsCreateSuccess() -- 是否已报名
	local can_enter = self.data:EnterSceneNpcCouldShow() -- 可入场
	local act_flag = ActivityData.IsOpen(ActType.HeroVanBattle) -- 活动状态
	local is_cross = LoginData.Instance:BaseData().is_on_crosssever -- 是否跨服

	--LogDG("?活跃置灰飘字!@是否报名期",can_enroll,"是否已报名",enter_team,"是否入场期",can_enter,"活动是否开启",act_flag)
    -- 活动未开启+报名期+报了名 = 您已报名，期待您在英雄会武中的精彩表现~
	if not act_flag and can_enroll and enter_team and not can_enter then 
		return Language.HeroVanBattle.UnActTipsTeam
	-- 活动开启+未报名+可入场 = 抱歉，报名阶段已结束，请您下次参与
    elseif act_flag and not enter_team and can_enter then 
		return Language.HeroVanBattle.UnActTipsUnTeam
	-- 活动开启+结束入场+未报名 = 抱歉，报名阶段已结束，请您下次参与
	elseif act_flag and not can_enter and not enter_team then 
		return Language.HeroVanBattle.UnActTipsUnTeam
	-- 活动开启+结束入场+报了名 = 抱歉，入场阶段已结束，请您下次参与 
	elseif act_flag and not can_enter and enter_team then 
		return Language.HeroVanBattle.UnActTipsEndEnter
	-- 活动未开启+不在报名期 = 抱歉，暂未到赛事报名时间
    elseif not act_flag and not can_enroll then 
		return Language.HeroVanBattle.BeforeSignUpTime
	-- 其他不管 
	else 
		return nil
	end 
end

-- 活跃的活动状态变化
function HeroVanBattleCtrl:ActiveSpStatus()
	-- 等级判断
	local level_flag = ActivityData.Instance:GetActivityInfo(ActType.HeroVanBattle):LevelRestrict() <= RoleData.Instance:GetRoleLevel()
	if not level_flag then 
		return nil
	end 

	-- 报名期且未报名·亮
	local can_enroll = self.data:TeamViewNpcCouldShow() -- 可报名
	local enter_team = self.data:TypeApplyIsCreateSuccess() -- 报了名
	local can_enter = self.data:EnterSceneNpcCouldShow() -- 可入场
	local act_flag = ActivityData.IsOpen(ActType.HeroVanBattle) -- 活动状态
	local is_cross = LoginData.Instance:BaseData().is_on_crosssever -- 是否跨服

	--LogDG("?活跃状态变化!@是否报名期",can_enroll,"是否已报名",enter_team,"是否入场期",can_enter,"活动是否开启",act_flag)
	if act_flag then 
	-- 活动期间 
	-- 报了名+可入场 亮
	    return (can_enter and enter_team) and ActStatusType.Open or ActStatusType.Close
	else
	-- 非活动期间
	-- 未报名+可报名
	-- 特别分类 非活动期间+不可报名回空 沿用通用逻辑
	    if can_enroll and not enter_team then 
			return ActStatusType.Open
		else 
			return nil
		end 
	end 

	-- return ((not act_flag and can_enroll and not enter_team) or (act_flag and can_enter and enter_team)) and  ActStatusType.Open or ActStatusType.Close
end

function HeroVanBattleCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	local list_plus = {}
	-- 报名按钮
	local act_enroll_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActEnroll, function ()

		if not TeamData.Instance:InTeam() or TeamData.Instance:MemberNumRole() < 5 then 
			PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.OutTeamError)
			return 
		end 

		if not self.data:MemberTeamCheck() then 
			if LoginData.Instance:IsOnCrossSever() then 
				PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.TeamMemberError)
			end 
			return 
		end 

		self.data:TypeRuleGetNeedShow()
	end, true)
	if self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow() and self.data:CheckWorldLevel() and FunOpen.Instance:GetFunIsOpened(Mod.HeroVanBattle.Main) then 
		-- 当应当展示报名界面的时候进行一次数据刷新
		self:SendTeamArenaReq(0)
		table.insert(list_plus, act_enroll_button)
	end 

	-- 入场按钮
	local act_enter_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActEnterMatch, function ()
		SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HERO_VAN_BATTLE_FB)
	end, true)
	if self.data:EnterSceneNpcCouldShow() and self.data:CheckWorldLevel() and self.data:TypeApplyIsCreateSuccess() then 
		table.insert(list_plus, act_enter_button)
	end 

	-- 战队浏览
	local act_teamView_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActTeamView, function ()
		self.data:TypeTeamViewShowTeam()
	end, true)
	if self.data:TeamViewNpcCouldShow() then 
		table.insert(list_plus, act_teamView_button)
	end 

	-- -- 实时排名
	-- local act_neoRank_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActNeoRankView, function ()
	-- 	self.data:TypeTeamViewShowTeam()
	-- end, true)
	-- if self.data:NeoRankNpcCouldShow() then 
	-- 	table.insert(list_plus, act_neoRank_button)
	-- end 

	-- 赛后排名
	local act_endRank_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActNeoRankView, function ()
		ViewMgr:OpenView(HeroVanBattleGloryView)
	end, true)
	if self.data:EndRankNpcCouldShow() then 
		table.insert(list_plus, act_endRank_button)
	end 

	-- 赛事规则
	local act_view_button = NpcDialogBehBtnData.FuncCreate(Language.HeroVanBattle.BtnActView, function ()
		self.data.is_pre_click = true
		ViewMgr:OpenView(HeroVanBattleView)
	end, true)
	table.insert(list_plus, act_view_button)

	return list_plus
end

function HeroVanBattleCtrl:OnLoginEvent()
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
		if not LoginData.Instance:IsOnCrossSever() then 
			return 
		end 

		-- 跨服 报名期内 且 没有报名信息时应主动刷新数据
		if self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow() then 
			if self.data:IsNeedFlushSign() then self:SendTeamArenaReq(0) end 
		end 
	end 
end 

function HeroVanBattleCtrl:SendSignInfo()
	self:SendTeamArenaReq(6)
end

function HeroVanBattleCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSWorldTeamArenaReq)
	self:RegisterProtocol(CSWorldTeamArenaSignUpReq)
	self:RegisterProtocol(CSActivityWorldTeamArenaSceneReq)
	self:RegisterProtocol(CSActivityWorldTeamArenaRankReq)
	self:RegisterProtocol(SCWorldTeamArenaAllSimpleInfo,"RecvWorldTeamArenaAllSimpleInfo")
	self:RegisterProtocol(SCWorldTeamArenaPreTeamSignUpInfo,"RecvWorldTeamArenaPreTeamSignUpInfo")
	self:RegisterProtocol(SCWorldTeamArenaNoticeInfo,"RecvWorldTeamArenaNoticeInfo")
	self:RegisterProtocol(SCWorldTeamArenaTeamSignUpDetailedInfo,"RecvWorldTeamArenaTeamSignUpDetailedInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaInfo,"RecvActivityWorldTeamArenaInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaRankAllInfo,"RecvActivityWorldTeamArenaRankAllInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaMyInfo,"RecvActivityWorldTeamArenaMyInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaSpecialInfo,"RecvActivityWorldTeamArenaSpecialInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaTeamSpecialInfo,"RecvActivityWorldTeamArenaTeamSpecialInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaFightInfo,"RecvActivityWorldTeamArenaFightInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaFinallyRankInfo,"RecvActivityWorldTeamArenaFinallyRankInfo")
	self:RegisterProtocol(SCActivityWorldTeamArenaOtherInfo,"RecvActivityWorldTeamArenaOtherInfo")
end

-- 英雄会武请求
-- 0 请求报名信息，1 确认成员报名，2 取消报名，3 请求查询某个战队的详细信息 param1 id
function HeroVanBattleCtrl:SendTeamArenaReq(op_type,param1,param2)
	-- --LogDG("4600?SendTeamArenaReq",op_type,param1,param2)
	local protocol = GetProtocol(CSWorldTeamArenaReq)
    protocol.op_type = op_type
	protocol.param1 = param1 or {[1] = 0,[2] =0 }
	protocol.param2 = param2 or 0
	--LogDG("4600?杰哥哥 请求协议内容哦 end !",protocol)
	-- --LogDG("4600?杰哥哥 请求协议内容哦param1 end !",tostring(int64.new(protocol.param1)))
    SendProtocol(protocol)
end

-- 英雄会武报名请求
function HeroVanBattleCtrl:SendTeamArenaSignUpReq(op_type,name)
	-- --LogDG("4602?SendTeamArenaSignUpReq",op_type,name)
	local protocol = GetProtocol(CSWorldTeamArenaSignUpReq)
    protocol.op_type = op_type
	protocol.name = name or 0
	--LogDG("4602?杰哥哥 报名请求协议内容哦 end !",protocol)
    SendProtocol(protocol)
end 

-- 英雄会武场景操作请求
function HeroVanBattleCtrl:SendTeamTeamArenaSceneReq(op_type,param1)
	-- --LogDG("4607?SendTeamTeamArenaSceneReq",op_type,( param1 == nil and "nil" or param1 ))
	local protocol = GetProtocol(CSActivityWorldTeamArenaSceneReq)
    protocol.op_type = op_type
	protocol.param1 = param1 or 0
	--LogDG("4607?杰哥哥 场景请求协议内容哦 end !",protocol)
    SendProtocol(protocol)
end 

-- 英雄会武最终榜单请求
function HeroVanBattleCtrl:SendTeamTeamArenaRankReq()
	--LogDG("杰哥我想看最终榜单？4613")
	local protocol = GetProtocol(CSActivityWorldTeamArenaRankReq)
    SendProtocol(protocol)
end

-- 所有报名战队信息下发
function HeroVanBattleCtrl:RecvWorldTeamArenaAllSimpleInfo(protocol)
	--LogDG("杰哥这是战队信息下发哦！4601 ",protocol)
	self.data:SetAllSimpleInfo(protocol)
end

-- 英雄会武预备报名信息下发
function HeroVanBattleCtrl:RecvWorldTeamArenaPreTeamSignUpInfo(protocol)
	--LogDG("杰哥哥救救我这是预备报名信息哦？4603 ",protocol)
	self.data:SetPreTeamSignUpInfo(protocol)

	if not ViewMgr:IsOpen(HeroVanBattleApplyView) then 
		self.data:TypeApplyViewMember()
	end 
end

-- 英雄会武通知信息下发
-- op_type = 0 成员确认报名 param1:member_index param2:determine_flag
-- op_type = 1 报名失败关闭报名界面 清除本地的报名信息！
function HeroVanBattleCtrl:RecvWorldTeamArenaNoticeInfo(protocol)
	-- --LogDG("4604 ",protocol)
	if protocol.notice_type == 0 then 
		--LogDG("杰哥哥，这是队员确认报名信息更新哦？4604 ",protocol)
		self.data:SetTeamArenaNoticeInfo(protocol)
	elseif protocol.notice_type == 1 then 
		--LogDG("杰哥哥，这是报名失败信息下发哦？4604 ",protocol)
		if ViewMgr:IsOpen(HeroVanBattleApplyView) then ViewMgr:CloseView(HeroVanBattleApplyView) end 

		self.data:SetPreTeamSignUpInfo({})
	end 
end

-- 英雄会武战队详细信息下发
function HeroVanBattleCtrl:RecvWorldTeamArenaTeamSignUpDetailedInfo(protocol)
	--LogDG("杰哥哥，这是详细的战队信息下发哦？4605 ",protocol)
	self.data:SetTeamSignUpDetailedInfo(protocol)

	if self.data:TypeApplyIsCreateSuccess() and ViewMgr:IsOpen(HeroVanBattleApplyView) then 
		ViewMgr:CloseView(HeroVanBattleApplyView)
	end 
end

-- 英雄会武活动信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaInfo(protocol)
	--LogDG("这是杰哥的活动信息?4606 ",protocol)
	self.data:SetTeamArenaInfo(protocol)

	ActivityRandData.Instance:CheckRandOpenData()
end

-- 英雄会武排行榜信息
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaRankAllInfo(protocol)
	--LogDG("这是杰哥的排行榜信息?4608 ",protocol)
	self.data:SetTeamArenaRankAllInfo(protocol)
end

-- 英雄会武自身战队排行榜信息
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaMyInfo(protocol)
	--LogDG("这是杰哥的自身战队排行信息?4609 ",protocol)
	self.data:SetTeamTeamArenaMyInfo(protocol)
end

-- 英雄会武特殊信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaSpecialInfo(protocol)
	--LogDG("杰哥哥 这是场景战斗信息下发哦？4610 ",protocol)
	self.data:SetTeamTeamArenaSpecialInfo(protocol)
end

-- 英雄会武特殊队伍信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaTeamSpecialInfo(protocol)
	--LogDG("杰哥哥，这是报名队伍信息哦？4611 ",protocol)
	self.data:SetArenaTeamViewSpecialInfo(protocol)
end

-- 英雄会武发起战斗信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaFightInfo(protocol)
	--LogDG("杰哥哥，发起战斗的信息哦？4612 ",protocol)
	self.data:SetTeamArenaFightInfo(protocol)
end

-- 英雄会武最终榜单信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaFinallyRankInfo(protocol)
	--LogDG("杰哥最终榜单救救我?4614 ",protocol)
	self.data:SetTeamArenaFinallyRankInfo(protocol)
end

-- 英雄会武其他信息下发
function HeroVanBattleCtrl:RecvActivityWorldTeamArenaOtherInfo(protocol)
	--LogDG("杰哥其他信息救救我?4615 ",protocol)
	self.data:SetTeamArenaOtherInfo(protocol)
end 
