
MainViewChat = MainViewChat or DeclareMono("MainViewChat", UIWFlushPanel)

function MainViewChat:MainViewChat()
	self.data = ChatCtrl.Instance:Data()
	self.mv_data = MainViewData.Instance
	self.main_base_data = MainViewData.Instance:BaseData()
	self.login_data = LoginData.Instance:BaseData()
	self.battle_data = BattleData.Instance
	self.battle_base_data = BattleData.Instance:BaseData()
	self.bullet_comments = self.battle_data.bullet_comments
	self.activity_data = ActivityData.Instance
	self.society_data = SocietyCtrl.Instance:Data()
	self.data_cares = {
		{data = self.battle_data:BaseData(), func = self.BulletSwitch, keys = {"bullet_on"}},
		{data = self.bullet_comments, func = self.FlushBulletComments},
		{ data = self.data.chat_unread, func = self.FlushUnread },
		{ data = self.data.show_back, func = self.ShowBack },
		{ data = self.main_base_data, func = self.ShowBack, keys = {"is_battle"}, init_call = false},
		{ data = self.login_data, func = self.ShowBack, keys = {"is_on_crosssever"}, init_call = false},
		{ data = RoleData.Instance:GetBaseData(), func = self.ShowBack, keys = {"level"}, init_call = true},
		{ data = SceneData.Instance:Base(), func = self.ShowBack, keys = {"scene_id"}, init_call = false},
		{ data = self.battle_data:BaseData(), func = self.ClearBulletScreen, keys = {"clear_bullets"}, init_call = false},

		{data = self.activity_data.activity_status_event, func = self.ShowBullet, keys = {"act_type", "status"}},
		{data = SceneData.Instance:Base(), func = self.ShowBullet, keys = {"scene_mode"} },
		{data = self.main_base_data, func = self.ShowBullet, keys = {"is_battle"}},
		{data = GuildData.Instance.embattle, func = self.ShowBullet, keys = {"flushstate"}},
		{data = self.society_data.my_rank_show, func = self.FlushRankShow},
		{data = RoleData.Instance:GetBaseData(), func = self.ChangeGameLevel, keys = {"level"}},
		{data = LevelComplementData.Instance.data_info, func = self.FlushLevelComplementIcon},
		{data = TeamData.Instance.team_invited_list, func = self.FlushBtnInvitedItem},
		{data = ServerActivityData.Instance.paimaihang_other_info, func = self.FlushPaiHangBangIcon},
		{data = self.login_data, func = self.FlushPaiHangBangIcon, keys = {"is_on_crosssever"}, init_call = false},
		{data = YinYangData.Instance.view_data, func = self.FlushYinYangEye, keys = {"opened"}, init_call = true},
		{data = YinYangData.Instance.view_data, func = self.SetYinYangEyeBtnShow, keys = {"show"}, init_call = false},
		{data = YinYangData.Instance.explore_data, func = self.FlushYinYangEyeEffect, keys = {"near_vo","near_distance"}, init_call = true},
		{data = RewardData.Instance.activity_back_red_point, func = self.FlushBtnFind},
	}
	--新增几个活动的场景里显示弹幕聊天
	-- OnlyFightData.Instance:IsInOnlyFight()
	-- GuildData.Instance:IsOpenGuildMelee()
	-- GuildData.Instance:IsInGuildHonorFB()
	self.yinyang_eye_effect = {
		4161031, 4161032, 4161033, 4161034,  4161035
	}
	self.yinyang_effect_key = 0

	GuideManager.Instance:RegisterGetGuideUi(
        "YinYangEyeBtn",
        function()
           return self.BtnYinYang, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi("LevelComplementBtn", function()
		return self.BtnLevelComplement, BindTool.Bind1(self.OnClickLevelComplement, self)
	end)

	-- TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.FlushYinYangBtn, self))
	TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.FlushYinYangBtn, self),{YinYangData.Instance.unlock_task})
	EventSys:Bind(GlobalEventKeys.TaskCompleteDataNotice, BindTool.Bind(self.TaskCompleteDataNotice, self))
	self:FlushYinYangBtn()
end
function MainViewChat:TaskCompleteDataNotice()
	self:FlushYinYangBtn()
end
function MainViewChat:FlushYinYangBtn()
	self.YinYangInter.Interactable = YinYangData.Instance:FuncIsOpen()
	if self.YinYangInter.Interactable == true then
		YinYangCtrl.Instance:CreateYinYangObj()
	end
