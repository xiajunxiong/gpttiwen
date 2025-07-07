VIEW_REQ("modules/role_unique_skill/role_unique_skill_panel")
VIEW_REQ("modules/role_unique_skill/role_xiu_wei")

RoleXiuWeiCtrl = RoleXiuWeiCtrl or BaseClass(BaseCtrl)

-- 修为请求类型
RoleXiuWeiCtrl.REQ_TYPE = {
    GET_INFO = 0,       -- 获取信息
    UP_LEVEL = 1,       -- 升级技能 p1 技能类型, p2 修炼丹类型
    ONE_KEY = 2,        -- 一键升级 p1 技能类型 p2 修炼丹类型
}

-- 修炼丹类型
RoleXiuWeiCtrl.ITEM_TYPE = {
    QUA1 = 0,       -- 普通修为丹
    QUA2 = 1,       -- 高级修为丹
}

-- 技能类型
RoleXiuWeiCtrl.SKILL_TYPE = {
    ROLE_ATK = 0,       -- 人物攻修
    ROLE_DEF = 1,       -- 人物防修
    PET_ATK = 2,        -- 宠物攻修
    PET_DEF = 3,        -- 宠物防修
    PARTNER_ATK = 4,    -- 伙伴攻修
    PARTNER_DEF = 5,    -- 伙伴防修
}

function RoleXiuWeiCtrl:__init()
    if RoleXiuWeiCtrl.Instance ~= nil then
        Debuger.LogError("[RoleXiuWeiCtrl] attempt to create singleton twice!")
    return
    end
    RoleXiuWeiCtrl.Instance = self
    self.data = RoleXiuWeiData.New()
    self.data = RoleXiuWeiData.Instance

    self:RegisterProtocol(CSXiuWeiReq)
    self:RegisterProtocol(SCXiuWeiAllInfo, "OnSCXiuWeiAllInfo")
    self:RegisterProtocol(SCXiuWeiInfo, "OnSCXiuWeiInfo")
    self:RegisterProtocol(SCXiuWeiDanYaoLimitInfo, "OnSCXiuWeiDanYaoLimitInfo")
    BagData.Instance:ListenItems({27870,27871},function ()
        self.data:RemindNotify()
    end)
    RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,self.data.remind_data,function ()
        return self.data:GetRemind()
    end)
end

function RoleXiuWeiCtrl:SendReq(req_type, p1, p2)
    local protocol = ProtocolPool.Instance:GetProtocol(CSXiuWeiReq)
	protocol.req_type = req_type      -- op_type : BicycleCfg.OP_TYPE
	protocol.p1 = p1 or 0
	protocol.p2 = p2 or 0
	SendProtocol(protocol)
end

function RoleXiuWeiCtrl:OnSCXiuWeiAllInfo(protocol)
    self.data:SetAllInfo(protocol.info_list)
end

function RoleXiuWeiCtrl:OnSCXiuWeiInfo(protocol)
    local skill_type = protocol.skill_type
    local skill_info = protocol.skill_info
    self.data:SetSkillInfo(skill_type, skill_info)
end

function RoleXiuWeiCtrl:OnSCXiuWeiDanYaoLimitInfo(protocol)
    self.data.use_info.use_count_qua1 = protocol.use_count_qua1
    self.data.use_info.use_count_qua2 = protocol.use_count_qua2
end



RoleXiuWeiData = RoleXiuWeiData or BaseClass()
function RoleXiuWeiData:__init()
    if RoleXiuWeiData.Instance ~= nil then
		Debuger.LogError("[RoleXiuWeiData] attempt to create singleton twice!")
		return
    end
    RoleXiuWeiData.Instance = self
    self.cfg = Config.xiuwei_auto
    self.skill_info_list = SmartData.New()
    self.use_info = {use_count_qua1 = 0, use_count_qua2 = 0}
    self.remind_data = SmartData.New({val = false})

    Cfg.Register("GetRoleXiuWeiSkillCfg",Cfg.RealTimeSearch2Key(self.cfg.xiuwei_att,"xiuwei_type","xiuwei_level"))
end

function RoleXiuWeiData:SetAllInfo(info_list)
    self.skill_info_list:Set(info_list)
end

function RoleXiuWeiData:SetSkillInfo(skill_type, info)
    self.skill_info_list[skill_type] = info
    self:RemindNotify()
end

function RoleXiuWeiData:GetAllInfo()
    return self.skill_info_list
end

function RoleXiuWeiData:GetSkillInfo(skill_type)
    local info = self.skill_info_list[skill_type]
    return info
end

function RoleXiuWeiData:GetSkillCfg(skill_type, level, is_log)
    is_log = is_log or false
    level = level or 1
    local cfg = Cfg.GetRoleXiuWeiSkillCfg(skill_type, level, is_log)
    return cfg
end

function RoleXiuWeiData:GetSkillDesc(skill_type)
    for i,v in pairs(self.cfg.xiuwei_dec) do
        if v.xiuwei_type == skill_type then
            return v
        end
    end
    return {}
end

function RoleXiuWeiData:GetSkillMaxLevel(skill_type)
    local desc_cfg = self:GetSkillDesc(skill_type)
    return desc_cfg.max_level or 0
end

