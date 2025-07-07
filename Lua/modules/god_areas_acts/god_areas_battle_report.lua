GodAreasBattleReport = GodAreasBattleReport or DeclareView("GodAreasBattleReport", "god_areas_acts/god_areas_battle_report")
function GodAreasBattleReport:GodAreasBattleReport()
    self.Board:SetData(self:GetType(), Language.GodAreasActs.BattleReport,Vector2.New(969.5288,679.5524))
end
function GodAreasBattleReport:LoadCallback(param_t)
    --[[ local temp = {}
    for i = 1, 10, 1 do
        table.insert(temp, {is_attacker_win = i%2 ==0 and 1 or 0, result_item1 = self:GuiXuZhiJingScenePVPResultItem(i), result_item2 = self:GuiXuZhiJingScenePVPResultItem(i + 1)})
    end ]]
    self.List:SetData(param_t)
end
function GodAreasBattleReport:GuiXuZhiJingScenePVPResultItem(i)
    local info = {}
    -- info.role_uuid = MsgAdapter.ReadLL()--plat_type + role_id == role_uuid
    info.role_id = i%2 == 0 and RoleData.Instance:GetRoleId() or 0
    info.plat_type = 0
    info.role_name = "傻逼玩意"
    info.server_id = 0
    info.prof = 1
    return info
end

GodAreasReportItem = GodAreasReportItem or DeclareMono("GodAreasReportItem", UIWidgetBaseItem)
function GodAreasReportItem:GodAreasReportItem()
    self.ruins_data = RuinsData.Instance
    self.palaces_data = PalacesData.Instance
end
function GodAreasReportItem:SetData(data)
    
    local info = data
    local role_id = RoleData.Instance:GetRoleId()
    local is_atker = info.result_item1.role_id == role_id
    local is_atk_win = info.is_attacker_win == 1
    local result_type = 0
    local result_type2 = 0
    local is_real_win = 0
    if is_atk_win then
        result_type = 0
        result_type2 = 4
        if is_atker then
            is_real_win = 1
        else
            is_real_win = 0
        end
    else
        result_type = 1
        result_type2 = 3
        if is_atker then
            is_real_win = 0
        else
            is_real_win = 1
        end
    end
    local reward2
    local reward
    if data.act_type == ActType.Ruins then
        reward = self.ruins_data:GetBattleReward(result_type)
        reward2 = self.ruins_data:GetBattleReward(result_type2)
    else
        reward = self.palaces_data:GetBattleReward(result_type)
        reward2 = self.palaces_data:GetBattleReward(result_type2)
    end
    if is_real_win == 1 then
        UH.SpriteName(self.ResultIcon, "Sheng")
    else
        UH.SpriteName(self.ResultIcon, "Bai")
    end
    local item1 = info.result_item1
    local item2 = info.result_item2
    UH.SetText(self.Name, item1.role_name)
    UH.SetText(self.Name2, item2.role_name)
    if is_atker then
        UH.SetText(self.BingLi, string.format(Language.GodAreasActs.BingLiZhi2, reward.power))
        UH.SetText(self.BingLi2, string.format(Language.GodAreasActs.BingLiZhi2, reward2.power))
    else
        UH.SetText(self.BingLi, string.format(Language.GodAreasActs.BingLiZhi2, reward.power))
        UH.SetText(self.BingLi2, string.format(Language.GodAreasActs.BingLiZhi2, reward2.power))
    end
    --[[ UH.SetText(self.Server, self.data:ServerName(item1.server_id))
    UH.SetText(self.Server2, self.data:ServerName(item2.server_id)) ]]
    UH.SpriteName(self.Prof, RoleData.GetProfSpByProfId(item1.prof or 0,false))
    UH.SpriteName(self.Prof2, RoleData.GetProfSpByProfId(item2.prof or 0,false))

    UH.SetAvatarp(self.RoleHead, item1.avatar_type, item1.avatar_id)
    UH.SetAvatarp(self.RoleHead2, item2.avatar_type, item2.avatar_id)
end