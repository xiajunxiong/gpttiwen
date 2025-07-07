
SocietySendView = SocietySendView or DeclareView("SocietySendView", SocietyConfig.ResPath .. "society_send", Mod.Society.Send)

function SocietySendView:SocietySendView()
	self.language = Language.Society
end

function SocietySendView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), self.language.SocietySend, Vector2.New(942, 550))
end

SocietySendViewF = SocietySendViewF or DeclareMono("SocietySendViewF", UIWFlushPanel)

function SocietySendViewF:SocietySendViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.bag_data = BagCtrl.Instance:Data()
	self.language = Language.Society.Share

	self.data_cares = {
		{data = self.data.send_info_sm, func = self.FlushSendNum, keys = {"send_num"}, init_call = false},
		{data = self.data.send_info_sm, func = self.FlushItemId, keys = {"sel_item"}, init_call = false},
		{data = self.data.send_info_sm, func = self.FlushTimes, keys = {"sel_role"}, init_call = false},
		{data = self.data.send_info_sm, func = self.FlushType, keys = {"type"}},
		{data = self.data.send_info_sm, func = self.FlushTimes, keys = {"role_infos"}},
		{data = self.bag_data.item_grid, func = self.FlushItems, init_call = false},
	}

	self.world_num_limit = 13

	local value_change_func = function ()
		local word_num = DataHelper.GetStringWordNum(self.InputField.text)
		if word_num > self.world_num_limit and word_num > self.word_num_per then
			PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		end
		self.word_num_per = word_num
		self.WordNum.text = string.format(word_num > self.world_num_limit and self.language.MsgWordNumError or self.language.MsgWordNum, word_num, self.world_num_limit)
	end

	Call(value_change_func)
	self.InputField.onValueChanged:AddListener(value_change_func)

	self.DownCurNum.onValueChanged:AddListener(function ()
		local num = tonumber(self.DownCurNum.text) or 0
		if num > self.down_max_num then
			num = math.min(num, self.down_max_num)
			if SocietyConfig.GiftItemTypeShow.flower2 == self.data:GetSendItemTypeShow(self.data.send_info_sm.sel_item) then
				PublicPopupCtrl.Instance:Center(self.language.Max1Num)
			else
				PublicPopupCtrl.Instance:Center(self.language.MaxNum)
			end
		end
		if num < 1 then
			num = math.max(1, num)
			PublicPopupCtrl.Instance:Center(self.language.MinNum)
		end
		self.down_cur_num = num
		self:FlushNumShow()
	end)

	self.is_empty_role = false
	self.is_empty_item = false
end

function SocietySendViewF:Awake()
	UIWFlushPanel.Awake(self)

	self.MonoTogs[self.data.send_info_sm.type].isOn = true
	self:FlushRoleList()
end

