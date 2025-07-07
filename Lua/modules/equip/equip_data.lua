EquipData = EquipData or BaseClass()

EquipSpiritType = {
    Chaos = 0,  --混沌
    Yin = 1,    --阴
    Yang = 2,   --阳
}

-- 根据品质颜色，每个品质对应五个不同方向的特效
SpiritEffectId = {
    [3] = {
        [0] = "3161001",
        [1] = "3161002",
        [2] = "3161003",
        [3] = "3161004",
        [4] = "3161005",
    },
    [4] = {
        [0] = "3161006",
        [1] = "3161007",
        [2] = "3161008",
        [3] = "3161009",
        [4] = "3161010",
    },
    [5] = {
        [0] = "3161011",
        [1] = "3161012",
        [2] = "3161013",
        [3] = "3161014",
        [4] = "3161015",
    },
}

-- 当前铸灵只有这几个颜色
EquipSpiritHasColor = {
    [1] = 3,
    [2] = 4,
    [3] = 5,
}

-- 混合特效
SpiritEffectBigId = {
    [3] = "3161016",    --蓝色
    [4] = "3161017",    --紫色
    [5] = "3161018",    --橙色
    [34] = "3161019",    --蓝紫
    [45] = "3161020",    --紫橙
}

-- 下方共鸣组合类型
ResonanceDownItemType = {
    None = 0,       --无
    Now = 1,        --当前
    Recommend = 2,  --推荐
}

-- 铸灵操作按钮
SpiritOperation = {
    Equip = 1,      --装备
    Level = 2,      --升级
    Remove = 3,     --卸下
    Resolve = 4,    --分解
}

-- 铸灵操作请求类型
SpiritReqType = {
    Level = 0,      --升级（该操作已换协议）
    Equip = 1,      --装备
    Remove = 2,     --卸下
}

-- xx在哪的类型
SpiritSrcType = {
    Bag = 0,        --背包里
    Equip = 1,      --装备里
    Jewelry = 2,    --灵饰里
}

-- 服务端给这个自定义了个类型，这边转换下
SpiritJewelryType = {
    [200] = 5,  --戒指
    [201] = 6,  --手镯
    [202] = 7,  --坠饰
}

function EquipData:__init()
    if EquipData.Instance ~= nil then
        Debuger.LogError("[EquipData] attempt to create singleton twice!")
        return
    end
    EquipData.Instance = self
    self:InitAllSpirirData()

    -- 当前所选装备
    self.now_select_equip_data = nil
    -- 当前所选装备的铸灵组合数据
    self.now_combine_data = nil
    -- 当前所选装备的铸灵列表颜色信息
    self.now_select_equip_spirit_color = nil
    -- 当前中间所选铸灵信息
    self.now_select_spirit_data = nil
    -- 当前所选装备的主要铸灵类型（如今铸灵列表中不会显示出自己不能装备的类型，已经没用，但逻辑仍然保留，不影响）
    self.now_equip_spirit_type = nil
    -- 是否显示所有已镶嵌
    self.is_show_equiped = false
    -- 当前左侧所选铸灵信息
    self.now_left_select_spirit = nil
    -- 点击中间五个铸灵时，手风琴记录当前点击的铸灵data
    self.now_accordion_need_select = nil
    -- 用过后存储，一个x级该铸灵等于多少个0级该铸灵
    self.spirit_equip_one_num_config = {}
    -- 用于记录重算背包内铸灵的时间戳，防止同一帧执行多次
    self.calculate_frame_count = 0
    -- 存储一个铸灵替换的方法，供中间五个选中的铸灵使用
    self.spirit_replace_fun = nil
    -- 预览铸灵组合所需数据
    self.equip_reasonance_data = SmartData.New({now_top_data = {}, now_left_data = {}})
    -- 预览单个铸灵组合所需数据
    self.small_reasonance_data = SmartData.New({now_select_data = {}})
    -- 铸灵升级界面数据（在背包中单个铸灵升级）（装备中的铸灵升级）（刚开始选中的铸灵信息）
    self.spirit_level_data = SmartData.New({spirit_level_bag = nil, spirit_level_equip = nil, spirit_level_config = nil, now_select_data = {}})
    -- 用于装备界面上的详情按钮显示刷新
    self.equip_detail_data = SmartData.New({is_on_1 = false, is_on_2 = false, info = {}})
    -- 铸灵分解界面数据
    self.spirit_resolve_data = SmartData.New({now_select_seq = -1, select_list = {}})

    self.chang_equip_item = SmartData.New()
	self.chang_equip_target_item = SmartData.New({item1 = nil, item2 = nil})
end

function EquipData:__delete()
    EquipData.Instance = nil
end

-- 重新登录执行
function EquipData:ResetAllData()
    self.is_show_equiped = true
    self:ClearAllData()
end

function EquipData:ClearAllData()
    self.now_select_equip_data = nil
    self.now_combine_data = nil
    self.now_select_equip_spirit_color = nil
    self.now_select_spirit_data = nil
    self.now_equip_spirit_type = nil
    self.now_left_select_spirit = nil
    self.now_accordion_need_select = nil
    self.spirit_replace_fun = nil
    self.equip_reasonance_data:Set({now_top_data = {}, now_left_data = {}})
    self.small_reasonance_data:Set({now_select_data = {}})
    self.spirit_level_data:Set({spirit_level_bag = nil, spirit_level_equip = nil, spirit_level_config = nil, now_select_data = {}})
    self.equip_detail_data:Set({is_on_1 = false, is_on_2 = false, info = {}})
    self.spirit_resolve_data:Set({now_select_seq = -1, select_list = {}})
end

