
ChatView = ChatView or DeclareView("ChatView", ChatConfig.ResPath .. "chat",Mod.Chat.Main)
VIEW_DECLARE_LEVEL(ChatView,ViewLevel.L1)
VIEW_DECLARE_MASK(ChatView,ViewMask.BlockClose)

function ChatView:ChatView()
	self.data = ChatCtrl.Instance:Data()
	self.language = Language.Chat
end

function ChatView:LoadCallback()
	if not MainViewData.Instance:IsBattle() then
		ViewMgr:MainViewVisible(false)
	end
	LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.TeamChannel)
	if IS_AUDIT_VERSION then
		self.ChannelListObj:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
		self.AuditChannelListObj:SetActive(not GLOBAL_CONFIG:PkgInfo().is_youyan)
		self.ArrowObj:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
		if not GLOBAL_CONFIG:PkgInfo().is_youyan then
			UH.SpriteName(self.AuditArrowImg, "ZhanKai_"..(AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1))
		end
	else
		self.ChannelListObj:SetActive(true)
		self.AuditChannelListObj:SetActive(false)
		self.ArrowObj:SetActive(true)
	end
end

function ChatView:OnClickClose()
	ViewMgr:CloseView(ChatView)
end

function ChatView:CloseCallback()
	if SceneModeType.AnimalRace ~= SceneData.Instance:GetMode() and not ViewMgr:IsOpen(FishView) and not ViewMgr:IsOpen(LandsBetweenMainView) then
		ViewMgr:MainViewVisible(true)
	end
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.TeamChannel)
end

ChatViewDown = ChatViewDown or DeclareMono("ChatViewDown", UIWFlushPanel)

function ChatViewDown:ChatViewDown()
	self.data = ChatCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.language = Language.Chat

	self.data_cares = {
		{data = self.data.chat_quick_input, func = self.FlushInputField, init_call = false},
		{data = self.data.cur_sel_channel, func = self.FlushSendTimer},
		{data = self.data.send_grey, func = self.FlushSendTimer, init_call = false},
		{data = self.data.copy_info, func = self.FlushCopy, init_call = false},
	}

	self.chat_helper = ChatHelper.New{
        input_field = self.InputField,
        limit_num = self.data.cur_sel_channel:LimitNum()
    }
end

function ChatViewDown:Awake()
	UIWFlushPanel.Awake(self)
	self.BtnVoiceInter:SetObjActive(GLOBAL_CONFIG:FlagsVoice())
	
	if IS_AUDIT_VERSION then
		for i = 1, self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
		for i = 1, self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(not GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
		if not GLOBAL_CONFIG:PkgInfo().is_youyan then
			local audit_seq = AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
			UH.SpriteName(self.AuditSendBtnImg, "SendBtn_"..audit_seq)
			UH.SpriteName(self.AuditEmojImg, "BiaoQing_"..audit_seq)
			UH.SpriteName(self.AuditRecordImg, "QiPaoTuBiao_"..audit_seq)
		end
	else
		for i = 1, self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(true)
		end
		for i = 1, self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(false)
		end
	end
end

function ChatViewDown:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_tt_send)

	self.data.copy_info:Set{is_show = false}
	self.data.chat_copy_pause = false
end

function ChatViewDown:FlushInputField()
	self.chat_helper:AddChatItem(self.data.chat_quick_input:Val())
end

function ChatViewDown:OnDownVoice()
	local remind_time = self.data.cur_sel_channel:RemindTime()
	if remind_time > 0 then return end
	self.data:VoiceStart()
end

function ChatViewDown:OnUpVoice()
	self.data:VoiceFinish(function (rec_key,time) 
		ChatCtrl.Instance:SendChannelVoice(rec_key,time)
		ChatData.Instance:SendGrey()
	end)
end

function ChatViewDown:OnDragVoice()
	self.data:VoiceCheck()
end

function ChatViewDown:OnClickSimpleRecord()
	self.SimpleRecordObj:SetActive(not self.SimpleRecordObj.activeSelf)
end

function ChatViewDown:OnClickEmoj()
	ViewMgr:OpenView(ChatQuickView)
end

