TreasureMapData = TreasureMapData or BaseClass()
function TreasureMapData:__init()
    if TreasureMapData.Instance ~= nil then
        Debuger.LogError("[TreasureMapData] attempt to create singleton twice!")
        return
    end
    TreasureMapData.Instance = self
    self.monster_info = nil
    self.task_info = SmartData.New()
    self.treasure_map_auto = Config.treasure_map_auto
    self.other_info = self.treasure_map_auto.other[1]
    self.reward_find = self.treasure_map_auto.reward_find
    self.reward_group = self.treasure_map_auto.reward_group
    self.monster_group = self.treasure_map_auto.monster_group
    self.content_info = self.treasure_map_auto.content
    self.treasure_map_config = self.treasure_map_auto.treasure_map_config
    self.monster_list = {}
    self.content_list = {}
    self.monster_group_list = {}
    self:init_config()
    self.cache_task_id = 0
    self.get_reward_time = 0
    self.cache_have_task = 0
    self.view_data = SmartData.New({map = {}, val = false, item_list = {}})
    self.scene_effect = {}
    -- 用于判断使用的是新的高级藏宝图
    self.new_flag = false
    
end

function TreasureMapData:AddEffect(client_uid,obj)
    if self.scene_effect[client_uid] then
        SE:Release(self.scene_effect[client_uid])
    end
    self.scene_effect[client_uid] = obj
end

function TreasureMapData:DeleteEffect(client_uid)
    if self.scene_effect[client_uid] then
        SE:Release(self.scene_effect[client_uid])
    end
    self.scene_effect[client_uid] = nil
end
function TreasureMapData:init_config()
    for i, v in ipairs(self.monster_group) do
        self.monster_list[v.monster_id] = v
    end
    for i, v in ipairs(self.content_info) do
        if self.content_list[v.seq] == nil then
            self.content_list[v.seq] = {}
        end
        if v.monster_id > 0 then
            self.monster_group_list[v.monster_id] = true
        end
        table.insert(self.content_list[v.seq], v)
    end
    self:CacheForgeCompose()
end

function TreasureMapData:SetMonsterInfo(info)
    if self.monster_list[info.id] then
        info.original_id = info.id
        info.id = self.monster_list[info.id].show_monster
    end
    self.monster_info = info
end

function TreasureMapData:MonsterInfo()
    return self.monster_info
end

function TreasureMapData:TaskInfo()
    return self.task_info
end
function TreasureMapData:ViewData()
    return self.view_data
end
function TreasureMapData:ClearResult()
    self.view_data.map = {}
    self.view_data.item_list = {}
end
function TreasureMapData:SetMapResult(info)
    self.view_data.map = info
    --0是普通高级宝图，11是特殊的高级藏宝图，里面只有一个高级必杀
    if info.seq == 0 or info.seq == 11 then
        local data = self:Content0(info.index, info.seq)
        if data.is_rare == 1 and next(data.item) ~= nil then
            local item = Item.Create(data.item)
            ViewMgr:OpenView(TreasureMapShow, {item = item})
            BagCtrl.Instance:SendHearsayInfoReq(0,item.item_id,info.item_id,data.is_notice)
            return
        end
    end
    self.view_data.item_list = {}
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
        self.handle_run = nil
    end
    self.handle_run =
        Runner.Instance:RunUntilTrue(
        function()
            local num = GameMath.Ramdon(1, self.other_info.max_index)
            local role_level = RoleData.Instance:GetRoleLevel()
            local temp
            for i, v in ipairs(self.content_list[info.seq]) do
                if role_level >= v.min_level and role_level <= v.max_level and v.index == num then
                    temp = v
                    break
                end
            end
            if temp and temp.is_show == 1 --[[ and role_level >= temp.min_level and role_level <= temp.max_level  ]] then
                table.insert(self.view_data.item_list, temp)
            end
            if table.nums(self.view_data.item_list) == 12 then
                return true
            end
            return false
        end
    )
end

