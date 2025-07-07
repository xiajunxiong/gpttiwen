Accrssory = Accrssory or BaseClass(Item)

function Accrssory:__init(vo)
    vo = vo or {}
    self.index = vo.index or -1
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 1
    self.param = vo.param or {}
    self.is_grey = vo.is_grey
    self.in_bag = vo.in_bag == nil and true or vo.in_bag
    self:Check(vo)
end

function Accrssory:Check(vo)
    if TableIsEmpty(self.param.attr_list) then
        self.param.attr_list = {}
        for i = 1, ACCRSSORY_ATTR_MAX do
            table.insert(self.param.attr_list, {attr_type = 0, attr_value = 0})
        end
    end
    self.score = 0
    self.param.strength_level = self.param.strength_level or 0
    self.is_fake = vo.is_fake or false
    -- 默认是自己的
    if vo.is_mine ~= nil then
        self.is_mine = vo.is_mine
    else
        self.is_mine = true
    end
    -- 默认需要计算战力
    if vo.need_capability == nil or vo.need_capability == true then
        self:CalculateScore()
    end
end

function Accrssory:TypeDesc()
    return nil
end

-- 配饰类型
function Accrssory:AccrssoryType()
    return self:Config().equip_type
end

-- 配饰类型名字
function Accrssory:AccrssoryTypeName()
    return AccrssoryConfig.TypeName[self:Config().equip_type]
end

-- 套装类型
function Accrssory:SuitType()
    return self:Config().suit_type
end

-- 角标名称
function Accrssory:SuitName()
    return self:Config().suit_name
end

-- 返回所有额外词条属性
function Accrssory:GetAttrList()
    return self.param.attr_list, self:GetAttrMaxNum()
end

-- 最大词条数量
function Accrssory:GetAttrMaxNum()
    if not TableIsEmpty(self:Config()) then
        return self:Config().entry_num
    else
        return 5
    end
end

-- 有效词条数量
function Accrssory:GetHasAttrNum()
    local num = 0
    for k, v in pairs(self.param.attr_list or {}) do
        if v.attr_value > 0 then
            num = num + 1
        end
    end
    return num
end

-- 获取有效词条下标和有效词条数量
function Accrssory:GetEffectiveAttr()
    local t = {}
    local num = 0
    for i = 1, self:GetAttrMaxNum() do
        if self.param.attr_list[i] and self.param.attr_list[i].attr_value > 0 then
            num = num + 1
            table.insert(t, {pos = i})
        end
    end
    return t, num
end

-- 获取空词条下标和空词条数量
function Accrssory:GetEmptyAttr()
    local t = {}
    local num = 0
    for i = 1, self:GetAttrMaxNum() do
        if self.param.attr_list[i] and self.param.attr_list[i].attr_value <= 0 then
            num = num + 1
            table.insert(t, {pos = i})
        end
    end
    return t, num
end

-- 用于显示自己Item的BaseInfo
function Accrssory:OneselfBaseIndex()
    return 4
end

-- 获取天命加成数值（是否百分制）
function Accrssory:GetDestinyAdd(is_percent)
    local destiny_config = AccrssoryData.Instance:GetCurAttrInfo()
    local add = 0
    if not TableIsEmpty(destiny_config.attr_per_list) then
        add = destiny_config.attr_per_list[self:AccrssoryType()] or 0
    end
    return is_percent and add / 100 or add / 10000
end

-- 获取对比对象
function Accrssory:Compare()
    if self.in_bag and self.item_id > 0 then
        return AccrssoryData.Instance.accrssory_equip_data.info[self:AccrssoryType()]
    end
    return nil
end

-- 基础属性
function Accrssory:GetBaseAttr(show_percent)
    if self.item_id > 0 then
        return AccrssoryData.Instance:GetAccrssoryBaseAttr(self.item_id, show_percent)
    end
end

-- 强化属性
function Accrssory:StrengthAttr()
    if self.item_id > 0 then
        return AccrssoryData.Instance:StrengthAttrConfig(self.item_id, self:StrengthLevel())
    end
end

-- 计算评分
function Accrssory:CalculateScore()
    if self.item_id == 0 then
        return
    end
    self.score = 0
    local base_list = self:GetBaseAttr()
    local extra_list = self:GetAttrList()
    local strength_list = self:StrengthAttr()
    -- 基础属性
    for k, v in pairs(base_list) do
        -- 这里要算上天命加成
        local attr_value = (1 + self:GetDestinyAdd()) * v.attr_value
        self.score = self.score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, attr_value)
    end
    -- 额外属性
    for k, v in pairs(extra_list) do
        self.score = self.score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
    end
    -- 强化属性
    for k, v in pairs(strength_list) do
        self.score = self.score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
    end
end

-- 获得评分
function Accrssory:Score()
    return self.score or 0
end

-- 获取强化等级
function Accrssory:StrengthLevel()
    return self.param.strength_level or 0
end

-- 是否是飞升配饰
function Accrssory:IsFlyAccrssory()
    return self:GetFlyLevel() > 0
end

-- 获取需要飞升多少阶段
function Accrssory:GetFlyLevel()
    local config = Item.GetConfig(self.item_id)
    return config.feisheng_level
end

-- 是否是假的
function Accrssory:IsFake()
    return self.is_fake
end

function Accrssory.DecodeInfo()
    local t = {}
    t.index = MsgAdapter.ReadShort()            --序号
    t.item_id = MsgAdapter.ReadUShort()         --道具id
    t.num = MsgAdapter.ReadShort()              --数量
    t.is_bind = MsgAdapter.ReadShort()          --是否绑定
    t.item_unique_id = MsgAdapter.ReadLL()      --道具唯一id
    Accrssory.BagParamDecode(t)
    return t
end

function Accrssory.BagParamDecode(tab)
    local t = tab or {}
    t.has_net_value = MsgAdapter.ReadInt()      --是否有净值（不管）
    t.attr_list = Accrssory.DecodeAttrInfo()    --附加属性列表
    t.strength_level = MsgAdapter.ReadInt()     --强化等级
    return t
end

function Accrssory.DecodeAttrInfo()
    local t = {}
    for i = 1, ACCRSSORY_ATTR_MAX do
        local t1 = {}
        t1.attr_type = MsgAdapter.ReadShort()
        t1.attr_value = MsgAdapter.ReadShort()
        table.insert(t, t1)
    end
    return t
end