end

function MainViewChat:SetYinYangEyeBtnShow()
	self.BtnYinYang:SetActive(YinYangData.Instance.view_data.show)
end

function MainViewChat:FlushYinYangEye()
	self.YinYangToggle.isOn = YinYangData.Instance.view_data.opened
end
function MainViewChat:FlushYinYangEyeEffect()
	local vo = YinYangData.Instance.explore_data.near_vo
	local dis = YinYangData.Instance.explore_data.near_distance
	if vo and dis ~= 0 then
		local effect_key = self.yinyang_eye_effect[dis / YinYangData.Instance.remindx_radius]
		if self.yinyang_effect_key ~= effect_key then
			if self.handle_yinyang_effect then
				self.EffectTool:Stop(self.handle_yinyang_effect)
			end
			self.handle_yinyang_effect = self.EffectTool:Play(effect_key)
			self.yinyang_effect_key = effect_key
		end
	else
		if self.handle_yinyang_effect then
			self.EffectTool:Stop(self.handle_yinyang_effect)
		end
		self.yinyang_effect_key = 0
	end
end
function MainViewChat:Awake()
	UIWFlushPanel.Awake(self)
	FunOpen.Instance:Register(
        Mod.YinYangEye.Main,
        function(opened)
			local show = opened and not self.main_base_data.is_battle and SceneData.Instance:GetMode() ~= SceneModeType.WeddingFB
			show = show and self:YinYangBtnIsShow()
            self.BtnYinYang:SetActive(show)
        end
    )
	FunOpen.Instance:Register(
        Mod.Rank.Main,
        function(opened)
            self.BtnRankObj:SetActive(opened)
        end
    )
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.SocietyBtnObj:SetActive(false)
		self.SocietyBtnAuditObj:SetActive(true)
		UH.SpriteName(self.SocietyAuditImg, "SheJiao_"..(AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1))
	else
		self.SocietyBtnObj:SetActive(true)
		self.SocietyBtnAuditObj:SetActive(false)
	end
end
function MainViewChat:FlushRankShow()
	local mrs = self.society_data.my_rank_show.val
	self.RankImage.enabled = 0 == mrs
	self.RankVal.text = mrs > 0 and mrs or ""
end

function MainViewChat:ShowBullet()
	if OnlyFightData.Instance:IsInOnlyFight(true, true) then
		self.BtnBullet:SetActive(true)
		BattleData.Instance:BaseData().bullet_on = true
		return
	end
	if GuildData.Instance:IsOpenGuildMelee(true, true) then
		self.BtnBullet:SetActive(true)
		BattleData.Instance:BaseData().bullet_on = true
		return
	end
	if GuildData.Instance:IsInGuildHonorFB(true, true) then
		self.BtnBullet:SetActive(true)
		return
	end
	if SCENE_TYPE.WEDDING_FB == SceneData.Instance:GetMode() then
		BattleData.Instance:BaseData().bullet_on = true
		self.BtnBullet:SetActive(true)
		return
	end
	if SCENE_TYPE.MARRY_PARTY == SceneData.Instance:GetMode() then
		BattleData.Instance:BaseData().bullet_on = true
		self.BtnBullet:SetActive(true)
		return
	end
	if self.main_base_data.is_battle == true then
		self.BtnBullet:SetActive(true)
	else
		BattleData.Instance:BaseData().bullet_on = false
		self.BtnBullet:SetActive(false)
	end
end
function MainViewChat:FlushBulletComments()
    for _, comment in ipairs(self.bullet_comments) do
        self.BulletScreen:Spwan(comment)
    end
    self.bullet_comments:Clear()
end
function MainViewChat:BulletSwitch()
	self.BulletScreen:SetObjActive(self.battle_data.base.bullet_on)
	self.BulletSender:SetActive(false)
end

function MainViewChat:ClearBulletScreen()
	self.BulletScreen:RecycleActiveObjs()
end

function MainViewChat:FlushUnread()
	local family_unread = self.data.chat_unread[ChatConfig.ChannelId.guild]
	local team_unread = self.data.chat_unread[ChatConfig.ChannelId.team]
	self.BtnTFObj:SetActive(family_unread > 0 or team_unread > 0)
	local chat_unread = Language.Chat.ChatUnread
	if team_unread > 0 then
		self.TFUnread.text = team_unread > 99 and chat_unread.more_t or string.format(chat_unread.less_t, team_unread)
	else
		self.TFUnread.text = family_unread > 99 and chat_unread.more_f or string.format(chat_unread.less_f, family_unread)
	end
