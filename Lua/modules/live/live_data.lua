LiveData = LiveData or BaseClass()
LiveData.SKILL_STATUS = {
    SKILL = 1,      -- 生活技能
    JOB = 2,        -- 生活职业
}
function LiveData:__init()
    if LiveData.Instance ~= nil then
		Debuger.LogError("[LiveData] attempt to create singleton twice!")
		return
	end
    LiveData.Instance = self

    self.collect_cfg = Config.collect_scene_auto
    self.live_cfg = Config.life_system_auto

    self.skill_all_info = SmartData.New()
    self.select_tab = SmartData.New({skill_type = -1})     -- 当前选中展示的生活技能类型
    self.sel_left_item_cell = SmartData.New({data = {}})
    self.prop_use_all_info = SmartData.New()
    self.other_data = SmartData.New({height = 0, left_data = {}})
    self.card_use_all_info = SmartData.New()        -- 变身卡使用信息
    self.gather_energy = SmartData.New({val = 0, is_remind = true})            -- 体力       
    self.active_recipe_flag = SmartData.New()            -- 激活的食谱信息      

    self.view_open_info = {tab_index = -1, is_refine = false, item_id = nil,level = nil}    -- 界面打开信息

    self.task_list = {}

    self.prop_use_item_info = {}        -- 道具使用信息

    self.battled_unfix_card = false     -- 战斗结束后是否脱下幻化卡

    self.select_season = 0  --当前选中的季节
    self.select_cook_type = 1  --当前选中的烹饪类型

    self:InitCfg()
end

function LiveData:OnLifeSkillRecipeInfo(protocol)
    self.active_recipe_flag.flag = protocol.active_recipe_flag
end

function LiveData:InitCfg()
    Cfg.Register("GetLiveSceneCfg",Cfg.RealTimeSearch(Config.scene_config_auto.stone_scene,"scene_id"))
    Cfg.Register("GetcollectSceneCfg",Cfg.RealTimeSearch(self.collect_cfg.gather_scene,"scene_id"))
    Cfg.Register("GetLivePropCfg",Cfg.RealTimeSearch(self.live_cfg.props,"seq"))
    Cfg.Register("GetRicipeCfg",Cfg.RealTimeSearch(self.live_cfg.recipe,"compose_item"))
    -- 技能等级关联采集物 用于获取采集倍数
    self.SceneMultipGroupCfg = DataHelper.ConvertGroup(self.collect_cfg.skill_level_and_gather, "scene_id")
end

-- 获取技能信息
function LiveData:GetSkillTypeInfo(skill_type)
    return self.skill_all_info[skill_type] or {}
end

-- 获取技能配置
function LiveData:GetSkillTypeCfg(skill_type)
    skill_type = skill_type or self.select_tab.skill_type
    for i,v in pairs(self.live_cfg.life_skill) do
        if skill_type == v.skill_type then
            return v,i
        end
    end
end

function LiveData:GetSkillTypeLevel(skill_type)
    local info = self:GetSkillTypeInfo(skill_type)
    return info.level or 0
end

-- 获取选择的技能
function LiveData:GetSelSkillType()
    return self.select_tab.skill_type or -1
end

function LiveData:SetSelSkillType(skill_type)
    self.select_tab.skill_type = skill_type
end

function LiveData:InitSkillOutputCfg()
    self.skill_output_cfg = {}  -- 技能产出
    for i,v in ipairs(self.collect_cfg.gather_scene) do
        if not self.skill_output_cfg[v.limit_skill] then
            self.skill_output_cfg[v.limit_skill] = {}
        end
        if not self.skill_output_cfg[v.limit_skill][v.limit_skill_level] then
            self.skill_output_cfg[v.limit_skill][v.limit_skill_level] = {}
        end
        table.insert(self.skill_output_cfg[v.limit_skill][v.limit_skill_level], v)
    end

    for i,v in ipairs(self.live_cfg.recipe) do
        if not self.skill_output_cfg[v.work_type] then
            self.skill_output_cfg[v.work_type] = {}
        end
        if not self.skill_output_cfg[v.work_type][v.work_level] then
            self.skill_output_cfg[v.work_type][v.work_level] = {}
        end
        table.insert(self.skill_output_cfg[v.work_type][v.work_level], v)
    end
end

function LiveData:GetSkillOutputCfg()
    if not self.skill_output_cfg then
        self:InitSkillOutputCfg()
    end
    return self.skill_output_cfg
end