function TreasureMapData:initScene(scene_id)
    if self.monster_info and self.monster_info.id > 0 and self.monster_info.scene_id == scene_id then
        VoMgr:AddMonster(self.monster_info)
    end
end

function TreasureMapData:disposeScene()
    VoMgr:DelMonster(CUSTOM_OBJID.TREASURE_MAP)
end

function TreasureMapData:TaskVo(vo)
    local scene_cfg = SceneData.Instance:GetSceneCfg(vo.scene_id)
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TREASURE_MAP
    taskvo.task_name = Language.Task.TaskName.Map
    taskvo.task_goal =
        string.format(Language.Task.TaskGoal.Map, scene_cfg.name, Cfg.MonsterById(vo.id).name)
    taskvo.task_goal_menu =
        string.format(Language.Task.TaskGoal.MapMenu, scene_cfg.name, Cfg.MonsterById(vo.id).name)
    taskvo.task_desc =
        string.format(Language.Task.TaskGoal.Map2, Cfg.MonsterById(vo.id).name, scene_cfg.name)
    taskvo.talk_desc =
        string.format(Language.Task.TaskGoal.Map3, Cfg.MonsterById(vo.id).name, scene_cfg.name)
    return taskvo
end

function TreasureMapData:SetMapInfo(item)
    self.treasure_item = item
end

function TreasureMapData:MapInfo()
    return self.treasure_item
end

--[[ function TreasureMapData:NextMapItem()
    if self.treasure_item then
        return BagData.Instance:TreasureMapItem(self.treasure_item)
    end
end ]]
function TreasureMapData:ClearItem()
    self.treasure_item = nil
end

function TreasureMapData:GetActivityBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(self.reward_find, self.reward_group)
end

function TreasureMapData:setGetRewarTime(time)
    self.get_reward_time = time
end

function TreasureMapData:IsFinishTask()
    return self.other_info.daily_times == self.get_reward_time
end
function TreasureMapData:Time()
    return self.other_info.daily_times - self.get_reward_time
end
function TreasureMapData:HaveTask()
    return self.cache_have_task > 0
end

function TreasureMapData:GetFailTip()
    return Language.SuperMap.FailTip
end

--宝图弹出逻辑
function TreasureMapData:UseTreasure()
    local next_item = self:TreasureMapItem()
    if next_item then
        -- 如果上一个使用的是普通藏宝图
        if self.last_is_sp_map == 2 then
            PublicPopupCtrl.Instance:Center(ErrorText[70])
            SceneLogic.Instance:AutoToPos(
                next_item.param.scene_id,
                next_item.param,
                BehaviorFactory.TreasureMap(next_item),
                true,
                true
            )
        elseif self.last_is_sp_map == 1 then
            self.new_flag = true
            MainOtherCtrl.Instance:OpenKeyUseView(next_item)
        end
    else
        SceneLogic.Instance.is_auto_map = false
    end
end

function TreasureMapData:TreasureMapItem()
    local list = BagData.Instance:GetGridByBigType(ITEM_BIG_TYPE.TREASURE_MAP)
    local next_item = nil
    if next(list) ~= nil then
        for _, value in ipairs(list) do
            -- 这里除去星辰藏宝图
            if not value:IsStarTreasureMap() then
                --增加是否可进入场景
                local id = UnlockNewMapCtrl.Instance:GetIrregularID(value.param.scene_id)
                local state = MapData.Instance:GetIrregularFlagState(id)
                if state and value:MapType() == self.last_is_sp_map then
                    next_item = value
                    break
                end
            end
        end
    end
    
    if next_item == nil then
        -- 有问号的那种藏宝图，如果有就自动使用掉
        if self.last_is_sp_map == 2 then
            if Item.Create({item_id = CommonItemId.MapItem}):Num() > 0 then
                return self:UseMap(CommonItemId.MapItem)
            end
        elseif self.last_is_sp_map == 1 then
            if Item.Create({item_id = CommonItemId.SuperMapItem}):Num() > 0 then
                return self:UseMap(CommonItemId.SuperMapItem)
            end
        else
            self.new_flag = false
        end
    end
    return next_item
