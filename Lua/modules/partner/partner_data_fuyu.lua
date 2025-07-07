function PartnerData:SetFuYuAllInfo(list)
    local temp = {}
    for k, v in pairs(list) do
        temp[v.partner_id] = temp[v.partner_id] or {}
        temp[v.partner_id][v.wear_idx] = v
    end
    for k, v in pairs(temp) do
        if self.partner_list[k] then
            self.partner_list[k]:SetFuYuInfo(v)
        end
    end
end

function PartnerData:SetFuYuInfo(protocol)
    if protocol.reason == 0 then--穿戴符玉
        local info = protocol.info
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
    elseif protocol.reason == 1 then--卸下符玉
        local info = protocol.info
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
    elseif protocol.reason == 2 then--强化成功
        self.qiang_hua_jie_guo = true
        local info = protocol.info
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
        self:OpenFuYuQiangHua(Item.Create(info), true)
        ViewMgr:OpenView(PartnerFuYuResult)
    elseif protocol.reason == 3 then--强化失败
        self.qiang_hua_jie_guo = false
        local info = protocol.info
        -- self.to_be_qiang_hua = Item.Create(info)
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
        self:OpenFuYuQiangHua(Item.Create(info), true)
        ViewMgr:OpenView(PartnerFuYuResult)
    elseif protocol.reason == 4 then--超时特效没了
        local info = protocol.info
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
    elseif protocol.reason == 5 or protocol.reason == 6 then--升华和转换
        local info = protocol.info
        if self.partner_list[info.partner_id] then
            self.partner_list[info.partner_id]:SetFuYuInfo(nil, info)
        end
        local item = Item.Create(info)
        if protocol.reason == 6 then
            MainOtherCtrl.Instance:OpenGiftView({[1]=item}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
            self:ClearFuYuChange()
        elseif  protocol.reason == 5 then
            self:SetSubFuYu(item)
        end
    end
    self.fuyu_flush:Notify()
end
function PartnerData:ClearFuYuChange()
    self:SetChangeFuYu({})
    self:SetChangeFuYuTarget1(nil)
    self:SetChangeFuYuTarget2(nil)
end
function PartnerData:ClearFuYuSub()
    self:SetSubFuYu({})
    self:SetSubFuYuTarget1(nil)
    self:SetSubFuYuTarget2(nil)
end
function PartnerData:FlyUpPartnerList(first_partner)
	local list = {}
    if first_partner then
        table.insert(list, first_partner)
    end
	for _, partner in pairs(self.partner_list) do
		if first_partner == nil or (partner:Id() ~= first_partner:Id()) then
            if partner:IsUnlocked() then             
                table.insert(list, partner)
            end
			-- if partner:SoarCount() > 0 then
			-- end
		end
	end
	return list
end
function PartnerData:FlyUpPartnerListNum()
    local num = 0
    for _, partner in pairs(self.partner_list) do
        -- if partner:SoarCount() > 0 then
        if partner:IsUnlocked() then
            num = num + 1
        end
	end
    return num
end

function PartnerData:GetFuYuBagList()
    local list = BagData.Instance:GetFuYuBagList()
    return list
end
function PartnerData:OpenFuYuQiangHua(item, ignore)
    if self.to_be_qiang_hua then
        if self.to_be_qiang_hua.param.sp_id == 0 and item.param.sp_id ~= 0 then
            self.fuyu_effect_show_new = true
        else
            self.fuyu_effect_show_new = false
        end
    end
    self.to_be_qiang_hua = item
    if not ignore then
        ViewMgr:OpenView(PartnerFuYuQiangHua)
    end
end
function PartnerData:GetQiangHuaItem()
    return self.to_be_qiang_hua
end
function PartnerData:GetQiangHuaMeterals(level)
    -- if self.fuyu_cailiao_list == nil then
        self.fuyu_cailiao_list = {}
        for i, v in ipairs(Config.fuyu_auto.strengthen_fuyu) do
            if v.is_show == 1 and v.materials_level <= level then
                table.insert(self.fuyu_cailiao_list, v)
            end
        end
    -- end
    return self.fuyu_cailiao_list
end
function PartnerData:GetNextLevelItem()
    if self.to_be_qiang_hua then
        local config = Item.GetConfig(self.to_be_qiang_hua.item_id)
        if config then
            local cur_level = config.fuyu_level
            local fuyu_type = config.fuyu_type
            local next_level = cur_level + 1
            local value1 = Cfg.ParnterFuYuByLevel(next_level, fuyu_type)
            value1.item_id = value1.id
            local value2 = Cfg.ParnterFuYuByLevel2(next_level)
            return value1, value2
        end
    end
    return nil
end
function PartnerData:GetSelectSmeltMedal()
    return self.fuyu_decom_opera.select
end 
function PartnerData:GetSelectSmeltMedalDust()
    local dust_num = 0
    local config
    for k,v in pairs(self.fuyu_decom_opera.select) do 
        config = Cfg.PartnerFuYuDecompose(v.item_id, false)
        if config then
            if v:IsGod() then
                dust_num = dust_num + config.resolve_num_3
            elseif v:IsChaos() then
                dust_num = dust_num + config.resolve_num_2
            else
                dust_num = dust_num + config.resolve_num_1
            end
        end
    end 
    return dust_num
end
-- 选择当前选中的勋章
function PartnerData:SetSelectSmeltMedal(item)
    if item == nil then 
        self.fuyu_decom_opera.select = {}
        return 
    end 
    local flag = 0
    for k,v in pairs(self.fuyu_decom_opera.select) do 
        if item.index == v.index then 
            flag = k
            break
        end 
    end 
    if flag == 0 then 
        table.insert(self.fuyu_decom_opera.select,item)
    else 
        table.remove(self.fuyu_decom_opera.select, flag)
    end 
end
--limited = { func_str = "方法名",fun_value = 限制值 }
--noempty = 是否不填充空框
function PartnerData:GetGridForSmeltMedal(paramt)
    local col_grids = BagData.Instance:GetGridByType({ItemType.FuYu})
    local new_list = {}
    for k,v in pairs(col_grids) do 
        if paramt ~= nil and paramt.limited ~= nil then 
            if v[paramt.limited.func_str](v) == paramt.limited.fun_value then
                table.insert(new_list, v)
            end
        else
            table.insert(new_list, v)
        end 
    end 

    if paramt == nil or paramt.noempty == nil then 
        if #new_list < 30 then 
            local plus = 30 - #new_list
            for i = plus ,1, -1 do 
                local item = {item_id = 0}
                table.insert(new_list,item)
            end 
        end 
    end
    return new_list
end

function PartnerData:ProtectItemCfg(level)
    local config = Config.fuyu_auto.protect_item
    return config[level]
end

function PartnerData:CheckMedalSmeltIsClicked(data)
    local flag = false
    for k,v in pairs(self.fuyu_decom_opera.select) do 
        if data.index == v.index then 
            flag = true
            break
        end 
    end 
    return flag
end
--设置升华符玉
function PartnerData:SetSubFuYu(item)
    self.sub_fuyu_item:Set(item)
end
function PartnerData:GetSubFuYu()
    return self.sub_fuyu_item
end

function PartnerData:GetSubFuYuTarget()
    return self.sub_fuyu_target_item
end
--设置升华材料1
function PartnerData:SetSubFuYuTarget1(item)
    self.sub_fuyu_target_item.item1 = item
end
--设置升华材料2
function PartnerData:SetSubFuYuTarget2(item)
    self.sub_fuyu_target_item.item2 = item
end
function PartnerData:GetSubFuYuResult()
    local vo = TableCopy(self.sub_fuyu_item.vo, 2)
    local item = Item.Create(vo)
    if self.sub_fuyu_target_item.item2 ~= nil then
        item.param.extra_sp_id = self.sub_fuyu_target_item.item2:SpId()
    end
    return item
end

--设置需要转换的符玉
function PartnerData:SetChangeFuYu(item)
    self.chang_fuyu_item:Set(item)
end

function PartnerData:GetChangeFuYu()
    return self.chang_fuyu_item
end
function PartnerData:GetChangeFuyuTarget()
    return self.chang_fuyu_target_item
end
--设置转换目标1符玉
function PartnerData:SetChangeFuYuTarget1(item)
    self.chang_fuyu_target_item.item1 = item
end
--设置转换目标2符玉
function PartnerData:SetChangeFuYuTarget2(item)
    self.chang_fuyu_target_item.item2 = item
end
--根据目标返回想要转换的符玉结果
function PartnerData:GetChangeFuYuResult()
    local item1 = self.chang_fuyu_target_item.item1
    local item2 = self.chang_fuyu_target_item.item2
    if item1 == nil then
        return {}
    end
    if item1 and item2 then
        item1.param.extra_sp_id = item2.param.sp_id
    end
    return item1
end

--获取可以进行转换的符玉
function PartnerData:GetFuYuChangeList()
    local list = {}
    local temp 
    for _, partner in pairs(self.partner_list) do
        temp = partner:FuYuItemList()
        for k, v in pairs(temp) do
            if v:SpId() > 0 and not v:IsExpired() then
                table.insert(list, v)
            end
        end
	end
    local bag_list = self:GetFuYuBagList()
    for k, v in pairs(bag_list) do
        if v:SpId() > 0 and not v:IsExpired() then
            table.insert(list, v)
        end
    end
    return list
end
--获取同名天赐符玉
function PartnerData:GetSameNameFuYuItems(item)
    local list = {}
    local bag_list = self:GetFuYuBagList()
    for k, v in pairs(bag_list) do
        if item.in_bag == true then
            if v:SpId() == item:SpId() and not v:IsExpired()
            and v:ExtraId() == 0 and item:GridIndex() ~= v:GridIndex() then
                table.insert(list, v)
            end
        else
            if v:SpId() == item:SpId() and not v:IsExpired() and v:ExtraId() == 0 then
                table.insert(list, v)
            end
        end
    end
    return list
end

function PartnerData:GetSubFuYuItems(item)
    local list = {}
    local effct_cfg = BagData.Instance:GetEquipEffectCfg(item:ExtraId())
    local bag_list = self:GetFuYuBagList()
    local effct_cfg2
    for k, v in pairs(bag_list) do
        if item:IsChaos() then
            effct_cfg2 = BagData.Instance:GetEquipEffectCfg(v:SpId())
            if not v:IsExc() and not v:IsExpired() and v:SpLevel() == 15
            and effct_cfg2.sp_type == effct_cfg.sp_type then
                table.insert(list, v)
            end
        else
            if not v:IsExc() and not v:IsExpired() and v:SpLevel() == 15 then
                table.insert(list, v)
            end 
        end
    end
    return list
end

--转换的符玉的限制
function PartnerData:CheckFuYuCanChange(item)
    if item:SpId() > 0 and not item:IsExpired() then
        return true
    end
    return false
end
--获取可转换天赐特效效果
function PartnerData:GetExcFuYuChangeItems()
    local list = {}
    local item
    for i, v in ipairs(self.partner_fuyu_exc_list) do
        item = self:ExcItem(v.sp_id)
        table.insert(list, item)
    end
    return list
end
--包装展示用的天赐符玉
function PartnerData:ExcItem(sp_id)
    local belong_cfg = Cfg.ParnterFuYuBelongSpId(sp_id)
    local param = {}
    param.sp_id = sp_id
    param.expired_timesstamp = 0
    local attr_cfg = Cfg.ParnterFuYuByLevel2(15)
    param.attr_num = attr_cfg.attri_num
    param.attr_list = {}
    for i = 1, 3, 1 do
        param.attr_list[i] = {
            attr_type = attr_cfg["attri_tpye"..i],
            attr_value = attr_cfg["value_"..i],
        }
    end
    local vo = {}
    vo.item_id = belong_cfg.id
    vo.num = 0
    vo.is_bind = 1
    vo.has_param = true
    vo.param = param
    return Item.Create(vo)
end

--职业列表
function PartnerData:GetProfList()
    local list = {}
    -- 通用
    table.insert(list,{prof = 0})
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
--根据职业显示可以转换的符玉列表
function PartnerData:GetProfFuYuChangeItems(prof, sp_id, sp_level)
    local list = self.partner_fuyu_prof_list[prof] or {}
    local new_list = {}
    local item
    for i, v in ipairs(list) do
        if v.sp_level == sp_level then
            item = self:NormalFuYuItem(v.sp_id)
            table.insert(new_list, item)
        end
    end
    return new_list
end
--包装普通符玉
function PartnerData:NormalFuYuItem(sp_id)
    local eff_cfg = Cfg.ParnterFuYuEffect(sp_id)
    local param = {}
    param.sp_id = sp_id
    param.expired_timesstamp = 0
    local attr_cfg = Cfg.ParnterFuYuByLevel2(eff_cfg.sp_level)
    param.attr_num = attr_cfg.attri_num
    param.attr_list = {}
    for i = 1, 3, 1 do
        param.attr_list[i] = {
            attr_type = attr_cfg["attri_tpye"..i],
            attr_value = attr_cfg["value_"..i],
        }
    end
    local vo = {}
    vo.item_id = attr_cfg.item_id
    vo.num = 0
    vo.is_bind = 1
    vo.has_param = true
    vo.param = param
    return Item.Create(vo)
end
function PartnerData:FuYuConvert()
    return Config.fuyu_auto.other[1].fuyu_convert
end

--============================展示===============================
function PartnerData:GetBagEffectShowParam()
    if self.fuyu_effect_show_cache ~= nil then 
        return self.fuyu_effect_show_cache
    end 

    local data_list = {}   
    
    data_list.title_name = Language.FuYu.BagSpShowTitle
    data_list.tab_list = Language.Conspiracy.TabList
    data_list.empty_lab = Language.Conspiracy.ShowListEmpty
    data_list.is_with = false
    data_list.show_list = {}
    data_list.ignore_gray = true
    local config
    local sp_type = 0
    for i, v in ipairs(Config.fuyu_auto.reward_show) do
        config = BagData.Instance:GetEquipEffectCfg(v.sp_id)
        if config and config.sp_type ~= sp_type then
            sp_type = config.sp_type
            local vo = {}
            vo.sp_id = config.sp_id
            vo.name = config.reward_name
            vo.reward_name = config.reward_name
            vo.tab_index = config.page
            vo.qua = config.sp_priority
            vo.desc = config.reward_description
            vo.icon_type = ICON_TYPE.BUFF
            vo.icon_id = config.icon_id
            vo.sp_type = config.sp_type
            table.insert(data_list.show_list, vo )
        end
    end
    if self.fuyu_effect_show_cache == nil then 
        self.fuyu_effect_show_cache = data_list
    end 
    return data_list
end
function PartnerData:GetFuYuEffectLevelList(sp_type)
    local list = {}
    local config
    for i, v in ipairs(Config.fuyu_auto.reward_show) do
        config = BagData.Instance:GetEquipEffectCfg(v.sp_id)
        if config and config.sp_type == sp_type then
            local vo = {}
            vo.sp_rank = config.sp_rank
            vo.level = Format(Language.FuYu.Jie,config.sp_rank)
            vo.desc = config.reward_description
            vo.name = config.name
            table.insert(list, vo )
        end
    end
    table.sort(list, DataHelper.SortFunc("sp_rank"))
    return list
end
function PartnerData:SetFuYuClickBodyItem(data)
    self.click_body_data = data
end
function PartnerData:GetFuYuClickBodyItem()
    return self.click_body_data
end

FuYuItem = FuYuItem or BaseClass(Item)
function FuYuItem:__init(vo)
    self.vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.wear_idx = vo.wear_idx or -1
    self.partner_id = vo.partner_id or -1
    self.in_bag = self.wear_idx == -1 and self.partner_id == -1
end
function FuYuItem:FuYuLevel()
    return Item.GetConfig(self.item_id).fuyu_level
end
function FuYuItem:Name(shallow, extra_level)
    --根据不同类型返回名字格式
    local name = Item.GetName(self.item_id)
    local sp_name = self:SpName()
    local extra_name = self:ExtraName()
    if not StringIsEmpty(sp_name) then
        if not StringIsEmpty(extra_name) then
            local shallowcolor = shallow and QualityColorStrShallow or QualityColorStr
            local extra_level = extra_level or self:ExtraLevel()
            local color = extra_level == 10 and shallowcolor[ItemColor.Orange] or shallowcolor[ItemColor.Red]
            return string.format("%s(%s&<color=#%s>%s</color>)", name, sp_name, color, extra_name)
        end
        return string.format("%s(%s)", name, sp_name)
    else
        return name
    end
end

function FuYuItem:QuaName(shallow, extra_level)
    local co = Item.GetConfig(self.item_id)
    if co == nil then
        LogError(tostring(self.item_id) .. " 不存在")
        return ""
    end
    local name = self:Name(shallow, extra_level)
    return ColorStr(name, self:ColorStr(shallow))
end

function FuYuItem:Name2(sp_id)
    local sp_id = sp_id or self.param.sp_id
    --[[ if sp_id and sp_id > 0 then
        return Format(Language.FuYu.Name3, self:ColorStr(), DataHelper.GetDaXie(self:FuYuLevel()), self:Name(), self:SpName(sp_id))
    else ]]
        return Format(Language.FuYu.Name2, self:ColorStr(), DataHelper.GetDaXie(self:FuYuLevel()), self:Name())
    --[[ end ]]
end
function FuYuItem:FuYuEffectDesc(sp_id, colorsp)
    local sp_id = sp_id or self.param.sp_id
    if sp_id == 0 then 
        return Language.ItemInfo.NoneEffect
    end
    local config = BagData.Instance:GetEquipEffectCfg(sp_id)
    if config then
        if colorsp == true then
            return ColorStr(config.description, self:ColorStr())
        end
        local desc = config.rank_up_description or ""
        return desc
    else
        LogDG("获取不到特效的配置 sp_id = ", sp_id)
        return sp_id
    end
end
function FuYuItem:SubSpName(name)
    return ColorStr(name, self:ColorStr())
end
function FuYuItem:SubExtraName(name)
    local shallowcolor = QualityColorStr
    return ColorStr(name, self:IsGod() and shallowcolor[ItemColor.Red] or shallowcolor[ItemColor.Orange])
end
function FuYuItem:SpName(sp_id)
    if self.param == nil then
        return ""
    end
    local sp_id = sp_id or self.param.sp_id
    if sp_id and sp_id > 0 then
        local config = BagData.Instance:GetEquipEffectCfg(sp_id)
        return config and config.name or "缺配置"..sp_id
    else
        return ""
    end
end
function FuYuItem:ExtraName()
    if self.param == nil then
        return ""
    end
    local sp_id = self:ExtraId()
    if sp_id and sp_id > 0 then
        local config = BagData.Instance:GetEquipEffectCfg(sp_id)
        return config and config.name or "缺配置"..sp_id
    else
        return ""
    end
end

function FuYuItem:SpId()
    return (self.param.sp_id and self.param.sp_id or 0)
end

function FuYuItem:ExtraId()
    return (self.param.extra_sp_id and self.param.extra_sp_id or 0)
end

function FuYuItem:NextSpId()
    local sp_id = self.param.sp_id or 0
    if sp_id > 0 then
        local config = Cfg.ParnterFuYuEffect(sp_id)
        if config then
            config =  Cfg.ParnterFuYuEffectLevel(config.sp_group, self:FuYuLevel() + 1, false)
            if config then
                return config.sp_id
            else
                return sp_id
            end
        end
        return 0
    end
    return 0
end
function FuYuItem:FuYuType()
    return Item.GetConfig(self.item_id).fuyu_type
end

function FuYuItem:SpGroupId()
    local sp_id = self:SpId()
    local config = Cfg.ParnterFuYuEffect(sp_id)
    if config then
        return config.sp_group
    end
end
function FuYuItem:SpGroupId2()
    local sp_id = self:ExtraId()
    local config = Cfg.ParnterFuYuEffect(sp_id, false)
    if config then
        return config.sp_group
    end
end

function FuYuItem:SpLevel()
    local sp_id = self:SpId()
    local config = Cfg.ParnterFuYuEffect(sp_id, false)
    if config then
        return config.sp_level
    end
    return 0
end
--不是10就是15
function FuYuItem:ExtraLevel()
    local sp_id = self:ExtraId()
    local config = Cfg.ParnterFuYuEffect(sp_id, false)
    if config then
        return config.sp_level
    end
    return 0
end

-- 特效是否生效
function FuYuItem:IsSpOk()
    if self:SpId() == 0 then
        return true
    end
    if not PartnerData.Instance.cur_sel_partner then
        return true
    end

    local data = PartnerData.Instance:GetPartnerInfoById(PartnerData.Instance.cur_sel_partner)
    if not data then
        return true
    end

    local fuyu_list = data:FuYuList()
    local max_sp_level = 0
    local min_wear_idx = 999
    local self_g_id = self:SpGroupId()
    for i = 1, PartnerConfig.FUYU_MAX_NUM do
        local fuyu = fuyu_list[i - 1]
        if fuyu then
            fuyu = Item.Create(fuyu)
            local g_id = fuyu:SpGroupId()       
            if g_id and g_id == self_g_id then
                local sp_level = fuyu:SpLevel()
                if sp_level > max_sp_level then
                    max_sp_level = sp_level
                    min_wear_idx = fuyu.wear_idx
                end
            end
        end
    end

    --LogDG(self:FuYuLevel(), self:SpLevel(), self.wear_idx,max_sp_level,min_wear_idx)

    if self:SpLevel() < max_sp_level then
        return false
    end

    if self:SpLevel() == max_sp_level and self.wear_idx == min_wear_idx then
        return true
    end

    return false 
end

-- 特效2是否生效
function FuYuItem:IsSpOk2()
    if self:ExtraId() == 0 then
        return true
    end
    if not PartnerData.Instance.cur_sel_partner then
        return true
    end

    local data = PartnerData.Instance:GetPartnerInfoById(PartnerData.Instance.cur_sel_partner)
    if not data then
        return true
    end

    local fuyu_list = data:FuYuList()
    local max_sp_level = 0
    local min_wear_idx = 999
    local self_g_id = self:SpGroupId2()
    for i = 1, PartnerConfig.FUYU_MAX_NUM do
        local fuyu = fuyu_list[i - 1]
        if fuyu then
            fuyu = Item.Create(fuyu)
            local g_id = fuyu:SpGroupId()
            if g_id and g_id == self_g_id then
                local sp_level = fuyu:SpLevel()
                if sp_level > max_sp_level then
                    max_sp_level = sp_level
                    min_wear_idx = fuyu.wear_idx
                end
            end
            g_id = fuyu:SpGroupId2()
            if g_id and g_id == self_g_id then
                local sp_level = fuyu:ExtraLevel()
                if sp_level > max_sp_level then
                    max_sp_level = sp_level
                    min_wear_idx = fuyu.wear_idx
                end
            end
        end
    end

    --LogDG(self:FuYuLevel(), self:SpLevel(), self.wear_idx,max_sp_level,min_wear_idx)

    if self:ExtraLevel() < max_sp_level then
        return false
    end

    if self:ExtraLevel() == max_sp_level and self.wear_idx == min_wear_idx then
        return true
    end

    return false 
end

--是否处于计时状态
function FuYuItem:IsExpired()
    return (self.param.expired_timesstamp or 0) > 0
end
--是否天赐
function FuYuItem:IsExc(sp_id)
    local is_exc = false --是否专属符玉
    local sp_id = sp_id or self:SpId()
    if sp_id > 0 then
        local config = Cfg.ParnterFuYuEffect(sp_id)
        if config and config.limit_type == 2 then
            is_exc = true
        end
    end
    return is_exc
end
--是否混元(两个特效一个天赐 一个10阶)
function FuYuItem:IsChaos()
    return self:ExtraLevel() == 10
end
--是否化神(两个特效一个天赐 一个15阶)
function FuYuItem:IsGod()
    return self:ExtraLevel() == 15
end
function FuYuItem:FuYuTypeName()
    return Language.FuYu.SpTypeName[self:FuYuJieType()]
end
function FuYuItem:FuYuJieType()
    if self:IsGod() then
        return 3
    end
    if self:IsChaos() then
        return 2
    end
    if self:IsExc() then
        return 1
    end
    return 0
end
FuYuCellPlus = FuYuCellPlus or DeclareMono("FuYuCellPlus", UIWFlushPanel)
function FuYuCellPlus:FuYuCellPlus()
    self.exc_sp = {
        "rank_n", "rank_n1", "rank_n2"
    }
end
function FuYuCellPlus:SetData(data)
    UH.SpriteName(self.RankSp, PartnerConfig.FuYuQuaPic[data:Color()])
    local is_exc = false
    if data.param then
        is_exc = data:IsExc()
        UH.SpriteName(self.ExcSp, self.exc_sp[data:FuYuJieType()])    
    end
    self.Exc:SetActive(is_exc)
    self.Rank:SetActive(not is_exc)
    UH.SetText(self.RankTex, "+" .. data:FuYuLevel())
    if data.param then
        local sp_id = data:SpId()
        local config = Cfg.ParnterFuYuEffect(sp_id, false)
        if config and config.limit_type == 1 then
            self.ProfSp:SetObjActive(true)
            UH.SpriteName(self.ProfSp, PartnerConfig.FuYuProfSp[config.param1].."1")
        else
            self.ProfSp:SetObjActive(false)
        end
    else
        self.ProfSp:SetObjActive(false)
    end
    if data.wear_idx == -1 and data.partner_id == -1 then
        self.Up:SetActive(PartnerData.Instance:PartnerFuYuUp())
    else
        self.Up:SetActive(false)
    end
    if data.param and data.param.expired_timesstamp then
        self.Time:SetActive(data.param.expired_timesstamp > 0 and not self.Up.activeSelf)
    else
        self.Time:SetActive(false)
    end
end