RushDungeonData = RushDungeonData or BaseClass()
function RushDungeonData:__init()
    if RushDungeonData.Instance ~= nil then
        Debuger.LogError("[RushDungeonData] attempt to create singleton twice!")
        return
    end
    RushDungeonData.Instance = self
    self.all_cfg = Config.drop_hundred_floor_auto
    self.rank_reward = self.all_cfg.rank_reward
    self.level_cfg = self.all_cfg.level_cfg
    self.npc_list = self.all_cfg.npc_list
    self.buff_list = self.all_cfg.buff_list
    self.other_info = self.all_cfg.other[1]
    self.reward_group = self.all_cfg.reward_group
    self.use_item_info = {}
    self.rank_info =
        SmartData.New(
        {
            role_rank = 0,
            rank_list = {}
        }
    )
    self.layer_info =
        SmartData.New(
        {
            start_level = 1,
            end_level = 1,
            map_level = 0,
        }
    )
    self.cur_level = 0
    self.reward_list = {}
    self.level_config = KeyList(self.level_cfg, "scene_id")
    self.play_times = 0
    self.scene_info = nil
    self.team_info = nil
    self.mop_up_list = {}
    for _, value in ipairs(self.level_cfg) do
        if value.mop_up_node == 1 then
            table.insert(self.mop_up_list, value)
        end
    end
    self.mop_up_info = SmartData.New({
        level = 1,
        index = 1,
        is_get = 0,
        show_level = 1,--客户端用来展示
        show_index = 1,
        world_level = 1,
    })
    self.mop_max_index = table.nums(self.mop_up_list)
    self.mop_up_level = 1
    ActivityData.Instance:RegisterRewardGetFunc(ActType.RushDungeon, function ()
        return self:GetTotalPreviewRewardShow()
    end)
end
function RushDungeonData:__delete()
end
function RushDungeonData:WeekPass(value)
    if value then
        self.week_pass_level = value
    end
    return self.week_pass_level or 0
end
function RushDungeonData:IsInScene()
    return self.level_config[SceneData.Instance:SceneId()]
end

function RushDungeonData:CheckNpcInDungeon(npc_id)
    for _, v in ipairs(self.npc_list) do
        if npc_id == v.npc_id then
            return v
        end
    end
end
function RushDungeonData:CacheNpcVo(vo)
    self.npc_vo = vo
end
function RushDungeonData:NpcVo()
    return self.npc_vo
end
function RushDungeonData:CacheNpcId(id)
    self.npc_id = id
end
function RushDungeonData:NpcId()
    return self.npc_id
end
function RushDungeonData:CacheObjId(id)
    self.obj_id = id
end
function RushDungeonData:ObjId()
    return self.obj_id
end

function RushDungeonData:SetRankInfo(protocol)
    -- self.rank_info.role_rank = protocol.count
    self.rank_info.rank_list = protocol.rank_list
end
function RushDungeonData:RankInfo()
    return self.rank_info
end
function RushDungeonData:SetLayerInfo(protocol)
    self.layer_info.start_level = protocol.start_level
    self.layer_info.end_level = protocol.end_level
    self.layer_info.map_level = protocol.end_level
    --奖励是当前-1
    --[[ self.reward_list = {}
    if self.layer_info.end_level - self.layer_info.start_level > 1 then
        for i = self.layer_info.start_level, self.layer_info.end_level do
            if self.level_cfg[i]  then
                table.insert(
                    self.reward_list,
                    Item.Create({item_id = CommonItemId.Exp, num = self.level_cfg[i].exp_reward})
                )
                if self.level_cfg[i].coin > 0 then
                    table.insert(
                    self.reward_list,
                    Item.Create({item_id = CommonItemId.Coin, num = self.level_cfg[i].coin})
                )
                end
                local list = self.level_cfg[i].reward
                for k, v in pairs(list) do
                    table.insert(self.reward_list, Item.Create(v))
                end
            end
        end
    else
        local list = self.level_cfg[self.layer_info.end_level].reward
        table.insert(
            self.reward_list,
            Item.Create({item_id = CommonItemId.Exp, num = self.level_cfg[self.layer_info.end_level].exp_reward})
        )
        if self.level_cfg[self.layer_info.end_level].coin > 0 then
            table.insert(
                self.reward_list,
                Item.Create({item_id = CommonItemId.Coin, num = self.level_cfg[self.layer_info.end_level].coin})
            )
        end
        for k, v in pairs(list) do
            table.insert(self.reward_list, Item.Create(v))
        end
    end ]]
end
function RushDungeonData:SetRewardList(level)
    self.reward_list = {}
    self.reward_list = self:GetPreviewReward()
    --[[ local list = self.level_cfg[level].reward
    table.insert(
        self.reward_list,
        Item.Create({item_id = CommonItemId.Exp, num = self.level_cfg[level].exp_reward})
    )
    if self.level_cfg[level].coin > 0 then
        table.insert(
            self.reward_list,
            Item.Create({item_id = CommonItemId.Coin, num = self.level_cfg[level].coin})
        )
    end
    for k, v in pairs(list) do
        table.insert(self.reward_list, Item.Create(v))
    end ]]
