FabaoData = FabaoData or BaseClass()
function FabaoData:__init()
    if FabaoData.Instance ~= nil then
        Debuger.LogError("[FabaoData] attempt to create singleton twice!")
        return
    end
    FabaoData.Instance = self
    self.talisman_cfg_auto = Config.talisman_cfg_auto
    self.talisman_compose = self.talisman_cfg_auto.talisman_compose
    self.talisman_attribute = self.talisman_cfg_auto.talisman_attribute
    self.talisman_practice = self.talisman_cfg_auto.talisman_practice
    self.talisman_explain = self.talisman_cfg_auto.talisman_explain
    self.other_info = self.talisman_cfg_auto.other[1]
    self.talisman_fight = self.talisman_cfg_auto.talisman_fight
    self.talisman_upgrade = self.talisman_cfg_auto.talisman_upgrade_new
    self.talisman_advance = self.talisman_cfg_auto.talisman_advance_new
    self.talisman_fetter = self.talisman_cfg_auto.talisman_fetter
    self.qiling_cfg = self.talisman_cfg_auto.fabao_qiling
    self.qiling_up = self:GetQiLingUpCfg()

    self.fabao_equip_bar = SmartData.New()
    self.fabao_list = SmartData.New()
    self.used_count = 0
    self.fabao_jie_list = {}
    self.fabao_level_list = {}
    self.view_data = SmartData.New({select_data = nil, flush = false, select = 1, jump = false})
    self.jieup_view_data = nil
    self.advanced_list = {}

    self.fabao_type_list = {}
    self.fabao_upgrade_list = {}
    self.fabao_advance_list = {}
    self.fetter_group_list = {}
    self.qiling_info = SmartData.New({qi_ling_list = nil, put_on_list = nil, yuling_num = nil, put_on_index_list = nil,active_num=nil})
    self:initConfig()
end

function FabaoData:Clear()
    self.fabao_list:Set({})
    self.fabao_equip_bar:Set({})
end
function FabaoData:initConfig()
    -- 通过item_id 获取法宝配置
    Cfg.Register("TalismanAttributeByFragment",Cfg.RealTimeSearch(self.talisman_attribute,"talisman_fragment_item"))
    
    for _, v in ipairs(self.talisman_explain) do
        if self.fabao_level_list[v.id] == nil then
            self.fabao_level_list[v.id] = {}
        end
        self.fabao_level_list[v.id][v.level] = v
        self.fabao_level_list[v.id][v.level].rewards = {}
        for _, v2 in pairs(v.return_item) do
            table.insert(self.fabao_level_list[v.id][v.level].rewards, Item.Create(v2))
        end
    end
    for _, value in pairs(self.talisman_attribute) do
        if value.is_open ~= 0 then
            local data_type =  value.type > 2 and 2 or value.type   --type为3的是特殊被动，也跟被动一样显示 
            self.fabao_type_list[data_type] = self.fabao_type_list[data_type] or {}
            table.insert(self.fabao_type_list[data_type], value)
        end
    end
    for i,v in ipairs(self.talisman_upgrade) do
        self.fabao_upgrade_list[v.id] = self.fabao_upgrade_list[v.id] or {}
        table.insert(self.fabao_upgrade_list[v.id], v)
    end
    for i,v in ipairs(self.talisman_advance) do
        self.fabao_advance_list[v.id] = self.fabao_advance_list[v.id] or {}
        if v.advanced_level > 0 then
            table.insert(self.fabao_advance_list[v.id], v )
        end
    end
    for index, value in ipairs(self.talisman_fetter) do
        self.fetter_group_list[value.fetter_group] = self.fetter_group_list[value.fetter_group] or {}
        table.insert(self.fetter_group_list[value.fetter_group], value)
    end
    for key, value in pairs(self.fetter_group_list) do
        table.sort(self.fetter_group_list[key], DataSorter.KeyLowerSorter("talisman_sort"))
    end
    
end
function FabaoData:ViewData()
    return self.view_data
