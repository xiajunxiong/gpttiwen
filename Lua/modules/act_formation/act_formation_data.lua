ActFormationData = ActFormationData or BaseClass()
ActFormationData.FormationType = {
    Empty = 0,
    MainRole = 1,
    Pet = 2,
    Partner = 3,
}

ACTFORMATION_STATE = {
    Empty = -1,
    Fight = 1,
    Assist = 2,
    StandBy = 3,
    Relax = 4,
}
--key是pos
ActFormationData.LockPosIndex = {
    [1] = {need_open_num = 3, oppo_pos = 6},
    [2] = {need_open_num = 1, oppo_pos = 7},
    [3] = {need_open_num = 0, oppo_pos = 8},
    [4] = {need_open_num = 2, oppo_pos = 9},
    [5] = {need_open_num = 4, oppo_pos = 10},
    [6] = {need_open_num = 3, oppo_pos = 1},
    [7] = {need_open_num = 1, oppo_pos = 2},
    [8] = {need_open_num = 0, oppo_pos = 3},
    [9] = {need_open_num = 2, oppo_pos = 4},
    [10] = {need_open_num = 4, oppo_pos = 5},
}
--key是解锁顺序
ActFormationData.AssistOpenList = {
    [1] = {pos = 7, oppo_pos = 2},
    [2] = {pos = 9, oppo_pos = 4},
    [3] = {pos = 6, oppo_pos = 1},
    [4] = {pos = 10, oppo_pos = 5},
}


ActFormationMode = {
    CloudArena = "CloudArena",
}

