-- >>>>> 符石
FuShi = FuShi or BaseClass(Item)
function FuShi.DecodeParam()
    local p = {}
    p.has_net_value = MsgAdapter.ReadInt()  -- //!< 是否已随机净值
    p.item_id = MsgAdapter.ReadUShort()
    p.attri_num = MsgAdapter.ReadShort()
    p.attri_list = {}
    for i = 1,p.attri_num do
        local attri = {}
        attri.attri_type = MsgAdapter.ReadShort()
        attri.attri_value = MsgAdapter.ReadShort()
        table.insert(p.attri_list,attri)
    end
    --LOG("PPPPPP",p)
    for i = p.attri_num + 1, 3 do
        MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
    end
    -- 符石给角色的属性，客户端暂不需要，读表显示
    p.role_attri = {}
    p.role_attri.attri_type = MsgAdapter.ReadShort()
    p.role_attri.attri_value = MsgAdapter.ReadShort()
    return p
end

function FuShi:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or -1             -- 背包的index
    self.hole_index = vo.hole_index or -1   -- 孔位的index
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 1
    self.param = vo.param-- or {}
    self.is_grey = vo.is_grey
    self.sheng_qi = vo.sheng_qi
end

function FuShi:Cfg()
    if not self.cfg then
        self.cfg = Cfg.FuShiCfg(self.item_id)
    end
    return self.cfg or {}
end

function FuShi:FuShiType()
    local cfg = self:Cfg()
    return cfg.fushi_type
end

function FuShi:ShengQi()
    return self.sheng_qi
end

function FuShi:Level()
    if not self.level then
        local cfg = self:Cfg()
        self.level = cfg.fushi_level or 1
    end
    return self.level
end

function FuShi:IsMax()
    return self:Level() >= 5
end

function FuShi:TypeDesc()
    local level_desc = string.format("等级：%s", self:Level())
    return {level_desc}
end

function FuShi:LevelDesc()
    local score = self:Score()
    if score == 0 then
        return ""
    end
    return string.format("评分：%s", self:Score())
end

function FuShi:AttriList()
    return self.param and self.param.attri_list or {}
end

function FuShi:AttriShowList()
    local attri_list = self:AttriList()
    local list = {}
    if TableIsEmpty(attri_list) then
        local info = ShengQiData.Instance:FormatAttriInfo()
        info.null_tip = "获得时随机激活1~3条属性"
        info.is_nothing = true
        table.insert(list, info)
    else
        for i,v in pairs(attri_list) do
            local info = ShengQiData.Instance:FormatAttriInfo(v.attri_type, v.attri_value)
            table.insert(list, info)
        end
    end
    return list
end

function FuShi:AttriPreviewList()
    local cfg = self:Cfg()
    if not self.attri_preview_list then
        self.attri_preview_list = {}
        for i = 0, SHENG_QI_ATTRI_TYPE.MAX do
            local info = {}
            local min_filed = SHENG_QI_ATTRI_SCORE_NAME[i] .. "_min"
            local max_filed = SHENG_QI_ATTRI_SCORE_NAME[i] .. "_max"
            info.attri_type = i
            info.min_value = cfg[min_filed]
            info.max_value = cfg[max_filed]
            table.insert(self.attri_preview_list, info)
        end
    end
    return self.attri_preview_list
end

function FuShi:Score()
    local score = 0
    local attri_list = self:AttriList()
    for i,v in pairs(attri_list) do
        local n = ShengQiData.Instance:GetAttriScore(v.attri_type) * v.attri_value
        score  = score + n
    end
    return score
end

-- 对角色增加的属性
function FuShi:RoleAttri()
    local cfg = self:Cfg()
    local role_prof = RoleData.Instance:GetProfession()
    local attri_type = cfg["att_type" .. role_prof]
    local attri_value = cfg["att" .. role_prof]
    local attri = {}
    attri.attri_type = attri_type or 0
    attri.attri_value = attri_value or 0
    return attri
end

-- 一级符石item_id
function FuShi:NormalItemId()
    local fushi_type = self:FuShiType()
    local cfg = Cfg.FuShiCfg2(fushi_type, 1)
    return cfg.item_id
end

-- 下级符石item_id
function FuShi:NextItemId()
    if self:IsMax() then
        return self.item_id
    end
    local fushi_type = self:FuShiType()
    local level = self:Level()
    local cfg = Cfg.FuShiCfg2(fushi_type, level + 1)
    return cfg.item_id
end