function ChatViewDown:OnClickSend()
	if self.chat_helper:IsLimitNum() then return end
	local str = self.chat_helper:RealText()
	if StringIsEmpty(str) then
		PublicPopupCtrl.Instance:Center(Language.Society.ChatIllegalTips.empty)
		return
	end
	if self:CheckGM(str) then return end
	if not self.chat_helper:Switch() and self.data:ChatCacheCheck(str, self.data.cur_sel_channel:ChannelId()) then
		return
	end

	local function callback(str)
		self.data.quick_last_chat = self.chat_helper:CopyInfo()
		ChatCtrl.Instance:SendChannelChatText(str, self.chat_helper:ItemList())
		self.chat_helper:Empty()
		ChatData.Instance:SendGrey()
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 2,callback)
	else
		callback(ChatFilter.Instance:Filter(str))
	end
end

function ChatViewDown:CheckGM(str)
	if string.find(str, "/gm") then
		local param_t = string.split(str, " ")
		local str = ""
		for i = 3, #param_t do
			str = str .. param_t[i] .. " "
		end
		if param_t[2] == "runspeed" then
			BehaviorGM.ClientSpeedRun(tonumber(str))
		else
			GMCmdCtrl.Instance:SendGMCommand(param_t[2], str)
		end
		self.InputField.text = ""
		return true
	end
end

function ChatViewDown:FlushSendTimer()
	TimeHelper:CancelTimer(self.timer_tt_send)
	local remind_time = self.data.cur_sel_channel:RemindTime()
	if remind_time > 0 then
		self.timer_tt_send = TimeHelper:AddCountDownTT(function ()
			self.BtnSendInter.Interactable = false
			self.BtnVoiceInter.Interactable = false
			self.BtnSimpleRecordInter.Interactable = false
			self.BtnEmojInter.Interactable = false
			self.BtnSend.text = string.format(self.language.SendRemind, self.data.cur_sel_channel:RemindTime())
			UH.SetText(self.AuditBtnSend, string.format(self.language.SendRemind, self.data.cur_sel_channel:RemindTime()))
		end, function ()
			self.BtnSend.text = self.language.Send
			UH.SetText(self.AuditBtnSend, self.language.Send)
			self.BtnSendInter.Interactable = true
			self.BtnVoiceInter.Interactable = true
			self.BtnSimpleRecordInter.Interactable = true
			self.BtnEmojInter.Interactable = true
		end, self.data.cur_sel_channel:RemindTime())
	else
		self.BtnSend.text = self.language.Send
		UH.SetText(self.AuditBtnSend, self.language.Send)
		self.BtnSendInter.Interactable = true
		self.BtnVoiceInter.Interactable = true
		self.BtnSimpleRecordInter.Interactable = true
		self.BtnEmojInter.Interactable = true
	end
end

function ChatViewDown:FlushCopy()
	local ci = self.data.copy_info
	self.CopyObj:SetActive(ci.is_show)
	if ci.is_show then
		self.CopyTarget:SetTarget(ci.target)
		self.CopyReport:SetActive(not ci.chat_info:IsSelf())
	end
end

function ChatViewDown:OnCloseCopy()
	self.data.copy_info:Set{is_show = false}
	self.data.chat_copy_pause = false
end

function ChatViewDown:OnClickCopy()
	self.chat_helper:CopyContent(self.data.copy_info.chat_info.info.content)
	self:OnCloseCopy()
end

function ChatViewDown:OnClickReport()
	local ci = self.data.copy_info.chat_info
	self:OnCloseCopy()
	if nil == ci then return end 
	self.society_data:CurSelRole({name = ci.info.name, uid = ci.info.from_uid})
	ViewMgr:OpenView(SocietyViewReport)
end

ChatViewChannelList = ChatViewChannelList or DeclareMono("ChatViewChannelList", UIWFlushPanel)

function ChatViewChannelList:ChatViewChannelList()
	self.data = ChatCtrl.Instance:Data()
end

function ChatViewChannelList:Awake()
	local chat_channel_list = self.data:GetChatChannelListShow()
	self.ChannelList:SetData(chat_channel_list)
	self.ChannelList:ClickItemData(self.data.cur_sel_channel:Val())
	self.BtnScroll.isOn = true
	self.BtnVoice.isOn = false
	self:OnClickScroll()
	self:OnClickVoice()
end

function ChatViewChannelList:OnClickScroll()
	local flag = self.BtnScroll.isOn
	self.ScrollT.localPosition = flag and Vector2.New(20, 0) or Vector2.New(-20, 0)
end

function ChatViewChannelList:OnClickVoice()
	local flag = self.BtnVoice.isOn
	self.VoiceT.localPosition = flag and Vector2.New(20, 0) or Vector2.New(-20, 0)
