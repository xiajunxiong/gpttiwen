YouMingData = YouMingData or BaseClass()

function YouMingData:__init()
    if YouMingData.Instance ~= nil then
        Debuger.LogError("[YouMingData] attempt to create singleton twice!")
        return
    end
    YouMingData.Instance = self
    self.youming_list = SmartData.New()     -- 幽冥背包列表
    self.equip_list = SmartData.New()       -- 幽冥装备背包列表
    self.select_youming = SmartData.New()   -- 界面选择的幽冥
    self.remodel_youming = SmartData.New()  -- 改造选择的幽冥
    self.select_equip = SmartData.New()    -- 选择的幽冥装备
    self.select_use_item = SmartData.New({item = nil, requa_item = nil, skill_item = nil, juexing_items = false})--选择的改造图 
    self.lottery_info = SmartData.New()     -- 幽冥探宝信息
    self.lottery_result = SmartData.New()   -- 探宝抽取结果
    self.dim_attr_list = {}--属性列表
    self.bless_pet_list = SmartData.New()   -- 附魂信息列表
    self.change_talent_group_maxs = nil       -- 改造天资组对应最大值
    self.change_skill_maxs = nil             -- 改造对应最大技能数
    self.change_item_list = nil             --改造物品列表
    self.change_talent_group_rate = nil     --改造属性概率
    self.change_skill_rate = nil            --改造技能孔概率
    self.select_juexing_items = {}
    self.seal_smart = SmartData.New({auto_seal = false, flush_card_count = false, flush_scene_title = false})       --幽冥捕捉

    --红点提醒
    self.remind_data = SmartData.New({
        lottrey_remind = SmartData.New({value = false}),
        compound_remind = SmartData.New({value = false}),
    })

	Cfg.Register("YouMingSanHunCfg", Cfg.RealTimeSearch3Key(Config.dim_pet_cfg_auto.dim_pet_level, "id", "type", "level"))
    Cfg.Register("YouMingQiPoCfg", Cfg.RealTimeSearch3Key(Config.dim_pet_cfg_auto.dim_pet_spirits, "id", "type", "level"))
    Cfg.Register("YouMingJueXing", Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.Skill_hole_up, "id", "level"))
    Cfg.Register("YouMingGaiZao", Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.dim_pet_change, "change_id", "change_consume"))
    Cfg.Register("YouMingEquipLevel", Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.nether_costume_up, "id", "level"))
    Cfg.Register("YouMingEquipLevel2", Cfg.RealTimeSearch3Key(Config.dim_pet_cfg_auto.nether_costume_up_1, "id", "attr_type", "level"))
    Cfg.Register("YouMingQiangHua", Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.nether_costume_strengthen, "id", "level"))
    Cfg.Register("YouMingQiangHua2", Cfg.RealTimeSearch3Key(Config.dim_pet_cfg_auto.nether_costume_strengthen_1, "id", "attr_type", "level"))
    Cfg.Register("YouMingAddBuff", Cfg.RealTimeSearch(Config.dim_pet_cfg_auto.talent_buff, "talent_differ"))
    --主动技能
    Cfg.Register("InitiativeSkillLevelCfg",Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.active_skill, "skill_id", "skill_level"))
    --附身技能描述配置
    Cfg.Register("AppendageSkillLevelCfg",Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.possess_skill, "skill_id", "skill_level"))
    --出场技能描述配置
    Cfg.Register("InbornSkillLevelCfg",Cfg.RealTimeSearch2Key(Config.dim_pet_cfg_auto.entrance_skill, "skill_id", "skill_level"))
    self.score_1 = {10,5,3.3,3,2}
    self.score_2 = {50,25,16.7,10,10}
    self.delay_pet_list = {}
end

function YouMingData:ClearData()
    self.youming_list:Clear()
    self.equip_list:Clear()
    self.bless_pet_list:Clear()
    self.select_youming:Clear()
    self.remodel_youming:Clear()
    self.select_equip:Clear()
    self.lottery_info:Clear()
end

function YouMingData:Cfg()
    return Config.dim_pet_cfg_auto