function EquipData:InitAllSpirirData()
    local config = Config.equip_spirit_auto
    -- 处理铸灵id对应的铸灵种类表（把相同铸灵分组）
    self.spirit_id_by_group = {}
    for k, v in pairs(config.spirit_stone) do
        self.spirit_id_by_group[v.spirit_stone_id] = v.type
    end

    -- 处理单个铸灵种类的等级配置表
    self.spirit_stone_level_config = DataHelper.Subdivision(config.spirit_stone, {"type", "spirit_stone_level"})

    -- 处理根据铸灵id获取配置表
    Cfg.Register("SpiritStoneConfigById", Cfg.RealTimeSearch(config.spirit_stone, "spirit_stone_id"))

    -- 处理铸灵分解表
    self.spirit_stone_resolve_config = DataHelper.Subdivision(config.spirit_stone_resolve, {"spirit_stone_type", "color", "level"})

    -- 处理共鸣组合表，该表根据组合id来获取对应组合
    Cfg.Register("SpiritStoneCombineConfig", Cfg.RealTimeSearch(config.resonance, "resonance_group_id"))

    -- 处理共鸣组合表1
    self.spirit_stone_combine_config1 = DataHelper.Subdivision(config.resonance, {"need_num", "sort"})

    -- 处理共鸣组合表2，该表将共鸣组合根据孔位排序好
    self.spirit_stone_combine_config2 = {}
    for k, v in pairs(config.resonance) do
        table.insert(self.spirit_stone_combine_config2, v)
    end
    table.sort(self.spirit_stone_combine_config2, function(a_data, b_data)
        return a_data.need_num * 100 + a_data.sort < b_data.need_num * 100 + b_data.sort
    end)

    -- 处理共鸣组合表3，该表根据组合品质颜色来获取对应组合（废除）
    -- self.spirit_stone_combine_config3 = {}
    -- for k, v in pairs(config.resonance) do
    --     local has_color = {}
    --     local index = 1
    --     local color_index = ""
    --     while v["color"..index] do
    --         if v["color"..index] ~= 0 then
    --             table.insert(has_color, {color = v["color"..index]})
    --         else
    --             break
    --         end
    --         index = index + 1
    --     end
    --     for j, w in ipairs(has_color) do
    --         color_index = color_index .. w.color
    --     end
    --     self.spirit_stone_combine_config3[color_index] = v
    -- end
    -- 处理共鸣组合加成表
    self.spirit_stone_combine_attr_config = {}
    for k, v in pairs(config.resonance_attr) do
        local job_list = Split(v.job_type, "|")
        local equip_list = Split(v.equip_type, "|")
        for _, w in pairs(job_list) do
            local num_w = tonumber(w)
            if not self.spirit_stone_combine_attr_config[num_w] then
                self.spirit_stone_combine_attr_config[num_w] = {}
            end
            for _, n in pairs(equip_list) do
                local num_n = tonumber(n)
                num_n = SpiritJewelryType[num_n] or num_n
                if not self.spirit_stone_combine_attr_config[num_w][num_n] then
                    self.spirit_stone_combine_attr_config[num_w][num_n] = {}
                end
                if not self.spirit_stone_combine_attr_config[num_w][num_n][v.resonance_group_id] then
                    self.spirit_stone_combine_attr_config[num_w][num_n][v.resonance_group_id] = {}
                end
                self.spirit_stone_combine_attr_config[num_w][num_n][v.resonance_group_id][v.resonance_level] = v
            end
        end
    end
    -- 处理共鸣组合对应大的特效表
    self.effect_id_by_combine = {}
    for k, v in pairs(config.resonance) do
        local has_color = {}
        local index = 1
        local last = 0
        local effect_id = ""
        while v["color"..index] do
            if v["color"..index] ~= 0 then
                if last ~= v["color"..index] then
                    last = v["color"..index]
                    table.insert(has_color, {color = last})
                end
            else
                break
            end
            index = index + 1
        end
        for j, w in ipairs(has_color) do
            effect_id = effect_id .. w.color
        end
        self.effect_id_by_combine[v.resonance_group_id] = tonumber(effect_id)
    end
    -- 处理共鸣组合推荐表
    Cfg.Register("EquipSpiritResonanceRecommendCfg", Cfg.RealTimeSearch(config.resonance_recommend, "color"))
    -- 处理装备类型对应允许穿戴的铸灵类型
    self.spirit_type_by_equip_type = {}
    for k, v in pairs(config.wear_restrictions) do
        local index = SpiritJewelryType[v.equip_type] or v.equip_type
        -- 这里要分割，只读第一个，第二个混沌默认加上
        local spirit_stone_type_list = Split(v.spirit_stone_type, "|")
        self.spirit_type_by_equip_type[index] = tonumber(spirit_stone_type_list[1])
    end

    --处理转换表 分装备转换和灵饰转换
    Cfg.Register("EquipConversionSearch",Cfg.RealTimeSearch(Config.compose_auto.equip_tran_list, "id"))
    local list = Config.compose_auto.target_equip_tran_list
    self.equip_change_map = {}
    local cfg 
    for i, v in ipairs(list) do
        cfg = nil
        if v.type == 1 then
            cfg = Config.equipment_auto[v.targe_id]
        else
            cfg = Config.ornament_item_auto[v.targe_id]
        end
        if cfg then
            self.equip_change_map[cfg.limit_prof] = self.equip_change_map[cfg.limit_prof] or {}
            self.equip_change_map[cfg.limit_prof][cfg.equip_type] = self.equip_change_map[cfg.limit_prof][cfg.equip_type] or {}
            self.equip_change_map[cfg.limit_prof][cfg.equip_type][cfg.limit_level] = self.equip_change_map[cfg.limit_prof][cfg.equip_type][cfg.limit_level] or {}
            table.insert(self.equip_change_map[cfg.limit_prof][cfg.equip_type][cfg.limit_level], cfg)
        end
    end
end

-- 获得铸灵类型分组id
function EquipData:GetSpiritGroup(item_id)
    if item_id then
        if self.spirit_id_by_group[item_id] then
            return self.spirit_id_by_group[item_id]
        end
        return
    end
    return self.spirit_id_by_group
end

-- 获得铸灵升级配置
function EquipData:GetSpiritLevelData(item_id, level)
    if item_id then
        local group = self:GetSpiritGroup(item_id)
        if group then
            if level then
                if self.spirit_stone_level_config[group][level] then
                    return self.spirit_stone_level_config[group][level]
                end
                return
            end
            return self.spirit_stone_level_config[group]
        end
        LogError("铸灵道具id有误，id", item_id)
    end
    LogError("获取铸灵升级配置必须传铸灵id")
end

-- 判断该铸灵是否满级
function EquipData:GetSpiritIsMaxLevel(item_id, level)
    return self:GetSpiritLevelData(item_id, level + 1) == nil
end

-- 获得铸灵类型
function EquipData:GetSpiritType(item_id)
    local config = self:GetSpiritLevelData(item_id)
    if config then
        return config[0].spirit_stone_type
    end
    LogError("获取铸灵类型错误，item_id", item_id)
end

