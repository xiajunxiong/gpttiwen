IllustrateData = IllustrateData or BaseClass()

function IllustrateData:__init()
    if IllustrateData.Instance ~= nil then
        Debuger.LogError("[IllustrateData] attempt to create singleton twice!")
        return
    end
    IllustrateData.Instance = self
    self.config = Config.collection_auto
    self.other = self.config.other[1]
    self:Initialization()
    self:FormatCollectConfig()
    self:InitCollectData()
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    BindTool.Bind(self.CheckIllustrateData, self))
    self.profession_handle = EventSys:Bind(GlobalEventKeys.RoleProfessionChange,
    BindTool.Bind(self.CheckIllustrateData, self))

    self.rare_data = SmartData.New({info = {}, remind = true})
    Cfg.Register("IllustrateRareCard", Cfg.RealTimeSearch(self.config.treasures_page, "seq"))
    Cfg.Register("IllustrateRareCardById", Cfg.RealTimeSearch(self.config.treasures_page, "id"))
    Cfg.Register("IllustrateRareCardLv", Cfg.RealTimeSearch2Key(self.config.treasures, "seq", "grade"))
    Cfg.Register("IllustrateCollectById", Cfg.RealTimeSearch(self.config.collect, "id"))
end

function IllustrateData:__delete()
    IllustrateData.Instance = nil
    EventSys:UnBind(self.complete_handle)
    EventSys:UnBind(self.profession_handle)
end

function IllustrateData:ClearData()
    if self.active_data then
        self.active_data = SmartData.New({list = {}})
    end
    if self.active_change then
        self.active_change = SmartData.New({flag = true})
    end
    if self.included_data then
        self.included_data = SmartData.New({can_active = {},has_active = {}})
    end
    self.included_data.has_active = {}
    self.included_data.can_active = {}

    for i=1,IllustrateConfig.MAX_PAGE_NUM do
        self.included_list[i] = {}
    end

    self.rare_data.info = {}
end

function IllustrateData:CheckIllustrateData()
    self.page_list = {}
    local profession = RoleData.Instance:GetRoleProf()
    for k,v in pairs(Config.collection_auto.collection_jihuo) do
        if self.page_list[v.page] == nil then
            self.page_list[v.page] = {}
        end
        if v.limit_prof == 0 or v.limit_prof == profession then
            v.item_list = self:GetActiveItemList(v)
            v.attr_data = DataHelper.FormAttrList(v)
            v.attr_list = self:GetAttrItemList(v)
            table.insert(self.page_list[v.page],v)
        end
    end
    for i=1,#self.page_list do
        table.sort(self.page_list[i],function(a,b)
            if a.color == b.color then
                return a.collection_sort < b.collection_sort
            else
                return a.color < b.color
            end
        end)
    end
    -- 这里图鉴卡册的数据单独处理
    self.illustrate_card_data = self:FormatCardConfig(self.page_list[7])
    self.active_change = SmartData.New({flag = true})
    self.active_data = SmartData.New({list = {}})
    self.page_red_piont = SmartData.New({list = {}})
    self:InitCollectGroupData()
end

-- 返回图鉴卡册的数据
function IllustrateData:GetIllustrateCardData()
    return self.illustrate_card_data
end

-- 处理图鉴卡册数据
function IllustrateData:FormatCardConfig(list)
    local card_list = {}
    for i, v in ipairs(list) do
        if not card_list[v.color - 1] then
            card_list[v.color - 1] = {}
            card_list[v.color - 1].color = v.color
            card_list[v.color - 1].item_list = {}
        end
        table.insert(card_list[v.color - 1].item_list, v)
    end
    return card_list
end

function IllustrateData:Initialization()
    self.select_data = SmartData.New({type = 7,page = 1,seq = -1})
end

function IllustrateData:SetSelectType(type)
	self.select_data.type = type
end

function IllustrateData:GetSelectType()
	return self.select_data.type
end

function IllustrateData:GetActiveItemList(data)
    local list,key = {},"collection_item"
    for i=1,data.fee_num do
        table.insert(list,{num = data[key.."num_"..i],item_id = data[key.."fee_"..i],have=0})
    end
    return list
end

function IllustrateData:GetAttrItemList(data)
    local list = {}
    for i,v in ipairs(DataHelper.FormAttrList(data)) do
        local attr_name = v.attr_name .. "+" .. v.attr_value
        table.insert(list,{color = data.color,name = attr_name})
    end
    return list
