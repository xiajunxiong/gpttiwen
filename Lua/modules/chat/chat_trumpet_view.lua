
ChatTrumpetView = ChatTrumpetView or DeclareView("ChatTrumpetView", ChatConfig.ResPath .. "chat_trumpet", Mod.ChatTrumpet.Main)

function ChatTrumpetView:ChatTrumpetView()
	self.data = ChatCtrl.Instance:Data()
	self.item_info_data = ItemInfoCtrl.Instance:Data()
	self.language = Language.Chat
	self.world_num_limit = self.data:GetChannelLimitNumByChannelId(ChatConfig.ChannelId.trumpet)

	self.InputField.onValueChanged:AddListener(function ()
		local word_num = DataHelper.GetStringWordNum(self.InputField.text)
		if word_num > self.world_num_limit and word_num > self.word_num_per then
			PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		end
		self.word_num_per = word_num
		self.Num.text = string.format("%s/%s", word_num, self.world_num_limit)
	end)
end

function ChatTrumpetView:OnFlush()
	local trumpet_item = self.item_info_data:GetItemData()
	if nil ~= trumpet_item then self.TrumpetCell:SetData(trumpet_item) end
	self.Num.text = "0/" .. self.world_num_limit
end

function ChatTrumpetView:OnClickClose()
	ViewMgr:CloseView(ChatTrumpetView)
end

function ChatTrumpetView:OnClickSend()
	if (self.word_num_per or 0) > self.world_num_limit then
		PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		return
	end
	if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevel() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevel()))
		return
	end

	local function callback(str)
		ChatCtrl.Instance:SendSpeaker(str)
		ViewMgr:CloseView(ChatTrumpetView)
		ViewMgr:CloseView(BagView)
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 2, callback)
	else
		callback(ChatFilter.Instance:Filter(self.InputField.text))
	end
end