-- 获得铸灵升级属性配置
function EquipData:SpiritLevelAttr(item_id, level)
    local config = self:GetSpiritLevelData(item_id, level)
    return self:GetAttrList(config, "type_", "value_")
end

-- 获得铸灵组合升级属性配置
function EquipData:SpiritCombineAttr(equip_type, resonance_group_id, level)
    local config = self:GetSpiritCombineAttrData(equip_type, resonance_group_id, level)
    local tab = {}
    tab.base_attr = self:GetAttrList(config, "type_", "value_")
    tab.extra_attr = self:GetAttrList(config, "extra_type_", "extra_value_")
    return tab
end

-- 计算铸灵列表中的铸灵属性
function EquipData:TotalSpiritListAttr(spirit_list)
    local attr_list = {}
    for k, v in pairs(spirit_list) do
        local attrs = self:SpiritLevelAttr(v.item_id, v.level)
        for l, w in pairs(attrs) do
            if not attr_list[w.attr_type] then
                attr_list[w.attr_type] = {}
                attr_list[w.attr_type].attr_type = w.attr_type
                attr_list[w.attr_type].attr_name = w.attr_name
                attr_list[w.attr_type].attr_value = w.attr_value
            else
                attr_list[w.attr_type].attr_value = attr_list[w.attr_type].attr_value + w.attr_value
            end
        end
    end
    for k, v in pairs(attr_list) do
        v.attr_percent_value = DataHelper.Percent(v.attr_type, v.attr_value)
    end
    return attr_list
end

-- 计算共鸣属性
function EquipData:TotalSpiritCombineAttr(spirit_combine_attr)
    local attr_list = {}
    -- 只有共鸣属性需要计算叠加，这块不应该有那种百分比的属性（物伤减免等）加成
    for k, v in pairs(spirit_combine_attr.extra_attr) do
        if not attr_list[v.attr_type] then
            attr_list[v.attr_type] = {}
            attr_list[v.attr_type].attr_type = v.attr_type
            attr_list[v.attr_type].attr_name = v.attr_name
            attr_list[v.attr_type].attr_value = v.attr_value
        else
            attr_list[v.attr_type].attr_value = attr_list[v.attr_type].attr_value + v.attr_value
        end
    end
    spirit_combine_attr.extra_attr = attr_list
end

-- 将有共鸣铸灵属性的铸灵列表处理成显示文本list
function EquipData:DealSpiritByCombineAttrToTxt(spirit_list, combine_lst, language1, language2)
    local tab = {}
    for i, v in pairs(spirit_list) do
        local t = {}
        if combine_lst.extra_attr[v.attr_type] then
            t.txt = Format(language2, v.attr_name, v.attr_percent_value, combine_lst.extra_attr[v.attr_type].attr_value / 100)
        else
            t.txt = Format(language1, v.attr_name, v.attr_percent_value)
        end
        t.attr_type = v.attr_type
        table.insert(tab, t)
    end
    return tab
end

-- 将有共鸣铸灵属性的铸灵列表处理成显示文本list（用于装备对比上）
function EquipData:DealSpiritByCombineAttrToTxt2(spirit_list, combine_lst, language1, language2)
    local tab = {}
    for i, v in pairs(spirit_list) do
        local t = {}
        if combine_lst.extra_attr[v.attr_type] then
            t.txt = Format(language2, v.attr_name, v.attr_percent_value)
        else
            t.txt = Format(language1, v.attr_name, v.attr_percent_value)
        end
        t.attr_type = v.attr_type
        table.insert(tab, t)
    end
    return tab
end

-- 将共鸣属性列表处理成显示文本list
function EquipData:DealCombineAttrToTxt(attr_list, language1, language2)
    local tab = {}
    for i, v in pairs(attr_list.base_attr) do
        local t = {}
        t.txt = Format(language1, v.attr_name, v.attr_percent_value)
        t.attr_type = v.attr_type
        table.insert(tab, t) 
    end
    for i, v in pairs(attr_list.extra_attr) do
        local t = {}
        t.txt = Format(language2, v.attr_name, v.attr_value / 100)
        table.insert(tab, t)
    end
    return tab
end

-- 将升级的共鸣属性列表处理成显示文本list
function EquipData:DealLevelCombineAttrToTxt(attr_list1, attr_list2, language1, language2)
    local tab = {}
    for i, v in pairs(attr_list1.base_attr) do
        local t = {}
        t.txt = Format(language1, v.attr_name, v.attr_percent_value, attr_list2.base_attr[i].attr_percent_value)
        t.attr_type = v.attr_type
        table.insert(tab, t)
    end
    for i, v in pairs(attr_list1.extra_attr) do
        local t = {}
        t.txt = Format(language2, v.attr_name, v.attr_value / 100, attr_list2.extra_attr[i].attr_value / 100)
        table.insert(tab, t)
    end
    return tab
end

-- 将铸灵属性列表处理成显示文本list
function EquipData:DealSpiritAttrToTxt(attr_list, language)
    local tab = {}
    for i, v in pairs(attr_list) do
        local t = {}
        t.txt = Format(language, v.attr_name, v.attr_percent_value)
        t.attr_type = v.attr_type
        table.insert(tab, t)
    end
    return tab
end

-- 获得属性配置列表
function EquipData:GetAttrList(config, key_part, value_part)
    local tab = {}
    local index = 1
    while config[value_part .. index] do
        if config[value_part .. index] > 0 then
            local attr_type_list = Split(config[key_part .. index], "|")
            if #attr_type_list > 1 then
                for i, v in ipairs(attr_type_list) do
                    local t = CommonStruct.AttrStruct(tonumber(v), config[value_part .. index])
                    table.insert(tab, t)
                end
            else
                local t = CommonStruct.AttrStruct(config[key_part .. index], config[value_part .. index])
                table.insert(tab, t)
            end
        end
        index = index + 1
    end
    return tab
end

-- 获得铸灵分解配置
function EquipData:GetSpiritResolveData(spirit_type, color, level)
    if spirit_type then
        if self.spirit_stone_resolve_config[spirit_type] then
            if color then
                if self.spirit_stone_resolve_config[spirit_type][color] then
                    if level then
                        if self.spirit_stone_resolve_config[spirit_type][color][level] then
                            return self.spirit_stone_resolve_config[spirit_type][color][level]
                        end
                        return
                    end
                    return self.spirit_stone_resolve_config[spirit_type][color]
                end
                return
            end
            return self.spirit_stone_resolve_config[spirit_type]
        end
        return
    end
    return self.spirit_stone_resolve_config