-- 获取产出列表
function LiveData:GetOutputListBySkillType(skill_type)
    skill_type = skill_type or self:GetSelSkillType()
    local cfg = self:GetSkillOutputCfg()
    local output = cfg[skill_type]
    return output or {}
end

-- 获取技能等级对应的产出 [{skill_type, limit_level, cfg}]
function LiveData:GetOutputListBySkillTypeAndLevel(skill_type, level)
    skill_type = skill_type or self:GetSelSkillType()
    local cfg = self:GetSkillOutputCfg()
    local output = cfg[skill_type][level]
    return output or {}
end

-- return : [{level , list_data : [{item_id,cfg}]}]
function LiveData:GetContentLeftListData(skill_type)
    skill_type = skill_type or self:GetSelSkillType()
    local is_feast= skill_type == LiveCtrl.LIVE_SKILL_TYPE.COOK and self:GetSelCookType() ==2
    local cfg =is_feast and self:GetCookFeastOutputCfg() or self:GetOutputListBySkillType(skill_type)
    local list_data = {}
    local skill_level = self:GetSkillTypeLevel(skill_type)
    for i, v in pairs(cfg) do
        local data = {}
        data.level = i
        data.list_data = {}
        local child_cfg = is_feast and v or self:GetOutputListBySkillTypeAndLevel(skill_type, i)
        if is_feast then
            data.title=Config.npc_mood_auto.feast_list[i].feast_name
        end
        for i, v in ipairs(child_cfg) do
            local value = is_feast and v.cfg or v
            if value.show_item and value.show_item ~= "" then
                local select_season=self:GetSelSeason()
                if select_season ==0 or value.season == 0   or self:GetSelSeason() == value.season then
                    local item_id_list = Split(value.show_item, "|")
                    for _, v2 in pairs(item_id_list) do
                        local item_id = tonumber(v2)
                        table.insert(data.list_data, {item_id = item_id, cfg = value})
                    end
                end
            end
            if value.compose_item and value.role_type then
                -- 区分职业类型
                local role_type_list = Split(value.role_type, "|")
                local role_type = RoleData.Instance:ProfType()
                local is_can = false
                for i, v in pairs(role_type_list) do
                    local v = tonumber(v)
                    if v == role_type then
                        is_can = true
                        break
                    end
                end
                if is_can then
                    table.insert(data.list_data, {item_id = value.compose_item, cfg = value,need_num = v.need_num or 0})
                end
            end
        end
        table.insert(list_data, data)
    end
    table.sort(list_data, function(a, b)
        return a.level < b.level
    end)
    return list_data
end

-- 宴席产出配置
function LiveData:GetCookFeastOutputCfg()
    if self.cook_output_cfg == nil then
        self.cook_output_cfg = {}
        local cfgs = Config.npc_mood_auto.feast_list
        for _, v in pairs(cfgs) do
            if not self.cook_output_cfg[v.seq] then
                self.cook_output_cfg[v.seq] = {}
            end
            for i = 1, 6 do
                local item_id = v["food_" .. i]
                if item_id ~= 0 and item_id ~= "" then
                    local info={}
                    info.cfg=Cfg.GetRicipeCfg(item_id)
                    info.need_num = v["num_" .. i]
                    table.insert(self.cook_output_cfg[v.seq],info)
                end
            end
        end
    end
    return self.cook_output_cfg
end

-- 是否锁住
function LiveData:GetIsLock(skill_type, limit_level)
    local skill_level = self:GetSkillTypeLevel(skill_type)
    return limit_level > skill_level
end

-- 判断技能身份
function LiveData:GetSkillStatus(skill_type)
    if skill_type <= 3 then
        return LiveData.SKILL_STATUS.SKILL
    else
        return LiveData.SKILL_STATUS.JOB
    end
end

function LiveData:GetSceneCfg(scene_id)
    local cfg = Cfg.GetLiveSceneCfg(scene_id)
    return cfg
end

-- 获取活力
function LiveData:GetEnergy()
    return FightData.Instance.gather_energy.val or 0
end

-- 技能等级加成配置
function LiveData:GetLevelAdditionCfg(skill_level)
    return self.live_cfg.level_up[skill_level] or {}
end

-- 最大触发双倍次数
function LiveData:GetTriggerMaxCount()
    return self.live_cfg.other[1].occupation_double_times
end

-- 获取最大等级
function LiveData:GetMaxLevel()
    return #self.live_cfg.level_up
end

