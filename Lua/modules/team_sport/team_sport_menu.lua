
TeamSportMenu = TeamSportMenu or DeclareMono("TeamSportMenu",UIWFlushPanel)


function TeamSportMenu:TeamSportMenu()
    self.data_cares = {
        {data = TeamSportData.Instance.join_data,func = self.FlushPanel},
        {data = TeamSportData.Instance.score_data,func = self.FlushPanel},
    }
    self:FlushPanel()
end

function TeamSportMenu:ClickMatch()
    ViewMgr:OpenView(TeamSportView)
end

function TeamSportMenu:OnOpenRewardInfoClick()
    local reward_list = TeamSportData.Instance:GetFormatAllReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward_list)
end

function TeamSportMenu:FlushPanel()
    local flag = TeamSportData.Instance:GetJoinRewardStatus()
    local box_flag = TeamSportData.Instance:GetBoxAllRewardStatus()
    self.RewardBtnEffectObj:SetActive(flag == 0 or box_flag == 0)
end