end

-- 根据列表获取能获得多少粉尘
function EquipData:GetCanGetDustNum(data)
    local num = 0
    if not TableIsEmpty(data) then
        for k, v in pairs(data) do
            local config = Item.GetConfig(v.item.item_id)
            local m_type = self:GetSpiritType(v.item.item_id)
            local config2 = Cfg.SpiritStoneConfigById(v.item.item_id)
            local resolve_config = self:GetSpiritResolveData(m_type, config.color, config2.spirit_stone_level)
            num = num + resolve_config.resolve_num * v.now_select_num
        end
    end
    return num
end

-- 获取孔位是否解锁，解锁等级，这里孔位id从0开始
function EquipData:GetSlotIsOpen(solt_id)
    for k, v in pairs(Config.equip_spirit_auto.limit_level) do
        if v.solt_id == solt_id then
            return RoleData.Instance:GetRoleLevel() >= v.limit_level, v.limit_level
        end
    end
    LogError("铸灵孔位出错，孔位id不能为", solt_id)
end

-- 获取已解锁多少个孔位
function EquipData:GetOpenSlotNum()
    local num = 0
    for i, v in ipairs(Config.equip_spirit_auto.limit_level) do
        if RoleData.Instance:GetRoleLevel() >= v.limit_level then
            num = num + 1
        else
            break
        end
    end
    return num
end

-- 获得共鸣组合配置列表（根据孔数）
function EquipData:GetSpiritCombineData(need_num)
    if need_num then
        if self.spirit_stone_combine_config1[need_num] then
            return self.spirit_stone_combine_config1[need_num]
        end
        return
    end
    return self.spirit_stone_combine_config1
end

-- 获得共鸣组合配置列表和共鸣等级（根据组合颜色和铸灵列表）
-- 现在共鸣组合会向下兼容，这里会计算出当前能激活的最好的组合
function EquipData:GetSpiritCombineDataByColorList(color_list, spirit_list)
    table.sort(color_list, function(data_a, data_b)
        return data_a.color < data_b.color
    end)

    local now_color_num = #color_list
    -- 计算当前对应颜色品质的数量
    local now_color_group_list = {}
    for i, v in ipairs(EquipSpiritHasColor) do
        now_color_group_list[v] = 0
    end
    for k, v in pairs(color_list) do
        now_color_group_list[v.color] = now_color_group_list[v.color] + 1
    end
    while now_color_num ~= 1 do
        -- 查找对应组合
        local spirit_stone_combine_config = self:GetSpiritCombineData(now_color_num)
        -- 增加容错
        if TableIsEmpty(spirit_stone_combine_config) then
            return
        end
        local spirit_stone_combine_config_num = #spirit_stone_combine_config
        for i = spirit_stone_combine_config_num, 1, -1 do
            -- 查找对应组合
            local is_right = true
            for k, v in pairs(EquipSpiritHasColor) do
                if spirit_stone_combine_config[i]["color_constitute_"..v] > now_color_group_list[v] then
                    is_right = false
                    break
                end
            end
            if is_right then
                -- 查到组合后，判断是否要获得共鸣等级
                if spirit_list then
                    -- 获取共鸣组合等级，规则：先找对应颜色品质等级最高的组合成当前共鸣，然后在当前组合中找到等级最低的来作为共鸣等级
                    -- 将铸灵列表按颜色分组
                    local spirit_color_list = {}
                    local level = -1
                    for k, v in pairs(EquipSpiritHasColor) do
                        if spirit_stone_combine_config[i]["color_constitute_"..v] ~= 0 then
                            spirit_color_list[v] = {}
                            for l, w in pairs(spirit_list) do
                                if Item.GetConfig(w.item_id).color == v then
                                    table.insert(spirit_color_list[v], w)
                                end
                            end
                            -- 如果该颜色数量刚刚好就不用排序了
                            local need_num, now_num = spirit_stone_combine_config[i]["color_constitute_"..v], #spirit_color_list[v]
                            if need_num == now_num then
                                for m, x in pairs(spirit_color_list[v]) do
                                    if level == -1 then
                                        level = x.level
                                    else
                                        level = level > x.level and x.level or level
                                    end
                                end
                            else
                                table.sort(spirit_color_list[v], DataSorter.KeyLowerSorter("level"))
                                local min_level = spirit_color_list[v][now_num - need_num + 1].level
                                if level == -1 then
                                    level = min_level
                                else
                                    level = level > min_level and min_level or level
                                end
                            end
                        end
                    end
                    return spirit_stone_combine_config[i], level
                end
                return spirit_stone_combine_config[i]
            end
        end
        now_color_num = now_color_num - 1
    end
end

-- 获取对应装备能装什么类型的铸灵
function EquipData:GetEquipSpiritType(equip_type)
    if equip_type and self.spirit_type_by_equip_type[equip_type] then
        return self.spirit_type_by_equip_type[equip_type]
    end
    LogError("装备类型错误" .. equip_type)
end

-- 设置当前所选装备的铸灵列表颜色信息
function EquipData:SetNowEquipSpiritColor()
    self.now_select_equip_spirit_color = {}
    if not TableIsEmpty(self.now_select_equip_data) then
        local spirit_list = self.now_select_equip_data:GetHasSpiritList()
        for k, v in pairs(spirit_list) do
            local config = Item.GetConfig(v.item_id)
            table.insert(self.now_select_equip_spirit_color, {color = config.color, slot = v.slot})
        end
    end
end

-- 获得当前职业的铸灵组合属性配置
function EquipData:GetSpiritCombineAttrData(equip_type, resonance_group_id, level)
    local job_type = RoleData.Instance:GetRoleProfType()
    if self.spirit_stone_combine_attr_config[job_type] then
        if equip_type then
            if self.spirit_stone_combine_attr_config[job_type][equip_type] then
                if resonance_group_id then
                    if self.spirit_stone_combine_attr_config[job_type][equip_type][resonance_group_id] then
                        if level then
                            if self.spirit_stone_combine_attr_config[job_type][equip_type][resonance_group_id][level] then
                                return self.spirit_stone_combine_attr_config[job_type][equip_type][resonance_group_id][level]
                            end
                            return
                        end
                        return self.spirit_stone_combine_attr_config[job_type][equip_type][resonance_group_id]
                    end
                    return
                end
                return self.spirit_stone_combine_attr_config[job_type][equip_type]
            end
            return
        end
        return self.spirit_stone_combine_attr_config[job_type]
    else
        LogError("找不到该职业id" .. job_type)
    end