ActFormationData.EmptyFormationGrid = {id = 0, type = 0, unique_id_1 = 0, unique_id_2 = 0, pet_exclusive_skill_disable_flag = bit:d8b(0),lian_yao_exclusive_skill_disable_flag = bit:d8b(0)}
function ActFormationData:__init()
    if ActFormationData.Instance ~= nil then
        Debuger.LogError("[ActFormationData] attempt to create singleton twice!")
        return
    end
    ActFormationData.Instance = self

    self.formation_smart = SmartData.New({
        cur_is_pet = 0,         --0是伙伴 1是宠物   --下排标签页
        partner = 0,
        pet_index = -1,
        click_pet_card = false,
        pos_change = {
            arrow = 0,
            lights = {}
        },
        click_char = false,
        pos_changed = false,
        cur_select_state = -1,
        auto_save = false,
        skill_change = -1,
        cur_copet_index = -1,
    })
    self.cur_sel_skills = {
            SmartData.New({auto_seq = 1, detail = SmartData.New(), move_type = BATTLE_MOVE_TYPE.NONE}),
            SmartData.New({auto_seq = 2, detail = SmartData.New(), move_type = BATTLE_MOVE_TYPE.NONE}),
        }

    self.cur_auto_seq = 1
    self.base_skills = {
		[BATTLE_MOVE_TYPE.NORMAL_ATTACK] = {skill_id = BATTLE_MOVE_TYPE.NORMAL_ATTACK, icon_id=1, skill_name = Language.Main.Battle.Attack, move_type = BATTLE_MOVE_TYPE.NORMAL_ATTACK},
		[BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = {skill_id = BATTLE_MOVE_TYPE.NORMAL_DEFENCE, icon_id=31, skill_name = Language.Main.Battle.Defence, move_type = BATTLE_MOVE_TYPE.NORMAL_DEFENCE},
		[BATTLE_MOVE_TYPE.AI_SKILL] = {skill_id = BATTLE_MOVE_TYPE.AI_SKILL, icon_id=81, skill_name = Language.Main.Battle.AISkill, move_type = BATTLE_MOVE_TYPE.AI_SKILL},
	}

    self.cur_formation_mode = ""
    self.act_formation_info_list = {}
    self.cur_system_type = -1
    self.system_type_func_list = {}
    -- self.dan_change_data = {old_dan = 1, new_dan = 2}
end

function ActFormationData:__delete()
    ActFormationData.Instance = nil
end

function ActFormationData:ClearCurSkillSmart()
    for i = 1, 2 do
        self.cur_sel_skills[i].detail:Clear()
        self.cur_sel_skills[i].move_type = BATTLE_MOVE_TYPE.NONE
    end
end

function ActFormationData:ClearCacheFormationInfo()
    self.act_formation_info_list = {}
end
-- func_table = {
--     MainType = 各种接口注册的类，要求所以接口统一，例：MainType = CloudArenaCtrl.Instance
--     challenge_func = 对应活动内容的挑战接口 string
--     pet_on_limit_func = 获得对应活动可上阵“助战”宠物的数量 0~4，string
--     assist_text_func = 获得对应活动布阵界面左上角信息文字的接口， string
--     assist_limit_text_func = 获得对应活动布阵界面中间提示文字的接口， {string,string,string,string} 返回值可为空或{}
--     crystal_tip_func = 获得推荐灵珠，位于布阵界面右上角返回按钮下面，item_id (对应灵珠的物品id)
--     后续如有新增接口注册需求，联系wfc
-- }
function ActFormationData:RegistFunctions(system_type, func_table)
    self.system_type_func_list[system_type] = func_table
end

function ActFormationData:GetChallengeFunc()
    local funcs_t = self.system_type_func_list[self.cur_system_type]
    if funcs_t ~= nil then
        return funcs_t.MainType[funcs_t.challenge_func], funcs_t.MainType
    end
end

function ActFormationData:GetPetOnLimitNumFunc(system_type)
    local funcs_t = self.system_type_func_list[system_type and system_type or self.cur_system_type]
    if funcs_t ~= nil then
        return funcs_t.MainType[funcs_t.pet_on_limit_func]
    end
end

function ActFormationData:GetAssistText()
    local funcs_t = self.system_type_func_list[self.cur_system_type]
    if funcs_t ~= nil then
        local func = funcs_t.MainType[funcs_t.assist_text_func]
        local assist_num = self:GetAssistCount()
        if func then
            local assist_text = func(funcs_t.MainType, assist_num)
            if not StringIsEmpty(assist_text) then
                return assist_text
            end
        end
    end
end

function ActFormationData:GetCrystalTip()
    local funcs_t = self.system_type_func_list[self.cur_system_type]
    if funcs_t ~= nil then
        local func = funcs_t.MainType[funcs_t.crystal_tip_func]
        if func then
            local crystal_id = func(funcs_t.MainType)
            if not StringIsEmpty(crystal_id) then
                local item_cfg = Item.GetConfig(crystal_id)
                if item_cfg then
                    return string.format(Language.ActFormation.SuggestCrystal, CrystalNameColor[crystal_id], item_cfg.name), item_cfg.icon_id
                end
            end
        end
    end
    return ""
end

function ActFormationData:GetAssistLimistTextList()
    self.assist_limit_text_list = {}
    local funcs_t = self.system_type_func_list[self.cur_system_type]
    if funcs_t ~= nil then
        local func = funcs_t.MainType[funcs_t.assist_limit_text_func]
        if func then
            self.assist_limit_text_list = func(funcs_t.MainType) or {}
        end
    end
    -- LogError("assist_limit_text_list",self.assist_limit_text_list)
end

function ActFormationData:FlushAssistLimitTextList()
    local add_index = 1
    self.limit_text_list = {}
    local add_func = function(pos)
        self.limit_text_list[pos] = self.assist_limit_text_list[add_index]
        add_index = add_index + 1
    end
    local pet_on_open_index = false
    local start_idx = 1--self:GetAssistCount() >= self.pet_on_limit and 1 or self.pet_on_limit + 1
    for i = start_idx, #ActFormationData.AssistOpenList do
        local v = ActFormationData.AssistOpenList[i]
        if add_index > #self.assist_limit_text_list then
            break
        end
        local oppo_pos_data = self.formation_grid_list[v.oppo_pos]
        local pos_data = self.formation_grid_list[v.pos]
        if self:GetAssistCount() >= self.pet_on_limit then
            if pos_data.type == ActFormationData.FormationType.Empty and oppo_pos_data.type ~= ActFormationData.FormationType.Pet then
                add_func(v.pos)
            elseif pos_data.type ~= ActFormationData.FormationType.Pet and oppo_pos_data.type == ActFormationData.FormationType.Empty then
                add_func(v.oppo_pos)
            end
        else
            if i < self.pet_on_limit and pos_data.type == ActFormationData.FormationType.Pet then
                pet_on_open_index = true
            end
            if ActFormationData.LockPosIndex[v.pos].need_open_num + (pet_on_open_index and 0 or self:GetAssistCount()) > self.pet_on_limit then
                if pos_data.type == ActFormationData.FormationType.Empty and oppo_pos_data.type ~= ActFormationData.FormationType.Pet then
                    add_func(v.pos)
                elseif pos_data.type ~= ActFormationData.FormationType.Pet and oppo_pos_data.type == ActFormationData.FormationType.Empty then
                    add_func(v.oppo_pos)
                end
            end
        end
    end
    -- LogError("limit_text_list",self.limit_text_list)
end

function ActFormationData:GetAssistLimistText(pos)
    return self.limit_text_list and self.limit_text_list[pos] or nil
end

function ActFormationData:SetClickChar(click)
    --LogError("SetClickChar",click)
    self.formation_smart.click_char = click
    self:FlushAssistLimitTextList()
end

function ActFormationData:GetFormationInfoByType(system_type)
    return self.act_formation_info_list[system_type]
end

function ActFormationData:SetActFormationInfo(protocol)
    if IS_EDITOR then
        LogError("protocol",protocol.system_type, protocol.skill_list)
    end
    self.act_formation_info_list[protocol.system_type] = {grid_list = TableCopy(protocol.grid_list), standby_list = TableCopy(protocol.standby_list), skill_list = TableCopy(protocol.skill_list)}
end

function ActFormationData:SetCurFormationInfo(system_type)
    self.cur_system_type = system_type
    if system_type == PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COURAGE_CHALLENGE then
        self.formation_smart.cur_is_pet = 1
    end
    self.formation_origin_info = self.act_formation_info_list[self.cur_system_type]
    if self.formation_origin_info == nil then
        -- BuglyReport("ActFormationData SetCurFormationInfo origin_info of system_type = ", self.cur_system_type, " is nil")
        self.wait_for_formation_info = system_type
        ActFormationCtrl.Instance:SendGetFormationInfo(system_type)
        return false
    end
    self.formation_grid_list = TableCopy(self.formation_origin_info.grid_list)

    local pet_on_limit_func = self:GetPetOnLimitNumFunc()
    self.pet_on_limit = pet_on_limit_func and pet_on_limit_func() ~= nil and pet_on_limit_func() or 0   --可助阵宠物数量限制可为空默认为0即没有助战宠物
    self.partner_id_list = {}
    self.pet_on_list = {}
    self.main_pet_index = -1
    for k,v in pairs(self.formation_origin_info.grid_list) do
        if v.type == ActFormationData.FormationType.Partner then
            self.partner_id_list[v.id] = v
        elseif v.type == ActFormationData.FormationType.Pet then
            local index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if index ~= -1 then
                if k == 3 or k == 8 then
                    self.main_pet_index = index
                end
                self.pet_on_list[index] = v
            end
        end
    end
    self.standby_index_list = {}
    for k,v in pairs(self.formation_origin_info.standby_list) do
        if v.pet_id ~= 0 then
            local cur_pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if cur_pet_index == -1 then
                self.standby_index_list[cur_pet_index] = PetData.Instance:GetPetInfo(cur_pet_index)
            end
        end
    end
    self:FormationParse()
    for k,v in pairs(self.formation_origin_info.skill_list) do
        local detail = v.skill_id > 11 and SkillData.GetDecConfig(v.skill_id, v.skill_level) or self.base_skills[v.move_type == BATTLE_MOVE_TYPE.NONE and BATTLE_MOVE_TYPE.NORMAL_ATTACK or v.move_type]
        if k == 1 and (v.move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK or v.move_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE) then
            local fight_cur_first_skill_detail = FightData.Instance.cur_sel_skills[1].detail
            if fight_cur_first_skill_detail.skill_id and fight_cur_first_skill_detail.skill_id > 11 then
                detail = SkillData.GetDecConfig(fight_cur_first_skill_detail.skill_id, fight_cur_first_skill_detail.skill_level)
            end
        end
        if nil == detail then
            if IS_EDITOR then
                LogError(string.format("msg 2971 error skill_id:%s move_type:%s", v.skill_id, v.move_type))
            else
                BuglyReport(string.format("msg 2971 error skill_id:%s move_type:%s", v.skill_id, v.move_type))
            end
            detail = self.base_skills[BATTLE_MOVE_TYPE.NORMAL_ATTACK]
        end
        self.cur_sel_skills[k].move_type = v.move_type
        self.cur_sel_skills[k].detail:Set(detail)
    end
    -- LogError("setcurformation Info",system_type)
    return true
end

function ActFormationData:StandbyCount()
    local count = 0
    if self.standby_index_list then
        for k,v in pairs(self.standby_index_list) do
            count = count + 1
        end
    end
    return count
end

function ActFormationData:GetFormationInfo()
    local formation_info = {}
    formation_info.grid_list = self.formation_grid_list

    formation_info.standby_list = {}
    for k,v in pairs(self.standby_index_list) do
        table.insert(formation_info.standby_list,{pet_id = v:ID(), unique_id_1 = v.info.pet_unique_id1, unique_id_2 = v.info.pet_unique_id2})
    end
    for i=#formation_info.standby_list,4 do
        table.insert(formation_info.standby_list,{})
    end
    formation_info.system_type = self.cur_system_type
    formation_info.skill_list = {}
    for k,v in pairs(self.cur_sel_skills) do
        if v.detail.skill_id then
            if v.move_type ~= BATTLE_MOVE_TYPE.AI_SKILL and v.detail.skill_id > BATTLE_MOVE_TYPE.AI_SKILL then
                v.move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
            end
        elseif IS_EDITOR then
            LogError("ActFormationData skill_id is nil index is",k,"system_type is ",self.cur_system_type,"detail is",v.detail)
            v.detail.skill_id = BATTLE_MOVE_TYPE.NORMAL_ATTACK
            v.move_type = BATTLE_MOVE_TYPE.NORMAL_ATTACK
        else
            BuglyReport("ActFormationData skill_id is nil index is",k,"system_type is ",self.cur_system_type,"detail is",v.detail)
            v.detail.skill_id = BATTLE_MOVE_TYPE.NORMAL_ATTACK
            v.move_type = BATTLE_MOVE_TYPE.NORMAL_ATTACK
        end
        table.insert(formation_info.skill_list,{skill_id = v.detail.skill_id, skill_level = v.detail.skill_level, move_type = v.move_type})
    end
    return formation_info
end

function ActFormationData:CheckFormationChange()
    local origin = self.formation_origin_info
    for k,v in pairs(self.formation_grid_list) do
        if v.type ~= origin.grid_list[k].type
            or v.id ~= origin.grid_list[k].id
            or v.unique_id_1 ~= origin.grid_list[k].unique_id_1
            or v.unique_id_2 ~= origin.grid_list[k].unique_id_2 then
            return true
        end
    end
    for i =1, 2 do
        local cur_skill = self.cur_sel_skills[i]
        if cur_skill.detail.move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL and self.cur_skill.move_type == origin.skill_list[i].move_type then
            if cur_skill.detail.skill_id ~= origin.skill_list[i].skill_id then
                return true
            end
        elseif cur_skill.detail.move_type ~= origin.skill_list[i].move_type then
            return true
        end
    end
end

function ActFormationData:FormationInfo(pos)
    if pos then
        return self.formation_info[pos]
    end
    return self.formation_info
end

function ActFormationData:PartnerOnFight(partner_id)
    return self.partner_id_list and self.partner_id_list[partner_id]
end

function ActFormationData:PetOnFight(pet_index)
    return self.pet_on_list and self.pet_on_list[pet_index]
end

function ActFormationData:PetFormationStatus(index)
    if index == self.main_pet_index then
        --主战
        return PPStateSp.main, ACTFORMATION_STATE.Fight
    elseif self.standby_index_list and self.standby_index_list[index] then
        --待机
        return PPStateSp.dai, ACTFORMATION_STATE.StandBy
    elseif self.pet_on_list and self.pet_on_list[index] then
        --助战
        return PPStateSp.zhu, ACTFORMATION_STATE.Assist
    end
end

function ActFormationData:GetFightIndex()
    return self.main_pet_index
end

function ActFormationData:GetPartnerList()
    local function sort_lock(a,b)
        local av = a.vo
        local bv = b.vo
        if av.is_unlocked == bv.is_unlocked then
            if self:PartnerOnFight(a.id) and self:PartnerOnFight(b.id) or (self:PartnerOnFight(a.id) == nil and self:PartnerOnFight(b.id) == nil) then
                local arp = a.acti_red_point.val and 1 or 0
                local brp = b.acti_red_point.val and 1 or 0
                if arp == brp then
                    if a.partner_quality == b.partner_quality then
                        return a.id < b.id
                    end
                    return a.partner_quality > b.partner_quality
                end
                return arp > brp
            else
                return self:PartnerOnFight(a.id) ~= nil
            end
        end
        return av.is_unlocked > bv.is_unlocked
    end
    local list = {}
    for _, info in pairs(PartnerData.Instance.partner_list) do
        -- if self.partner_id_list == nil or self.partner_id_list[info.id] == nil then
            table.insert(list, info)
        -- end
    end
    table.sort(list, sort_lock)
    return list
end

function ActFormationData:GetCopetList()
    local pet_list = {}
    for _, value in ipairs(self:GetPetList()) do
        if value.index ~= self.main_pet_index and self:PetOnFight(value.index) then
            table.insert(pet_list, value)
        end
    end
    return pet_list
end

function ActFormationData:GetPetList()
    local pet_list = PetData.Instance:GetPetList()
    pet_list = table.rearrange(pet_list)
    local function sort_func(a,b)
        local order_a = 0
        local order_b = 0
        --排序优先级（出战>助战>待机>评分>等级>品质>强化等级>序号）
        if a.index == self.main_pet_index then
            order_a = order_a + 1000000
        elseif b.index == self.main_pet_index then
            order_b = order_b + 1000000
        elseif self:PetOnFight(a.index) or self:PetOnFight(b.index) then
            order_a = order_a + (self:PetOnFight(a.index) and 500000 or 0)
            order_b = order_b + (self:PetOnFight(b.index) and 500000 or 0)
        elseif self.standby_index_list[a.index] ~= nil or self.standby_index_list[b.index] ~= nil then
            order_a = order_a + (self.standby_index_list[a.index] ~= nil and 100000 or 0)
            order_b = order_b + (self.standby_index_list[b.index] ~= nil and 100000 or 0)
        end
        ---------------
        if a:Quality() > b:Quality() then
            order_a = order_a + 100
        elseif a:Quality() < b:Quality() then
            order_b = order_b + 100
        end
        if a:Level() > b:Level() then
            order_a = order_a + 1000
        elseif a:Level() < b:Level() then
            order_b = order_b + 1000
        end
        if a:GetScore() > b:GetScore() then
            order_a = order_a + 10000
        elseif a:GetScore() < b:GetScore() then
            order_b = order_b + 10000
        end
        if a:StrLevel() > b:StrLevel() then
            order_a = order_a + 10
        elseif a:StrLevel() < b:StrLevel() then
            order_b = order_b + 10
        end
        if a.index < b.index then
            order_a = order_a + 1
        elseif a.index > b.index then
            order_b = order_b + 1
        end
        return order_a > order_b
    end
    table.sort(pet_list, sort_func)
    return pet_list
end

function ActFormationData:RoleAndPartnerPosChangeLights(pos)
    local lights = {}
    if 3 == pos or 8 == pos then
        lights = {[3] = true, [8] = true}
    else
        local info_p = self.formation_grid_list[pos]
        local oppo_is_pet = self.formation_grid_list[pos > 5 and pos - 5 or pos + 5].type == ActFormationData.FormationType.Pet
        if info_p.type == ActFormationData.FormationType.Partner then
            for i = 1, 10 do
                if 3 ~= i and 8 ~= i then
                    local oppo = i > 5 and i - 5 or i + 5
                    if self.formation_grid_list[i].type ~= ActFormationData.FormationType.Pet or (i == pos or oppo == pos) or not oppo_is_pet then
                        lights[i] = ((self.formation_grid_list[oppo].type ~= ActFormationData.FormationType.Partner) or (i == pos or oppo == pos))
                    end
                end
            end
        end
    end
    return lights
end

function ActFormationData:PetPosChangeLights(pos)
    local lights = {}
    local click_pos_info = self.formation_grid_list[pos]
    local click_oppo_info = self.formation_grid_list[pos > 5 and pos - 5 or pos + 5]
    if click_pos_info.type == ActFormationData.FormationType.Pet then
        if pos == 3 or pos == 8 and not self.formation_smart.click_pet_card then
            self.formation_smart.cur_select_state = ACTFORMATION_STATE.Fight
            if self.pet_on_limit < 1 then
                return {[pos] = true,[pos == 3 and 8 or 3] = true}
            end
        end
        --点击助战位时
        if pos ~= 3 and pos ~= 8 then
            self.formation_smart.cur_select_state = ACTFORMATION_STATE.Assist
        end
        
        local assist_num = self:GetAssistCount()
        -- LogError("assist_num",assist_num, self.pet_on_limit,"pos",pos,"cur_select_state",self.formation_smart.cur_select_state)
        for i = 1, 10 do
            local oppo = i > 5 and i - 5 or i + 5
            local i_type = self.formation_grid_list[i].type
            local oppo_type = self.formation_grid_list[oppo].type
            --助战，当是点击角色卡时包括主战位
            -- if 3 ~= i and 8 ~= i or self.formation_smart.click_card then
            --     lights[i] = ((oppo_type ~= ActFormationData.FormationType.Pet and i_type ~= ActFormationData.FormationType.Partner)
            --                  or (i == pos or oppo == pos))
            -- elseif 3 == i or 8 == i then
            --     lights[i] = i_type ~= ActFormationData.FormationType.MainRole
			-- end

            --助战满
            if assist_num >= self.pet_on_limit then
                if self.formation_smart.cur_select_state == ACTFORMATION_STATE.Assist then
                    if oppo == pos then
                        lights[i] = true
                    elseif i_type == ActFormationData.FormationType.MainRole then--and oppo_type == ActFormationData.FormationType.Pet then
                        lights[i] = false
                    elseif i_type == ActFormationData.FormationType.Pet or oppo_type == ActFormationData.FormationType.MainRole then
                        lights[i] = true
                    elseif i_type == click_oppo_info.type and i_type ~= ActFormationData.FormationType.Empty then
                        lights[i] = false
                    elseif oppo_type ~= ActFormationData.FormationType.Pet and i_type ~= click_oppo_info.type then
                        lights[i] = true
                    else
                        lights[i] = oppo_type ~= ActFormationData.FormationType.MainRole and oppo_type ~= ActFormationData.FormationType.Pet and i_type == ActFormationData.FormationType.Empty
                            or (oppo_type == ActFormationData.FormationType.Empty and click_oppo_info.type == ActFormationData.FormationType.Empty)
                    end
                elseif self.formation_smart.cur_select_state == ACTFORMATION_STATE.Fight then
                    lights[i] = i == 3 or i == 8 or i_type == ActFormationData.FormationType.Pet
                end
            else                
                if pos == oppo then
                    lights[i] = true
                elseif i_type == ActFormationData.FormationType.MainRole then--and oppo_type == ActFormationData.FormationType.Pet then
                    lights[i] = false
                elseif i_type == click_oppo_info.type and i_type ~= ActFormationData.FormationType.Empty then
                    lights[i] = false
                else
                    lights[i] = (oppo_type ~= ActFormationData.FormationType.Pet and i_type ~= ActFormationData.FormationType.MainRole and i_type ~= ActFormationData.FormationType.Partner)
                        or (oppo_type == ActFormationData.FormationType.Empty and click_oppo_info.type == ActFormationData.FormationType.Empty)
                end
            end
        end
    end
    return lights
end

function ActFormationData:FormationParse()
    local detail = self.formation_grid_list
    if nil == detail then return end
    local formation_info = {}
    for k,v in pairs(detail) do
        if v.type == ActFormationData.FormationType.MainRole then
            formation_info[k] = FormationInfo.RoleParse(k)
        elseif v.type == ActFormationData.FormationType.Partner then
            formation_info[k] = FormationInfo.PartnerParse(v.id, k)
        elseif v.type == ActFormationData.FormationType.Pet then
            local pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            local pet = PetData.Instance:GetPetInfo(pet_index)
            if pet_index and pet_index ~= -1 and pet then
                formation_info[k] = FormationInfo.PetParse(pet_index, k)
            end
        end
    end
    self.formation_info = SmartData.New(formation_info)
end

function ActFormationData:PartnerAddBegin()
    local lights = self:PatnerAddLights()
    self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == ACTFORMATION_STATE.Empty and ACTFORMATION_STATE.Relax or self.formation_smart.cur_select_state
    self.formation_smart.pos_change = {
        arrow = 0,
        lights = lights,
    }
end

function ActFormationData:PetAddBegin()
    local lights = self:PetAddLights()
    if self.standby_index_list[self.formation_smart.pet_index] ~= nil then
        self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == ACTFORMATION_STATE.Empty and ACTFORMATION_STATE.StandBy or self.formation_smart.cur_select_state
    elseif self:PetOnFight(self.formation_smart.pet_index) then
        self.formation_smart.cur_select_state = self.main_pet_index == self.formation_smart.pet_index and ACTFORMATION_STATE.Fight or ACTFORMATION_STATE.Assist
    else
        self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == ACTFORMATION_STATE.Empty and ACTFORMATION_STATE.Relax or self.formation_smart.cur_select_state
    end

    local cur_pos = 0
    if self.pet_on_list[self.formation_smart.pet_index] ~= nil then
        for k,v in pairs(self.formation_info) do
            if v.pet and v.pet_index == self.formation_smart.pet_index then
                cur_pos = k
                break
            end
        end
    end
    if cur_pos == 3 or cur_pos == 8 then
        lights[3] = true
        lights[8] = true
    else
        lights[3] = self.formation_info[3] == nil or not self.formation_info[3].role
        lights[8] = self.formation_info[8] == nil or not self.formation_info[8].role
    end
    self.formation_smart.pos_change = {
        arrow = cur_pos,
        lights = lights,
    }
end

function ActFormationData:PatnerAddLights()
    local lights = {}
    for i = 1, 10 do
        if 3 ~= i and 8 ~= i then
            local oppo = i > 5 and i - 5 or i + 5
            local info_i = self.formation_info[i]
            lights[i] = (self.formation_info[oppo] == nil or not self.formation_info[oppo].partner) and (info_i == nil or info_i.partner)
        end
    end
    return lights
end

function ActFormationData:PetAddLights()
    local assist_num = self:GetAssistCount()
    local can_add = assist_num < self.pet_on_limit
    local lights = {}
        for i = 1, 10 do
            if i ~= 3 and i ~= 8 or self.formation_smart.click_pet_card then
                local oppo = i > 5 and i - 5 or i + 5
                local info_i = self.formation_info[i]
                if can_add then
                    lights[i] = (self.formation_info[oppo] == nil or not self.formation_info[oppo].pet) and (info_i == nil or info_i.pet)
                else
                    lights[i] = info_i ~= nil and info_i.pet
                end
            end
        end
    return lights
end

function ActFormationData:PartnerAddEnd(pos, partner)
    local pos_change = self.formation_smart.pos_change
    if pos_change.lights[pos] then
        self.formation_smart.partner = 0
        if self.formation_info[pos] ~= nil then
            self.partner_id_list[self.formation_info[pos].partner_id] = nil
        end
        self.formation_info[pos] = FormationInfo.PartnerParse(partner, pos)
        self.formation_grid_list[pos] = {id = partner, type = ActFormationData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0, pet_exclusive_skill_disable_flag = bit:d8b(0),lian_yao_exclusive_skill_disable_flag = bit:d8b(0)}
        self.partner_id_list[partner] = {id = partner, type = ActFormationData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0}

        self.formation_smart:SetDirty("pos_changed")
        self:PosChangeReset()
        self.formation_smart.cur_select_state = ACTFORMATION_STATE.Empty
    else
        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
    end
end

function ActFormationData:PetAddEnd(pos, pet_index)
    local pos_change = self.formation_smart.pos_change
    if pos_change.lights[pos] then
        self.formation_smart.pet_index = 0
        if self.formation_info[pos] ~= nil then
            self.pet_on_list[self.formation_info[pos].pet_index] = nil
        end
        local pet_info = PetData.Instance:GetPetInfo(pet_index)
        self.formation_info[pos] = FormationInfo.PetParse(pet_index, pos)
        self.formation_grid_list[pos] = {id = pet_info:ID(), type = ActFormationData.FormationType.Pet, unique_id_1 = pet_info.info.pet_unique_id1, unique_id_2 = pet_info.info.pet_unique_id2, pet_exclusive_skill_disable_flag = bit:d8b(0),lian_yao_exclusive_skill_disable_flag = bit:d8b(0)}
        self.pet_on_list[pet_index] = {id = pet_info:ID(), type = ActFormationData.FormationType.Pet, unique_id_1 = pet_info.info.pet_unique_id1, unique_id_2 = pet_info.info.pet_unique_id2, pet_exclusive_skill_disable_flag = bit:d8b(0),lian_yao_exclusive_skill_disable_flag = bit:d8b(0)}
        self.standby_index_list[pet_index] = nil
        local skill_dirty = false
        if pos == 3 or pos == 8 then
            if self.main_pet_index ~= pet_index then
                skill_dirty = true
            end
            self.main_pet_index = pet_index
        end
        self.formation_smart:SetDirty("pos_changed")
        if skill_dirty then
            if self.formation_smart.skill_change ~= -1 then
                self.formation_smart.skill_change = -1
            else
                self.formation_smart:SetDirty("skill_change")
            end
        end
        self:PosChangeReset()
        self.formation_smart.cur_select_state = ACTFORMATION_STATE.Empty
    else
        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
    end
end

function ActFormationData:PartnerChangePos(id)
    local pos = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == ActFormationData.FormationType.Partner and v.id == id then
            self.formation_smart.cur_select_state = ACTFORMATION_STATE.Fight
            pos = k
            break
        end
    end
    if pos ~= 0 then
        self:PosChange(pos)
    end
end

function ActFormationData:PetChangePos(pet_index)
    local pos = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == ActFormationData.FormationType.Pet then 
            local index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if index == pet_index then
                self.formation_smart.cur_select_state = (k == 3 or k == 8) and ACTFORMATION_STATE.Fight or ACTFORMATION_STATE.Assist
                pos = k
                break
            end
        end
    end
    if pos ~= 0 then
        self:PosChange(pos)
    end
end

function ActFormationData:PosChange(pos)
    local pos_change = self.formation_smart.pos_change
    if 0 == pos_change.arrow then
        if self.formation_info[pos] then
            local lights = self.formation_info[pos].pet and self:PetPosChangeLights(pos) or self:RoleAndPartnerPosChangeLights(pos)
            self.formation_smart.pos_change = {
                arrow = pos,
                lights = lights,
            }
        else
            if pos == 3 or pos == 8 then
                PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPetTips)
            else
                local oppo = pos > 5 and pos - 5 or pos + 5
                if self.formation_info[oppo] then
                    if self.formation_info[oppo].partner then
                        PublicPopupCtrl.Instance:Center(self:GetAssistLimistText(pos) or Language.Formation.SelNoPetTips)
                    elseif self.formation_info[oppo].pet then
                        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPartnerTips)
                    end
                else
                    PublicPopupCtrl.Instance:Center(self:GetAssistLimistText(pos) or (self.formation_smart.cur_is_pet == 1 and Language.Formation.SelNoPetTips or Language.Formation.SelNoPartnerTips))
                end
            end
        end
    elseif pos_change.arrow ~= pos then
        if pos_change.lights[pos] then
            local info = self.formation_info[pos]
            self.formation_info[pos] = self.formation_info[pos_change.arrow]
            self.formation_info[pos_change.arrow] = info
            if self.formation_info[pos] then self.formation_info[pos].pos = pos end
            if self.formation_info[pos_change.arrow] then self.formation_info[pos_change.arrow].pos = pos_change.arrow end

            local grid_info = self.formation_grid_list[pos]
            self.formation_grid_list[pos] = self.formation_grid_list[pos_change.arrow]
            self.formation_grid_list[pos_change.arrow] = grid_info
            local skill_dirty = false
            if self.formation_info[pos].pet then
                if pos == 3 or pos == 8 then
                    if self.main_pet_index ~= self.formation_info[pos].pet_index then
                        skill_dirty = true
                    end
                    self.main_pet_index = self.formation_info[pos].pet_index
                elseif pos_change.arrow == 3 or pos_change.arrow == 8 then
                    if self.formation_info[pos_change.arrow] == nil or self.main_pet_index ~= self.formation_info[pos_change.arrow].pet_index then
                        skill_dirty = true
                    end
                    self.main_pet_index = self.formation_info[pos_change.arrow] and self.formation_info[pos_change.arrow].pet and self.formation_info[pos_change.arrow].pet_index or -1
                end
            end
            self.formation_smart:SetDirty("pos_changed")
            if skill_dirty then
                if self.formation_smart.skill_change ~= -1 then
                    self.formation_smart.skill_change = -1
                else
                    self.formation_smart:SetDirty("skill_change")
                end
            end
            self:PosChangeReset()
        else
            PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
        end
    else
        self:PosChangeReset()
    end
