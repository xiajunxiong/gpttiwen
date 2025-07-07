
TreasureTaskData = TreasureTaskData or BaseClass()

function TreasureTaskData:__init()
    if TreasureTaskData.Instance ~= nil then
        Debuger.LogError("[TreasureTaskData] attempt to create singleton twice!")
        return
    end
    TreasureTaskData.Instance = self
    self:TreasureTaskConfig()
    self:TreasureTaskData()

    Cfg.Register("TreasureTaskRewardCoordinateBySeq",Cfg.RealTimeSearch(Config.treasure_task_auto.reward_coordinate, "seq"))
end

function TreasureTaskData:TreasureTaskConfig()
    self.config = Config.treasure_task_auto
    self.config_task_list = self.config.task_list
    self.config_turntable = self.config.turntable
    self.config_other = self.config.other[1]
    self.task_info = self.config_task_list[next(self.config_task_list)]

    self.reward_coordinate_scene_list = {}
    for k, v in pairs(Config.treasure_task_auto.reward_coordinate) do
        self.reward_coordinate_scene_list[v.scene] = self.reward_coordinate_scene_list[v.scene] or {}
        table.insert(self.reward_coordinate_scene_list[v.scene], v)
    end
end

function TreasureTaskData:TreasureTaskData()
    self.info_sm = SmartData.New()
    self.info_special = SmartData.New{}
    self.explore_data = SmartData.New{is_show = false}

    self.effect_list = {}
end

function TreasureTaskData:ClearData()
    self.info_sm:Set{}
    TimeHelper:CancelTimer(self.timer_rt)
    self.explore_data.is_show = false
    self.show_turntable = false
    self.reward_list_all_show = false
end

function TreasureTaskData:ConfigOtherQuantity()
    return self.config_other.quantity
end

function TreasureTaskData:SetTreasureTaskInfo(protocol)
    local info = protocol.info
    local task_id = self.info_sm.task_id or -1
    self.info_sm_pre = self.info_sm:Val()
    self.info_sm:Set(info)
    if info.task_id > 0 and info.task_id <= self:ConfigOtherQuantity() then
        local task = TaskData.Instance.task_list[CUSTOM_OBJID.TREASURE_TASK] or SmartData.New(TaskInfoTreasureTask.New():TaskInfo())
        local progress = string.format("(%s/%s)", info.task_id, self:ConfigOtherQuantity())
        task:TaskVo(info, self.task_info, progress)
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TREASURE_TASK, task, progress)
        if 1 == info.task_id - task_id and not self.show_turntable then
            task:GoTask()
        end
    else
        TaskData.Instance:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TREASURE_TASK)
    end
    TimeHelper:CancelTimer(self.timer_rt)
    self.explore_data:Set{is_show = false}
    if info.pos_id > 0 then
        local co = Cfg.TreasureTaskRewardCoordinateBySeq(info.pos_id)
        self.timer_rt = TimeHelper:AddRunTimer(function ()
            local is_show = false
			if SceneCtrl.Instance:CurScene() and SceneCtrl.Instance:CurScene().PosToServer and MainPlayer then
                local main_x, main_y = SceneCtrl.Instance:CurScene():PosToServer(MainPlayer:GetPos())
                is_show = GameMath.GetDistance(co.x, co.y, main_x, main_y, false) < 100
			end
            self.explore_data.is_show = is_show
        end, 0.5)
    end
    if self.show_turntable then
        self.show_turntable = false
        self.is_show_gift_view = false
        self.is_show_special_view = false
        ViewMgr:CloseView(GiftOpenView)
        ViewMgr:OpenView(TreasureTaskView)
    end
end

function TreasureTaskData:AccpetGoTask()
    local task_id = self.info_sm.task_id
    if 0 == task_id then
        TreasureTaskCtrl.Instance:SendTreasureTaskReqAccpet()
    else
        local task = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.TREASURE_TASK)
        if task then
            task:GoTask()
        end
    end
end

function TreasureTaskData:IsBoxShow(seq)
    return (self.info_sm.pos_id or 0) == seq
end

function TreasureTaskData:OpenBox()
    local pos_id = self.info_sm.pos_id
    local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.TreasureTaskObj, pos_id)
    local obj = SceneCtrl.Instance:Objs().client_objs[obj_id]
    if obj ~= nil then
        local effect_handle1 = SE:Play(SceneEffectConfig.EffectList.BoxTreasureTaskEffect1 , obj:GetPos())
        local effect_handle2 = SE:Play(SceneEffectConfig.EffectList.BoxTreasureTaskEffect2 , obj:GetPos())
        self.effect_list[obj_id] = {eff1 = effect_handle1, eff2 = effect_handle2}
        -- table.insert(self.effect_list, effect_handle1)
        -- table.insert(self.effect_list, effect_handle2)
        obj:OpenBox()
    end
end

function TreasureTaskData:ClearCacheBoxTreasureTask()
    for k, v in pairs(self.effect_list) do 
        if v.eff1 ~= nil then
            SE:Stop(v.eff1)
        end
        if v.eff2 ~= nil then
            SE:Stop(v.eff2)
        end
    end 
    self.effect_list = {}
end

function TreasureTaskData:DelBoxEffByObjId(obj_id)
    if self.effect_list[obj_id] ~= nil then
        if self.effect_list[obj_id].eff1 then
            SE:Stop(self.effect_list[obj_id].eff1)
        end
        if self.effect_list[obj_id].eff2 then
            SE:Stop(self.effect_list[obj_id].eff2)
        end
        self.effect_list[obj_id] = nil
    end
end

function TreasureTaskData:SetResultInfo(protocol)
    self.show_turntable = true
    self.result_info = protocol
end

function TreasureTaskData:SetResultInfoAll(protocol)
    local list1, list2 = {}, {}
    for _, item in ipairs(protocol.item_list) do
        if list1[item.item_id] then
            list1[item.item_id].num = list1[item.item_id].num + item.num
        else
            list1[item.item_id] = item
        end
    end
    for _, item in pairs(list1) do
        table.insert(list2, item)
    end
    self.reward_list_all = DataHelper.FormatItemList(list2)
    self.reward_list_all_show = true
end

function TreasureTaskData:GetTreasureTaskRewardList()
    local pool = self.info_sm_pre.pool
    local level = self.info_sm_pre.level or 1
    local reward_list = {}
    for k, v in ipairs(self.config_turntable) do
        if v.level_min <= level and (level <= v.level_max or 0 == v.level_max) then
            if pool and pool[v.seq] then
                table.insert(reward_list, v)
            end
        end
    end
    if pool then
        table.sort(reward_list, function (a, b)
            if not a or not b then return a end
            local as = pool[a.seq]
            local bs = pool[b.seq]
            return as < bs
        end)
    end
    return reward_list
end

function TreasureTaskData:GetRewardCoordinateBySceneId(scene_id)
    return self.reward_coordinate_scene_list[scene_id]
end

function TreasureTaskData:ShowRewardAll()
    if self.reward_list_all_show then
        self.reward_list_all_show = false
	    ViewMgr:OpenView(TreasureTaskRewardView)
    end
end

