MazeRunMainSub = MazeRunMainSub or DeclareMono("MazeRunMainSub", UIWFlushPanel)
-----WaitObj==========GameObject
-----RuningObj========GameObject
-----RuneObj==========GameObject
-----WaitTitle========Text
-----WaitContent========Text
-----WaitContentTM======UIWTimeMeter
-----RuningFloor========Text
-----RunningTime========UIWTimeMeter
-----RankList===========UIWidgetList[MazeRunSubRankItem]
function MazeRunMainSub:MazeRunMainSub()
    self.data_cares = {
        {data = SceneData.Instance:Base(), func = self.OnSceneModeChange, keys = {"scene_mode"}},
        {data = MazeRunData.Instance:WaitData(), func = self.FlushWaitInfo},
        {data = MazeRunData.Instance:RunningData(), func = self.FlushRunningInfo},
        {data = TeamData.Instance:GetTeamInfo(), func = self.FlushRuneObj},
        {data = MazeRunData.Instance.pass_count, func = self.FlushRewardTip, init_call = false }
    }
    self.language = Language.MazeRun
    self.pre_wait_content = Format(self.language.WaitContent, COLORSTR.Yellow1, "%d", COLORSTR.Yellow1, "%d", COLORSTR.Yellow1, "%s")
end

function MazeRunMainSub:FlushRuneObj()
    local cur_mode = SceneData.Instance:GetMode()
    if cur_mode == SceneModeType.MazeRun then
        self.RuneObj:SetActive(true)--TeamData.Instance:IsLeader())
    elseif cur_mode == SceneModeType.MazeRunWait then
        self.RewardTipObj:SetActive(false)
        MazeRunCtrl.Instance:ReqProc(MazeRunOperType.ROLE_INFO)
    end
end

function MazeRunMainSub:FlushRewardTip()
    self.RewardTipObj:SetActive(MazeRunData.Instance.pass_count.count ~= nil and MazeRunData.Instance.pass_count.count > 0)
end

function MazeRunMainSub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushRuneObj()
end

function MazeRunMainSub:OnSceneModeChange()
    local cur_mode = SceneData.Instance:GetMode()
    if cur_mode == SceneModeType.MazeRun then
        self.WaitObj:SetActive(false)
        self.RuningObj:SetActive(true)
        self:FlushRunningInfo()
        self.RuneObj:SetActive(true)--TeamData.Instance:IsLeader())
    elseif cur_mode == SceneModeType.MazeRunWait then
        self.WaitObj:SetActive(true)
        self.RuningObj:SetActive(false)
        self:FlushWaitInfo()
        self.RuneObj:SetActive(false)
    else
        self.RuneObj:SetActive(false)
    end
end

function MazeRunMainSub:FlushWaitInfo()
    local cur_mode = SceneData.Instance:GetMode()
    if cur_mode == SceneModeType.MazeRunWait then
        local wait_data = MazeRunData.Instance:WaitData()
        self.WaitTitle.text = Format(self.language.WaitTitle, wait_data.min_lv)
        local t = Format(self.pre_wait_content, wait_data.wait_num, 50, "%s")
        self.WaitContentTM:StampTime(wait_data.begin_timestamp, TimeType.Type_Special_0, t)
    end
end

function MazeRunMainSub:FlushRunningInfo()
    local cur_mode = SceneData.Instance:GetMode()
    if cur_mode == SceneModeType.MazeRun then
        local running_data = MazeRunData.Instance:RunningData()
        self.RuningFloor.text = Format(self.language.Floor, running_data.cur_floor)
        self.RankList:SetData(running_data.rank_list)
        if running_data.is_timing == false then
            self.RunningTime:CloseCountDownTime()
        else
            self.RunningTime:CreateRunStampTime(running_data.begin_timestamp)
        end
    end
end

function MazeRunMainSub:OnClickExitWait()
    PublicPopupCtrl.Instance:AlertTip(
        self.language.ExitConfirm,
        function()
            SceneCtrl.Instance:RequestLeaveFb()
            LoginCtrl.Instance:SendBackToOriginServer()
        end
    )
end

function MazeRunMainSub:OnClickRankBtn()
    ViewMgr:OpenView(MazeRunRankView)
end

function MazeRunMainSub:OnClickExitRuning()
    PublicPopupCtrl.Instance:AlertTip(
        self.language.ExitConfirm,
        function()
            SceneCtrl.Instance:RequestLeaveFb()
            LoginCtrl.Instance:SendBackToOriginServer()
        end
    )
end

function MazeRunMainSub:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[11].desc)
end

function MazeRunMainSub:OnClickRewardInfo()
    local reward_list = MazeRunData.Instance:GetAllRewardInfo()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward_list)
end

MazeRunSubRankItem = MazeRunSubRankItem or DeclareMono("MazeRunSubRankItem", UIWidgetBaseItem)
--Name======Text
--Floor=====Text
function MazeRunSubRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = Format("%d %s", data.rank, data.leader_name)
    if data.cur_layer == 0 then
        self.Floor.text = Language.MazeRun.StageClear
    else
        self.Floor.text = Format(Language.MazeRun.Floor, data.cur_layer)
    end
    if data.team_index == TeamData.Instance:TeamIndex() then
        self.Floor.color = COLORS.Green5
        self.Name.color = COLORS.Green5
    else
        self.Floor.color = COLORS.Yellow7
        self.Name.color = COLORS.Yellow7
    end
end

MazeRunMainSubRune = MazeRunMainSubRune or DeclareMono("MazeRunMainSubRune", UIWFlushPanel)
-----RuneInteractors====InterctorHandler[]
-----RuneNums===========Text[]
function MazeRunMainSubRune:MazeRunMainSubRune()
    self.data_cares = {
        {data = MazeRunData.Instance:RuneBag(), func = self.FlushRuneBag},
        {data = TeamData.Instance:GetTeamInfo(), func = self.FlushRuneTeamBag}
    }
    self.language = Language.MazeRun
end

function MazeRunMainSubRune:flushOneRune(rune_id, num)
    local num_txt = self.RuneNums[rune_id + 1]
    local interactor = self.RuneInteractors[rune_id + 1]
    if num_txt ~= nil then
        num_txt.text = tostring(num)
    end
    if interactor ~= nil then
        interactor.Interactable = num > 0
    end
end

function MazeRunMainSubRune:FlushRuneTeamBag()
    local rune_bag = MazeRunData.Instance:RuneBag()
    for _, rune_id in pairs(MazeRunData.Rune) do
        self:flushOneRune(rune_id, rune_bag[rune_id])
    end
end

function MazeRunMainSubRune:FlushRuneBag(changed_keys)
    local rune_bag = MazeRunData.Instance:RuneBag()
    if changed_keys ~= nil then
        for rune_id, _ in pairs(changed_keys) do
            self:flushOneRune(rune_id, rune_bag[rune_id])
        end
    else
        for _, rune_id in pairs(MazeRunData.Rune) do
            self:flushOneRune(rune_id, rune_bag[rune_id])
        end
    end
end

function MazeRunMainSubRune:OnClickRune(rune_id)
    MazeRunCtrl.Instance:RequestUseRune(rune_id)
end