end

function IllustrateData:GetPageList(page)
    return self.page_list[page or self:GetSelectType()] or {}
end

--服务端下发
function IllustrateData:SetCollectionAllInfo(protocol)
    self.active_data.list = protocol.active_info
end

function IllustrateData:SetCollectionSingleInfo(protocol)
    self.active_data.list[protocol.seq] = protocol.is_active
    self.active_change.flag = not self.active_change.flag
end

function IllustrateData:GetActiveSeq(seq)
    if self.active_data.list[seq] then
        return self.active_data.list[seq] == 1
    end
    return false
end

function IllustrateData:GetCollectionName(collection_sep)
	for k,v in pairs(Config.collection_auto.collection_jihuo) do
		if v.back_card == 1 and v.collection_sep == collection_sep then
			return v.collection_name
		end
	end
end

--激活加成属性
function IllustrateData:GetActiveAttrData()
    local attr_data = {}
    for k,v in pairs(self.active_data.list) do
        if v == 1 then
            local config = Config.collection_auto.collection_jihuo[k+1] or {}
            attr_data = DataHelper.AttrListAdd(attr_data,config,true)
        end
    end
    return attr_data
end

-- 获取list中的总属性
function IllustrateData:GetActiveAttrDataByList(list)
    local attr_data = {}
    for k, v in pairs(list) do
        if self:GetActiveSeq(v.seq) then
            local config = Config.collection_auto.collection_jihuo[v.seq + 1] or {}
            attr_data = DataHelper.AttrListAdd(attr_data, config, true)
        end
    end
    return attr_data
end

function IllustrateData:GetAttrTypeData(attr_data,type_list)
    local list = {}
    for i,v in ipairs(type_list) do
        local attr_type = AttrTypeForName[v]
        table.insert(list, {
            attr_type = attr_type,
            attr_name = Language.Common.AttrList[attr_type],
            attr_value = attr_data[v] or 0
        })
    end
    return list
end

--红点
function IllustrateData:GetRemindNum()
    if self.page_red_piont == nil then
        return 0
    end
    local list,num = {},0
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.page_list or {}) do
        if role_level >= self:GetcollectionPageLevel(k) then
            list[k] = self:CheckPageRedNum(v)
            num = num + list[k]
        end
    end
    self.page_red_piont.list = list
    return num
end

function IllustrateData:CheckPageRedNum(page)
    for k,v in pairs(page) do
        if not self:GetActiveSeq(v.seq) then
            if self:CheckItemRedNum(v.item_list) then
                return 1
            end
        end
    end
    return 0
end

function IllustrateData:GetListActiveNum(list)
    local num = 0
    for k, v in pairs(list) do
        if self:GetActiveSeq(v.seq) then
            num = num + 1
        end
    end
    return num
end

function IllustrateData:CheckItemRedNum(item_list)
    for k,v in pairs(item_list) do
        if Item.GetNum(v.item_id) < v.num then
            return false
        end
    end
    return true
end

function IllustrateData:GetPageRedNum(page)
    if self.page_red_piont.list[page] then
        return self.page_red_piont.list[page]
    end
    return 0
end

function IllustrateData:GetcollectionPageLevel(page)
    local config = Config.collection_auto.collection_page[page]
    return config and config.level or 0
end

function IllustrateData:GetcollectionPageConfig(page)
    return Config.collection_auto.collection_page[page] or {}
end

-----------------收录系统-------------------------
function IllustrateData:InitCollectData()
    self.collect_page = {}
    for k,v in pairs(Config.collection_auto.collect) do
        if self.collect_page[v.page] == nil then
            self.collect_page[v.page] = {}
        end
        if self.collect_page[v.page][v.color] == nil then
            self.collect_page[v.page][v.color] = {}
        end
        v.attr_list = self:GetAttrItemList(v)
        if v.is_show == 1 then
            table.insert(self.collect_page[v.page][v.color],v)
        end
    end
    self.included_list = {}  --总属性
    self.included_page = {}
    for i=1,IllustrateConfig.MAX_PAGE_NUM do
        local list,num = {},0
        for k,v in pairs(self.collect_page[i]) do
            table.insert(list,{item_list = v,color = k,page = i})
            num = num + #v
        end
        self.included_page[i] = {page_list = list,target_num = num}
        self.included_list[i] = {}
    end

    self.included_data = SmartData.New({can_active = {},has_active = {}})
    self.included_red_piont = SmartData.New({list = {}})
