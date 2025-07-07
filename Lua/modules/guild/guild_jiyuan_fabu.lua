GuildJiYuanFaBu = GuildJiYuanFaBu or DeclareView("GuildJiYuanFaBu", "guild/guild_jiyuan_fabu")
function GuildJiYuanFaBu:GuildJiYuanFaBu()
    self.lang = Language.Guild
end

function GuildJiYuanFaBu:CloseCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function GuildJiYuanFaBu:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab13, Vector2.New(885, 552))
end

GuildJiYuanFaBuPanel = GuildJiYuanFaBuPanel or DeclareMono("GuildJiYuanFaBuPanel", UIWFlushPanel)
function GuildJiYuanFaBuPanel:GuildJiYuanFaBuPanel()
    self.data = GuildData.Instance
	self.lang = Language.Guild
--[[	self.data_cares = {
        {data = self.data.guild_jiyuan_fabu_list, func = self.FlushJiLuFabuView, init_call = false},
    }--]]
	self.patch_item = 0
	self.is_show_select = false
	self.is_show_type = 0
	self.patch_type = 0
end

function GuildJiYuanFaBuPanel:onFlush()
	if self.data.publish_is_send_flag == 0 then
		self.IsSendMessage.isOn = true
		self.is_on = true
	else
		self.IsSendMessage.isOn = false
		self.is_on = false
	end
	self:FlushJiLuFabuView()
	self:FlushFaBuTypeList()
end

function GuildJiYuanFaBuPanel:FlushFaBuTypeList()
	local quality_list = 
	{
		[1] = 0,
		[2] = 10,
		[3] = 4,
		[4] = 3,
		[5] = 2,
		[6] = 1,
	}
	local type_list = {}
	for i=1,#quality_list do
		type_list[i] = {}
        type_list[i].name = self.lang.JiYuanFaBuType[i]
		type_list[i].quality = quality_list[i]
	end
	UH.SetText(self.SelectName,type_list[1].name)
	self.BtnList:SetData(type_list)
end

function GuildJiYuanFaBuPanel:FlushJiLuFabuView()
	self.ItemList:SetData(self.data:GetPetPublishGatherDreamListByQuality(0))
end

function GuildJiYuanFaBuPanel:FlushJiLuFabuPartnerView()
	self.ItemList:SetData(self.data:GetPartnerCanPublishGatherDreamList())
end

function GuildJiYuanFaBuPanel:OnClickShowPartner()
	self.is_show_select = false
	self.BtnListObj:SetActive(self.is_show_select)
	self.BtnSelect:SetActive(false)
	self:FlushJiLuFabuPartnerView()
end

function GuildJiYuanFaBuPanel:OnClickShowPet()
	if self.is_show_type == 10 then
		self.ItemList:SetData(self.data:GetRoleCanPublishGatherDreamList())
	else
		self.ItemList:SetData(self.data:GetPetPublishGatherDreamListByQuality(self.is_show_type))
	end
	self.BtnSelect:SetActive(true)
end

function GuildJiYuanFaBuPanel:OnClickFaBuItem(selected,data)
	self.patch_item = data.patch_id
	self.patch_type = data.patch_type
end

function GuildJiYuanFaBuPanel:OnClickJiYuanFaBu()
	if self.patch_item == 0 then
		local show_tip = self.lang.JiYuanFaBuKong
		PublicPopupCtrl.Instance:Center(show_tip)
		return
	end
	local send_flag = 1
	if self.is_on then
		send_flag = 0
	end
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_ROLE_SET_PUBLISH_SEND,param = send_flag})
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_PUBLISH_GATHER_DREAM,param = self.patch_item,param1 = self.patch_type})
	ViewMgr:CloseView(GuildJiYuanFaBu)
end

function GuildJiYuanFaBuPanel:ToggleChange()
	self.is_on = self.IsSendMessage.isOn
end

function GuildJiYuanFaBuPanel:OnClickselect()
	self.is_show_select = not self.is_show_select
	self.BtnListObj:SetActive(self.is_show_select)
end

function GuildJiYuanFaBuPanel:OnClickBtn(data)
	self:OnClickselect()
	UH.SetText(self.SelectName,data.name)
	self.is_show_type = data.quality
	if data.quality == 10 then
		self.ItemList:SetData(self.data:GetRoleCanPublishGatherDreamList())
	else
		self.ItemList:SetData(self.data:GetPetPublishGatherDreamListByQuality(data.quality))
	end
end


GuildJiYuanFaBuItem = GuildJiYuanFaBuItem or DeclareMono("GuildJiYuanFaBuItem", UIWidgetBaseItem)
function GuildJiYuanFaBuItem:GuildJiYuanFaBuItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end

function GuildJiYuanFaBuItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local patch_id = data.patch_id
	local item_data = Item.New()
    item_data.item_id = patch_id
	local item_name = item_data:QuaName()
	self.Cell:SetData(item_data)
	UH.SetText(self.Name,item_name)
	local ihave = Item.GetNum(patch_id) or 0
	local itotal = self:GetComposeNum(patch_id)
	local total_value = ihave
	local total_all = itotal
	local rate = total_value / total_all
	local p_text = tostring(total_value).." / "..tostring(total_all)
	self.ProgressFaBu:SetProgress(rate)
	self.ProgressFaBu:SetText(p_text)
end

function GuildJiYuanFaBuItem:GetComposeNum(patch_id)
	local itotal = 0
	if self.data.patch_type == 0 then
		local pet_compose = Config.pet_cfg_auto.pet_compose
		for k, v in pairs(pet_compose) do
			if v.patch_id == patch_id then
				itotal = v.patch_num
				break
			end
		end
	else
		itotal = self.data.patch_compose
	end
	return itotal
end	

----------------------------GuildJiYuanFaBuBtnItem----------------------------
GuildJiYuanFaBuBtnItem = GuildJiYuanFaBuBtnItem or DeclareMono("GuildJiYuanFaBuBtnItem", UIWidgetBaseItem)

function GuildJiYuanFaBuBtnItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
end