-- 根据场景id获取全部可能出现的奖励
function LiveData:GetSceneAllReward(scene_id)
    local cfg = Cfg.GetcollectSceneCfg(scene_id)
    if not cfg then
        return {}
    end
    local list = {}
    local item_id_list = Split(cfg.show_item, "|")
    for i,v in pairs(item_id_list) do
        local item_id = tonumber(v)
        table.insert(list, Item.Create({item_id = item_id}))
    end
    return list
end

-- 根据场景id获取奖励描述
function LiveData:GetSceneAllRewardDesc(scene_id)
    local item_list = self:GetSceneAllReward(scene_id)
    local desc = self:FormatRewardDesc(item_list)
    return desc
end

function LiveData:FormatRewardDesc(item_list)
    local desc = ""
    for i,v in pairs(item_list) do
        local str = v:QuaName()
        if i ~= #item_list then
            str = str .. "、"
        end
        desc = desc .. str
    end
    return desc
end

-- 根据场景获取技能类型
function LiveData:GetSkillBySceneId(scene_id)
    local cfg = Cfg.GetcollectSceneCfg(scene_id)
    if not cfg then
        return -1
    end
    return cfg.limit_skill
end

-- 获取道具加成配置
function LiveData:GetPropCfg(card_seq)
    -- for i,v in pairs(self.live_cfg.props) do
    --     if v.seq == card_seq then
    --         return v
    --     end
    -- end
    return Cfg.GetLivePropCfg(card_seq)
end

-- 通过变身卡索引获取模型id
function LiveData:GetResIdByCardSeq(card_seq)
    local cfg = self:GetPropCfg(card_seq) or {}
    return cfg.shift_card_res
end

-- 通过变身卡索引获取属性表
function LiveData:GetAttriListByCardSeq(card_seq)
    local cfg = self:GetPropCfg(card_seq) or {}
    local len = cfg.attribute_num or 0
    local attri_list = {}
    for i = 1, len do
        local attri_type = cfg["attr_type_" .. i]
        local attri_value = cfg["attribute_value_" .. i]
        local data = {attri_type = attri_type, attri_value = attri_value}
        table.insert(attri_list, data)
    end
    return attri_list
end

function LiveData:FormatPropAttriList(item)
    local prop_seq = item:Config().param1
    local attri_list = self:GetAttriListByCardSeq(prop_seq) or {}
    local list_data = {}
    for i,v in pairs(attri_list) do
        local data = CommonStruct.AttrStruct(v.attri_type, v.attri_value)
        table.insert(list_data, data)
    end
    return list_data
end

-- 是否使用变身卡形象，是的话返回模型id
-- 已废弃，升级使用幻兽形象
function LiveData:CardModel(appearance)
    -- if appearance == nil then
    --     return
    -- end
    -- if appearance.model_card_flag ~= 1 or appearance.model_card_seq < 1 then
    --     return
    -- end
    -- local res_id = self:GetResIdByCardSeq(appearance.model_card_seq)   -- 可为空
    -- return res_id
    return HuanShouData.Instance:GetFuShenModelByApp(appearance)
end

-- 角色是否展示变身卡形象
function LiveData:RoleIsShowModel()
    local appearance = RoleData.Instance:GetApearance()
    return appearance.model_card_flag == 1
end

-- 获取角色使用了哪个幻化seq
function LiveData:GetRoleUsedCardSeq()
    local appearance = RoleData.Instance:GetApearance()
    return appearance.model_card_seq
end

function LiveData:SetCardSkillId(skill_id)
    self.card_skill_id = skill_id
end

function LiveData:GetCardSkillId(is_accord)
    return self.card_skill_id or 0
end

-- 角色变身卡技能信息 is_accord:true则筛选主动技能出来
function LiveData:GetCardSkillInfo(seq,is_accord)
    if not seq or seq <= 0 then
        return
    end
    local cfg = self:GetPropCfg(seq)
    if not cfg or cfg.skill_id == 0 then
        return
    end
    local skill_id = cfg.skill_id
    if is_accord then
        local skill_type = SkillData.GetSkillType(skill_id)
        -- 是被动技能不返回
        if skill_type >= 7 and skill_type <= 9 then
            return
        end
    end
    local sk_info = {}
    sk_info.is_card_skill = true
    sk_info.skill_id = skill_id
    sk_info.skill_level = 1
    sk_info.type_text = ColorStr(Language.Fight.SkillTitle[7], COLORSTR.Purple2)
    sk_info.type_bg = "JiNengJiaoBiao3"
    local need_point = cfg.skill_point
    local has_point = self:GetCardPoint(seq)
    if has_point >= need_point then
        local color = COLORSTR.Green4
        sk_info.tip_text = string.format("%s/%s",ColorStr(need_point,color), has_point)
        sk_info.is_card_mask = false
    else
        sk_info.is_card_mask = true
    end
    return sk_info
