BattlefieldReportView = BattlefieldReportView or DeclareView("BattlefieldReportView", "battlefield/battlefield_report")

function BattlefieldReportView:BattlefieldReportView()
    self.data = BattlefieldData.Instance
end

function BattlefieldReportView:LoadCallback()
    self:FlushReportView(self.data:GetReportList())
end

function BattlefieldReportView:FlushReportView(list)
    self.List:SetData(list)
    self.NotHasTip:SetActive(#list == 0)
    self.WinRate:SetObjActive(#list ~= 0)
    self:FlushWinInfoView()
end

--刷新战绩
function BattlefieldReportView:FlushWinInfoView()
    local win_info = self.data:GetRanksInfo()
    local win_times = self.data:GetWinStreakTimes()
    local tolal_times = win_info.tolal_times or 1
    tolal_times = tolal_times == 0 and 1 or tolal_times
    local win_rate = math.floor(win_info.win_times / tolal_times * 100)
    UH.SetText(self.WinRate,Format(Language.Battlefield.ReportWinRate,win_rate))
end

function BattlefieldReportView:OnClickClose()
    ViewMgr:CloseView(BattlefieldReportView)
end

function BattlefieldReportView:OnClickSeed(data)
    self.Detaile:SetActive(not self.Detaile.activeSelf)
    if data ~= nil then
        self.DetaileList[1]:SetData(data.left_user_list)
        self.DetaileList[2]:SetData(data.right_user_list)
        UH.SpriteName(self.WinImg[1],"Win" .. data.fight_result)
        UH.SpriteName(self.WinImg[2],"Win" .. tostring(data.fight_result <= 1 and math.abs(data.fight_result - 1) or data.fight_result))
    end
end

----------------------------BattlefieldReportItem----------------------------
BattlefieldReportItem = BattlefieldReportItem or DeclareMono("BattlefieldReportItem", UIWidgetBaseItem)

function BattlefieldReportItem:SetData(data)
    for i=1,self.RoleItem:Length() do
        self.RoleItem[i]:SetData(data.left_user_list[i])
    end
    UH.SpriteName(self.Win,"Win" .. data.fight_result)
    UH.SpriteName(self.ImgItem,data.index % 2 == 0 and "WengZiDi1" or "WengZiDi2")
    UH.SetText(self.Round,Format(Language.Battlefield.FightRoundTip,data.fight_round))
    local sprite = RichTextHelper.SpriteStrAtlas(CommonItemId.BattlefieldPoint, 40, 40, nil, "public")
    UH.SetText(self.Score,BattlefieldData.Instance:GetReportScoreDesc(data) .. sprite .. Format(" +%s",data.add_coin))
    UIWidgetBaseItem.SetData(self, data)
end

----------------------------BattlefieldReportRoleItem----------------------------
BattlefieldReportRoleItem = BattlefieldReportRoleItem or DeclareMono("BattlefieldReportRoleItem", UIWidgetBaseItem)

function BattlefieldReportRoleItem:SetData(data)
    if data ~= nil then
        UH.SetAvatar(self.Head,data.avatar_type,data.avatar_id,data.avatar_quality)
        local index = BattlefieldData.Instance:GetTeamLabel(data)
        UH.SpriteName(self.Captain,index ~= 0 and "TeamLabel" .. index or nil)
        UH.SetText(self.Label,Language.Battlefield.TeamLabel[index] or "")
    else
        UH.SetAvatar(self.Head)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldReportRoleItem:OnClickItem()
	BattlefieldCtrl.Instance:OnRoleSimpleInfo(self.data)
end

----------------------------BattlefieldReportDetaileRoleList----------------------------
BattlefieldReportDetaileRoleList = BattlefieldReportDetaileRoleList or DeclareMono("BattlefieldReportDetaileRoleList", UIWidgetBaseItem)

function BattlefieldReportDetaileRoleList:SetData(data)
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(data[i])
    end
end

----------------------------BattlefieldReportDetaileRoleItem----------------------------
BattlefieldReportDetaileRoleItem = BattlefieldReportDetaileRoleItem or DeclareMono("BattlefieldReportDetaileRoleItem", UIWidgetBaseItem)

function BattlefieldReportDetaileRoleItem:SetData(data)
    local role_name = data.role_name
    local rank_name = BattlefieldData.QuaName(data.rank_config,true)
    UH.SpriteName(self.Prof, ProfSettingB[data.prof])
    UH.SetText(self.Rank,data.is_top_race == 1 and Language.Battlefield.NotRankTip or rank_name)
    UH.SetAvatar(self.Head,data.avatar_type,data.avatar_id,data.avatar_quality)
    if data.is_captain == true then
        local index = BattlefieldData.Instance:GetTeamLabel(data)
        UH.SpriteName(self.Captain,index ~= 0 and "TeamLabel" .. index or nil)
        UH.SetText(self.Label,Language.Battlefield.TeamLabel[index or ""])
        role_name = data.is_self and ColorStr(role_name,COLORSTR.Green9) or role_name
    else
        role_name = ColorStr(role_name,COLORSTR.Red5)
    end
    UH.SetText(self.Name,role_name)
    self.Captain:SetObjActive(data.is_captain == true)
    for i=1,self.Progress:Length() do
        self.Progress[i]:SetProgress((data.hurt_list[i] or 1)/ (data.max_hurt_list[i] or 1))
        self.Progress[i]:SetText(data.hurt_list[i] or 0)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldReportDetaileRoleItem:OnClickItem()
	BattlefieldCtrl.Instance:OnRoleSimpleInfo(self.data)
end