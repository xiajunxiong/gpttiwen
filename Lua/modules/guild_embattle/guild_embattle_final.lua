GuildEmbattleFinal = GuildEmbattleFinal or DeclareView('GuildEmbattleFinal', 'guild_embattle/guild_embattle_final')
-- VIEW_DECLARE_LEVEL(GuildEmbattleFinal, ViewLevel.L1)
VIEW_DECLARE_MASK(GuildEmbattleFinal, ViewMask.BgBlock)
function GuildEmbattleFinal:LoadCallback()
end
function GuildEmbattleFinal:OnClickClose()
    ViewMgr:CloseView(GuildEmbattleFinal)
end

GuildEmbattleFinalPanel = GuildEmbattleFinalPanel or DeclareMono('GuildEmbattleFinalPanel', UIWFlushPanel)
function GuildEmbattleFinalPanel:GuildEmbattleFinalPanel()
    self.data = GuildEmbattleData.Instance
end
function GuildEmbattleFinalPanel:onFlush()
    --获取最后一轮的对战列表
    local info = self.data:FinalMatchList()
    if table.nums(info) == 0 then
        info = self.data:FinalMatchList2()
    end
    self.List:SetData(info)
end

GuildEmbattleFinalItem = GuildEmbattleFinalItem or DeclareMono('GuildEmbattleFinalItem', UIWidgetBaseItem)
function GuildEmbattleFinalItem:GuildEmbattleFinalItem()
    self.embattle_data = GuildEmbattleData.Instance
end
function GuildEmbattleFinalItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    --分两部分
    local figher1 = data[1]
    local figher2 = data[2]
    if figher1 == nil then
        LogDG('数据有问题')
        return
    end
    if  figher1.match_info.win_rank == 1 then
        UH.SetText(self.WinRank, Language.GuildEmbattle.RankTitle[1])
    elseif figher1.match_info.win_rank == 3 then
        UH.SetText(self.WinRank, Language.GuildEmbattle.RankTitle[2])
    else
        UH.SetText(self.WinRank, string.format(Language.GuildEmbattle.WinRank, figher1.match_info.win_rank))
    end
    self:FlushLeft(figher1, data)
    self:FlushRight(figher2, data)
    self.Watch:SetActive(false)
    --[[  local act_info = ActivityData.Instance:GetActivityStatusInfo(ActType.GuildEmbattle)
    local base = GuildEmbattleData.Instance:Base()
    if act_info.status == ActStatusType.Close then
        self.Watch:SetActive(false)
    else
        if data and data.battle_result and base.state == 1 then
            local is_show = false
            for _, value in ipairs(data.battle_result) do
                if value == 0 then
                    is_show = true
                end
            end
            if is_show == true and data.is_finish == 1 then
                is_show = false
            end
            self.Watch:SetActive(is_show)
        else
            self.Watch:SetActive(false)
        end
    end
 ]]
end
function GuildEmbattleFinalItem:FlushLeft(data, all)
    if data == nil then
        self.LeftShow:SetActive(false)
        self.LeftNone:SetActive(true)
        return
    end
    self.LeftShow:SetActive(true)
    self.LeftNone:SetActive(false)
    local guild_id = GuildData.Instance:GetGuildID()
    UH.SetText(
        self.BaseInfo1[1],
        guild_id == data.guild_id and ColorStr(self.embattle_data:GuildName(data.name), COLORSTR.Green7) or data.name
    )
    UH.SetText(
        self.BaseInfo1[2],
        string.format(Language.GuildEmbattle.ServerName, self.embattle_data:ServerName(data.server_id))
    )
    UH.SetText(self.BaseInfo1[3], data.banner)
    -- 上届标识
    if data.last_match_rank and data.last_match_rank > 0 and data.last_match_rank < 4 then
        local info = Language.GuildEmbattle.LastRank[data.last_match_rank]
        UH.SpriteName(self.BaseInfo1[5], info[1])
        UH.SetText(self.BaseInfo1[4], info[2])
        self.BaseInfo1[5]:SetObjActive(true)
    else
        self.BaseInfo1[5]:SetObjActive(false)
    end
    local result = 0
    if data then
        result = data:ResultType()
    end
    self.LeftSucc:SetActive(result == 1)
    if result == 1 then
        self.FailInter[1].Interactable = true
    elseif result == 2 then
        self.FailInter[1].Interactable = false
    else
        self.FailInter[1].Interactable = true
    end
end
function GuildEmbattleFinalItem:FlushRight(data, all)
    if data == nil then
        self.RightNone:SetActive(true)
        self.RightShow:SetActive(false)
        return
    end
    self.RightNone:SetActive(false)
    self.RightShow:SetActive(true)
    local guild_id = GuildData.Instance:GetGuildID()
    UH.SetText(
        self.BaseInfo2[1],
        guild_id == data.guild_id and ColorStr(self.embattle_data:GuildName(data.name), COLORSTR.Green7) or data.name
    )
    UH.SetText(
        self.BaseInfo2[2],
        string.format(Language.GuildEmbattle.ServerName, self.embattle_data:ServerName(data.server_id))
    )
    UH.SetText(self.BaseInfo2[3], data.banner)
    -- 上届标识
    if data.last_match_rank and data.last_match_rank > 0 and data.last_match_rank < 4 then
        local info = Language.GuildEmbattle.LastRank[data.last_match_rank]
        UH.SpriteName(self.BaseInfo2[5], info[1])
        UH.SetText(self.BaseInfo2[4], info[2])
        self.BaseInfo2[5]:SetObjActive(true)
    else
        self.BaseInfo2[5]:SetObjActive(false)
    end
    local result = 0
    if data then
        result = data:ResultType()
    end
    self.RightSucc:SetActive(result == 1)
    if result == 2 then
        self.FailInter[2].Interactable = false
    elseif result == 1 then
        self.FailInter[2].Interactable = true
    else
        self.FailInter[2].Interactable = true
    end
end