end

-- 判断该铸灵组合是否满级
function EquipData:GetSpiritCombineIsMaxLevel(equip_type, resonance_group_id, level)
    return self:GetSpiritCombineAttrData(equip_type, resonance_group_id, level + 1) == nil
end

-- 获得铸灵组合特效配置
function EquipData:GetCombineEffectData(resonance_group_id)
    if resonance_group_id then
        if self.effect_id_by_combine[resonance_group_id] then
            return self.effect_id_by_combine[resonance_group_id]
        end
        return
    end
    return self.effect_id_by_combine
end

-- 获取装备列表，包括身上的装备
function EquipData:GetEquipList()
    local list1 = BagData.Instance:GetEquipList()
    local list2 = BagData.Instance:GetJewelryList()
    local list3 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor, ItemType.Jewelry})
    
    local new_list = {}
    for index, value in pairs(list1) do
        table.insert(new_list, value)
    end
    for index, value in pairs(list2) do
        table.insert(new_list, value)
    end
    for index, value in ipairs(list3) do
        table.insert(new_list, value)
    end
    return new_list
end

-- 获得单条共鸣组合数据中的颜色组合数据（是否需要增加判断显示已装备的圆圈底）
function EquipData:GetColorListByCombine(config, is_just_equip)
    local tab = {}
    local index = 1
    while config["color"..index] do
        if config["color"..index] ~= 0 then
            table.insert(tab, {color = config["color"..index]})
        else
            break
        end
        index = index + 1
    end
    if is_just_equip and self.now_select_equip_data then
        local spirit_list = self.now_select_equip_data:GetHasSpiritList()
        for k, v in pairs(spirit_list) do
            local m_config = Item.GetConfig(v.item_id)
            for i, w in ipairs(tab) do
                if not w.has and w.color == m_config.color then
                    w.has = true
                    break
                end
            end
        end
    end
    return tab
end

-- 获取下方共鸣组合表，排序，区分当前和推荐
function EquipData:GetResonanceDownItemData()
    if not self.now_select_equip_data then
        return {}
    end
    local recommend_combine = -1
    local now_combine = self.now_combine_data and self.now_combine_data.combine_attr.resonance_group_id or -1
    local tab = {}
    if now_combine ~= -1 then
        table.insert(tab, {m_type = ResonanceDownItemType.Now, config = self.now_combine_data.combine_config})
    end

    local spirit_list = self.now_select_equip_data:GetHasSpiritList(true)
    local color_str = nil
    if not TableIsEmpty(spirit_list) then
        color_str = ""
        for i, v in ipairs(spirit_list) do
            local config = Item.GetConfig(v.item_id)
            color_str = color_str .. config.color
        end
        local recommend_config = Cfg.EquipSpiritResonanceRecommendCfg(tonumber(color_str), false)
        recommend_combine = recommend_config and recommend_config.resonance_group_id or -1
    end

    if recommend_combine ~= -1 then
        table.insert(tab, {m_type = ResonanceDownItemType.Recommend, config = DataHelper.GetSearch(Config.equip_spirit_auto.resonance, "resonance_group_id", recommend_combine)})
    end
    for i, v in ipairs(self.spirit_stone_combine_config2) do
        if v.resonance_group_id ~= now_combine and v.resonance_group_id ~= recommend_combine then
            table.insert(tab, {m_type = ResonanceDownItemType.None, config = v})
        end
    end
    return tab
end

-- 获取左侧铸灵信息列表
function EquipData:GetSpiritItemData()
    local tab = {}
    local in_bag_list = BagData.Instance:GetBagSpiritList()
    local now_equip_can_type = self:GetEquipSpiritType(self.now_select_equip_data:EquipType())
    for k, v in pairs(Language.Equip.SpiritListColor) do
        tab[k] = {}
        tab[k].name = v
        tab[k].id = k
        tab[k].children = {}
    end
    local t1 = {}
    local t2 = {}
    -- 按品质分类
    for k, v in pairs(in_bag_list) do
        -- 要用到continue的效果，下同
        while true do
            -- 根据当前装备类型来判断能放阴还是阳类型的铸灵
            if self:GetSpiritType(v.item_id) ~= EquipSpiritType.Chaos and self:GetSpiritType(v.item_id) ~= now_equip_can_type then
                break
            end
            local color = Item.GetConfig(v.item_id).color
            if not t1[color] then
                t1[color] = {}
            end
            local _t1 = {}
            _t1.item_id = v.item_id
            _t1.level = v.param.level
            _t1.slot = v.param.slot
            table.insert(t1[color], {item = v, config = _t1})
            break
        end
    end
    -- 根据等级和铸灵类型排序
    for k, v in pairs(t1) do
        table.sort(v, function(a_data, b_data)
            local a_index = self:CalculateSpiritIndex(a_data.item.item_id) + a_data.config.level
            local b_index = self:CalculateSpiritIndex(b_data.item.item_id) + b_data.config.level
            return a_index > b_index
        end)
    end

    -- 显示所有已镶嵌，需要遍历所有的装备
    if self.is_show_equiped then
        local equip_list = self:GetEquipList()
        for k, v in pairs(equip_list) do
            local spirit_list = v:GetHasSpiritList()
            if not TableIsEmpty(spirit_list) then
                for l, w in pairs(spirit_list) do
                    while true do
                        -- 根据当前装备类型来判断能放阴还是阳类型的铸灵
                        if self:GetSpiritType(w.item_id) ~= EquipSpiritType.Chaos and self:GetSpiritType(w.item_id) ~= now_equip_can_type then
                            break
                        end
                        local color = Item.GetConfig(w.item_id).color
                        if not t2[color] then
                            t2[color] = {}
                        end
                        -- 这里筛选出当前装备中选中的铸灵
                        if self.now_select_spirit_data.item_id ~= 0
                        and BagData.Instance:JudgeEquipIsEqual(self.now_select_equip_data, v)
                        and self.now_select_spirit_data.slot == w.slot then
                            self.now_accordion_need_select = {equip = self.now_select_equip_data, config = self.now_select_spirit_data}
                            table.insert(t2[color], self.now_accordion_need_select)
                        else
                            table.insert(t2[color], {equip = v, config = w})
                        end
                        break
                    end
                end
            end
        end
        for k, v in pairs(t2) do
            table.sort(v, function(a_data, b_data)
                local a_index = self:CalculateSpiritIndex(a_data.config.item_id) + a_data.config.level
                local b_index = self:CalculateSpiritIndex(b_data.config.item_id) + b_data.config.level
                return a_index > b_index
            end)
        end
    else
        if self.now_select_spirit_data.item_id ~= 0 then
            -- 就算不显示所有已镶嵌，也要把当前选中的铸灵放进去
            local color = Item.GetConfig(self.now_select_spirit_data.item_id).color
            t2[color] = {}
            self.now_accordion_need_select = {equip = self.now_select_equip_data, config = self.now_select_spirit_data}
            table.insert(t2[color], self.now_accordion_need_select)
        end
    end
    -- 整合放入children
    for k, v in pairs(tab) do
        if not TableIsEmpty(t1[k]) then
            for l, w in ipairs(t1[k]) do
                table.insert(v.children, w)
            end
        end
        if not TableIsEmpty(t2[k]) then
            for l, w in ipairs(t2[k]) do
                table.insert(v.children, w)
            end
        end
    end
    -- 检查children是否为空，为空的先去掉
    local t4 = {}
    for k, v in pairs(tab) do
        if TableIsEmpty(v.children) then
            tab[k] = nil
        end
    end

    -- 除掉空后给index赋值
    local num = table.nums(Language.Equip.SpiritListColor)
    local parent_index = 0
    for i = 2 + num, 3, -1 do
        if not TableIsEmpty(tab[i]) then
            parent_index = parent_index + 1
            local children_index = 0
            for j, v in ipairs(tab[i].children) do
                children_index = children_index + 1
                v.children_index = children_index
                v.parent_index = parent_index
            end
        end
    end
    return tab
