DailyGuildTask = DailyGuildTask or DeclareView("DailyGuildTask", "daily_activity/daily_guild_task", Mod.GuildTask.Main)

-- 存放在self.lang依然是访问lua table，对应一个存放在堆heap的hash table，与直接访问Language.Guild实际上换汤不换药，而local是放到lua的堆栈stack中的，访问更快
-- local lang = Language.Guild

function DailyGuildTask:DailyGuildTask()
end

function DailyGuildTask:LoadCallback()
    self.Board:SetData(self:GetType(), Language.DailyGuildTask.Title, 967)
end

DailyGuildTaskFlushView = DailyGuildTaskFlushView or DeclareMono("DailyGuildTaskFlushView", UIWFlushPanel)

function DailyGuildTaskFlushView:DailyGuildTaskFlushView()
    DailyGuildTaskFlushView.view_data = SmartData.New()
    self.data_cares = {
        {data = GuildData.Instance.guild_task_list, func = self.FlushList, init_call = true},
        -- {data = GuildData.Instance.guild_task_list, func = self.FlushDetail, init_call = false},
        {data = DailyGuildTaskFlushView.view_data, func = self.FlushDetail, init_call = false}
    }
    -- LogError("DailyGuildTaskFlushView")
end

function DailyGuildTaskFlushView:FlushList()
    -- LogError("FlushList")
    -- self.TaskList:SetData({{id = 1}, {id = 2}})
    self.TaskList:SetData(GuildData.Instance:GetGuildTaskShowList())
    -- self.TaskList:ClickItem(1)
    self.TaskList:SetectItem(1, true)
    -- self:FlushDetail()
end

function DailyGuildTaskFlushView:OnDestroy()
    -- LogError("DailyGuildTaskFlushView:OnDestroy()")
    TaskCtrl.Instance:CSTaskTraceStatus()
    UIWFlushPanel.OnDestroy(self)
end

function DailyGuildTaskFlushView:FlushDetail()
    -- LogError("FlushDetail")
    -- LogError(DailyGuildTaskFlushView.view_data)
    local data = DailyGuildTaskFlushView.view_data
    if not data then
        return
    end
    -- if not data.id then
    --     return
    -- end
    local cfg = GuildData.Instance.guild_task_cfg.task[data.id]
    if not cfg then
        return
    end

    local monster_cfg = Cfg.MonsterById(cfg.c_param1)
    local monster_name = monster_cfg.name
    local scene_name = SceneData.Instance:GetSceneCfg(cfg.target_scene).name
    local goal_desc = string.format(Language.DailyGuildTask.GoalDesc, cfg.c_param2, monster_name)
    local task_desc = string.format(Language.DailyGuildTask.TaskDesc, scene_name, cfg.c_param2, monster_name)
    -- LogError(cfg)
    self.TaskName.text = cfg.task_name
    self.TaskDesc.text = task_desc

    local goal =
        string.format(
        "%s(%d/%d)",
        goal_desc,
        data.status == DAILY_GUILD_TASK_STATUS.FINISHED and cfg.c_param2 or math.min(data.progress, cfg.c_param2),
        -- data.progress,
        cfg.c_param2
    )

    self.TaskTarget.text = goal
    -- local reward_list = {}

    -- -- local temp = {}
    -- local mission_reward_cfg = GuildData.Instance.guild_task_cfg.mission_reward[RoleData.Instance:GetRoleLevel()]
    -- if mission_reward_cfg then
    --     table.insert(reward_list, Item.New({item_id = CommonItemId.Exp, num = mission_reward_cfg.exp}))
    --     table.insert(reward_list, Item.New({item_id = CommonItemId.Coin, num = mission_reward_cfg.coin}))
    -- end
    -- local group_list = string.split(cfg.reward_show, "|")
    -- for index, value in ipairs(group_list) do
    --     local item_struct = string.split(value, ":")
    --     local item_id = tonumber(item_struct[1])
    --     if item_id ~= CommonItemId.Exp and item_id ~= CommonItemId.Coin then
    --         table.insert(reward_list, Item.New({item_id = item_id, num = item_struct[2]}))
    --     end
    -- end

    -- self.reward_list[value.level][value.num].reward_list = reward_list
    -- self.reward_list[value.level][value.num].num = value.num

    -- local group_id_list = string.split(cfg.reward_group, "|")
    -- for _, str_id in pairs(group_id_list) do
    --     local group_id = tonumber(str_id)
    --     for k, v in pairs(GuildData.Instance.guild_task_cfg.reward_group) do
    --         if v.group_id == group_id then
    --             local item = Item.New()
    --             item.item_id = v.reward.item_id
    --             item.is_bind = v.reward.is_bind == 1
    --             item.num = 1
    --             table.insert(reward_list, item)
    --         end
    --     end
    -- end
    self.TaskDropList:SetData(TaskInfoGuild.RewardList(data.id))

    self.target_scene = nil
    if data.status == DAILY_GUILD_TASK_STATUS.FINISHED then
        self.ButtonIHs[3].Interactable = false
        self.ButtonIHs[4].Interactable = false
        self.AcceptText.text = Language.DailyGuildTask.Finished
    elseif data.status == DAILY_GUILD_TASK_STATUS.ACCEPTABLE then
        self.ButtonIHs[3].Interactable = true
        self.ButtonIHs[4].Interactable = true
        self.AcceptText.text = Language.DailyGuildTask.Accept
    else
        if data.progress >= cfg.c_param2 then
            self.ButtonIHs[3].Interactable = false
            self.ButtonIHs[4].Interactable = true
            self.AcceptText.text = Language.DailyGuildTask.Commit
        else
            self.target_scene = cfg.target_scene
            self.ButtonIHs[3].Interactable = true
            self.ButtonIHs[4].Interactable = true
            self.AcceptText.text = Language.DailyGuildTask.GoTo
        end
    end