-- 获取修为技能属性数据
function RoleXiuWeiData:GetSkillAttriListData(skill_type, level)
    local info = self:GetSkillInfo(skill_type)
    local level = level or info.level
    local cfg = self:GetSkillCfg(skill_type, level) or {}
    local next_cfg = self:GetSkillCfg(skill_type, level + 1) or {}

    local read_cfg
    if TableIsEmpty(cfg) then
        read_cfg = next_cfg
    else
        read_cfg = cfg
    end


    local list_data = {}
    local sp_data
    for i,v in pairs(read_cfg) do
        local attri_name = Language.RoleXiuWei.Attri[i]
        if attri_name and v > 0 then
            local data = {}
            data.name = attri_name
            if level == 0 then
                data.value = 0
                data.next_value = v
            else
                data.value = v
                data.next_value = next_cfg[i] or v
            end

            if i ~= "hp_add" then
                data.value = (data.value / 10) .. "%"
                data.next_value = (data.next_value / 10) .. "%"
            end
            table.insert(list_data, data)
        elseif i == "sp_id" and v > 0 then
            local data = {}
            local cur_sp_id = v
            local next_sp = next_cfg.sp_id or cur_sp_id
            local cur_sp_cfg = Cfg.SpecialEffect(cur_sp_id)
            local next_sp_cfg = Cfg.SpecialEffect(next_sp)
            data.name = cur_sp_cfg.name
            if level == 0 then
                data.value = "0%"
                data.next_value = cur_sp_cfg.description
            else
                data.value = cur_sp_cfg.description
                data.next_value = next_sp_cfg.description
            end
            sp_data = data
            --table.insert(list_data, data)
        end
    end
    if sp_data then
        table.insert(list_data,sp_data)
    end
    return list_data
end

-- 获取修为功能开启标记，表示开启，0表示飞升阶段不够，-1表示等级不够，隐藏
function RoleXiuWeiData:GetOpenFlag()
    local level = RoleData.Instance:GetRoleLevel()
    local limit_level = self.cfg.other[1].open_level
    if level < limit_level then
        return -1
    end
    local fly_stage = FlyUpData.Instance:GetStage()
    local limit_stage = self.cfg.other[1].need_feisheng
    if fly_stage < limit_stage then
        return 0
    end
    return 1
end

function RoleXiuWeiData:GetOpenData()
    local data = {}
    local flag = self:GetOpenFlag()
    data.red_num = 0
    if flag == -1 then
        data.is_shield = true
    elseif flag == 0 then
        data.is_lock = true
        data.lock_tip = Language.RoleXiuWei.Tip10
    elseif flag == 1 then
        data.red_num = self:GetRemind()
    end
    return data
end

-- 修为升级标记 1 ：可升级，0：最大等级，-1，使用上限，-2材料不足
function RoleXiuWeiData:GetUpFlag(skill_type, item_type, item_id)
    if TableIsEmpty(self.sel_skill_data) then
        return -1
    end

    skill_type = skill_type or self.sel_skill_data.xiuwei_type
    local skill_info = self:GetSkillInfo(skill_type)
    local cfg = RoleXiuWeiData.Instance:GetSkillDesc(skill_type)
    if skill_info.level >= cfg.max_level then
        return 0
    end

    if item_type and item_type == RoleXiuWeiCtrl.ITEM_TYPE.QUA2 then
        local use_count = self.use_info.use_count_qua2
        local limit_count = self.cfg.other[1].use_times
        if use_count >= limit_count then
            return -1
        end
    end

    if item_id then
        local num = Item.GetNum(item_id)
        if num == 0 then
            return -2
        end
    end
    
    return 1
end

-- 获取修为可升的最大等级
function RoleXiuWeiData:GetCanUpLevel(skill_type)
    local role_level = RoleData.Instance:GetRoleLevel()
    level = 0
    for i,v in pairs(self.cfg.xiuwei_att) do
        if v.xiuwei_type == skill_type then
            if role_level >= v.open_level and level < v.xiuwei_level then
                level = v.xiuwei_level
            end
        end
    end
    return level
end

-- 修为提醒 返回大于0的就是可升级修为
function RoleXiuWeiData:GetRemind()
    -- 是否有材料
    local other_cfg = self.cfg.other[1]
    local item_id1 = other_cfg.cost1
    local item_id2 = other_cfg.cost2
    
    local item_num_1 = Item.GetNum(item_id1)
    local item_num_2 = Item.GetNum(item_id2)

    if item_num_1 == 0 and item_num_2 == 0 then
        return 0
    end

    local func = function (skill_type)
        -- 是否到最大等级
        local skill_info = self:GetSkillInfo(skill_type)
        if TableIsEmpty(skill_info) then
            return 0
        end
        local cfg = self:GetSkillDesc(skill_type)
        if skill_info.level >= cfg.max_level then
            return -1
        end

        -- 等级限制
        local skill_att_cfg = self:GetSkillCfg(skill_type, skill_info.level + 1)
        local role_level = RoleData.Instance:GetRoleLevel()
        if role_level < skill_att_cfg.open_level then
            return -2
        end

        -- 使用限制
        local use_count = 0
        local limit_count = 0
        local item_type = item_num_1 > 0 and RoleXiuWeiCtrl.ITEM_TYPE.QUA1 or RoleXiuWeiCtrl.ITEM_TYPE.QUA2
        if item_type == RoleXiuWeiCtrl.ITEM_TYPE.QUA1 then
            use_count = self.use_info.use_count_qua1
            limit_count = other_cfg.use_times1
        else
            use_count = self.use_info.use_count_qua2
            limit_count = other_cfg.use_times
        end

        local can_use_count = limit_count - use_count
        if can_use_count <= 0 then
            return -3
        end
        return 1
    end

    for skill_type = 0, 5 do
        local remind_num = func(skill_type)
        if remind_num > 0 then
            return 1
        end
    end
    return 0
end

-- 红点通知
function RoleXiuWeiData:RemindNotify()
    self.remind_data.val = not self.remind_data.val
end