end

function MainViewChat:ShowBack()
	self.BtnBackObj:SetActive((self.data.show_back.val and not self.main_base_data.is_battle) or BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE == self.battle_base_data.battle_mode)
	self.BtnBack.text = BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE == self.battle_base_data.battle_mode and Language.Chat.BtnBackShow.back_battle or Language.Chat.BtnBackShow.back_fb
	-- self.login_data.is_on_crosssever and 
	if not self.login_data.is_on_crosssever and RoleData.Instance:GetRoleLevel() < 30 then
		self.BtnBackCrossObj:SetActive(false)
	else
		self.BtnBackCrossObj:SetActive((not self.main_base_data.is_battle) and not self.data.show_back.val)
	end
	UH.SetText(self.BtnBackTex, Language.Chat.CrossSever2[self.login_data.is_on_crosssever and 2 or 1])

	self.BtnYinYang:SetActive(self:YinYangBtnIsShow())
	self.GuildRoleNum:SetActive(GuildData.Instance:IsInAnswerScene())
end
function MainViewChat:YinYangBtnIsShow()
	if not YinYangData.Instance.view_data.show then
		return false
	end
	if FunOpen.Instance:GetFunIsOpened(Mod.YinYangEye.Main) == true then
		if self.main_base_data.is_battle == true then
			return false
		end
		if SceneData.Instance:GetMode() == SceneModeType.WeddingFB then
			return false	
		end
		local scene_id = SceneData.Instance:SceneId()
    	local is_cant_show = YinYangData.Instance:IsCantShow(YinYangData.ShieldType.Scene, scene_id)
		if is_cant_show == true then
			return false
		end
		return true
	end
	return false
	
end

--等级改变需要解锁图标
function MainViewChat:ChangeGameLevel()
    self:FlushLevelComplementIcon()
end

function MainViewChat:FlushLevelComplementIcon()
	local is_open = LevelComplementData.Instance:IsFunOpen()
	if not is_open then
		self.BtnLevelComplement:SetActive(false)
		return
	end
	self.BtnLevelComplement:SetActive(true)
	--刷新进度条
	local remain_exp = LevelComplementData.Instance:GetRemainExp()
	local exp_limit = LevelComplementData.Instance:GetExpLimit()
	self.LevelComplementSlider:SetProgress(exp_limit > 0 and remain_exp / exp_limit or 0)
	--显示10秒弹窗
	if remain_exp > 0 and LevelComplementData.Instance:IsShowTips() then
		LevelComplementData.Instance:SetIsShowTips(false)
		self.LevelComplementBubble:SetActive(true)
		self.LevelComplementEffect:SetActive(true)
		local show_str = string.format(Language.LevelComplement.SmallTips, remain_exp, LevelComplementData.Instance:GetMultiple() * 100)
		UH.SetText(self.LevelComplementDesc, show_str)
		self.level_complement_timer = TimeHelper:AddDelayTimer(BindTool.Bind(self.CloseLevelComplementTips, self), LevelComplementData.Instance:ShowTipsTime())
	end
end
function MainViewChat:CloseLevelComplementTips()
	TimeHelper:CancelTimer(self.level_complement_timer)
	self.LevelComplementBubble:SetActive(false)
	self.LevelComplementEffect:SetActive(false)
end

-- 阴阳眼上方的提示按钮{
--收到组队请求
function MainViewChat:FlushBtnInvitedItem()
	self.BtnInvitedItem:SetData(TeamData.Instance.team_invited_list:Val())
end

-- 点击找回按钮
function MainViewChat:OnFindClick()
	self.ObjFind:SetActive(false)
	ViewMgr:OpenViewByKey(Mod.Activity.ActivityBack)
	RewardData.Instance:SetMainFindNotShow()
end

-- 找回按钮刷新
function MainViewChat:FlushBtnFind()
	if RewardData.Instance:MainFindFlag() then
		local flag = RewardData.Instance.activity_back_red_point.val
		self.ObjFind:SetActive(flag)
		self.FindRemind:SetNum(flag and 1 or 0)
	else
		self.ObjFind:SetActive(false)
		self.FindRemind:SetNum(0)
	end
end
-- 阴阳眼上方的提示按钮}

