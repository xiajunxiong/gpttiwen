FILE_REQ("modules/bicycle/bicycle_data_config")
FILE_REQ("modules/bicycle/bicycle_data")
FILE_REQ("modules/bicycle/bicycle_other_data")
VIEW_REQ("modules/bicycle/bicycle_view")
VIEW_REQ("modules/bicycle/bicycle_child_view")
VIEW_REQ("modules/bicycle/bicycle_tab_panel")
VIEW_REQ("modules/bicycle/bicycle_show_view")
VIEW_REQ("modules/bicycle/bicycle_halidom")
VIEW_REQ("modules/bicycle/bicycle_change")
VIEW_REQ("modules/bicycle/bicycle_skill_compose_view")
VIEW_REQ("modules/bicycle/bicycle_sel_skill_stuff_view")
VIEW_REQ("modules/bicycle/bicycle_target_skill_view")
VIEW_REQ("modules/bicycle/bicycle_skill_convert_view")
VIEW_REQ("modules/bicycle/bicycle_skill_book_all_up")

BicycleCtrl = BicycleCtrl or BaseClass(BaseCtrl)

function BicycleCtrl:__init()
    if BicycleCtrl.Instance ~= nil then
		Debuger.LogError("[BicycleCtrl] attempt to create singleton twice!")
		return
	end
    BicycleCtrl.Instance = self
    self.data = BicycleData.New()
    BicycleOtherData.New()

    self:RegisterAllProtocol()
    self:RegisterAllRemind()

    BagData.Instance:ListenItems(self.data:GetRemindItemList(),function ()
        self.data:CheckRemind()
    end)
    RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.data.CheckRemind, self.data),"level")
end

function BicycleCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSBicycleReq)
    self:RegisterProtocol(CSBicycleSkillReq)
    self:RegisterProtocol(CSSmartMountsSystemKeyComposeReq)
    self:RegisterProtocol(SCBicycleAllInfo,"OnSCBicycleAllInfo")
    self:RegisterProtocol(SCBicycleOneInfo,"OnSCBicycleOneInfo")
    self:RegisterProtocol(SCBicycleAttri,"OnSCBicycleAttri")
    self:RegisterProtocol(SCBicyclePetInfo,"OnSCBicyclePetInfo")
    self:RegisterProtocol(SCBicycleNotice,"OnSCBicycleNotice")
    self:RegisterProtocol(SCBicycleRefineSkillInfo,"OnSCBicycleRefineSkillInfo")
    self:RegisterProtocol(SCBicycleOtherInfo,"OnSCBicycleOtherInfo")
    self:RegisterProtocol(SCBicycleChangeInfo,"OnSCBicycleChangeInfo")
end

function BicycleCtrl:RegisterAllRemind()
    RemindCtrl.Instance:Remind():Register(
        Mod.Bicycle.Basice,
        self.data.remind_data,
        BindTool.Bind(self.data.BicycleUnlockRemindNum, self.data)
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Bicycle.Foster,
        self.data.remind_data,
        BindTool.Bind(self.data.BicycleUpLevelRemindNum, self.data)
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Bicycle.Skill,
        self.data.remind_data,
        BindTool.Bind(self.data.BicycleUpSkillRemindNum, self.data)
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Bicycle.DianHua,
        self.data.remind_data,
        function ()
            local red_num = self.data:BicycleDianHuaRemindNum()
            return red_num
        end
    )
    BagData.Instance.item_grid[ItemColumnType.Item]:Care(function ()
        self.data:CheckRemind()
    end)
end

function BicycleCtrl:__delete()
    
end

-- 请求服务端
function BicycleCtrl:Req(op_type, p1, p2, p3)
    local protocol = ProtocolPool.Instance:GetProtocol(CSBicycleReq)
	protocol.op_type = op_type      -- op_type : BicycleCfg.OP_TYPE
	protocol.param_1 = p1 or 0
	protocol.param_2 = p2 or 0
	protocol.param_3 = p3 or 0
	SendProtocol(protocol)
end


-- 技能相关请求
function BicycleCtrl:ReqSkill(op_type, p1, p2, p3, p4)
    local protocol = ProtocolPool.Instance:GetProtocol(CSBicycleSkillReq)
	protocol.op_type = op_type      -- op_type : BicycleCfg.OP_TYPE
	protocol.param_1 = p1 or 0
	protocol.param_2 = p2 or 0
    protocol.param_3 = p3 or 0
    protocol.param_4 = p4 or 0
	SendProtocol(protocol)