end

-- 计算该铸灵阴阳优先级，用于排序
function EquipData:CalculateSpiritIndex(item_id)
    local index = 0
    local m_type = self:GetSpiritType(item_id)
    if m_type == EquipSpiritType.Chaos then
        index = 1000
    else
        if self.now_equip_spirit_type == EquipSpiritType.Yang then
            index = m_type == EquipSpiritType.Yang and 500 or 0
        else
            index = m_type == EquipSpiritType.Yang and 0 or 500
        end
    end
    return index
end

-- 打开左侧手风琴时显示哪
function EquipData:GetSpiritJumpData(data)
    -- 这里不能计算data的数量，因为有可能被移除了
    local num = table.nums(Language.Equip.SpiritListColor)
    -- 用于记录第一个有铸灵children的数据
    -- 如果当前选的铸灵不是空，就要定位到那个铸灵上
    local is_first = true
    local first_has = {}
    -- 现在如果当前选中装备上的铸灵不是空格的话也不用去跳转到当前了
    -- if self.now_select_spirit_data.item_id ~= 0 then
    --     first_has = self.now_accordion_need_select
    -- else
        -- 颜色品质最低是3，蓝色
        for i = 2 + num, 3, -1 do
            if not TableIsEmpty(data[i]) then
                local children = data[i].children
                if not TableIsEmpty(children) then
                    if is_first then
                        is_first = false
                        first_has = children[1]
                    end
                    if children[1].item ~= nil then
                        if self:GetSpiritType(children[1].item.item_id) == self.now_equip_spirit_type or self:GetSpiritType(children[1].item.item_id) == EquipSpiritType.Chaos then
                            return children[1]
                        end
                    else
                        if self:GetSpiritType(children[1].config.item_id) == self.now_equip_spirit_type or self:GetSpiritType(children[1].config.item_id) == EquipSpiritType.Chaos then
                            return children[1]
                        end
                    end
                end
            end
        end
    -- end
    return first_has
end

-- 获取分解列表data
function EquipData:GetSpiritResolveListData()
    local tab = {}
    local bag_spiirt_list = BagData.Instance:GetBagSpiritList()
    for i, v in ipairs(bag_spiirt_list) do
        local t = {}
        t.item = {}
        t.seq = i   -- 用于知道是数组第几个
        t.item.item_id = v.item_id
        t.item.is_bind = v.is_bind and 1 or 0
        t.item.num = v.num
        t.column = v:GridColumn()
        t.index = v.index
        t.now_select_num = 0
        table.insert(tab, t)
    end
    return tab
end

-- 铸灵分解协议下发
function EquipData:GetSpiritResolveInfo(protocol)
    -- 粉尘服务端飘
    -- PublicPopupCtrl.Instance:CenterI({item_id = CommonItemId.DustCoin, num = protocol.fen_chen_num})
    -- local dust_coin = Item.Init(CommonItemId.DustCoin, protocol.fen_chen_num)
    -- ChatCtrl.Instance:ChannelChatCustom{item_get = {item = dust_coin, num = protocol.fen_chen_num}}
    if protocol.count > 0 then
        -- 飘意外惊喜
        local item_list = {}
        for i = 1, protocol.count do
            item_list[i] = {item_id = protocol.item_id_list[i].item_id, is_bind = 1, num = protocol.item_id_list[i].num}
        end
        MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(item_list), GIFT_OPEN_TYPE.SURPRISE)
    end
end

-- 获取当前铸灵相当于多少个0级的铸灵
function EquipData:GetSpiritEquipOneNum(item_id)
    if not self.spirit_equip_one_num_config[item_id] then
        local group_level_config = self:GetSpiritLevelData(item_id)
        local group_num = table.nums(group_level_config)
        local need_num = 1
        for i = 0, group_num - 1 do
            local now_group_level_config = group_level_config[i]
            if i ~= 0 then
                need_num = need_num * now_group_level_config.need_num
            end
            self.spirit_equip_one_num_config[now_group_level_config.spirit_stone_id] = need_num
        end
    end
    return self.spirit_equip_one_num_config[item_id]
end