-- 获取相当于多少个一级符石
function FuShi:GetPriceStuffNum()
    local level = self:Level()
    local pow_num = level - 1
    return math.pow(5, pow_num)
end

-- 升级需要多少个一级符石
function FuShi:GetUpNeedNormalNum()
    local level = self:Level()
    return 4 * math.pow(5, level - 1)
end


FuShiData = FuShiData or BaseClass()

function FuShiData:__init()
    if FuShiData.Instance ~= nil then
        Debuger.LogError("[FuShiData] attempt to create singleton twice!")
        return
    end
    FuShiData.Instance = self
    self:InitCfg()

    self.fu_shi_list = SmartData.New({})          -- 全部的符石

    self.reset_fushi = SmartData.New()          -- 需要重铸的符石

    self.sel_book_data = SmartData.New({})        -- 图鉴选择的符石

    --self.fusion_fushi = SmartData.New()         -- 合成的符石
    self.sel_fusion_fushi_list = SmartData.New({list = {}})    -- 选择的要合成的符石列表
end

function FuShiData:ClearData()

end

function FuShiData:InitCfg()
    self.item_cfg = Config.fushi_auto
    self.cfg = Config.shengqi_auto

    -- 符石配置
    Cfg.Register("FuShiCfg",Cfg.RealTimeSearch(self.cfg.fushi,"item_id"))
    -- 符石孔配置
    Cfg.Register("FuShiHoleCfg",Cfg.RealTimeSearch(self.cfg.fushi_hole,"hole"))

    -- 通过类型和等级获取符石配置
    Cfg.Register("FuShiCfg2",Cfg.RealTimeSearch2Key(Config.shengqi_auto.fushi,"fushi_type", "fushi_level"))
end

-- 插入/变化单个符石
function FuShiData:FuShiChange(fu_shi)
    if fu_shi.item_id == 0 then
        self.fu_shi_list[fu_shi.index] = nil
        return
    end
    self.fu_shi_list[fu_shi.index] = fu_shi
end

-- 移除符石
function FuShiData:RemoveFuShi(bag_index)
    self.fu_shi_list[bag_index] = nil
end


-- 背包排序整理
function FuShiData:BagSort(fu_shi_list, t_qua)
    fu_shi_list = fu_shi_list or self.fu_shi_list
    table.sort(fu_shi_list, function (a, b)
        local a_qua = a:Color()
        local b_qua = b:Color()
        if a_qua == b_qua then
            local a_score = a:Score()
            local b_score = b:Score()
            if a_score == b_score then
                return a.item_id < b.item_id
            end
            return a_score > b_score
        end
        if a_qua == t_qua then
            return true
        end
        if b_qua == t_qua then
            return false
        end
        return a_qua > b_qua
    end)
end

-- 获取背包显示的符石
function FuShiData:GetBagShowList(t_qua,t_type)
    local show_list = {}
    for i,v in pairs(self.fu_shi_list) do
        local fu_shi_type = v:FuShiType()
        if not t_type or t_type == fu_shi_type or t_type == FU_SHI_TYPE.Common then
            table.insert(show_list, v)
        end
    end
    self:BagSort(show_list,t_qua)
    return show_list
end


-- 是否可解锁，多少级解锁 -1 已解锁，0，不可解锁，等级不够，1可解锁
function FuShiData:FuShiCheckLock(sheng_qi, hole_index, is_check_stuff)
    sheng_qi = sheng_qi or ShengQiData.Instance:GetSelShengQi()
    if sheng_qi:FuShiIsUnlock(hole_index) then
        return -1,0
    end
    local level = sheng_qi:Level()
    local cfg = Cfg.FuShiHoleCfg(hole_index)
    if level >= cfg.shengqi_level then
        if not is_check_stuff then
            return 1,cfg.shengqi_level
        else
            local stuff_id = cfg.unlock_cost
            local need_num = cfg.num
            local has_num = Item.GetNum(stuff_id)
            if has_num >= need_num then
                return 1,cfg.shengqi_level
            else
                return 0,cfg.shengqi_level
            end
        end
    end
    return 0,cfg.shengqi_level
end

-- 需要重铸的符石
function FuShiData:SetResetFushi(fu_shi)
    fu_shi = fu_shi or {}
    self.reset_fushi:Set(fu_shi)
end
function FuShiData:GetResetFushi()
    return self.reset_fushi
end

