MedalTaskView = MedalTaskView or DeclareView("MedalTaskView", "medal/medal_task_view", Mod.MedalTask.Main)
VIEW_DECLARE_LEVEL(BagView, ViewLevel.L1)
function MedalTaskView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(MedalTaskView)
end

MedalTaskFlushPanel = MedalTaskFlushPanel or DeclareMono("MedalTaskFlushPanel", UIWFlushPanel)

function MedalTaskFlushPanel:MedalTaskFlushPanel()
    self.data = MedalTaskData.Instance

    --[[  self.TaskList:SetData(self.data:GetTaskList())

    self.data_cares = {
        {data = self.data.view_data, func = self.FlushRightInfo,keys = {"cur_select_main_task"} },
    }

    self.data:SetCurSelectMainTask(self.data:GetTaskList()[1])

    self.cur_task = nil ]]
    self.next_index = 0
    self.medal_type = {
        BattleCinematics.Type.FaZhenWeiJi, --开始答题
        BattleCinematics.Type.ZhengYiShiMing, --开始答题
        BattleCinematics.Type.ShouChaoLaiXi, --开始答题
        BattleCinematics.Type.XuanDuZhiWei, --开始答题
        BattleCinematics.Type.XiuFuShenZhu, --开始答题
        BattleCinematics.Type.MoZhuBaoDong --开始答题
    }
end
function MedalTaskFlushPanel:OnDestroy(params)
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    TimeHelper:CancelTimer(self.handle_delay4)
end
function MedalTaskFlushPanel:onFlush()
    self.desc_split_list = {}
    self.task_data = self.data:GetTaskList()
    self.max_index = table.nums(self.task_data)
    local level = RoleData.Instance:GetRoleLevel()
    local select_index
    for index, value in ipairs(self.task_data) do
        self.NameList[index].text = value.name
        if index == self.max_index then
            self.LockList[index].text = string.format(Language.MedalTask.LockTip2, value.level)
        else
            self.LockList[index].text =
                string.format(Language.MedalTask.LockTip, value.level, DataHelper.GetDaXie(index))
        end
        self.InterList[index].Interactable = level >= value.level
        local doing, finish = self:CheckTaskStatus(value)
        if select_index == nil then
            if doing then
                select_index = index
            end
        end
        self.FinishList[index]:SetActive(finish)
    end
    --选中默认进行中
    if select_index ~= nil then
        self:OnClickShow(select_index)
    end
end
function MedalTaskFlushPanel:OnClickGo()
    --[[ local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.medal)
    if task then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
        ViewMgr:CloseView(MedalTaskView)
    end ]]
    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.medal)
    local data = self.task_data[self.select_index]
    if TaskData.Instance:GetTaskIsCompleted(data.special_taskid2) == true then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
        ViewMgr:CloseView(MedalTaskView)
        return
    end
    if TaskData.Instance:GetTaskIsCompleted(data.special_taskid1) ~= true then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
        ViewMgr:CloseView(MedalTaskView)
        return
    end
    PublicPopupCtrl.Instance:BattleCinematics(
        self.medal_type[self.select_index],
        1,
        nil,
        function()
            MedalTaskCtrl.Instance:SendMedalTaskPlayVideo(self.select_index)
            ViewMgr:CloseView(MedalTaskView)
        end,
        1.1
    )
end
function MedalTaskFlushPanel:OnClickShow(index)
    self.select_index = index
    local data = self.task_data[index]
    local level = RoleData.Instance:GetRoleLevel()
    if level < data.level then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
        return
    end
    local show = true
    if self.cur_anim == string.format("show%s", index) then
        self.cur_anim = "default"
        show = false
    else
        TimeHelper:CancelTimer(self.handle_delay1)
        if self.next_index ~= (index + 1) then
            local next_index = self.next_index
            local next_item = self.ItemList[next_index]
            self.handle_delay1 =
                TimeHelper:AddDelayTimer(
                function()
                    if next_item then
                        self.LockObjList[next_index]:SetActive(false)
                        next_item:SetSiblingIndex(next_index)
                    end
                end,
                0.5
            )
        end
        self.cur_anim = string.format("show%s", index)
        TimeHelper:CancelTimer(self.handle_delay2)
        self.handle_delay2 =
            TimeHelper:AddDelayTimer(
            function()
                --显示描述
                local desc_list
                if self.desc_split_list[index] then
                    desc_list = self.desc_split_list[index]
                else
                    desc_list = string.split(data.description, "|")
                    self.desc_split_list[index] = desc_list
                end
                for i = 1, 8 do
                    self.DescList[i].text = desc_list[i]
                end
                if index == self.max_index then
                    self.TitleTex.text = Language.MedalTask.TitleTip2
                else
                    self.TitleTex.text = string.format(Language.MedalTask.TitleTip, DataHelper.GetDaXie(index))
                end
                --显示解锁
                self.TipTex.text = string.format(Language.MedalTask.TaskTip, data.name, data.scene_name)
                local doing, finish = self:CheckTaskStatus(data)
                self.BtnGo:SetActive(doing)
                self.Finish:SetActive(finish)
            end,
            0.5
        )
    end
    self.next_index = index + 1
    TimeHelper:CancelTimer(self.handle_delay3)
    self.handle_delay3 =
        TimeHelper:AddDelayTimer(
        function()
            --将下一幕移到最上边
            local next_item = self.ItemList[self.next_index]
            if next_item then
                if show then
                    next_item:SetAsLastSibling()
                else
                    next_item:SetSiblingIndex(self.next_index)
                end
                local next_data = self.task_data[self.next_index]
                if level < next_data.level and show then
                    self.LockObjList[self.next_index]:SetActive(true)
                else
                    self.LockObjList[self.next_index]:SetActive(false)
                end
            end
        end,
        0.5
    )
    TimeHelper:CancelTimer(self.handle_delay4)
    self.handle_delay4 =
        TimeHelper:AddDelayFrameTimer(
        function()
            self.MedalAnim:SetTrigger(APH(self.cur_anim))
        end,
        2
    )

    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.MenuOpen)