end

function ActFormationData:PosChangeReset()
    self.formation_smart.pos_change = {
        arrow = 0,
        lights = {},
    }
    self.formation_smart.partner = 0
    self.formation_smart.pet_index = -1
    self.formation_smart.cur_select_state = -1
    self.formation_smart.click_pet_card = false
end

function ActFormationData:SetSelectPartner(id)
    self.formation_smart.cur_select_state = self:PartnerOnFight(id) and ACTFORMATION_STATE.Fight or ACTFORMATION_STATE.Relax
    if id == self.formation_smart.partner and id ~= 0 then
        self.formation_smart.partner = 0
        self:PosChangeReset()
    else
        self.formation_smart.partner = id
    end
end

function ActFormationData:SetSelectPet(index)
    local _, state = self:PetFormationStatus(index)
    self.formation_smart.cur_select_state = state ~= nil and state or ACTFORMATION_STATE.Relax
    if self.formation_smart.pet_index == index then
        self.formation_smart.pet_index = -1
        self:PosChangeReset()
    else
        self.formation_smart.pet_index = index
    end
end

function ActFormationData:SetSelectState(state)
    if state == ACTFORMATION_STATE.Relax or state == ACTFORMATION_STATE.StandBy then
        if state == ACTFORMATION_STATE.StandBy then
            local count_limit = self.main_pet_index == -1 or self.formation_smart.pet_index == self.main_pet_index
            if self:StandbyCount() >= (count_limit and 5 or 4) then
                PublicPopupCtrl.Instance:Center(string.format(Language.Pet.PetOnFightMax,5))
                self.formation_smart:SetDirty("cur_select_state")
                return
            end
        end
        local skill_dirty = false
        if self.formation_smart.partner > 0 or self.formation_smart.pet_index then
            for k,v in pairs(self.formation_info) do
                if v.pet and v.pet_index == self.formation_smart.pet_index or v.partner and v.partner_id == self.formation_smart.partner then
                    if v.partner then
                        self.partner_id_list[v.partner_id] = nil
                    elseif v.pet then
                        if k == 3 or k == 8 then
                            skill_dirty = true
                            self.main_pet_index = -1
                        end
                        self.pet_on_list[v.pet_index] = nil
                    end
                    self.formation_info[k] = nil
                    self.formation_grid_list[k] = ActFormationData.EmptyFormationGrid
                end
            end
            self.standby_index_list[self.formation_smart.pet_index] = state == ACTFORMATION_STATE.StandBy and PetData.Instance:GetPetInfo(self.formation_smart.pet_index) or nil
        end
        self.formation_smart.pos_change = {
            arrow = 0,
            lights = {},
        }
        -- local need_flush_model = self.formation_smart.cur_select_state ~= Relax
        self.formation_smart.cur_select_state = state
        if skill_dirty then
            if self.formation_smart.skill_change ~= -1 then
                self.formation_smart.skill_change = -1
            else
                self.formation_smart:SetDirty("skill_change")
            end
        end
        -- if need_flush_model then
            self.formation_smart:SetDirty("pos_changed")
        -- end
    elseif state == ACTFORMATION_STATE.Assist then
        if self.formation_smart.cur_select_state ~= ACTFORMATION_STATE.Assist then
            --顺序不可调换----------------------------------------
            self.formation_smart.click_pet_card = false             --
            self.formation_smart.cur_select_state = ACTFORMATION_STATE.Assist--
            self:PetAddBegin()                                  --
            ------------------------------------------------------
        end
    elseif state == ACTFORMATION_STATE.Fight then
        if self.formation_smart.cur_select_state ~= ACTFORMATION_STATE.Fight then
            self.formation_smart.click_pet_card = false
            if self.formation_smart.cur_is_pet == 0 then
                --顺序不可调换----------------------------------------
                self:PartnerAddBegin()                              --
                self.formation_smart.cur_select_state = ACTFORMATION_STATE.Fight --
                ------------------------------------------------------
            else
                self.formation_smart.cur_select_state = ACTFORMATION_STATE.Fight
                local pos = 8
                if self.formation_info[3] == nil or self.formation_info[3].pet then
                    pos = 3
                end
                if self:PetOnFight(self.formation_smart.pet_index) then
                    self:PetAddBegin()
                    self:PosChange(pos)
                else
                    self:PetAddBegin()
                    self:PetAddEnd(pos, self.formation_smart.pet_index)
                end
            end
            
        end
    end