end
function YouMingData:ChangeTalentMax(group_id)
    if self.change_talent_group_maxs == nil then
        self.change_talent_group_maxs = {}
        for i, v in ipairs(Config.dim_pet_cfg_auto.change_talent_group) do
            if self.change_talent_group_maxs[v.group_id] == nil then
                self.change_talent_group_maxs[v.group_id] = v.talent_num
            else
                self.change_talent_group_maxs[v.group_id] = math.max(self.change_talent_group_maxs[v.group_id], v.talent_num)
            end
        end
        return self.change_talent_group_maxs[group_id] or 0
    else
        return self.change_talent_group_maxs[group_id] or 0
    end
end
function YouMingData:ChangeSkillMax(group_id)
    if self.change_skill_maxs == nil then
        self.change_skill_maxs = {}
        for i, v in ipairs(Config.dim_pet_cfg_auto.change_skill_group) do
            if self.change_skill_maxs[v.group_id] == nil then
                self.change_skill_maxs[v.group_id] = v.skill_hole_num
            else
                self.change_skill_maxs[v.group_id] = math.max(self.change_skill_maxs[v.group_id], v.skill_hole_num)
            end
        end
        return self.change_skill_maxs[group_id] or 0
    else
        return self.change_skill_maxs[group_id] or 0
    end
end
function YouMingData:ChangeSkillRate(group_id)
    if self.change_skill_rate == nil then
        self.change_skill_rate = {}
        for i, v in ipairs(Config.dim_pet_cfg_auto.change_skill_group) do
            if self.change_skill_rate[v.group_id] == nil then
                self.change_skill_rate[v.group_id] = {}
            end
            table.insert(self.change_skill_rate[v.group_id], v.rate)
        end
    end
    return self.change_skill_rate[group_id]
end

function YouMingData:ChangeAttrRate(group_id)
    if self.change_talent_group_rate == nil then
        self.change_talent_group_rate = {}
        for i, v in ipairs(Config.dim_pet_cfg_auto.change_talent_group) do
            if v.talent_num > 0 then
                if self.change_talent_group_rate[v.group_id] == nil then
                    self.change_talent_group_rate[v.group_id] = v.rate
                else
                    self.change_talent_group_rate[v.group_id] = self.change_talent_group_rate[v.group_id] + v.rate
                end
            end
        end
    end
    return self.change_talent_group_rate[group_id] or 0
end
    --三魂配置
    
function YouMingData:GetYouMingCfg(id)
    return Config.dim_pet_cfg_auto.dim_pet_base[id]
end
function YouMingData:GetYouMingEquipCfg(id)
    return Config.nether_costume_auto[id]
end

function YouMingData:GetSkillCfg(skill_id, skill_level, skill_type)
    if skill_type == YouMingCfg.SkillType.Initiative then
        return Cfg.InitiativeSkillLevelCfg(skill_id, skill_level)
    elseif skill_type == YouMingCfg.SkillType.Appendage then
        return Cfg.AppendageSkillLevelCfg(skill_id, skill_level)
    elseif skill_type == YouMingCfg.SkillType.Inborn then
        return Cfg.InbornSkillLevelCfg(skill_id, skill_level)
    end
end

function YouMingData:GetYouMingList()
    return self.youming_list
end

function YouMingData:SetYouMing(youming, index)
    index = index or youming.index
    self.youming_list[index] = youming
    self.youming_list:Notify()
    if not TableIsEmpty(self.select_youming) then
        if self.select_youming.index == index then
            self.select_youming:Set(youming or {})
            self.select_youming:Notify()
            if youming == nil then
                self:SetBlessInfo(index, nil)
            end
        end
    end
    if not TableIsEmpty(self.remodel_youming) then
        if self.remodel_youming.index == index then
            self.remodel_youming:Set(youming or {})
            self.remodel_youming:Notify()
        end
    end
end

function YouMingData:SetYouMingEquip(youming, index)
    index = index or youming.index
    self.equip_list[index] = youming
    --[[ if not TableIsEmpty(self.select_equip) then
        if self.select_equip.index == index then
            self.select_equip:Set(youming or {})
            self.select_equip:Notify()
        end
    end ]]
end
function YouMingData:GetEquipBagList()
    return self.equip_list
end

function YouMingData:GetYouMing(index)
    return self.youming_list[index]
end