end

-- 是否在炼制中
function LiveData:IsRefine()
    return self.is_refine
end

function LiveData:SetIsRefine(bool)
    self.is_refine = bool
end

-- 生活技能是否可升级 1，可升级，-1，已达最大等级，0，家族贡献不足，-2，家族书房建筑限制等级不足
function LiveData:GetSkillUpFlag(skill_type)
    local skill_level = self:GetSkillTypeLevel(skill_type)
    local max_level = self:GetMaxLevel()
    if skill_level == 0 then
        BuglyReport("生活系统数据异常：：", skill_type, " Level::", skill_level)
        return 0
    end
    if skill_level == max_level then
        return -1
    end
    if skill_level >= self:LimitMaxLevel() then
        return -2
    end
    local skill_level = self:GetSkillTypeLevel(skill_type)
    local up_cfg = self:GetLevelAdditionCfg(skill_level)
    local need_num = up_cfg.up_expend or 0
    local has_num = RoleData.Instance:GetCurrency(CurrencyType.GuildCotribution)
    if has_num < need_num then
        return 0
    end
    return 1
end

-- 获取背包所有的生活道具
function LiveData:GetLiveItemList(item_id)
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    local list = {}
    local sel_item
    for i,v in pairs(col_grids) do
        local show_type = v:ShowType()
        if show_type == ShowType.LiveFood then
            table.insert(list, v)
            if item_id == v.item_id then
                sel_item = v
            end
        end
    end
    return list, sel_item or list[1]
end


-- 是否有使用加成道具
function LiveData:IsProping()
    for i,v in pairs(self.prop_use_all_info) do
        if not TableIsEmpty(v) and v.seq ~= 0 then
            return true
        end
    end
    return false
end
-- 界面打开跳转到并显示指定item上
function LiveData:JumpItem(item_id, is_flush, feast_seq)
    if not item_id then
        return
    end
    if feast_seq ~= nil then
        self:SetSelCookType(2)
        local _, index = self:GetSkillTypeCfg(LiveCtrl.LIVE_SKILL_TYPE.COOK)
        self.view_open_info.tab_index = index
        self.view_open_info.item_id = item_id
        self.view_open_info.level = feast_seq
    else
        for i = 0, 5 do
            local skill_type = i
            local data = self:GetContentLeftListData(skill_type)
            -- 寻找哪个标签下有该道具
            for _, v in pairs(data or {}) do
                for _, v2 in pairs(v.list_data) do
                    -- 这个item_id在这里面才记录进来
                    if v2.item_id == item_id then
                        local _, index = self:GetSkillTypeCfg(skill_type)
                        self.view_open_info.tab_index = index
                        self.view_open_info.item_id = item_id
                        if skill_type == LiveCtrl.LIVE_SKILL_TYPE.PICK_FLOWER then
                            self:SetSelSeason(v2.season == 0 and 1 or v2.season)
                        end
                        if skill_type == LiveCtrl.LIVE_SKILL_TYPE.COOK then
                            self:SetSelCookType(1)
                        end
                        if is_flush then
                            self.select_tab:Notify()
                        end
                        return
                    end
                end
            end
        end
    end
end



-- 是否加入了家族
function LiveData:IsInGuild()
    local guild_id = GuildData.Instance:GetGuildID() or 0
    return guild_id > 0
end

-- 最大可升至多少级
function LiveData:LimitMaxLevel()
    return GuildData.Instance:GetLiveLimitLevel()
end

-- 下一阶段的数据
function LiveData:GetNextUnlockData(skill_type)
    local left_list_data = self:GetContentLeftListData(skill_type)
    local level = self:GetSkillTypeLevel(skill_type)
    for i,v in pairs(left_list_data) do
        if v.level > level then
            return v
        end
    end
end

-- 获取升级消耗多少公会贡献
function LiveData:GetNeedCurrNum(skill_type, level)
    local cfg = self:GetLevelAdditionCfg(level)
    if not cfg then
        return 0
    end
    if skill_type <= 3 then
        return cfg.up_expend
    elseif skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE then
        return cfg.up_expend_medicine
    elseif skill_type == LiveCtrl.LIVE_SKILL_TYPE.COOK then
        return cfg.up_expend_cook
    end
end

--获取道具加成的的配置
function LiveData:GetAddCfgByItemId(item_id)
    for i,v in pairs(self.live_cfg.props) do
        if item_id == v.item then
            return v
        end
    end