end

ChatViewChannelItem = DeclareMono("ChatViewChannelItem", UIWidgetBaseItem)

function ChatViewChannelItem:ChatViewChannelItem()
	self.chat_data = ChatCtrl.Instance:Data()
end

function ChatViewChannelItem:Awake()
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		if self.NorImg then
			local audit_seq = AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
			UH.SpriteName(self.NorImg, "RenWuAnNiu_"..audit_seq)
			UH.SpriteName(self.SelImg, "RenWuAnNiuXuanZhong_"..audit_seq)
			UH.Color(self.ChannelNames[1], ChatData.AuditViewChannelTextColor[audit_seq][1])
			UH.Color(self.ChannelNames[2], ChatData.AuditViewChannelTextColor[audit_seq][2])
		end
	end
end

function ChatViewChannelItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	for i = 1, self.ChannelNames:Length() do
		self.ChannelNames[i].text = data:ChannelNameShow()
	end
end

function ChatViewChannelItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function ChatViewChannelItem:OnClickChannel()
	self.chat_data.cur_sel_channel:Set(self.data:Val())
end

ChatViewRecordList = ChatViewRecordList or DeclareMono("ChatViewRecordList", UIWFlushPanel)

function ChatViewRecordList:ChatViewRecordList()
	self.data = ChatCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Chat

	self.data_cares = {
		{data = self.data:GetChatChannelList(), func = self.FlushRecordList},
		{data = self.data.cur_sel_channel, func = self.FlushRecordList},
		{data = self.data:GetTeamChannelList(), func = self.FlushRecruitList},
		{data = self.data.world_question_info, func = self.FlushWorldQuestion},
		{data = self.data.cur_sel_channel, func = self.FlushWorldQuestion},
		{data = self.data.cur_sel_channel, func = self.ResetNewMsg},
		{data = self.data.new_msg, func = self.FlushNewMsg},
		-- {data = self.data.cur_sel_channel, func = self.FlushAutoSize},
		{data = self.data.simple_records, func = self.FlushSimpleRecords},
		{data = self.data.simple_records_switch, func = self.CloseSimpleRecords, init_call = false},
		{data = GuildBuildingData.Instance.info_data, func = self.FlushGuildBuilding, keys = {"speed_info", "finish_time", "open_info"}},
		{data = self.data.cur_sel_channel, func = self.FlushGuildBuilding},
	}

	local chat_channel_list = self.data:GetChatChannelList()
	self.data.cur_sel_channel:Set(self.data:GetSelChannelList() or chat_channel_list[next(chat_channel_list)])

	self.ScrollRect.onValueChanged:AddListener(BindTool.Bind(self.OnDragRecordList, self))
	self.no_recruit = false
	self.no_cross = false
	self.auto_buttom = false

	-- 这个是原本的top，不能改
	self.normal_top = 10
	-- 这个可以根据自己需要的top来改
	self.change_top = 10
	if not GLOBAL_CONFIG:Flags().block_antifraud then
		RichTextHelper.Bind(self.AntiFraudTips, BindTool.Bind(RichTextItem.OnClickLink, self))
		self.AntiFraudTips.text = self.language.AntiFraudTips.tips1 .. RichTextHelper.LinkStr(self.language.AntiFraudTips.tips2, "_ul", COLORSTR.Green7, "modKey", Mod.Reward.AntiFraud)
	end
end