end
function FabaoData:FabaoList()
    local list = {}
    local max = 999
    for i, v in ipairs(self.talisman_attribute) do
        if v.is_open ~= 0 then
            list[v.id] = self.talisman_compose[v.id]
            max = math.min(v.id, max)
        end
    end
    return list, max
end
function FabaoData:FocusFabaoList()
    local list = {}
    local max = 999
    for k, v in pairs(self.talisman_attribute) do
        if v.is_open ~= 0 then
            local vo = {}
            vo.attr_cfg = v
            vo.comp_cfg = self.talisman_compose[v.id]
            list[v.id] = vo
            max = math.min(v.id, max)
        end
    end

    return list, max
end

function FabaoData:FabaoInfo(id)
    return self.talisman_compose[id]
end
function FabaoData:FabaoAttrInfo(id)
    return self.talisman_attribute[id]
end
function FabaoData:SetFabaoBar(info)
    self.fabao_equip_bar:Set(info)
end
function FabaoData:ChangeFabaoList(index, info)
    for key, value in pairs(self.fabao_list) do
        value.play = false
    end
    if self.fabao_list[index] and info.fabao_level ~= self.fabao_list[index].fabao_level then
        info.play = true
    else
        info.play = false
    end
    self.fabao_list[index] = info
end
function FabaoData:EquipBar()
    return self.fabao_equip_bar
end
function FabaoData:SetFabaoList(info)
    for key, value in pairs(self.fabao_list) do
        if info[key].fabao_level ~= value.fabao_level then
            info[key].play = true
        else
            info[key].play = false
        end
    end
    self.fabao_list:Set(info)
    self.qiling_info:Notify()
end

function FabaoData:FabaoListInfo(index)
    return self.fabao_list[index]
end
function FabaoData:GetFabaoList()
    return self.fabao_list
end
function FabaoData:SetUsedCount(count)
    self.used_count = count
end
function FabaoData:UsedCount()
    return self.used_count
end

function FabaoData:GetCanUse()
    if next(self.fabao_equip_bar) == nil then
        return 0
    end
    for i, v in ipairs(self.fabao_equip_bar) do
        if v.id == -1 then
            return i - 1
        end
    end
    return 0
end
function FabaoData:IsEquip(id)
    for i, v in ipairs(self.fabao_equip_bar) do
        if v.id == id then
            return true, i - 1
        end
    end
    return false
end

function FabaoData:ExlainInfo(id, level)
    return self.fabao_level_list[id][level]
end
function FabaoData:XianQiItem()
    return self.other_info.xianqi_item
end

function FabaoData:UplevelInfo(level)
    return self.talisman_practice[level]
end
function FabaoData:XiuLianLimit()
    return self.other_info.exp_limit
end

function FabaoData:MaxJie()
    return self.other_info.max_advanced
end

function FabaoData:GetNextAdvanceLevel(level, jie_level)
    local cur_info = self.talisman_practice[level]
    if cur_info.is_need_advanced ~= jie_level and cur_info.advanced_level == jie_level then
        return level
    end

    for i = level, #self.talisman_practice do
        local test_info = self.talisman_practice[i]
        if test_info.is_need_advanced ~= jie_level and test_info.advanced_level == jie_level then
            return i
        end
    end
end
function FabaoData:GetSkillAsset(skill_type)
    for key, value in pairs(self.talisman_fight) do
        if value.skill_type == skill_type then
            if StringIsEmpty(value.asset_id) then
                return BattleSkillPool.ConstAssetId.UseFabao, true
            else
                return value.asset_id, false
            end
        end
    end
    return BattleSkillPool.ConstAssetId.UseFabao, true
end
function FabaoData:GetFabaoSkillInfo(skill_type)
    for key, value in pairs(self.talisman_fight) do
        if value.skill_type == skill_type then
            return value
        end
    end
    return nil
end

