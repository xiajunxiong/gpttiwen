VIEW_REQ("modules/pet_amulet/amulet_assembly_view")
VIEW_REQ("modules/pet_amulet/amulet_reset_view")
VIEW_REQ("modules/pet_amulet/amulet_getway")
VIEW_REQ("modules/pet_amulet/amulet_intensify_view")

AmuletCtrl = AmuletCtrl or BaseClass(BaseCtrl)

PET_EQUIP_REQ_TYPE = {
    REQ_TYPE_COMPOSE = 0,      --合成       p1:合成序号 p2:背包index1 p3:背包index2
    REQ_TYPE_REFRESH = 1,      --洗练/重置  p1:洗炼类型 p2:洗练序号 p3:背包index
    REQ_TYPE_SAVE_REFRESH = 2, --保存洗练/重置 p1:洗练类型 p2:背包index
    REQ_TYPE_BREAK = 3,        --分解  p1:背包index
}
PetEquipResetType = {
    Bag = 1,                   --背包里面的护符重置
    Pet = 2,                   --宠物身上的护符重置
}

function AmuletCtrl:__init()
    if AmuletCtrl.Instance ~= nil then
		Debuger.LogError("[AmuletCtrl] attempt to create singleton twice!")
		return
	end
    AmuletCtrl.Instance = self
    self.data = AmuletData.New()
    self:RegisterProtocol(CSMtPetEquipmentReq)
end

function AmuletCtrl:__delete()
    self:UnRegisterProtocol(CSMtPetEquipmentReq)
    AmuletCtrl.Instance = nil
    self.data = nil
end

function AmuletCtrl:SendOperateReq(op_type, param1, param2, param3)
    -- Debuger.LogError("op_type "..op_type.."|param1 "..param1.."|param2 "..param2.."|param3 "..param3)
    local protocol = GetProtocol(CSMtPetEquipmentReq)
    protocol.op_type = op_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.param3 = param3 or 0
    SendProtocol(protocol)
end

--分解护符
function AmuletCtrl:Break(item)
    if item == nil or item.item_id == nil then
        return
    end
    local item_cfg = Item.GetConfig(item.item_id)
    if BagData.Instance:TryFindEmptyInBag(item_cfg.break_item) == false then
        PublicPopupCtrl.Instance:Center(Language.Pet.PetEquip.NotBagEmptyTip)
        return
    end
    local break_item_cfg = Item.GetConfig(item_cfg.break_item)
    local item_color = QualityColorStr[item_cfg.color]
    local break_item_color = QualityColorStr[break_item_cfg.color]
    local param_t = {
        content = string.format(Language.Pet.PetEquip.BreakContent,item_color,item_cfg.name,break_item_color,break_item_cfg.name,item_cfg.break_num),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                self:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_BREAK,item.index)
            end 
        },
        --tnr = DTTodayNotRemind.PetEquipBreak
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

--======== AmuletData ================
AmuletData = AmuletData or BaseClass()

AmuletData.MaxIntensifyLevel = 5

function AmuletData:__init()
    if AmuletData.Instance ~= nil then
		Debuger.LogError("[AmuletData] attempt to create singleton twice!")
		return
	end
    AmuletData.Instance = self
    self.SelectedResetData = SmartData.New()
    self.SelectedAssemblyData = SmartData.New()
    self.AssemblyData = SmartData.New({[1] = {},[2] = {}})
    self.pet_equipment_cfg = Config.pet_equipment_auto
    self.pet_equipment_item_cfg = Config.pet_equipment_item_auto
    self.assembly_pos = 1     --这个就是护符合成界面那两个格子的位置标记
   -- self.reset_ctrl_type = self.reset_ctrl_type or {}  --重置界面操作类型

    Cfg.Register("AmuletInstensifySkillCfg",Cfg.RealTimeSearch(Config.pet_equipment_auto.strengthen,"skill_id"))
    Cfg.Register("AmuletInstensifySkillCfgBy2Key",Cfg.RealTimeSearch2Key(Config.pet_equipment_auto.strengthen, "type", "skill_level"))

    self.intensify_data = SmartData.New()   -- 护符强化
    self:InitData()
end

function AmuletData:__delete()
    self.SelectedResetData:Set({})
    self.SelectedResetData = nil
    self.SelectedAssemblyData:Set({})
    self.self.SelectedAssemblyData = nil
    self.AssemblyData:Set({})
    self.AssemblyData = nil
