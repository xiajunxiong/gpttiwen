local result_str = {
    [1] = "Loc_ShengLi_art",
    [2] = "Loc_ShiBai_art",
}

TombRaiderFinishView = TombRaiderFinishView or DeclareView("TombRaiderFinishView", "tomb_raider/tomb_raider_finish")
function TombRaiderFinishView:TombRaiderFinishView()
    self.data = TombRaiderData.Instance

    local finish_info = self.data:GetFBFinishDetailInfo()
    self.teamRaider:SetData(finish_info.attack_team)
    self.teamGuard:SetData(finish_info.defend_team)

    local win_num = self.data:GetFBFinishInfo() and 1 or 2
    UH.SpriteName(self.Result, result_str[win_num])

    self.close_timer = nil

    UH.SetText(self.my_point,string.format( Language.TombRaider.FinishMyPoint, finish_info.my_point))
end

function TombRaiderFinishView:LoadCallback(param_t)
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = TimeHelper:AddDelayTimer(function()
        -- ViewMgr:CloseView(TombRaiderFinishView)
        self.block:SetActive(true)
    end,2)

    TombRaiderBattleShow.Instance:DeleteAllEffect()

    SceneCtrl.Instance:CurScene():StopTouchMoving()
    MainPlayer:StopMove()
end

function TombRaiderFinishView:OnClickClose()
    ViewMgr:CloseView(TombRaiderFinishView)
    SceneCtrl.Instance:RequestLeaveFb()
end

function TombRaiderFinishView:CloseCallback()
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = nil
end

--============================TombFinishMember=========================================
TombFinishMember = TombFinishMember or DeclareMono("TombFinishMember", UIWidgetBaseItem)
function TombFinishMember:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local is_empty = self.data.uid == 0 

    UH.SetText(self.Name,is_empty and "" or data.play_name)
    UH.SetText(self.Feats,is_empty and "" or data.gongxian)
    UH.SetText(self.Result,is_empty and "" or data.add_score)

    self.is_mvp:SetActive(data.is_mvp)
end