-- 返回当前法宝是否需要进阶
function FabaoData:GetNeedAdvanced(id)
    local fabao_info = self:FabaoListInfo(id)
    -- local info = self:FabaoAttrInfo(id)
    local exp_info = self:UplevelInfo(fabao_info.fabao_level)
    if exp_info == nil then
        return false
    end
    local need_advanced = exp_info.is_need_advanced - exp_info.advanced_level > 0
    if need_advanced and exp_info.advanced_level == fabao_info.fabao_jie then
        return true
    else
        return false
    end
end

--返回当前法宝是否满级
function FabaoData:GetHadMax(id)
    local fabao_info = self:FabaoListInfo(id)
    local attr_info = self:FabaoAttrInfo(id)
    local flag_level = attr_info.max_level == fabao_info.fabao_level
    local flag_jie = attr_info.max_advanced == fabao_info.fabao_jie
    return flag_level and flag_jie
end

function FabaoData:GetFabaoUpDesc(data, is_level)
    local level = data.info.fabao_level
    local jie_level = data.info.fabao_jie
    local attr_info = self:FabaoAttrInfo(data.id)

    local jie_plus =
        data.type == 1 and (attr_info.advanced_addition / 1000 * jie_level) or attr_info.advanced_addition * jie_level
    local add_cur = attr_info.base_addition + ((level * attr_info.level_addition) / 1000) + jie_plus
    local str_cur = string.format(attr_info.desc_1, add_cur)

    local str_next = ""
    if is_level then
        local add_next = attr_info.base_addition + ((level + 1) * attr_info.level_addition / 1000) + jie_plus
        str_next = string.format(attr_info.desc_1, add_next)
    else
        local jie_one_plus = data.type == 1 and (attr_info.advanced_addition / 1000) or attr_info.advanced_addition
        str_next = string.format(attr_info.addition_desc, add_cur + jie_one_plus)
    end

    return str_cur, str_next
end
function FabaoData:GetTypeList(type)
    return self.fabao_type_list[type]
end
function FabaoData:IsUnlock(id)
    local info = self.fabao_list[id]
    if info and info.fabao_index then
        return info.fabao_index <= -1
    end
    return true
end
function FabaoData:IsMax(id)
    local info = self.fabao_list[id]
    if info and info.fabao_level then
        return info.fabao_jie >= self.other_info.max_advanced and info.fabao_level >= self.other_info.max_level
    end
    return false
end
function FabaoData:UpgradeCfg(id, level)
    if level == nil then
        local info = self.fabao_list[id]
        return Cfg.FabaoUpgrade(id, info.fabao_level)
    end
    return Cfg.FabaoUpgrade(id, level)
end
function FabaoData:NextUpgradeCfg(id, level)
    if level == nil then
        local info = self.fabao_list[id]
        return self:UpgradeCfg(id, info.fabao_level + 1)
    end
    return self:UpgradeCfg(id, level)
end
function FabaoData:AdvanceCfg(id, fabao_jie)
    return Cfg.FabaoAdvance(id, fabao_jie)
end
function FabaoData:IsCanUp(id)
    local info = self.fabao_list[id]
    if info then
        local cfg = self:UpgradeCfg(id, info.fabao_level)
        if cfg == nil then
            return false
        end
        local cur_jie = math.floor(info.fabao_level / 3)
        return cfg[1].is_need_advanced == 1 and info.fabao_jie < cur_jie
    end
    return false
end
--已激活法宝的属性加成
function FabaoData:AttrAddition()
    local list = {}
    local temp
    local a, b
    for _, v in ipairs(self.fabao_list) do
        if not self:IsUnlock(v.id) then
            temp = self:UpgradeCfg(v.id, v.fabao_level)[1]
            for i = 1, 2 do
                a = temp["attr_num_" .. i]
                b = temp["attr_type_" .. i]
                if a > 0 then
                    list[b] = list[b] or 0
                    list[b] = list[b] + a
                end
            end
        end
    end
    return list
end
function FabaoData:UpgradeList(id)
    return self.fabao_upgrade_list[id]