end

--处理下配置， 策划要自定义的品质
function IllustrateData:FormatCollectConfig()
    for k,v in pairs(Config.collection_auto.collect) do
        if v.page == 1 then
            v.color = IllustrateConfig.ChangeList[v.color]
        end
    end
end

function IllustrateData:SetSelectPage(page)
	self.select_data.page = page
end

function IllustrateData:GetSelectPage()
	return self.select_data.page
end

function IllustrateData:GetCollectPageList(page)
    return self.included_page[page or self:GetSelectPage()]
end

function IllustrateData:GetIncludedItem(page,id)
    if IllustrateConfig.INCLUDED_ITEM_FUNC[page] then
        return IllustrateConfig.INCLUDED_ITEM_FUNC[page](id)
    end
end

function IllustrateData:SetCollectionIncludedInfo(protocol)
    self.included_data.can_active = protocol.can_active
    self.included_data.has_active = protocol.has_active
    self:CheckIncludedList()
    self:CheckIncludedRedNum()
end

function IllustrateData:SetCollectionIncludedSingle(protocol)
    self.included_data.can_active[protocol.seq] = protocol.can_active
    self.included_data.has_active[protocol.seq] = protocol.has_active
    if protocol.has_active == 1 then
        self:AddIncludedList(protocol.seq)
    end
    self:CheckIncludedRedNum()
end

--收录可激活
function IllustrateData:GetActiveCanFalg(seq)
    return self.included_data.can_active[seq] or 0
end

function IllustrateData:GetActiveHasFalg(seq)
    return self.included_data.has_active[seq] or 0
end

function IllustrateData:GetPageActiveNum(list)
    local num = 0
    for k,v in pairs(list) do
        num = num + self:GetActiveHasFalg(v.seq)
    end
    return num
end

--已激活属性
function IllustrateData:CheckIncludedList()
    for k,v in pairs(Config.collection_auto.collect) do
        if self:GetActiveHasFalg(v.seq) == 1 then
            self.included_list[v.page][v.seq] = v
        end
    end
end

function IllustrateData:AddIncludedList(seq)
    local config = Config.collection_auto.collect[seq]
    if config and self:GetActiveHasFalg(seq) == 1 then
        self.included_list[config.page][seq] = config
    end
end

--当前页激活数量
function IllustrateData:GetIncludePageNum(page)
    local num = 0
    for k,v in pairs(self.included_list[page] or {}) do
        num = num + 1
    end
    return num
end

--当前页激活属性
function IllustrateData:GetIncludedAttrData(page)
    local attr_data = {}
    for k,v in pairs(self.included_list[page]) do
        attr_data = DataHelper.AttrListAdd(attr_data,v,true)
    end
    attr_data = self:GetAttrTypeData(attr_data,IllustrateConfig.BaseType)
    local attr_list = {}
    for i,v in ipairs(attr_data) do
        if v.attr_value > 0 then
            table.insert(attr_list,v)
        end
    end
    return attr_list
end

--收录红点
function IllustrateData:CheckIncludedRedNum()
    local list = {}
    for k,v in pairs(Config.collection_auto.collect) do
        if not list[v.page] then
            if v.is_show == 1 and self:GetActiveHasFalg(v.seq) == 0 and self:GetActiveCanFalg(v.seq) == 1 then
                list[v.page] = true
            end
        end
    end
    self.included_red_piont.list = list
end

function IllustrateData:GetIncludedPageRedNum(page)
    return self.included_red_piont.list[page] and 1 or 0
end

function IllustrateData:GetIncludedRedNum()
    for k,v in pairs(self.included_red_piont.list) do
        if v then
            return 1
        end
    end
    return 0
end

function IllustrateData:GetCollectPageData()
    local list = Config.collection_auto.collect_page
    table.sort(list,DataHelper.SortFunc([[range]]))
    return list
end

--卡片组
function IllustrateData:InitCollectGroupData()
    self.collect_group_list = {}
    local key = "collection_item"
    local profession = RoleData.Instance:GetRoleProf()
    for k,v in pairs(Config.collection_auto.collection_jihuo) do
        if v.limit_prof == 0 or v.limit_prof == profession then
            for i=1,v.fee_num do
                local item_id = v[key.."fee_"..i]
                if self.collect_group_list[item_id] == nil then
                    self.collect_group_list[item_id] = {}
                end
                table.insert(self.collect_group_list[item_id],{
                    seq = v.seq,
                    page = v.page,
                    color = v.color,
                    item_id = item_id,
                    name = v.collection_name,
                    need_num = v[key.."num_"..i],
                    page_name = self:GetcollectionPageConfig(v.page).page_name,
                })
            end
        end
    end
