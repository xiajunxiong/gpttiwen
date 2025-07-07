HeartSkillData = HeartSkillData or BaseClass()

function HeartSkillData:__init()
    if HeartSkillData.Instance ~= nil then
        Debuger.LogError("[HeartSkillData] attempt to create singleton twice!")
        return
    end
    HeartSkillData.Instance = self
    self:InitHeartSkillData()
    self.equipes_info = SmartData.New({info = {},unlock_flag = {}})
    self.initiat_info = SmartData.New({info = {},val = false})
    self.passive_info = SmartData.New({info = {},val = false})
    self.red_point_data = SmartData.New({val = false})
end

function HeartSkillData:InitHeartSkillData()
    self.config = Config.heart_skill_cfg_auto
    self.skill_list = {}
    for k,v in pairs(self.config.heart_skill_base) do
        if self.skill_list[v.heart_skill_type] == nil then
            self.skill_list[v.heart_skill_type] = {}
        end
        table.insert(self.skill_list[v.heart_skill_type],v)
    end
    self.upgrade_config = {}
    for k,v in pairs(self.config.heart_skill_level) do
        if self.upgrade_config[v.heart_skill_seq] == nil then
            self.upgrade_config[v.heart_skill_seq] = {}
        end
        self.upgrade_config[v.heart_skill_seq][v.skill_level] = v
    end
    for type,skill_list in pairs(self.skill_list) do
        for k,v in pairs(skill_list) do
            v.color = Item.GetColor(v.item_id)
        end
    end
    self.skill_config = KeyList(self.config.heart_skill_base,"heart_skill_seq")

    self.resonance_level_config = {}
    for i,v in ipairs(self.config.resonance_level) do
        if self.resonance_level_config[v.resonance_group] == nil then
            self.resonance_level_config[v.resonance_group] = {}
        end
        table.insert(self.resonance_level_config[v.resonance_group],v)
    end
    self.resonance_config = {}
    for i,v in ipairs(self.config.heart_resonance_group) do
        if self.resonance_config[v.resonance_group] == nil then
            self.resonance_config[v.resonance_group] = {title = v.resonance_name,item_list = {},
            resonance_group = v.resonance_group,attr_list = self.resonance_level_config[v.resonance_group],
            max_group = self.config.heart_resonance_group[#self.config.heart_resonance_group].resonance_group
        }
        end
        table.insert(self.resonance_config[v.resonance_group].item_list,self.skill_config[v.heart_skill_seq])
    end
    self.change_config = {}
    local config = Config.heart_skill_cfg_auto.other[1]
    for i=1,3 do
        local list = {}
        for i,v in ipairs(DataHelper.SplitList(config["switch_group_"..i],"|")) do
            table.insert(list,{item_id = v})
        end
        self.change_config[config["switch_comsums_"..i]] = list
    end
    self.heart_attr_config = TabGroup(Config.heart_skill_cfg_auto.heart_attr,[[limit_prof]],[[heart_skill_seq]],[[heart_level]])
end

--获取心法技能列表
function HeartSkillData:GetSkillList(type)
    local list = self.skill_list[type]
    table.sort(list,function(a,b)
        local a_weight = a.heart_skill_seq
        local b_weight = b.heart_skill_seq
        if a.color == b.color then
            if self:GetSkillActive(a) then
                a_weight = a_weight - 1000
            end
            if self:GetSkillActive(b) then
                b_weight = b_weight - 1000
            end
            return a_weight < b_weight
        end
        return a.color > b.color
    end)
    return list
end

--获取心法共鸣列表
function HeartSkillData:GetResonanceList()
    return self.resonance_config
end

function HeartSkillData:GetSkillItemLevel(data)
    return self:GetSkillInfo(data).level or 0
end

function HeartSkillData:GetItemRemindFlag(data)
    if RoleData.Instance:GetRoleLevel() >= data.deblocking_level and self:GetEquipSkill(data.hole_seq) == -1 then
        if self:GetUnlockFlag(data.hole_seq) == 0 then
            return true
        else
            if table.nums(self:GetSkillInfoList(data.heart_skill_type)) ~= 0 then
                return true
            end
        end
    end
    return false
end

function HeartSkillData:GetRemindNum()
    if not FunOpen.Instance:IsFunOpen(Mod.Role.RoleHeartSkill) then
        return 0
    end
    for i,v in ipairs(self.config.heart_skill_hole) do
        if self:GetItemRemindFlag(v) then
            return 1
        end
    end
    for k,v in pairs(self.config.heart_skill_base) do
        if self:GetRemindItemNum(v) then
            return 1
        end
    end
    return 0
end

function HeartSkillData:GetRemindItemNum(data)
    local level = self:GetSkillItemLevel(data)
    if level == data.limit_max_level then
        return false
    end
    local config = self:GetUpgradeConfig(data.heart_skill_seq,level + 1)
    return self:IsHasItemConsume(config or {})
end

function HeartSkillData:GetRemindListNum(list)
    for k,v in pairs(list or {}) do
        if self:GetRemindItemNum(v) then
            return 1
        end
    end
    return 0
end

function HeartSkillData:GetSkillInfoList(type)
    local list = {}
    local function IsHasIndex(tab,index)
        for k,v in pairs(tab) do
            if self:GetEquipSkill(v) ~= -1 and self:GetEquipSkill(v) == index then
                return true
            end
        end
        return false
    end
    if type == 1 then
        for k,v in pairs(self.initiat_info.info) do
            if not IsHasIndex({0,5},v.index) then
                table.insert(list,v)
            end
            if #list >= 1 then
                return list
            end
        end
    else
        for k,v in pairs(self.passive_info.info) do
            if not IsHasIndex({1,2,3,4},v.index) then
                table.insert(list,v)
            end
            if #list >= 1 then
                return list
            end
        end
    end 
    return list
end

function HeartSkillData:IsHasItemConsume(data)
    local item_consume = data.item_consume[0] or {}
    local item_id = item_consume.item_id or 0
    return Item.GetNum(item_id) >= (item_consume.num or 0)
end

function HeartSkillData:GetItemConsume(data)
    local item_consume = data.item_consume[0] or {}
    local item_id,num = (item_consume.item_id or 0),(item_consume.num or 0)
    return Item.Init(item_id),Item.GetNum(item_id),num
end

function HeartSkillData:GetSkillName(data,shallow)
    local skill_name = SkillData.Instance:GetSkillDescName(data.heart_skill_id)
    return skill_name ~= "" and ColorStr(skill_name,shallow and QualityColorStrShallow[data.color] or QualityColorStr[data.color]) or Item.GetQuaName(data.item_id,shallow)
end

function HeartSkillData:GetSkillSeq(type,index)
    local info = DataHelper.GetSearch(type == 1 and self.initiat_info.info or self.passive_info.info,[[index]],index)
    if info then
        return info.seq or 0,info.level or 0
    end
    return 0,1
end

function HeartSkillData:GetClickDesc(data)
    local desc = ""
    for i=1,data.limit_max_level do
        local attr_config = self:GetAttrConfig(data.heart_skill_seq,i)
        desc = desc .. Format(Language.HeartSkill.LevelTitle,i)
        desc = desc .. Format(Language.HeartSkill.Description,self:GetAttrDesc(attr_config))
        local config = SkillData.GetDecConfig(data.heart_skill_id,i)
        desc = desc .. config.description .. (i == data.limit_max_level and "" or "\n")
    end
    return desc
end

function HeartSkillData:GetAttrList(data,is_main)
    local list = {}
    for i=1,4 do
        local type,value = data["attr_type_" .. i],data["attr_num_"..i]
        if value ~= 0 then
            table.insert(list,{
                is_main = is_main,
                attr_type = type,
                attr_name = Language.Common.AttrList[type],
                attr_value = self:Percent(type,value)
            })
        end
    end
    return list
end

function HeartSkillData:Percent(attr_type,attr_value)
    if DataHelper.IsPercent(attr_type) then
        return Format("%.1f%%", attr_value / 10)
    end
    return attr_value
end

function HeartSkillData:GetResonateMinLevel(item_list)
    local resonance_level
    for i,v in ipairs(item_list) do
        local level = HeartSkillData.Instance:GetSkillLevel(v)
        if resonance_level == nil or (resonance_level and level <= resonance_level) then
            resonance_level = level
        end
    end
    return resonance_level or 1
end

function HeartSkillData:IsResonateActive(item_list)
    for k,v in pairs(item_list) do
        if not HeartSkillData.Instance:GetSkillActive(v) then
            return true
        end
    end
    return false
end

function HeartSkillData:GetResonateMain(data_list,level,is_active)
    for k,v in pairs(data_list) do
        v.is_active = is_active
        if v.resonance_level == level then
            v.is_main = true
        end
    end
    return data_list
end

function HeartSkillData:GetTargetData(heart_skill_color)
    for k,v in pairs(self.config.heart_skill_displace) do
        if v.heart_skill_color == heart_skill_color then
            return v
        end
    end
end

--是否是心法消耗道具
function HeartSkillData:GetConsumeItemData(item_id)
    return CacheList(self,[[consume_data]],item_id,function()
        for k,v in pairs(self.config.heart_skill_level) do
            if v.item_consume[0].item_id == item_id then
                local list = {}
                local config = self.skill_config[v.heart_skill_seq] or {}
                for i=1,config.limit_max_level or 0 do
                    local attr_config = self:GetAttrConfig(v.heart_skill_seq,i)
                    list[i] = {
                        equip_level = i,
                        skill_cfg = SkillData.GetDecConfig(config.heart_skill_id,i),
                        attr_desc = Format(Language.HeartSkill.Description,self:GetAttrDesc(attr_config,true))
                    }
                end
                return list
            end
        end
	end)
end

--找背包里面的心法技能
function HeartSkillData:GetBagSkillList(select_list)
    local function GetNum(list,data)
        local num = 0
        for k,v in pairs(list) do
            if v.index and v.index == data.index then
                num = num + 1
            end
        end
        return data.num - num
    end
    local list = {}
    local color = Item.GetColor(item_id)
    for col,item_list in pairs(BagData.Instance.item_list or {}) do
		if col ~= ItemColumnType.Storage and col ~= ItemColumnType.Temporary then
			for k, item in pairs(item_list) do
				if item:ShowType() == ShowType.HeartSkill and item:Color() <= 5 then
                    local num = GetNum(select_list,item)
                    if num > 0 then
                        table.insert(list,{num = num,index = item.index,item_id = item.item_id,column_type = item.column_type})
                    end
                end
			end
		end
    end
    return list
end

--找背包里面的心法技能
function HeartSkillData:GetBagSkillItem(color)
    for col,item_list in pairs(BagData.Instance.item_list or {}) do
		if col ~= ItemColumnType.Storage and col ~= ItemColumnType.Temporary then
			for k, item in pairs(item_list) do
				if item:ShowType() == ShowType.HeartSkill and item:Color() == color then
                    return item
                end
			end
		end
    end
end

function HeartSkillData:GetBagSkillFullList(select_list,need_num,color)
    local list = {}
    local item_list = self:GetBagSkillList(select_list)
    local item_group = self:GetSplitList(item_list)
    if color ~= nil then
        if item_group[color] and #item_group[color] >= need_num then
            return item_group[color]
        else
            for i=ItemColor.Blue,ItemColor.Red do
                if item_group[i] and #item_group[i] >= need_num then
                    return item_group[i]
                end
            end
        end
    else
        for i=ItemColor.Blue,ItemColor.Red do
            if item_group[i] and #item_group[i] >= need_num then
                return item_group[i]
            end
        end
    end
    local max_list = {}
    for i=ItemColor.Blue,ItemColor.Red do
        for k,v in pairs(item_group[i] or {}) do
            table.insert(max_list,v)
        end
    end
    return max_list
end

--拆分结果 品质分组
function HeartSkillData:GetSplitList(item_list)
    local list = {}
    for k,item in pairs(item_list) do
        local color = Item.GetColor(item.item_id)
        list[color] = list[color] or {}
        for i=1,item.num do
            table.insert(list[color],{index = item.index,item_id = item.item_id,column_type = item.column_type})
        end
    end
    return list
end

--心法协议全部信息下发
function HeartSkillData:SetHeartSkillAllInfo(protocol)
    self.equipes_info.info = protocol.equip_skill_list
    self.initiat_info.info = protocol.initiat_skill_list
    self.passive_info.info = protocol.passive_skill_list
    self.equipes_info.unlock_flag = protocol.unlock_flag
end

--心法协议单独信息下发
function HeartSkillData:SetHeartSkillSingleInfo(protocol)
    local info = protocol.info or {}
    if protocol.type == 1 then
        self.initiat_info.info[info.seq] = info
        self.initiat_info.val = not self.initiat_info.val
    else
        self.passive_info.info[info.seq] = info
        self.passive_info.val = not self.passive_info.val
    end
    self.red_point_data.val = not self.red_point_data.val
end

--心法协议其他信息下发
function HeartSkillData:SetHeartSkillOtherInfo(protocol)
    self.equipes_info.info = protocol.equip_skill_list
    self.equipes_info.unlock_flag = protocol.unlock_flag
    self.red_point_data.val = not self.red_point_data.val
end

--根据心法配置获取心法数据info
function HeartSkillData:GetSkillInfo(data)
    local seq = data.heart_skill_seq
    if data.heart_skill_type == 1 then
        return self.initiat_info.info[seq] or {}
    else
        return self.passive_info.info[seq] or {}
    end
end

--根据心法配置获取该技能是否激活 
function HeartSkillData:GetSkillActive(data)
    return self:GetSkillInfo(data).level ~= nil
end

--根据seq获取对应心法配置
function HeartSkillData:GetSkillConfig(seq)
    return self.skill_config[seq]
end

--根据seq和level获取对应心法升级配置
function HeartSkillData:GetUpgradeConfig(seq,level)
    return self.upgrade_config[seq][level]
end

--根据心法配置获取心法技能等级
function HeartSkillData:GetSkillLevel(data)
    return self:GetSkillInfo(data).level or 1
end

--获取心法镶嵌对应的seq
function HeartSkillData:GetEquipSkill(seq)
    return self.equipes_info.info[seq] or -1
end

function HeartSkillData:GetUnlockFlag(seq)
    return self.equipes_info.unlock_flag[8-seq] or 0
end

--获取心法镶嵌对应配置
function HeartSkillData:GetHoleData(index)
    return self.config.heart_skill_hole[index]
end

function HeartSkillData:GetResonanceConfig(resonance_group,level,is_active)
    local config = self.resonance_level_config[resonance_group][level]
    config.is_active = is_active
    return config
end

function HeartSkillData:GetChangeConfig(item_id)
    return self.change_config[item_id]
end

function HeartSkillData:IsUseItemCell(data)
    local info = self:GetSkillInfo(data) or {}
    for k,v in pairs(self.config.heart_skill_hole) do
        if self:GetEquipSkill(v.hole_seq) == info.index and v.heart_skill_type == data.heart_skill_type then
            return true
        end
    end
    return false
end

function HeartSkillData:GetAttrConfig(heart_skill_seq,heart_level)
    if self.heart_attr_config[RoleData.Instance:GetRoleProf()] then
        if self.heart_attr_config[RoleData.Instance:GetRoleProf()][heart_skill_seq] then
            return self.heart_attr_config[RoleData.Instance:GetRoleProf()][heart_skill_seq][heart_level]
        end
    end
end

function HeartSkillData:GetAttrDesc(config,is_not_none)
    if config == nil then
        return ""
    end
    local desc = ""
    for i=1,2 do
        local attr_type = config["attr_type_"..i] or 0
        local attr_value = config["attr_add_"..i] or 0
        if attr_value ~= 0 then
            local none = is_not_none == nil and "  " or ""
            local attr_name = Language.Common.AttrList[attr_type] .. none
            local value = DataHelper.Percent(attr_type,attr_value)
            desc = desc .. Format("%s<color=#fd5d47>+%s</color> ",attr_name,value)
        end
    end
    return desc
end

function HeartSkillData:GetAttrItemList(config)
    local attr_list = {}
    for i=1,2 do
        local attr_type = config["attr_type_"..i] or 0
        local attr_value = config["attr_add_"..i] or 0
        attr_list[attr_type] = attr_value
    end
    return attr_list
end

function HeartSkillData:GetAttrMainList()
    local base_list = DataHelper.ResetAttrList({})
    for k,v in pairs(self.config.heart_skill_hole) do
        local index = self:GetEquipSkill(v.hole_seq)
        if index ~= -1 then
            local heart_skill_seq,level = self:GetSkillSeq(v.heart_skill_type,index)
            local config = self:GetAttrConfig(heart_skill_seq,level)
            if config then
                local add_list = self:GetAttrItemList(config)
                base_list = DataHelper.AttrListAdd(base_list,add_list)
            end
        end
    end
    local attr_list = {}
    for k,v in pairs(base_list) do
        if v ~= 0 then
            table.insert(attr_list,{attr_type = k,attr_value = v})
        end
    end
    table.sort(attr_list,DataHelper.SortFunc([[attr_type]]))
    return attr_list
end