GuildJiYuan = GuildJiYuan or DeclareMono("GuildJiYuan", UIWFlushPanel)
function GuildJiYuan:GuildJiYuan()
	self.lang = Language.Guild
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.guild_jiyuan_list, func = self.FlushJiYuanView, init_call = false},
		{data = BagData.Instance.item_grid[ItemColumnType.Task],func = self.FlushJiYuanView, init_call = false}
	}
	self.flush_item_handle = nil
end

function GuildJiYuan:onFlush()
	if self.data.today_is_publish_gather_dream_red then
		self.data.guild_jiyuan_get_point.count = 0
		self.data.today_is_publish_gather_dream_red = false
	end
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GATHER_DREAM_LIST})
	self:FlushJiYuanView()
end

function GuildJiYuan:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.flush_item_handle ~= nil then
		TimeHelper:CancelTimer(self.flush_item_handle)
		self.flush_item_handle = nil
	end
end 

function GuildJiYuan:FlushJiYuanView()
	self.data.guild_jiyuan_haven_give = 0
	local dream_data = self.data:GetGatherDreamInfoList()
	local data_count = table.nums(dream_data)
	if data_count > 0 then
		self.Kong:SetActive(false)
	else
		self.Kong:SetActive(true)
	end
    self.JiYuanList:SetData(dream_data)
	local can_publish = false
	if self.data.today_is_publish_gather_dream_flag == 0 then
		can_publish = true	
	end
	self.ButtonFaBu:SetActive(can_publish)
	self.ButtonFaBuHui:SetActive(not can_publish)
	
	if self.data.guild_jiyuan_open_index_id ~=0 then	
		if data_count ~= 0 then
			local index = self:GetIndex(dream_data,self.data.guild_jiyuan_open_index_id)
			if index > 0 then
				if self.flush_item_handle ~= nil then
					TimeHelper:CancelTimer(self.flush_item_handle)
					self.flush_item_handle = nil
				end
		
				self.flush_item_handle = TimeHelper:AddDelayTimer(function ()
					self.flush_item_handle = nil
					self.JiYuanList:JumpVerticalIndex(index, 140)
				end, 0.2)
			end
		end
		self.data.guild_jiyuan_open_index_id = 0
	end
end


function GuildJiYuan:GetIndex(list,seq)
    for k,v in pairs(list or {}) do
        if v.role_id == seq then
            return k
        end
    end
	return 0
end

function GuildJiYuan:OnClickJiLu()
    ViewMgr:OpenView(GuildJiYuanJiLu)
end

function GuildJiYuan:OnClickFaBu()
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_ROLE_CAN_PUBLISH_LIST})
    ViewMgr:OpenView(GuildJiYuanFaBu)
end

function GuildJiYuan:OnClickFaBuHui()
	local show_tip = self.lang.JiYuanCantPublish
	PublicPopupCtrl.Instance:Center(show_tip)
end

GuildJiYuanItem = GuildJiYuanItem or DeclareMono("GuildJiYuanItem", UIWidgetBaseItem)
function GuildJiYuanItem:GuildJiYuanItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end
function GuildJiYuanItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self:SetPlayInfo()
	self:SetItemInfo()
	self:ButtonTypeChange()
end

function GuildJiYuanItem:SetPlayInfo()
	local player_name = self.data.from_name --"Player"
	local avatar_type = self.data.avatar_type
	local avatar_id = self.data.avatar_id
	local avatar_quality = 0

	local help_times = self.data.help_num--0
	local help_times_show = string.format(self.lang.JiYuanHelpTimes,help_times)
	local is_show_help = true
	local is_my_player = false
	local role_id = RoleData.Instance:GetRoleId()
	if role_id == self.data.role_id then
		is_my_player = true
		is_show_help = false
	end
	local color_type = COLORS.Red9
	if is_my_player then
		color_type = COLORS.Purple3
		is_show_help = false
	end
	self.HelpObj:SetActive(is_show_help)
	UH.Color(self.Name,color_type)
	UH.SetText(self.Name,player_name)
	UH.SetText(self.Help,help_times_show)
	local avatardata = {
		appearance = {avatar_type = avatar_type,avatar_id = avatar_id,avatar_quality = avatar_quality}
	}
	self.Avatar:SetData(avatardata)
end