end

--获取配方配置
function LiveData:GetRecipeCfgByItemId(item_id)
    for i,v in pairs(self.live_cfg.recipe) do
        if item_id == v.compose_item then
            return v
        end
    end
end

--获取生活技能采集倍数
function LiveData:GetSceneMultiple(scene_id, level)
    -- local cfg = Cfg.GetSceneMultipleCfg(scene_id, level) or {}
    -- return cfg.multiple or 0
    local group_cfg = self.SceneMultipGroupCfg[scene_id] or {}
    local multiple = 0
    for i,v in pairs(group_cfg) do
        if level >= v.gather_skill_level then
            multiple = v.multiple
        else
            return multiple,v
        end
    end
    return multiple
end


--变身卡类型List
function LiveData:GetCardTypeList()
    if self.page_sn_list then
        return self.page_sn_list
    end
    self.page_sn_list = {}
    for i,v in pairs(self.live_cfg.change_page) do
        if not self.page_sn_list[v.page_sn] then
            local data = {}
            data.page_sn = v.page_sn
            data.page_name = v.page_name
            self.page_sn_list[v.page_sn] = data
        end
    end
    return self.page_sn_list
end

--根据分页序号获取变身卡列表
function LiveData:GetCardList(page_sn)
    if not self.card_list_cfg then
        self.card_list_cfg = {}
    end
    if not self.card_list_cfg[page_sn] then
        self.card_list_cfg[page_sn] = {}
        local index = 0
        for i,v in pairs(self.live_cfg.change_page) do
            if v.page_sn == page_sn then
                local cfg = self:GetPropCfg(v.change_seq)
                cfg = TableCopy(cfg)
                index = index + 1
                cfg.index = index
                table.insert(self.card_list_cfg[page_sn], cfg)
            end
        end
    end
    return self.card_list_cfg[page_sn]
end

--获取变身卡在哪个分页
function LiveData:GetCardSeqPage(seq)
    local page_list = self:GetCardTypeList()
    for _,v in pairs(page_list) do
        local group = self:GetCardList(v.page_sn)
        for i, v2 in pairs(group) do
            if v2.seq == seq then
                return v.page_sn,i
             end
        end
    end
end

--获取变身卡幻化点数
function LiveData:GetCardPoint(seq)
    local info = self.card_use_all_info[seq]
    if not info then
        return 0
    end
    return info.point_num or 0
end

function LiveData:SetPropUseInfo(item_id)
    self.prop_use_item_info.item_id = item_id
end

function LiveData:GetPropUseInfo()
    return self.prop_use_item_info
end

-- 体力信息
function LiveData:SetGatherInfo(protocol)
    self.gather_energy.val = protocol.gather_power
    local power_max = Config.collect_scene_auto.other[1].power_max
    local has_value = self.gather_energy.val
    local targer_rate = Config.life_system_auto.other[1].red_point or 800
    local rate = has_value / power_max
    if rate * 1000 < targer_rate then
        self.gather_energy.is_remind = true
    end
end


-- 红点提醒
function LiveData:GetRemind()
    -- 当活力超过80%，小于100%，点击进来查看关闭后红点消失
    -- 当活力达到100%，红点一直存在。
    local power_max = Config.collect_scene_auto.other[1].power_max
    local has_value = self.gather_energy.val
    local rate = has_value / power_max
    if rate >= 1 then
        return 1
    end
    local targer_rate = Config.life_system_auto.other[1].red_point or 800
    if rate * 1000 >= targer_rate and self.gather_energy.is_remind then
        return 1
    end
    return 0
end

--当前季节
function LiveData:GetSeason()
    local day = TimeCtrl.Instance:GetCurOpenServerDay()
    local season = day % 4
    if season == 0 then
        season = 4
    end
    return season
end

--选中季节
function LiveData:SetSelSeason(season)
    self.select_season=season
end

function LiveData:GetSelSeason()
    return self.select_season
end

-- 选中烹饪类型
function LiveData:SetSelCookType(type)
    self.select_cook_type = type
end

function LiveData:GetSelCookType()
    return self.select_cook_type 
end

--是否未解锁配方
function LiveData:IsRecipeLock(cfg)
    local is_lock = false
    if cfg.work_type == LiveCtrl.LIVE_SKILL_TYPE.COOK and cfg.unlock_item ~= 0 then
        if self.active_recipe_flag.flag[cfg.recipe_seq+1] == 0 then
            is_lock = true
        end
    end
    return is_lock
end