end

function BicycleCtrl:SendCSSmartMountsSystemKeyComposeReq(count, column, index_flag)
    local protocol = GetProtocol(CSSmartMountsSystemKeyComposeReq)
	protocol.count = count or 0
	protocol.column = column or 0
	protocol.index_flag = index_flag or {}
	SendProtocol(protocol)
end

----------- callback ---------

-- 全部灵骑信息下发
function BicycleCtrl:OnSCBicycleAllInfo(protocol)
    --LogError("灵骑全部信息",protocol)
    self.data:SetBicycleList(protocol)
    self.data:CheckRemind()
end

-- 单个灵骑信息下发
function BicycleCtrl:OnSCBicycleOneInfo(protocol)
    --LogError("单个灵骑信息",protocol)
    local bicycle = Bicycle.New(protocol.bicycle)
    self.data:CurBicycle(bicycle)
    self.data:CheckRemind()
    local show_type = self.data:BicycleShowType()
    if BicycleCfg.SHOW_TYPE_CARE[show_type] then
        self:HandleShowType()
    end
    if self.is_flush_role then
        self.is_flush_role = nil
        self:FlushRoleAppearance()
    end
end

-- 灵骑基础属性列表下发
function BicycleCtrl:OnSCBicycleAttri(protocol)
    --LogError("灵骑属性信息",protocol)
    local info = protocol.info
    self.data.protocol_info_2103:Set(info)
end

-- 灵骑统御单个信息下发
function BicycleCtrl:OnSCBicyclePetInfo(protocol)
    --LogError("灵骑统御信息",protocol)
    local info = protocol.info
    local bicycle = self.data:GetBicycleByIndex(info.index)
    bicycle:PetIndexList(info.pet_list)
    self.data:UpdatePetBicycle()
    self.data.protocol_info_2104:Set(info)
    self.data:CheckRemind()
end

-- 灵骑操作通知
function BicycleCtrl:OnSCBicycleNotice(protocol)
    --LogError("灵骑操作通知",protocol)
    local info = protocol.info
    self.data.protocol_info_2105:Set(info)
    local bicycle = self.data:GetBicycleByIndex(info.index)
    local notice_type = info.notice_type
    if not bicycle then
        return
    end
    if notice_type == BicycleCfg.NOTICE_TYPE.ACTIVE then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.ACTIVE_BICYCLE)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.LEVEL_UP then
        --local effect_info = {effect = BicycleCfg.EFFECT.UP_LEVEL}
        --self.data:CurEffect(effect_info)
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.UP_BICYCLE)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.BASE_SKILL_UP then
        local base_skill_list = bicycle:BaseSkillList()
        local skill = base_skill_list[info.param_2]
        if info.param_1 > skill:Level() then
            local effect_info = {effect = BicycleCfg.EFFECT.UP_SKILL_START,skill_index = info.param_2 + 1}
            self.data:CurEffect(effect_info)
            self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.UP_SKILL)
        end
        skill:Level(info.param_1)
        self.data.change_bicycle.other_info = skill
        
    elseif notice_type == BicycleCfg.NOTICE_TYPE.EXCLUSIVE_SKILL_UP then    -- 专属技能砍掉了
        -- local base_skill_list = bicycle:ExclusiveSkillList()
        -- local skill = base_skill_list[info.param_2]
        -- if info.param_1 > skill:Level() then
        --     local effect_info = {effect = BicycleCfg.EFFECT.UP_SKILL_START,skill_index = info.param_2 + 4}
        --     self.data:CurEffect(effect_info)
        --     self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.UP_SKILL) 
        -- end
        -- skill:Level(info.param_1)
        -- self.data.change_bicycle.other_info = skill
    elseif notice_type == BicycleCfg.NOTICE_TYPE.BREAK_SUCC then
        local effect_info = {effect = BicycleCfg.EFFECT.UP_LEVEL}
        self.data:CurEffect(effect_info)
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.BREAK_SUCC)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.ENERGY_VALUE then
        bicycle:Energy(info.param_1)
        self.data.bicycle_energy_tip.info = {bicycle_id = bicycle.id}
    elseif notice_type == BicycleCfg.NOTICE_TYPE.ACTIVE_EQUIP then

    elseif notice_type == BicycleCfg.NOTICE_TYPE.UP_EQUIP then

    elseif notice_type == BicycleCfg.NOTICE_TYPE.ACTIVE_HALIDOM then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.HALIDOM_ACTIVE)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.UP_HALIDOM then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.HALIDOM_UP)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.UP_DIAN_HUA then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.DIAN_HUA_SUCC)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.LEARA_SKILL then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.SKILL_STUDY)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.REPLACE_SKILL then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.SKILL_CONVERT)
    elseif notice_type == BicycleCfg.NOTICE_TYPE.HOLY_CONVERT then
        self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.HALIDOM_ACTIVE)
        local s_bicycle = self.data:GetBicycleByIndex(info.param_2)
        local level = info.param_3
        local halidom = s_bicycle:Halidom()
        if level == 0 then
            halidom.icon_id = 0
        end
        halidom:Level(level)
        bicycle.show_is_hlidom_canvert = true
    end
    self.data:LastBicycle(bicycle)
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SINGLE_INFO,bicycle:Index())
end

