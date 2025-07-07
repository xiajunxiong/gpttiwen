
TreasureTaskView = TreasureTaskView or DeclareView("TreasureTaskView", "treasure_task/" .. "treasure_task", Mod.TreasureTask.Main)
VIEW_DECLARE_LEVEL(TreasureTaskView, ViewLevel.L1)
VIEW_DECLARE_MASK(TreasureTaskView, ViewMask.BgBlockNoAnim)

function TreasureTaskView:TreasureTaskView()
    self.data = TreasureTaskData.Instance
end

function TreasureTaskView:LoadCallback()
end

function TreasureTaskView:CloseCallback()
    -- 如果是神秘大奖且则手动第一次关闭界面不寻路
    if 0 == self.data.result_info.param1 and not self.data.is_show_special_view then return end
    local task = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.TREASURE_TASK)
    if task and task:IsAutoGo() then
        task:GoTask()
    -- elseif RoleData.Instance:CreatedDay() <= 1 then
    --     TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
    end
end


function TreasureTaskView:OnClickClose()
    ViewMgr:CloseView(TreasureTaskView)
end

TreasureTaskViewF = TreasureTaskViewF or DeclareMono("TreasureTaskViewF", UIWFlushPanel)

function TreasureTaskViewF:TreasureTaskViewF()
    self.data = TreasureTaskData.Instance
    self.language = Language.TreasureTask

    self.data_cares = {
        {data = self.data.info_special, func = self.FlushSpecial, init_call = false},
    }

    self.rolling_index = 1

    self.SHOW_ITEM_MAX = 8
    self.INTERVAL_MIN = 0.05
    self.INTERVAL_MAX = 0.1
    self.INTERVAL_PER = 0.1
    self.WAIT_TIME = 1
    self.AUTO_CLOSE = 4
end

function TreasureTaskViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushRewards()
end

function TreasureTaskViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_rt)
    TimeHelper:CancelTimer(self.timer_dt)
    TimeHelper:CancelTimer(self.timer_ct)

    if 0 == self.data.result_info.param1 then
       if not self.data.is_show_special_view then
            ViewMgr:OpenView(TreasureTaskSpecialView)
       else
            self:ShowGiftView()
       end
    else
        self:ShowGiftView()
    end
end

function TreasureTaskViewF:FlushRewards()
    local list = self.data:GetTreasureTaskRewardList()
    for i = 1, self.ShowItems:Length() do
        if list[i] then
            self.ShowItems[i]:SetData(list[i])
        end
    end
    self.ShowItems[self.rolling_index]:SetSelected(true)

    if self.data.is_show_special_view then
        self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushSpecial, self), self.WAIT_TIME)
    else
        self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushDraw, self, self.data.result_info.param1), self.WAIT_TIME)
    end
    self.BtnStart.text = self.language.WaitTips
end

function TreasureTaskViewF:FlushDraw(index)
    self.BtnStart.text = self.language.WaitTips
    self:Rolling(index + 1, self.INTERVAL_MIN, false)
end

function TreasureTaskViewF:Rolling(result_index, interval, init_call)
    TimeHelper:CancelTimer(self.timer_rt)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        if self.rolling_index == result_index then
            TimeHelper:CancelTimer(self.timer_rt)
            if interval > self.INTERVAL_MAX then
                self.ShowItems[self.rolling_index]:SetSelected(true)
                self.ShowItems[self.rolling_index]:ShowSelEffect()
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
                self.BtnStart.text = self.language.ResultTips
                if 0 == self.data.result_info.param1 and not self.data.is_show_special_view then
                    ViewMgr:OpenView(TreasureTaskSpecialView)
                else
                    self:ShowGiftView()
                    self:AutoClose()
                end
                return
            end
            interval = interval + self.INTERVAL_PER
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
            self:Rolling(result_index, interval, false)
        else
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
        end
    end, interval, nil, init_call)
end

function TreasureTaskViewF:FlushSpecial()
    local param2 = self.data.result_info.param2
    if param2 > 0 then
        self:FlushDraw(self.data.result_info.param2)
    else
        self:ShowGiftView()
        self:AutoClose()
    end
end