-- 铸灵红点（传了装备就是计算该装备是否有红点）
function EquipData:GetSpiritRemind(equip)
    if not FunOpen.Instance:GetFunIsOpened(Mod.Equip.Spirit) then
        return 0
    end
    self.open_slot_num = self:GetOpenSlotNum()
    -- 没有孔位开放没有红点
    if self.open_slot_num == 0 then
        return 0
    end
    -- 背包没有铸灵就没有红点
    self.now_bag_spirit_list = BagData.Instance:GetBagSpiritList()
    if TableIsEmpty(self.now_bag_spirit_list) then
        return 0
    end
    self:ReCalculateSpiritInBag(self.now_bag_spirit_list)
    if not equip then
        -- 获取身上的装备列表
        local list1 = BagData.Instance:GetEquipList()
        if self:GetEquipListRemind(list1) == 1 then
            return 1
        end
        -- 获取身上的灵饰列表
        local list2 = BagData.Instance:GetJewelryList()
        return self:GetEquipListRemind(list2)
    else
        local list3 = {}
        table.insert(list3, equip)
        return self:GetEquipListRemind(list3)
    end
end

-- 获取该装备列表中的红点
function EquipData:GetEquipListRemind(list)
    for k, v in pairs(list) do
        -- 判断是否有孔位能装备
        local spirit_list = v:GetSpiritList()
        local m_type = self:GetEquipSpiritType(v:EquipType())
        for l, w in ipairs(spirit_list) do
            -- 有孔位，判断类型是否符合
            if w.item_id == 0 and self.open_slot_num > w.slot then
                if self.bag_spirit_all_type == EquipSpiritType.Chaos or m_type == EquipSpiritType.Chaos then
                    return 1
                elseif self.bag_spirit_all_type == m_type then
                    return 1
                else
                    break
                end
            end
        end
        -- 判断是否能升级
        local equiped_spirit_list = v:GetHasSpiritList()
        for _, w in pairs(equiped_spirit_list) do
            if self:JudgeEquipSpiritCanLevelUp(w.item_id, w.level) then
                return 1
            end
        end
    end
    return 0
end

-- 重算背包内的铸灵
function EquipData:ReCalculateSpiritInBag(now_bag_spirit_list)
    -- 防止同一帧执行多次
    if self.calculate_frame_count ~= Time.frameCount then
        self.calculate_frame_count = Time.frameCount
    else
        return
    end
    self.now_bag_spirit_list = now_bag_spirit_list or BagData.Instance:GetBagSpiritList()
    -- 拿到铸灵列表后处理，这样就不用每次去遍历计算对应的铸灵数量
    -- 根据类型和等级的存起来当前所拥有的数量
    self.now_bag_spirit_num_list = {}
    for k, v in pairs(self.now_bag_spirit_list) do
        local now_group = self:GetSpiritGroup(v.param.item_id)
        if not self.now_bag_spirit_num_list[now_group] then
            self.now_bag_spirit_num_list[now_group] = {}
        end
        if not self.now_bag_spirit_num_list[now_group][v.param.level] then
            self.now_bag_spirit_num_list[now_group][v.param.level] = 0
        end
        self.now_bag_spirit_num_list[now_group][v.param.level] = self.now_bag_spirit_num_list[now_group][v.param.level] + v.num
    end
    self.bag_spirit_all_type = self:GetBagSpiritAllType()
end

function EquipData:GetNowBagSpiritNumList()
    self:ReCalculateSpiritInBag()
    return self.now_bag_spirit_num_list
end

-- 判断装备上的铸灵是否可以升级
function EquipData:JudgeEquipSpiritCanLevelUp(item_id, level)
    self:ReCalculateSpiritInBag()
    local now_group = self:GetSpiritGroup(item_id)
    if self.now_bag_spirit_num_list[now_group] and not self:GetSpiritIsMaxLevel(item_id, level) then
        -- 能否升级判断可用比当前等级更低的铸灵去组合消耗
        local now_group_level_config = self:GetSpiritLevelData(item_id)
        -- 用于while循环递减
        local select_item_level = level
        -- 装备上本身的铸灵算一个，所以第一个消耗要-1
        local now_need_num = now_group_level_config[select_item_level].need_num - 1
        while select_item_level >= 0 do
            -- 判断是否有这个等级数量的铸灵
            local now_level_has = self.now_bag_spirit_num_list[now_group][select_item_level] or 0
            local consume_config = now_group_level_config[select_item_level]
            -- 数量足够就可升级，否则向下查找等级低的数量是否足够
            if now_level_has >= now_need_num then
                return true
            else
                now_need_num = now_need_num - now_level_has
            end
            -- 查看上一级合成上来需要多少个
            select_item_level = select_item_level - 1
            if select_item_level >= 0 then
                local last_consume_config = now_group_level_config[select_item_level]
                now_need_num = now_need_num * last_consume_config.need_num
            end
        end
    end
    return false
end

-- 获取背包里的铸灵主要类型
-- 因为阴阳互斥的原则，所以只要有混沌就返回混沌
-- 同时有阴和阳也返回混沌
function EquipData:GetBagSpiritAllType()
    local m_type = EquipSpiritType.Chaos
    local has_yin = false
    local has_yang = false
    for k, v in pairs(self.now_bag_spirit_list) do
        local now_type = self:GetSpiritType(v.item_id)
        if now_type == EquipSpiritType.Chaos then
            return now_type
        elseif now_type == EquipSpiritType.Yin then
            has_yin = true
        elseif now_type == EquipSpiritType.Yang then
            has_yang = true
        end
        if has_yin and has_yang then
            return EquipSpiritType.Chaos
        end
    end
    return has_yin and EquipSpiritType.Yin or EquipSpiritType.Yang
end

-- 替换装备时的返回，装备是否有替换铸灵
function EquipData:GetEquipNoticeInfo(protocol)
    if protocol.is_exchange == 0 then
        EquipCtrl.Instance:OpenNotAutoExchangeTip()
    elseif protocol.is_exchange == 1 then
        PublicPopupCtrl.Instance:Center(Language.Equip.AutoExchangeSpiritTip)
    end
end