function MainViewChat:FlushPaiHangBangIcon()
	if not self.login_data.is_on_crosssever and ServerActivityData.Instance.paimaihang_other_info.show_record == true then
		self.BtnPaiMaiHang:SetActive(true)
	else
		self.BtnPaiMaiHang:SetActive(false)
	end
	if MainPlayer then
        MainPlayer:StopMove()
    end
end

function MainViewChat:OnClickSociety()
	ViewMgr:OpenView(SocietyView)
end

function MainViewChat:OnClickRank()
	ViewMgr:OpenView(RankView)
end

function MainViewChat:OnClickBullet()
    self.BulletSender:SetActive(not self.BulletSender.activeSelf)
end


function MainViewChat:OnClickTF()
	local team_unread = self.data.chat_unread[ChatConfig.ChannelId.team]
	self.data:SelChannelId(team_unread > 0 and ChatConfig.ChannelId.team or ChatConfig.ChannelId.guild, true)
	ViewMgr:OpenView(ChatView)
end

function MainViewChat:OnDownVoice(type)
	self.data:VoiceStart()
end

function MainViewChat:OnUpVoice()
	self.data:VoiceFinish()
end

function MainViewChat:OnDragVoice()
	self.data:VoiceCheck()
end

function MainViewChat:OnClickSetting()
	ViewMgr:OpenView(ChatViewChannelSel)
end

function MainViewChat:OnClickList()
	ViewMgr:OpenView(ChatView)
end

function MainViewChat:OnClickBack()
	if BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE == BattleData.Instance:BattleMode() then
		local info = {
			content = Language.Main.Battle.GiveUpTip,
			confirm = {
					func = function ()
					PublicPopupCtrl.Instance:CloseDialogTips()
					BattleCtrl.GiveUp()
					end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
		return
	end
	local mode = SceneData.Instance:GetMode()
	if mode == SceneModeType.FightGather then
		MainOtherCtrl.Instance:CloseGather()
	end
	if MainPlayer then
		MainPlayer:StopMove()
	end
	local tip = Language.Chat.BackTips .. FightCtrl.GetWarnTips(mode)
	if mode == SceneModeType.RabbitRace then
		tip = Language.RabbitRun.Tip1
	end
	PublicPopupCtrl.Instance:AlertTip(tip,function()
		if SceneData.Instance:GetMode() == SceneModeType.Material then
			MaterialCtrl.Instance:OnBackChallengeView()
		else
			SceneCtrl.Instance:RequestLeaveFb()
		end
	end)
end
function MainViewChat:OnClickYinYangEye()
	if not YinYangData.Instance:FuncIsOpen() then
		PublicPopupCtrl.Instance:Center(ErrorText[189])
		return
	end
	-- self.YinYangToggle.isOn = not self.YinYangToggle.isOn
	--[[ if self.YinYangToggle.isOn == true then
		YinYangCtrl.CSYinYangEyeOpReq(0)
	else
		
	end ]]
	OperateFrequency.Operate(function ()
		YinYangCtrl.CSYinYangEyeOpReq(0)
	end,"yinyangeye_openorclose", 2)
	
end
--返回原服
function MainViewChat:OnClickBackCross()
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return true
    end
	if self.login_data.is_on_crosssever == false then
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackCrossTips2,function()
			if BattleData.Instance:BaseData().server_fighting == true then
				PublicPopupCtrl.Instance:Center(ErrorText[170])
			else
				LoginCtrl.Instance:SendCrossStartReq()
			end
		end)
		return
	end
	local mode = SceneData.Instance:GetMode()
	if mode == SceneModeType.None then
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackCrossTips,function()
			if BattleData.Instance:BaseData().server_fighting == true then
				PublicPopupCtrl.Instance:Center(ErrorText[154])
			else
				LoginCtrl.Instance:SendBackToOriginServer()
			end
		end)
	else
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackSceneTips .. FightCtrl.GetWarnTips(mode),function()
			if BattleData.Instance:BaseData().server_fighting == true then
				PublicPopupCtrl.Instance:Center(ErrorText[154])
			else
				SceneCtrl.Instance:RequestLeaveFb()
				LoginCtrl.Instance:SendBackToOriginServer()
			end
		end)
	end
end

function MainViewChat:OnClickLevelComplement()
    ViewMgr:OpenView(LevelComplementView)
	self:CloseLevelComplementTips()
end

function MainViewChat:OnClickLevelComplementBubble()
	self:CloseLevelComplementTips()
    ViewMgr:OpenView(LevelComplementView)