function YouMingData:OnSCNetherWorldBagSingleInfo(protocol)
    local reason = protocol.reason
    local put_reason = protocol.put_reason
    local vo = protocol.vo
    if vo.item_id <= 0 then
        if vo.index >= 0 and vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
            self:SetYouMing(nil, vo.index)
        elseif vo.index >= 0 and vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
            self:SetYouMingEquip(nil, vo.index)
        end
    else
        local youming = nil
        if vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
            youming = YouMing.New(vo)
        elseif vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
            youming = YouMingEquip.New(vo)
        end
        -- local youming = YouMing.New(vo)
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            if not BlockCenter[put_reason] then
                PublicPopupCtrl.Instance:CenterI(youming)
            end
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_YOU_MING_COMPOUND then
        --ViewMgr:OpenView(YouMingGetShowView, {list = {youming}})
            ViewMgr:OpenView(YouMingShowView, {list = {youming}})
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_YOU_MING_SEAL then
            if MainViewData.Instance:IsBattle() then
                local info = {youming}
                table.insert(self.delay_pet_list, info)
            else
                ViewMgr:OpenView(YouMingShowView, {list = {youming}})
            end
        end
        if vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
            self:SetYouMing(youming)
        elseif vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
            self:SetYouMingEquip(youming)
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_YOU_MING_EQUIP_TAKE_OFF then
            
        end
    end
end

function YouMingData:PopDelayPetShow()
    if self.delay_pet_list[1] then
        local info = table.remove(self.delay_pet_list, 1)
        ViewMgr:OpenView(YouMingShowView, {list = info})
    end
end


-- ========== 附魂 ==========

-- 设置附魂信息
function YouMingData:SetBlessInfo(youming_index, pet_u_id_t)
    self.bless_pet_list[youming_index] = pet_u_id_t
end
function YouMingData:GetBlessInfo(youming_index)
    return self.bless_pet_list[youming_index]
end

-- 获取宠物付魂的幽冥
function YouMingData:GetBlessYouMing(pet)
    for i,v in pairs(self.bless_pet_list) do
        if pet.info.pet_unique_id1 == v.pet_unique_id1 and pet.info.pet_unique_id2 == v.pet_unique_id2 then
            local youming = self:GetYouMing(i)
            return youming
        end
    end
end

-- 获取付魂宠物列表
function YouMingData:GetPetList(youming)
    youming = youming or self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return {}
    end
    local pet_list = PetData.Instance:GetPetList()
    local list = {}
    for i,v in pairs(pet_list) do
        -- local pet_sheng_qi = v:ShengQi()
        -- if (pet_sheng_qi and pet_sheng_qi.index == sheng_qi.index) or pet_sheng_qi == nil then
        --     table.insert(list, v)
        -- end
        table.insert(list, v)
    end
    return list
end


function YouMingData:SetAttrList(index, list)
    self.dim_attr_list[index] = list
end

function YouMingData:GetAttrInfo(index)
    return self.dim_attr_list[index] or {}
end

-- =========== 幽冥背包 ==========

function YouMingData:SetSelectYouMing(youming)
    self.select_youming:Set(youming or {})
end
function YouMingData:GetSelectYouMing()
    return self.select_youming
end
-- 改造界面
function YouMingData:SetRemodelYouMing(youming)
    self.remodel_youming:Set(youming or {})
end
function YouMingData:GetRemodelYouMing()
    return self.remodel_youming
end
--冥装操作
function YouMingData:SetSelectYouMingEquip(youming_equip)
    self.select_equip:Set(youming_equip or {})
end
function YouMingData:GetYouMingEquip()
    return self.select_equip
end
function YouMingData:GetBagList()
    local list = {}
    for i,v in pairs(self.youming_list) do
        table.insert(list,v)
    end
    -- 背包排序：评分>品质>等级>ID从大到小
    table.sort(list, function (a,b)
        if a:Score() == b:Score() then
            if a:Qua() == b:Qua() then
                if a:Level() == b:Level() then
                    return a.id > b.id
                end
                return a:Level() > b:Level()
            end
            return a:Qua() > b:Qua()
        end
        return a:Score() > b:Score()
    end)
    return list
end
--获取觉醒可作为觉醒材料的幽冥列表
function YouMingData:GetJueXingCostList()
    local youming = self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return {}
    end
    local list = {}
    local cur, max 
    for i,v in pairs(self.youming_list) do
        cur, max = v:Aptitude()
        if v.item_id == youming.id and v.index ~= youming.index
        and (max - cur) <= Config.dim_pet_cfg_auto.other[1].wake_up_talent_limit  then
            table.insert(list,v)
        end
    end
    return list