function SocietySendViewF:FlushRoleList()
	local role_list = self.data:GetFriendListByType(SocietyConfig.FriendGroupType.friend, true)
	local send_info_sm = self.data.send_info_sm

	if send_info_sm.uid > 0 then
		for index, role in ipairs(role_list) do
			if role.info.uid == send_info_sm.uid then
				table.remove(role_list, index)
				table.insert(role_list, 1, role)
				break
			end
		end
	end

	self.EmptyObj1:SetActive(0 == #role_list)
	self.is_empty_role = 0 == #role_list
	self.RoleList:SetData(role_list)
	self.RoleList:ClickItemData(role_list[1])
end

function SocietySendViewF:FlushTimes()
	local remain_times = self.data:GetSendRemainTimes()
	self.remain_times = remain_times or 0
	self.RemainTimes.text = string.format(self.language.RemainTimes, remain_times)
end

function SocietySendViewF:FlushSendNum()
	local sel_data = self.ItemList:GetSelectedData()
	if nil == sel_data[1] then return end
	local index = self.ItemList.data_t[sel_data[1]]
	if nil == index then return end
	local cell = self.ItemList.index_item[index]
	if nil == cell then return end
	cell.SocietySend.text = self.data.send_info_sm.send_num
end

function SocietySendViewF:FlushItemId()
	local sel_item = self.data.send_info_sm.sel_item
	local co = self.data:GetSendItemTypeShow(sel_item)
	self.BgPos.sizeDelta = Vector2.New(self.BgPos.sizeDelta.x, co.bg_height)
	self.ItemListPos.sizeDelta = Vector2.New(self.ItemListPos.sizeDelta.x, co.list_height)
	self.Flower100Obj:SetActive(co.show_flower100)
	self.SendMsgObj:SetActive(co.show_msg)
	self.AutoBuyObj:SetActive(SocietyConfig.ShareType.GiftFlowers == self.data.send_info_sm.type and nil ~= ShopData.GetFastBuyData(sel_item))
	self.down_max_num = self.data:GetSendFlowerMaxNum(sel_item, self.TogAutoBuy.isOn)
	self.down_cur_num = 1
	self:FlushNumShow()
	self:FlushTimes()
end

function SocietySendViewF:FlushType()
	self:FlushItems()
	self:FlushItemId()
end

function SocietySendViewF:FlushItems()
	local item_list, sel_item = self.data:GetSendItemList()
	self.EmptyObj2:SetActive(0 == #item_list)
	self.is_empty_item = 0 == #item_list
	self.ItemList:SetData(item_list)
	self.ItemList:ClickItemData(sel_item)
end

function SocietySendViewF:OnToggleMono(value)
	self.data.send_info_sm.type = value
	if SocietyConfig.ShareType.GiftFlowers == value then
		self.data.is_send_gift_block_item_info = true
	end
end

function SocietySendViewF:OnToggleAutoBuy()
	self.down_max_num = self.data:GetSendFlowerMaxNum(self.data.send_info_sm.sel_item, self.TogAutoBuy.isOn)
end

function SocietySendViewF:FlushNumShow()
	self.DownCurNum.text = self.down_cur_num
	self.data.send_info_sm.send_num = self.down_cur_num
end

function SocietySendViewF:OnClickDownAdd()
	if self.down_max_num > self.down_cur_num then
		self.down_cur_num = self.down_cur_num + 1
		self:FlushNumShow()
	elseif SocietyConfig.GiftItemTypeShow.flower2 == self.data:GetSendItemTypeShow(self.data.send_info_sm.sel_item) then
		PublicPopupCtrl.Instance:Center(self.language.Max1Num)
	else
		PublicPopupCtrl.Instance:Center(self.language.MaxNum)
	end
end

function SocietySendViewF:OnClickDownSub()
	if 1 < self.down_cur_num then
		self.down_cur_num = self.down_cur_num - 1
		self:FlushNumShow()
	else
		PublicPopupCtrl.Instance:Center(self.language.MinNum)
	end
end

function SocietySendViewF:OnClickSend()
	if self.is_empty_role then
		PublicPopupCtrl.Instance:Center(self.language.EmptyRoleTips)
		return
	end
	if self.is_empty_item then
		PublicPopupCtrl.Instance:Center(self.language.EmptyItemTips)
		return
	end
	local sis = self.data.send_info_sm
	if SocietyConfig.ShareType.GiftFlowers == self.data.send_info_sm.type and not self.data:IsOnLineByUid(sis.sel_role.info.uid) then
		PublicPopupCtrl.Instance:Center(self.language.NotIsOnline)
		return
	end
	if not self.data:SocietySendGiftNumEnough(self.language, self.TogAutoBuy.isOn, self.down_cur_num) then
		return
	end
	if (self.word_num_per or 0) > self.world_num_limit then
		PublicPopupCtrl.Instance:Center(Language.Chat.WordNumTips)
		return
	end
	
	local _type = self.data.send_info_sm.type
	if SocietyConfig.ShareType.GodLandTrace == _type or SocietyConfig.ShareType.LiveItem == _type then
		local str = StringIsEmpty(self.InputField.text) and self.language.DefaultMsg or self.InputField.text
		
		local function callback(str_f)
			sis.send_num = sis.send_num or 1
			if self.remain_times < sis.send_num then
				PublicPopupCtrl.Instance:Error(self.language.GiveTip)
				return
			end
			SocietyCtrl.Instance:SendGiftReqGive(sis.sel_role.info.uid, sis.sel_item, sis.send_num, str_f)
		end

		if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
			ChatSubCustom.PCChatFilter(str, 2, callback)
		else
			callback(ChatFilter.Instance:Filter(str))
		end
		
	else
		SocietyCtrl.Instance:SendGiftReqSendFlower(sis.sel_role.info.uid, sis.sel_item, sis.send_num, self.TogAutoBuy.isOn and nil ~= ShopData.GetFastBuyData(sis.sel_item), str_f)
	end

	ViewMgr:CloseView(SocietySendView)
	ViewMgr:CloseView(GodLandTracesView)
	ViewMgr:OpenView(SocietyView)
end

SocietyShareViewRoleItem = DeclareMono("SocietyShareViewRoleItem", UIWidgetBaseItem)

function SocietyShareViewRoleItem:SocietyShareViewRoleItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyShareViewRoleItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	self.IntimacyVal.text = self.society_data:GetFriendIntimacyVal(info.uid)
	if info.top_level and info.top_level > 0 then
		self.PeakLevel:SetActive(true)
		UH.SetText(self.Level, info.top_level)
		UH.Color(self.Level, COLORS.Red8)
	else
		UH.SetText(self.Level, info.level)
		UH.Color(self.Level, COLORS.White)
		self.PeakLevel:SetActive(false)
	end
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_id,info.avatar_quality)
	AvatarEffect.SetImageEnabled(self.PeakImg,info.avatar_quality)
	local is_online = info.is_online >= 1
	self.MaskObj:SetActive(not is_online)
end

function SocietyShareViewRoleItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyShareViewRoleItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.society_data.send_info_sm.sel_role:Set(self.data)
    end
end