end

--通过item_id 获取对应的卡片组
function IllustrateData:GetCollectGroupData(item_id)
    local group_list = self.collect_group_list[item_id] or {}
    if #group_list > 1 then
        table.sort(group_list,function(a,b)
            if a.page == b.page then
                return a.color < b.color
            else
                return a.page < b.page
            end
        end)
    end
    return group_list
end

function IllustrateData:GetCollectConfig(seq)
    return Config.collection_auto.collection_jihuo[seq + 1]
end

function IllustrateData:SetSelectSeq(seq)
	self.select_data.seq = seq or -1
end

function IllustrateData:GetSelectSeq()
	return self.select_data.seq
end

function IllustrateData:SetSelectFunc(func)
    self.select_func = func
end

function IllustrateData:SelectFunc(page)
    if self.select_func then
        self.select_func(page)
    end
end

function IllustrateData:GetBtnList()
    local list = Config.collection_auto.collection_page
    table.sort(list,function(a,b)
        return a.range < b.range
    end)
    return list
end

function IllustrateData:GetCardHight(seq)
    local hight = 0
    local function GetItemHight(item_list,seq)
        for k,v in pairs(item_list) do
            if v.seq == seq then
                return math.floor(k / 6) * 236
            end
        end
    end
    for i,v in ipairs(self:GetIllustrateCardData()) do
        local item_hight = GetItemHight(v.item_list,seq)
        if item_hight ~= nil then
            return hight + item_hight
        else
            hight = hight + math.ceil(#v.item_list / 6) * 236 + 30
        end
    end
end

--[变强]激活X组图鉴
function IllustrateData:GetBianQiangIllustrateActived()
    local max_num = 0
    local match_num = 0
    for k, list in pairs(self.page_list) do
        max_num = max_num + #list
        for i, v in pairs(list) do
            if self:GetActiveSeq(v.seq) then
                match_num = match_num + 1
            end
        end
    end
    return max_num, match_num
end

-- 珍品数据
function IllustrateData:GetRareInfo(seq)
    return self.rare_data.info[seq] or 0
end

function IllustrateData:SetRareInfo(protocol)
    self.rare_data.info[protocol.seq] = protocol.level
    self.rare_data:SetDirty("info")
end

function IllustrateData:SetRareAllInfo(protocol)
    self.rare_data.info = protocol.info
    self.rare_data:SetDirty("info")
end

function IllustrateData:RareRemind()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Illustrate.Rare) then
		return 0
    end
    for k, v in pairs(self.config.treasures_page) do
        if self:RareIsCanLvUp(v.seq) then
            return 1
        end
    end
    return 0
end

-- 珍品图鉴卡是否可以升级
function IllustrateData:RareIsCanLvUp(seq)
    local lv = self:GetRareInfo(seq)
    if lv >= self:RareMaxLevel() then
        -- 等级上限
        return false, 1
    end
    local next_lv = lv + 1
    local cfg = Cfg.IllustrateRareCardLv(seq, next_lv, false)
    if TableIsEmpty(cfg) then
        -- 等级上限
        return false, 1
    else
        local has = Item.GetNum(cfg.id)
        if has < cfg.consume_quantity then
            -- 道具不足
            return false, 2
        end
    end
    return true
end

-- 用于红点的道具监听列表
function IllustrateData:RemindRareItemList()
    local rare_item_list = {}
    for k, v in pairs(self.config.treasures_page) do
        table.insert(rare_item_list, v.id)
    end
    return rare_item_list
end

-- 藏品卡最大等级
function IllustrateData:RareMaxLevel()
    return self.other.treasures_level
end

-- 返回当前需要消耗多少张卡升级
function IllustrateData:RareNeedCard(seq)
    local lv = self:GetRareInfo(seq)
    local next_lv = lv + 1
    local cfg = Cfg.IllustrateRareCardLv(seq, next_lv, false)
    if TableIsEmpty(cfg) then
        return 0
    else
        return cfg.consume_quantity
    end
end

