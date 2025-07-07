FILE_REQ("modules/treasure_map/treasure_map_data")
VIEW_REQ("modules/treasure_map/treasure_map_view")
VIEW_REQ("modules/treasure_map/treasure_map_show")
TreasureMapCtrl = TreasureMapCtrl or BaseClass(BaseCtrl)
TREASURE_MAP_REQ_TYPE = {
    INFO = 0,
    ACCPET = 1,
    GIVE_UP = 2,
    BATCH_USE = 3,
}
function TreasureMapCtrl:__init()
    if TreasureMapCtrl.Instance ~= nil then
        Debuger.LogError("[TreasureMapCtrl] attempt to create singleton twice!")
        return
    end
    TreasureMapCtrl.Instance = self
    self.data = TreasureMapData.New()
    self:RegisterProtocol(CSTreasureMapReq)
    self:RegisterProtocol(SCTreasureMapInfo, "RecvTreasureMapInfo")
    self:RegisterProtocol(SCTreasureMapItemInfo, "RecvTreasureMapItemInfo")
    self:RegisterProtocol(SCTreasureMapItemInfoOpenResult, "RecvTreasureMapItemInfoOpenResult")
    self:RegisterProtocol(SCTreasureMapBatchUsedResult, "RecvTreasureMapBatchUsedResult")

    BehaviorData.Instance:CustomNpcTalkBtnsSeq(102, BindTool.Bind(self.SetupBehaviourBtn, self))
    FinishData.Instance:FailRegister(
        BATTLE_MODE.BATTLE_MODE_SUPER_MAP,
        {
            desc_func = function()
                return self.data:GetFailTip()
            end
        }
    )
    ActivityData.Instance:RegisterCondition(
        ActType.TreasureMap,
        function()
            return Format(Language.SuperMap.ActivityMapNum, self.data.get_reward_time, self.data.other_info.daily_times)
        end
    )
    EventSys:Bind(
        GlobalEventKeys.BattleEnd,
        function()
            if self.data:IsMapMonster(BattleData.Instance:GetBossGroupId()) == true then
                TimeHelper:AddDelayTimer(function ()
                    -- self.data.last_is_sp_map = 1
                    self.data:UseTreasure()
                end,1)
            end
        end)
end
function TreasureMapCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.id == 40107 and btn_list then
        for i, v in ipairs(btn_list) do
            if v.detail_info ~= nil then
                self:SetupBehaviourBtnDetail(btn_list[i])
                break
            end
        end
    end
end
function TreasureMapCtrl:SetupBehaviourBtnDetail(params)
    local info = RewardData.Instance:GetFindConfig(FindType.TreasureMap)
    local find_info = self.data:FindInfo()
    local cost = find_info.find_type == 0 and info.coin_price or info.gold_price
    cost = cost * self.data:Time()
    --对第一个按钮进行修改
    params.detail_info.act_type = ActType.TreasureMap
    params.detail_info.custom_type = DetailInfo1Custom.back
    params.detail_info.custom_show = Language.Task.TaskBtn.OneKey
    --NpcDialogData.GetDetailInfoBtnBackName(self.data:Cost() * self.data:Time())

    if self.data:Time() == 0 then
        params.detail_info.custom_func = function()
            PublicPopupCtrl.Instance:Center(ErrorText[73])
        end
    else
        params.detail_info.custom_func =
            BindTool.Bind(
            ViewManager.OpenView,
            ViewMgr,
            DialogTipsBackView,
            {
                cost = cost,
                cost_type = find_info.find_type,
                name = Language.Task.TreasureMap,
                rewards = TreasureMapData.Instance:Reward(),
                 --params.detail_info.rewards,
                confirm_func = BindTool.Bind(TaskCtrl.SendOnekeyFinish, TaskCtrl.Instance, OnekeyFinishType.TreasureMap)
            }
        )
    end