end

function AmuletData:InitData()
    self:InitShowExplainCfg()
    self:InitSkillGroup()
end
--整合概率配置表
function AmuletData:InitShowExplainCfg()
    self.show_explain_cfg = {}
    for _,v in pairs(self.pet_equipment_cfg.show_explain) do
        if self.show_explain_cfg[v.synthesis_type] == nil then
            self.show_explain_cfg[v.synthesis_type] = {}
        end
        table.insert(self.show_explain_cfg[v.synthesis_type],v)
    end
end
--整合技能组
function AmuletData:InitSkillGroup()
    self.skill_group = {}
    for _,v in pairs(self.pet_equipment_cfg.skill_group) do
        if self.skill_group[v.seq] == nil then
            self.skill_group[v.seq] = {}
        end
        local cfg=self:GetGiftCfg(v.gift_id)
        table.insert(self.skill_group[v.seq],cfg)
    end
end

function AmuletData:SetSelectedResetData(data)
    self.SelectedResetData:Set(data)
end

function AmuletData:GetSelectedResetData()
    return self.SelectedResetData
end

function AmuletData:SetSelectedAssemblyData(data)
    self.SelectedAssemblyData:Set(data)
end

function AmuletData:GetSelectedAssemblyData()
    return self.SelectedAssemblyData
end

-- function AmuletData:SetResetCtrlType(type)
--     self.reset_ctrl_type = type
-- end

-- function AmuletData:GetResetCtrlType()
--     return self.reset_ctrl_type
-- end

--添加合成物品
function AmuletData:SetAssemblyData(item_data,index)
    index = index or self.assembly_pos
    self.AssemblyData[index] = item_data
end

function AmuletData:GetAssemblyData()
    return self.AssemblyData
end


--获取宠物天赋配置
function AmuletData:GetGiftCfg(param, level)
    level = level or self:GetIntensifySkillLevel(param)
    local cfg = PetData.Instance:GetGiftDetail(param, level)
    return cfg
end

--获取护符重置消耗材料配置
function AmuletData:GetResetStuffCfg(item_id)
    local cfg = self.pet_equipment_cfg.refining
    for _,v in pairs(cfg) do
        if item_id == v.id then
            return v
        end
    end
end

--获取技能组
function AmuletData:GetSkillGroup(seq)
    return self.skill_group[seq]
end
--老虎机效果技能配置列表
function AmuletData:GetGiftCfgList(seq,gift_id)
    local skill_list = TableCopy(self:GetSkillGroup(seq))
    local random_list = {}
    local leng = #skill_list
    local target_index = leng%2 == 0 and leng/2 or (leng-1)/2
    if leng<target_index then
        target_index=leng
    end

    local rand_index_t=self:GetRamdonList(1,leng)

    local target_gift_id_ind=target_index
    for i=1,leng do
        local v=skill_list[rand_index_t[i]]
        v.not_name=1
        random_list[i]=v
        if v.param_1==gift_id then
            target_gift_id_ind=i
        end
    end
    local T = random_list[target_gift_id_ind]
    random_list[target_gift_id_ind]=random_list[target_index]
    random_list[target_index]=T
    local list_length=#random_list
    random_list[list_length+1]=random_list[1]
    random_list[list_length+2]=random_list[2]
    random_list[list_length+3]=random_list[3]
    return random_list,target_index
end

--获取一组随机数组成的列表
function AmuletData:GetRamdonList(n,m)
    local function IsHasNum(list,num)
        for i,v in ipairs(list) do
            if v == num then
                return true
            end
        end
        return false
    end
    local list = {}
    local num = GameMath.Ramdon(n,m)
    while #list < m do
        while IsHasNum(list,num) do
            num = GameMath.Ramdon(n,m)
        end
        table.insert(list,num)
    end
    return list
end

--获取护符重置索引--重置就是洗练
function AmuletData:GetResetSeq(item_id)
    local cfg = self:GetResetStuffCfg(item_id)
    return cfg.seq
end

--通过护符获取技能seq
function AmuletData:GetSkillSeqByItemId(item_id, group_index)
    local cfg = self.pet_equipment_item_cfg[item_id]
    return group_index == 1 and cfg.skill_group or cfg.skill_group_2
end

