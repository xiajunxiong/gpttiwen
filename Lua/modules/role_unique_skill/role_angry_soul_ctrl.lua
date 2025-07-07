VIEW_REQ("modules/role_unique_skill/role_angry_soul")
VIEW_REQ("modules/role_unique_skill/role_angry_eff_preview")
VIEW_REQ("modules/role_unique_skill/role_angry_success_view")

RoleAngrySoulCtrl = RoleAngrySoulCtrl or BaseClass(BaseCtrl)

function RoleAngrySoulCtrl:__init()
    if RoleAngrySoulCtrl.Instance ~= nil then
        Debuger.LogError("[RoleAngrySoulCtrl] attempt to create singleton twice!")
    return
    end
    RoleAngrySoulCtrl.Instance = self
    self.data = RoleAngrySoulData.New()
    self.data = RoleAngrySoulData.Instance

    self.level_up_mark = -1
    self:RegisterProtocol(CSNuHunSikkReq)
    self:RegisterProtocol(SCNuHunSkillInfo, "OnSCNuHunSkillInfo")

    local CheckRemind = function()
        return self.data:GetRemindNum()
    end

    RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,self.data.nu_hun_info,CheckRemind)
    RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,BagData.Instance.item_grid,CheckRemind)
end 

function RoleAngrySoulCtrl:SendReq(req_type, p1)
    LogDG("平哥哥，这是怒魂的请求哦",req_type, p1)
    local protocol = ProtocolPool.Instance:GetProtocol(CSNuHunSikkReq)
	protocol.req_type = req_type
	protocol.param = p1 or 0
	SendProtocol(protocol)
end

function RoleAngrySoulCtrl:OnSCNuHunSkillInfo(protocol)
    LogDG("平哥哥，这是怒魂的下发哦",protocol)
    self.data:SetAllInfo(protocol.nu_hun_skill_level)
end


RoleAngrySoulData = RoleAngrySoulData or BaseClass()
function RoleAngrySoulData:__init()
    if RoleAngrySoulData.Instance ~= nil then
		Debuger.LogError("[RoleAngrySoulData] attempt to create singleton twice!")
		return
    end
    RoleAngrySoulData.Instance = self

    self.cfg = Config.role_feisheng_auto.anger_soul
    self.nu_hun_info = SmartData.New({skill_level = {},flush = 0})
end 

function RoleAngrySoulData:SetAllInfo(info_list)
    self.nu_hun_info.skill_level = info_list

    if self:CheckLevelUpMark() then 
        self:ClearLevelUpMark()
    end 

    self.nu_hun_info.flush= self.nu_hun_info.flush + 1
end

function RoleAngrySoulData:GetSoulCfg(level,type)
    for k,v in pairs(self.cfg) do 
        if v.type == type and v.level == level then 
            return v
        end 
    end 
end

function RoleAngrySoulData:GetPreViewList(type)
    local list = {}
    for k,v in pairs(self.cfg) do 
        if v.type == type and v.level > 0 then 
            local level = v.level

            local attr_str = ""
            for i = 1,2 do 
                if v["attr_value_"..i] > 0 then 
                    local str = Language.Common.AttrList[v["att_type_"..i]]
                        ..(string.format(Language.Common.Xstr,COLORSTR.Green4,("+"..v["attr_value_"..i])))
                    attr_str = attr_str .. str .. "       "
                end
            end 

            local effect_cfg = BagData.Instance:GetEquipEffectCfg(v.sp_id)
            local sp_show = effect_cfg == nil and "" or effect_cfg.description
            local info = {
                level = level,
                attr_str = attr_str,
                sp_show = sp_show
            }
            table.insert(list,info)
        end 
    end 

    table.sort(list, function(a, b)
        return a.level < b.level
    end)

    return list
end

function RoleAngrySoulData:GetTypeGroups()
    local list = {}
    for k,v in pairs(self.nu_hun_info.skill_level) do 
        local cur_cfg = self:GetSoulCfg(v,k)
        local next_cfg = self:GetSoulCfg(v+1,k)
        local is_level_max = next_cfg == nil

        local item_id = cur_cfg.item_id
        if item_id == 0 then
            item_id = self:GetSoulCfg(0,k).item_id
        end

        local info = {
            item_id = item_id,
            level = v,
            -- cur_cfg = cur_cfg,
            -- next_cfg = next_cfg,
        }
        table.insert(list,info)
    end 

    for k, v in pairs(list) do
        v.index = k
    end

    return list
end

