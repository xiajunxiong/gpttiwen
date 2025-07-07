LingBaoData = LingBaoData or BaseClass()

-- 操作请求类型
LingBaoReqType = {
    AllInfo = 0,            --请求灵宝系统所有信息(背包+圣痕)
    BagInfo = 1,            --请求灵宝系统背包所有信息
    ShengHenAllInfo = 2,    --请求灵宝系统圣痕所有信息
    ShengHenSingleInfo = 3, --请求灵宝系统单个圣痕信息，p1：圣痕类型
    WareLingBao = 4,        --请求灵宝系统佩戴灵宝，p1：圣痕类型，p2：背包index
    TakeOffLingBao = 5,     --请求灵宝系统脱下灵宝，p1：圣痕类型，p2：灵宝类型（屏蔽了）
    LingBaoStrUpAll1 = 6,   --请求灵宝系统一键强化圣痕中的所有灵宝，p1：圣痕类型
    LingBaoStrUp = 7,       --请求灵宝系统强化圣痕指定灵宝，p1：圣痕类型，p2：灵宝类型
    LingBaoStrUpAll2 = 8,   --请求灵宝系统一键强化圣痕中的指定灵宝，p1：圣痕类型，p2：灵宝类型
    ShengHenStrUp = 9,      --请求灵宝系统强化圣痕，p1：圣痕类型
    ShengHenSkillUp = 10,   --请求灵宝系统圣痕技能升级，p1：圣痕类型，p2：技能格子（0~2）
}

-- 圣痕类型
ShengHenType = {
    Role = 0,   --主角圣痕
    Pet = 1,    --宠物圣痕
    Partner = 2,--伙伴圣痕
}

-- 灵宝类型
LingBaoType = {
    Hairpin = 0,    --簪
    face = 1,       --面
    eye = 2,        --眼
    heart = 3,      --心
    accessory = 4,  --佩
    seal = 5,       --印
}

-- 协议5407通知类型
LingBaoNoticeType = {
    LingBaoStrUp = 0,   --灵宝强化，p1：圣痕类型，p2：灵宝类型，p3：强化到几级（需要自己设置）
    ShengHenStrUp = 1,  --圣痕觉醒升级，p1：圣痕类型，p2：强化到几级（需要自己设置）
    ShengHenUnLock = 2, --圣痕解锁弹窗，p1：圣痕类型
    ShengHenSkillUp = 3,--圣痕技能升级，p1：圣痕类型，p2：第几个技能，p3：升到几级（需要自己设置）
    LingBaoLvUp = 4,    --灵宝合成(升星/升阶)，p1：圣痕类型，p2：灵宝类型，p3：道具id
    IsOpenFlag = 5,     --灵宝是否开启，p1：is_open
}

function LingBaoData:__init()
    if LingBaoData.Instance ~= nil then
        Debuger.LogError("[LingBaoData] attempt to create singleton twice!")
        return
    end
    LingBaoData.Instance = self
    self.config = Config.lingbao_auto
    self.other = self.config.other[1]

    -- 灵宝背包信息，是new好的格子实例，根据index做下标
    self.item_list = SmartData.New()
    self.equip_list = SmartData.New()
    self.info_data = SmartData.New()
    -- 灵宝是否开启
    self.is_open = SmartData.New({flag = false})

    -- 用于打开灵宝升级界面时关闭界面的监听
    self.ling_bao_flag = SmartData.New()

    -- 灵宝升星界面用
    self.compose_data = SmartData.New()
    
    self:ResetData()
    self:InitData()
end

function LingBaoData:__delete()
    LingBaoData.Instance = nil
end

function LingBaoData:InitData()
    self.lingbao_lv_up_cfg = DataHelper.Subdivision(self.config.lingbao_level, {"hole_type", "level"})
    self.lingbao_lv_attr_cfg = DataHelper.Subdivision(self.config.strengthen_attr, {"shenghen_type", "hole_type", "strengthen_level"})
    self.resonance_cfg = DataHelper.Subdivision(self.config.resonante, {"shenghen_type", "resonante_level", "lingbao_num"})
    self.shenghen_lv_attr_cfg = DataHelper.Subdivision(self.config.trace_awaken_attr, {"shenghen_type", "awaken_level"})
    self.shenghen_skill_cfg = DataHelper.Subdivision(self.config.trace_skill, {"shenghen_type", "trace_skill_pos", "trace_skill_level"})

    -- 圣痕最大强化等级
    self.shenghen_max_lv = #self.config.trace_awaken_consume
    -- 灵宝最大等级
    self.lingbao_max_lv = #self.lingbao_lv_up_cfg[ShengHenType.Role]
    -- 灵宝最大阶数
    self.lingbao_max_state = self.lingbao_lv_up_cfg[ShengHenType.Role][self.lingbao_max_lv].class_level
    -- 灵宝最大强化等级
    self.lingbao_max_str_lv = #self.config.strengthen_consume

    Cfg.Register("LingBaoLvUp", Cfg.RealTimeSearch(self.config.lingbao_level, "item_id"))
end

-- 主界面当前选中的圣痕类型
function LingBaoData:MainShengHenType(shenghen_type)
    if shenghen_type then
        self.main_shenghen_type = shenghen_type
    else
        return self.main_shenghen_type or ShengHenType.Partner
    end
end

-- 主界面当前选中的技能
function LingBaoData:MainShengHenSkillIndex(index)
    if index then
        self.main_shenghen_skill_index = index
    else
        return self.main_shenghen_skill_index or 0
    end
end

-- 主界面当前选中的灵宝类型
function LingBaoData:MainLingBaoType(lingbao_type)
    if lingbao_type then
        self.main_lingbao_type = lingbao_type
    else
        return self.main_lingbao_type or LingBaoType.Hairpin
    end
end

