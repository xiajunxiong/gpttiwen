CoolBreezeView = CoolBreezeView or DeclareView("CoolBreezeView","serveractivity/cool_breeze", Mod.SummerFirst.CoolBreeze)
VIEW_DECLARE_LEVEL(CoolBreezeView, ViewLevel.L1)
function CoolBreezeView:CoolBreezeView()
    CoolBreezeView.play_effect = false
end

function CoolBreezeView:OnClickClose()
    ViewMgr:CloseView(CoolBreezeView)
end

CoolBreezeViewF = CoolBreezeViewF or DeclareMono("CoolBreezeViewF", UIWFlushPanel)

function CoolBreezeViewF:CoolBreezeViewF()
    self.data = ServerActivityCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.CoolBreeze

    self.data_cares = {
        {data = self.act_data.activity_status_event, func = self.FlushState},
        {data = self.data.cool_breeze_data, func = self.FlushShow},
    }

    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(function ()
        if self.AnimOpen:get_Item("cool_breeze").normalizedTime >= 0.72 then
            self.RectMask.enabled = true
            return true
        end
        return false
    end)
end

function CoolBreezeViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRemainTime()
end

function CoolBreezeViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
end

function CoolBreezeViewF:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RAND_COOL_BREEZE then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COOL_BREEZE)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(CoolBreezeView)
    end
end

function CoolBreezeViewF:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COOL_BREEZE)
    local end_time = activity_status and activity_status.param_2 or 0
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
        return
    end
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(self.language.RemainTime, ft.hour + 24 * ft.day, ft.min, ft.s)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
    end, end_time, 1)
end

function CoolBreezeViewF:FlushShow()
    local show_list = self.data:GetCoolBreezeShowList()
    for i = 1, self.ShowItems:Length() do
        if show_list[i] then
            self.ShowItems[i]:SetData(show_list[i])
            self.ShowItems[i]:SetObjActive(true)
        else
            self.ShowItems[i]:CancelTimer()
            self.ShowItems[i]:SetObjActive(false)
        end
    end
    self.ListPos.sizeDelta = Vector2.New(#show_list > 3 and 1360 or 1234, 580)
    self.HLayoutGroup.spacing = #show_list > 3 and 8 or 50
    self.ScrollRect.enabled = #show_list > 3
end

CoolBreezeViewItem = DeclareMono("CoolBreezeViewItem", UIWidgetBaseItem)

function CoolBreezeViewItem:CoolBreezeViewItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    self.language = Language.CoolBreeze
end

function CoolBreezeViewItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    self:CancelTimer()
end

function CoolBreezeViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.InfoObj:SetActive(data.type > 0)
    self.WaitObj:SetActive(0 == data.type)
    self.EmptyObj:SetActive(-1 == data.type)
    TimeHelper:CancelTimer(self.timer_ct)
    if 0 == data.type then
        local cbd = self.act_data.cool_breeze_data
        local end_time = cbd.task_refresh_timestamp_min
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local last_time = end_time - TimeHelper.GetServerTime()
            local ft = TimeHelper.FormatDHMS(last_time)
            self.WaitTime.text = string.format(self.language.RemainTimeW, ft.hour + 24 * ft.day, ft.min, ft.s)
        end, function ()
            self.WaitTime.text = string.format(self.language.RemainTimeW, 0, 0, 0)
        end, end_time, 1)
    else
        local task = Cfg.CoolBreezeCoByType(data.type)
        if task then
            local is_complete = data.schedule >= task.param_1
            self.Name.text = task.task_name
            self.Desc.text = string.format(self.language.DescShow, task.progress_desc, math.min(data.schedule, task.param_1), task.param_1) 
            if nil == task.rewards then
                local rewards = {}
			    for i = 0, #task.reward_item do
                    local item_data = task.reward_item[i]
                    table.insert(rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
			    end
                task.rewards = rewards
            end
            self.Rewards:SetData(task.rewards)
            self.FixeObj:SetActive(1 == task.task_group and 0 == data.refresh_num)
            self.RandomObj:SetActive(2 == task.task_group)
            if 2 == task.task_group then
                local cbd = self.act_data.cool_breeze_data
                local end_time = cbd.next_random_task_refresh_timestamp + (cbd.last_refresh_random_open_day == data.open_day and (60 * 60 * 24) or 0)
                self.timer_ct = TimeHelper:AddCountDownCT(function ()
                    local last_time = end_time - TimeHelper.GetServerTime()
                    local ft = TimeHelper.FormatDHMS(last_time)
                    self.RemainTime.text = string.format(self.language.RemainTimeR, ft.hour + 24 * ft.day, ft.min, ft.s)
                end, function ()
                    self.RemainTime.text = string.format(self.language.RemainTimeR, 0, 0, 0)
                end, end_time, 1)
            end
            self.task_info = task
            self.BtnGoObj:SetActive(not is_complete)
            self.BtnGetObj:SetActive(is_complete)
            if data.play_effect then
                data.play_effect = false
                self.EffectTool:Play(6165030)
            end
        end
    end
end

function CoolBreezeViewItem:CancelTimer()
    TimeHelper:CancelTimer(self.timer_ct)
end

function CoolBreezeViewItem:OnClickGo()
    ViewMgr:OpenViewByKey(self.task_info.mod_key)
    ViewMgr:CloseView(SummerFirstView)
    if 0 == self.task_info.is_return then
        ViewMgr:CloseView(CoolBreezeView)
    end
end

function CoolBreezeViewItem:OnClickGet()
    ServerActivityCtrl.Instance:SendRACoolBreezeCommit(self.data.index, self.task_info.task_group - 1)
end

function CoolBreezeViewItem:OnClickNew()
    ViewMgr:OpenView(CoolBreezeTipsView, {index = self.data.index})
end


CoolBreezeTipsView = CoolBreezeTipsView or DeclareView("CoolBreezeTipsView","serveractivity/cool_breeze_tips")

function CoolBreezeTipsView:CoolBreezeTipsView()
    self.language = Language.CoolBreeze
end

function CoolBreezeTipsView:LoadCallback(param_t)
    self.index = param_t.index
    local co = Config.cool_summer_auto.other[1]
    self.Content.text = string.format(self.language.RefreshCost, co.gold)
end

function CoolBreezeTipsView:OnClickConfirm()
    CoolBreezeView.play_effect = true
    ServerActivityCtrl.Instance:SendRACoolBreezeRefresh(self.index)
    ViewMgr:CloseView(CoolBreezeTipsView)
end

function CoolBreezeTipsView:OnClickCancel()
    ViewMgr:CloseView(CoolBreezeTipsView)
end