function RoleAngrySoulData:GetAngrySoulDetail(index)
    local key = index - 1 
    local level = self.nu_hun_info.skill_level[key]
    
    local check_level = level == 0 and level+1 or level

    local cur_cfg = self:GetSoulCfg(check_level,key)
    local next_cfg = self:GetSoulCfg(check_level+1,key)
    local is_level_max = next_cfg == nil

    local item_id = cur_cfg.item_id
    if item_id == 0 then
        item_id = self:GetSoulCfg(0,key).item_id
    end

    local name = Item.GetName(item_id)

    local cur_attr = {}
    local next_attr = {}
    for i = 1,2 do 
        if cur_cfg["attr_value_"..i] > 0 then 
            table.insert(cur_attr,{
                type = Language.Common.AttrList[cur_cfg["att_type_"..i]],
                value = "+"..cur_cfg["attr_value_"..i],
            })
        end

        if not is_level_max then
            if next_cfg["attr_value_"..i] > 0 then 
                table.insert(next_attr,{
                    type = Language.Common.AttrList[next_cfg["att_type_"..i]],
                    value = "+"..next_cfg["attr_value_"..i],
                })
            end
        end
    end 

    local effect_cfg = BagData.Instance:GetEquipEffectCfg(cur_cfg.sp_id)

    local oper_str = level == 0 and Language.AngrySoul.OperActive or Language.AngrySoul.OperLevelUp

    return {
        name = name ,
        level = level,
        is_level_max = is_level_max ,
        cur_attr = cur_attr,
        next_attr = next_attr,
        sp_show = effect_cfg ~=nil and effect_cfg.description or "",
        next_sp_show = (not is_level_max) and BagData.Instance:GetEquipEffectCfg(next_cfg.sp_id).description or "",

        item_id = item_id,
        num = cur_cfg.num,
        oper_str = oper_str,
    }
end

function RoleAngrySoulData:GetSuccessParam()

    local key = self.level_up_mark
    local level = self.nu_hun_info.skill_level[key]

    local cur_cfg = self:GetSoulCfg(level-1,key)
    local next_cfg = self:GetSoulCfg(level,key)

    local item_id = next_cfg.item_id > 0 and next_cfg.item_id or cur_cfg.item_id
    
    local name = Item.GetName(item_id)

    local cur_attr = ""
    local next_attr = ""
    for i = 1,2 do 
        if cur_cfg["attr_value_"..i] > 0 then 
            local str = Language.Common.AttrList[cur_cfg["att_type_"..i]] .. "    +" ..cur_cfg["attr_value_"..i] 
            cur_attr = cur_attr .. str .. "      "
        end

        if next_cfg["attr_value_"..i] > 0 then 
            local str = Language.Common.AttrList[next_cfg["att_type_"..i]] 
            .. (string.format(Language.Common.Xstr,COLORSTR.Green4,("    +"..next_cfg["attr_value_"..i])))
            next_attr = next_attr .. str .. "      "
        end
    end 
    local effect_cfg = BagData.Instance:GetEquipEffectCfg(cur_cfg.sp_id)
    local next_effect_cfg = BagData.Instance:GetEquipEffectCfg(next_cfg.sp_id)

    self.level_up_mark = -1
    return {
        type = key+1,
        item_id = item_id,
        name = name,
        cur_attr = cur_attr,
        next_attr = next_attr,
        cur_eff_show =effect_cfg ~=nil and effect_cfg.description or "",
        next_eff_show = next_effect_cfg ~=nil and next_effect_cfg.description or "",
        is_active = level == 1,
    }
end

function RoleAngrySoulData:SetLevelUpMark(mark)
    self.level_up_mark = mark
end 

function RoleAngrySoulData:CheckLevelUpMark()
    return self.level_up_mark ~= nil and self.level_up_mark > -1
end

function RoleAngrySoulData:ClearLevelUpMark()
    ViewMgr:OpenView(AngrySoulSuccessShow)
end

-- 0~2
function RoleAngrySoulData:GetRemindForIndex(index)
    local key = index
    local level = self.nu_hun_info.skill_level[index]
    level = level or 0

    local cur_cfg = self:GetSoulCfg(level,key)
    local next_cfg = self:GetSoulCfg(level+1,key)
    local is_max = next_cfg == nil

    local item_id = cur_cfg.item_id
    if item_id == 0 then
        item_id = self:GetSoulCfg(0,key).item_id
    end
    local num = cur_cfg.num
    local cur_num = Item.GetNum(item_id)

    return (not is_max and cur_num >= num) and 1 or 0
end

function RoleAngrySoulData:GetRemindNum()
    for i = 0,2 do 
        local red_num = self:GetRemindForIndex(i)
        if red_num > 0 then
            return red_num     
        end
    end 
    return 0
end

function RoleAngrySoulData:GetOpenData()
    return {
        red_num = self:GetRemindNum(),
    }
end