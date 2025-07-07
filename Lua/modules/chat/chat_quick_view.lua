
ChatQuickView = ChatQuickView or DeclareView("ChatQuickView", ChatConfig.ResPath .. "chat_quick")
VIEW_DECLARE_MASK(ChatQuickView,ViewMask.BlockClose)

function ChatQuickView:ChatQuickView()
	self.data = ChatCtrl.Instance:Data()
end

function ChatQuickView:LoadCallback()
	local chat_quick_type_list = self.data:GetChatQuickTypeList()
	self.TypeList:SetData(chat_quick_type_list)
	self.TypeList:ClickItemData(chat_quick_type_list[1])
end

ChatQuickViewList = ChatQuickViewList or DeclareMono("ChatQuickViewList", UIWFlushPanel)

function ChatQuickViewList:ChatQuickViewList()
	self.data = ChatCtrl.Instance:Data()
	self.bag_data = BagCtrl.Instance:Data()
	self.pet_data = PetCtrl.Instance:Data()
	
	self.data_cares = {
		{data = self.bag_data.item_grid, func = self.ItemGrids},
		{data = self.pet_data.pet_list, func = self.PetList},
		{data = ShengQiData.Instance.sheng_qi_list, func = self.ShengQiList},
	}
end

function ChatQuickViewList:ItemGrids()
	self.data:ChatDataQuickItem()
end

function ChatQuickViewList:PetList()
	self.data:ChatDataQuickPet()
end

function ChatQuickViewList:ShengQiList()
	self.data:ChatDataQuickShengQi()
end

function ChatQuickViewList:OnClickType(type_data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	local type_seq = type_data.seq
	local quick_list = self.data:GetChatQuickListBySeq(type_seq)
	self.ChatListsToggle[type_seq].isOn = true
	self.ChatLists[type_seq]:SetData(quick_list)
end

ChatQuickViewTypeItem = DeclareMono("ChatQuickViewTypeItem", UIWidgetBaseItem)

function ChatQuickViewTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	for i = 1, self.TypeNames:Length() do
		self.TypeNames[i].text = data.send_content
	end
end

function ChatQuickViewTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function ChatQuickViewTypeItem:GetTypeData()
	return self.data
end

ChatQuickViewEmojItem = DeclareMono("ChatQuickViewEmojItem", UIWidgetBaseItem)

function ChatQuickViewEmojItem:ChatQuickViewEmojItem()
	self.chat_data = ChatCtrl.Instance:Data()
end

function ChatQuickViewEmojItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if data.is_dy then
		UISpriteAnimTool.AnimName(self.EmojAnim, data.res_id)
		UISpriteAnimTool.PicNum(self.EmojAnim, data.pic_num)
		UISpriteAnimTool.Speed(self.EmojAnim, data.speed)
		self.EmojAnim.enabled = true
	else
		self.EmojAnim.enabled = false
		self.EmojSp.SpriteName = data.res_id
	end
end

function ChatQuickViewEmojItem:OnClickEmoj()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	self.chat_data.chat_quick_input:Set(ChatItemEmoj.New(self.data))
	ViewMgr:CloseView(ChatQuickView)
end

ChatQuickViewPetItem = DeclareMono("ChatQuickViewPetItem", UIWidgetBaseItem)

function ChatQuickViewPetItem:ChatQuickViewPetItem()
	self.chat_data = ChatCtrl.Instance:Data()
end

function ChatQuickViewPetItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data:Name()
	self.Level1.text = "Lv." .. data:Level()
	if data.IsLianYao then
		UH.Color(self.Name, data:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
		if self.YaoObj then
			self.YaoObj:SetActive(data:IsLianYao())
		end
	end

	local pet_state = data:State()
	local show_zdx = false
	local state_sp = PPStateSp.zhan
	if PetState.Fight == pet_state then
		show_zdx = true
		state_sp = PPStateSp.zhan 
	end
	if PetState.StandBy == pet_state then
		show_zdx = true
		state_sp = PPStateSp.dai
	end
	if PetState.Train == pet_state then
		show_zdx = true
		state_sp = PPStateSp.xun
	end
	self.ZDX.SpriteName = state_sp
	self.ZDX:SetObjActive(show_zdx)
	self.LeiObj:SetActive(data:ArenaState())

    self.Cell:SetData(data)
    UH.SetText(self.Level2, data:Level())
    PetData.SetStrLevel(nil, self.StrLevel, data:StrLevel())
end

function ChatQuickViewPetItem:OnClickPet()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	if ViewMgr:IsOpen(ChatView) then
		ChatCtrl.Instance:SendChannelChatText(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)})
		ChatData.Instance:SendGrey()
	elseif ViewMgr:IsOpen(SocietyView) then
		local society_data = SocietyCtrl.Instance:Data()
		if SocietyConfig.FriendGroupType.group == society_data.friend_cur_sel_group.group_type then
			ChatCtrl.Instance:SendChannelChatText(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)}, ChatConfig.ChannelType.group, society_data.cur_sel_chat_group:GroupId())
		else
			society_data:AddChatInfo(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)})
    		ViewMgr:FlushView(SocietyView, "SocietyViewChat/SocietyViewChatRecordList")
		end
	else
		local society_data = SocietyCtrl.Instance:Data()
		local cur_sel = society_data.quick_chat_info.cur_sel
        if "string" == type(cur_sel) then
			ChatCtrl.Instance:SendChannelChatText(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)}, ChatConfig.ChannelType.group, SocietyData.GroupKey2Id(cur_sel))
		elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= society_data.chat_group_temp then
			ChatCtrl.Instance:SendChannelChatText(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)}, ChatConfig.ChannelType.group, society_data.chat_group_temp:GroupId())
		else
			society_data.cur_sel_role = society_data.friend_list[cur_sel]
			society_data:AddChatInfo(ChatItemPet.ItemIntro(self.data), {ChatItemPet.New(self.data)})
			society_data:FlushChatQuickRecords()
		end
	end
	ViewMgr:CloseView(ChatQuickView)
end


ChatRecordEidtView = ChatRecordEidtView or DeclareView("ChatRecordEidtView", ChatConfig.ResPath .. "record_edit")
VIEW_DECLARE_MASK(ChatRecordEidtView,ViewMask.BgBlockNoAnim)

function ChatRecordEidtView:ChatRecordEidtView()
	self.data = ChatCtrl.Instance:Data()
	self.world_num_limit = 15

	self.InputField.onValueChanged:AddListener(function ()
		local word_num = DataHelper.GetStringWordNum(self.InputField.text)
		if word_num > self.world_num_limit and word_num > self.word_num_per then
			PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		end
		self.word_num_per = word_num
		self.Num.text = string.format("%s/%s", word_num, self.world_num_limit)
	end)
end

function ChatRecordEidtView:LoadCallback(param_t)
	self.param_t = param_t
end

function ChatRecordEidtView:OnFlush()
	self.Num.text = "0/" .. self.world_num_limit
end

function ChatRecordEidtView:OnClickSave()
	if (self.word_num_per or 0) > self.world_num_limit then
		PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		return
	end
	if StringIsEmpty(self.InputField.text) then
		ViewMgr:CloseView(ChatRecordEidtView)
		return
	end
	local function callback(str)
		ChatCtrl.Instance:SendChatSimpleRecordSet(self.param_t.index, str)
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 2, callback)
	else
		callback(ChatFilter.Instance:Filter(self.InputField.text))
	end
	
	ViewMgr:CloseView(ChatRecordEidtView)
end

function ChatRecordEidtView:OnClickClose()
	ViewMgr:CloseView(ChatRecordEidtView)
end