end

function YouMingData:BagItemCount()
    return table.nums(self.youming_list:Val())
end

function YouMingData:BagMaxCount()
    return YouMingCfg.MAX_NETHER_WORLD_SYSTEM_BAG_NUM - YouMingCfg.MAX_NETHER_WORLD_EQUIP_BAG_NUM
end

-- 背包是否有这个幽冥
function YouMingData:IsInBag(youming_id)
    for i,v in pairs(self.youming_list) do
        if v.id == youming_id then
            return true
        end
    end
end

function YouMingData:IsActive(youming_id)
    local cfg = Cfg.IllustrateCollectById(youming_id)
    if cfg then
        -- 去收录查找
        local can_active = IllustrateData.Instance:GetActiveCanFalg(cfg.seq) == 1
        local has_active = IllustrateData.Instance:GetActiveHasFalg(cfg.seq) == 1
        return can_active or has_active
    end
    return false
end

-- ============ 探宝 ============

function YouMingData:GetLotteryInfo(tab_type)
    return self.lottery_info[tab_type]
end

function YouMingData:GetLuckyValue(tab_type)
    local info = self:GetLotteryInfo(tab_type) or {}
    local luck_value = info.lucky_value or 0
    return luck_value
end

-- 1可领，0未达成, -1已领取
function YouMingData:GetLotteryRewardFlag(data)
    local tab_type = data.turntable_id
    local info = self:GetLotteryInfo(tab_type)
    if not info then
        return 0
    end
    if info.reward_flag[data.seq] == 1 then
        return -1
    end
    local luck_value = info.lucky_value
    if luck_value >= data.schedule then
        return 1
    end
    return 0
end



-- ======= 三魂 ========= --
--魂解锁等级
function YouMingData:SoulUnlockLevel()
    return Config.dim_pet_cfg_auto.other[1].pre_soul_level
end
--魂重置消耗
function YouMingData:SoulResetConsume()
    return Config.dim_pet_cfg_auto.other[1].reset_consume
end



--- ======================= 红点逻辑 ====================

-- 探宝红点
function YouMingData:GetLottreyRemind()
    for i,v in pairs(Config.dim_pet_cfg_auto.turntable_schedule_reward) do
        local flag = self:GetLotteryRewardFlag(v)
        if flag > 0 then
            return 1
        end
    end
    local item_list = self:GetLottreyRemindItemList()
    for i,v in pairs(item_list) do
        if Item.GetNum(v) > 0 then
            return 1
        end
    end
    return 0
end
function YouMingData:GetLottreyRemindItemList()
    local t = {
        Config.dim_pet_cfg_auto.other[1].turntable_use,
        Config.dim_pet_cfg_auto.other[1].high_turntable_use,
    }
    return t
end

function YouMingData:CheckLottreyRemind()
    self.remind_data.lottrey_remind.value = not self.remind_data.lottrey_remind.value
end

-- 幽冥附魂红点
function YouMingData:GetYouMingBlessRemind(youming)
    if TableIsEmpty(youming) then
        return 0
    end
    local pet = youming:BePet()
    if pet then
        return 0
    end
    local pet_list = self:GetPetList(youming)
    for i,v in pairs(pet_list) do
        if not v:YouMing() then
            return 1
        end
    end
    return 0
end

-- 碎片合成红点
function YouMingData:GetCompoundRemind()
    local list_data = Config.dim_pet_cfg_auto.compose
    for i,v in pairs(list_data) do
        local has_num = Item.GetNum(v.compose_consume)
        if has_num >= v.compose_consume_num then
            return 1
        end
    end
    return 0
end

function YouMingData:GetCompoundRemindItemList()
    local list = {}
    local list_data = Config.dim_pet_cfg_auto.compose
    for i,v in pairs(list_data) do
        table.insert(list, v.compose_consume)
    end
    return list
end

function YouMingData:CheckCompoundRemind()
    self.remind_data.compound_remind.value = not self.remind_data.compound_remind.value
end


--=====幽冥改造领悟
function YouMingData:NormalItem()
    return Config.dim_pet_cfg_auto.other[1].grasp_consume_item[0]
end
function YouMingData:HighItem()
    return Config.dim_pet_cfg_auto.other[1].high_grasp_consume_item[0]