end
function FabaoData:AdvanceList(id)
    return self.fabao_advance_list[id]
end



function FabaoData:PushJieupViewInfo(id,info)
    self.jieup_view_data = DataHelper.TableCopy(info)
    self.jieup_view_data.id = id
end

function FabaoData:PopJieupViewInfo()
    local re_val = self.jieup_view_data
    self.jieup_view_data = nil
    return re_val
end

function FabaoData:IsRemindUpgradEnable(id,info)
    if info.fabao_index >= 0 and
        not self:IsMax(id) then   --法宝已解锁且没满级
        local is_up_jie = self:IsCanUp(id,info.fabao_level)
        if is_up_jie then
            local cfg = self:AdvanceCfg(id, info.fabao_jie)
            if cfg == nil then
                return false
            end
            local advance = cfg[1]
            if BagData.Instance:GetNumByItemId(advance.consume_item_1) >= advance.consume_num_1 and
                BagData.Instance:GetNumByItemId(advance.consume_item_2) >= advance.consume_num_2 then
                return true
            end 
        else
            local cfg = self:UpgradeCfg(id, info.fabao_level)
            if cfg == nil then
                return false
            end
            local upgrade = cfg[1]
            --羁绊内满阶才可继续升级
            local fetters = self:FettersInfo(id)
            local res = self:FettersDesc(id, fetters.fetter_group)
            if res == 2 then
                return false
            end
            if BagData.Instance:GetNumByItemId(upgrade.consume_item) >= upgrade.consume_num then
                return true
            end
        end
    end
    return false
end

function FabaoData:RemindBagData()
    for id,info in pairs(self.fabao_list) do
        if self:IsRemindUpgradEnable(id,info) then
            return 1
        end
    end
    return 0
end

--返回主动[bool],被动[bool]是否能升级
function FabaoData:RemindTypeUpgradEnable()
    local initiative = nil  --主动标识
    local passivity = nil   --被动标识
    for id,info in pairs(self.fabao_list) do
        if initiative == nil and passivity == nil then
            if self:IsRemindUpgradEnable(id,info) then
                if self:FabaoAttrInfo(id).type == 1 then
                    initiative = true
                else
                    passivity = true
                end
            end
        else        --两个标识至少有一个已经被设置
            local fabao_cfg = self:FabaoAttrInfo(id)
            if (initiative ~= nil and fabao_cfg.type ~= 1) or 
                (passivity ~= nil and fabao_cfg.type == 1) then
                if self:IsRemindUpgradEnable(id,info) then
                    if fabao_cfg.type == 1 then
                        initiative = true
                    else
                        passivity = true
                    end
                    return initiative,passivity
                end
            end
        end
    end
    if initiative == nil then initiative = false end
    if passivity == nil then passivity = false end

    return initiative,passivity
end
function FabaoData:FettersInfo(id)
    return Cfg.FabaoFettersId(id)
end
function FabaoData:FettersGroup(group_id)
    return self.fetter_group_list[group_id]
end
function FabaoData:WillUpJie(group_id)
    local list = self:FettersGroup(group_id)
    local jie = 999
    for _, value in ipairs(list) do
        jie = math.min(self.fabao_list[value.id].fabao_jie, jie)
    end
    return jie + 1
end
function FabaoData:FettersDesc(id, group_id)
    --先判断有没有解锁或者已满级
    if self:IsUnlock(id) then
        return 1
    end
    if self.fabao_list[id].fabao_jie == self:MaxJie() then
        return 4
    end
    --羁绊内将升到的阶
    local jie = self:WillUpJie(group_id)
    -- local level = jie * 3
    if self.fabao_list[id].fabao_jie >= jie then
        return 2
    else
        return 3
    end
end
function FabaoData:FettersNum(group_id)
    local group = self:FettersGroup(group_id)
    local finish = 0
    for index, value in ipairs(group) do
        if self:FettersDesc(value.id, group_id) == 2 or self:FettersDesc(value.id, group_id) == 4 then
            finish = finish + 1
        end
    end
    return finish, table.nums(group)
