LookingForRabbits = LookingForRabbits or DeclareView('LookingForRabbits', 'looking_for_rabbits/looking_for_rabbits')
VIEW_DECLARE_LEVEL(LookingForRabbits, ViewLevel.L0)
VIEW_DECLARE_MASK(LookingForRabbits, ViewMask.None)
LookingForRabbitsPanel = LookingForRabbitsPanel or DeclareMono('LookingForRabbitsPanel', UIWFlushPanel)
function LookingForRabbitsPanel:LookingForRabbitsPanel()
    self.data = LookingForRabbitsData.Instance
    self.data_cares = {
        {data = self.data:Game(), func = self.GameFlush, keys = {'pass_idx'}},
        {data = self.data:Game(), func = self.GameState, keys = {'state'}},
        {data = self.data:Game(), func = self.QiPaoShow, keys = {'state'}},
        {data = self.data:Game(), func = self.GameFlush, keys = {'state'}}
        -- {data = self.data.change, func = self.QiPaoShow, keys = {'init'}},
    }
end
--打开界面时开始倒计时3秒，然后5秒进行交换，之后开始游戏,选择结果后重新倒计时3秒
function LookingForRabbitsPanel:onFlush()
    --打开界面的时候场景已经加载完成了
    self:GameFlush()
end
function LookingForRabbitsPanel:OnClickOut()

    local info = {
        content = Language.LFRabbits.OutTip,
        confirm = {
            func = function()
                -- PublicPopupCtrl.Instance:CloseDialogTips()
                -- ViewMgr:CloseView(LookingForRabbits)
                -- SceneCtrl.Instance:RequestLeaveFb()
                LookingForRabbitsCtrl.StartGame()
                LookingForRabbitsCtrl.ReqResult(false)
                LookingForRabbitsCtrl.EndGame()
            end
        },
    }
    PublicPopupCtrl.Instance:DialogTips(info)
    -- SceneCtrl.Instance:RequestLeaveFb()
end
function LookingForRabbitsPanel:QiPaoShow()
    local info = self.data:Game()
    local idxs = self.data:GetRightIdxs()
    local state = self.data:Game().state
    if state == -0.9 then
        self:QiPaoHide()
        if idxs then
            for i, v in ipairs(idxs) do
                self.QiPaoTexts[v].text = self.data:RamdonBullet(state)
                self.QiPaoItems[v]:SetActive(true)
            end
        end
    elseif state == -0.8 then
        self:QiPaoHide()
    elseif state == 0.1 then
        self:QiPaoHide()
        for i, v in ipairs(idxs) do
            self.QiPaoTexts[v].text = self.data:RamdonBullet(state)
            self.QiPaoItems[v]:SetActive(true)
        end
    elseif state == 0.3 then
        self:QiPaoHide()
        local selects = self.data:SelectIdxs()
        for i, v in ipairs(selects) do
            self.QiPaoTexts[v].text = self.data:RamdonBullet(state)
            self.QiPaoItems[v]:SetActive(true)
        end
    elseif state == 1.1 then
        self:QiPaoHide()
        for i, v in ipairs(idxs) do
            self.QiPaoTexts[v].text = self.data:RamdonBullet(state)
            self.QiPaoItems[v]:SetActive(true)
        end
    end
end
function LookingForRabbitsPanel:QiPaoHide()
    for i = 1, self.QiPaoItems:Length() do
        self.QiPaoItems[i]:SetActive(false)
        self.QiPaoTexts[i].text = ''
    end