end

function MainViewChat:OnClickPaiMaiHang()
	ViewMgr:OpenView(PaiMaiHangView)
end

MainViewChatRecordList = MainViewChatRecordList or DeclareMono("MainViewChatRecordList", UIWFlushPanel)

function MainViewChatRecordList:MainViewChatRecordList()
	self.data = ChatCtrl.Instance:Data()
	
	self.data_cares = {
		{data = self.data:GetChatChannelList(), func = self.FlushRecordList},
		-- {data = self.data.need_flush, func = self.FlushRecordList, init_call = false},
		{data = self.data.trumpet_switch, func = self.FlushTrumpetInfo},
		{data = self.data.trumpet_switch, func = self.FlushTrumpetInfo},
		{data = self.data.world_question_info, func = self.FlushWorldQuestion},
	}

	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))
end

function MainViewChatRecordList:RoleNoticeComplete()
    self:FlushRecordList()
end

function MainViewChatRecordList:FlushRecordList()
	local chat_list = self.data:GetMainViewChatList()
    self.RecordList:SetDataList(chat_list)
    self.RecordList:JumpToDown()
end

function MainViewChatRecordList:FlushTrumpetInfo()
	local value, trumpet_info = self.data:NextTrumpetInfo()
	if value then
		self:FlushTrumpetDelay(trumpet_info, 5)
	end
	return value
end

function MainViewChatRecordList:FlushWorldQuestion()
	local info = self.data.world_question_info
	local value = (RoleData.Instance:GetRoleLevel() >= GLOBAL_CONFIG:TalkLevel() and info.now_answer_subject > 0 and info.answer_tamp > 0 and 1 == info.is_broadcast) or self.WorldQuestionObj.activeSelf
	for k,v in pairs(ChatConfig.HideQuestionView) do
		if ViewMgr:IsOpen(ModToView[Mod.ParseKey(v)]) then
			value = false
			break
		end
	end
	if ViewMgr:IsOpen(EndlessTowerView) or
	RuinsData.Instance:IsInRuins() or PalacesData.Instance:IsInPalaces() then
		value = false
	end
	if FishData.Instance:InFishMap() or LookingForRabbitsData.Instance:IsInGame() or LuckyRingData.Instance:InRing() then
		value = false
	end
	self.WorldQuestionObj:SetActive(value)
	TimeHelper:CancelTimer(self.timer_world_question)
	if not value then return end
	local language = Language.Chat.WorldQuestion
	local co_q, co_a = ChatData.Instance:WorldQuestionCoById(info.now_answer_subject)
	self.WorldQuestionDesc.text = co_q.question
	self.timer_world_question = TimeHelper:AddCountDownCT(function ()
		self.WorldQuestionTitle.text = string.format(language.title, math.floor(info.answer_tamp - TimeHelper.GetServerTime()))
	end, function ()
		self.WorldQuestionObj:SetActive(false)
	end,
	info.answer_tamp, 1)
end

function MainViewChatRecordList:OnClickWorldQuestionAnswer()
	self.data:SelChannelId(ChatConfig.ChannelId.world, true)
	ViewMgr:OpenView(ChatView)
	self.WorldQuestionObj:SetActive(false)
end

function MainViewChatRecordList:OnClickWorldQuestionClose()
	self.WorldQuestionObj:SetActive(false)
end

function MainViewChatRecordList:FlushTrumpetDelay(trumpet_info, time)
	TimeHelper:CancelTimer(self.timer_dt)
	self.TrumpetName.text = trumpet_info.name
	self.TrumpetContent.text = trumpet_info.content
	self.TrumpetObj:SetActive(true)
	self.timer_dt = TimeHelper:AddDelayTimer(function ()
		if 5 == time then
			local value = self:FlushTrumpetInfo()
			if not value then
				self:FlushTrumpetDelay(trumpet_info, 15)
			end
		else
			self.TrumpetObj:SetActive(false)
		end
	end, time)
end

function MainViewChatRecordList:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_world_question)
	EventSys:UnBind(self.complete_handle)
    self.complete_handle = nil
end


MainViewChatRecordItem = DeclareMono("MainViewChatRecordItem", RichTextItem)
--RecIconAnim=====Animation
--RecTimeTxt======Text
--RecTran=========RectTransform

function MainViewChatRecordItem:MainViewChatRecordItem()
	RichTextHelper.Bind(self.Content, BindTool.Bind(self.OnClickLink, self))
	self.care_handle = ChatData.Instance:RecordData():Care(BindTool.Bind(MainViewChatRecordItem.FlushPlayingKey,self),"cur_playing_key")