-- 灵宝背包获取途径表
function LingBaoData:GetWayConfig()
    return self.config.get_way
end

-- 圣痕最大强化等级
function LingBaoData:ShengHenMaxLv()
    return self.shenghen_max_lv
end

-- 灵宝最大等级
function LingBaoData:LingBaoMaxLv()
    return self.lingbao_max_lv
end

-- 灵宝最大阶数
function LingBaoData:LingBaoMaxState()
    return self.lingbao_max_state
end

-- 灵宝最大强化等级
function LingBaoData:LingBaoMaxStrLv()
    return self.lingbao_max_str_lv
end

-- 灵宝主界面问号说明
function LingBaoData:MainDesc()
    return self.other.lingbao_desc
end

-- 灵宝共鸣界面问号说明
function LingBaoData:ResonanceDesc()
    return self.other.resonante_desc
end

-- 获取途径表
function LingBaoData:GetWayData()
    return self.config.get_way
end

-- 圣痕背景图片路径
function LingBaoData:ShengHenBgPath(shenghen_type)
    return "ui/textures/ling_bao/ShengHenBg"..shenghen_type..".png"
end

-- 圣痕背景图片底字路径
function LingBaoData:ShengHenFontBgPath(shenghen_type)
    return "ui/textures/ling_bao/ShengHenFontBg"..shenghen_type..".png"
end

function LingBaoData:ResetData()
    self:ResetInfoData()
    self.item_list:Clear()
    self:ResetEquipList()
    self.item_num_data = {}
    self.item_by_type_data = {}
    self.resonance_info = {}
    self:ResetComposeData()
    self:ResetLingBaoStrChange()
    self.is_open.flag = false
end


-- 用于灵宝强化数值改变
function LingBaoData:ResetLingBaoStrChange()
    self.lingbao_str_change = {}
    for k, v in pairs(ShengHenType) do
        self.lingbao_str_change[v] = {}
        for l, w in pairs(LingBaoType) do
            self.lingbao_str_change[v][w] = 0
        end
    end
end

function LingBaoData:GetLingBaoStrChange(shenghen_type, lingbao_type)
    if shenghen_type and lingbao_type then
        return self.lingbao_str_change[shenghen_type][lingbao_type]
    elseif shenghen_type then
        return self.lingbao_str_change[shenghen_type]
    end
    return self.lingbao_str_change
end

function LingBaoData:SetLingBaoStrChange(shenghen_type, lingbao_type, value)
    self.lingbao_str_change[shenghen_type][lingbao_type] = value
end

function LingBaoData:ResetComposeData()
    self.compose_data.shenghen_type = ShengHenType.Partner
    self.compose_data.lingbao_type = LingBaoType.Hairpin
    self.compose_data.state = 1
    self.compose_data.star = 2
    self.compose_data.level = 2
end

function LingBaoData:ResetInfoData()
    self.info_data:Clear()
    for k, v in pairs(ShengHenType) do
        local t = {}
        t.shenghen_type = v
        t.is_unlock = 0                             --是否解锁
        t.level = 0                                 --觉醒等级
        t.skill_count = LingBaoConfig.SkillMax      --技能数量
        t.skill_level = {}
        for i = 0, t.skill_count - 1 do
            t.skill_level[i] = 0                    --技能等级，0：未解锁
        end
        t.lingbao_count = LingBaoConfig.LingBaoMax  --灵宝数量
        t.lingbao_str_lv = {}
        for i = 0, t.lingbao_count - 1 do
            t.lingbao_str_lv[i] = 0                 --强化等级
        end
        self.info_data[v] = t
    end
end

function LingBaoData:ResetEquipList()
    self.equip_list:Clear()
    for i, v in pairs(ShengHenType) do
        for j, w in pairs(LingBaoType) do
            if TableIsEmpty(self.equip_list[v]) then
                self.equip_list[v] = {}
            end
            local t = {}
            t.shenghen_type = v
            t.lingbao_type = w
            t.item = {}
            self.equip_list[v][w] = t
        end
    end
end

-- 消耗所需材料item_list，红点用
function LingBaoData:LingBaoRemindItemList()
    if not self.lingbao_item_id_list then
        self.lingbao_item_id_list = {}
        local item_id_list = {}
        -- 圣痕强化消耗
        for k, v in pairs(self.config.trace_awaken_consume) do
            if not item_id_list[v.awaken_consume] then
                item_id_list[v.awaken_consume] = v.awaken_consume
            end
        end
        for k, v in pairs(item_id_list) do
            table.insert(self.lingbao_item_id_list, v)
        end
    end
    return self.lingbao_item_id_list
end

-- 返回灵宝强化消耗的item_id
function LingBaoData:LingBaoStrItem()
    return HuoBi[CurrencyType.GodSpaceEssence]
end

-- 返回当前灵宝强化消耗的道具的数量
function LingBaoData:LingBaoStrItemNum()
    -- 防止同一帧执行多次，红点计算时
    if self.calculate_frame_count1 ~= Time.frameCount then
        self.calculate_frame_count1 = Time.frameCount
        self.lingbao_str_item_num = RoleData.Instance:GetCurrency(CurrencyType.GodSpaceEssence)
    end
    return self.lingbao_str_item_num or 0
end

-- 总红点
function LingBaoData:Remind()
    if not FunOpen.Instance:IsFunOpen(Mod.LingBao.Main) then
        return 0
    end
    for k, v in pairs(ShengHenType) do
        if self:ShengHenRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

-- 灵宝是否开启，服务端控制
function LingBaoData:IsOpen()
    return self.is_open.flag == true
end