end

function DailyGuildTaskFlushView:FindRewards()
    ViewMgr:OpenViewByKey(Mod.Activity.ActivityBack)
end

function DailyGuildTaskFlushView:AcceptAll()
    for i = 0, GuildData.Instance:GetGuildTaskNum() - 1 do
        GuildCtrl.Instance:CSGuildTaskReq(0, i)
    end
end

function DailyGuildTaskFlushView:QuicklyFinish()
    local info = {
        content = string.format(Language.DailyGuildTask.QuickFinish, GuildData.Instance.guild_task_cfg.other[1].quick_finish),
        confirm = {
            func = function()
                GuildCtrl.Instance:CSGuildTaskReq(1, GuildData.Instance:GetGuildTaskIndexById(DailyGuildTaskFlushView.view_data.id))
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }

    PublicPopupCtrl.Instance:DialogTips(info)
    -- local param_t = {
    --     size = Vector2.New(503, 253),
    --     title_str = string.format(
    --         Language.DailyGuildTask.QuickFinish,
    --         GuildData.Instance.guild_task_cfg.other[1].quick_finish
    --     ),
    --     -- on_click_cancel = function ()
    --     --     LogError("OnClickCancel")
    --     -- end,
    --     on_click_ok = function()
    --         GuildCtrl.Instance:CSGuildTaskReq(
    --             1,
    --             GuildData.Instance:GetGuildTaskIndexById(DailyGuildTaskFlushView.view_data.id)
    --         )
    --         ViewMgr:CloseView(UIWAlertInstance)
    --     end
    -- }

    -- WidgetInstanceCtrl.Instance:OpenAlert(param_t)
    -- self.Alert:SetData(self:GetType(), Vector2.New(503, 253))
    -- self.Alert:OpenAlert(param_t)
    -- GuildCtrl.Instance:CSGuildTaskReq(1, GuildData.Instance:GetGuildTaskIndexById(DailyGuildTaskFlushView.view_data.id))
end

function DailyGuildTaskFlushView:Accept()
    -- LogError("Accept")
    if self.target_scene then
        local data = DailyGuildTaskFlushView.view_data
        local cfg = {
            behavior_id = 9999,
            behavior_type = 9,
            param1 = self.target_scene,
            param2 = 822,
            param3 = ""
        }
        BehaviorCtrl.ExeByCfgVo(cfg)
        TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] = CUSTOM_OBJID.GUILD_TASK_BEGIN + data.id
        ViewMgr:CloseView(DailyGuildTask)
    else
        GuildCtrl.Instance:CSGuildTaskReq(0, GuildData.Instance:GetGuildTaskIndexById(DailyGuildTaskFlushView.view_data.id))
    end
end

DailyGuildTaskItem = DailyGuildTaskItem or DeclareMono("DailyGuildTaskItem", UIWidgetBaseItem)

function DailyGuildTaskItem:DailyGuildTaskItem()
    -- body
end

function DailyGuildTaskItem:SetData(data)
    -- LogError("DailyGuildTaskItem", data)
    self.data = data
    local cfg = GuildData.Instance.guild_task_cfg.task[data.id]
    -- local task =
    --     string.format(
    --     "%s（%d/%d）",
    --     cfg.task_name,
    --     data.status == DAILY_GUILD_TASK_STATUS.FINISHED and cfg.condition or data.progress,
    --     -- data.progress,
    --     cfg.condition
    -- )
    local monster_cfg = Cfg.MonsterById(cfg.c_param1)
    local monster_name = monster_cfg.name
    local goal_desc = string.format(Language.DailyGuildTask.GoalDesc, cfg.c_param2, monster_name)
    local goal =
        string.format(
        "%s(%d/%d)",
        goal_desc,
        -- data.status == DAILY_GUILD_TASK_STATUS.FINISHED and cfg.condition or data.progress,
        data.status == DAILY_GUILD_TASK_STATUS.FINISHED and cfg.c_param2 or math.min(data.progress, cfg.c_param2),
        -- data.progress,
        cfg.c_param2
    )

    UH.SetIcon(self.Icon, monster_cfg.icon_id, ICON_TYPE.ITEM)

    self.Task1.text = cfg.task_name
    self.Descr1.text = goal
    self.Task2.text = cfg.task_name
    self.Descr2.text = goal

    self.TakenText = Language.DailyGuildTask.Accepted
    self.Taken:SetActive(data.status == DAILY_GUILD_TASK_STATUS.ACCEPTED)
    self.FinishedText = Language.DailyGuildTask.Finished
    self.Finished:SetActive(data.status == DAILY_GUILD_TASK_STATUS.FINISHED)
    -- if data.status == DAILY_GUILD_TASK_STATUS.ACCEPTED then
    --     self.Taken:SetActive(true)
    --     self.TakenText = Language.DailyGuildTask.Accepted
    -- elseif data.status == DAILY_GUILD_TASK_STATUS.FINISHED then
    --     self.Taken:SetActive(true)
    --     self.TakenText = Language.DailyGuildTask.Finished
    -- else
    --     self.Taken:SetActive(false)
    -- end
end

function DailyGuildTaskItem:SetSelected(selected)
    if self.Selector ~= nil then
        self.Selector.isOn = selected
    end
    self.is_selected = selected
    self:OnSelect(selected)
end

function DailyGuildTaskItem:OnSelect(selected)
    -- LogError("OnSelect", self.data)
    DailyGuildTaskFlushView.view_data:Set(self.data)
    UIWidgetBaseItem.OnSelect(self, selected)
    DailyGuildTaskFlushView.view_data:Notify()
end

function DailyGuildTaskItem:Click()
    self.ClickEvent:Trigger()
end