end
function MedalTaskFlushPanel:CheckTaskStatus(data)
    local flag_is_complete = true
    local flag_is_doing = false
    for k, v in pairs(data.task_list) do
        if not TaskData.Instance:GetTaskIsCompleted(v.task_id) then
            flag_is_complete = false
        end

        if TaskData.Instance:GetTaskIsAccepted(v.task_id) then
            flag_is_doing = true
        end
    end
    return flag_is_doing, flag_is_complete
end

--[[ function MedalTaskFlushPanel:FlushRightInfo()
    local cur_select = self.data:GetCurSelectMainTask()
    local cur_task = nil
    local is_accepted = true
    for k,v in pairs(cur_select.task_list) do
        if TaskData.Instance:GetTaskIsAccepted(v.task_id)  then 
            cur_task = v
        end 
    end

    if cur_task == nil then 
        is_accepted = false
        cur_task = cur_select.task_list[1]
    end 

    self.cur_task = TaskData.Instance:GetTaskInfoByTaskId(cur_task.task_id)

    local is_unstart = self.cur_task == nil

    local is_complete = true
    for k,v in pairs(cur_select.task_list) do 
        if not TaskData.Instance:GetTaskIsCompleted(v.task_id) then 
            is_complete = false
        end 
    end 
    self.TaskComplete:SetActive(is_complete)
    
    local monster_cfg = Cfg.MonsterById(cur_select.monster_id)
    UH.SetIcon(self.TaskTarget, monster_cfg.icon_id, ICON_TYPE.ITEM)

    local medal_cfg = Item.GetConfig(cur_select.medal_id)
    local item_ = Item.Create({item_id = medal_cfg.id})
    self.MedalIcon:SetData(item_)

    self.MedalName.text = medal_cfg.name

    self.TaskName.text = cur_task.task_name
    self.TaskCurProg.text = cur_select.description -- cur_task.progress_desc
    
    local str_1 = ""
    local str_2 = ""
    local str_sp = ""

    local attr_list = item_:CfgAttr()
    local index = 1
    for k,v in pairs(attr_list) do 
        if v > 0 then 
            local str = string.format(Language.MedalTask.Medal_Attr, 
            Language.Common.AttrList[k], 
            DataHelper.IsPercent(k) and Percent(v) or v,"")
            if index % 2 == 0 then 
                str_1 = str_1..str .. "\n"
            else 
                str_2 = str_2..str .. "\n"
            end  
            index = index + 1
        end 
    end 

    self.MedalAttrStr.text = str_2
    self.MedalAttr2Str.text = str_1

    local cfg = Item.GetConfig(item_.item_id)
    str_sp = string.format(Language.ItemInfo.ItemTitles.EffectOtE, COLORSTR.Purple4, Equip.GetEffectDesc(cfg.sp_id))

    self.MedalSpecAttrStr.text = str_sp

    local operate_str = ""

    if is_complete then 
        operate_str = Language.MedalTask.OperaterComplete
    elseif is_accepted then 
        operate_str = Language.MedalTask.OperaterDoing
    elseif is_unstart then 
        operate_str = Language.MedalTask.OperaterUnStart
    else 
        operate_str = Language.MedalTask.OperaterAccept
    end 

    self.OperateText.text = operate_str
    self.OperateInteractor.Interactable = not is_complete and not is_unstart
end ]]

--[[ function MedalTaskFlushPanel:OnClickTips()
    ViewMgr:OpenView(MedalTaskHelpView)
    -- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = "\n\n\n"})
end
 ]]
--[[ function MedalTaskFlushPanel:OnClickOperate()
    -- TaskCtrl.Instance:GoTask(self.cur_task)

    AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self.cur_task)}, 1)
    ViewMgr:CloseView(MedalTaskView)
end
 ]]
--[[ MedalListTaskItem = MedalListTaskItem or DeclareMono("MedalListTaskItem", UIWidgetBaseItem)

function MedalListTaskItem:SetData(data)
    -- body

    self.data = data 
 
    self.TaskTitle.text = self.data.name
    self.TaskLevel.text = string.format( Language.MedalTask.Item_level,self.data.level ) 
    UH.SetIcon(self.TaskIcon, self.data.medal_id, ICON_TYPE.ITEM)
    self:CheckTaskStatus()
    if data.index == 1 then 
        self.SelectEffect.isOn = true 
    end 
    self.LowLevel:SetActive(self.data.level > RoleData.Instance:GetRoleLevel() )
end

function MedalListTaskItem:CheckTaskStatus()
    local flag_is_complete = true
    local flag_is_doing = false

    for k,v in pairs(self.data.task_list) do
        if not TaskData.Instance:GetTaskIsCompleted(v.task_id) then 
            flag_is_complete = false
        end 

        if TaskData.Instance:GetTaskIsAccepted(v.task_id)  then 
            flag_is_doing = true
        end 
    end 

    self.DoingObj:SetActive(flag_is_doing and not flag_is_complete)
    self.CompleteObj:SetActive(flag_is_complete)

end

function MedalListTaskItem:OnClick()
    MedalTaskData.Instance:SetCurSelectMainTask(self.data)
end

MedalTaskProgItem = MedalTaskProgItem or DeclareMono("MedalTaskProgItem", UIWidgetBaseItem)

function MedalTaskProgItem:SetData(data)
    -- body
end
 ]]
