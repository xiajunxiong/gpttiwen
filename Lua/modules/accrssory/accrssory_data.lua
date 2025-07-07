AccrssoryData = AccrssoryData or BaseClass()

AccrssoryConfig = {
    TypeName = {
        [1] = "HuJing",
        [2] = "HuaDian",
        [3] = "JinDai",
        [4] = "XiangNang",
        [5] = "XiuJian",
    },
}

-- 配饰操作请求类型
AccrssoryReqType = {
    Info = 0,       --请求下发
    Equip = 1,      --装备，p1：背包栏id（ItemColumnType.Accrssory），p2：背包格子index，p3：配饰类型
    Remove = 2,     --卸下，p1：配饰类型
    Resolve = 3,    --分解，p1：背包coloumn，p2：背包index，p3：数量（默认1，天命丹需要）
    Strength = 4,   --强化，p1：是否穿戴，p2：背包index或者配饰类型
    Change = 5,     --置换，p1：是否穿戴，p2：背包coloumn，p3：背包index或者配饰类型，p4：新道具id
    Extral = 6,     --仙饰升级， p1：1~2
}

-- 配饰继承使用到类型
AccrssoryInheritType = {
    Now = 1,
    Material = 2,
}

-- 配饰置换使用到类型
AccrssoryChangeType = {
    Now = 1,
    Change = 2,
}

AccrssoryRecommendLevel = {
    [1] = 80,
    [2] = 110,
}

AccrssoryColorType = {
    [1] = 4,
    [2] = 5,
    [3] = 6,
    [4] = 9,
}

function AccrssoryData:__init()
    if AccrssoryData.Instance ~= nil then
        Debuger.LogError("[AccrssoryData] attempt to create singleton twice!")
        return
    end
    AccrssoryData.Instance = self
    self.config = Config.peishi_auto
    self.other = self.config.other[1]

    AccrssoryRecommendLevel[1] = self.other.open_level
    AccrssoryRecommendLevel[2] = self.other.feisheng_open
    
    self:InitAllAccrssoryData()

    self.destiny_cfg = KeyList(self.config.tianming,"tianming_level")

    self.illustration_data = SmartData.New({suit_id = 1, part = 1, is_show_fly = 0})
    self.tianming_data = SmartData.New({level = 0, exp = 0,sys_type_level = {}})
    self.tianming_eff_mark = false
    -- 这里存储的是Accrssory实例对象，还有套装技能
    self.accrssory_equip_data = SmartData.New({info = {}, skill = {}, capability = 0})
    -- 当前配饰选择中
    self.now_select_equip = SmartData.New({info = {}})
    -- 配饰继承使用
    self.accrssory_inherit_data = SmartData.New({now = {}, material = {}, inherit_type = AccrssoryInheritType.Now})
    -- 当前显示的panel
    self.now_show_panel = SmartData.New({mod_id = Mod.Accrssory.Main})
    -- 配饰置换使用
    self.change_data = SmartData.New({now = {}, change = {}, select_type = AccrssoryChangeType.Now})
    -- 角色推荐套装
    self.recommend_suit = {}
    -- 红点
    self.remind_data = SmartData.New()
    -- 同一帧执行限制
    self.calculate_frame_count = 0
    self.calculate_frame_count2 = 0
    -- 记录继承界面每一个配饰是否有红点
    self.inherit_remind = {}
    for i = 1, ACCRSSORY_MAX do
        self.inherit_remind[i] = 0
    end
    -- 记录强化界面每一个配饰是否有红点
    self.strength_remind = {}
    for i = 1, ACCRSSORY_MAX do
        self.strength_remind[i] = 0
    end

    self.accrssory_extra_data = SmartData.New({list = {[1]=0,[2]=0},type = 0,flush = 1})

    Cfg.Register("AccrssorySkillDesc", Cfg.RealTimeSearch2Key(Config.skill_description_auto.peishi_skill, "skill_id", "skill_level"))
    Cfg.Register("AccrssorySkillDesc2", Cfg.RealTimeSearch2Key(Config.skill_description_auto.peishi_skill, "skill2_profession", "skill_level"))
    Cfg.Register("AccrssoryCanInheritCount", Cfg.RealTimeSearch(self.config.chuancheng, "entry_num"))
    Cfg.Register("AccrssoryInheritConsume", Cfg.RealTimeSearch2Key(self.config.chuancheng_cost, "Material_level", "Material_quality"))
    Cfg.Register("AccrssoryStrengthen", Cfg.RealTimeSearch2Key(self.config.strengthen, "peishi_id", "peishi_level"))
    Cfg.Register("AccrssoryChangeConsume", Cfg.RealTimeSearch2Key(self.config.display, "displace_quality", "entry"))
end

function AccrssoryData:__delete()
    AccrssoryData.Instance = nil
end

function AccrssoryData:ResetChange()
    self.change_data.now = {}
    self.change_data.change = {}
    self.select_type = AccrssoryChangeType.Now