function ChatViewRecordList:FlushRecordList()
	local channel_id = self.data.cur_sel_channel:ChannelId()
	if not self:FlushRecruitList() then
		local in_team = self.team_data:InTeam()
		local chat_list = self.data:GetChannelChatListByChannelId(channel_id, self.no_recruit, self.no_cross)
		self.RecordListObj:SetActive(true)
		self.RecruitListObj:SetActive(false)
		local pos_num = self.ScrollRect.verticalNormalizedPosition
		self.auto_scroll = pos_num < 0.01 or (self.ScrollRect.content.sizeDelta.y < self.ScrollRectPos.rect.height)
		self.auto_scroll = self.auto_scroll and (not self.data.chat_copy_pause)
		self.new_msg = true
		self.RecordList:SetDataList(chat_list, #chat_list)
		if self.auto_scroll or self.auto_buttom then
			self.auto_buttom = false
			self.RecordList:JumpToDown()
		else
			self.data.new_msg.num = self.data.new_msg.num + 1
			self.NewMsgPos:SetObjActive(true)
		end
	end

	local value = self.data:CheckDownState()
	self.DownNorObj:SetActive(value)
	self.DownNoObj:SetActive(not value)
	local no_desc = self.language.NoJoinDesc[channel_id]
	local no_desc_btn = self.language.NoJoinBtn[channel_id]
	if no_desc then
		if ChatConfig.ChannelId.trumpet == channel_id then
			local is_enough = self.data.trumpet_ic:Num() > 0 or self.data.trumpet_nc:Num() > 0
			self.DownNoDesc.text = is_enough and no_desc.bag or no_desc.mall
		else
			self.DownNoDesc.text = string.format(no_desc, self.data.cur_sel_channel:ChannelName())
		end
	end
	self.BtnNoObj:SetActive(nil ~= no_desc_btn)
	if nil ~= no_desc_btn then
		if ChatConfig.ChannelId.trumpet == channel_id then
			local is_enough = self.data.trumpet_ic:Num() > 0 or self.data.trumpet_nc:Num() > 0
			self.BtnNo.text = is_enough and no_desc_btn.bag or no_desc_btn.mall
		else
			self.BtnNo.text = no_desc_btn
		end
	end

	self.data:EmptyChatUnread(channel_id)
end

-- 刷新家族加速建造
function ChatViewRecordList:FlushGuildBuilding()
	self:FlushAutoSize()
	local builidng_info = GuildBuildingData.Instance.info_data.speed_info
	local open_info = GuildBuildingData.Instance.info_data.open_info
	self.SpeedTimer:CloseCountDownTime()
	TimeHelper:CancelTimer(self.speed_up_handler)
	local now_building_type, finish_time = GuildBuildingData.Instance:NowBuildingData()
	if ChatConfig.ChannelId.guild == self.data.cur_sel_channel:ChannelId() and GuildBuildingData.Instance:IsSpeedUping() and now_building_type > 0 then
		self.ObjGuildSpeedUp:SetActive(true)
		local building_cfg = GuildBuildingData.Instance:BuildingConfig(now_building_type)
		self.SpeedTimer:StampTime(finish_time, TimeType.Type_Time_0, Language.GuildBuilding.AtLeastSpeedUp)
			self.speed_up_handler = TimeHelper:AddCountDownTT(function()
				local now_time = TimeHelper.GetServerTime()
				local need_time = finish_time - now_time
				self.BuildProgress:SetProgress(1 - need_time / building_cfg.up_expend_time)
			end,
			function()
				self.BuildProgress:SetProgress(1)
			end,
			finish_time,
			1)
		if open_info.open_post >= 0 then
			local post_name = GuildData.Instance:GetJobName(open_info.open_post)
			local building_name = GuildBuildingData.Instance:BuildingName(now_building_type)
			UH.SetText(self.TxtSpeedTip, Format(Language.GuildBuilding.StartSpeedUp, post_name, open_info.open_name, building_name))
		end
	else
		self.ObjGuildSpeedUp:SetActive(false)
		self.BuildProgress:SetProgress(0)
	end
	self.IHBtnSpeed.Interactable = GuildBuildingData.Instance:TodayCanSpeedTimesPerson() > 0
	UH.SetText(self.TxtConsume, GuildBuildingData.Instance:SpeedUpCs())
	UH.SetText(self.TxtSpeedTime, Format(Language.GuildBuilding.SpeedUpPersonTime, GuildBuildingData.Instance:TodayCanSpeedTimesPerson()))
end

function ChatViewRecordList:OnSpeedUpClick()
	if GuildBuildingData.Instance:TodayCanSpeedTimesPerson() == 0 then
		PublicPopupCtrl.Instance:Center(Language.GuildBuilding.SpeededTip)
		return
	end
	local consume = GuildBuildingData.Instance:SpeedUpCs()
	if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, consume) then
		GuildBuildingCtrl.Instance:SendGuildBuildingReq(0, GuildBuildingReqType.SpeedUp)
	end
end

function ChatViewRecordList:FlushRecruitList()
	local in_team = self.team_data:InTeam()
	local channel_id = self.data.cur_sel_channel:ChannelId()
	if ChatConfig.ChannelId.team == channel_id and not in_team then
		local team_channel = self.data:GetTeamChannelList()
		self.RecordListObj:SetActive(false)
		self.RecruitListObj:SetActive(true)
		self.RecruitList:SetDataList(team_channel)
		return true
	end
	return false
end

