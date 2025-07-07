RuinsResult = RuinsResult or DeclareView("RuinsResult","god_areas_ruins/ruins_result")

function RuinsResult:RuinsResult()
    self.data = RuinsData.Instance
    self.ruins_data = RuinsData.Instance
    self.palaces_data = PalacesData.Instance
end
function RuinsResult:LoadCallback(param_t)
    if param_t.protocol == nil then
        return
    end
    local info = param_t.protocol

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
        is_real_win = 0
        if is_atker then
            is_real_win = 0
        else
            is_real_win = 1
        end
    end
    local reward2
    local reward
    if info.act_type == ActType.Ruins then
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

    if is_real_win == 1 then
        self.EffectTool:Play(10010204)
    else
        self.EffectTool:Play(10010245)
    end
    local item1 = info.result_item1
    local item2 = info.result_item2
    UH.SetText(self.Name, item1.role_name)
    UH.SetText(self.Name2, item2.role_name)
    UH.SetText(self.Server, self.data:ServerName(item1.server_id))
    UH.SetText(self.Server2, self.data:ServerName(item2.server_id))
    UH.SpriteName(self.Prof, RoleData.GetProfSpByProfId(item1.prof or 0,false))
    UH.SpriteName(self.Prof2, RoleData.GetProfSpByProfId(item2.prof or 0,false))
    if is_atker then
        UH.SetText(self.BingLi, string.format(Language.GodAreasActs.BingLiZhi2, reward.power))
        UH.SetText(self.BingLi2, string.format(Language.GodAreasActs.BingLiZhi2, reward2.power))
        UH.SetText(self.JiFen, string.format(Language.GodAreasActs.JiFen2, reward.score))
        UH.SetText(self.JiFen2, string.format(Language.GodAreasActs.JiFen2, reward2.score))
    else
        UH.SetText(self.BingLi, string.format(Language.GodAreasActs.BingLiZhi2, reward.power))
        UH.SetText(self.BingLi2, string.format(Language.GodAreasActs.BingLiZhi2, reward2.power))
        UH.SetText(self.JiFen, string.format(Language.GodAreasActs.JiFen2, reward.score))
        UH.SetText(self.JiFen2, string.format(Language.GodAreasActs.JiFen2, reward2.score))
    end
    UH.SetAvatarp(self.RoleHead, item1.avatar_type, item1.avatar_id)
    UH.SetAvatarp(self.RoleHead2, item2.avatar_type, item2.avatar_id)
end
function RuinsResult:OnClickClose()
    ViewMgr:CloseView(RuinsResult)
end