end

function YouMingData:NormalLearn()
    -- Config.dim_pet_cfg_auto.ordinar_grasp[1].
end
--=====技能库
function YouMingData:SkillGroup()
    local list = {}
    local item
    for i, v in ipairs(Config.dim_pet_cfg_auto.change_skill_item) do
        item = BagData.Instance:IsHasItemById(v.skill_item)
        if item then
            table.insert(list, item) 
        end
    end
    return list
end
--=====改造图纸
function YouMingData:RemodelBooks(change_id)
    if self.change_item_list == nil then
        self.change_item_list = Split(Config.dim_pet_cfg_auto.other[1].change_item_list, "|")
    end
    local list = {}
    local item
    for i, v in ipairs(self.change_item_list) do
        if i >= change_id then
            item = BagData.Instance:IsHasItemById(tonumber(v))
            if item then
                table.insert(list, item) 
            end
        end
    end
    return list
end
function YouMingData:RemodelBookChangeId(item_id)
    if self.change_item_list == nil then
        self.change_item_list = Split(Config.dim_pet_cfg_auto.other[1].change_item_list, "|")
    end
    for i, v in ipairs(self.change_item_list) do
        if tonumber(v) == item_id then
            return i
        end
    end
    return 0
end

function YouMingData:ChangeItemList()
    if self.change_item_list == nil then
        self.change_item_list = Split(Config.dim_pet_cfg_auto.other[1].change_item_list, "|")
    end
    return self.change_item_list
end
function YouMingData:SetRemodelUse(item)
    self.select_use_item.item = item
end
function YouMingData:SetReQuaUse(item)
    self.select_use_item.requa_item = item
end
function YouMingData:SetSkillUse(item)
    self.select_use_item.skill_item = item
end
function YouMingData:FlushJueXingSelect()
    self.select_use_item.juexing_items = not self.select_use_item.juexing_items
end
function YouMingData:GetRemodelUse()
    return self.select_use_item
end
--选择觉醒消耗列表
function YouMingData:SetJueXingItems(list)
    self.select_juexing_items = list
end
function YouMingData:GetJueXingItems()
    return self.select_juexing_items
end

--==天资果实
function YouMingData:RemodelFruits()
    local list = {}
    local item
    for i, v in ipairs(Config.dim_pet_cfg_auto.fruit) do
        item = BagData.Instance:IsHasItemById(v.item_id)
        if item then
            table.insert(list, item) 
        end
    end
    return list
end

--==重塑天资
function YouMingData:GetReQuaItem()
    return Config.dim_pet_cfg_auto.other[1].remould_consume
end

--==领悟配置
function YouMingData:LingWuConfig(change_id)
    return Config.dim_pet_cfg_auto.skill_learn_itemlist[change_id]
end

--- ==========================幽冥抓捕
function YouMingData:SetCardBuyInfo(protocol)
    self.buy_card_times = protocol.buy_times
    self.seal_smart:SetDirty("flush_card_count")
end
--获取scene_id是否为抓捕场景，传空则为判断当前场景是否为抓捕场景
function YouMingData:IsInSealScene(scene_id)
    scene_id = scene_id or SceneData.Instance:SceneId()
    for _, value in pairs(Config.ghost_hunt_auto.hunt_sence) do
        if value.sence_id == scene_id then
            return true
        end
    end
    return false
end

function YouMingData:GetSealSceneId(id)
    if id ~= nil and Config.ghost_hunt_auto.ghost_hunt_config[id] then
        return Config.ghost_hunt_auto.ghost_hunt_config[id].scene_id
    else
        for _, value in pairs(Config.ghost_hunt_auto.hunt_sence) do
            if value.level_min <= RoleLevel() and value.level_max >= RoleLevel() then
                return value.sence_id
            end
        end
    end
end

function YouMingData:GetSealSceneShowName(scene_id)
    scene_id = scene_id or SceneData.Instance:SceneId()
    local scene_show_cfg = MapData.Instance:GetMapShowData(scene_id)
    local name_str = ""
    for _, value in pairs(Config.ghost_hunt_auto.hunt_sence) do
        if value.sence_id == scene_id then
            name_str = Format(Language.YouMing.SealSceneNameTitle, value.level_min, value.level_max)
            break
        end
    end
    if scene_show_cfg then
        name_str = scene_show_cfg.name..name_str
    end
    return name_str
