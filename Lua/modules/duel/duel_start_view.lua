DuelStartView = DuelStartView or DeclareView("DuelStartView", "duel/duel_start")
VIEW_DECLARE_MASK(DuelStartView, ViewMask.None)
local DuelTexts = {
    [1] = "_LocJueDou",
    [2] = "_LocYuFu",
    [3] = "_LocJieBiao"
}

function DuelStartView:LoadCallback()
    if self.close_timer == nil then
        self.close_timer = TimeHelper:AddDelayTimer(BindTool.Bind1(self.OnClickClose, self), 3)
    end
end

function DuelStartView:OnFlush()
    local info = DuelData.Instance:GetInfo()
    local duel_str = DuelTexts[1]
    local list = {}
    if DuelData.Instance:IsAttacker() then
        local name_format = #info.def_list > 1 and Language.Duel.InTeam or "%s"
        local name_str = string.format(name_format, info.def_name)
        if info.duel_type == DuelBattleType.TYPE_ESCORT then
            table.insert(list, string.format(Language.Duel.Atk.FightEscortTips1, name_str))
            duel_str = DuelTexts[3]
        end
        table.insert(list, string.format(Language.Duel.Atk.FightTips1, name_str))
    else
        duel_str = DuelTexts[2]
        local name_format = #info.att_list > 1 and Language.Duel.InTeam or "%s"
        local name_str = string.format(name_format, info.att_name)
        if info.duel_type == DuelBattleType.TYPE_ESCORT then
            table.insert(list, string.format(Language.Duel.Def.FightEscortTips1, name_str))
        end
        table.insert(list, string.format(Language.Duel.Def.FightTips1, name_str))
    end
    self:SetDescs(list)
    UH.SpriteName(self.NameImage, duel_str)
end

function DuelStartView:OnClickClose()
    ViewMgr:CloseView(DuelStartView)
end

function DuelStartView:CloseCallback()
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = nil
end

function DuelStartView:SetDescs(list)
    for i = 1, 2 do
        self.TipsObjs[i]:SetActive(list[i] ~= nil)
        if list[i] ~= nil then
            UH.SetText(self.TipsTexts[i], list[i])
        end
    end
end