end

function AccrssoryData:InitAllAccrssoryData()
    -- 处理套装表，id对应套装名字和是否飞升套装配饰
    self.suit_id_name_config = {}
    for k, v in pairs(self.config.tujian) do
        if not self.suit_id_name_config[v.suit_type] then
            self.suit_id_name_config[v.suit_type] = {}
            self.suit_id_name_config[v.suit_type].name = v.suit_name
            self.suit_id_name_config[v.suit_type].is_fly = Config.peishi_item_auto[v.peishi_id].feisheng_level > 0
        end
    end
    -- 处理技能id对应职业限定字段的表
    self.skill_id_prof_config = {}
    for k, v in pairs(self.config.peishi_skill) do
        if not self.skill_id_prof_config[v.skill_1] then
            self.skill_id_prof_config[v.skill_1] = v.skill1_profession
        end
        if not self.skill_id_prof_config[v.skill_2] then
            self.skill_id_prof_config[v.skill_2] = v.skill2_profession
        end
    end
    -- 处理配饰技能表，根据套装类型和最低品质
    self.accrssory_skill_config = {}
    self.accrssory_skill_config = DataHelper.Subdivision(self.config.peishi_skill, {"suit_type", "min_color"})
    -- 处理配饰随机词条奖励组
    self.rand_attr_reward_config = {}
    self.rand_attr_reward_config = DataHelper.Subdivision(self.config.entry_reward, {"seq", "att_type"})
    -- 处理套装和品质对应得到奖励词条组
    self.rand_reward_config = {}
    self.rand_reward_config = DataHelper.Subdivision(self.config.entry, {"suit_type", "peishi_color"})

    -- 处理套装类型、颜色、部位得到对应的道具id表
    self.suit_color_part_config = {}
    -- 处理套装类型对应套装角标名字
    self.suit_get_name_config = {}

    -- 根据是否飞升（0，2）颜色和配饰类型获取一组道具id
    self.fly_color_type_config = {}

    for k, v in pairs(Config.peishi_item_auto) do
        if not self.suit_color_part_config[v.suit_type] then
            self.suit_color_part_config[v.suit_type] = {}
        end
        if not self.suit_color_part_config[v.suit_type][v.color] then
            self.suit_color_part_config[v.suit_type][v.color] = {}
        end
        self.suit_color_part_config[v.suit_type][v.color][v.equip_type] = v.id

        if not self.suit_get_name_config[v.suit_type] then
            self.suit_get_name_config[v.suit_type] = v.suit_name
        end

        if not self.fly_color_type_config[v.feisheng_level] then
            self.fly_color_type_config[v.feisheng_level] = {}
        end
        if not self.fly_color_type_config[v.feisheng_level][v.color] then
            self.fly_color_type_config[v.feisheng_level][v.color] = {}
        end
        if not self.fly_color_type_config[v.feisheng_level][v.color][v.equip_type] then
            self.fly_color_type_config[v.feisheng_level][v.color][v.equip_type] = {}
        end
        table.insert(self.fly_color_type_config[v.feisheng_level][v.color][v.equip_type], k)
    end

    -- 根据职业限制对应到套装类型
    self.prof_suit_config = {}
    for k, v in pairs(self.config.peishi_skill) do
        if not self.prof_suit_config[v.skill2_profession] then
            self.prof_suit_config[v.skill2_profession] = {}
        end
        if not self.prof_suit_config[v.skill2_profession][v.suit_type] then
            self.prof_suit_config[v.skill2_profession][v.suit_type] = 1
        end
    end

    -- 根据套装id对应最低品质颜色
    self.suit_min_color_config = {}
    for k, v in pairs(self.suit_color_part_config) do
        self.suit_min_color_config[k] = AccrssoryColorType[#AccrssoryColorType]
        for l, w in pairs(v) do
            if self.suit_min_color_config[k] > l then
                self.suit_min_color_config[k] = l
            end
        end
    end

    -- 调整表·仙饰
    self.extral_equip_cfg = {}
    for k,v in pairs(Config.peishi_auto.xianshi) do
        if self.extral_equip_cfg[v.type] == nil then 
            self.extral_equip_cfg[v.type] = {}
        end 
        self.extral_equip_cfg[v.type][v.level] = v
    end 
end

function AccrssoryData:GetSuitIdNameConfig()
    return self.suit_id_name_config
end

function AccrssoryData:GetSkillIdProfConfig()
    return self.skill_id_prof_config
end

function AccrssoryData:GetSkillConfig()
    return self.accrssory_skill_config
end

function AccrssoryData:GetRandAttrRewardConfig()
    return self.rand_attr_reward_config
end

function AccrssoryData:GetRandRewardConfig()
    return self.rand_reward_config
end

function AccrssoryData:GetSuitColorPartConfig()
    return self.suit_color_part_config
end

function AccrssoryData:GetSuitGetNameConfig()
    return self.suit_get_name_config
end

function AccrssoryData:GetProfSuitConfig()
    return self.prof_suit_config
end

function AccrssoryData:GetSuitMinColorConfig()
    return self.suit_min_color_config
end

function AccrssoryData:GetFlyColorTypeConfig()
    return self.fly_color_type_config
end

-- 返回配饰置换卡id，是否飞升
function AccrssoryData:ChangeItem(is_fly)
    return is_fly and self.other.displace_card_feisheng or self.other.displace_card
end

-- 根据item返回应该所需的置换卡id和所需数量
function AccrssoryData:NeedChangeItem(item)
    local item_id = self:ChangeItem(item:IsFlyAccrssory())
    local cfg = Cfg.AccrssoryChangeConsume(item:Color(), item:GetHasAttrNum())
    return item_id, not TableIsEmpty(cfg) and cfg.cost_num or 0
end

-- 根据配饰id和强化等级获取数据
function AccrssoryData:StrengthConfig(item_id, level)
    return Cfg.AccrssoryStrengthen(item_id, level, false) or {}
end

-- 判断当前强化等级是否是最大等级了
function AccrssoryData:IsStrengthLevelMax(level)
    return self.other.strengthen_max == level
end

-- 根据配饰id和强化等级获取属性列表
function AccrssoryData:StrengthAttrConfig(item_id, level)
    local config = Cfg.AccrssoryStrengthen(item_id, level, false)
    local t = {}
    local index = 1
    local need_zero = level <= 0
    level = need_zero and 1 or level
    if need_zero then
        config = Cfg.AccrssoryStrengthen(item_id, 1, false)
    end
    if not TableIsEmpty(config) then
        while config["attr_value_"..index] and config["attr_value_"..index] > 0 do
            local attr = CommonStruct.AttrStruct(config["att_type_"..index], need_zero and 0 or config["attr_value_"..index])
            attr.not_need = true
            attr.show_percent = true
            table.insert(t, attr)
            index = index + 1
        end
    end
    return t
end

-- 返回该配饰可以置换的其他配饰
function AccrssoryData:GetCanChangeItem(item)
    local t = {}
    if self.fly_color_type_config[item:GetFlyLevel()] then
        if self.fly_color_type_config[item:GetFlyLevel()][item:Color()] then
            if self.fly_color_type_config[item:GetFlyLevel()][item:Color()][item:AccrssoryType()] then
                for k, v in pairs(self.fly_color_type_config[item:GetFlyLevel()][item:Color()][item:AccrssoryType()]) do
                    if v ~= item.item_id then
                        local vo = {}
                        vo.item_id = v
                        vo.is_bind = true
                        vo.in_bag = false
                        vo.is_mine = true
                        vo.need_capability = false
                        vo.is_fake = true
                        vo.param = TableCopy(item.param, 2)
                        table.insert(t, {item = Accrssory.New(vo)})
                    end
                end
            end
        end
    end
    return t
end

-- 获取套装id名字数据（根据是否飞升来获取）
function AccrssoryData:GetSuitIdNameData(is_fly)
    local suit_id_name_data = {}
    local is_show_fly = self:IsShowFly()
    for i, v in ipairs(self.suit_id_name_config) do
        local tab = {}
        tab.suit_id = i
        tab.suit_name = v.name
        tab.is_select = false
        if is_fly ~= nil then
            if is_fly == v.is_fly then
                table.insert(suit_id_name_data, tab)
            end
        else
            if not v.is_fly or is_show_fly then
                table.insert(suit_id_name_data, tab)
            end
        end
    end
    return suit_id_name_data
end

-- 获取图鉴数据
function AccrssoryData:GetIllustrationConfig()
    if not self.illustration_config then
        self.illustration_config = DataHelper.Subdivision(self.config.tujian, {"suit_type", "peishi_part"})
    end
    return self.illustration_config
end

-- 获取单个配饰的基础属性
function AccrssoryData:GetAccrssoryBaseAttr(item_id, show_percent)
    local t = {}
    local config = Item.GetConfig(item_id)
    if not TableIsEmpty(config) then
        local index = 1
        while config["attr_value_"..index] and config["attr_value_"..index] > 0 do
            local t1 = CommonStruct.AttrStruct(config["attr_type_"..index], config["attr_value_"..index])
            t1.show_percent = show_percent or false
            table.insert(t, t1)
            index = index + 1
        end
    end
    return t
end

-- 获取单个配饰的可能获得的最大属性
function AccrssoryData:GetAccrssoryRandMaxAttr(item_id)
    local t = {}
    local config = Item.GetConfig(item_id)
    if not TableIsEmpty(config) then
        local reward = self.rand_reward_config[config.suit_type][config.color]
        for k, v in pairs(self.rand_attr_reward_config[reward.seq]) do
            table.insert(t, CommonStruct.AttrStruct(v.att_type, v.att_max))
        end
    end
    return t
end

function AccrssoryData:SetTianMingInfo(protocol)
    self.tianming_data.level = protocol.level
    self.tianming_data.exp = protocol.exp
    self.tianming_data.sys_type_level = protocol.sys_type_level
end

function AccrssoryData:MarkResolve()
    self.tianming_eff_mark = true
end

function AccrssoryData:ReleaseMarkResolve()
    self.tianming_eff_mark = false
end

function AccrssoryData:GetTianMingInfo()
    return self.tianming_data
end

function AccrssoryData:GetTianMingLevelCfg()
    local cur_cfg = nil
    local next_cfg = nil
    LogError("?asd ",self.tianming_data.level)
    for k,v in pairs(self.config.tianming) do 
        if v.tianming_level == self.tianming_data.level then 
            cur_cfg = v
        end 
    end 

    for k,v in pairs(self.config.tianming) do 
        if v.tianming_level == self.tianming_data.level + 1 then 
            next_cfg = v
        end 
    end 

    return cur_cfg,next_cfg
end

function AccrssoryData:IsMaxTianMing()
    return #self.config.tianming <= self.tianming_data.level
end

function AccrssoryData:GetCurAttrInfo()
    local cache = {}
    for k,v in pairs(self.config.tianming) do 
        if v.tianming_level <= self.tianming_data.level then 
            table.insert(cache,v)
        end 
    end 

    local attr_per_list = {}
    local attr_per_tol_list = {}
    for k,v in pairs(cache) do 
        if attr_per_list[v.att_type] == nil then attr_per_list[v.att_type] = 0 end 
        if attr_per_tol_list[v.att_type] == nil then attr_per_tol_list[v.att_type] = 0 end 

        attr_per_list[v.att_type] = attr_per_list[v.att_type] + v.att_per
        attr_per_tol_list[v.att_type] = attr_per_tol_list[v.att_type] + v.att_per_tol
    end 

    return {
        cur_cfg = cache[#cache],
        next_cfg = self.destiny_cfg[self.tianming_data.level + 1],
        attr_per_tol_list = attr_per_tol_list,
        attr_per_list = attr_per_list
    }
end

function AccrssoryData:GetTianMingCfgByLevel(level)
    return self.destiny_cfg[level]
end 

function AccrssoryData:GetTianMingPreViewData()
    local cache = {}
    local cur_data = nil
    for k,v in pairs(self.destiny_cfg) do 
        local vo = {}
        vo.str = ColorStr(v.tianming_name .. v.att_dec,
            self.tianming_data.level == v.tianming_level and COLORSTR.Yellow16 
            or (self.tianming_data.level > v.tianming_level and COLORSTR.Yellow20 or COLORSTR.Yellow14 ))
        vo.bg = math.floor(v.tianming_level%2)
        table.insert(cache,vo)
        if self.tianming_data.level == v.tianming_level then 
            cur_data = v
        end 
    end 

    if cur_data == nil then cur_data = self.destiny_cfg[1] end 

    return cache,cur_data
end

-- 通过强化等级逆推共有多少强化石
function AccrssoryData:GetStoneNumByList(list)
    local stone_num = 0
    for k,v in pairs(list) do 
        if v.StrengthLevel ~= nil then 
            local level = v:StrengthLevel()
            if level > 0 then 
                for i = 1,level do
                    local cfg = self:StrengthConfig(v.item_id, i)
                    stone_num = cfg.ltem_list[0].num + stone_num
                end 
            end
        end 
    end 
    return stone_num
end

-- 获取背包的配饰数据(是否补齐，补齐到最小数量，数量需要能整除多少，筛选套装：0或者不填就是默认全部，筛选部位：0或者不填就是默认全部)
function AccrssoryData:GetAccrssoryBagData(is_fill, min_num, sqrt_num, select_suit, select_part)
    local bag_list = BagData.Instance:GetAccrssoryList(select_suit, select_part)
    if is_fill then
        local need_add = min_num - #bag_list
        if need_add < 0 then
            need_add = math.abs(need_add)
            need_add = (sqrt_num - need_add % sqrt_num) % sqrt_num
        end
        for i = 1, need_add do
            table.insert(bag_list, Accrssory.New())
        end
    end
    return bag_list
end

-- 获取套装可选标签的数据
function AccrssoryData:GetAccrssorySuitTabData(now_select_suit)
    local suit_tab_data = {}
    local now_select_name = Language.Accrssory.AllSuit
    local tab = {}
    tab.suit_id = 0
    tab.is_select = now_select_suit == tab.suit_id
    tab.suit_name = now_select_name
    table.insert(suit_tab_data, tab)
    for i, v in ipairs(self:GetSuitIdNameData()) do
        if now_select_suit == v.suit_id then
            now_select_name = v.suit_name
        end
        table.insert(suit_tab_data, v)
    end
    return now_select_name, suit_tab_data
end

function AccrssoryData:GetDenstinyEffectiD(tuJi)
    local light = {
        [1] = "4165082",
        [2] = "4165081",
        [3] = "4165083",
        [4] = "4165080",
        [5] = "4165084",
    }

    local light_light = {
        [1] = "4165088",
        [2] = "4165087",
        [3] = "4165089",
        [4] = "4165086",
        [5] = "4165090",
    }

    return light[tuJi],light_light[tuJi]
end

-- 所有装备上的配饰
function AccrssoryData:SetAllAccrssoryInfo(protocol)
    self.accrssory_equip_data.capability = protocol.capability
    for k, v in pairs(protocol.equipment_list) do
        self:SetEquipDataInfo(v, self.accrssory_equip_data.info)
    end
    self.accrssory_equip_data.skill = self:CalculateSkill(self.accrssory_equip_data.info)
    self.remind_data:Notify()
    BagData.Instance.crystal_red_piont:Notify()
    self.accrssory_equip_data:Notify()
end

-- 单个配饰
function AccrssoryData:SetAccrssoryInfo(protocol)
    self.accrssory_equip_data.capability = protocol.capability
    self:SetEquipDataInfo(protocol.equip_info, self.accrssory_equip_data.info)
    self.accrssory_equip_data.skill = self:CalculateSkill(self.accrssory_equip_data.info)
    self.remind_data:Notify()
    BagData.Instance.crystal_red_piont:Notify()
    self.accrssory_equip_data:Notify()
end

-- 4514 仙饰下发
function AccrssoryData:SetAccrssoryExtral(protocol)
    self.accrssory_extra_data.list = protocol.accessories_special_equip_list
    self.accrssory_extra_data.flush = self.accrssory_extra_data.flush + 1
end

-- 协议下发后会去调用，设置装备上的配饰实例（data，载体，是否自己，是否需要计算战力）
function AccrssoryData:SetEquipDataInfo(data, info, is_mine, need_capability)
    if not TableIsEmpty(data) and data.item_id > 0 then
        local vo = {}
        vo.index = data.index
        vo.item_id = data.item_id
        vo.num = data.num
        vo.is_bind = data.is_bind == 1
        vo.in_bag = false
        if is_mine ~= nil then
            vo.is_mine = is_mine
        end
        if need_capability ~= nil then
            vo.need_capability = need_capability
        end
        vo.param = {}
        vo.param.attr_list = data.attr_list
        vo.param.strength_level = data.strength_level
        info[data.index] = Accrssory.New(vo)
    else
        info[data.index] = nil
    end
end

-- 获取有效的装备了的配饰
function AccrssoryData:GetEquipDataInfo()
    local t = {}
    for i = 1, ACCRSSORY_MAX do
        if self.accrssory_equip_data.info[i] then
            table.insert(t, {item = self.accrssory_equip_data.info[i]})
        end
    end
    return t
end

-- 获取身上的和背包里的配饰
function AccrssoryData:GetAllEquipDataInfo()
    local t = {}
    for k, v in pairs(self:GetEquipDataInfo()) do
        table.insert(t, v)
    end
    for k, v in pairs(self:GetAccrssoryBagData()) do
        table.insert(t, {item = v})
    end
    return t
end

-- 配饰改变的话要重算一遍当前技能，考虑到会计算别人的配饰，所以算好后会返回出去
function AccrssoryData:CalculateSkill(info)
    local suit_color_config = {}
    local skill_config = {}
    -- 用于记录只激活了一个2件套技能对应的5件套技能
    local record_fake_skill_2 = {}
    for i = 1, ACCRSSORY_MAX do
        if info[i] then
            local suit_type = info[i]:SuitType()
            local color = info[i]:Color()
            if not suit_color_config[suit_type] then
                suit_color_config[suit_type] = {}
                suit_color_config[suit_type].count = 0
                suit_color_config[suit_type].color_num = {}
            end
            suit_color_config[suit_type].count = suit_color_config[suit_type].count + 1
            if not suit_color_config[suit_type].color_num[color] then
                suit_color_config[suit_type].color_num[color] = 0
            end
            suit_color_config[suit_type].color_num[color] = suit_color_config[suit_type].color_num[color] + 1
        end
    end
    -- 第三个参数是，1拿两件套的技能，2拿五件套的技能
    local get_skill_data = function(suit_type, min_color, index)
        local config = self.accrssory_skill_config[suit_type][min_color]
        return {skill_id = config["skill_"..index], skill_level = config.skill_level, skill_num_suit = index == 1 and 2 or 5}
    end
    for k, v in pairs(suit_color_config) do
        -- 计算两件套技能
        if v.count >= 2 then
            local num = 2
            for i = #AccrssoryColorType, 1, -1 do
                local m_color = AccrssoryColorType[i]
                if v.color_num[m_color] then
                    num = num - v.color_num[m_color]
                    if num <= 0 then
                        local skill_now = get_skill_data(k, m_color, 1)
                        -- 这里需要判断是否重复的，如果有重复的要保留更高级的
                        local has_equal = false
                        for l, w in pairs(skill_config) do
                            if w.skill_id == skill_now.skill_id and skill_now.skill_level > w.skill_level then
                                w.skill_level = skill_now.skill_level
                                if not TableIsEmpty(record_fake_skill_2) then
                                    record_fake_skill_2[1].suit_type = k
                                    record_fake_skill_2[1].skill_level = skill_now.skill_level
                                end
                                has_equal = true
                                break
                            end
                        end
                        if not has_equal then
                            table.insert(skill_config, skill_now)
                            -- 只会记录一次
                            if TableIsEmpty(record_fake_skill_2) then
                                local skill_fake = get_skill_data(k, m_color, 2)
                                skill_fake.is_recommand = true
                                skill_fake.suit_type = k
                                table.insert(record_fake_skill_2, skill_fake)
                            end
                        end
                        break
                    end
                end
            end
        end
        -- 计算五件套技能
        if v.count >= 5 then
            for i = 1, #AccrssoryColorType do
                local m_color = AccrssoryColorType[i]
                if v.color_num[m_color] and v.color_num[m_color] > 0 then
                    table.insert(skill_config, get_skill_data(k, m_color, 2))
                    break
                end
            end
        end
    end
    -- 如果只有一个技能，那肯定是两件套的技能，这时候要展示出对应的置灰的五件套技能
    if #skill_config == 1 then
        table.insert(skill_config, record_fake_skill_2[1])
    end
    return skill_config
end

-- 是否展示飞升的配饰按钮
function AccrssoryData:IsShowFly()
    return RoleData.Instance:GetRoleLevel() >= AccrssoryRecommendLevel[2]
end

-- 飞升配饰按钮是否可以点
function AccrssoryData:IsCanClickFly()
    return self:IsShowFly() and FlyUpData.Instance:IsFinish()
end

-- 计算推荐套装
function AccrssoryData:SetRecommendSuit()
    -- 推荐套装，有80级的和110级的，小于110级就推荐80级的，否则推荐110级的
    -- 这里拿最低品质的技能来显示
    local min_color = {}
    for i = 1, #AccrssoryRecommendLevel do
        min_color[AccrssoryRecommendLevel[i]] = AccrssoryColorType[#AccrssoryColorType]
    end
    local prof = RoleData.Instance:GetProfession()
    local level = RoleData.Instance:GetRoleLevel()
    local recommand_level = level < AccrssoryRecommendLevel[2] and AccrssoryRecommendLevel[1] or AccrssoryRecommendLevel[2]
    -- 这里获得当前职业对应的80级和110级套装类型
    local tab_limit_level_item_id = {}
    for k, v in pairs(self.prof_suit_config[prof]) do
        local color = self.suit_min_color_config[k]
        local item_id = self.suit_color_part_config[k][color][1]
        local level = Item.GetLimitLv(item_id)
        tab_limit_level_item_id[level] = k
        min_color[level] = color
    end
    local suit_type = tab_limit_level_item_id[recommand_level]
    local skill_config = self.accrssory_skill_config[suit_type][min_color[recommand_level]]
    self.recommend_suit = {}
    table.insert(self.recommend_suit, {skill_id = skill_config.skill_1, skill_level = skill_config.skill_level, skill_num_suit = 2, is_recommand = true, suit_type = suit_type})
    table.insert(self.recommend_suit, {skill_id = skill_config.skill_2, skill_level = skill_config.skill_level, skill_num_suit = 5, is_recommand = true, suit_type = suit_type})
end

-- 配饰界面红点
function AccrssoryData:CheckMainRemind()
    if not FunOpen.Instance:IsFunOpen(Mod.Accrssory.Main) then
        return 0
    end
    local bag_list = self:GetAccrssoryBagData()
    for k, v in pairs(bag_list) do
        local equip_type = v:AccrssoryType()
        if self:CompareScore(v, equip_type) then
            return 1
        end
    end

    -- 增加一个仙饰的红点
    for i = 1,2 do 
       local check_num = self:GetExtralEquipRemindNum(i)
       if check_num > 0 then 
        return 1
       end 
    end 

    return 0
end

function AccrssoryData:GetExtralEquipRemindNum(index)
    local cur_cfg = self:GetExtralEquip(index)
    if not cur_cfg.is_max and not cur_cfg.is_empty then 
        local next_cfg = self:GetExtralEquip(index,cur_cfg.level+1)
        local suff_item = Item.Create({item_id = next_cfg.level_cfg.item_list[0].item_id})
        if suff_item:Num() >= next_cfg.level_cfg.item_list[0].num then 
            return 1
        end 
    elseif cur_cfg.is_empty then
        local suff_item = Item.Create({item_id = cur_cfg.level_cfg.item_list[0].item_id})
        if suff_item:Num() >= cur_cfg.level_cfg.item_list[0].num then 
            return 1
        end 
    end 
    return 0
end

-- 比较两个配饰评分，第一个是背包上的，第二个是装备上的配饰index
-- 第一个配饰评分是否大于第二个+5，这个5是因为服务端计算的评分会向下取整，有误差
function AccrssoryData:CompareScore(item, index)
    if item.item_id > 0 then
        if not self.accrssory_equip_data.info[index] then
            return true
        else
            local now_score = self.accrssory_equip_data.info[index]:Score()
            -- 大于5点才显示，这个设定写死
            return item:Score() > (now_score + 5)
        end
    else
        return false
    end
end

-- 继承界面红点
function AccrssoryData:CheckInheritRemind()
    if not FunOpen.Instance:IsFunOpen(Mod.Accrssory.Inherit) then
        return 0
    end
    self:CalculateInheritRemind()
    for k, v in pairs(self.inherit_remind) do
        if v > 0 then
            return 1
        end
    end
    return 0
end

-- 强化界面红点
function AccrssoryData:CheckStrengthRemind()
    if not FunOpen.Instance:IsFunOpen(Mod.Accrssory.Strength) then
        return 0
    end
    self:CalculateStrengthRemind()
    for k, v in pairs(self.strength_remind) do
        if v > 0 then
            return 1
        end
    end
    return 0
end

-- 这里计算到每一个继承装备上是否有红点
function AccrssoryData:CalculateInheritRemind()
    -- 防止同一帧执行多次
    if self.calculate_frame_count ~= Time.frameCount then
        self.calculate_frame_count = Time.frameCount
        for i = 1, ACCRSSORY_MAX do
            local now_equip_info = self.accrssory_equip_data.info[i]
            -- 没穿戴就不会有继承红点
            if now_equip_info then
                -- 穿了的话检查每个词条是否都有属性
                if now_equip_info:GetAttrMaxNum() ~= now_equip_info:GetHasAttrNum() then
                    local suit_type = now_equip_info:SuitType()
                    local bag_list = self:GetAccrssoryBagData(false, 0, 0, suit_type, i)
                    self.inherit_remind[i] = TableIsEmpty(bag_list) and 0 or 1
                else
                    self.inherit_remind[i] = 0
                end
            else
                self.inherit_remind[i] = 0
            end
        end
    end
    return self.inherit_remind
end

-- 这里计算到每一个强化装备上是否有红点
function AccrssoryData:CalculateStrengthRemind()
    -- 防止同一帧执行多次
    if self.calculate_frame_count2 ~= Time.frameCount then
        self.calculate_frame_count2 = Time.frameCount
        for i = 1, ACCRSSORY_MAX do
            local now_equip_info = self.accrssory_equip_data.info[i]
            -- 没穿戴就不会有继承红点
            if now_equip_info then
                -- 穿了的话判断是否可以强化
                local now_level = now_equip_info:StrengthLevel()
                local cfg = self:StrengthConfig(now_equip_info.item_id, now_level + 1)
                if self:IsStrengthLevelMax(now_level) then
                    -- 判断强化等级是否已满
                    self.strength_remind[i] = 0
                elseif RoleLevel() < (cfg.role_level or 0) then
                    -- 判断角色等级是否不足
                    self.strength_remind[i] = 0
                else
                    -- 判断各个物品是否足够
                    self.strength_remind[i] = 1
                    for k, v in pairs(cfg.ltem_list) do
                        local now_num = Item.GetNum(v.item_id)
                        if v.num > now_num then
                            self.strength_remind[i] = 0
                            break
                        end
                    end
                end
            else
                self.strength_remind[i] = 0
            end
        end
    end
    return self.strength_remind
end

function AccrssoryData:GetAccrssoryResolveExp(data)
    local total = 0
    for _, accrssory in ipairs(data) do
        if accrssory.item_id == CommonItemId.DestinyItem then 
            total = total + self.other.tianmingdan_exp * accrssory:Num()
        else 
            local cfg = accrssory:Config()
            total = total + cfg.fenjie_tianming + self.other.entry_exp * accrssory:GetHasAttrNum()
        end 
    end 

    return total
end

--获取可分解的配饰
function AccrssoryData:GetCanResolveAccrssoryItem()
    local list_init = self:GetAccrssoryBagData()
    local item = BagData.Instance:GetItemsByItemId(CommonItemId.DestinyItem)
    if #item > 0 then 
        for k,v in pairs(item) do 
            table.insert(list_init,v)
        end 
    end 
    return list_init
end

function AccrssoryData:GetResolveLevelPlus(exp_plus)
    local total_level = 0
    -- local res_exp = 0
    local all_exp = exp_plus
    for i = self.tianming_data.level + 1, #self.destiny_cfg do 
        local change_exp = all_exp - self.destiny_cfg[i].exp
        if change_exp < 0 then 
            total_level = i -1
            -- res_exp = (self.tianming_data.level + 1 == i) and all_exp or (self.destiny_cfg[i].exp + change_exp)
            break
        else 
            all_exp = change_exp
        end 
    end 
    
    return total_level
end 

function AccrssoryData:GetDestinyRate(type)
    local cur_info = self:GetCurAttrInfo()

    return cur_info.attr_per_list[type] == nil and 0 or cur_info.attr_per_list[type] / 10000
end 

-- 获取仙饰的当前信息
function AccrssoryData:GetAllExtralEquip()
    local list = {}
    for i = 1,2 do list[i] = self:GetExtralEquip(i) end 
    return list
end

function AccrssoryData:GetExtralEquip(index,target_level)
    local level = self.accrssory_extra_data.list[index]
    local cfg_list = self.extral_equip_cfg[index]
    local cfg = {}
    cfg.level_cfg = cfg_list[target_level == nil and ((level == nil or level == 0) and 1 or level) or target_level]
    cfg.icon = Config.peishi_auto.other[1]["xianshi_icon_"..index]
    cfg.level = level

    cfg.is_max = level > 0 and cfg_list[level + 1] == nil
    if cfg.level_cfg == nil then 
        cfg.is_empty = true
        cfg.level_cfg = level == 0 and cfg_list[1] or nil
    end 

    cfg.eff_desc = string.gsub(Config.peishi_auto.other[1]["xianshi_text_"..index],"sp_desc",cfg.level_cfg.spec_value_2/10)

    return cfg
end

function AccrssoryData:GetExtralAllDesc()
    local level = self.accrssory_extra_data.list[self.accrssory_extra_data.type]
    local cfg_list = self.extral_equip_cfg[self.accrssory_extra_data.type]

    local all_str = ""
    for i = 1,#cfg_list do 
        local str = cfg_list[i].level..Language.Common.Level..":"
            ..string.gsub(Config.peishi_auto.other[1]["xianshi_text_"..self.accrssory_extra_data.type],"sp_desc",cfg_list[i].spec_value_2/10)

        if cfg_list[i].level == level then
            str = string.format(str,COLORSTR.Yellow16)
            str = ColorStr(str,COLORSTR.Yellow16)
        elseif cfg_list[i].level < level then
            str = string.format(str,COLORSTR.Yellow7)
            str = ColorStr(str,COLORSTR.Yellow7)
        else 
            str = string.format(str,COLORSTR.Yellow15)
        end



        all_str = all_str.. str .. (i == #cfg_list and "" or "\n")
    end 

    return all_str
end

function AccrssoryData:GetExtralAllDescByIdWithNomal(item_id)
    local list = {}
    local cur_type = Config.peishi_auto.other[1].xianshi_item_1 == item_id and 1 or 2
    local cfg_list = self.extral_equip_cfg[Config.peishi_auto.other[1].xianshi_item_1 == item_id and 1 or 2]
    
    for k,v in pairs(cfg_list) do 
        local vo = {}
        vo.level =  cfg_list[k].level
        vo.desc =  string.format(string.gsub(Config.peishi_auto.other[1]["xianshi_text_"..cur_type],"sp_desc", ColorStr(v.spec_value_2/10,COLORSTR.Red10)),COLORSTR.Yellow15)
        for i = 1,6 do 
            if v["attr_value_"..i] > 0 then 
                local attri_type = v["att_type_"..i]
                local attri_value = v["attr_value_"..i]
                vo.desc = vo.desc .."\n" ..Language.Common.AttrList[attri_type].."+"..
                    ColorStr((DataHelper.IsPercent(attri_type) and Percent(attri_value) or attri_value),COLORSTR.Red10)
            end 
        end 
        table.insert(list,vo)
    end 

    return list
end

-- 判断该道具是不是仙饰道具
function AccrssoryData:CheckIsExtralStuff(item_id)
    return Config.peishi_auto.other[1].xianshi_item_1 == item_id or 
    Config.peishi_auto.other[1].xianshi_item_2 == item_id
end

function AccrssoryData:GetExtralBaseAttri(item_id)
    local attrs = {}
    local cfg_list = nil
    if Config.peishi_auto.other[1].xianshi_item_1 == item_id then 
        cfg_list = self.extral_equip_cfg[1]
    end 

    if Config.peishi_auto.other[1].xianshi_item_2 == item_id then 
        cfg_list = self.extral_equip_cfg[2]
    end 

    if cfg_list ~= nil then 
        for i = 1,6 do 
            local vo = {}
            if cfg_list[1]["attr_value_"..i] > 0 then 
                vo.attri_type = cfg_list[1]["att_type_"..i]
                vo.attri_value = cfg_list[1]["attr_value_"..i]
                table.insert(attrs,vo)
            end 
        end 
    end 

    return attrs
end