-- 灵骑技能洗练结果下发
function BicycleCtrl:OnSCBicycleRefineSkillInfo(protocol)
    --LogError("洗练结果",protocol)
    local info = protocol.info
    local bicycle = self.data:GetBicycleByIndex(info.index)
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SINGLE_INFO,bicycle:Index())
    bicycle:BaseSkillList(info.skill_list, true)
    bicycle:NewBaseSkillList(info.new_skill_list, true)
    self.data.protocol_info_2106:Set(info)
end

-- 灵骑其它信息
function BicycleCtrl:OnSCBicycleOtherInfo(protocol)
    self.data:RideBicycleIndex(protocol.ride_bicycle_index)
    self.data:GuideFlag(protocol.is_guide_flag)
end

-- 灵骑变化信息
function BicycleCtrl:OnSCBicycleChangeInfo(protocol)
    --LogError("灵骑变化信息",protocol)
    local info = protocol.info
    local change_type = info.change_type
    local bicycle = self.data:GetBicycleByIndex(info.index)
    if change_type == BicycleCfg.CHANGE_TYPE.ENERGY then
        bicycle:Energy(info.param_1)
        if info.param_1 < 500 then
            self.data:CheckRemind()
        end
    elseif change_type == BicycleCfg.CHANGE_TYPE.NATURE then
        bicycle:Nature(info.param_1 / 100)
        bicycle:Attri(nil,true)
    elseif change_type == BicycleCfg.CHANGE_TYPE.ORNAMENTS then
        if info.param_1 > 0 then
            local effect_info = {effect = BicycleCfg.EFFECT.UP_LEVEL}
            self.data:CurEffect(effect_info)
            self.data:BicycleShowType(BicycleCfg.SHOW_TYPE.EQUIP)
        end
        self.data:LastBicycle(bicycle)
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SINGLE_INFO,bicycle:Index())
        -- 装备变化刷新下主角坐骑状态
        self.is_flush_role = true
    elseif change_type == BicycleCfg.CHANGE_TYPE.UP_ORNAMENT then
        local ornement = bicycle:Ornament()
        local new_level = info.param_1
        local orn_cfg = Cfg.GetOrnamentCfg(ornement:Id(),new_level)
        local new_ornament = BicycleOrnament.New(ornement:IconId(),new_level,bicycle.id,orn_cfg)
        bicycle:Ornament(new_ornament)
        bicycle:Attri(nil, true)
    elseif change_type == BicycleCfg.CHANGE_TYPE.MODEL_CHANGE then
        bicycle:CurModelType(info.param_1)
    elseif change_type == BicycleCfg.CHANGE_TYPE.AUTO_RECOVER_FLAG then
        bicycle.is_auto_recover = info.param_1
    end
    self.data.protocol_info_2108:Set(info)
end

-- 刷新主角坐骑变化
function BicycleCtrl:FlushRoleAppearance()
    local appearance = TableCopy(RoleData.Instance:GetApearance())
    RoleData.Instance:SetApearance(appearance)
end

-- 处理是否需要打开展示界面
function BicycleCtrl:HandleShowType()
    local show_type = self.data:BicycleShowType()
    if show_type == nil or show_type == BicycleCfg.SHOW_TYPE.Not then
        return
    end
    if ViewMgr:IsOpen(BicycleShowView) then
        return
    end
    local param_t = {}
    param_t.show_type = show_type 
    param_t.show_info = self.data.change_bicycle
    ViewMgr:OpenView(BicycleShowView,param_t)
end