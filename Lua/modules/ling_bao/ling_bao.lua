LingBao = LingBao or BaseClass(Item)

function LingBao:__init(vo)
    vo = vo or {}
    self.index = vo.index or -1
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 1
    self.param = vo.param or {}
    self.is_grey = vo.is_grey
    self.in_bag = vo.in_bag == nil and true or vo.in_bag
end

function LingBao:TypeDesc()
    return nil
end

-- ItemInfoViewBase用于显示等级
function LingBao:LimitProfDesc()
    return Format(Language.LingBao.Lv2, self:ShowLevel())
end

-- ItemInfoViewBase用于显示阶数
function LingBao:LevelDesc()
    return Format(Language.LingBao.State2, DataHelper.GetDaXie(self:StateLevel()))
end

-- 灵宝部位类型
function LingBao:LingBaoType()
    return self:Config().hole_type
end

function LingBao.GetLingBaoType(item_id)
    return Item.GetConfig(item_id).hole_type
end

-- 灵宝等级
function LingBao:ShowLevel()
    return self:Config().level
end

function LingBao.GetShowLevel(item_id)
    return Item.GetConfig(item_id).level
end

-- 灵宝星级
function LingBao:StarLevel()
    return self:Config().star_level
end

function LingBao.GetStarLevel(item_id)
    return Item.GetConfig(item_id).star_level
end

-- 灵宝阶数
function LingBao:StateLevel()
    return self:Config().class_level
end

function LingBao.GetStateLevel(item_id)
    return Item.GetConfig(item_id).class_level
end

-- 灵宝基础属性
function LingBao:BaseAttr()
    local config = self:Config()
    return LingBaoData.Instance:GetAttrList(config)
end

function LingBao.GetBaseAttr(item_id)
    local config = Item.GetConfig(item_id)
    return LingBaoData.Instance:GetAttrList(config)
end

-- 获得灵宝星星表
function LingBao:StarData()
    local star_data = {}
    for i = 1, self:StarLevel() do
        table.insert(star_data, i)
    end
    return star_data
end

-- 装备对比
function LingBao:Compare()
    if self.in_bag and self.item_id > 0 and self.LingBaoType then
        local shenghen_type = LingBaoData.Instance:MainShengHenType()
        local now_equip = LingBaoData.Instance:EquipList(shenghen_type, self:LingBaoType())
        if not TableIsEmpty(now_equip.item) then
            return LingBao.New(now_equip.item)
        end
    end 
end


function LingBao.DecodeInfo()
    local t = {}
    t.item_id = MsgAdapter.ReadUShort()             --道具id
    t.index = MsgAdapter.ReadShort()                --序号
    t.is_market_buy_flag = MsgAdapter.ReadChar()    --（不管）
    t.num = MsgAdapter.ReadInt()                    --数量
    t.is_bind = MsgAdapter.ReadChar()               --是否绑定
    t.has_param = MsgAdapter.ReadChar()             --是否有净值
    t.invalid_time = MsgAdapter.ReadUInt()          --（不管）
    t.item_unique_id_1 = MsgAdapter.ReadUInt()      --唯一id1
    t.item_unique_id_2 = MsgAdapter.ReadUInt()      --唯一id2
    if t.has_param ~= 0 then 
        t.param_length = MsgAdapter.ReadShort()
        t.param = MsgAdapter.ReadStrN(t.param_length)
    end
    return t
end

function LingBao.DecodeWareInfo()
    local t = {}
    t.shenghen_type = MsgAdapter.ReadShort()           --圣痕类型
    t.lingbao_type = MsgAdapter.ReadShort()            --灵宝类型
    local item = {}
    item.item_id = MsgAdapter.ReadUShort()             --道具id
    item.num = MsgAdapter.ReadShort()                  --数量
    item.is_bind = MsgAdapter.ReadChar()               --是否绑定
    item.has_param = MsgAdapter.ReadChar()             --是否有净值
    item.invalid_time = MsgAdapter.ReadUInt()          --（不管）
    item.in_bag = false
    if item.has_param ~= 0 then 
        item.param_length = MsgAdapter.ReadShort()
        item.param = MsgAdapter.ReadStrN(item.param_length)
    end
    t.item = item
    return t
end

function LingBao.DecodeShengHenInfo(shenghen_type)
    local t = {}
    t.shenghen_type = shenghen_type
    t.is_unlock = MsgAdapter.ReadChar()                 --是否解锁
    t.level = MsgAdapter.ReadShort()                    --觉醒等级
    t.skill_count = MsgAdapter.ReadShort()              --技能数量
    t.skill_level = {}
    for i = 0, t.skill_count - 1 do
        t.skill_level[i] = MsgAdapter.ReadShort()       --技能等级，0：未解锁
    end
    t.lingbao_count = MsgAdapter.ReadShort()            --灵宝数量
    t.lingbao_str_lv = {}
    for i = 0, t.lingbao_count - 1 do
        t.lingbao_str_lv[i] = MsgAdapter.ReadShort() --强化等级
    end
    return t
end