end

function ActFormationData:GetAssistCount()
    local count = 0
    for k,v in pairs(self.formation_info or {}) do
        if k ~= 3 and k ~= 8 then
            if v.pet then
                count = count + 1
            end
        end
    end
    return count
end

function ActFormationData:GetHasEmptyAssist(system_type, assist_num)
    local formation_info = self:GetFormationInfoByType(system_type)
    if assist_num == nil then
        local pet_on_limit_func = self:GetPetOnLimitNumFunc(system_type)
        if pet_on_limit_func then
            assist_num = pet_on_limit_func() or 0
        end
    end
    if formation_info and formation_info.grid_list then
        local index_list = {}
        local assist_count = 0
        for i = 1, 10 do
            if i ~= 3 and i ~= 8 then
                local v = formation_info.grid_list[i]
                if v.id > 0 and v.unique_id_1 ~= 0 then
                    local pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
                    if pet_index ~= nil and pet_index >= 0 then
                        if index_list[pet_index] == nil then
                            index_list[pet_index] = pet_index
                            assist_count = assist_count + 1
                        end
                    end
                end
            end
        end
        return assist_count < assist_num
    else
        return false
    end
end

function ActFormationData:CheckHasEmptyPosCanUse()
    local empty_pos_count = 0
    local pet_on_count = 0
    local partner_on_count = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == ActFormationData.FormationType.Pet then
            pet_on_count = pet_on_count + 1
        elseif v.type == ActFormationData.FormationType.Partner then
            partner_on_count = partner_on_count + 1
        elseif v.type ~= ActFormationData.FormationType.MainRole then
            empty_pos_count = empty_pos_count + 1
        end
    end
    if empty_pos_count > 0 then
        if partner_on_count < PartnerData.Instance:GetUnlockedPartnerNum() and partner_on_count < 4 then
            return true
        elseif pet_on_count < table.nums(PetData.Instance:GetPetList()) and pet_on_count < self.pet_on_limit + 1 then
            return true
        end
    end