end
function TreasureMapData:UseMap(item_id)
    local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, item_id)
    local param_t = {}
    param_t.column = ItemColumnType.Item
    param_t.index = index - 1
    BagCtrl.Instance:SendUseItem(param_t)
    TimeHelper:AddDelayTimer(
        function()
            return self:UseTreasure()
        end,
        1
    )
end

function TreasureMapData:Content0(index, num)
    num = num or 0
    local role_level = RoleData.Instance:GetRoleLevel()
    if self.content_list[num] then
        for i, v in ipairs(self.content_list[num]) do
            if role_level >= v.min_level and role_level <= v.max_level and v.index == index then
                return v
            end
        end
    end
end

function TreasureMapData:Reward()
    local list = {}
    -- table.insert(list, Item.Create({item_id = CommonItemId.BaoTu, num = self.other_info.daily_times}))
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.reward_find) do
        if level >= value.min_level and level <= value.max_level then
            if value.exp > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Exp, num = value.exp * self:Time()}))
            end
            if value.coin > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Coin, num = value.coin * self:Time()}))
            end
            break
        end
    end
    return list
end
function TreasureMapData:Reward2()
    local list = {}
    -- table.insert(list, Item.Create({item_id = CommonItemId.BaoTu, num = self.other_info.daily_times}))
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.treasure_map_config) do
        if level >= value.min_level and level <= value.max_level then
            if value.exp > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Exp, num = value.exp * self.other_info.daily_times}))
            end
            if value.coin > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Coin, num = value.coin * self.other_info.daily_times}))
            end
            if next(value.reward) then
                table.insert(list, Item.Create(value.reward[0]))
            end
            break
        end
    end
    return list
end
function TreasureMapData:FindInfo()
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.reward_find) do
        if level >= value.min_level and level <= value.max_level then
            return value
        end
    end
end

function TreasureMapData:GetStarTreasureMapId()
    return self.other_info.star_treasure_map_id
end
function TreasureMapData:GetShengQiTreasureMapId()
    return self.other_info.shengqi_treasure_map_id
end

function TreasureMapData:GetTreasurePos(item)
    if item:IsStarTreasureMap() and not item:IsActiveMap()then
        return ""
    end
    if item == nil or item.param == nil then
        return ""
    end
    local cfg = SceneData.Instance:GetSceneCfg(item.param.scene_id)
    return "\n\n" .. Format(Language.Common.ShowTypeDesc[4], cfg.name or "", item.param.x or 0, item.param.y or 0)
end

-- 记录上一次用的藏宝图
function TreasureMapData:SetLastMapType(is_sp_map)
    self.last_is_sp_map = is_sp_map
end

-- 返回上一次使用的藏宝图类型
function TreasureMapData:GetLastMapType()
    return self.last_is_sp_map
end

function TreasureMapData:SetNewFlag(value)
    self.new_flag = value
end

function TreasureMapData:GetNewFlag()
    return self.new_flag
end
function TreasureMapData:IsMapMonster(group_id)
    return self.monster_group_list[group_id]
end

function TreasureMapData:CacheForgeCompose()
    local id = self:GetStarTreasureMapId()
    local config = WorkshopData.Instance:GetForgeComposeCfg(id)
    self.is_star_map_compose = {}
    for i=1,5 do
        local item_id = config["stuff_id_"..i]
        if item_id ~= 0 then
            self.is_star_map_compose[item_id] = true
        end
    end
    id = self:GetShengQiTreasureMapId()
    config = WorkshopData.Instance:GetForgeComposeCfg(id)
    self.is_sheng_qi_map_compose = {}
    if config then
        for i=1,5 do
            local item_id = config["stuff_id_"..i]
            if item_id ~= 0 then
                self.is_sheng_qi_map_compose[item_id] = true
            end
        end
    end
end
function TreasureMapData:MapType(item_id)
    for k, v in pairs(self.is_star_map_compose) do
        if k == item_id then
            return 3
        end
    end
    for k, v in pairs(self.is_sheng_qi_map_compose) do
        if k == item_id then
            return 4
        end
    end
    return 3
end