end

function MainViewChatRecordItem:ExpendSize(data, axis)
    return data:HeightMainViewChat(self.Content)
end

function MainViewChatRecordItem:SetData(data)
	RichTextItem.SetData(self,data)
	self.Content.text = data:GetMainViewChat()
	self.ChannelName.text = data:ChannelColorName()
	
	if(data:IsRecord()) then
		self.RecTran:SetObjActive(true)
		local rec_time = data:RecordTime()
		local t_min = math.floor(rec_time/60)
		local t_sec = rec_time%60
		if t_min > 0 then
			self.RecTimeTxt.text = string.format("%d'%d''",t_min,t_sec)
		else
			self.RecTimeTxt.text = string.format("%d''",t_sec)
		end
		self:FlushPlayingKey()
		self.RecTran.anchoredPosition = Vector2.New(self.Content.preferredWidth,0)
	else
		self.RecTran:SetObjActive(false)
	end
end

function MainViewChatRecordItem:OnClickRec()
	if ChatData.Instance:RecordData().cur_playing_key == self.data:RecordKey() then
		ChatData.Instance:SetCurPlayingRec("")
		VoiceRecorder.StopPlayRecord()
	else

		local play_suc = VoiceRecorder.PlayRecord(self.data:RecordKey(),
			function (com_code,rec_key)
				if com_code ~= GVoiceComCode.PlayFileDone then
					return
				end
				if self.data:RecordKey() == ChatData.Instance:RecordData().cur_playing_key then
					ChatData.Instance:SetCurPlayingRec("")
				end
		end)
		if play_suc == GVoiceErrorNo.Succ then
			ChatData.Instance:SetCurPlayingRec(self.data:RecordKey())
		end
	end
end

function MainViewChatRecordItem:FlushPlayingKey()
    if self:IsEnabled() == false or self.data == nil or nil == self.data.IsRecord or not self.data:IsRecord() then
        return
    end
   local cur_playing_key = ChatData.Instance:RecordData().cur_playing_key
    if cur_playing_key ==self.data:RecordKey() then
        self.RecIconAnim:Play("rec_icon_playing")
    else
        self.RecIconAnim:Play("rec_icon_idle")
    end
end

function MainViewChatRecordItem:OnDestroy()
    ChatData.Instance:RecordData():Uncare(self.care_handle)
    self.care_handle = nil
    if self:IsEnabled() then
        local cur_playing_key = ChatData.Instance:RecordData().cur_playing_key
        if cur_playing_key == self.data:RecordKey() then
            VoiceRecorder.StopPlayRecord()
            ChatData.Instance:SetCurPlayingRec("")
        end
    end
end

BtnInvitedItem = DeclareMono("BtnInvitedItem", UIWidgetBaseItem)

function BtnInvitedItem:BtnInvitedItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Team.InvitedShow
end

function BtnInvitedItem:OnDestroy()
	UIWidgetBaseItem.OnDestroy(self)
	if self.timer_tt_flush then
		TimeHelper:CancelTimer(self.timer_tt_flush)
		self.timer_tt_flush = nil
	end
end

function BtnInvitedItem:SetData(data)
	local count = 0
	local is_knightly = false
    for _, info in pairs(data) do
		count = count + 1
		if not is_knightly then
			is_knightly = info.add_chivalrous_value > 0
		end
	end
	self.ItemObj:SetActive(count > 0)
	self.IconSp.SpriteName = is_knightly and "ZuDuiXiaYi" or "ZuDui"
	self.Name.text = is_knightly and self.language.NameKnightly or self.language.NameTeam
	self.Num.text = count
	if 0 == count then
		ViewMgr:CloseView(TeamInvitedView)
		if self.timer_tt_flush then
			TimeHelper:CancelTimer(self.timer_tt_flush)
			self.timer_tt_flush = nil
		end
	elseif nil == self.timer_tt_flush then
		self.timer_tt_flush = TimeHelper:AddCountDownTT(function ()
		end, function ()
			TimeHelper:CancelTimer(self.timer_tt_flush)
			self.timer_tt_flush = nil
			self.team_data:TeamInvitedListFlush()
		end, 30, 1)
	end
end

function BtnInvitedItem:OnClickItem()
    ViewMgr:OpenView(TeamInvitedView)
end