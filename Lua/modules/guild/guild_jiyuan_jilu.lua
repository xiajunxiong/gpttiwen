GuildJiYuanJiLu = GuildJiYuanJiLu or DeclareView("GuildJiYuanJiLu", "guild/guild_jiyuan_jilu")
function GuildJiYuanJiLu:GuildJiYuanJiLu()
    self.lang = Language.Guild
end

function GuildJiYuanJiLu:CloseCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function GuildJiYuanJiLu:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab12, Vector2.New(973, 600))
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GIFT_GATHER_DRAEM_LIST})
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GET_GATHER_DRAEM_LIST})
end

GuildJiYuanJiLuPanel = GuildJiYuanJiLuPanel or DeclareMono("GuildJiYuanJiLuPanel", UIWFlushPanel)
function GuildJiYuanJiLuPanel:GuildJiYuanJiLuPanel()
    self.data = GuildData.Instance
    self.lang = Language.Pet
	self.data_cares = {
        {data = self.data.guild_jiyuan_jilu_get_list, func = self.FlushJiLuGetView, init_call = false},
		{data = self.data.guild_jiyuan_jilu_send_list, func = self.FlushJiLuSendView, init_call = false},
    }
end

function GuildJiYuanJiLuPanel:FlushJiLuGetView()
	local get_dream_data = self.data:GetRoleGatherDreamGetList()
	local data_count = table.nums(get_dream_data)
	if data_count > 0 then
		self.GetKong:SetActive(false)
	else
		self.GetKong:SetActive(true)
	end
    self.JiYuanJiLuGetList:SetData(get_dream_data)
end

function GuildJiYuanJiLuPanel:FlushJiLuSendView()
	local send_dream_data = self.data:GetRoleGatherDreamSendList()
	local data_count = table.nums(send_dream_data)
	if data_count > 0 then
		self.SendKong:SetActive(false)
	else
		self.SendKong:SetActive(true)
	end
    self.JiYuanJiLuSendList:SetData(send_dream_data)
end

GuildJiYuanJiLuItem = GuildJiYuanJiLuItem or DeclareMono("GuildJiYuanJiLuItem", UIWidgetBaseItem)
function GuildJiYuanJiLuItem:GuildJiYuanJiLuItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end

function GuildJiYuanJiLuItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self:SetPlayInfo()
	self:SetItemInfo()
end

function GuildJiYuanJiLuItem:SetPlayInfo()
	local player_name = self.data.from_name--"Player"
	local player_positin = self.data.post --"Position"
	local position_name = self.guild_data:GetJobInfo(player_positin).member_name
	local avatar_type = self.data.avatar_type--10
	local avatar_id = self.data.avatar_id--10
	local avatar_quality = 0
	UH.SetText(self.Name,player_name)
	UH.SetText(self.Position,position_name)
	local avatardata = {
		appearance = {avatar_type = avatar_type,avatar_id = avatar_id,avatar_quality = avatar_quality}
	}
	self.Avatar:SetData(avatardata)
end

function GuildJiYuanJiLuItem:SetItemInfo()
	local time_show = self.data.timestamp or os.time()
	local jilu_type = 0	
	if self.data.info_type  == 1 then
		jilu_type = 1
	end
	local tex_dec = self.lang.JiYuanJiLuGiveMe
	local string_f = self.lang.JiYuanJiLuTimeM
	if jilu_type == 0 then
		string_f = self.lang.JiYuanJiLuTimeT
	else
		local sex = RoleData.Instance:GetRoleSex(self.data.avatar_type)
		if sex == 0 then
			tex_dec = self.lang.JiYuanJiLuGiveHer
		else
			tex_dec = self.lang.JiYuanJiLuGiveHe
		end
	end

    local _time = TimeCtrl.Instance:ChangDataTimeDetail(time_show)
    local function t_add(time)
        return time < 10 and tostring("0"..time) or time
    end
    _time.hour =  t_add(_time.hour)
    _time.min =  t_add(_time.min)
    local time_dec = string.format(string_f,_time.month,_time.day,_time.hour,_time.min)	
	
	UH.SetText(self.ItemJiLu,time_dec)
	UH.SetText(self.ItemDec,tex_dec)
	local item_id = self.data.item_id
	self.ItemTex:SetDataByItemId(item_id)
end