end

function ActFormationData:GetCurSelSkills()
    local role_skill_list = FightData.Instance:GetRoleSkillList()

    local role_change = true
    if next(role_skill_list.skills) then
        --角色转职后防止出现之前职业的技能
        if self.cur_sel_skills[1].move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
            for _,v in pairs(role_skill_list.skills) do
                if self.cur_sel_skills[1].detail:Val() ~= nil and self.cur_sel_skills[1].detail.skill_id == v.skill_id then
                    role_change = false
                end
            end
        else
            role_change = false
        end
        if role_change then
            -- self.cur_sel_skills[1].detail:Set(role_skill_list.skills[1].skill_cfg and role_skill_list.skills[1].skill_cfg or role_skill_list.skills[1])
        end
    else
        self.cur_sel_skills[1].detail:Set(self.base_skills[BATTLE_MOVE_TYPE.NORMAL_ATTACK])
    end
    -- LogError("role_change",self.cur_sel_skills[1].detail.skill_id,role_change)
    
    local pet_skill_data = self:GetPetSkillList()
    if next(pet_skill_data.skills) then
        local pet_changed = true
        if self.cur_sel_skills[2].detail.skill_id >= 11 then
            for k,v in pairs(pet_skill_data.skills) do
                if self.cur_sel_skills[2].detail.skill_id == v.skill_id then
                    pet_changed = false
                end
            end
            -- LogError("skill_change",self.formation_smart.skill_change)
            if self.formation_smart.skill_change == -1 and pet_changed then
                local skill_cfg = pet_skill_data.skills[1].skill_cfg
                self.cur_sel_skills[2].detail:Set(skill_cfg)
                self.cur_sel_skills[2].move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
            end
        end
    else
        self.cur_sel_skills[2].detail:Set(self.base_skills[BATTLE_MOVE_TYPE.NORMAL_ATTACK])
        self.cur_sel_skills[2].move_type = BATTLE_MOVE_TYPE.NORMAL_ATTACK
    end
    return self.cur_sel_skills