function ChatViewRecordList:OnClickNo()
	local channel_id = self.data.cur_sel_channel:ChannelId()
	if ChatConfig.ChannelId.guild == channel_id then
		ViewMgr:OpenView(GuildList)
	elseif ChatConfig.ChannelId.trumpet == channel_id then
		local is_enough = self.data.trumpet_ic:Num() > 0 or self.data.trumpet_nc:Num() > 0
		if is_enough then
			ViewMgr:OpenView(BagView)
		else
			MallCtrl.Instance:Jump(self.data.trumpet_ic.item_id)
		end
	end
	ViewMgr:CloseView(ChatView)
end

function ChatViewRecordList:FlushWorldQuestion()
	self:ResetNewMsg()
	self:FlushAutoSize()
	local info = self.data.world_question_info
	local pre_info = info.pre_info
	local value = (ChatConfig.ChannelId.world == self.data.cur_sel_channel:ChannelId() and RoleData.Instance:GetRoleLevel() >= GLOBAL_CONFIG:TalkLevel()) and (info.now_answer_subject > 0 and info.answer_tamp > 0)
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
	if FishData.Instance:InFishMap() then
	   value = false
	end
	self.WorldQuestionObj:SetActive(value)
	TimeHelper:CancelTimer(self.timer_world_question)
	if not value then return end
	local is_tr = 1 == self.data.world_question_ret
	local role_answer_id = is_tr and self.data.role_answer_id or 1
	local co_q, co_a = ChatData.Instance:WorldQuestionCoById(info.now_answer_subject)
	self.WorldQuestionDesc.text = co_q.question
	for i = 1, self.WorldQuestionAnswers:Length() do
		self.WorldQuestionAnswers[i]:SetData(co_a, i, self.data.role_answer_id)
	end
	local tamp = info.answer_tamp
	self.timer_world_question = TimeHelper:AddCountDownCT(function ()
	   self.WorldQuestionTime.text = string.format("%ss", math.floor(tamp - TimeHelper.GetServerTime()))
	end, function ()
	   self.WorldQuestionTime.text = string.format("%ss", 0)
	end,
	tamp, 1)
 end
function ChatViewRecordList:OnDragRecordList()
	if self.new_msg and not self.auto_scroll and self.ScrollRect.verticalNormalizedPosition < 0.01 and self.data.new_msg.num > 0 then
		self.new_msg = false
		self:ResetNewMsg()
	end
end

function ChatViewRecordList:FlushNewMsg()
	if not self.auto_scroll then
		local channel_id = self.data.cur_sel_channel:ChannelId()
		-- local num = ChatConfig.ChannelId.system == channel_id and self.data.new_msg.num or ((self.data.new_msg.num + 1) / 2)
		local num = self.data.new_msg.num
		self.NewMsgNum.text = 0 == self.data.new_msg.num and "" or string.format(ErrorText[121], num)
	end
end

function ChatViewRecordList:FlushAutoSize()
	local bottom = 20
	local world_question_info = self.data.world_question_info
	local channel_id = self.data.cur_sel_channel:ChannelId()
	local show_anti_fraud = ChatConfig.ChannelId.system ~= channel_id and FunOpen.Instance:GetFunIsOpened(Mod.RewardOpen.Main) and not GLOBAL_CONFIG:Flags().block_antifraud --后台配置控制是否屏蔽防诈提示
	local show_world_question = (ChatConfig.ChannelId.world == self.data.cur_sel_channel:ChannelId() and RoleData.Instance:GetRoleLevel() >= GLOBAL_CONFIG:TalkLevel()) and (world_question_info.now_answer_subject > 0 and world_question_info.answer_tamp > 0)
	if show_world_question then
		for k,v in pairs(ChatConfig.HideQuestionView) do
		   if ViewMgr:IsOpen(ModToView[Mod.ParseKey(v)]) then
				show_world_question = false
			  break
		   end
		end
		if ViewMgr:IsOpen(EndlessTowerView) then
			show_world_question = false
		end
		if FishData.Instance:InFishMap() then
			show_world_question = false
		end
	end
	if ChatConfig.ChannelId.general == channel_id then
		self.change_top = 61
	elseif ChatConfig.ChannelId.guild == channel_id and GuildBuildingData.Instance:IsSpeedUping() then
		self.change_top = 200
	else
		self.change_top = self.normal_top
	end
	if show_world_question then
		bottom = 232
	elseif show_anti_fraud then
		bottom = 85
	end
	self.RecordListPos.offsetMax = Vector2.New(0, -self.change_top)
	self.RecordListPos.offsetMin = Vector2.New(0, bottom)
	self.RecruitListPos.offsetMin = Vector2.New(0, bottom)

	self.NewMsgPos.anchoredPosition = Vector2.New(0, show_anti_fraud and 110 or 31.5)
	self.AllOptionsObj:SetActive(ChatConfig.ChannelId.general == channel_id)
	self.AntiFraudTipsObj:SetActive(show_anti_fraud)

	self:CloseSimpleRecords()