end
function RushDungeonData:GetPreviewReward(level)
    local level = level or (self:LayerInfo().map_level)
    if self.level_cfg[level] == nil then return {} end
    local config = self.level_cfg[level]
    local role_level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.reward_group) do
        if config.reward_group == value.seq
        and role_level >= value.min_level 
        and role_level <= value.max_level then
            local list = DataHelper.FormatItemList(value.reward)
            table.insert(
                list,
                Item.Create({item_id = CommonItemId.Exp, num = value.exp_reward})
            )
            if value.coin > 0 then
                table.insert(
                    list,
                    Item.Create({item_id = CommonItemId.Coin, num = value.coin})
                )
            end
            return list
        end
    end
    return {}
end
function RushDungeonData:LayerInfo()
    return self.layer_info
end

function RushDungeonData:RewardList()
    return self.reward_list
end

--获取奖励
function RushDungeonData:GetRankReward(level)
    local res
    for index, value in ipairs(self.rank_reward) do
        if level <= value.max and level >= value.min then
            res = value
            break
        end
    end
    return res
end

function RushDungeonData:GetFailTip()
    return Language.RushDungeon.FailTip
end

-- 返回预览奖励
function RushDungeonData:GetPreReward()
	local rewards = {}
	local co = ActivityData.Instance:GetActRewardData(ActType.RushDungeon)
	if co then
		for _, item_id in pairs(string.split(co.reward_id, ":")) do
			table.insert(rewards, Item.Init(tonumber(item_id)))
		end
	end
	return rewards
end

function RushDungeonData:MiniNum()
    local cfg = ActivityData.GetConfig(ActType.RushDungeon)
    return cfg.number_min
end
function RushDungeonData:DoorName(name)
    if self:IsInScene() then
        local cur_level = self:LayerInfo().end_level
        local max_level = self.other_info.max_layer
       --[[  if cur_level == max_level - 1 then
            name = Language.RushDungeon.DoorNames[2] ]]
        if cur_level == max_level then
            name = Language.RushDungeon.DoorNames[3]
        else
            name = Language.RushDungeon.DoorNames[1]
        end
    end
    return name
end
function RushDungeonData:ClearData()
    self.layer_info:Set(
        {
            start_level = 1,
            end_level = 1,
            map_level = 0
        }
    )
    self.npc_id = nil
    self.obj_id = nil
    self.play_times = 0
    -- self.week_pass_level = 0
end

function RushDungeonData:SceneInfo(info)
    if info ~= nil then
        self.scene_info = info
    else
        return self.scene_info
    end
end
function RushDungeonData:BuffCfg(id)
    return self.buff_list[id]
end

local special_act_remind = {
    [ActType.RushDungeon] = 1
}

function RushDungeonData.GetOpenRemind(act_type)
    if special_act_remind[act_type] == 1 then
        local time = UnityPlayerPrefs.GetInt(PrefKeys.ActTypeClickTime(act_type))
        if time ~= 0 and tonumber(os.date("%Y%m%d")) - time <= 1 then
            return true
        end
    end
    return false
end

function RushDungeonData.SetOpenRemind(act_type)
    if special_act_remind[act_type] == 1 then
        UnityPlayerPrefs.SetInt(PrefKeys.ActTypeClickTime(act_type), tonumber(os.date("%Y%m%d")))
    end
end

function RushDungeonData:SetBraveGroundUseItemInfo(protocol)
    self.use_item_info = protocol.use_item_info
end

function RushDungeonData:GetUseItemInfoByItemId(item_id)
    local use_type = Item.GetUseType(item_id)
    local limit_times = 0
    for k, v in pairs(Config.drop_hundred_floor_auto.expend_num) do
        if use_type == v.use_type then
            limit_times = v.xz_num
            break
        end
    end
    return limit_times - (self.use_item_info[use_type] or 0), limit_times
end
function RushDungeonData:SetTeamInfo(params)
    self.team_info = params.info
    self.min_level = 9999
    --得到最小层
    for i = 1, 5 do
        if self.min_level == nil and params.info[i].role_id > 0 then
            self.min_level = params.info[i].reached_level
        end
        if params.info[i].role_id > 0 then
            self.min_level = math.min(self.min_level, params.info[i].reached_level)
        end
    end
end
function RushDungeonData:MinLevel()
    return self.min_level or 0
end
function RushDungeonData:TeamInfo()
    return self.team_info
end
function RushDungeonData:SetHistoryLevel(params)
    self.history_level = params
end
function RushDungeonData:GetHistoryLevel()
    return self.history_level or 0
end
function RushDungeonData:SetMopUpLevel(level)
    self.mop_up_info.level = level
end
function RushDungeonData:SetMopUpIndex(index)
    self.mop_up_info.index = index
end
function RushDungeonData:SetMopUpShowLevel(level)
    self.mop_up_info.show_level = level