end

function ActFormationData:GetSkillList()
	local show_pet_icon = self.main_pet_index ~= -1
	if show_pet_icon then
		if 1 == self.cur_auto_seq then
			return {FightData.Instance:GetRoleSkillList(), self:GetBaseSkillList()}
		else
			return {self:GetPetSkillList(), self:GetBaseSkillList(true)}
		end
	else
		if self.cur_sel_skills[1].detail.skill_id > 0 then
			if 1 == self.cur_auto_seq then
				return {FightData.Instance:GetRoleSkillList(), self:GetBaseSkillList()}
			else
				return {self:GetBaseSkillList()}
			end
		elseif self.cur_sel_skills[2].detail.skill_id > 0 then
			if 2 == self.cur_auto_seq then
				return {FightData.Instance:GetRoleSkillList(), self:GetBaseSkillList()}
			else
				return {self:GetBaseSkillList()}
			end
		else
			return {FightData.Instance:GetRoleSkillList(), self:GetBaseSkillList()}
		end
	end
end

function ActFormationData:GetBaseSkillList(is_pet)
	local skill_list = {}
	for k, v in pairs(self.base_skills) do
		-- if BATTLE_MOVE_TYPE.CHANGE_POSITION ~= k and BATTLE_MOVE_TYPE.RUN_AWAY ~= k then
			v.is_base = true
			-- 宠物不能智能施法
			if not is_pet or BATTLE_MOVE_TYPE.AI_SKILL ~= v.skill_id then
				table.insert(skill_list, v)
			end
		-- end
	end
	self.SortSkill(skill_list)
	return {skills = skill_list, type = 2}