end

function ChatViewRecordList:ResetNewMsg()
	self.NewMsgPos:SetObjActive(false)
	self.data.new_msg.num = 0
	self.RecordList:JumpToDown()
end

function ChatViewRecordList:FlushSimpleRecords()
	self.SimpleRecords:SetData(self.data.simple_records:Val())
end

function ChatViewRecordList:CloseSimpleRecords()
	self.SimpleRecords:SetObjActive(false)
end

function ChatViewRecordList:OnClickSwitchRecruit(value)
	self.no_recruit = value
	self.auto_buttom = true
	self:FlushRecordList()
end

function ChatViewRecordList:OnClickSwitchCross(value)
	self.no_cross = value
	self.auto_buttom = true
	self:FlushRecordList()
end

function ChatViewRecordList:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_world_question)
	TimeHelper:CancelTimer(self.speed_up_handler)
end

ChatViewRecordItem = DeclareMono("ChatViewRecordItem", RichTextItem)
--RecItem=====UIWSubstitute(ChatRecordItem)
--RecExpTxt===Text 用来计算转化文字之后 的item高度
function ChatViewRecordItem:ChatViewRecordItem()
	self.chat_data = ChatCtrl.Instance:Data()
	RichTextHelper.Bind(self.Content, BindTool.Bind(self.OnClickLink, self))
end

function ChatViewRecordItem:ExpendSize(data, axis)
	if data:IsRecord() then
		local he = ChatRecordItem.ItemHeight(data,self.RecExpTxt)
		-- LogError("data===",data:IsRecord(),"|",he,"|",data)
		return he
	else
		return data:HeightChatView(self.Content)
	end
    -- return  data:IsRecord() and self.RecItem:ItemHeight() or data:HeightChatView(self.Content)
end

function ChatViewRecordItem:SetData(data)
	if nil == data or nil == data.info then return end
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local content = data:GetContent()
	local fly_flag = info.fly_flag or 0
	self.is_item = data:IsItem()
	self.is_pet = data:IsPet()
	self.is_sheng_qi = data:IsShengQi()
	self.is_guild_task = data:IsGuildTask()
	self.is_warn = info.warn or false
	local can_copy = not data:IsSystem() and not self.is_item and not self.is_guild_task and (nil == string.find(content, "(<a.-</a>)"))
	self.CopyObj:SetActive(can_copy)

	if not self.is_item then RichTextMgr.CalcPreferredHeight(self.Content, content, data:IsSystem() and 410 or 310) end
	
	if not self.is_pet and not data:IsHongbao() and not data:IsHongBaoGet() and not self.is_sheng_qi then		--宠物和红包都不会打开Content对象，故不用设置text
		self.Content.text = content
	end
	-- self.Content.text = (self.is_pet or data:IsHongbao()) and content.name or content
	if info.name then
		local cur_channel_id = self.chat_data.cur_sel_channel:ChannelId()
		if self.data:IsSelf() then
			self.Name.text = (ChatConfig.ChannelId.general == cur_channel_id and data:ChannelColorName() or "") .. info.name
		else
			self.Name.text = info.name .. (ChatConfig.ChannelId.general == cur_channel_id and data:ChannelColorName() or "")
		end
		self.NameFText.text = info.name
		self.NameFChannel.text = ChatConfig.ChannelId.general == cur_channel_id and data:ChannelColorName() or ""
		self.NamePos:SetObjActive(0 == fly_flag)
		self.NameFPos:SetObjActive(fly_flag > 0)
		UH.SpriteName(self.NameF.BgSet, UIWFlyBg.RoleFlyS[fly_flag])
		self.NameFText.transform.localScale = Vector3.New(self.data:IsSelf() and -1 or 1, 1, 1)
		self.NameFChannel.transform.localScale = Vector3.New(self.data:IsSelf() and -1 or 1, 1, 1)
	end
	if not data:BlockRoleAvatar() then
		UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
		
		if info.top_level and info.top_level > 0 then
			self.PeakObj:SetActive(info.top_level and info.top_level > 0)
			UH.SetText(self.Level, info.top_level)
			UH.Color(self.Level, COLORS.Red8)
		else
			UH.SetText(self.Level, info.level)
			UH.Color(self.Level, COLORS.Yellow16)
			self.PeakObj:SetActive(false)
		end
	end
	self.ChannelName.text = data:GetShowChannelName() and data:ChannelColorName() or ""
	if not self.is_item then self.Content.text = content end
	if self.is_pet then self.PetItem:SetData(content, info.item_list[1]) end
	if self.is_guild_task then self.GuildTaskItem:SetData(data) end
	if self.data:IsRecord() then self.RecItem:SetData(self.data:RecordKey(),self.data:RecordTime(),self.data:IsSelf()) end
	if self.data:IsHongbao() then self.HongBaoItem1:SetData(content,data:IsSelf()) end
	if self.data:IsHongBaoGet() then self.HongBaoGetItem2:SetData(content) end

	self.ShengQiInfo:SetObjActive(self.is_sheng_qi)
	if self.is_sheng_qi then self.ShengQiInfo:SetData(content,info.item_list[1]) end
	self:LayoutAdapt()