function GuildJiYuanItem:SetItemInfo()
	local item_data = Item.New()
    item_data.item_id = self.data.item_id--23233
	local item_name = Item.GetQuaName(item_data.item_id,true)
	self.ItemTex:SetData(item_data)
	local total_value = self.data.gather_num--10
	local total_all = self.data.need_max_num--100
	local rate = total_value / total_all
	local p_text = tostring(total_value).." / "..tostring(total_all)
	self.Progress:SetProgress(rate)
	self.Progress:SetText(p_text)
	local ihave = BagData.Instance:GetNumByItemId(self.data.item_id) + BagData.Instance:GetStorageNumByItemId(self.data.item_id)
	local ihave_show = string.format(self.lang.JiYuanItemHave,ihave)
	UH.SetText(self.ItemName,item_name)
	UH.SetText(self.ItemHave,ihave_show)
end

function GuildJiYuanItem:ButtonTypeChange()
	local b_type = self.data.b_type
	local role_id = RoleData.Instance:GetRoleId()
	if role_id == self.data.role_id then
		if self.data.gather_num > 0 and self.data.gather_num > self.data.fetch_num then
			b_type = 2
			self.guild_data.guild_jiyuan_get_point.count = 1
			self.redpoint:SetNum(1)
		else
			b_type = 4
			self.guild_data.guild_jiyuan_get_point.count = 0
			self.redpoint:SetNum(0)
			if self.data.gather_num == 0 then
				self.get_show_type = 0
			else
				self.get_show_type = 1
			end
		end
	else
		if b_type ~= 3 then
			local ihave = BagData.Instance:GetNumByItemId(self.data.item_id) + BagData.Instance:GetStorageNumByItemId(self.data.item_id)
			if self.data.gather_num == self.data.need_max_num or ihave == 0 then
				b_type = 5
				if self.data.gather_num == self.data.need_max_num then
					self.cant_give_type = 0
				else
					self.cant_give_type = 1
				end
			else
				b_type = 1
			end
		end
	end
	
	if b_type == 3 then
		self.guild_data.guild_jiyuan_haven_give = self.guild_data.guild_jiyuan_haven_give + 1
	end
	
	for i = 1,5 do
		if b_type == i then
			self["Button"..i]:SetActive(true)
		else
			self["Button"..i]:SetActive(false)
		end
	end
end

function GuildJiYuanItem:OnClickGive()
	if self.guild_data.guild_jiyuan_haven_give < Config.family_auto.other[1].pray_give_maxnum then 
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GIFT_GATHER_DREAM,param = self.data.role_id})
	else
		local show_tip = self.lang.JiYuanHaveGiveFull
		PublicPopupCtrl.Instance:Center(show_tip)
	end
end

function GuildJiYuanItem:OnClickGet()
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_FETCH_GATHER_DREAM})
end

function GuildJiYuanItem:OnClickGetHui()
	local show_tip = ""
	if self.get_show_type == 0 then
		show_tip = self.lang.JiYuanGetHuiZero
	else
		show_tip = self.lang.JiYuanGetHuiFull
	end
	PublicPopupCtrl.Instance:Center(show_tip)
end

function GuildJiYuanItem:OnClickGiveHui()
	local show_tip = ""
	if self.cant_give_type == 0 then
		show_tip = self.lang.JiYuanGiveHui2
	else
		show_tip = self.lang.JiYuanGiveHui
	end
	PublicPopupCtrl.Instance:Center(show_tip)
end

function GuildJiYuanItem:OnClickHaveGive()
	local show_tip = self.lang.JiYuanGiveHui1
	PublicPopupCtrl.Instance:Center(show_tip)
end

function GuildJiYuanItem:OnClickGet()
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_FETCH_GATHER_DREAM})
end

function GuildJiYuanItem:GetFaBuRewardList(patch_item)
	local quality = 1
	local pet_id = PetData.Instance:GetPetIdByPatchId(patch_item)	
	local pet_cfg = Config.pet_cfg_auto.pet_info_2
	for k1, v1 in pairs(pet_cfg) do
		if v1.id == pet_id then
			quality = v1.quality
			break
		end
	end
	local family_cfg = Config.family_auto.family_pray
	local c_family = family_cfg[quality]
	local list = {}
	local coin_data = Item.New()
    coin_data.item_id = c_family.coin_icon
	coin_data.num = c_family.coin
	table.insert(list,coin_data)
	
	local gongxian_data = Item.New()
    gongxian_data.item_id = c_family.gongxian_icon
	gongxian_data.num = c_family.gongxian
	table.insert(list,gongxian_data)
	
	local exp_data = Item.New()
    exp_data.item_id = c_family.exp_icon
	exp_data.num = c_family.exp
	table.insert(list,exp_data)	 
	return list
end