-- 点击装备按钮
function EquipData:SendEquipInfo()
    local param_t = {}
    param_t.req_type = SpiritReqType.Equip
    -- 判断选了的装备是不是背包里的
    if self.now_select_equip_data.in_bag then
        param_t.param1 = SpiritSrcType.Bag
    elseif self.now_select_equip_data:IsEquip() then
        param_t.param1 = SpiritSrcType.Equip
    elseif self.now_select_equip_data:IsJewelry() then
        param_t.param1 = SpiritSrcType.Jewelry
    end
    param_t.param2 = self.now_select_equip_data.index
    param_t.param3 = self.now_select_equip_data:GridColumn()
    param_t.param4 = self.now_select_spirit_data.slot

    -- 判断选了的铸灵是不是背包里的
    if self.now_left_select_spirit.item then
        param_t.param5 = SpiritSrcType.Bag
        param_t.param6 = self.now_left_select_spirit.item.index
        param_t.param7 = self.now_left_select_spirit.item:GridColumn()
    else
        if self.now_left_select_spirit.equip.in_bag then
            param_t.param5 = SpiritSrcType.Bag
        elseif self.now_left_select_spirit.equip:IsEquip() then
            param_t.param5 = SpiritSrcType.Equip
        elseif self.now_left_select_spirit.equip:IsJewelry() then
            param_t.param5 = SpiritSrcType.Jewelry
        end
        param_t.param6 = self.now_left_select_spirit.equip.index
        param_t.param7 = self.now_left_select_spirit.equip:GridColumn()
        param_t.param8 = self.now_left_select_spirit.config.slot
    end
    EquipCtrl.Instance:SendSpiritReq(param_t)
end

-- 点击升级按钮（这里只会去设置数据，需要自己打开升级界面）
function EquipData:OpenSpiritLevelView()
    -- 先清除一遍数据
    self.spirit_level_data:Set({spirit_level_bag = nil, spirit_level_equip = nil, spirit_level_config = nil, now_select_data = {}})
    if self.now_left_select_spirit then
        -- 判断选了的铸灵是不是背包里的
        if self.now_left_select_spirit.item then
            self.spirit_level_data.spirit_level_bag =  self.now_left_select_spirit.item
        else
            self.spirit_level_data.spirit_level_equip =  self.now_left_select_spirit.equip
        end
        self.spirit_level_data.spirit_level_config = self.now_left_select_spirit.config
    else
        self.spirit_level_data.spirit_level_equip =  self.now_select_equip_data
        self.spirit_level_data.spirit_level_config = self.now_select_spirit_data
    end
end

-- 点击卸下按钮
function EquipData:SendRemoveInfo()
    local param_t = {}
    param_t.req_type = SpiritReqType.Remove
    if self.now_left_select_spirit then
        -- 判断选了的铸灵是不是背包里的
        if self.now_left_select_spirit.equip then
            if self.now_left_select_spirit.equip.in_bag then
                param_t.param1 = SpiritSrcType.Bag
            elseif self.now_left_select_spirit.equip:IsEquip() then
                param_t.param1 = SpiritSrcType.Equip
            elseif self.now_left_select_spirit.equip:IsJewelry() then
                param_t.param1 = SpiritSrcType.Jewelry
            end
            param_t.param2 = self.now_left_select_spirit.equip.index
            param_t.param3 = self.now_left_select_spirit.equip:GridColumn()
            param_t.param4 = self.now_left_select_spirit.config.slot
        else
            LogError("背包的铸灵无法卸下")
            return
        end
    else
        if TableIsEmpty(self.now_select_equip_data) then
            -- 按道理来说是不会走到这块逻辑的
            PublicPopupCtrl.Instance:Center(Language.Equip.NoSelectEquipTip)
            return
        end
        if self.now_select_equip_data.in_bag then
            param_t.param1 = SpiritSrcType.Bag
        elseif self.now_select_equip_data:IsEquip() then
            param_t.param1 = SpiritSrcType.Equip
        elseif self.now_select_equip_data:IsJewelry() then
            param_t.param1 = SpiritSrcType.Jewelry
        end
        param_t.param2 = self.now_select_equip_data.index
        param_t.param3 = self.now_select_equip_data:GridColumn()
        param_t.param4 = self.now_select_spirit_data.slot
    end
    EquipCtrl.Instance:SendSpiritReq(param_t)
end

-------------------------------------装备转换-----------------------------------------
function EquipData:GetProfList()
    local list = {}
    -- 通用
    -- table.insert(list,{prof = 0})
    -- 本职业
    local my_prof = RoleData.Instance:GetRoleProf()
    table.insert(list,{prof= my_prof})
    -- 剩下的职业
    local vo
    for i = 1, 8 do 
        if my_prof ~= i then 
            vo = {prof= i}
            table.insert(list,vo)
        end 
    end 
    return list
end

function EquipData:EquipConvert()
    return Config.compose_auto.equip_tran[1].tran_cost_item.item_id
end
function EquipData:EquipConvertNum()
    return Config.compose_auto.equip_tran[1].tran_cost_item.num
end

--设置需要转换的装备
function EquipData:SetChangeEquip(item)
    self.chang_equip_item:Set(item)
end


function EquipData:GetChangeEquip()
    return self.chang_equip_item
end
function EquipData:GetChangeEquipTarget()
    return self.chang_equip_target_item
end
--设置转换目标1装备
function EquipData:SetChangeEquipTarget1(item)
    self.chang_equip_target_item.item1 = item
end
--设置转换目标2装备(无用)
function EquipData:SetChangeEquipTarget2(item)
    self.chang_equip_target_item.item2 = item
end
function EquipData:ClearEquipChange()
    self:SetChangeEquip({})
    self:SetChangeEquipTarget1(nil)
end

function EquipData:GetChangeEquipResult()
    --这里返回一个自己构造的Item
    local item1 = self.chang_equip_target_item.item1
    -- local item2 = self.chang_fuyu_target_item.item2
    if item1 == nil then
        return {}
    end
    -- if item1 and item2 then
    --     item1.param.extra_sp_id = item2.param.sp_id
    -- end
    return item1
end
--可转换列表
function EquipData:GetEquipChangeList()
    return BagData.Instance:GetConverEquipList()
end

--获取可转换装备列表(无用)
function EquipData:GetExcFuYuChangeItems()
    local list = {}
    local item
    -- for i, v in ipairs(self.partner_fuyu_exc_list) do
    --     item = self:ExcItem(v.sp_id)
    --     table.insert(list, item)
    -- end
    return list
end
--包装要转换的装备
function EquipData:ExcItem(cfg)
    local item = self:GetChangeEquip()
    local vo = {}
    vo.item_id = cfg.id
    vo.num = 0
    vo.is_bind = 1
    vo.has_param = true
    vo.param = item.param
    return Item.Create(vo)
end

--根据职业显示可以转换的符玉列表
function EquipData:GetEquipChangeItems(prof, equip_type, level)
    local list = self.equip_change_map[prof][equip_type][level]
    local new_list = {}
    for k, v in pairs(list) do
        table.insert(new_list, self:ExcItem(v))
    end
    return new_list
end

-------------------------------------装备转换-----------------------------------------