end

function ChatViewRecordItem:LayoutAdapt()
	local is_system = self.data:IsSystem()
	local is_self = self.data:IsSelf()
	local bubble_id = self.data:BubbleId()
	local info = self.data.info
	local fly_flag = info.fly_flag or 0
	self.ItemLayout.padding.left = (is_self or is_system or self.data:BlockRoleAvatar()) and 0 or (0 == bubble_id and 100 or 120)
	self.ItemLayout.padding.right = is_self and 100 or 0
	self.ItemLayout.padding.top = (is_system or self.data:BlockRoleAvatar()) and 0 or (fly_flag > 0 and 40 or 35)
	self.BgImage.enabled = not is_system and 0 == bubble_id
	self.BgLayout.padding.top = is_system and 5 or 15
	self.BgLayout.padding.bottom = is_system and 10 or 20
	self.BgLayout.padding.left = is_self and (0 == bubble_id and 15 or 35) or 25
	-- self.BgLayout.padding.right = is_self and 25 or (0 == bubble_id and 15 or 35)
	-- self.BgLayout.padding.left = is_self and 15 or 25
	self.BgLayout.padding.right = is_self and 25 or 15
	self.GoObj:SetSiblingIndex(is_self and self.BgObj:GetSiblingIndex() - 1 or self.BgObj:GetSiblingIndex() + 1)
	self.UpPos:SetObjActive(not self.data:BlockRoleAvatar())
	self.GuildTaskItem.gameObject:SetActive(self.is_guild_task)
	self.WarnObj:SetActive(self.is_warn)

	self.PetItemObj:SetActive(self.is_pet)
	self.RecItem.gameObject:SetActive(self.data:IsRecord())
	self.HongBaoItem1.gameObject:SetActive(self.data:IsHongbao())
	self.HongBaoGetItem2.gameObject:SetActive(self.data:IsHongBaoGet())
	self.GoObj:SetActive(not self.is_item and (not self.is_guild_task))
	self.BgObj:SetActive(not self.is_item and (not self.is_guild_task))
	self.BubbleSp:SetObjActive(not is_system and bubble_id > 0)
	if is_system or self.data:BlockRoleAvatar() then return end
	if 0 == bubble_id then self.BgSp.SpriteName = is_self and "XiaoXi_ZiJi" or "XiaoXi_DuiFang" end
	self.UpPos.anchoredPosition = Vector2(is_self and 183 or -183, -47)
	self.DetailPos.anchoredPosition = Vector2(is_self and -55 or 55, 30)
	self.NamePos.pivot = Vector2.New(is_self and 1 or 0, 0.5)
	self.NameFPos.localScale = Vector3.New(is_self and -1 or 1, 1, 1)
	self:BubbleShow(is_system, bubble_id, is_self)
end