-- 获得某一行数据的属性列表，数据，是否清空值（seq，等级，用于0级的属性显示拿1级的置空，是否用属性类型作为key）
function IllustrateData:GetRareAttrBySeq(seq, lv, is_zero, is_use_type_key)
    lv = lv or self:GetRareInfo(seq)
    local cfg = Cfg.IllustrateRareCardLv(seq, lv, false)
    return self:GetRareAttrByCfg(cfg, is_zero, is_use_type_key) or {}
end

-- 获得某一行数据的属性列表，数据，是否清空值（属性表，用于0级的属性显示拿1级的置空，是否用属性类型作为key）
function IllustrateData:GetRareAttrByCfg(config, is_zero, is_use_type_key)
    local index = 1
    local t = {}
    if TableIsEmpty(config) then
        return t
    end
    while config["attr_value_"..index] and config["attr_value_"..index] > 0 do
        local t1 = CommonStruct.AttrStruct(config["attr_type_"..index], config["attr_value_"..index])
        if is_zero then
            t1 = CommonStruct.AttrStruct(config["attr_type_"..index], 0)
        end
        if is_use_type_key then
            t[t1.attr_type] = t1
        else
            table.insert(t, t1)
        end
        index = index + 1
    end
    return t
end

-- 获取当前珍品中的所有属性（是否需要属性值为0的属性）
function IllustrateData:GetRareAllAttr(is_zero)
    if not self.rare_all_zero_attr then
        self.rare_all_zero_attr = {}
        for k, v in pairs(self.config.treasures_page) do
            local attr = self:GetRareAttrBySeq(v.seq, self:RareMaxLevel())
            for l, w in pairs(attr) do
                if not self.rare_all_zero_attr[w.attr_type] then
                    self.rare_all_zero_attr[w.attr_type] = CommonStruct.AttrStruct(w.attr_type, 0)
                end
            end
        end
    end
    local zero_list = TableCopy(self.rare_all_zero_attr, 2)
    for k, v in pairs(self.config.treasures_page) do
        local lv = self:GetRareInfo(v.seq)
        local attr = self:GetRareAttrBySeq(v.seq, lv, nil, true)
        if not TableIsEmpty(attr) then
            for l, w in pairs(attr) do
                zero_list[w.attr_type] = CommonStruct.AttrAdd(w.attr_type, w.attr_value, zero_list[w.attr_type].attr_value)
            end
        end
    end
    if not is_zero then
        local new_attr = {}
        for k, v in pairs(zero_list) do
            if v.attr_value > 0 then
                table.insert(new_attr, v)
            end
        end
        return new_attr
    end
    return zero_list
end

-- 根据卡片数据计算评分
function IllustrateData:RareScore(seq, lv)
    local score = 0
    local attr = self:GetRareAttrBySeq(seq, lv)
    for k, v in pairs(attr) do
        score = score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
    end
    return math.floor(score)
end

-- 返回珍品图片路径
function IllustrateData:RareIconPath(config)
	return Format("ui/textures/illustrate_rare/%s.png", config.icon_id)
end

function IllustrateData:GetRareListData()
    if not self.rare_list_data then
        self.rare_list_data = {}
        for i, v in ipairs(self.config.treasures_page) do
            if not self.rare_list_data[v.group_id] then
                self.rare_list_data[v.group_id] = {}
                self.rare_list_data[v.group_id].list = {}
                self.rare_list_data[v.group_id].color = Item.GetColor(v.id)
            end
            table.insert(self.rare_list_data[v.group_id].list, v)
        end
    end
    return self.rare_list_data
end

-- 返回已激活数量和总共数量
function IllustrateData:GeRareGroupNum(config)
    local num1, num2 = 0, 0
    for k, v in pairs(config) do
        if self:GetRareInfo(v.seq) > 0 then
            num1 = num1 + 1
        end
        num2 = num2 + 1
    end
    return num1, num2
end

-- 获取珍品对应卡的等级属性描述
function IllustrateData:GetAllLvRareAttr(item_id)
    local list = {}
    local cfg = Cfg.IllustrateRareCardById(item_id)
    if not TableIsEmpty(cfg) then
        for i = 1, self:RareMaxLevel() do
            local cfg2 = Cfg.IllustrateRareCardLv(cfg.seq, i)
            if not TableIsEmpty(cfg2) then
                local t = {}
                t.level_text = Format(Language.Illustrate.Rare.Lv2, i)
                t.desc = cfg2.describe or ""
                table.insert(list, t)
            end
        end
    end
    return list
end