end
function LookingForRabbitsPanel:GameState()
    self:FlushFollow()
    local info = self.data:Game()
    local state = self.data:Game().state
    if state == -0.9 then
        TimeHelper:CancelTimer(self.handle_wait)
        self.data:ClearSelect()
        self.notify_start = false
        UH.SetText(self.Tip, Language.LFRabbits.WillStart)
        local start = self.data:StartTime()
        self.Time.text = start
        TimeHelper:CancelTimer(self.handle_start)
        self.handle_start =
            TimeHelper:AddRunTimer(
            function()
                if start < 1 then
                    self.Time.text = 'S'
                    if not self.notify_start then
                        self.data:ClearSelect()
                        self.data:Game().state = -0.8
                        self.notify_start = true
                    end
                else
                    self.Time.text = start
                end
                start = start - 1
                self:FlushFollow()
            end,
            1,
            self.data:StartTime() + 2
        )
    elseif state == 0 then
        UH.SetText(self.Tip, Language.LFRabbits.NeedFind)
        local num = 10
        self.Time.text = num
        TimeHelper:CancelTimer(self.handle_wait)
        self.handle_wait =
            TimeHelper:AddCountDownCT(
            function()
                if num >= 0 then
                    self.Time.text = num
                end
                num = num - 1
            end,
            function()
                self:Overtime()
            end,
            info.choose_tamp,
            1,
            true
        )
    elseif state == 0.1 then --胜利
        TimeHelper:CancelTimer(self.handle_wait)
        UH.SetText(self.Tip, Language.LFRabbits.ResultRight)
    elseif state == 0.2 or state == 1.1 then
        TimeHelper:CancelTimer(self.handle_wait)
        TimeHelper:CancelTimer(self.handle_end)
        self.handle_end =
        TimeHelper:AddDelayTimer(
        function()
            LookingForRabbitsCtrl.EndGame()
        end,
        2)
    end
end
function LookingForRabbitsPanel:GameFlush()
    local state = self.data:Game().state
    if state ~= -1 and state ~= -0.9 then
        return
    end
    local cur_level = self.data:CurLevel()
    local reward_level = self.data:RewardLevel()
    UH.SetText(self.CurLevel, cur_level)
    if reward_level == nil then
        UH.SetText(self.RewardLevel, Language.LFRabbits.FinalReward)
    else
        UH.SetText(self.RewardLevel, reward_level)
    end
    if self.data:IsOutCount() then
        UH.SetText(self.OutCountTip, Language.LFRabbits.OutCount)
    else
        UH.SetText(self.OutCountTip, "")
    end
    UH.SetText(self.ItemName, self.data:PassRewardDesc())
    self.RewardList:SetData(self.data:ShowReward())
end
function LookingForRabbitsPanel:FlushFollow()
    local objs = self.data:GetPosObj()
    if objs ~= nil then
        for i = 1, objs:Length() do
            self.FollowItems[i].Target = objs[i].transform
        end
    end
end
function LookingForRabbitsPanel:OnDestroy()
    TimeHelper:CancelTimer(self.handle_start)
    TimeHelper:CancelTimer(self.handle_wait)
    UIWFlushPanel.OnDestroy(self)
end
function LookingForRabbitsPanel:ChangeRabbits()
    --根据当前关卡选择使用3 4 5个格子的功能
end
function LookingForRabbitsPanel:OnClickSelect(index)
    local state = self.data:Game().state
    if state ~= 0 and state ~= 0.3 then
        return
    end
    local idxs = self.data:GetRightIdxs()
    if idxs ~= nil then
        if #idxs == 1 then
            if idxs[1] == index then
                LookingForRabbitsCtrl.ReqResult(true)
            else
                LookingForRabbitsCtrl.ReqResult(false)
            end
        else
            local select_idxs = self.data:SelectIdxs()
            if self:HadRabbit(index) then
                if not self:HadSelect(index) then
                    self.data:SetSelect(index)
                    UH.SetText(self.Tip, Language.LFRabbits.NeedSelect)
                else
                    return
                end
                if #idxs == #select_idxs then
                    LookingForRabbitsCtrl.ReqResult(true)
                    return
                end
                self.data:Game().state = 0.3
            else
                LookingForRabbitsCtrl.ReqResult(false)
            end
        end
    end
end
function LookingForRabbitsPanel:HadSelect(index)
    local selects = self.data:SelectIdxs()
    for i, v in ipairs(selects) do
        if v == index then
            return true
        end
    end
    return false
end
function LookingForRabbitsPanel:HadRabbit(idx)
    local idxs = self.data:GetRightIdxs()
    if idxs ~= nil then
        for i, v in ipairs(idxs) do
            if v == idx then
                return true
            end
        end
    end
    return false
end

function LookingForRabbitsPanel:Overtime()
    --超时退出副本且展示结算
    --[[  SceneCtrl.Instance:RequestLeaveFb()
    ViewMgr:CloseView(LookingForRabbits)
    local info = self.data:Game()
    LogDG('结果 :', info) ]]
end