function ChatViewRecordItem:BubbleShow(is_system, bubble_id, is_self)
	if is_system or 0 == bubble_id then return end
	local bubble_co = ChatData.Instance:GetBubbleCo(bubble_id)
	for i = 1, self.BubbleDecos:Length() do
		local pos_t = bubble_co[i]
		self.BubbleDecos[i]:SetObjActive(nil ~= pos_t)
		if pos_t then
			UH.SetIcon(self.BubbleDecos[i], string.format(ChatConfig.BubbleDecoTag, ChatConfig.BubbleDeco[i], bubble_id), ICON_TYPE.BUBBLE)
			self.BubbleDecoPs[i].anchoredPosition = Vector2.New(pos_t.x, pos_t.y)
		end
	end
	self.BubblePos.rotation = Vector3.New(0, is_self and 0 or 180, 0)
	self.BubblePos.offsetMin = Vector2.New(is_self and 0 or -20, 0)
	-- self.BubblePos.offsetMax = Vector2.New(is_self and -20 or 0, 0)
	UH.SetIcon(self.BubbleSp, string.format(ChatConfig.BubbleTag, bubble_id), ICON_TYPE.BUBBLE)
end

function ChatViewRecordItem:OnClickRole()
	if self.data:IsSelf() then return end
	local vo = self.data.info
	SocietyCtrl.Instance:ClickToRoleVo{
        role_id = vo.from_uid,
        name = vo.name,
        level = vo.level,
		guild_name = vo.family,
		profession = vo.prof,
		appearance = {
			prof_base = RoleData.ProfIdToProfType(vo.prof),
			avatar_type = vo.avatar_type,
			avatar_id = vo.avatar_id,
			avatar_quality = vo.avatar_quality
		},
		pingbi_pk = true,
		top_level = vo.top_level,
    }
end

function ChatViewRecordItem:OnPressDownCopy()
	self.chat_data:CopyStart(self.ItemPos, self.data)
end

function ChatViewRecordItem:OnPressUpCopy()
	self.chat_data:CopyFinish()
end

ChatViewRecruitItem = DeclareMono("ChatViewRecruitItem", UIWidgetBaseItem)

function ChatViewRecruitItem:ChatViewRecruitItem()
	self.language = Language.Chat.Recruit
end

function ChatViewRecruitItem:SetData(data)
	if nil == data then return end
	UIWidgetBaseItem.SetData(self, data)
	local name = data.member_list[1].name or ""
	self.IsCrossObj:SetActive(TeamConfig.CrossType.is_cross == data.is_cross)
	self.Target.text = RichTextHelper.ColorStr(data.target.desc, COLORSTR.Green8) .. RichTextHelper.ColorStr(string.format(self.language.level, data.limit_level_low, data.limit_level_high), COLORSTR.Yellow15)
	self.Level.text = data.role_level .. RichTextHelper.ColorStr(self.language.ji, COLORSTR.Yellow15)
	self.Time.text = TimeHelper.Ago(data.create_team_timestamp)
	self.Name.text = name
	self.NumProgress:SetProgress(data.member_count / TeamConfig.TEAM_MEMBER_MAX)
	-- self.NumProgress:SetText(string.format("%s %s/%s", RichTextHelper.ColorStr(name, COLORSTR.Yellow15), data.member_count, TeamConfig.TEAM_MEMBER_MAX))
	self.NumProgress:SetText(string.format("%s/%s", data.member_count, TeamConfig.TEAM_MEMBER_MAX))
end

function ChatViewRecruitItem:OnClickJoin()
	TeamCtrl.Instance:SendJoinTeam(self.data.team_index, self.data.is_cross)
end

ChatWorldQuestionAnswerItem = DeclareMono("ChatWorldQuestionAnswerItem", UIWidgetBaseItem)

function ChatWorldQuestionAnswerItem:ChatWorldQuestionAnswerItem()
	self.language = Language.Chat.WorldQuestion
end

function ChatWorldQuestionAnswerItem:SetData(data, index, role_answer_id)
	self.data = data
	self.index = index
	self.role_answer_id = role_answer_id

	self.AnswerShow.text = data["answer" .. index]
	local show_gou, show_cha = false, false
	if role_answer_id > 0 then
		show_gou = self.index == data.answer_true
		show_cha = self.index == role_answer_id and not show_gou
	end
	self.GouObj:SetActive(show_gou)
	self.ChaObj:SetActive(show_cha)
	self.SelObj:SetActive(self.index == role_answer_id)
end

function ChatWorldQuestionAnswerItem:OnClickItem()
	if self.role_answer_id > 0 then
		PublicPopupCtrl.Instance:Center(self.language.answered)
		return
	end
	ChatCtrl.Instance:SendChannelChatText(self.data["answer" .. self.index])
	ChatData.Instance:SendGrey()
end