--获取背包里的护符数据
function AmuletData:GetAmuletsList(is_sort)
    is_sort=is_sort or true
    if is_sort then
        local list = BagData.Instance:GetPetEquipmentList()
        local sort_func = function (a,b)
            if a.item_id == b.item_id then
                return a.index < b.index
            else
                return a.item_id > b.item_id
            end
        end
        table.sort(list, sort_func)
        return list
    else
        return BagData.Instance:GetPetEquipmentList()
    end
end

--获取背包里面的护符数据和已经装备的护符数据
function AmuletData:GetBagAndPetAmuletList(is_front)
    local pet_amulet_list = PetData.Instance:GetPetEquipmentList()
    local bag_amulet_list = self:GetAmuletsList()
    if is_front then
        return AndList(bag_amulet_list,pet_amulet_list)
    else
        return AndList(pet_amulet_list,bag_amulet_list)
    end
end

--获取护符合成列表项
function AmuletData:GetAssemblyList()
    local cfg_list = {}
    local cfg = self.pet_equipment_cfg.synthesis
    --没有分类写死
    cfg_list[1] = {id = 1,name = Language.Pet.PetEquip.Assembly,children = {}}
    for _,v in pairs(cfg) do
        local children = {
            type = 1,
            name = Item.GetName(v.stuff_id_1),
            sub_type = v.synthesis_type,
            item_id = v.stuff_id_1,
        }
        table.insert(cfg_list[1].children,children)
    end 
    return cfg_list

end

--获取护符合成材料配置
function AmuletData:GetAssemblyStuffCfg(synthesis_type)
    local cfg = self.pet_equipment_cfg.synthesis
    for _,v in pairs(cfg) do
        if synthesis_type == v.synthesis_type then
            return v
        end
    end
end

--检测护符是否能选择
function AmuletData:CheckIsCanAssembly(item_data)
    --检测是否重复选择
    for i = 1,2 do
        if self.AssemblyData[i].param then
            if item_data.param.pet_index then
                if self.AssemblyData[i].param.pet_index == item_data.param.pet_index then
                    return false
                end
            else
                if self.AssemblyData[i].index == item_data.index then
                    return false
                end
            end
        end
    end
    --检测是否属性改合成类型的消耗材料
    if self.SelectedAssemblyData.sub_type == nil then
        return false
    end 
    local cfg = self:GetAssemblyStuffCfg(self.SelectedAssemblyData.sub_type)
    if item_data.item_id ~= cfg.stuff_id_1 and item_data.item_id ~= cfg.stuff_id_2 then
        return false
    else
        return true
    end
end

--获取选择合成护符的数量
function AmuletData:GetSelectedAssemblyNum(item_id)
    local num = 0
    if item_id == nil then
        return num
    end
    local data = self:GetAssemblyData()
    if data == nil then
        return num
    end
    for _,v in pairs(data) do
        if item_id == v.item_id then
            num = num+1
        end
    end
    return num
end

--获取概率配置
function AmuletData:GetShowExplainCfg(synthesis_type)
    return self.show_explain_cfg[synthesis_type]
end

--匹配护符背包index
function AmuletData:GetItemIndexByItemIdAndSkill(item)
    local list = BagData.Instance:GetPetEquipmentList()
    for _,v in pairs(list) do
        if v.item_id == item.item_id then
            local is_ok = true
            for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
                if v.param.equipment_skill_list[i] ~= item.param.equipment_skill_list[i] then
                    is_ok=false
                end
            end
            if is_ok then
                return v
            end
        end
    end
end

--获取已装备的装备的数量
function AmuletData:GetUsedNum(item_id)
    local list = PetData.Instance:GetPetEquipmentList()
    local num=0
    for _,v in pairs(list) do
        if v.item_id == item_id then
            num = num + 1
        end
    end
    return num
end


-- ===================== 护符强化 ===================

-- 护符强化功能是否开启
function AmuletData:IsOpenIntensify()
    local fly_level = FlyUpData.Instance:GetStage()
    return fly_level ~= FlyUpStageType.None
end


function AmuletData:SetAmuletIntensifyData(pos, item)
    if pos == 1 then
        self.intensify_data[2] = nil
    end
    self.intensify_data[pos] = item
end
function AmuletData:GetAmuletIntensifyData(pos)
    return self.intensify_data[pos] or {}
end
function AmuletData:ClearIntensifyData()
    self.intensify_data:Set({})
end

function AmuletData:GetItemSkillIdList(item)
    return item.param.equipment_skill_list or {}
