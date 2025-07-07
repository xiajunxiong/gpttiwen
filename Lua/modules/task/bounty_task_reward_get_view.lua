BountyTaskRewardGetView = BountyTaskRewardGetView or DeclareView("BountyTaskRewardGetView", "task/bounty_task_reward_get")

function BountyTaskRewardGetView:LoadCallback(data)
    UH.SetText(self.TitleTex, data.title or Language.Task.BountyTaskReward)
    self.close_func = data.close_func
end

function BountyTaskRewardGetView:CloseCallback()
    if self.close_func then
        Call(self.close_func)
    end
end

function BountyTaskRewardGetView:OnClickClose()
    ViewMgr:CloseView(BountyTaskRewardGetView)
end

BountyTaskRewardGetViewDetail = BountyTaskRewardGetViewDetail or DeclareMono("BountyTaskRewardGetViewDetail",UIWFlushPanel)

function BountyTaskRewardGetViewDetail:Awake()
    UIWFlushPanel.Awake(self)
    self.bounty_info = TaskData.Instance:GetBountyTaskData()
    self.BtnCloseObj:SetActive(false)
    self:FlushReward()
end

function BountyTaskRewardGetViewDetail:FlushReward()
    for i, reward in ipairs(self.bounty_info.reward_list) do
        if self.RewardList[i] then
            self.RewardList[i]:SetData(reward)
        end
    end
end

function BountyTaskRewardGetViewDetail:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.roll_run)
    self.roll_run = nil
end

function BountyTaskRewardGetViewDetail:OnClickGet()
    if self.BtnGetInter.Interactable == false then
        PublicPopupCtrl.Instance:Center(Language.Task.NoramlGetTip)
        return
    end
    self.BtnGetInter.Interactable = false
    self:ShowItemGet()
end

function BountyTaskRewardGetViewDetail:ShowItemGet()
    self.end_index = self.bounty_info.reward_idx + 1
    self.end_index = math.max(self.end_index, 1)
    self.end_index = math.min(self.end_index, 12)
    TimeHelper:CancelTimer(self.roll_run)
    self.roll_run = nil
    self.round_num = 1
    self:StartRoll(0.1, nil, function ()
        if self.pos_index == self.end_index then
            if self.round_num < 2 then
                self.round_num = self.round_num + 1
                return
            end
            TimeHelper:CancelTimer(self.roll_run)
            if self.bounty_info.reward_type == BountyTaskRewardGetType.TaskChain then
                TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.FETCH_REWARD)
            else
                TaskCtrl.Instance:SendBountyTaskTurnTableRewardFetch()
            end
            self.BtnCloseObj:SetActive(true)
        end
    end)
end

function BountyTaskRewardGetViewDetail:StartRoll(speed_rate, start_pos, jump_func)
    if self.roll_run == nil then
        self.pos_index = start_pos or 1
        self.roll_run = TimeHelper:AddRunTimer(function ()
            if self.cur_select_obj ~= nil then
                self.cur_select_obj:SetActive(false)
            end
            self.pos_index = self.pos_index > 12 and 1 or self.pos_index
            self.RewardList[self.pos_index].SelectObj:SetActive(true)
            self.cur_select_obj = self.RewardList[self.pos_index].SelectObj
            if jump_func ~= nil then
                jump_func()
            end
            self.pos_index = self.pos_index + 1
        end,speed_rate)
    end
end

BountyTaskRewardGetViewItem = BountyTaskRewardGetViewItem or DeclareMono("BountyTaskRewardGetViewItem", UIWidgetBaseItem)

function BountyTaskRewardGetViewItem:SetData(data)
    local item = Item.Create(data)
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
end