-- 获取重铸的材料列表
function FuShiData:GetResetStuffList(fushi)
    local level = fushi:Level()
    if level <= 1 then
        return {}
    end
    local list = {}
    for i,v in pairs(self.fu_shi_list) do
        if v.item_id ~= fushi.item_id and v:Level() == level - 1 then
            table.insert(list, v)
        end
    end
    return list
end

-- 重铸需要多少铜币
function FuShiData:GetResetNeedCoin(fushi)
    local level = fushi:Level()
    for i,v in pairs(Config.shengqi_auto.fushi_chongzhu) do
        if level == v.fushi_level then
            return v.cost
        end
    end
    return 0
end

function FuShiData:GetFusionCfg(level)
    for i,v in pairs(Config.shengqi_auto.fushi_compose) do
        if v.fushi_level == level then
            return v
        end
    end
end

-- 合成条件，目标等级和需要的数量
function FuShiData:FusionContion()
    if TableIsEmpty(self.sel_fusion_fushi_list.list) then
        return 0,5
    end
    local first_fushi
    for i,v in pairs(self.sel_fusion_fushi_list.list) do
        first_fushi = v
        break
    end
    local cfg = self:GetFusionCfg(first_fushi:Level())
    return cfg.fushi_level, cfg.need_num
end

-- 合成预览
function FuShiData:GetFusionPreview()
    local list = self.sel_fusion_fushi_list.list
    local t_level, max_num = self:FusionContion()
    if #list < max_num then
        return Item.Create({item_id = 0}),0
    end
    local is_ok = true
    local t_type
    for i,v in pairs(list) do
        if not t_type then
            t_type = v:FuShiType()
        end
        if v:FuShiType() ~= t_type then
            is_ok = false
        end
    end

    local first_fushi = list[1]
    local cfg = self:GetFusionCfg(first_fushi:Level())
    local item_id
    if is_ok then
        local _type = first_fushi:FuShiType()
        item_id = cfg["show" .. _type]
    else
        item_id = cfg["show6"]
    end
    return Item.Create({item_id = item_id}),cfg.cost
end

-- 是否可以合成 1 可以 0 不行
function FuShiData:IsCanFusion(fushi)
    local level = fushi:Level()
    if level == 5 then
        return 0
    end
    local t_level, max_num = self:FusionContion()
    local count = 0
    for i,v in pairs(self.sel_fusion_fushi_list.list) do
        count = count + 1
    end
    if count >= max_num then
        return 0
    end
    if t_level and t_level > 0 and level ~= t_level then
        return 0
    end
    return 1
end

-- 添加合成材料 1 为添加，-1为减去
function FuShiData:AddFusionStuff(fushi, value)
    if value == 1 then
        for i = 1,5 do
            if not self.sel_fusion_fushi_list.list[i] then
                self.sel_fusion_fushi_list.list[i] = fushi
                break
            end
        end
    else
        local key
        for i,v in pairs(self.sel_fusion_fushi_list.list) do
            if v == fushi then
                key = i
                break
            end
        end
        if key then
            self.sel_fusion_fushi_list.list[key] = nil
        end
    end
    self.sel_fusion_fushi_list:SetDirty()
end

function FuShiData:IsInFusion(fushi)
    for i,v in pairs(self.sel_fusion_fushi_list.list) do
        if v == fushi then
            return true
        end
    end
    return false
end

function FuShiData:SetBookSelData(data)
    self.sel_book_data:Set(data)
end

function FuShiData:GetBookSelData(data)
    return self.sel_book_data
end

function FuShiData:GetBookList()
    if not self.book_cfg then
        self.book_cfg = DataHelper.ConvertGroup(self.cfg.fushi, "fushi_type")
    end

    local list_data = {}
    for i,v in pairs(self.book_cfg) do
        local data = {
            type = i,
            list = v,
        }
        table.insert(list_data, data)
    end
    table.sort(list_data, function (a,b)
        return a.type < b.type
    end)
    return list_data
end

-- 单个符石针对圣器的红点
function FuShiData:Remind(fushi,sheng_qi)
    if not TableIsEmpty(sheng_qi) then
        local score = fushi:Score()
        local e_fushi = sheng_qi:GetEqualityFuShi(fushi)
        if e_fushi then
            if score > e_fushi:Score() then
                return 1
            end
        else
            for i,v in pairs(sheng_qi:GetAllFushi()) do
                if v and v.item_id == 0 and sheng_qi:FuShiIsUnlock(i) == true then
                    return 1
                end
                if v and v.item_id > 0 then
                    if score > v:Score() then
                        return 1
                    end
                end
            end
        end
    end
    return 0
end