function TreasureTaskViewF:ShowGiftView()
    if self.data.is_show_gift_view then return end
    self.data.is_show_gift_view = true
    if 0 == self.data.result_info.param1 then
        MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(self.data.result_info.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS, nil, nil, nil, 2, function ()
            self.data:ShowRewardAll()
        end)
    else
        PublicPopupCtrl.Instance:ShowCustom(DataHelper.FormatItemList(self.data.result_info.item_list))
        self.data:ShowRewardAll()
    end
end

function TreasureTaskViewF:AutoClose()
    self.TimerObj:SetActive(true)
    local cur_time = self.AUTO_CLOSE
    self.timer_ct = TimeHelper:AddCountDownTT(function ()
        cur_time = cur_time - 1
        self.TimerShow.text = string.format(self.language.AutoCloseShow, cur_time)
    end, function ()
        self.TimerShow.text = string.format(self.language.AutoCloseShow, 0)
        ViewMgr:CloseView(TreasureTaskView)
    end, self.AUTO_CLOSE, 1)
end

TreasureTaskViewRewardItem = DeclareMono("TreasureTaskViewRewardItem", UIWidgetBaseItem)

function TreasureTaskViewRewardItem:TreasureTaskViewRewardItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
end

function TreasureTaskViewRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Init(data.item)
    self.Cell:SetData(item)
    self.Name.text = item:Name()
end

function TreasureTaskViewRewardItem:RollingEnd(seq)
    if self.data and self.data.seq == seq then
        self:SetSelected(true)
    end
end

function TreasureTaskViewRewardItem:IsResultItem(seq)
    return self.data and self.data.seq == seq
end

function TreasureTaskViewRewardItem:ShowSelEffect()
    -- self.EffectTool:Play(6161015)
end

function TreasureTaskViewRewardItem:SetSelected(selected)
    UIWidgetBaseItem.SetSelected(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
    end
end

TreasureTaskSpecialView = TreasureTaskSpecialView or DeclareView("TreasureTaskSpecialView", "treasure_task/" .. "treasure_task_special", Mod.TreasureTask.Special)
VIEW_DECLARE_MASK(TreasureTaskSpecialView, ViewMask.BgBlockNoAnim)

function TreasureTaskSpecialView:LoadCallback()
    if MainPlayer then
        MainPlayer:StopMove()
    end
end

function TreasureTaskSpecialView:CloseCallback()

end

TreasureTaskSpecialViewF = TreasureTaskSpecialViewF or DeclareMono("TreasureTaskSpecialViewF", UIWFlushPanel)

function TreasureTaskSpecialViewF:TreasureTaskSpecialViewF()
    self.data = TreasureTaskData.Instance
    self.language = Language.TreasureTask
    self.data.is_show_special_view = true
end

function TreasureTaskSpecialViewF:Awake()
    UIWFlushPanel.Awake(self)

    local param2 = self.data.result_info.param2
    self.AllObj:SetActive(0 == param2)
    self.DoubleObj:SetActive(param2 > 0)

    TimeHelper:CancelTimer(self.timer_dt)
    self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickOpen, self), param2 > 0 and 3 or 30)
end

function TreasureTaskSpecialViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dt)
end

function TreasureTaskSpecialViewF:OnClickOpen()
    ViewMgr:CloseView(TreasureTaskSpecialView)
    if ViewMgr:IsOpen(TreasureTaskView) then
        self.data.info_special:Set{}
    else
        local param2 = self.data.result_info.param2
        if param2 > 0 then
            ViewMgr:OpenView(TreasureTaskView)
        else
            self.data.is_show_gift_view = true
            MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(self.data.result_info.item_list),GIFT_OPEN_TYPE.CONGRATULATIONS, nil, nil, nil, 2, function ()
                self.data:ShowRewardAll()
            end)
            local task = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.TREASURE_TASK)
            if task then
                task:GoTask()
            end
        end
    end
end


TreasureTaskExplorePanel = TreasureTaskExplorePanel or DeclareMono("TreasureTaskExplorePanel", UIWFlushPanel)
function TreasureTaskExplorePanel:TreasureTaskExplorePanel()
    self.data = TreasureTaskData.Instance
    self.data_cares = {
        {data = self.data.explore_data, func = self.OnShowChanged},
    }
    self.is_playing_eff = false
    self.is_can_open = false
end

function TreasureTaskExplorePanel:OnDestroy()
    TimeHelper:CancelTimer(self.timer_dt)
    UIWFlushPanel.OnDestroy(self)
