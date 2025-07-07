GuildList = GuildList or DeclareView("GuildList", "guild/guild_list")
VIEW_DECLARE_LEVEL(GuildList, ViewLevel.L1)

function GuildList:GuildList()
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
end

function GuildList:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.Tab6)
    self.Currency:DefaultSet()
end

GuildListInfo = GuildListInfo or DeclareMono("GuildListInfo", UIWFlushPanel)
function GuildListInfo:GuildListInfo()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.guild_notice, func = self.FlushNotice},
        {data = self.data.player_apply_list, func = self.FlushGuildList}
    }
    self.is_lower = true
    self.is_upper = true
end

function GuildListInfo:onFlush()
    for i = 1, self.BtnList:Length() do
        self.BtnList[i].Interactable = self.data:GetGuildID() == 0
    end
end

function GuildListInfo:FlushGuildList()
    local guild_list = self.data:GetGuildList()
	local data = table.rearrange2(guild_list)
    table.sort(data, function (a,b)
        local s_a = 1
        local s_b = 1

        if a.seven_active > b.seven_active then
            s_a = s_a + 1000000
        elseif a.seven_active == b.seven_active then
            s_a = s_a + 1000000
            s_b = s_b + 1000000
        else
            s_b = s_b + 1000000
        end		
		
        if a.level > b.level then
            s_a = s_a + 100000
        elseif a.level == b.level then
            s_a = s_a + 100000
            s_b = s_b + 100000
        else
            s_b = s_b + 100000
        end

		if a.last_change_times < b.last_change_times then
            s_a = s_a + 10000
        elseif a.last_change_times == b.last_change_times then
            s_a = s_a + 10000
            s_b = s_b + 10000	
        else
            s_b = s_b + 10000
        end
		
		if a.guild_id < b.guild_id then
			s_a = s_a + 1000
		else
			s_b = s_b + 1000
		end
        return s_a > s_b
    end)
	
	for k, v in pairs(data) do
		v.rank_index = k
	end
    self.List:SetData(data)
    self.List:ClickItem(1)
    self.NoneList[1]:SetActive(table.nums(data) == 0)
    self.NoneList[2]:SetActive(table.nums(data) == 0)
end

function GuildListInfo:OnClickSortLevel()
    local data = self.data:GetGuildList()
    data = table.rearrange2(data)
    if self.is_lower then
        table.sort(data, DataSorter.KeyLowerSorter("level"))
        self.is_lower = false
    else
        table.sort(data, DataSorter.KeyUpperSorter("level"))
        self.is_lower = true
    end
    self.List:SetData(data)
    self.List:ClickItem(1)
end

function GuildListInfo:OnClickSortId()
    local data = self.data:GetGuildList()
    data = table.rearrange2(data)
    if self.is_upper then
        table.sort(data, DataSorter.KeyUpperSorter("guild_id"))
        self.is_upper = false
    else
        table.sort(data, DataSorter.KeyLowerSorter("guild_id"))
        self.is_upper = true
    end
    self.List:SetData(data)
    self.List:ClickItem(1)
end

function GuildListInfo:OnClickListItem(flag,data)
    if data == nil then
        return
    end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_NOTICE, param = data.guild_id})
    UH.SetText(self.GuildName, data.guild_name)
	UH.SetText(self.GuildId,"ID: "..data.guild_id)
    self.select_data = data
    self:FlushNotice()
end

function GuildListInfo:FlushNotice()
    if self.select_data == nil then
        return
    end
    local notice = self.data:GetGuildNotcie(self.select_data.guild_id)
    UH.SetText(self.GuildNotice, notice)
end

function GuildListInfo:OnClickApply()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.select_data == nil then
        return
    end
    if self.select_data.member_count >= self.select_data.level_info.max_member then
        PublicPopupCtrl.Instance:Center(ErrorText[111])
        return
    end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    GuildCtrl.SendApplyJoinGuildReq(self.select_data.guild_id)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
end

function GuildListInfo:OnClickCreate()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:OpenView(GuildCreate)
end

function GuildListInfo:OnClickCall()
    if self.select_data == nil then
        return
    end
    SocietyCtrl.Instance:SendFriendReq({req_type = SocietyConfig.FriendOperType.is_online, param1 = self.select_data.uid})
end

function GuildListInfo:OnClickSearch()
	AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")
    local temp = self.Input.text
    local data = self.data:GetGuildList()
    local list = {}
    for _, v in pairs(data) do
        local a, b = string.find(v.guild_name, temp)
        if a and b and a > 0 and b > 0 then
            list[v.guild_id] = v
        end
        a, b = string.find(v.guild_id, temp)
        if a and b and a > 0 and b > 0 then
            list[v.guild_id] = v
        end
    end
    list = table.rearrange2(list)
    if next(list) then
        self.List:SetData(list)
        self.List:ClickItem(1)
    else
        if temp == "" then
            self.List:SetData(data)
            self.List:ClickItem(1)
            return
        end
        self.List:SetData({})
        PublicPopupCtrl.Instance:Center(ErrorText[161])
    end
end

function GuildListInfo:OnClickOneKey()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    local all = self.data:GetGuildList()
    GuildCtrl.SendApplyJoinGuildReq(0, 1)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
end

GuildListItem = GuildListItem or DeclareMono("GuildListItem", UIWidgetBaseItem)

function GuildListItem:SetData(data)
    local level_info = GuildData.Instance:GetLevelInfo(data.level)
    UH.SetText(self.Id, data.guild_id)
    UH.SetText(self.Name, data.guild_name)
    UH.SetText(self.Level, data.level)
    UH.SetText(self.Num, Format(Language.Common.Slash, data.member_count, level_info.max_member + data.extra_member_list))
    UH.SetText(self.PName, data.patriarch_name)
    UH.SetText(self.Arms, data.arms)
	UH.SetText(self.SevenActive, data.seven_active)
    self.Gou:SetActive(GuildData.Instance:CheckIsApply(data.guild_id))
    UH.Alpha(self.Bg, data.index % 2 == 0 and 0.65 or 1)
    data.level_info = level_info
	local show_one = false	
	if data.rank_index == 1 then
		show_one = true
	end
	self.ActiveOne:SetActive(show_one)
    UIWidgetBaseItem.SetData(self, data)
end

function GuildListItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end