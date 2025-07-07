ConspiracyFinish = ConspiracyFinish or DeclareView("ConspiracyFinish", "conspiracy/conspiracy_finish")
VIEW_DECLARE_LEVEL(ConspiracyFinish, ViewLevel.L1)
VIEW_DECLARE_MASK(ConspiracyFinish, ViewMask.BgBlock)

ConspiracyFinishPanel = ConspiracyFinishPanel or DeclareMono("ConspiracyFinishPanel", UIWFlushPanel)
function ConspiracyFinishPanel:ConspiracyFinishPanel()
    self.data = ConspiracyData.Instance
    self.data_cares = {
        {data = self.data:GetFinishReward(), func = self.FlushReward}
    }
    self.handle_runer_list = {}
    self.req_reason = POKER_REQ_REASON_TYPE.TYPE_SELF --玩家or系统翻牌
end
function ConspiracyFinishPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    for index, value in ipairs(self.handle_runer_list) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_runer_list = {}
end
function ConspiracyFinishPanel:FlushReward()
    --LogError("翻牌奖励内容刷新了")
    --检查如果都点完了就 把其他的都打开
    local all_open = true
    --判断如果我还没有选那就按照顺序
    local list = self.data:GetFinishReward()
    for i, v in ipairs(list) do
        -- self.CellList[i]:SetData(Item.Create({item_id = v.reward_id, num = v.reward_count}))
        if v.index ~= 0 then
            self.CellList[v.index]:SetData(Item.Create({item_id = v.reward_id, num = v.reward_count, param = v.param}))
            self.NameList[v.index].text = v.role_name
            if v.fixed_reward.reward_id ~= 0 then
                self.ItemRewardList[v.index].gameObject:SetActive(true)
                UH.SpriteName(self.ItemRewardSp[v.index], v.fixed_reward.reward_id)
                self.ItemRewardList[v.index].text = string.format(Language.Conspiracy.ItemReward, v.fixed_reward.num)
            end
        end

        if v.role_id > 0 then
            if v.is_can_turn == 1 and v.is_turned == 0 then
                all_open = false
            else
                if v.index ~= 0 then
                    if self.ResultList[v.index].eulerAngles.y == 90 then
                        self.ItemAnim[v.index]:Play()
                        if self.data:IsSR(v.reward_id) then
                            table.insert(self.handle_runer_list, self:ShowEffect(self.ResultList[v.index]))
                        end
                        if self.data:IsSSR(v.reward_id) then
                            table.insert(self.handle_runer_list, self:ShowEffect(self.ResultList[v.index], true))
                        end
                    end
                end
            end
        end
    end
    if all_open == true and self.time_end == false then
        --全打开了
        self.handle_delay1 =
            TimeHelper:AddDelayTimer(
            function()
                self:OpenAll()
            end,
            self.data.other.close_countdown
        )
    end
end
function ConspiracyFinishPanel:ShowEffect(obj, ssr)
    local handle_runer = Runner.Instance:RunUntilTrue(function ()
        --if obj.eulerAngles.y == 0 then
            self.EffectTool:Play(ssr == true and "10010868" or "10010869", obj.gameObject)
            return true
        -- end
    end)
    return handle_runer
end
function ConspiracyFinishPanel:OpenAll()
    local list = self.data:GetFinishReward()
    for i = 1, 5 do
        if self.ResultList[i].eulerAngles.y == 90 then
            self.ItemAnim[i]:Play()
            if self.data:IsSR(list[i].reward_id) then
                table.insert(self.handle_runer_list, self:ShowEffect(self.ResultList[i]))
            end
            if self.data:IsSSR(list[i].reward_id) then
                table.insert(self.handle_runer_list, self:ShowEffect(self.ResultList[i], true))
            end
        end
    end
    self.Time:TotalTime(self.data.other.close_countdown, TimeType.Type_Special_5)
    self.Time:SetCallBack(nil)
    self.Time.complete_fun = nil
    self.handle_delay2 =
        TimeHelper:AddDelayTimer(
        function()
            ViewMgr:CloseView(ConspiracyFinish)
            self.data:FinishReward({})
        end,
        self.data.other.close_countdown
    )
end
function ConspiracyFinishPanel:onFlush()
    self.time_end = false
    local list = self.data:GetFinishReward()
    for i = 1, 5 do
        self.CellList[i]:SetData(Item.Create({item_id = list[i].reward_id, num = list[i].reward_count, param = list[i].param}))
        self.NameList[i].text = ""
        self.ItemRewardList[i].gameObject:SetActive(false)
    end
    --15秒
    self.Time:TotalTime(self.data.other.choose_countdown, TimeType.Type_Special_5)
    self.Time:SetCallBack(
        function()
            self.time_end = true
            --倒计时结束如果我还没点就自动点一个
            if not self:IsSelfSelect() then
                for i = 1, 5 do
                    if self.ResultList[i].eulerAngles.y == 90 then
                        self.req_reason = POKER_REQ_REASON_TYPE.TYPE_SYSTEM
                        self:OnClickOpen(i)
                        break
                    end
                end
            end
            self.handle_delay3 =
                TimeHelper:AddDelayTimer(
                function()
                    self:OpenAll()
                end,
                self.data.other.close_countdown
            )
        end
    )
end
function ConspiracyFinishPanel:OnClickOpen(index)
    local list = self.data:GetFinishReward()
    local id = RoleData.Instance:GetRoleId()
    local reward = nil
    for i = 1, 5 do
        if list[i] ~= nil and list[i].role_id == id then
            reward = list[i]
        end
    end
    if reward ~= nil and reward.is_can_turn == 1 and reward.is_turned == 0 and reward.index == 0 then
        ConspiracyCtrl.Instance:MiYiPokerReq(self.req_reason, index)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[152])
    end
end

function ConspiracyFinishPanel:IsSelfSelect()
    local list = self.data:GetFinishReward()
    local id = RoleData.Instance:GetRoleId()
    for i = 1, 5 do
        if list[i] ~= nil and list[i].role_id == id then
            if list[i].is_can_turn == 1 and list[i].is_turned == 0 then
                return false
            end
            return true
        end
    end
    return true
end