end

function ActFormationData:GetPetSkillList()
	local skill_list = {}
    if self.main_pet_index == -1 then
        return {skills = skill_list, type = 3}
    end

	local fight_pet = PetData.Instance:GetPetInfo(self.main_pet_index)
	if nil ~= fight_pet then
		local pet_skill_data = fight_pet:HadSkillList()
		for k, v in pairs(pet_skill_data) do
			v.name = v.skill_cfg.skill_name
			local skill_type = SkillData.GetSkillType(v.skill_id)
			-- 筛除宠物被动技能
			if 7 ~= skill_type and 8 ~= skill_type then
				table.insert(skill_list,v)
			end
		end
		-- for k, v in pairs(self.base_skills) do
		-- 	-- if BATTLE_MOVE_TYPE.CHANGE_POSITION ~= k and BATTLE_MOVE_TYPE.RUN_AWAY ~= k then
		-- 		v.is_base = true
		-- 		table.insert(skill_list, v)
		-- 	-- end
		-- end
		self.SortSkill(skill_list)
	end
	return {skills = skill_list, type = 3}
end

function ActFormationData.SortSkill(list)
	table.sort(list, function(a, b)
		if not a or not b then return a end
        local as = a.is_base and 1 or 0
		local bs = b.is_base and 1 or 0
		if as == bs then return a.skill_id < b.skill_id end
        return as < bs
	end)