end

function AmuletData:GetIntensifySkillCfg(skill_id)
    local cfg = Cfg.AmuletInstensifySkillCfg(skill_id, false)
    return cfg
end

function AmuletData:GetIntensifySkillLevel(skill_id)
    local cfg = self:GetIntensifySkillCfg(skill_id)
    if not cfg then
        return
    end
    return cfg.skill_level
end

-- 护符是否能强化
function AmuletData:IsCanIntensify(item)
    if not self:IsOpenIntensify() then
        return false
    end

    local skill_list = self:GetItemSkillIdList(item)
    for i,v in pairs(skill_list) do
        local cfg = self:GetIntensifySkillCfg(v)
        if cfg ~= nil and cfg.skill_level < AmuletData.MaxIntensifyLevel then
            return true
        end
    end
    return false
end

-- 护符是否能成为强化材料
function AmuletData:IsCanIntensifyStuff(item, type_list)
    local skill_list = self:GetItemSkillIdList(item)
    for i,v in pairs(skill_list) do
        local cfg = self:GetIntensifySkillCfg(v)
        if cfg ~= nil then
            for i,v in pairs(type_list) do
                if cfg.type == v then
                    return true
                end
            end
        end
    end
    return false
end


function AmuletData:GetIntensifySkillInfo()
    local a_item = self:GetAmuletIntensifyData(1)
    local b_item = self:GetAmuletIntensifyData(2)
    if TableIsEmpty(a_item) or TableIsEmpty(b_item) then
        return {}
    end
    local a_skill_list = self:GetItemSkillIdList(a_item)
    local b_skill_list = self:GetItemSkillIdList(b_item)
    local t = {}
    for i,v in pairs(a_skill_list) do
        local cfg = self:GetIntensifySkillCfg(v)
        if cfg then
            local info = {}
            info.cur_skill = {
                skill_id = v,
                skill_level = cfg.skill_level,
                is_fu_skill = true,
            }
            info.next_skill = {
                skill_id = v,
                skill_level = cfg.skill_level,
                is_fu_skill = true,
            }
            if cfg.skill_level < AmuletData.MaxIntensifyLevel then
                for _,v2 in pairs(b_skill_list) do
                    local cfg2 = self:GetIntensifySkillCfg(v2)
                    if cfg2 and cfg2.type == cfg.type then
                        local next_cfg = Cfg.AmuletInstensifySkillCfgBy2Key(cfg.type, cfg.skill_level + 1)
                        info.next_skill = {
                            skill_id = next_cfg.skill_id,
                            skill_level = next_cfg.skill_level,
                            is_fu_skill = true,
                        }
                        break
                    end
                end
            end
            table.insert(t, info)
        end
    end
    return t
end

function AmuletData:GetIntensifyStuffList(pos, t_item_id)
    t_item_id = t_item_id or 0
    pos = pos or 2
    local bag_item_list = self:GetAmuletsList(true)
    local list = {}
    if pos == 1 then
        local pet_equip_list = PetData.Instance:GetPetEquipmentList()
        for i,v in pairs(pet_equip_list) do
            if self:IsCanIntensify(v) and (t_item_id == 0 or v.item_id == t_item_id) then
                table.insert(list, v)
            end
        end
        for i,v in pairs(bag_item_list) do
            if self:IsCanIntensify(v) and (t_item_id == 0 or v.item_id == t_item_id) then
                table.insert(list, v)
            end
        end
    else
        local a_item = self:GetAmuletIntensifyData(1)
        if TableIsEmpty(a_item) then
            return {}
        end
        local type_list = {}
        local skill_list = self:GetItemSkillIdList(a_item)
        for i,v in pairs(skill_list) do
            local cfg = self:GetIntensifySkillCfg(v)
            if cfg then
                table.insert(type_list, cfg.type)
            end
        end
        for i,v in pairs(bag_item_list) do
            if v.index ~= a_item.index and self:IsCanIntensifyStuff(v, type_list) and (t_item_id == 0 or v.item_id == t_item_id) then
                table.insert(list, v)
            end
        end
    end
    return list
end


function AmuletData:IsNeedTip(item)
    local skill_list = self:GetItemSkillIdList(item)
    for i,v in pairs(skill_list) do
        local cfg = self:GetIntensifySkillCfg(v)
        if cfg and cfg.skill_level > 1 then
            return true
        end
    end
    return false
end