end
function RushDungeonData:SetMopUpShowIndex(index)
    self.mop_up_info.show_index = index
end
function RushDungeonData:GetLevelCfg(level)
    return self.level_cfg[level]
end
function RushDungeonData:SetMopUpReward(flag)
    self.mop_up_info.is_get = flag
end
function RushDungeonData:SetCurLevel(level)
    self.cur_level = math.max(self.cur_level, level)
end
function RushDungeonData:GetModMaxIndex()
    return self.mop_max_index
end
function RushDungeonData:GetMopUpReward()
    local list = DataHelper.FormatItemList(self.level_cfg[1].reward)
    table.insert(
        list,
        Item.Create({item_id = CommonItemId.Exp, num = 0})
    )
    table.insert(
        list,
        Item.Create({item_id = CommonItemId.Coin, num = 0})
    )
    local level = self.mop_up_info.show_level
    for i = 1, level do
        list[1].num = list[1].num + self.level_cfg[i].reward[0].num
        list[2].num = list[2].num + self.level_cfg[i].exp_reward
        list[3].num = list[3].num + self.level_cfg[i].coin
    end
    list[1].num = list[1].num - 1
    return list
end
function RushDungeonData:GetRewardNum()
    if self.mop_up_info.level > 0 and self.mop_up_info.is_get == 0 then
        return 1
    end
    return 0
end
--获取上一个或者下一个的展示奖励
function RushDungeonData:FlushShowReward(flag)
    local level = self.mop_up_info.show_level
    local temp = 1
    for index, value in ipairs(self.mop_up_list) do
        if value.level == level then
            temp = index
            break
        end
    end
    temp = temp + flag
    self.mop_up_info.show_level = self.mop_up_list[temp].level
    self.mop_up_info.show_index = temp
end
function RushDungeonData:SetWorldLevel(level)
    self.mop_up_info.world_level = level
end
function RushDungeonData:GetOpenLevel()
    local level = 1
    for index, value in ipairs(self.level_cfg) do
        if value.need_world_level <= self.mop_up_info.world_level then
            level = math.max(level, value.level)
        end
    end
    return level
end
function RushDungeonData:CanEnterDoor()
    --[[ local config = self:IsInScene()
    if config then
        local open_level = self:GetOpenLevel()
        if config.level >= open_level and config.level ~= self.other_info.max_layer then
            config = self.level_cfg[open_level + 1]
            PublicPopupCtrl.Instance:Center(string.format(Language.RushDungeon.EnterDoorTip, config.need_world_level))
            return false
        else
            return true
        end
    end ]]
    return true
end
function RushDungeonData:GetTotalPreviewRewardShow()
    local reward_list = {
        [1] = {
            toggle_name = Language.RushDungeon.BaseReward,
            reward_item_list = {}
        },
        [2] = {
            toggle_name = Language.RushDungeon.RankReward,
            reward_item_list = {}
        },
        [3] = {
            toggle_name = Language.RushDungeon.TipsNotice,
            reward_item_list = {{tips_act_type = ActType.RushDungeon}}
        },
    }
    --通关奖励
    local temp = {}
    temp.child_list = {}
    temp.title_name = Language.RushDungeon.SuccReward
    reward_list[1].reward_item_list.child_list = {}
    for index, value in ipairs(self.level_cfg) do
        local partic_temp = {}
        partic_temp.child_title = string.format(Language.RushDungeon.Level, value.level)
        partic_temp.title_type = 2
        partic_temp.item_list = self:GetPreviewReward(value.level)
        table.insert(temp.child_list, partic_temp)
    end
    table.insert(reward_list[1].reward_item_list, temp)
    --排行奖励
    for k,v in ipairs(self.rank_reward) do
        local temp = {}
        if v.min == v.max then
            temp.title_name = string.format(Language.RushDungeon.RankTitleFormat[1], v.min)
        else
            temp.title_name = string.format(Language.RushDungeon.RankTitleFormat[2], v.min, v.max)
        end
        temp.title_type = 2
        temp.item_list = {}
        for _,w in pairs(v.reward_show) do
            table.insert(temp.item_list, Item.Create(w))
        end
        table.insert(reward_list[2].reward_item_list, temp)
    end
    reward_list[3].title_name = Language.RushDungeon.TipsNotice
    return reward_list
end
function RushDungeonData:RankReward()
    local list = {}
    for k,v in ipairs(self.rank_reward) do
        local temp = v
        temp.is_rank = true
        temp.item_list = {}
        for _,w in pairs(v.reward_show) do
            table.insert(temp.item_list, Item.Create(w))
        end
        table.insert(list, temp)
    end
    return list
end

function RushDungeonData:PassReward()
    local list = {}
    for k,v in ipairs(self.level_cfg) do
        local temp = {}
        temp.level = v.level
        temp.is_rank = false
        temp.item_list = self:GetPreviewReward(temp.level)
        table.insert(list, temp)
    end
    return list
end