end

--[变强]X件法宝达到Y级
function FabaoData:GetBianQiangFaBaoLevel(target_level)
    local max_num = 0
    local match_num = 0
    for k, list in pairs(self.fabao_type_list) do
        for i, v in pairs(list) do
            max_num = max_num + 1
            local info = self:FabaoListInfo(v.id)
            if info ~= nil and info.fabao_index >= 0 and info.fabao_level >= target_level then
                match_num = match_num + 1
            end
        end
    end
    return max_num, match_num
end

function FabaoData.GetUseFlag(item)
    if item:ShowType() == ShowType.FabaoSprite then
        local cfg = Cfg.TalismanAttributeByFragment(item.item_id)
        local info = FabaoData.Instance:FabaoListInfo(cfg.id)
        if info.fabao_index < 0 then
            return false
        else
            return true
        end
    elseif item:ShowType() == ShowType.FaBao then
        local item_cfg = Item.GetConfig(item.item_id)
        local id = item_cfg.param1
        local info = FabaoData.Instance:FabaoListInfo(id)
        if info.fabao_index < 0 then
            return false
        else
            return true
        end
    end
    return false
end
--=========================== 法宝器灵 ===========================
function FabaoData:GetQiLingUpCfg()
    if self.qiling_up == nil then
        local qiling_up_cfg = {}
        local cfg = Config.talisman_cfg_auto.qiling_up
        for _, v in pairs(cfg) do
            if not qiling_up_cfg[v.qiling_id] then
                qiling_up_cfg[v.qiling_id] = {}
            end
            qiling_up_cfg[v.qiling_id][v.level] = v
        end
        self.qiling_up = qiling_up_cfg
    end
    return self.qiling_up
end


function FabaoData:SetQiLingInfo(protocol)
    self.qiling_info.qi_ling_list=protocol.qi_ling_list
    self.qiling_info.put_on_list=protocol.put_on_list
    self.qiling_info.yuling_num=protocol.yuling_num
    self.qiling_info.put_on_index_list=protocol.put_on_index_list
    self.qiling_info.active_num=protocol.active_num
end

function FabaoData:RemindQiling()
    if self.qiling_info.yuling_num==nil then
        return 0
    end
    if self.qiling_info.yuling_num < self.qiling_info.active_num and self.qiling_info.yuling_num < FA_BAO_QI_LING_PUT_ON_MAX_NUM then
        return 1
    end
    for i, v in pairs(self.qiling_info.qi_ling_list) do
        local cfg = self:GetQiLingUpCfg()[v.id][v.level]
        if self:CheckRemindById(v.id) == 1 then
            return 1
        end
    end
    return 0
end


function FabaoData:CheckRemindById(qiling_id)
    local num = 0
    local level=self.qiling_info.qi_ling_list[qiling_id].level
    local next_cfg = self.qiling_up[qiling_id][level + 1]
    if next_cfg ~= nil and not self:IsUnlock(self.qiling_cfg[qiling_id + 1].fabao_id) then
        local has_num = Item.GetNum(next_cfg.expend_item)
        num = has_num >= next_cfg.expend_num and 1 or 0
    end
    return num
end


function FabaoData:IsYuLing(fabao_id)
    return self.qiling_info.put_on_list[fabao_id] ~= nil 
end

--器灵激活/升级需要消耗的物品 红点用
function FabaoData:GetQiLingRemingItemList()
    local item_list={}
    local un_repeat_list={}
    for k, v in pairs(Config.talisman_cfg_auto.qiling_up) do
        if un_repeat_list[v.expend_item] == nil then
            un_repeat_list[v.expend_item]=1
            table.insert(item_list,v.expend_item)
        end
    end
    return item_list
end

function FabaoData:GetRealLevel(qiling_id,level)
    local max_level= #self:GetQiLingUpCfg()[qiling_id]
    return level<=max_level and level or max_level
end