end

function TreasureTaskExplorePanel:OnShowChanged()
    local flag = self.data.explore_data.is_show

    TimeHelper:CancelTimer(self.timer_dt)
    
    self.BtnCatch:SetActive(flag)
    
    if flag and not self.is_playing_eff then
        self.is_can_open = true
        self.KeepEffect:SetActive(true)
        self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickCache, self), 1)
    else
        self.is_can_open = false
        self.KeepEffect:SetActive(false)
    end
end

function TreasureTaskExplorePanel:OnClickCache()
    if not self.is_can_open then return end
    self.is_can_open = false
    
    self.EffectTool:Play("10170004")
    self.data:OpenBox()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    
    self.KeepEffect:SetActive(false)

    self.is_playing_eff = true
end

function TreasureTaskExplorePanel:EndCacheEffect()
    self.is_playing_eff = false
    TreasureTaskCtrl.Instance:SendTreasureTaskReqOpenBox()   
end


TreasureTaskRewardView = TreasureTaskRewardView or DeclareView("TreasureTaskRewardView", "treasure_task/" .. "treasure_task_reward")
VIEW_DECLARE_MASK(TreasureTaskRewardView, ViewMask.BgBlock)
function TreasureTaskRewardView:TreasureTaskRewardView()
end

function TreasureTaskRewardView:LoadCallback()
end

TreasureTaskRewardViewF = TreasureTaskRewardViewF or DeclareMono("TreasureTaskRewardViewF", UIWFlushPanel)
function TreasureTaskRewardViewF:TreasureTaskRewardViewF()
    self.data = TreasureTaskData.Instance
    self.lanuage = Language.TreasureTask
end
--箱子展示，倒计时自动打开（或手动打开） 播放箱子打开特效 然后播奖励展示
function TreasureTaskRewardViewF:onFlush()
    self.List:SetData(self.data.reward_list_all)
    self.handle_start = self.EffectTool:Play(6165115)
    UH.SetText(self.TipsText, self.lanuage.OpenBoxTip)
    self.handle_canopen = TimeHelper:AddDelayTimer(function ()
        self.OpenObj:SetActive(true)
    end, 2)
    self.auto_open_handle = TimeHelper:AddDelayTimer(function ()
        self:OnClickOpen()
    end,4)
end
function TreasureTaskRewardViewF:OnClickOpen()
    self.OpenObj:SetActive(false)
    if self.auto_open_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_open_handle)
    end
    self.auto_close_handle = TimeHelper:AddDelayTimer(function ()
        self:OnClickClose()
    end,10)
    if self.handle_start then
        self.EffectTool:Stop(self.handle_start)
    end
    self.EffectTool:Play(6165114)
    UH.SetText(self.TipsText, "")
    self.handle_showreward = TimeHelper:AddDelayTimer(function ()
        -- self.ContentAnim:Play()
        self.ContentObj:SetActive(true)
        -- for k,v in pairs(self.data.reward_list_all) do
        --     ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(v), num = v.num}}
        --     -- local big_type = Item.GetBagType(v.item_id)
        --     -- if big_type and big_type == ITEM_BIG_TYPE.VIRTUAL then
        --         PublicPopupCtrl.Instance:CenterI(Item.Create(v))
        --     -- else

        --     -- end
        -- end
    end, 1.5)
    self.handle_canclose = TimeHelper:AddDelayTimer(function ()
        UH.SetText(self.TipsText, self.lanuage.AnyCloseTip)
        self.CloseObj:SetActive(true)
    end, 2)
end
function TreasureTaskRewardViewF:OnClickClose()
    if self.auto_close_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_close_handle)
    end
    ViewMgr:CloseView(TreasureTaskRewardView)
end
function TreasureTaskRewardViewF:OnDestroy()
    TimeHelper:CancelTimer(self.handle_canopen)
    TimeHelper:CancelTimer(self.handle_showreward)
    TimeHelper:CancelTimer(self.auto_open_handle)
    TimeHelper:CancelTimer(self.auto_close_handle)
    UIWFlushPanel.OnDestroy(self)

    -- TimeHelper:AddDelayTimer(function ()
    --     SceneCtrl.Instance:RequestLeaveFb()
    -- end, 1)
end
