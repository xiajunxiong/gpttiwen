MainRightLoversMatchMenu = MainRightLoversMatchMenu or DeclareMono("MainRightLoversMatchMenu", UIWFlushPanel)
function MainRightLoversMatchMenu:MainRightLoversMatchMenu()
    self.data_cares = {
        {data = LoversMatchData.Instance.fb_info, func = self.FlushView},
    }
end

function MainRightLoversMatchMenu:FlushView()
    if not LoversMatchData.Instance:IsActOpen() or not SceneData.Instance:GetMode() == SceneModeType.LoversMatch then
        return
    end
    self:FlushView2()
end

function MainRightLoversMatchMenu:FlushView2()
    local fb_info = LoversMatchData.Instance.fb_info
    UH.SetText(self.Target, Format(Language.LoversMatch.MenuTarget, fb_info.succ_match))
    local is_open = not (fb_info.begin_game_time <= 0 or TimeHelper.GetServerTime() + 1 < fb_info.begin_game_time)
    if not is_open then --未开始
        self.Timer:SetTime(Language.LoversMatch.MenuTimeState[1])
    else
        self.Timer:StampTime(fb_info.end_game_time, TimeType.Type_Special_0, nil, Language.LoversMatch.MenuTimeState[2])
    end
    UH.SetText(self.Score, fb_info.integral)
    self:FlushTask()
end

function MainRightLoversMatchMenu:FlushTask()
    local fb_info = LoversMatchData.Instance.fb_info
    local time_cfg, index = LoversMatchData.Instance:GetNextTaskCfg()
    local is_open = not (fb_info.begin_game_time <= 0 or TimeHelper.GetServerTime() + 1 < fb_info.begin_game_time)
    if time_cfg ~= nil and is_open then
        self.BGRect.sizeDelta = Vector2.New(264, 288)
        self.Limit:SetActive(true)
        local prog_str = Format("（%s/%s）", ColorStr(fb_info.succ_match, COLORSTR.Red8), time_cfg.match_couple)
        UH.SetText(self.Task, Format(Language.LoversMatch.MenuTask, time_cfg.task_desc .. prog_str))
        if fb_info.reward_level == 0 then
            UH.SetText(self.Reward, Language.LoversMatch.MenuReward2)
        else
            UH.SetText(self.Reward, Format(Language.LoversMatch.MenuReward, Language.LoversMatch.MenuRewardLevel[fb_info.reward_level + 1]))
        end
        self.time_cfg = time_cfg
        local func_update = BindTool.Bind1(self.OnTimeUpdate, self)
        local func_finish = BindTool.Bind1(self.OnTimeFinish, self)
        local spend_time = TimeHelper.GetServerTime() - fb_info.begin_game_time
        self.play_time = time_cfg.play_time - spend_time + 1
        TimeHelper:CancelTimer(self.count_timer)
        self.count_timer = TimeHelper:AddCountDownTT(func_update, func_finish, self.play_time, 1, true)
    else
        self.BGRect.sizeDelta = Vector2.New(264, 140)
        self.Limit:SetActive(false)
        TimeHelper:CancelTimer(self.count_timer)
    end
end

function MainRightLoversMatchMenu:OnTimeUpdate()
    if self.time_cfg ~= nil then
        self.Progress:SetProgress(self.play_time / self.time_cfg.play_time)
        self.play_time = self.play_time - 1
    end
end

function MainRightLoversMatchMenu:OnTimeFinish()
    self:FlushTask()
end