-- 圣痕标签页红点
function LingBaoData:ShengHenRemind(shenghen_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    if self:BagRemind(shenghen_type) > 0 then
        return 1
    elseif self:LingBaoLvRemind(shenghen_type) > 0 then
        return 1
    elseif self:LingBaoStrRemind(shenghen_type) > 0 then
        return 1
    elseif self:ShengHenStrRemind(shenghen_type) > 0 then
        return 1
    elseif self:ShengHenSkillRemind(shenghen_type) > 0 then
        return 1
    end
    return 0
end

-- 圣痕通用判断
function LingBaoData:ShengHenComomJudge(shenghen_type)
    if not self:ShengHenIsUnLock(shenghen_type) then
        return false
    end
    return true
end

-- 对应圣痕类型的背包红点
function LingBaoData:BagRemind(shenghen_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    for k, v in pairs(LingBaoType) do
        local now_equip = self:EquipList(shenghen_type, v)
        local lv = 0
        if not TableIsEmpty(now_equip.item) then
            lv = LingBao.GetShowLevel(now_equip.item.item_id)
        end
        -- 遍历对应圣痕类型灵宝类型所有道具，看看有没有比这个更高级的
        for l, w in pairs(self:ItemListByType(v)) do
            if w:ShowLevel() > lv then
                return 1
            end
        end
    end
    return 0
end

-- 灵宝升星红点1，计算该圣痕中所有灵宝星级可升星
function LingBaoData:LingBaoLvRemind(shenghen_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    for k, v in pairs(LingBaoType) do
        if self:LingBaoLvRemind1(shenghen_type, v) > 0 then
            return 1
        end
    end
    return 0
end

-- 灵宝升星红点2，计算该圣痕中的对应灵宝中所有星级可升星
function LingBaoData:LingBaoLvRemind1(shenghen_type, lingbao_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    for i = 1, self.lingbao_max_lv do
        if self:LingBaoLvRemind2(shenghen_type, lingbao_type, i) > 0 then
            return 1
        end
    end
    return 0
end

-- 灵宝升星红点3，计算该圣痕中的对应灵宝中对应星级可升星
function LingBaoData:LingBaoLvRemind2(shenghen_type, lingbao_type, level)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    -- 一级的不能合成
    if level == 1 then
        return 0
    end
    local now_equip = self:EquipList(shenghen_type, lingbao_type)
    local lingbao_cfg = self:LingBaoLvUpCfg(lingbao_type, level)
    local num = self:ItemNum(lingbao_cfg.compose_item)
    if (not TableIsEmpty(now_equip.item)) and lingbao_cfg.compose_item == now_equip.item.item_id then
        if num >= (LingBaoConfig.ComposeNum - 1) then
            return 1
        end
    else
        if num >= LingBaoConfig.ComposeNum then
            return 1
        end
    end
    return 0
end

-- 灵宝强化红点1，计算该圣痕中所有灵宝可强化
function LingBaoData:LingBaoStrRemind(shenghen_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    for k, v in pairs(LingBaoType) do
        if self:LingBaoStrRemind1(shenghen_type, v) > 0 then
            return 1
        end
    end
    return 0
end

-- 灵宝强化红点2，计算该圣痕中对应灵宝可强化
function LingBaoData:LingBaoStrRemind1(shenghen_type, lingbao_type)
    local now_equip = self:EquipList(shenghen_type, lingbao_type)
    if not TableIsEmpty(now_equip.item) then
        if self:LingBaoIsCanStr(shenghen_type, lingbao_type) then
            return 1
        end
    end
    return 0
end

-- 圣痕强化红点
function LingBaoData:ShengHenStrRemind(shenghen_type)
    if self:ShengHenIsCanStr(shenghen_type) then
        return 1
    end
    return 0
end

-- 圣痕技能总红点
function LingBaoData:ShengHenSkillRemind(shenghen_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    for i = 0, LingBaoConfig.SkillMax - 1 do
        if self:ShengHenSkillRemind1(shenghen_type, i) > 0 then
            return 1
        end
    end
    return 0
end

-- 圣痕技能对应孔位红点
function LingBaoData:ShengHenSkillRemind1(shenghen_type, index)
    if self:ShengHenSkillIsCanUp(shenghen_type, index) then
        return 1
    end
    return 0
end

-- 灵宝对应部位红点
function LingBaoData:LingBaoEquipRemind(shenghen_type, lingbao_type)
    if not self:ShengHenComomJudge(shenghen_type) then
        return 0
    end
    local now_equip = self:EquipList(shenghen_type, lingbao_type)
    if TableIsEmpty(now_equip.item) then
        -- 判断可穿戴红点
        if not TableIsEmpty(self:ItemListByType(lingbao_type)) then
            return 1
        end
    else
        -- 判断可强化红点
        if self:LingBaoIsCanStr(shenghen_type, lingbao_type) then
            return 1
        end
    end
    return 0
end

-- 圣痕界面的灵宝是否可一键强化
function LingBaoData:LingBaoIsCanStrAll(shenghen_type)
    local language = Language.LingBao.LingBaoStrAllTip
    if not self:ShengHenComomJudge(shenghen_type) then
        -- 没解锁
        return false, language[1], 1
    end

    local ware_list, ware_num = self:HasWareLingBaoList(shenghen_type)
    local is_ware_all = ware_num == LingBaoConfig.LingBaoMax
    if TableIsEmpty(ware_list) then
        -- 一件没穿
        return false, language[2], 2
    end
    
    -- 强化到当前限制上限数量
    local all_max_num = 0
    -- 强化到最高数量
    local all_true_max_num = 0
    for i, v in ipairs(ware_list) do
        local str_lv = self:LingBaoStrLv(shenghen_type, v.lingbao_type)
        if str_lv == self.lingbao_max_str_lv then
            all_max_num = all_max_num + 1
            all_true_max_num = all_true_max_num + 1
        else
            local next_cfg = self:LingBaoStrengthSpendCfg(str_lv + 1)
            if not TableIsEmpty(next_cfg) then
                local state = LingBao.GetStateLevel(v.item.item_id)
                if next_cfg.class_limit > state then
                    all_max_num = all_max_num + 1
                end
            else
                all_true_max_num = all_true_max_num + 1
            end
        end
    end

    if all_max_num == ware_num or all_true_max_num == ware_num then
        if all_max_num == ware_num then
            -- 当前穿戴的所有灵宝强化达到当前限制，需要升阶
            return false, language[4], 4
        end
        if all_true_max_num == ware_num then
            if ware_num == LingBaoConfig.LingBaoMax then
                -- 所有灵宝强化已满
                return false, language[3], 3
            else
                -- 当前穿戴的所有灵宝强化已满，但还能穿其他部位
                return false, language[2], 2
            end
        end
    end

    local has_num = self:LingBaoStrItemNum()
    local need_num = self:LingBaoStrAllNeed(shenghen_type)
    local need_add = need_num - has_num
    if need_add > 0 then
        return false, Format(language[5], Item.GetName(self:LingBaoStrItem()), need_add), 5, self:LingBaoStrItem(), need_add
    end
    return true
end

-- 判断灵宝是否都强化满级
function LingBaoData:IsAllFullStr(shenghen_type)
    for k, v in pairs(LingBaoType) do
        if self.lingbao_max_str_lv ~= self:LingBaoStrLv(shenghen_type, v) then
            return false
        end
    end
    return true
end

-- 根据强化等级从低到高返回有穿戴灵宝的列表
function LingBaoData:HasWareLingBaoList(shenghen_type)
    local list = {}
    local t = {}
    for k, v in pairs(self:EquipList(shenghen_type)) do
        if not TableIsEmpty(v.item) then
            table.insert(t, v.lingbao_type)
        end
    end
    table.sort(t, function(a_data, b_data)
        return self:LingBaoStrLv(shenghen_type, b_data) > self:LingBaoStrLv(shenghen_type, a_data)
    end)
    for i, v in ipairs(t) do
        table.insert(list, self:EquipList(shenghen_type, v))
    end
    return list, #list
end

-- 圣痕界面的一键强化需要多少材料
function LingBaoData:LingBaoStrAllNeed(shenghen_type)
    local num = self:LingBaoStrengthSpendCfg(1).quintessence
    local ware_list, ware_num = self:HasWareLingBaoList(shenghen_type)
    if ware_num > 0 then
        local str_lv = self:LingBaoStrLv(shenghen_type, ware_list[1].lingbao_type)
        if str_lv ~= self.lingbao_max_str_lv then
            local next_cfg = self:LingBaoStrengthSpendCfg(str_lv + 1)
            if not TableIsEmpty(next_cfg) then
                num = next_cfg.quintessence
            end
        end
    end
    return num
end

-- 获得灵宝强化总等级
function LingBaoData:GetLingBaoStrAll(shenghen_type)
    local total_lingbao_lv = 0
    for k, v in pairs(self:LingBaoStrLv(shenghen_type)) do
        total_lingbao_lv = total_lingbao_lv + v
    end
    return total_lingbao_lv
end

-- 判断灵宝是否可强化
function LingBaoData:LingBaoIsCanStr(shenghen_type, lingbao_type)
    local language = Language.LingBao.LingBaoStrTip
    if not self:ShengHenComomJudge(shenghen_type) then
        return false, language[1], 1
    end
    local now_equip = self:EquipList(shenghen_type, lingbao_type)
    if TableIsEmpty(now_equip.item) then
        return false, language[2], 2
    end
    local str_lv = self:LingBaoStrLv(shenghen_type, lingbao_type)
    if str_lv == self.lingbao_max_str_lv then
        return false, language[3], 3
    end
    local next_cfg = self:LingBaoStrengthSpendCfg(str_lv + 1)
    if not TableIsEmpty(next_cfg) then
        local state = LingBao.GetStateLevel(now_equip.item.item_id)
        if next_cfg.class_limit > state then
            return false, Format(language[4], DataHelper.GetDaXie(next_cfg.class_limit)), 4, next_cfg.class_limit
        end
    else
        return false, language[3], 3
    end
    local has_num = self:LingBaoStrItemNum()
    local need_add = next_cfg.quintessence - has_num
    if need_add > 0 then
        return false, Format(language[5], Item.GetName(self:LingBaoStrItem()), next_cfg.quintessence - has_num), 5, self:LingBaoStrItem(), need_add
    end
    return true
end

-- 判断圣痕是否可强化
function LingBaoData:ShengHenIsCanStr(shenghen_type)
    local language = Language.LingBao.ShengHenStrTip
    if not self:ShengHenComomJudge(shenghen_type) then
        return false, language[1], 1
    end
    local now_equip_list = self:EquipList(shenghen_type)
    for k, v in pairs(now_equip_list) do
        if TableIsEmpty(v.item) then
            return false, language[2], 2
        end
    end
    local str_lv = self.info_data[shenghen_type].level
    if str_lv == self.shenghen_max_lv then
        return false, language[3], 3
    end
    local next_cfg = self:ShengHenLvUpSpendCfg(str_lv + 1)
    if not TableIsEmpty(next_cfg) then
        local total_lingbao_lv = self:GetLingBaoStrAll(shenghen_type)
        if next_cfg.strengthen_limit > total_lingbao_lv then
            return false, Format(language[4], next_cfg.strengthen_limit), 4
        end
    else
        return false, language[3], 3
    end
    local has_num = Item.GetNum(next_cfg.awaken_consume)
    local need_add = next_cfg.consume_num - has_num
    if need_add > 0 then
        return false, Format(language[5], Item.GetName(next_cfg.awaken_consume), next_cfg.consume_num - has_num), 5, next_cfg.awaken_consume, need_add
    end
    return true
end

-- 判断圣痕技能是否可升级
function LingBaoData:ShengHenSkillIsCanUp(shenghen_type, index)
    local language = Language.LingBao.ShengHenSkillUpTip
    if not self:ShengHenComomJudge(shenghen_type) then
        return false, language[1], 1
    end
    local str_lv = self:ShengHenSkillLv(shenghen_type, index)
    local next_cfg = self:ShengHenSkillCfg(shenghen_type, index, str_lv + 1)
    if not TableIsEmpty(next_cfg) then
        if next_cfg.awaken_level > self.info_data[shenghen_type].level then
            return false, Format(language[3], next_cfg.awaken_level), 3, next_cfg.awaken_level
        end
    else
        return false, language[2], 2
    end
    return true
end

function LingBaoData:ItemList(index)
    return index and self.item_list[index] or self.item_list
end

function LingBaoData:EquipList(shenghen_type, lingbao_type)
    if shenghen_type then
        if self.equip_list[shenghen_type] then
            if lingbao_type then
                return self.equip_list[shenghen_type][lingbao_type]
            else
                return self.equip_list[shenghen_type]
            end
        else
            return
        end
    end
    return self.equip_list
end

function LingBaoData:InfoData(shenghen_type)
    return shenghen_type and self.info_data[shenghen_type] or self.info_data
end

-- 返回当前圣痕穿了多少件灵宝
function LingBaoData:EquipCount(shenghen_type)
    local now_equip_list = self:EquipList(shenghen_type)
    local num = 0
    for k, v in pairs(now_equip_list) do
        if not TableIsEmpty(v.item) then
            num = num + 1
        end
    end
    return num
end

-- 圣痕中灵宝的强化等级
function LingBaoData:LingBaoStrLv(shenghen_type, lingbao_type)
    if lingbao_type then
        return self.info_data[shenghen_type].lingbao_str_lv[lingbao_type]
    end
    return self.info_data[shenghen_type].lingbao_str_lv
end

-- 圣痕技能等级
function LingBaoData:ShengHenSkillLv(shenghen_type, skill_index)
    if skill_index then
        return self.info_data[shenghen_type].skill_level[skill_index]
    end
    return self.info_data[shenghen_type].skill_level
end

-- 判断该圣痕是否有解锁
function LingBaoData:ShengHenIsUnLock(shenghen_type)
    return self.info_data[shenghen_type].is_unlock > 0
end

-- 整个背包信息下发
function LingBaoData:SetBagInfo(protocol)
    self.item_list:Clear()
    for k, v in pairs(protocol.item_list) do
        self.item_list[v.index] = SmartData.New(LingBao.New(v))
    end
    self:CalculateItemNum()

    self.item_list:Notify()
end

-- 单个背包格子信息变化
function LingBaoData:SetBagSingleInfo(protocol)
    -- 先走一次飘字
	if protocol.item_info.item_id ~= 0 and protocol.reason ~= BAGITEM_CHANGEREASON.INSIDE_OUT then
        local item_cell = LingBao.New(protocol.item_info)
        if protocol.reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            PublicPopupCtrl.Instance:CenterI({item_id = item_cell.item_id, num = item_cell.num,
                color_str = item_cell:ColorStr(), item = item_cell, icon_id = item_cell:IconId()})
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item_cell, num = item_cell.num}}
        end
        self.item_list[protocol.item_info.index] = SmartData.New(LingBao.New(protocol.item_info))
    else
        self.item_list[protocol.item_info.index] = nil
    end
    self:CalculateItemNum()

    self.item_list:Notify()
end

-- 身上灵宝道具全部下发
function LingBaoData:SetEquipListInfo(protocol)
    self:ResetEquipList()
    for k, v in pairs(protocol.ware_list) do
        self.equip_list[v.shenghen_type][v.lingbao_type] = v
    end

    for k, v in pairs(ShengHenType) do
        self:SetResonanceInfo(v)
    end

    self.equip_list:Notify()
end

-- 身上灵宝道具单个下发
function LingBaoData:SetEquipListSingleInfo(protocol)
    self.equip_list[protocol.info.shenghen_type][protocol.info.lingbao_type] = protocol.info
    self:SetResonanceInfo(protocol.info.shenghen_type)

    self.equip_list:Notify()
end

-- 圣痕信息全部下发
function LingBaoData:SetShengHenInfo(protocol)
    for k, v in pairs(protocol.shenghen_info) do
        self.info_data[v.shenghen_type] = v
    end
    self.is_open.flag = protocol.is_open == 1
    self.info_data:Notify()
end

-- 圣痕中所有灵宝强化信息单条下发
function LingBaoData:SetShengHenStrSingleInfo(protocol)
    -- 主界面一键强化的时候会发一遍
    for k, v in pairs(protocol.lingbao_str_lv) do
        local add = v - self.info_data[protocol.shenghen_type].lingbao_str_lv[k]
        self:SetLingBaoStrChange(protocol.shenghen_type, k, add)
    end
    self.info_data[protocol.shenghen_type].lingbao_str_lv = protocol.lingbao_str_lv

    self.info_data:Notify()
end

-- 灵宝系统相关通知下发
-- protocol.notice_type
-- protocol.param1
-- protocol.param2
-- protocol.param3
function LingBaoData:SetNotifyInfo(protocol)
    if protocol.notice_type == LingBaoNoticeType.LingBaoStrUp then
        -- 灵宝强化，只有单个灵宝部位的强化走这里
        -- local add = protocol.param3 - self.info_data[protocol.param1].lingbao_str_lv[protocol.param2]
        -- self:SetLingBaoStrChange(protocol.param1, protocol.param2, add)
        self.info_data[protocol.param1].lingbao_str_lv[protocol.param2] = protocol.param3
        self.info_data:Notify()
    elseif protocol.notice_type == LingBaoNoticeType.ShengHenStrUp then
        -- 圣痕觉醒升级
        self.info_data[protocol.param1].level = protocol.param2
        self.info_data:Notify()
    elseif protocol.notice_type == LingBaoNoticeType.ShengHenUnLock then
        -- 圣痕解锁弹窗，需要关闭圣痕觉醒界面的时候再弹
        self:ShengHenUnlockFlag(protocol.param1)
    elseif protocol.notice_type == LingBaoNoticeType.ShengHenSkillUp then
        -- 圣痕技能升级
        self.info_data[protocol.param1].skill_level[protocol.param2] = protocol.param3
        self.info_data:Notify()
    elseif protocol.notice_type == LingBaoNoticeType.LingBaoLvUp then
        -- 灵宝合成
        ViewMgr:OpenView(LingBaoGetView, {item_id = protocol.param3})
    elseif protocol.notice_type == LingBaoNoticeType.IsOpenFlag then
        -- 灵宝是否开启
        self.is_open.flag = protocol.param1 == 1
        LandsBetweenData.Instance:ForceFlushExtra()
        MainViewData.Instance:FlushRbButton()
    end
end

-- 圣痕解锁需要弹窗的标志
function LingBaoData:ShengHenUnlockFlag(value)
    if value then
        self.shenghen_unlock_flag = value
    else
        return self.shenghen_unlock_flag or -1
    end
end

-- 计算
function LingBaoData:CalculateItemNum()
    self.item_num_data = {}
    self.item_by_type_data = {}
    for k, v in pairs(self.item_list) do
        -- 计算item_id对应数量，并存储下index，数量不包括身上的
        if not self.item_num_data[v.item_id] then
            local t = {}
            t.num = 1
            t.index_list = {}
            table.insert(t.index_list, v.index)
			self.item_num_data[v.item_id] = t
        else
			self.item_num_data[v.item_id].num = self.item_num_data[v.item_id].num + 1
            table.insert(self.item_num_data[v.item_id].index_list, v.index)
        end
        -- 存下对应灵宝类型到对应表
        local lingbao_type = v:LingBaoType()
        if not self.item_by_type_data[lingbao_type] then
            self.item_by_type_data[lingbao_type] = {}
        end
        table.insert(self.item_by_type_data[lingbao_type], v)
    end
end

-- 返回该道具数量和对应的index_list，数量不包括身上的
function LingBaoData:ItemNum(item_id)
    if self.item_num_data[item_id] then
        return self.item_num_data[item_id].num, self.item_num_data[item_id].index_list
    else
        return 0, {}
    end
end

-- 根据灵宝类型返回背包的item_list
function LingBaoData:ItemListByType(lingbao_type)
    return self.item_by_type_data[lingbao_type] or {}
end

-- 根据灵宝类型和灵宝等级获得对应道具数量和对应的index_list，数量不包括身上的
function LingBaoData:ItemNumByTypeLv(lingbao_type, level)
    local item_id = self:LingBaoLvUpCfg(lingbao_type, level).item_id
    return self:ItemNum(item_id)
end

-- 圣痕评分
function LingBaoData:ShengHenScore(shenghen_type)
    local score = 0
    if self:ShengHenComomJudge(shenghen_type) then
        -- 灵宝自身评分
        for k, v in pairs(self:EquipList(shenghen_type)) do
            if not TableIsEmpty(v.item) then
                local cfg = self:LingBaoLvUpCfg(shenghen_type, LingBao.GetShowLevel(v.item.item_id))
                score = score + cfg.score
            end
        end
        -- 灵宝强化评分
        for k, v in pairs(self:LingBaoStrLv(shenghen_type)) do
            local cfg = self:LingBaoLvAttrCfg(shenghen_type, k, v)
            if not TableIsEmpty(cfg) then
                score = score + cfg.score
            end
        end
        -- 共鸣评分
        for k, v in pairs(self:GetResonanceInfo(shenghen_type)) do
            score = score + v.score
        end
        -- 圣痕觉醒评分
        local cfg1 = self:ShengHenLvAttrCfg(shenghen_type, self.info_data[shenghen_type].level)
        if not TableIsEmpty(cfg1) then
            score = score + cfg1.score
        end
        -- 圣痕技能评分
        for k, v in pairs(self.info_data[shenghen_type].skill_level) do
            local cfg = self:ShengHenSkillCfg(shenghen_type, k, v)
            if not TableIsEmpty(cfg) then
                score = score + cfg.score
            end
        end
    end
    return score
end

-- 计算灵宝共鸣
function LingBaoData:SetResonanceInfo(shenghen_type)
    self.resonance_info[shenghen_type] = {}
    local lv_list = {}
    for k, v in pairs(self:EquipList(shenghen_type)) do
        if not TableIsEmpty(v.item) then
            local show_lv = LingBao.GetShowLevel(v.item.item_id)
            table.insert(lv_list, show_lv)
        end
    end
    local ware_count = #lv_list
    -- 从大到小排序
    table.sort(lv_list, function(a_data, b_data)
        return a_data > b_data
    end)
    for i, v in ipairs(LingBaoConfig.Suit) do
        if ware_count >= v then
            table.insert(self.resonance_info[shenghen_type], self:ResonanceCfg(shenghen_type, lv_list[v], v))
        else
            break
        end
    end
end

-- 灵宝共鸣
function LingBaoData:GetResonanceInfo(shenghen_type)
    return self.resonance_info[shenghen_type] or {}
end

-- 灵宝共鸣神域额外加成数值累加
function LingBaoData:GetResonanceAddValue(shenghen_type)
    local add = 0
    for k, v in pairs(self:GetResonanceInfo(shenghen_type)) do
        add = add + v.shenyu_add_value
    end
    return add
end

-- 根据item_id获得升级和分解信息
function LingBaoData:LingBaoLvUpCfgById(item_id)
    return Cfg.LingBaoLvUp(item_id, false)
end

-- 根据灵宝类型和等级获得升级和分解信息
function LingBaoData:LingBaoLvUpCfg(hole_type, level)
    if hole_type and level then
        return self.lingbao_lv_up_cfg[hole_type][level] or {}
    elseif hole_type then
        return self.lingbao_lv_up_cfg[hole_type] or {}
    end
    return self.lingbao_lv_up_cfg
end

-- 圣痕排序后返回
function LingBaoData:ShengHenSort(shenghen_type)
    if TableIsEmpty(self.shenghen_sort_cfg) then
        self.shenghen_sort_cfg = self.config.shenghen
        table.sort(self.shenghen_sort_cfg, DataSorter.KeyLowerSorter("order"))
        self.shenghen_order_change = {}
        for i, v in ipairs(self.shenghen_sort_cfg) do
            self.shenghen_order_change[v.shenghen_type] = v.order
        end
    end
    if shenghen_type then
        for i, v in ipairs(self.shenghen_sort_cfg) do
            if shenghen_type == v.shenghen_type then
                return v
            end
        end
    end
    return self.shenghen_sort_cfg, self.shenghen_order_change
end

-- 灵宝强化消耗表
function LingBaoData:LingBaoStrengthSpendCfg(level)
    return self.config.strengthen_consume[level] or {}
end

-- 根据圣痕类型、灵宝类型和等级获得属性
function LingBaoData:LingBaoLvAttrCfg(shenghen_type, hole_type, level)
    if shenghen_type and hole_type and level then
        return self.lingbao_lv_attr_cfg[shenghen_type][hole_type][level] or {}
    elseif shenghen_type and hole_type then
        return self.lingbao_lv_attr_cfg[shenghen_type][hole_type] or {}
    elseif shenghen_type then
        return self.lingbao_lv_attr_cfg[shenghen_type] or {}
    end
    return self.lingbao_lv_attr_cfg
end

-- 根据圣痕类型、共鸣等级和套装件数获得共鸣属性
function LingBaoData:ResonanceCfg(shenghen_type, resonante_level, lingbao_num)
    if shenghen_type and resonante_level and lingbao_num then
        return self.resonance_cfg[shenghen_type][resonante_level][lingbao_num] or {}
    elseif shenghen_type and resonante_level then
        return self.resonance_cfg[shenghen_type][resonante_level] or {}
    elseif shenghen_type then
        return self.resonance_cfg[shenghen_type] or {}
    end
    return self.resonance_cfg
end

-- 圣痕强化消耗表
function LingBaoData:ShengHenLvUpSpendCfg(level)
    return self.config.trace_awaken_consume[level] or {}
end

-- 根据圣痕类型和觉醒等级获得圣痕觉醒属性
function LingBaoData:ShengHenLvAttrCfg(shenghen_type, level)
    if shenghen_type and level then
        return self.shenghen_lv_attr_cfg[shenghen_type][level] or {}
    elseif shenghen_type then
        return self.shenghen_lv_attr_cfg[shenghen_type] or {}
    end
    return self.shenghen_lv_attr_cfg
end

-- 根据圣痕类型和技能孔位和技能等级获得圣痕技能属性
function LingBaoData:ShengHenSkillCfg(shenghen_type, trace_skill_pos, level)
    if shenghen_type and trace_skill_pos and level then
        return self.shenghen_skill_cfg[shenghen_type][trace_skill_pos][level] or {}
    elseif shenghen_type and trace_skill_pos then
        return self.shenghen_skill_cfg[shenghen_type][trace_skill_pos] or {}
    elseif shenghen_type then
        return self.shenghen_skill_cfg[shenghen_type] or {}
    end
    return self.shenghen_skill_cfg
end

-- 圣痕技能信息表
function LingBaoData:ShengHenSkillData(shenghen_type)
    local skill_data = {}
    for k, v in pairs(self.info_data[shenghen_type].skill_level) do
        local t = {}
        t.shenghen_type = shenghen_type
        t.skill_index = k
        t.skill_level = v
        skill_data[k] = t
    end
    return skill_data
end

-- 获得某一行数据的属性列表，数据，是否清空值（用于0级的属性显示拿1级的置空）
function LingBaoData:GetAttrList(config, is_zero)
    local index = 1
    local t = {}
    -- if TableIsEmpty(config) then
    --     return t
    -- end
    while config["attr_add_"..index] and config["attr_add_"..index] > 0 do
        local t1 = CommonStruct.AttrStruct(config["attr_type_"..index], config["attr_add_"..index])
        if is_zero then
            t1 = CommonStruct.AttrStruct(config["attr_type_"..index], 0)
        end
        table.insert(t, t1)
        index = index + 1
    end
    return t
end

-- 获取背包的灵宝数据
-- 是否补齐，补齐到最小数量，数量需要能整除多少
-- 灵宝类型：不填或者-1就是默认全部，阶数：不填或者0就是默认全部
function LingBaoData:GetBagData(is_fill, min_num, sqrt_num, lingbao_type, state)
    local bag_list = {}
    for k, v in pairs(self.item_list) do
        local is_can = true
        if lingbao_type and lingbao_type ~= -1 then
            is_can = v:LingBaoType() == lingbao_type
        end
        if state and state ~= 0 and is_can then
            is_can = v:StateLevel() == state
        end
        if is_can then
            table.insert(bag_list, v)
        end
    end
    -- 排序：等级，类型，格子
    table.sort(bag_list, function(a_data, b_data)
        local a_lv = a_data:ShowLevel()
        local b_lv = b_data:ShowLevel()
        local a_type = a_data:LingBaoType()
        local b_type = b_data:LingBaoType()
        return (a_lv * 10000 - a_type * 1000 - a_data.index) > (b_lv * 10000 - b_type * 1000 - b_data.index)
    end)
    -- 背包的灵宝重算一遍better数据
    for k, v in pairs(bag_list) do
        local l_type = v:LingBaoType()
        local lv = 0
        local now_equip = self:EquipList(self:MainShengHenType(), l_type)
        if not TableIsEmpty(now_equip.item) then
            lv = LingBao.GetShowLevel(now_equip.item.item_id)
        end
        v.is_better = v:ShowLevel() > lv
    end
    -- 填补
    if is_fill then
        local need_add = min_num - #bag_list
        if need_add < 0 then
            need_add = math.abs(need_add)
            need_add = (sqrt_num - need_add % sqrt_num) % sqrt_num
        end
        for i = 1, need_add do
            table.insert(bag_list, LingBao.New())
        end
    end
    return bag_list
end

-- 根据传入的item_list，返回筛选阶数的表
function LingBaoData:GetStateDataByList(now_select)
    local t_list = {}
    local t = {}
    t[0] = 1
    for i = 1, self.lingbao_max_state do
        t[i] = 0
    end
    local bag_list = self:GetBagData()
    for k, v in pairs(bag_list) do
        if v.item_id > 0 and v.StateLevel then
            local state = v:StateLevel()
            t[state] = 1
        end
    end
    for i = 0, self.lingbao_max_state do
        if t[i] > 0 then
            local t1 = {}
            t1.state = i
            if i == 0 then
                t1.name = Language.LingBao.All
            else
                t1.name = Format(Language.LingBao.ItemState, DataHelper.GetDaXie(i))
            end
            t1.is_select = i == now_select
            table.insert(t_list, t1)
        end
    end
    return t_list
end

-- 根据装备列表获取分解物品
function LingBaoData:GetResolveList(data)
	local marks = {}
	for k, v in pairs(data or {}) do 
		local cfg = Cfg.LingBaoLvUp(v.item_id)
		if not TableIsEmpty(cfg) then 
			local item = Item.Create(cfg.decompose)
			if marks[item.item_id] == nil then 
				marks[item.item_id] = item
			else 
				marks[item.item_id].num = item.num + marks[item.item_id].num
			end 
		end 
	end 

	local result = {}
	for k,v in pairs(marks) do 
		table.insert(result, v)
	end 
	return result
end

-- 用于灵宝升星的灵宝类型数据
function LingBaoData:LingBaoTypeData()
    local data = {}
    for k, v in pairs(LingBaoType) do
        local t = {}
        t.lingbao_type = v
        data[v] = t
    end
    return data
end

-- 用于灵宝升星的圣痕类型数据
function LingBaoData:ShengHenTypeData()
    local data = {}
    for k, v in pairs(ShengHenType) do
        local t = {}
        t.shenghen_type = v
        data[v] = t
    end
    return data
end

-- 灵宝升星用手风琴
function LingBaoData:ALingBaoLvUpData()
	if not self.a_lingbao_lv_up_data then
        self.a_lingbao_lv_up_data = {}
        local lv_cfg = self:LingBaoLvUpCfg(LingBaoType.Hairpin)
        for k, v in pairs(lv_cfg) do
            if not self.a_lingbao_lv_up_data[v.class_level] then
                local parent = {}
                parent.name = Format(Language.LingBao.ItemState, DataHelper.GetDaXie(v.class_level))
                parent.id = v.class_level
                parent.children = {}
                self.a_lingbao_lv_up_data[v.class_level] = parent
            end
        end
        for k, v in pairs(lv_cfg) do
            -- 等于0的是1等级为1的，无法合成
            if v.compose_item ~= 0 then
                local children = {}
                children.name = Format(Language.LingBao.ItemStar, v.star_level)
                children.parent_index = self.a_lingbao_lv_up_data[v.class_level].id
                children.star = v.star_level
                children.level = v.level
                table.insert(self.a_lingbao_lv_up_data[v.class_level].children, children)
            end
        end
        for k, v in pairs(self.a_lingbao_lv_up_data) do
            table.sort(v.children, DataSorter.KeyLowerSorter("level"))
            for k, v in pairs(v.children) do
                v.children_index = k
            end
        end
    end
	return self.a_lingbao_lv_up_data
end

-- 打开灵宝升星用手风琴时应该跳转到哪
function LingBaoData:ALingBaoLvUpJumpData(data)
    if self:LingBaoLvRemind2(self.compose_data.shenghen_type, self.compose_data.lingbao_type, self.compose_data.level) > 0 then
        if self.compose_data.state == 1 then
            return data[self.compose_data.state].children[self.compose_data.star - 1]
        else
            return data[self.compose_data.state].children[self.compose_data.star]
        end
    end
    for i, v in ipairs(data) do
        for j, w in ipairs(v.children) do
            if self:LingBaoLvRemind2(self.compose_data.shenghen_type, self.compose_data.lingbao_type, w.level) > 0 then
                return w
            end
        end
    end
    if self.compose_data.state == 1 then
        return data[self.compose_data.state].children[self.compose_data.star - 1]
    else
        return data[self.compose_data.state].children[self.compose_data.star]
    end
end

-- 灵宝升星界面计算应该跳到哪个灵宝按钮上
function LingBaoData:LingBaoLvUpJumpLingBaoType()
    local jump = self.compose_data.lingbao_type
    if self:LingBaoLvRemind1(self.compose_data.shenghen_type, self.compose_data.lingbao_type) > 0 then
        return jump
    end
    for i = 0, LingBaoConfig.LingBaoMax - 1 do
        if self:LingBaoLvRemind1(self.compose_data.shenghen_type, i) > 0 then
            return i
        end
    end
    return jump
end