end
function TreasureMapCtrl:RecvTreasureMapInfo(protocol)
    -- 包含任务信息
    -- 如果有id 转换成vo去生成
    -- 打完清理old vo
    self.data:setGetRewarTime(protocol.get_reward_times)
    self.data:disposeScene()
    --先清理
    local vo = CommonStruct.MonsterVo(protocol)
    vo.npc_type_t = NpcHelper.NpcTypeStruct()
    vo.npc_type_t.monster = true
    vo.npc_type_t.elite_monster = true
    --[[ vo.is_boss = 2
    vo.icon_type = 2 ]]
    vo.tit_name = ""
    --Language.NpcDialog.NpcSpecialTitle[2]
    self.data:SetMonsterInfo(vo)
    self.data.cache_have_task = protocol.task_id
    if protocol.task_id > 0 then
        vo.obj_id = CUSTOM_OBJID.TREASURE_MAP
        vo.behavior_type = 12
        vo.param1 = BATTLE_MODE.BATTLE_MODE_TREASURE_MAP
        self.data:initScene(SceneData.Instance:SceneId())
        local task = self.data:TaskInfo()
        --一个smdata
        --local task_info = TaskCtrl.Instance.data:GetTaskInfoByTaskId(CUSTOM_OBJID.TREASURE_MAP)
        local taskvo = self.data:TaskVo(vo)
        local task_info = TaskInfoMap.New()
        task:Set(task_info:TaskInfo())
        task:TaskVo(taskvo, self.data.cache_task_id ~= 0 and protocol.pos_y ~= self.data.cache_task_id)
        task:IsNew(protocol.is_new ~= 0)
        TaskCtrl.Instance.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TREASURE_MAP, task)
        EventSys:UnBind(self.handle_event)
        if ViewMgr:IsOpen(MainView) and self.data.cache_task_id == 0 and protocol.task_id > 0 then
            --在102附近
            local param_t = CommonStruct.CustomTalk()
            table.insert(
                param_t.talk_list,
                {
                    talker = 40107,
                    talk_text = taskvo.talk_desc
                }
            )
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
            self.handle_event =
                EventSys:Bind(
                GlobalEventKeys.CloseDialogView,
                function()
                    task_info:GoTask()
                end
            )
        end

        self.data.cache_task_id = protocol.pos_y
    else
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TREASURE_MAP)
    end
    ActivityData.Instance:SetActivityFinishInfo(
        ActType.TreasureMap,
        protocol.get_reward_times,
        self.data.other_info.daily_times
    )
end

function TreasureMapCtrl:RecvTreasureMapItemInfo(protocol)
end