end

function YouMingData:GetCardBuyTime(seq)
    return self.buy_card_times and self.buy_card_times[seq] or 0
end

function YouMingData:SetAutoSealState(protocol)
    self.seal_smart.auto_seal = protocol.is_auto_seal == 1
end
--===================================

function YouMingData:SetAwakeLevelChange(data)
    self.awake_level_change_data = data
end
function YouMingData:GetAwakeLevelChange()
    return self.awake_level_change_data
end


---=====七魄技能描述
function YouMingData:GetClickDesc(skill)
    local desc = ""
    local list = {}
    for i, v in ipairs(Config.dim_pet_cfg_auto.possess_skill) do
        if skill and skill.skill_id == v.skill_id then
            list[v.skill_level] = v
        end
    end
    for i, v in ipairs(list) do
        desc = desc .. Format(Language.YouMingPractice.LevelTitle,i)
        desc = desc .. Format(Language.YouMingPractice.Description,self:GetAttrDesc(v))        
        desc = desc .. v.description .. (i == #list and "" or "\n")
    end
    return desc
end

function YouMingData:GetAttrDesc(config,is_not_none)
    if config == nil then
        return ""
    end
    local desc = ""
    for i=1,5 do
        local attr_type = config["attr_type_"..i] or 0
        local attr_value = config["attr_value_"..i] or 0
        if attr_value ~= 0 then
            local none = is_not_none == nil and "  " or ""
            local attr_name = Language.Common.AttrList[attr_type] .. none
            local value = DataHelper.Percent(attr_type,attr_value)
            desc = desc .. Format("%s<color=#fd5d47>+%s</color> ",attr_name,value)
        end
    end
    return desc
end

--==主动技能描述
function YouMingData:GetClickDesc2(skill)
    local desc = ""
    local list = {}
    for i, v in ipairs(Config.dim_pet_cfg_auto.active_skill) do
        if skill and skill.skill_id == v.skill_id then
            list[v.skill_level] = v
        end
    end
    for i, v in ipairs(list) do
        desc = desc .. Format(Language.YouMingPractice.LevelTitle2,i)
        -- desc = desc .. Language.YouMingPractice.Description2
        desc = desc .. v.description .. (i == #list and "" or "\n")
    end
    return desc
end

--冥装数量问题
function YouMingData:GetNumByItemId(item_id)
    local num = 0
    for i, v in pairs(self.equip_list) do
        if item_id == v.item_id then
            num = num + 1
        end
    end
    return num
end

--冥装强化数量限制
function YouMingData:GetQHEquipNum(item_id)
    local num = 0
    for i, v in pairs(self.equip_list) do
        if item_id == v.item_id and v:StrLevel() == 1 then
            num = num + 1
        end
    end
    return num
end

--冥装背包index
function YouMingData:SetEquipIndex(index)
    self.youming_equip_index = index
end
function YouMingData:GetEquipIndex()
    return self.youming_equip_index or 1
end


--==========红点逻辑======================
function YouMingData:GetSanHunRemind()
    local youming = self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return 0
    end
    return youming:SanHunRemind()
end

function YouMingData:QiPoRemind()
    local youming = self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return 0
    end
    return youming:QiPoRemind()
end

function YouMingData:JueXingRemind()
    local youming = self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return 0
    end
    return youming:JueXingRemind()
end

function YouMingData:GaiZao1Remind()
    local youming = self:GetRemodelYouMing()
    if TableIsEmpty(youming) then
        return 0
    end
    return youming:GaiZao1Remind()
end

function YouMingData:MingZhuangRemind()
    local youming = self:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return 0
    end
    return youming:MingZhuangRemind()
end

function YouMingData:GetRemodelSkillList(flag)
    local list = {}
    for i, v in ipairs(Config.dim_pet_cfg_auto.change_skill_item) do
        if flag == v.skill_group then
            table.insert(list, YouMingRemodelSkill.New({skill_id = v.skill_id, skill_level = 1}))
        end
    end 
    return list
end

function YouMingData.AddValueLevel(addValue)
    for _, v in pairs(Config.dim_pet_cfg_auto.change_talent_color) do
        if addValue >= v.talent_min and addValue <= v.talent_max then
            return v.color
        end
    end
    return 1
end