end

function ActFormationData:SetCurCopetInfo(data)
    self.cur_copet = data
    self.formation_smart.cur_copet_index = data.index
end

function ActFormationData:GetCurCopetMainSkillList()
    if self.formation_smart.cur_copet_index > -1 then
        local skill_list = self.cur_copet:GetMainSkillList({})
        local copet_main_skill_list = {}
        local main_skill = bit:d8b(0)
        local lianyao_skill = bit:d8b(0)
        if self:PetOnFight(self.formation_smart.cur_copet_index) then
            main_skill = self.pet_on_list[self.formation_smart.cur_copet_index].pet_exclusive_skill_disable_flag
            lianyao_skill = self.pet_on_list[self.formation_smart.cur_copet_index].lian_yao_exclusive_skill_disable_flag
        end
        local exclusive_num = 0
        local lian_yao_num = 0
        for k, v in ipairs(skill_list) do
            local skill_type = SkillData.GetSkillType(v.skill_id)
			-- 筛除宠物被动技能
			if 7 ~= skill_type and 8 ~= skill_type then
                if not v.lian_yao_skill then
                    table.insert(copet_main_skill_list,{gift_info = v, is_forbid = main_skill[#main_skill - exclusive_num], main_skill_num = exclusive_num})
                    exclusive_num = exclusive_num + 1
                else
                    table.insert(copet_main_skill_list,{gift_info = v, is_forbid = lianyao_skill[#lianyao_skill - lian_yao_num], main_skill_num = lian_yao_num})
                    lian_yao_num = lian_yao_num + 1
                end
            end
        end
        return copet_main_skill_list
    end
end

function ActFormationData:SetCopetSkillPermission(data)
    if not data.gift_info.lian_yao_skill then
        local pet_exclusive = self.pet_on_list[self.formation_smart.cur_copet_index].pet_exclusive_skill_disable_flag
        pet_exclusive[#pet_exclusive - data.main_skill_num] = data.is_forbid
        self.pet_on_list[self.formation_smart.cur_copet_index].pet_exclusive_skill_disable_flag = pet_exclusive
    else
        local lianyao_exclusive = self.pet_on_list[self.formation_smart.cur_copet_index].lian_yao_exclusive_skill_disable_flag
        lianyao_exclusive[#lianyao_exclusive - data.main_skill_num] = data.is_forbid
        self.pet_on_list[self.formation_smart.cur_copet_index].lian_yao_exclusive_skill_disable_flag = lianyao_exclusive
    end
    for k, v in pairs(self.formation_grid_list) do
        if v.type == ActFormationData.FormationType.Pet and self.formation_smart.cur_copet_index == PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2) then
           self.formation_grid_list[k] = self.pet_on_list[self.formation_smart.cur_copet_index]
        end
    end
end