function TreasureMapCtrl:SendTreasureMapReq(param_t)
    local protocol = GetProtocol(CSTreasureMapReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    protocol.param3 = param_t.param3 or 0
    SendProtocol(protocol)
end

function TreasureMapCtrl:AutoTask()
    TimeHelper:AddDelayFrameTimer(
        function()
            SceneLogic.Instance:AutoToNpc(self.data:MonsterInfo(), nil, true, true)
        end,
        1
    )
end

function TreasureMapCtrl:RecvTreasureMapItemInfoOpenResult(protocol)
    local param_t = {seq = protocol.seq, index = protocol.index, item_id = protocol.item_id}
    if Item.GetConfig(param_t.item_id).is_sp_map == 3  then
		local item_data = StarTreasureMapData.Instance:GetRewardItem(param_t)
		if item_data ~= nil then
			ViewMgr:OpenView(StarTreasureMapRollerView,item_data)
		end
    elseif Item.GetConfig(param_t.item_id).is_sp_map == 4 then
        local item_data = StarTreasureMapData.Instance:GetReward2Item(param_t)
		if item_data ~= nil then
			ViewMgr:OpenView(ShengQiTreasureMapRollerView,item_data)
		end
    else
        self.data:SetMapResult(param_t)
	end
end
-- 2176000
function TreasureMapCtrl:SendUseItem(item)
    BagCtrl.Instance:SendUseItem({column = item.column_type, index = item.index})
end

function TreasureMapCtrl:GetNextItem(item)
    for i, v in ipairs(BagData.Instance:GetItemsByItemId(item.item_id)) do
        if v.param.client_need_uid1 == item.param.client_need_uid1 then
            return v
        end
    end
    return item
end

--宝图快捷弹窗使用
function TreasureMapCtrl:KeyUseItem(is_supermap, item)
    self.data:SetLastMapType(Item.GetConfig(item.item_id).is_sp_map)
    if is_supermap == 1 then
        -- 右下角使用新高级藏宝图，如果背包有的话
        if self.data:GetNewFlag() then
            self.data:SetNewFlag(false)
            SceneLogic.Instance:AutoToPos(item.param.scene_id, nil, function()
                SceneLogic.Instance:AutoToPos(item.param.scene_id, item.param, function ()
                    SceneLogic.Instance:ClearMoveCache()
                end, true)
                self.data:SetMapInfo(self:GetNextItem(item))
                ViewMgr:OpenView(Compass)
            end, true, true)
            return
        end
        if item.param.monster_id > 0 and Item.GetNum(26080) == 0 then
            PublicPopupCtrl.Instance:AlertTip(
                Language.SuperMap.MapTip,
                function()
                    self:SendUseItem(item)
                    ViewMgr:CloseView(Compass)
                    MainOtherCtrl.Instance:CloseKeyUse()
                end
            )
        else
            if item.param.is_rand_map == 0 then
                self:SendUseItem(item)
                ViewMgr:CloseView(Compass)
                return
            end
            -- LogDG("服务端预知宝图物品", item.param)
            if item.param.param_type == 2 or item.param.param_type == 3 then
                local data = self.data:Content0(item.param.param2)
                if next(data.item) == nil then
                    self:SendUseItem(item)
                    ViewMgr:CloseView(Compass)
                    return
                end
                local temp = Item.Create(data.item)
                -- local temp = Item.Init(item.param.param1, item.param.param2, item.param.param3)
                local color = temp:Color()
                local target_pos = SceneCtrl.Instance:CurScene():PosToClint(item.param.x, item.param.y)
                if color > 2 and color < 6 then
                    SE:Play(SceneEffectConfig.EffectList["SuperMapEffect"..(color - 2)], target_pos)
                    MainOtherCtrl.Instance:OpenGather(
                        {
                            seq = 0,
                            desc = 4,
                            time = 3,
                            finish_func = function()
                                self:SendUseItem(item)
                                ViewMgr:CloseView(Compass)
                                TimeHelper:AddDelayTimer(function()
                                    self.data:UseTreasure()
                                end, 1)
                            end
                        }
                    )
                else
                    self:SendUseItem(item)
                    ViewMgr:CloseView(Compass)
                end
            else
                self:SendUseItem(item)
                ViewMgr:CloseView(Compass)
            end
        end
    elseif is_supermap == 3 then
        self:SendUseItem(item)
        StarTreasureMapData.Instance:DeleteEffect(item.param.client_need_uid1)
    elseif is_supermap == 4 then
        MainOtherCtrl.Instance.stop = true
        self:SendUseItem(item)
        StarTreasureMapData.Instance:DeleteEffect(item.param.client_need_uid1)
    else
        ChatCtrl.Instance.stop = true
        MainOtherCtrl.Instance.stop = true
        PublicPopupCtrl.Instance.stop = true

        MainOtherCtrl.Instance:OpenGather(
            {
                seq = 0,
                desc = 4,
                time = 1,
                finish_func = function()
                    if next(self.data:ViewData().map) then
                        ViewMgr:OpenView(TreasureMapView)
                    end
                end
            }
        )
        self:SendUseItem(self:GetNextItem(item))
    end
end

function TreasureMapCtrl:RecvTreasureMapBatchUsedResult(protocl)
    local huobi = {type = CurrencyType.CoinBind, num = protocl.coin}
    if protocl.coin == 0 then
        huobi = nil
    end
    AllSoulsStorybookCtrl.Instance:OpenRewardView(protocl.rewards, AllSoulsRewardType.Sample, AllSoulsRewardTitleType.GetReward, false, huobi)
end