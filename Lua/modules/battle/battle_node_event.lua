BattleNodeEvent =
    BattleNodeEvent or
    {
        displayer = nil,
        detail_player = nil,
        param_stacks = {},
        --key
        behit_trigger_stack = {},
        not_at_pos = {},--存储是否不在位置上
        spread_skill = {},--扩散技能相同扩散组需要同时释放
        spread_skill_hit_trigger = nil,--扩散技能触发器
        daxiong_skill_hit_trigger = nil,--大凶触发器
    }

local ParamKey = {
    Atker = 1, --攻击者
    CombineBehit = 2, --合击被击者id
    CombineCount = 3, --合击计数器
    NotAtPosition = 4, --正在自己位置外的角色
    Guarder = 5, --正在帮别人护卫的角色idx
    BeGuarer = 6, --正在被别人护卫的角色idx
    BehiterIdxs = 7, --被命中的角色idx(int[])
    ComboSkillId = 8, --连击技能ID
    SkillId = 9, --技能ID
    TargetList = 10, --技能目标列表
    EventType = 11, --事件类型
    CombineLastAtker = 12, --合击最后一个攻击者idx
    SummonReason = 13,
    -- TriggerList = 14,
    TotalDamage = 15, --我方造成的总伤害
    Hitter = 16, --Trigger触发者(攻击事件)
    ComboIdx = 17, --连击者
    SummonMutiple = 18,--是否召唤多个
    -- AtkBeh = 19, --攻击事件
    BuffTargets = 20,--buff对象
    LinkTarget = 21,--链接对象
    AtkBacker = 22,--反击者
    ShadowIdxs = 23,--影子
    HallowSkill = 24,--圣器技能相关信息
    FairyDragonSkill = 25,  --s神龙技能相关信息
}
local function pushDesc(eve_type, desc)
    if BattleNodeEvent.detail_player then
        BattleNodeEvent.detail_player:PushDesc(eve_type, desc)
    end
end

--battle_lua_function 某些情况下由逻辑抛出触发点
function BattleNodeEvent.GenSkillEventTriggerIdx(skill_id, atker, event_type)
    return skill_id * 10000 + atker * 100 + event_type
end

function BattleNodeEvent.pushParam(key, value)
    local stack = BattleNodeEvent.param_stacks[key]
    if stack == nil then
        stack = {}
        BattleNodeEvent.param_stacks[key] = stack
    end
    table.insert(stack, value)
end

function BattleNodeEvent.popParam(key)
    local stack = BattleNodeEvent.param_stacks[key]
    if stack == nil then
        return nil
    end
    local count = #stack
    if count <= 0 then
        return nil
    end
    local re = stack[count]
    table.remove(stack, count)
    return re
end

function BattleNodeEvent.getParam(key)
    local stack = BattleNodeEvent.param_stacks[key]
    if stack == nil then
        return nil
    end
    local count = #stack
    if count <= 0 then
        return nil
    end
    return stack[count]
end

function BattleNodeEvent.setParam(key, value)
    local stack = BattleNodeEvent.param_stacks[key]
    if stack == nil then
        return nil
    end
    local count = #stack
    if count <= 0 then
        return nil
    end
    stack[count] = value
end

function BattleNodeEvent.pushHitTrigger(hit_idx, behit_idx)
    local trigger = BattleBehitTrigger.New(hit_idx, behit_idx)
    table.insert(BattleNodeEvent.behit_trigger_stack, trigger)
    return trigger
end

function BattleNodeEvent.getHitTrigger(hit_idx, behit_idx)
    for i = #BattleNodeEvent.behit_trigger_stack, 1, -1 do
        local tr = BattleNodeEvent.behit_trigger_stack[i]
        if tr.hit_idx == hit_idx and tr.behit_idx == behit_idx then
            return tr
        end
    end
    return nil
end

function BattleNodeEvent.popHitTrigger(hit_idx, behit_idx)
    for i = #BattleNodeEvent.behit_trigger_stack, 1, -1 do
        local tr = BattleNodeEvent.behit_trigger_stack[i]
        if tr.hit_idx == hit_idx and tr.behit_idx == behit_idx then
            table.remove(BattleNodeEvent.behit_trigger_stack, i)
            return tr
        end
    end
    return nil
end

function BattleNodeEvent.checkNeedMove(grid_idx, eve_data)
    local need_move = false
    local ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    local prof_info = nil
    if ch_info then
        if ch_info.character_type == BATTLE_CH_TYPE.PARTNER or ch_info.character_type == BATTLE_CH_TYPE.PARTNER_ROBOT then
            prof_info = RoleData.Instance:GetProfessionDesc(ch_info.prof, ch_info.prof > 100)
        else
            prof_info = RoleData.Instance:GetProfessionDesc(ch_info.prof, true)
        end
    end
    if eve_data.skill_id >= 100 then
        local skill_cfg = SkillData.GetConfig(eve_data.skill_id)
        if skill_cfg ~= nil and BattleHelper.SKILL_GUARD[skill_cfg.asset_id_2] then
            return need_move
        end
        if skill_cfg ~= nil then
            if skill_cfg.is_close_range_skill == 1 then
                need_move = true
            elseif skill_cfg.is_close_range_skill == 0 then
                need_move = false
            elseif skill_cfg.is_close_range_skill == 2 then -- 均为近战只是弓箭特殊
                need_move = true
                if ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then --怪物读表
                    local cfg = Cfg.MonsterById(ch_info.character_id) or {}
                    need_move = cfg.is_long_range == 0
                end
                if prof_info ~= nil and prof_info.job_type == RoleProfessionType.ShenSheShou then
                    need_move = false
                end
            end
        end
        --冲刺类不需要额外移动
        if need_move and BattleHelper.CHONGCHI_SKILL[skill_cfg.asset_id_2] then
            need_move = false
        end
    else
        --普攻
        if #eve_data.target_list == 1 and eve_data.target_list[1] ~= grid_idx then
            need_move = true
            if ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then --怪物读表
                local cfg = Cfg.MonsterById(ch_info.character_id) or {}
                need_move = cfg.is_long_range == 0
            else
                if prof_info ~= nil and prof_info.job_type == RoleProfessionType.ShenSheShou then
                    --增加武器判断
                    need_move = false
                    if ch_info and ch_info.character_type == BATTLE_CH_TYPE.ROLE and ch_info.weapon_id <= 0 then
                        need_move = true
                    end
                end
            end
        end
    end
    --自己打自己
    if eve_data.target_list and #eve_data.target_list == 1 and eve_data.target_list[1] == grid_idx then
        need_move = false
    end
    return need_move
end

-- [1] == 事件信息解析，[2] == 行为添加 , [3] == 开始处理 , [4] == 结束处理, [5] == 开始处理后处理 , [6] == 结束处理后处理
local config = {
    [BATTLE_DATA_EVENT.INVALID] = {"parseDataEmpty", "setDataEmpty"},
    [BATTLE_DATA_EVENT.PERFORM_SKILL] = {"parsePerformSkill", "setDataPerformSkill", "setDataPerformSkillBegin", "setDataPerformSkillEnd"},
    [BATTLE_DATA_EVENT.DODGE] = {"parseDataEmpty", "setDataDodge"},
    [BATTLE_DATA_EVENT.MODIFY_HP] = {"parseModifyHp", "setDataModifyHp"},
    [BATTLE_DATA_EVENT.COMBINE_ATTACK] = {"parseCombineAttack", "setDataCombineAttack", "setDataCombineAttackBegin", "setDataCombineAttackEnd"},
    [BATTLE_DATA_EVENT.RUN_AWAY] = {"parseRunAway", "setDataRunAway"},
    [BATTLE_DATA_EVENT.REPLACE_BUFF] = {"parseReplaceBuff", "setDataReplaceBuff"},
    [BATTLE_DATA_EVENT.MERGE_BUFF] = {"parseMergeBuff", "setDataMergeBuff"},
    [BATTLE_DATA_EVENT.ADD_BUFF_TO_LIST] = {"parseAddBuffToList", "setDataAddBuffToList", [6] = "setDataAddBuffToListAfterEnd"},
    [BATTLE_DATA_EVENT.REMOVE_BUFF] = {"parseRemoveBuff", "setDataRemoveBuff",[4] = "setDataRemoveBuffEnd", [6] = "setDataRemoveBuffAfterEnd"},
    [BATTLE_DATA_EVENT.SELF_DESTRUCT] = {"parseDataSelfDestruct", "setDataSelfDestruct", "setDataSelfDestructBegin", "setDataSelfDestructEnd"},
    [BATTLE_DATA_EVENT.CHARACTER_DIE] = {"parseCharacterDie", "setDataCharacterDie"},
    [BATTLE_DATA_EVENT.COUNTER_ATTACK] = {"parseCounterAttack", "setDataCounterAttack", nil, "setDataCounterAttackEnd"},
    [BATTLE_DATA_EVENT.BE_GUARD] = {"parseBeGuard", "setDataBeGuard", "setDataBeGuardBegin", "setDataBeGuardEnd", "setDataBeGuardPreBegin"},
    [BATTLE_DATA_EVENT.CHANGE_POSITION] = {"parseDataEmpty", "setDataChangePosition"},
    [BATTLE_DATA_EVENT.USE_ITEM] = {"parseUseItem", "setDataUseItem", "setDataUseItemBegin", "setDataUseItemEnd"},
    [BATTLE_DATA_EVENT.CHANGE_EQUIPMENT] = {"parseChangeEquipment", "setDataChangeEquipment"},

    [BATTLE_DATA_EVENT.ROUND_END] = {nil, "setRoundEnd", nil, nil},
    [BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER] = {"parseDataSummonCharacter", "setDataSummonCharacter"},
    [BATTLE_DATA_EVENT.SUMMON] = {"parseDataSummon", "setdataSummon", nil, "setDataSummonEnd"},
    [BATTLE_DATA_EVENT.RETRIEVE_PET] = {"parseDataRetrievePet", "setDataRetrievePet"},
    [BATTLE_DATA_EVENT.COMBINE_ATTACK_PERFORM_ONE_HIT] = {"parseDataEmpty", "setDataCombineAttackPerformOneHit", nil, "setDataCombineAttackPerformOneHitEnd"},
    [BATTLE_DATA_EVENT.MODIFY_MP] = {"parseModifyMp", "setDataModifyMp"},
    [BATTLE_DATA_EVENT.COMBINE_ATTACK_TOTAL] = {"parseCombineAttackTotal", "setDataCombineAttackTotal"},
    [BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL] = {"parseComboSkill", "setDataComboSkill", "setDataComboSkillBegin", "setDataComboSkillEnd", nil},
    [BATTLE_DATA_EVENT.COMBO_SKILL_ONE_HIT] = {"parseComboSkillOneHit", "setComboSkillOneHit", "setComboSkillOneHitBegin", "setComboSkillOneHitEnd"},
    [BATTLE_DATA_EVENT.SEAL_PET] = {"parseSealPet", "setDataSealPet", nil, nil},
    [BATTLE_DATA_EVENT.USE_FABAO] = {"parseUseFabao", "setDataUseFabao", nil, "setUseFabaoEnd"},
    [BATTLE_DATA_EVENT.ELEMENT_CHANGE] = {"parseElementChange", "setDataElementChange", nil, nil},
    [BATTLE_DATA_EVENT.VANISH] = {nil, "setVanish", nil, nil},

    [BATTLE_DATA_EVENT.SKILL_PRE_EFFECT] = {"parseDataEmpty", "setDataSkillPreEffect", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_POST_EFFECT] = {"parseDataEmpty", "setDataSkillPostEffect", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_EFFECT] = {"parseDataEmpty", "setDataSkillEffect", "setDataSkillEffectEnd", nil},

    [BATTLE_DATA_EVENT.ABSORB_DAMAGE] = {"parseAbsorbDamage", "setDataAbsorbDamage", "setDataAbsorbDamageEnd", nil},
    [BATTLE_DATA_EVENT.SKILL_SPECIAL_EFFECT] = {"parseSkillSpecialEffect", "setDataSkillSpecialEffect", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_HIT] = {"parseSkillHit", "setDataSkillHit", nil, nil},
    [BATTLE_DATA_EVENT.IN_VAIN] = {"parseDataEmpty", "setDataInVain", nil, nil},
    [BATTLE_DATA_EVENT.TALKING] = {"parseTalking", "setDataTalking", nil, nil},
    [BATTLE_DATA_EVENT.SHOT_EFFECT] = {"parseShotEffect", "setDataShotEffect", nil, nil},
    [BATTLE_DATA_EVENT.CLIENT_GUIDE] = {"parseClientGuide", "setDataClientGuide", nil, nil},
    [BATTLE_DATA_EVENT.MONSTER_TRANSFORM] = {"parseMonsterTransform", "setDataMonsterTransform", nil, nil},
    [BATTLE_DATA_EVENT.BUFF_MISS] = {"parseBuffMiss", "setDataBuffMiss", nil, nil},
    [BATTLE_DATA_EVENT.USE_MPITEM] = {"parseUseMpItem", "setDataUseMpItem", nil, nil},
    [BATTLE_DATA_EVENT.BUFF_SUSTAIN_NUM_CHANG] = {"parseSustainNumChange", "setDataSustainNumChange", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_PERFORM_FAIL] = {"parseSkillPerformFail", "setDataSkillPerformFail", nil, nil},
    [BATTLE_DATA_EVENT.MODIFY_RAGE_POINTS] = {"parseModifyRagePoints", "setDataModifyRagePoints", nil, nil},
    [BATTLE_DATA_EVENT.NOTICE_NUM] = {"parseNoticeNum", "setDataNoticeNum", nil, nil},
    [BATTLE_DATA_EVENT.CLIENT_CUSTOM] = {"parseClientCustom", "setDataClientCustom", nil, nil},
    [BATTLE_DATA_EVENT.SIDE_SHOW_HP] = {"parseSideShowHp", "setDataSideShowHp", nil, nil},
    [BATTLE_DATA_EVENT.FABAO_INFO_CHANGE] = {"parseFabaoInfoChange","setDataFabaoInfoChange",nil,nil},
    [BATTLE_DATA_EVENT.ADD_HALO] = {"parseAddHalo", "setDataAddHalo", nil, nil},
    [BATTLE_DATA_EVENT.REMOVE_HALO] = {"parseRemoveHalo", "setDataRemoveHalo", nil, nil},
    [BATTLE_DATA_EVENT.HALO_CHANGE_POSITION] = {"parseHaloChangePosition", "setDataHaloChangePosition", nil, nil},
    [BATTLE_DATA_EVENT.IMPRISON_PERFORM_FAILED] = {"parseImprisonPerformFailed", "setDataImprisonPerformFailed", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_REMAIN_CAST_TIMES] = {"parseSkillRemainCastTimes", "setDataSkillRemainCastTimes", nil, nil},
    [BATTLE_DATA_EVENT.HEART_SKILL_INFO_CHANGE] = {"parseHeartSkillInfoChange", "setDataHeartSkillInfoChang", nil, nil},
    [BATTLE_DATA_EVENT.LINK_TOGETHER] = {"parseLinkTogether", "setDataLinkTogether", nil, "setDataLinkTogetherEnd"},
    [BATTLE_DATA_EVENT.LINK_BREAK] = {"parseLinkBreak", "setDataLinkBreak", nil, nil},
    [BATTLE_DATA_EVENT.SKILL_INSTANT_KILL] = {"parseSkillInstantKill", "setDataSkillInstantKill", nil, nil},
    [BATTLE_DATA_EVENT.CHARACTER_SPECIAL_EFFECT] = {"parseCharacterSpecialEffect", "setDataCharacterSpecialEffect", nil, nil},
    [BATTLE_DATA_EVENT.ADD_SINGLE_SP] = {"parseAddSingleSpecialEffect", "setDataAddSingleSpecialEffect", nil, nil},
    [BATTLE_DATA_EVENT.REMOVE_SINGLE_SP] = {"parseRemoveSingleSpecialEffect", "setDataRemoveSingleSpecialEffect", nil, nil},
    [BATTLE_DATA_EVENT.SPIRIT_POINT_CHANGE] = {"parseSpiritPointChange", "setDataSpiritPointChange", nil, nil},
    [BATTLE_DATA_EVENT.SEED_EXPLODE] = {"parseSeedExplode", "setDataSeedExplode", nil, nil},
    [BATTLE_DATA_EVENT.SPECIAL_EFFECT_TARGET] = {"parseSpecialEffectTarget", "setDataSpecialEffectTarget", nil, "setDataSpecialEffectTargetEnd"},
    [BATTLE_DATA_EVENT.SPREAD_SKILL] = {"parseSpreadSkill", "setDataSpreadSkill", nil, "setDataSpreadSkillEnd"},
    [BATTLE_DATA_EVENT.HALLOW_PERFORM_SKILL] = {"parseHallowSkill", "setDataHallowSkill", nil, "setDataHallowSkillEnd"},
    [BATTLE_DATA_EVENT.HALLOW_SKILL_HIT] = {"parseHallowSkillHit", "setDataHallowSkillHit", nil, "setDataHallowSkillHitEnd"},
    [BATTLE_DATA_EVENT.HALLOW_SKILL_EFFECT] = {nil, "setDataHallowSkillEffect", nil, "setDataHallowSkillEffectEnd"},
    [BATTLE_DATA_EVENT.HALLOW_FLY_AWAY] = {"parseHallowFlyAway", "setDataHallowFlyAway", nil, "setDataHallowFlyAwayEnd"},
    [BATTLE_DATA_EVENT.HALLOW_FLY_WORDS] = {"parseHallowFlyWords", "setDataHallowFlyWords", nil, "setDataHallowFlyWordsEnd"},
    [BATTLE_DATA_EVENT.HALLOW_ACTIVATE_LIST] = {"parseHallowActivateList", "setDataHallowActivateList", nil, "setDataHallowActivateListEnd"},
    [BATTLE_DATA_EVENT.HALLOW_INTERVENE] = {"parseHallowIntervene", "setDataHallowIntervene", nil, "setDataHallowInterveneEnd"},
    [BATTLE_DATA_EVENT.HALLOW_GIFT_TRIGGER] = {"parseHallowGiftTrigger", "setDataHallowGiftTrigger", nil, "setDataHallowGiftTriggerEnd"},
    [BATTLE_DATA_EVENT.HUANSHOU_SKILL_CD] = {"parseHuanShouSkillCD",nil, nil, nil},
    [BATTLE_DATA_EVENT.DRAGON_SKILL_PERFORM] = {"parseDragonSkillPerform", "setDataDragonSkillPerform", nil, nil},
    [BATTLE_DATA_EVENT.DRAGON_SKILL_HIT] = {"parseDragonSkillHit", nil, nil, nil},
    [BATTLE_DATA_EVENT.DRAGON_SKILL_EFFECT] = {nil, nil, nil, nil},
    [BATTLE_DATA_EVENT.DRAGON_PASSIVE_EFFECT] = {"parseDragonPassiveEffect", "setDataDragonPassiveEffect", nil, nil},
    [BATTLE_DATA_EVENT.DRAGON_STAR_CHANGE] = {"parseDragonStarChange", "setDataDragonStarChange", nil, nil},
    [BATTLE_DATA_EVENT.RABBIT_SHIELD_GRID_INFO_CHANGE] = {"parseBattleShieldGridInfoChange", "setDataBattleShieldGridInfoChange", nil, nil},
    [BATTLE_DATA_EVENT.RABBIT_SHIELD_GRID_FLICKER] = {nil, "setDataBattleShieldGridFlicker", nil, nil},
    [BATTLE_DATA_EVENT.SEAL_NETHER_WORLD] = {"parseSealYouMing", "setDataSealYouMing", nil, nil},
}

local behaviour_queue = nil

function BattleNodeEvent.SetBehaviourQueue(queue)
    behaviour_queue = queue
end

function BattleNodeEvent.defaultParseData(eve_type, data_len)
    if data_len ~= nil then
        MsgAdapter.ReadStrN(data_len)
    end
end

function BattleNodeEvent.ParseData(eve_type, data_len)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and not StringIsEmpty(eve_co[1]) and eve_co[1] or "defaultParseData"
    return BattleNodeEvent[func_name](eve_type, data_len)
end

function BattleNodeEvent.SetDataBegin(eve_type, grid_idx, eve_data)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and eve_co[3] or nil
    if not StringIsEmpty(func_name) then
        BattleNodeEvent[func_name](grid_idx, eve_data)
    end
end

function BattleNodeEvent.SetDataAfterBegin(eve_type, grid_idx, eve_data)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and eve_co[5] or nil
    if not StringIsEmpty(func_name) then
        BattleNodeEvent[func_name](grid_idx, eve_data)
    end
end

function BattleNodeEvent.SetDataAfterEnd(eve_type, grid_idx, eve_data)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and eve_co[6] or nil
    if not StringIsEmpty(func_name) then
        BattleNodeEvent[func_name](grid_idx, eve_data)
    end
end

function BattleNodeEvent.SetDataPerform(eve_type, grid_idx, eve_data, child_data, last_data)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and eve_co[2] or nil
    if StringIsEmpty(func_name) then
        -- Debuger.LogWarning("Unconfig BattleNodeEvent SetData Perform Func====EveType=%s", tostring(eve_type))
    else
        BattleNodeEvent[func_name](grid_idx, eve_data, child_data, last_data)
    end
end

function BattleNodeEvent.SetDataEnd(eve_type, grid_idx, eve_data)
    local eve_co = config[eve_type]
    local func_name = eve_co ~= nil and eve_co[4] or nil
    if not StringIsEmpty(func_name) then
        BattleNodeEvent[func_name](grid_idx, eve_data)
    end
end

---------1 PERFORM_SKILL Perform skill Begin----------------

function BattleNodeEvent.parsePerformSkill()
    local data = {}
    data.skill_id = MsgAdapter.ReadUShort()
    local target_num = MsgAdapter.ReadShort()
    data.target_list = {}
    for i = 1, target_num do
        data.target_list[i] = MsgAdapter.ReadShort() + 1
    end
    return data
end

function BattleNodeEvent.setDataPerformSkillBegin(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.TotalDamage, {})
    BattleNodeEvent.PopupSkill = false
end

local function pushAttackAudio(grid_idx)
    local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
    local aud_config = AudioMgr:GetAttackName(ch_data)
    if aud_config then
        local aud_beh = BattleBehaviourAudioPlayer.New(aud_config.attack_1)
        behaviour_queue:PushSyncBehaviour(aud_beh)
        aud_beh = BattleBehaviourAudioPlayer.New(aud_config["voice_"..GameMath.Ramdon(5)])
        behaviour_queue:PushSyncBehaviour(aud_beh)
    end
end

function BattleNodeEvent.setDataPerformSkill(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    BattleNodeEvent.pushParam(ParamKey.SkillId, eve_data.skill_id)
    BattleNodeEvent.pushParam(ParamKey.TargetList, eve_data.target_list)
    BattleNodeEvent.pushParam(ParamKey.EventType, BATTLE_DATA_EVENT.PERFORM_SKILL)
    if BattleHelper.SKILL_SHOW_IGNORE[eve_data.skill_id] then
        return
    end
    if eve_data.skill_id < 100 then
        pushAttackAudio(grid_idx)
    end
    local skill_cfg = SkillData.GetConfig(eve_data.skill_id)
    if skill_cfg and not StringIsEmpty(skill_cfg.extra_logic) then
        if skill_cfg.extra_logic == BattleHelper.ExtraLogic.FenShen then
            BattleNodeEvent.CreateFenShen(grid_idx, eve_data)
        end
    end
    -- 4061 告诉子节点要创建多少个残影对象，
end
function BattleNodeEvent.CreateFenShen(grid_idx, eve_data)
    --创建分身
    local beh
    local idx
    local shadow = {}
    for i = 1, table.nums(eve_data.target_list) - 1 do
        idx = grid_idx * 100 + i
        table.insert(shadow, idx)
        beh = BattleCreateFenShen.New(idx, grid_idx)
        behaviour_queue:PushSyncBehaviour(beh)
    end
    BattleNodeEvent.pushParam(ParamKey.ShadowIdxs, shadow)
end
function BattleNodeEvent.FenShenMove(target_list, move_beh)
    --创建分身
    local shadows = BattleNodeEvent.getParam(ParamKey.ShadowIdxs)
    local add_beh
    if shadows and target_list[2] and shadows[1] then
        add_beh = BattleBehaviourMove.New(shadows[1], target_list[2])
        move_beh:AddBehavior(add_beh)
    end
    if shadows and target_list[3] and shadows[2] then
        add_beh = BattleBehaviourMove.New(shadows[2], target_list[3])
        move_beh:AddBehavior(add_beh)
    end
end
function BattleNodeEvent.FenShenSkill(skill_id, target_list, atk_beh, version)
    local shadows = BattleNodeEvent.getParam(ParamKey.ShadowIdxs)
    local add_beh
    if shadows and target_list[2] and shadows[1] then
        add_beh = BattleBehaviourPerformSkill.New(skill_id, shadows[1], {target_list[2]}, {}, SKILL_TIMING_TYPE.IN, nil, version)
        atk_beh:AddBehavior(add_beh)
    end
    if shadows and target_list[3] and shadows[2] then
        add_beh = BattleBehaviourPerformSkill.New(skill_id, shadows[2], {target_list[3]}, {}, SKILL_TIMING_TYPE.IN, nil, version)
        atk_beh:AddBehavior(add_beh)
    end
end

local function pushTotalDamageBeh()
    local total_damage = BattleNodeEvent.popParam(ParamKey.TotalDamage)
    if #total_damage > 1 then
        behaviour_queue:PushQueueBehaviour(BattleTotalDamage.New(total_damage))
    end
end
local function getSkillAssetId(skill_id)
    if skill_id and skill_id < 100 then
        return ""
    end
    if skill_id == nil then
        return ""
    end
    local skill_cfg = SkillData.GetConfig(skill_id)
    if skill_cfg and skill_cfg.asset_id_2 then
        return skill_cfg.asset_id_2
    end
    return ""
end
function BattleNodeEvent.setDataPerformSkillEnd(grid_idx, eve_data)
    pushTotalDamageBeh()
    BattleNodeEvent.popParam(ParamKey.Atker)
    BattleNodeEvent.popParam(ParamKey.SkillId)
    BattleNodeEvent.popParam(ParamKey.TargetList)
    BattleNodeEvent.popParam(ParamKey.EventType)
    if BattleHelper.SKILL_SHOW_IGNORE[eve_data.skill_id] then
        return
    end
    local skill_id = eve_data.skill_id
    --两个复制对象不需要移动回去直接删除
    local shadows = BattleNodeEvent.getParam(ParamKey.ShadowIdxs)
    if shadows then
        local beh
        for _, idx in ipairs(shadows) do
            beh = BattleDeleteBattleCh.New(idx)
            behaviour_queue:PushSyncBehaviour(beh)
        end
    end
    BattleNodeEvent.popParam(ParamKey.ShadowIdxs)
    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, eve_data)
    local asset_id = getSkillAssetId(skill_id)
    if BattleHelper.CHONGCHI_PRE_SKILL[asset_id] then
        need_move = true
    end
    if need_move then
        local post_skill_idx = BattleNodeEvent.popParam(ParamKey.NotAtPosition)
        if post_skill_idx then
            local die_flag = post_skill_idx - grid_idx - 1
            local move_beh
            if die_flag == 0 then --回位置，死亡倒地
                move_beh = BattleBehaviourMove.New(grid_idx)
                behaviour_queue:PushQueueBehaviour(move_beh)
                local death_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Death, false)
                behaviour_queue:PushQueueBehaviour(death_beh)
            elseif die_flag == 1 then --回位置，死亡消失
                move_beh = BattleBehaviourMove.New(grid_idx)
                behaviour_queue:PushQueueBehaviour(move_beh)
                local diedis_beh = BattleDieDisappear.New(grid_idx)
                behaviour_queue:PushQueueBehaviour(diedis_beh)
            elseif die_flag == 2 then --不回位置，已经被击飞
            else --正常回位置
                move_beh = BattleBehaviourMove.New(grid_idx, nil)
                behaviour_queue:PushQueueBehaviour(move_beh)
            end
        else
            BuglyReport("post_skill_idx == nil, skill_id = ",skill_id)
        end
    end
    for _, target_idx in pairs(eve_data.target_list) do
        BattleNodeEvent.popHitTrigger(grid_idx, target_idx)
        if asset_id == BattleHelper.MFDanTiFengRenMoFaXuLi then--风刃系被击
            BattleNodeEvent.popHitTrigger(BattleNodeEvent.GenSkillEventTriggerIdx(skill_id, grid_idx, 1), target_idx)
        end
    end
    BattleNodeEvent.popParam(ParamKey.BehiterIdxs)
    BattleNodeEvent.not_at_pos[grid_idx] = false
    BattleNodeEvent.show_once_zhimingyiji = nil
end

local function getSkillId()
    local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
    local skill_id
    if event_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
        skill_id = BattleNodeEvent.getParam(ParamKey.ComboSkillId)
    else
        skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    end
    return skill_id
end

--------------------Skill Effect-----------------------------

function BattleNodeEvent.setDataSkillPreEffect(grid_idx, eve_data)
    local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
    local skill_id = getSkillId()
    if BattleHelper.SKILL_SHOW_IGNORE[skill_id] then
        return
    end
    -- 前后置技能只对自己生效
    local target_list = BattleNodeEvent.getParam(ParamKey.TargetList)
    local asset_id = getSkillAssetId(skill_id)
    if skill_id >= 100 then
        local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
        local audio_cfg = AudioMgr:GetAttackName(ch_data)
        local atk_beh
        if BattleHelper.COMBO_TATGET_NOONE[asset_id] then
            --百兽之怒前置目标需要时敌方
            local temp_target_idx = grid_idx > 10 and (grid_idx - 10) or (grid_idx + 10)
            atk_beh = BattleBehaviourPerformSkill.New(skill_id, grid_idx, {temp_target_idx}, {temp_target_idx}, SKILL_TIMING_TYPE.PRE, audio_cfg)
        else
            atk_beh = BattleBehaviourPerformSkill.New(skill_id, grid_idx, {grid_idx}, {grid_idx}, SKILL_TIMING_TYPE.PRE, audio_cfg)
        end
        if BattleHelper.SKILL_GUARD[asset_id] and target_list then
            for _, tar_idx in pairs(target_list) do
                atk_beh:AddToActorList(tar_idx)
            end
        end
        behaviour_queue:PushQueueBehaviour(atk_beh)
    end
    if event_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
        return
    end
    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, {skill_id = skill_id, target_list = target_list})
    if BattleHelper.CHONGCHI_PRE_SKILL[asset_id] then
        need_move = false
        BattleNodeEvent.pushParam(ParamKey.NotAtPosition, grid_idx)--需要告知不在位置上
    end

    if need_move then
        local move_beh
        if asset_id == BattleHelper.YuanHu then
            --LogError("援护移动",grid_idx, target_list[1])
            move_beh = BattleRabbitShiledMove.New(grid_idx, target_list[1])
        else
            move_beh = BattleBehaviourMove.New(grid_idx, target_list[1])
        end
        
        local skill_cfg = SkillData.GetConfig(skill_id)
        if skill_cfg and not StringIsEmpty(skill_cfg.extra_logic) then
            if skill_cfg.extra_logic == BattleHelper.ExtraLogic.FenShen then
                BattleNodeEvent.FenShenMove(target_list, move_beh)
            end
        end
        behaviour_queue:PushQueueBehaviour(move_beh)
        BattleNodeEvent.pushParam(ParamKey.NotAtPosition, grid_idx)
        BattleNodeEvent.not_at_pos[grid_idx] = true
    end
end
local function packingName(grid_idx)
    local ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    if ch_info == nil then
        --出现这种情况说明出现了死亡或者其他情况使预数据为空
        ch_info = BattleData.Instance:GetChInfo(grid_idx)
    end
    local temp = {}
    temp.name = ch_info and ch_info.name or "神秘人"
    if not BattleData.IsEnemyIdx(grid_idx) then
        temp.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", temp.name)
    end
    return temp.name
end
local function packingName2(grid_idx, name)
    local temp = {}
    temp.name = name or "神秘圣器"
    if not BattleData.IsEnemyIdx(grid_idx) then
        temp.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", temp.name)
    end
    return temp.name
end
function BattleNodeEvent.setDataSkillEffect(grid_idx, eve_data, child_data)
    local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
    if event_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
        BattleNodeEvent.ComboTotalNum = 0
        if child_data then
            for _, v in ipairs(child_data) do
                if v.eve_type == BATTLE_DATA_EVENT.COMBO_SKILL_ONE_HIT then
                    BattleNodeEvent.ComboTotalNum = BattleNodeEvent.ComboTotalNum + 1
                end
            end
        end
        return
    end
    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    if BattleHelper.SKILL_SHOW_IGNORE[skill_id] then
        return
    end
    local behiter_idxs = {}
    BattleNodeEvent.pushParam(ParamKey.BehiterIdxs, behiter_idxs)

    local target_list = BattleNodeEvent.getParam(ParamKey.TargetList)
    if table.nums(target_list) == 0 then
        --服务端没目标的都发我也没办法
        return
    end
    local asset_id = getSkillAssetId(skill_id)
    --控制具体使用哪个技能
    local version = 1
    if BattleHelper.SHUANG_JI[asset_id] == true then
        version = BattleData.Instance:IsLiuXingOrHuiXing(grid_idx)
    end
    if BattleHelper.SheMingYiji == asset_id then
        local ch = BattleData.Instance:GetChInfo(grid_idx)
        if ch and (ch.character_type == BATTLE_CH_TYPE.PET or ch.character_type == BATTLE_CH_TYPE.PET_SHADOW) then
            --默认版本1 牛牛版本 版本2非牛牛版本
            local cfg = PetData.Instance:GetPetCfg(ch.character_id)
            if cfg and cfg.prototype_id ~= 50185 then
                version = 2
            end
        end
    end

    local atk_beh
    if skill_id >= 100 then
        atk_beh = BattleBehaviourPerformSkill.New(skill_id, grid_idx, target_list, behiter_idxs, SKILL_TIMING_TYPE.IN, nil, version)
        local skill_cfg = SkillData.GetConfig(skill_id)
        if skill_cfg and not StringIsEmpty(skill_cfg.extra_logic) then
            if skill_cfg.extra_logic == BattleHelper.ExtraLogic.FenShen then
                BattleNodeEvent.FenShenSkill(skill_id, target_list, atk_beh, version)
            end
        end
    else
        
        -- local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SKILL_HIT], packingName(grid_idx), Language.BattleDetail.Attack)
        -- pushDesc(BATTLE_DATA_EVENT.SKILL_HIT, desc)

        atk_beh = BattleBehaviourPerformAttack.New(grid_idx, target_list, behiter_idxs)
        atk_beh.detail_player = BattleNodeEvent.detail_player
    end
    if asset_id == BattleHelper.SheMingYiJi2 then
        atk_beh.binding_behavior = BattlePlayBuffAssetStart.New(grid_idx, BattleHelper.SheShengQuYiBuff)
    end
    BattleNodeEvent.pushParam(ParamKey.Hitter, atk_beh)

    if BattleHelper.HUWEI[asset_id] then
        behaviour_queue:PushSyncBehaviour(atk_beh)
    else
        behaviour_queue:PushQueueBehaviour(atk_beh)
    end
    --增加触发器
    for _, target_idx in pairs(target_list) do
        atk_beh:AddHitTrigger(BattleNodeEvent.pushHitTrigger(grid_idx, target_idx))
        if asset_id == BattleHelper.MFDanTiFengRenMoFaXuLi then
            atk_beh:AddHitTrigger(BattleNodeEvent.pushHitTrigger(BattleNodeEvent.GenSkillEventTriggerIdx(skill_id, grid_idx, 1), target_idx))
        end
    end
end

function BattleNodeEvent.setDataSkillEffectEnd(grid_idx, eve_data)
    --注意这里如果其他地方比这慢要换个地方pop掉
    BattleNodeEvent.popParam(ParamKey.Hitter)
end

function BattleNodeEvent.setDataSkillPostEffect(grid_idx, eve_data)
    local skill_id = getSkillId()
    if BattleHelper.SKILL_SHOW_IGNORE[skill_id] then
        return
    end
    -- 前后置技能只对自己生效
    if skill_id >= 100 then
        local atk_beh = BattleBehaviourPerformSkill.New(skill_id, grid_idx, {grid_idx}, {grid_idx}, SKILL_TIMING_TYPE.POST)
        behaviour_queue:PushQueueBehaviour(atk_beh)
    end
end
--吸收伤害
function BattleNodeEvent.parseAbsorbDamage()
    local data = {}
    data.buff_id = MsgAdapter.ReadShort()
    data.source_skill_id = MsgAdapter.ReadUShort()
    return data
end

function BattleNodeEvent.setDataAbsorbDamage(grid_idx, eve_data)
    BattleNodeEvent.absorb_damage = true
    local beh = BattleBuff.New(grid_idx, {stage_key = "AbsorbDamage"}, BATTLE_DATA_EVENT.PLAY_ALL_BUFF)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    if trigger then
        beh:SetTrigger(trigger)
    end
    behaviour_queue:PushSyncBehaviour(beh)
    --新增吸收飘字
    local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.XISHOU_DAMAGE)
    if trigger then
        popup_beh:SetTrigger(trigger)
    end
    behaviour_queue:PushSyncBehaviour(popup_beh)
end

function BattleNodeEvent.setDataAbsorbDamageEnd()
    BattleNodeEvent.absorb_damage = false
end

function BattleNodeEvent.setDataInVain(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    local hp_beh = BattleHpChange.New(grid_idx, 0, -1, BATTLE_HP_DESC.IN_VAIN, 0, 0, atker, BattleNodeEvent.detail_player)
    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    if trigger then
        hp_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(hp_beh)
    else
        behaviour_queue:PushQueueBehaviour(hp_beh)
    end
    --破空失效不存在被击效果
    if atker ~= 0 and skill_id == BattleHelper.PoKong then
        local popup_beh = BattlePopup.New(atker, BATTLE_HP_DESC.SKILL, skill_id, BattleNodeEvent.detail_player)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
end

function BattleNodeEvent.parseSkillHit()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.skill_id = MsgAdapter.ReadUShort()
    return data
end

local function pushBehitBeh(tar_idx, grid_idx, count, interval)
    count = count - 1
    for i = 0, count do
        local be_hit_beh = BattleBehaviourBeHit.New(tar_idx, grid_idx, false, true, nil, true)
        local hit_trigger = BattleNodeEvent.getHitTrigger(grid_idx, tar_idx)
        be_hit_beh:SetTrigger(hit_trigger)
        be_hit_beh:SetDelayTime(interval * i)
        behaviour_queue:PushSyncBehaviour(be_hit_beh)
    end
end
function BattleNodeEvent.setDataSkillHit(grid_idx, eve_data)
    local behiter_idxs = BattleNodeEvent.getParam(ParamKey.BehiterIdxs)
    if behiter_idxs ~= nil then
        table.insert(behiter_idxs, eve_data.target_grid_idx)
    end
    local asset_id = getSkillAssetId(eve_data.skill_id)
    if eve_data.skill_id == 1470 then
        BattleNodeEvent.pushParam(ParamKey.LinkTarget, eve_data.target_grid_idx)
    end
    if BattleHelper.MULTI_BEHIT5[asset_id] == true then
        pushBehitBeh(eve_data.target_grid_idx, grid_idx, 5, 0.2)
    elseif BattleHelper.MULTI_BEHIT4[asset_id] == true then
        pushBehitBeh(eve_data.target_grid_idx, grid_idx, 4, 0.1)
    end
    local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
    if grid_idx ~= 0 and event_type ~= BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL and eve_data.skill_id >= 100
    and BattleNodeEvent.PopupSkill == false then
        BattleNodeEvent.PopupSkill = true
        if BattleHelper.SKILLNAME_BIG_SHOW[asset_id] then
            return
        end
        -- local skill_cfg = SkillData.GetConfig(eve_data.skill_id)
        -- local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SKILL_HIT], packingName(grid_idx), skill_cfg.skill_name)
        -- pushDesc(BATTLE_DATA_EVENT.SKILL_HIT, desc)

        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.SKILL, eve_data.skill_id, BattleNodeEvent.detail_player)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
end

function BattleNodeEvent.parseSkillSpecialEffect()
    local data = {}
    data.sp_id = MsgAdapter.ReadInt()
    return data
end

function BattleNodeEvent.setDataSkillSpecialEffect(grid_idx, eve_data)
    if BattleHelper.HIDE_SP_EFFECT[eve_data.sp_id] == true then
        return
    end

    local sp_id = eve_data.sp_id
    local skill_cfg = Cfg.SkillPassiveSpID(sp_id, false)
    if skill_cfg then
        skill_cfg = Cfg.HeartSkillByID(skill_cfg.skill_id, false)
        if skill_cfg then
            local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SKILL_SPECIAL_EFFECT][1], packingName(grid_idx), skill_cfg.heart_skill_name)
            pushDesc(BATTLE_DATA_EVENT.SKILL_SPECIAL_EFFECT, desc)
        end
    end


    local spbeh = BattleSpecial.New(grid_idx, eve_data.sp_id, BattleNodeEvent.detail_player)
    behaviour_queue:PushSyncBehaviour(spbeh)
    local cfg = BagData.Instance:GetEquipEffectCfg(eve_data.sp_id)
    if cfg and cfg.show_effect_name == 1 then
        if cfg.name ~= ErrorText[182] or (cfg.name == ErrorText[182] and BattleNodeEvent.show_once_zhimingyiji == nil) then
            local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.SP_EFF, eve_data.sp_id)
            behaviour_queue:PushSyncBehaviour(popup_beh)
            if cfg.name == ErrorText[182] then
                BattleNodeEvent.show_once_zhimingyiji = 1
            end
        end
    elseif cfg and cfg.is_purify_effect == 1 then
        local beh = BattlePlayBuffAssetStart.New(grid_idx, BattleHelper.YiChangJieChu)
        behaviour_queue:PushSyncBehaviour(beh)
    end
    local skill_cfg = Cfg.SkillPassiveSpID(eve_data.sp_id, false)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    if skill_cfg and skill_cfg.asset_id_1 ~= "" and atker ~= nil then
        local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        local beh = BattleBehaviourPerformSkillByAssetId.New(skill_cfg.asset_id_1, grid_idx, {grid_idx}, {grid_idx})
        beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(beh)
    end
    local skill_cfg = Cfg.SkillPassiveLevelSpID(eve_data.sp_id, false)
    if skill_cfg and skill_cfg.asset_id_1 ~= "" and skill_cfg.asset_type ~="" and skill_cfg.asset_id_1 ~= BattleHelper.DuoHunShePoAsset then
        if skill_cfg.asset_type == 1 then
            if skill_cfg.asset_id_1 == BattleHelper.ZhiMingYiJi then
                local behiters = BattleNodeEvent.getParam(ParamKey.BehiterIdxs)
                if behiters and next(behiters) then
                    local beh = BattlePlayBuffAssetStart.New(behiters[#behiters], skill_cfg.asset_id_1)
                    behaviour_queue:PushSyncBehaviour(beh)
                else
                    BuglyReport("behiter_idxs is nil ??????", BattleData.Instance:BattleMode(),
                    BattleData.Instance:GetBossGroupId(), grid_idx, eve_data)
                end
            elseif skill_cfg.asset_id_1 == BattleHelper.HuaXianWeiYi then
                local beh = BattlePlayBuffAssetStart.New(BattleData.Instance:GetPetIdxByPlayerIdx(grid_idx), skill_cfg.asset_id_1)
                behaviour_queue:PushSyncBehaviour(beh)
            else
                local beh = BattlePlayBuffAssetStart.New(grid_idx, skill_cfg.asset_id_1)
                behaviour_queue:PushSyncBehaviour(beh)
            end
        else
            local beh = BattleBehaviourPerformSkillByAssetId.New(skill_cfg.asset_id_1, grid_idx, {grid_idx}, {grid_idx})
            behaviour_queue:PushSyncBehaviour(beh)
        end
    end
end

---------Perform skill End----------------

function BattleNodeEvent.setDataDodge(grid_idx, eve_data, detail_player)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local comboIdx = BattleNodeEvent.getParam(ParamKey.ComboIdx)
    local skill_id = getSkillId()
    local asset_id = getSkillAssetId(skill_id)
    if atker and skill_id then
        local config = SkillData.GetConfig(skill_id)
        local desc =
            string.format(
            Language.BattleDetail[BATTLE_DATA_EVENT.DODGE],
            packingName(grid_idx),
            packingName(atker),
            config.skill_name
        )
        pushDesc(BATTLE_DATA_EVENT.DODGE, desc)
    end
    if atker ~= nil and not BattleHelper.COMBO_SKILL_SP[asset_id] then
        local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        if BattleHelper.IGNORE_ATKER_DODGE[asset_id] == true then
            atker = 0
        end
        local dod_beh = BattleDodge.New(grid_idx, atker, comboIdx, BattleNodeEvent.detail_player)
        dod_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(dod_beh)
    else
        local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.DODGE)
        popup_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
end

--------------------MODIFY_HP 3-----------------------------

function BattleNodeEvent.parseModifyHp()
    local data = {}
    data.modify_num = MsgAdapter.ReadInt()
    data.cur_num = MsgAdapter.ReadInt()
    data.modify_desc = MsgAdapter.ReadShort() --BATTLE_HP_DESC
    data.behit_desc = MsgAdapter.ReadShort() --BATTLE_BE_HIT_DESC
    data.attr_injure_num = MsgAdapter.ReadInt()

    data.behit_buff_id = MsgAdapter.ReadShort()
    data.behit_buff_source_skill = MsgAdapter.ReadUShort()
    data.hp_shield = MsgAdapter.ReadInt()
    data.frozen_shield = MsgAdapter.ReadInt()
    data.frozen_shield_full = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataModifyHp(grid_idx, eve_data, child_data, last_data)
    BattleNodeEvent.BehitDesc = eve_data.behit_desc
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    if skill_id == nil then
        skill_id = BattleNodeEvent.getParam(ParamKey.ComboSkillId)
        if skill_id then
            atker = BattleNodeEvent.getParam(ParamKey.ComboIdx)
        end
    end
    local asset_id = getSkillAssetId(skill_id)
    if atker == grid_idx then
        --猜测出现该情况的原因
        --一般为反噬
        --还有一种是主动出击时被反击
        local temp = BattleNodeEvent.getParam(ParamKey.AtkBacker)
        if temp then
            atker = temp
        end
    end
    local ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    if ch_info == nil then
        --出现这种情况说明出现了死亡或者其他情况使预数据为空
        ch_info = BattleData.Instance:GetChInfo(grid_idx)
    end
    local ch_info2 = BattleData.Instance:GetDataChInfo(atker)
    local name = packingName(grid_idx)
    local atk_name = ""
    if atker and atker ~= 0 then
        atk_name = packingName(atker)
    end

    if eve_data.behit_desc == BATTLE_BE_HIT_DESC.BLOCK_1 then
        -- 格挡
        local anima_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Defence, false)
        behaviour_queue:PushQueueBehaviour(anima_beh)
    end

    local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
    if event_type == BATTLE_DATA_EVENT.HALLOW_PERFORM_SKILL then
        local data = BattleNodeEvent.getParam(ParamKey.HallowSkill)
        if data then
            local config = Cfg.ShengQiCfgById(data.hallow_id)
            if config then
                local name = packingName2(data.grid_idx, config.shengqi_name)
                local name2 = packingName(grid_idx)
                local skill = Cfg.ShengQiZhuDongSkill(data.hallow_skill_id)
                if eve_data.modify_num < 0 then
                    --受伤
                    local desc = string.format(Language.BattleDetail[event_type][1], name, skill.skill_name, name2, -eve_data.modify_num)
                    pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                else
                    --回复
                    local desc = string.format(Language.BattleDetail[event_type][2], name, skill.skill_name, name2, eve_data.modify_num)
                    pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                end
            else
                LogDG("没获取到圣器配置")
            end
        end
    end
    if event_type == BATTLE_DATA_EVENT.DRAGON_SKILL_HIT then
        local data = BattleNodeEvent.getParam(ParamKey.FairyDragonSkill)
        local skill_id = data.skill_id
        if data and skill_id then
            local skill_cfg = Cfg.FairyDragonSkillCfg(skill_id)
            if eve_data.modify_num < 0 then
                local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.DRAGON_SKILL_HIT][1], skill_cfg.skill_name, name, -eve_data.modify_num)
                pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
            else
                local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.DRAGON_SKILL_HIT][2], skill_cfg.skill_name, name, eve_data.modify_num)
                pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
            end
        end
    end
    if last_data == nil then
        if eve_data.modify_num < 0 then
            local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][8], name, COLORSTR.Red10, -eve_data.modify_num)
            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
        else
            local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][5], name, COLORSTR.Green10, eve_data.modify_num)
            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
        end
    elseif event_type == BATTLE_DATA_EVENT.SELF_DESTRUCT then
        local desc =
            string.format(
            Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][2],
            atk_name,
            Language.BattleDetail.SelfDestruct,
            name,
            -eve_data.modify_num
        )
        pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
    else
        local skill_cfg = nil
        if last_data and last_data.data then
            local last_eve = last_data.data.eve_type
            if last_eve == BATTLE_DATA_EVENT.COMBINE_ATTACK_PERFORM_ONE_HIT then
                skill_cfg = nil
            elseif skill_id then
                skill_cfg = SkillData.GetConfig(skill_id)
            end
        end
        --无id情况1 合击
        if skill_cfg and eve_data.modify_desc == BATTLE_HP_DESC.ADD_HP or eve_data.modify_desc == BATTLE_HP_DESC.HP_CRITICAL then --加血
            local desc = string.format(
                    Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][3],
                    atk_name,
                    skill_cfg.skill_name,
                    name,
                    eve_data.modify_num
                )
            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
        elseif skill_cfg and eve_data.modify_desc == BATTLE_HP_DESC.DAMAGE_ABSORB_HP then
            local desc = string.format(
                Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][6],
                atk_name,
                skill_cfg.skill_name,
                -- name,
                eve_data.modify_num
            )
            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
        else
            if skill_cfg and ch_info2 and ch_info and ch_info.name ~= ch_info2.name then
                local flag = tonumber(eve_data.attr_injure_num) < 0
                local last_eve = last_data.data
                local atk_back = BattleNodeEvent.getParam(ParamKey.AtkBacker)
                local is_atk_back = (atk_back == last_data.grid_idx and last_eve.skill_id == 1)
                if eve_data.attr_injure_num == 0 then
                    local desc =
                        string.format(
                        Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][2],
                        atk_name,
                        is_atk_back and Language.BattleDetail.AtkBack or skill_cfg.skill_name,
                        name,
                        -eve_data.modify_num,
                        flag and COLORSTR.Red10 or COLORSTR.Green10,
                        eve_data.attr_injure_num
                    )
                    -- 格挡不进入战报
                    if eve_data.behit_desc ~= BATTLE_BE_HIT_DESC.BLOCK_1 then
                        pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                    end
                else
                    local desc =
                        string.format(
                        Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][1],
                        atk_name,
                        is_atk_back and Language.BattleDetail.AtkBack or skill_cfg.skill_name,
                        name,
                        -eve_data.modify_num,
                        flag and COLORSTR.Red10 or COLORSTR.Green10,
                        eve_data.attr_injure_num
                    )
                    pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                end
            else
                --自己打自己
                if last_data and last_data.eve_type == BATTLE_DATA_EVENT.SKILL_HIT then
                    local last_eve = last_data.data
                    local skill_cfg = SkillData.GetConfig(last_eve.skill_id)
                    local ch_info2 = BattleData.Instance:GetDataChInfo(last_data.grid_idx)
                    if skill_cfg and ch_info2 then
                        local atk_name = packingName(last_data.grid_idx)
                        local flag = eve_data.attr_injure_num < 0
                        if eve_data.attr_injure_num == 0 then
                            local desc =
                                string.format(
                                Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][2],
                                atk_name,
                                skill_cfg.skill_name,
                                name,
                                -eve_data.modify_num,
                                flag and COLORSTR.Red10 or COLORSTR.Green10,
                                flag and eve_data.attr_injure_num or -eve_data.attr_injure_num
                            )
                            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                        else
                            local desc =
                                string.format(
                                Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_HP][1],
                                atk_name,
                                skill_cfg.skill_name,
                                name,
                                -eve_data.modify_num,
                                flag and COLORSTR.Red10 or COLORSTR.Green10,
                                eve_data.attr_injure_num
                            )
                            pushDesc(BATTLE_DATA_EVENT.MODIFY_HP, desc)
                        end
                    end
                end
            end
        end
    end
    

    local hit_trigger_behit_idx = grid_idx
    local cur_guarder = BattleNodeEvent.getParam(ParamKey.Guarder)
    if cur_guarder == grid_idx then
        hit_trigger_behit_idx = BattleNodeEvent.getParam(ParamKey.BeGuarer)
    end
    if (eve_data.modify_desc == BATTLE_HP_DESC.DAMAGE_ABSORB_HP
    or eve_data.modify_desc == BATTLE_HP_DESC.ADD_HP) and atker == hit_trigger_behit_idx then
        local behiter_idxs = BattleNodeEvent.getParam(ParamKey.BehiterIdxs)
        if behiter_idxs ~= nil and next(behiter_idxs) then
            hit_trigger_behit_idx = behiter_idxs[1]
        end
    elseif BattleHelper.SKILL_JIANSHE[asset_id] or eve_data.behit_desc == BATTLE_BE_HIT_DESC.SHARE_DAMAGE then
        local behiter_idxs = BattleNodeEvent.getParam(ParamKey.BehiterIdxs)
        if behiter_idxs ~= nil and next(behiter_idxs) then
            hit_trigger_behit_idx = behiter_idxs[1]
        end
    end
    --获取正确的被击trigger
    local trigger = BattleNodeEvent.getHitTrigger(atker, hit_trigger_behit_idx)
    if BattleNodeEvent.spread_skill_hit_trigger then
        --这里是获取到扩散技能的触发器
        trigger = BattleNodeEvent.spread_skill_hit_trigger
        BattleNodeEvent.spread_skill_hit_trigger = nil
    end
    local total_damage = BattleNodeEvent.getParam(ParamKey.TotalDamage)
    if total_damage and eve_data.modify_num < 0 and atker and not BattleData.IsEnemyIdx(atker) and hit_trigger_behit_idx ~= atker then
        table.insert(total_damage, -eve_data.modify_num)
    end
    --夺魂摄魄释放时机
    if eve_data.modify_desc == BATTLE_HP_DESC.DESPERATE then
        local popup_beh = BattlePopup.New(atker, BATTLE_HP_DESC.SKILL, BattleHelper.DuoHunShePo, BattleNodeEvent.detail_player)
        behaviour_queue:PushSyncBehaviour(popup_beh)
        popup_beh:SetTrigger(trigger)
        local beh = BattlePlayBuffAssetStart.New(grid_idx, BattleHelper.DuoHunShePoAsset)
        behaviour_queue:PushSyncBehaviour(beh)
    end
    local ch_info = BattleData.Instance:GetChInfo(grid_idx)
    if ch_info then
        ch_info.frozen_shield_full = eve_data.frozen_shield_full
    end
    local hp_beh
    if eve_data.modify_desc == BATTLE_HP_DESC.COMBINE_ATTACK then
        hp_beh = BattleHpChange.New(grid_idx, 0, eve_data.cur_num, eve_data.modify_desc, 
            nil, eve_data.behit_desc, nil, BattleNodeEvent.detail_player,eve_data.hp_shield,
        eve_data.frozen_shield)
    else
        hp_beh = BattleHpChange.New(grid_idx, eve_data.modify_num, eve_data.cur_num, eve_data.modify_desc, 
            eve_data.attr_injure_num, eve_data.behit_desc, atker, BattleNodeEvent.detail_player,eve_data.hp_shield,
        eve_data.frozen_shield)
    end
    if BattleHelper.DAMAGE_DELAY_SHOWTIME[asset_id] then
        hp_beh:SetDelayTime(BattleHelper.DAMAGE_DELAY_SHOWTIME[asset_id])
    end
    if not trigger and eve_data.modify_desc == BATTLE_HP_DESC.REAL_DAMAGE then
        -- hp_beh:SetDelayTime(0.2)
        -- 延迟影响了事件顺序
    end
    if BattleNodeEvent.use_fabao_skill_type and BattleNodeEvent.use_fabao_skill_type == 112 then
        hp_beh:SetDelayTime(0.4)
    end
    if asset_id and asset_id == BattleHelper.XianLingZhuFu then
        local data = {}
        data.config = SceneEffectConfig.EffectList.XianLingJiaXue
        data.sign = "xianlingjiaxue"
        data.grid_idx = grid_idx
        data.auto_release = true
        local eff_beh = BattlePlayEffectOnCh.New(data)
        if trigger then
            eff_beh:SetTrigger(trigger)
        end
        behaviour_queue:PushSyncBehaviour(eff_beh)
    end
    -- elseif skill_id == 132 then--or skill_id == 232 or skill_id == 496 then 前提是没有trigger
    --     --吸血类技能 XiXueGongJi XianLingZhuFu
    --     local recover_trigger = BattleNodeEvent.pushHitTrigger(atker, atker)
    --     local hitter = BattleNodeEvent.getParam(ParamKey.Hitter)
    --     hitter:AddHitTrigger(recover_trigger)
    --     hp_beh:SetTrigger(recover_trigger)
    --     behaviour_queue:PushSyncBehaviour(hp_beh)
    --     BattleNodeEvent.popHitTrigger(atker, atker)
    --     if skill_id == 132 then
    --         hp_beh:SetDelayTime(0.3)
    --     end
    if trigger and eve_data.modify_desc ~= BATTLE_HP_DESC.COMBINE_ATTACK then
        hp_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(hp_beh)
    else
        behaviour_queue:PushQueueBehaviour(hp_beh)
    end
    if eve_data.modify_desc == BATTLE_HP_DESC.POISON or eve_data.modify_desc == BATTLE_HP_DESC.BURN then--中毒
        local beh = BattleBuff.New(grid_idx, {stage_key = "Poisoning"}, BATTLE_DATA_EVENT.PLAY_ALL_BUFF)
        if trigger then
            beh:SetTrigger(trigger)
        end
        behaviour_queue:PushSyncBehaviour(beh)
    end

    if asset_id == BattleHelper.ZhanFuGeDangXuLi then
        local beh = BattleBuff.New(grid_idx, {stage_key = "ZhanFuGeDang"}, BATTLE_DATA_EVENT.PLAY_ALL_BUFF)
        if trigger then
            beh:SetTrigger(trigger)
        end
        behaviour_queue:PushSyncBehaviour(beh)
    end

    if eve_data.modify_num < 0 then
        local show_behit = eve_data.cur_num > 0
        local audio_cfg
        --合击时血量归零了依然要播放被击动画，直到被最后一个攻击者打
        if show_behit == false and eve_data.modify_desc == BATTLE_HP_DESC.COMBINE_ATTACK then
            show_behit = BattleNodeEvent.getParam(ParamKey.CombineLastAtker) ~= atker
        end
        if eve_data.behit_desc == BATTLE_BE_HIT_DESC.DEFENCE then
            show_behit = true
        end
        if eve_data.modify_desc == BATTLE_HP_DESC.SACRIFICE_HP then
            show_behit = false
        end
        if BattleHelper.HIT_SELF_NOSHOW[asset_id] and grid_idx == atker then
            show_behit = false
        end
        if BattleHelper.SKILL_NO_BEHIT[asset_id] then
            show_behit = false
        end
        if BattleBuffManager.Instance:HasBingFeng(grid_idx) then--and eve_data.behit_desc == BATTLE_HP_DESC.DEFENCE then
            show_behit = false
        end
        if show_behit then
            --被击音效
            if skill_id == 1 then--被普攻攻击
                local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
                audio_cfg = AudioMgr:GetAttackName(ch_data)
            end
            --注意防御技能会影响某些技能触发，所以必须要等到防御技能表现播放完
            local be_hit_beh
            local defence = (eve_data.behit_desc == BATTLE_BE_HIT_DESC.DEFENCE or eve_data.behit_desc == BATTLE_BE_HIT_DESC.SHIELD)
            if asset_id ~= "" then
                be_hit_beh = BattleBehaviourBeHit.New(grid_idx, atker, defence, true, audio_cfg, BattleHelper.IGNORE_ACTOR_LIMIT[asset_id])
            else
                be_hit_beh = BattleBehaviourBeHit.New(grid_idx, atker, defence, true, audio_cfg)
            end
            if defence and not trigger then
                be_hit_beh.need_wait = true
            end
            be_hit_beh:SetTrigger(trigger)
            behaviour_queue:PushSyncBehaviour(be_hit_beh)
            --非石化 且 （中毒 或者 吸收盾)
            if not BattleHelper.SKILL_SHIHUA[asset_id] and (eve_data.modify_desc == BATTLE_HP_DESC.POISON
         or BattleNodeEvent.absorb_damage == true) then
                be_hit_beh.need_wait = true
            end
        end
    elseif eve_data.modify_num > 0 and eve_data.modify_num >= eve_data.cur_num then --复活
        local relive_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Idle, false)
        relive_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(relive_beh)
    end
end

--------------------COMBINE_ATTACK 4-----------------------------
function BattleNodeEvent.parseCombineAttack()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.combine_num = MsgAdapter.ReadShort()
    data.combine_list = {}
    for _ = 1, data.combine_num do
        table.insert(data.combine_list, MsgAdapter.ReadShort() + 1)
    end
    return data
end

function BattleNodeEvent.setDataCombineAttackBegin(grid_idx, eve_data)
    local num = eve_data.combine_num
    local index = 1
    for i = 1, num do
        local atker_id = eve_data.combine_list[i]
        local need_move = BattleNodeEvent.checkNeedMove(atker_id,{skill_id = 0, target_list = {eve_data.target_grid_idx}})
        if need_move then
            BattleNodeEvent.not_at_pos[grid_idx] = true
            local move_beh = BattleBehaviourMoveCombineHit.New(atker_id, eve_data.target_grid_idx, i, num)
            if index ~= 1 then
                behaviour_queue:PushSyncBehaviour(move_beh)
            else
                behaviour_queue:PushQueueBehaviour(move_beh)
            end
            move_beh:AddHitTrigger(BattleNodeEvent.pushHitTrigger(atker_id, eve_data.target_grid_idx))--这里应该改到onehit的地方暂时不改先这样，反而更快
            index = index + 1
        end
    end
end

function BattleNodeEvent.setDataCombineAttack(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.CombineBehit, eve_data.target_grid_idx)
    BattleNodeEvent.pushParam(ParamKey.CombineLastAtker, eve_data.combine_list[eve_data.combine_num])
end

function BattleNodeEvent.setDataCombineAttackEnd(grid_idx, eve_data)
    local move_beh = BattleBehaviourMove.New(eve_data.combine_list[1])
    behaviour_queue:PushQueueBehaviour(move_beh)

    for i = 1, eve_data.combine_num do
        local atker_id = eve_data.combine_list[i]
        if i ~= 1 then
            move_beh = BattleBehaviourMove.New(atker_id)
            behaviour_queue:PushSyncBehaviour(move_beh)
        end
        BattleNodeEvent.popHitTrigger(atker_id, eve_data.target_grid_idx)
    end
    BattleNodeEvent.popParam(ParamKey.CombineLastAtker)
    BattleNodeEvent.popParam(ParamKey.CombineBehit)
    BattleNodeEvent.show_once_zhimingyiji = nil
    BattleNodeEvent.not_at_pos[grid_idx] = false
end

--------------------RUN_AWAY 5-----------------------------

function BattleNodeEvent.parseRunAway()
    local data = {}
    data.is_run_away_succ = (MsgAdapter.ReadChar() == 1)
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataRunAway(grid_idx, eve_data)
    local run_away_beh = BattleBehaviourRunAway.New(grid_idx, eve_data.is_run_away_succ)
    -- 为了使逃跑行为在其他行为结束之后执行
    behaviour_queue:PushQueueBehaviour(run_away_beh)
    -- behaviour_queue:PushSyncBehaviour(run_away_beh)
    -- 预修改解析数据中的角色info数据，为后续解析需要使用的数据做准备
    if eve_data.is_run_away_succ then
        local idx = grid_idx
        BattleData.Instance:SetDataChInfo(idx, nil)
    end
end

--------------------ADD_BUFF_TO_LIST 8-----------------------------

function BattleNodeEvent.parseAddBuffToList()
    local data = {}
    data.new_buff_id = MsgAdapter.ReadShort()
    data.new_buff_level = MsgAdapter.ReadShort()
    data.new_buff_key = MsgAdapter.ReadUInt()
    data.sustain_num = MsgAdapter.ReadShort()
    data.buff_layer = MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataAddBuffToList(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local linker = BattleNodeEvent.getParam(ParamKey.LinkTarget)
    local trigger = nil
    if atker ~= nil then
        -- 提取当前攻击事件（即攻击者打我）的trigger
        trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    end
    if not trigger and linker ~= grid_idx then
        trigger = BattleNodeEvent.getHitTrigger(atker, linker)
    end
    local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.ADD_BUFF_TO_LIST)
    -- 技能播放完后才显示特效，所以加上trigger
    if trigger then
        beh:SetTrigger(trigger)
    end
    -- 材质变化且停止动画机的BUFF需要被延迟
    if not BattleHelper.BUFF_DELAY_PLAY[eve_data.new_buff_id] then
        behaviour_queue:PushSyncBehaviour(beh)
    end
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch_info then
        data_ch_info.buff_list[eve_data.new_buff_key] = eve_data.new_buff_id
    end
    if eve_data.new_buff_id == BattleHelper.WuDiBuff then
        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.BUFF_SHOW, eve_data.new_buff_id - 1)
        behaviour_queue:PushSyncBehaviour(popup_beh)
        if trigger then
            popup_beh:SetTrigger(trigger)
        end
    end
    --[[ if BattleData.IsHuiYan(grid_idx, eve_data.new_buff_id)then
        --慧眼给带隐身的对象加特效
        local chs = BattleData.Instance:IsHasInvisibleCh()
        local beh
        for _, idx in ipairs(chs) do
            beh = BattlePlayBuffAssetStage.New(idx, BattleHelper.HuiYanAsset, BuffStageKey.Persistence)
            behaviour_queue:PushSyncBehaviour(beh)
            if trigger then
                beh:SetTrigger(trigger)
            end
        end
    end ]]
    --判断荆轲铁马技能buff为目标添加特效
    if eve_data.new_buff_id == BattleHelper.JinGeTieMa then
        local target_list = BattleNodeEvent.getParam(ParamKey.TargetList)
        BattleNodeEvent.pushParam(ParamKey.BuffTargets, target_list)
        local data = {}
        data.config = SceneEffectConfig.EffectList.JinGeTieMa
        data.sign = "jingetiema"
        if target_list then
            for _, value in ipairs(target_list) do
                data.grid_idx = value
                beh = BattlePlayEffectOnCh.New(data)
                -- beh:SetDelayTime(0.8)
                if trigger then
                    beh:SetTrigger(trigger)
                end
                behaviour_queue:PushSyncBehaviour(beh)
            end
        end
    end
end

function BattleNodeEvent.setDataAddBuffToListAfterEnd(grid_idx, eve_data)
    local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.ADD_BUFF_TO_LIST)
    if BattleHelper.BUFF_DELAY_PLAY[eve_data.new_buff_id] then
        behaviour_queue:PushQueueBehaviour(beh)
    end
end

--------------------REPLACE_BUFF 6-----------------------------

function BattleNodeEvent.parseReplaceBuff()
    local data = {}
    data.new_buff_id = MsgAdapter.ReadShort()
    data.new_buff_level = MsgAdapter.ReadShort()
    data.new_buff_key = MsgAdapter.ReadUInt()
    data.replaced_buff_key = MsgAdapter.ReadUInt()
    data.sustain_num = MsgAdapter.ReadShort()
    data.buff_layer = MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataReplaceBuff(grid_idx, eve_data)
    local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.REPLACE_BUFF)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local trigger = nil
    if atker ~= nil then
        -- 提取当前攻击事件（即攻击者打我）的trigger
        trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    end
    -- 技能播放完后才显示特效，所以加上trigger
    if trigger then
        beh:SetTrigger(trigger)
    end
    behaviour_queue:PushSyncBehaviour(beh)
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch_info then
        data_ch_info.buff_list[eve_data.new_buff_key] = eve_data.new_buff_id
        data_ch_info.buff_list[eve_data.replaced_buff_key] = nil
    end
end

--------------------MERGE_BUFF 7-----------------------------

function BattleNodeEvent.parseMergeBuff()
    local data = {}
    data.new_buff_id = MsgAdapter.ReadShort()
    data.new_buff_level = MsgAdapter.ReadShort()
    data.new_buff_key = MsgAdapter.ReadUInt()--merge_buff_key
    data.sustain_num = MsgAdapter.ReadShort()
    data.buff_layer = MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataMergeBuff(grid_idx, eve_data)
    local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.MERGE_BUFF)
    behaviour_queue:PushSyncBehaviour(beh)
end

--------------------REMOVE_BUFF 9-----------------------------

function BattleNodeEvent.parseRemoveBuff()
    local data = {}
    data.removed_buff_key = MsgAdapter.ReadUInt()
    return data
end
function BattleNodeEvent.CheckUseUnseal(grid_idx)
    if BattleNodeEvent.UseItem ~= nil then
        --假设使用了解封丸那就不播放特效
        if BattleNodeEvent.UseItem.target_grid_idx == grid_idx then
            local config = Item.GetConfig(BattleNodeEvent.UseItem.item_id)
            if config.param2 == 0 and config.param3 > 0 then
                return true
            end
        end
    end
    return false
end
function BattleNodeEvent.setDataRemoveBuff(grid_idx, eve_data)
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    local buff_id
    if data_ch_info then
        buff_id = data_ch_info.buff_list[eve_data.removed_buff_key]
    end
    if buff_id then
        local cfg = BattleData.BuffConfig(buff_id)
        if cfg and BattleData.Instance:IsPurifyEffect(cfg.buff_type) == true then
            if not BattleNodeEvent.CheckUseUnseal(grid_idx) then
                --没有使用解封丸的情况下播放特效
                local beh = BattlePlayBuffAssetStart.New(grid_idx, BattleHelper.YiChangJieChu)
                behaviour_queue:PushSyncBehaviour(beh)
            end
        end
    end
    if buff_id and BattleHelper.BUFF_STOP_NOW[buff_id] == true then
        -- local atker = BattleNodeEvent.getParam(ParamKey.Atker)
        -- local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.REMOVE_BUFF)
        behaviour_queue:PushSyncBehaviour(beh)
    end
    
    --我慧眼没了 看不到隐身的角色 那些角色身上就没有慧眼特效了
    --[[ if BattleData.IsHuiYan(grid_idx, buff_id)then
        local chs = BattleData.Instance:IsHasInvisibleCh()
        local beh
        for _, idx in ipairs(chs) do
            beh = BattleRemoveBuffAsset.New(idx, BattleHelper.HuiYanAsset)
            behaviour_queue:PushSyncBehaviour(beh)
        end
    end ]]
    --我隐身没了所以慧眼特效就没了
    --[[ if buff_id and buff_id == BattleHelper.YinSheng then
        local beh = BattleRemoveBuffAsset.New(grid_idx, BattleHelper.HuiYanAsset)
        behaviour_queue:PushSyncBehaviour(beh)
    end ]]
    if buff_id and buff_id == BattleHelper.JinGeTieMa then
        local target_list = BattleNodeEvent.getParam(ParamKey.BuffTargets)
        local beh
        local data = {}
        data.sign = "jingetiema"
        if target_list then
            for _, value in ipairs(target_list) do
                data.grid_idx = value
                beh = BattleStopEffectOnCh.New(data)
                behaviour_queue:PushSyncBehaviour(beh)
            end
        end
    end
end
function BattleNodeEvent.setDataRemoveBuffEnd(grid_idx, eve_data)
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    local buff_id
    if data_ch_info then
        buff_id = data_ch_info.buff_list[eve_data.removed_buff_key]
    end
    if buff_id and BattleHelper.BUFF_STOP_NORMAL[buff_id] == true then
        local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.REMOVE_BUFF)
        -- beh:SetTrigger(trigger)
        -- 虚化屏障BUFF问题修改为queue否侧无法显示特效,因为还没播就remove了
        behaviour_queue:PushQueueBehaviour(beh)
    end
end
function BattleNodeEvent.setDataRemoveBuffAfterEnd(grid_idx, eve_data)
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    local buff_id
    if data_ch_info then
        buff_id = data_ch_info.buff_list[eve_data.removed_buff_key]
    end
    if (not buff_id or not BattleHelper.BUFF_STOP_NOW[buff_id]) or (buff_id and not BattleHelper.BUFF_STOP_NORMAL[buff_id]) then
        local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.REMOVE_BUFF)
        -- beh:SetTrigger(trigger)
        -- 虚化屏障BUFF问题修改为queue否侧无法显示特效,因为还没播就remove了
        behaviour_queue:PushQueueBehaviour(beh)
    end
end
--------------------SELF_DESTRUCT 10-----------------------------
function BattleNodeEvent.parseDataSelfDestruct()
    local data = {}
    data.type = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    data.param1 = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataSelfDestructBegin(grid_idx, eve_data)
    local move = true
    if eve_data.param1 > 0 then
        local config = Cfg.SpecialEffect(eve_data.param1)
        if config.effect_id_1 == BattleHelper.JueDiFanJi or config.effect_id_1 == BattleHelper.YanHuo then
            move = false
        end
    end
    if move then
        local is_enemy_idx = grid_idx > 10
        local move_idx = is_enemy_idx and 3 or 13
        local move_beh = BattleBehaviourMove.New(grid_idx, move_idx)
        behaviour_queue:PushQueueBehaviour(move_beh)
    end
end

function BattleNodeEvent.setDataSelfDestruct(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    BattleNodeEvent.pushParam(ParamKey.EventType, BATTLE_DATA_EVENT.SELF_DESTRUCT)

    local is_enemy_idx = grid_idx > 10
    local idx_off = is_enemy_idx and 0 or 10
    local targets = {}
    for i = 1, 10 do
        table.insert(targets, i + idx_off)
    end
    --改成目标只有活的
    local dest_beh = BattleSelfDestruct.New(grid_idx, targets, eve_data)
    behaviour_queue:PushQueueBehaviour(dest_beh)
    local setnil = true
    if eve_data.param1 > 0 then
        local config = Cfg.SpecialEffect(eve_data.param1)
        if config.effect_id_1 == BattleHelper.JueDiFanJi or config.effect_id_1 == BattleHelper.YanHuo then
            setnil = false
        end
    end
    if setnil then
        BattleData.Instance:SetDataChInfo(grid_idx, nil)
    end
end

function BattleNodeEvent.setDataSelfDestructEnd(grid_idx, eve_data)
    local is_enemy_idx = grid_idx > 10
    local idx_off = is_enemy_idx and 0 or 10
    for i = 1, 10 do
        BattleNodeEvent.popHitTrigger(grid_idx, i + idx_off)
    end
    BattleNodeEvent.popParam(ParamKey.Atker)
    BattleNodeEvent.popParam(ParamKey.EventType)
    BattleNodeEvent.show_once_zhimingyiji = nil
end

--------------BATTLE_DATA_EVENT.CHARACTER_DIE 11---------------------

function BattleNodeEvent.parseCharacterDie(eve_type, data_len)
    local data = {}
    data.die_type = MsgAdapter.ReadShort() ---0 死亡倒地,1 死亡倒地原地消失,2 死亡击飞消失,3 复活（凤凰涅槃）
    --data.die_desc = BATTLE_DIE_DESC[data.die_type]
    MsgAdapter.ReadShort() --reserve_sh
    return data
end

function BattleNodeEvent.setDataCharacterDieBegin(grid_idx, eve_data)
end

function BattleNodeEvent.setDataCharacterDieEnd(grid_idx, eve_data)
end

function BattleNodeEvent.setDataCharacterDie(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    if atker == nil then
        atker = BattleNodeEvent.getParam(ParamKey.CombineLastAtker)
    end
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    
    local not_in_pos_idx = BattleNodeEvent.getParam(ParamKey.NotAtPosition)
    local ch_not_in_pos = not_in_pos_idx == grid_idx
    --这里不在位置时会等待整个节点结束时去做死亡逻辑

    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId) or BattleNodeEvent.getParam(ParamKey.ComboSkillId)
    local asset_id = getSkillAssetId(skill_id)

    if BattleHelper.SKILL_JIANSHE[asset_id] then--溅射类技能
        local hit_trigger_behit_idx = grid_idx
        local behiter_idxs = BattleNodeEvent.getParam(ParamKey.BehiterIdxs)
        if behiter_idxs ~= nil and next(behiter_idxs) then
            hit_trigger_behit_idx = behiter_idxs[1]
        end
        trigger = BattleNodeEvent.getHitTrigger(atker, hit_trigger_behit_idx)
    end

    if eve_data.die_type ~= 2 and asset_id == BattleHelper.MFDanTiFengRenMoFaXuLi then
        trigger = BattleNodeEvent.getHitTrigger(BattleNodeEvent.GenSkillEventTriggerIdx(skill_id, atker, 1), grid_idx) or trigger
    end
    
    local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
    local audio_cfg = AudioMgr:GetAttackName(ch_data)
    if eve_data.die_type == 2 then
        local diaup_beh = BattleDiaup.New(grid_idx, audio_cfg)
        diaup_beh:SetTrigger(trigger)
        if asset_id == BattleHelper.MFDanTiFengRenMoFaXuLi then
            diaup_beh.dont_play_behit = true
        end
        if BattleNodeEvent.BehitDesc == BATTLE_BE_HIT_DESC.DEFENCE then
            diaup_beh.dont_play_behit = true
            BattleNodeEvent.BehitDesc = BATTLE_BE_HIT_DESC.NORMAL
        end
        if BattleBuffManager.Instance:HasShiHua(grid_idx) then
            diaup_beh.dont_play_behit = true
        end
        if BattleBuffManager.Instance:HasBingFeng(grid_idx) then
            diaup_beh.dont_play_behit = true
        end
        behaviour_queue:PushSyncBehaviour(diaup_beh)
        BattleData.Instance:SetDataChInfo(grid_idx, nil)
    elseif eve_data.die_type == 3 then
        local death_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Death, true)
        death_beh:SetTrigger(trigger)
        behaviour_queue:PushQueueBehaviour(death_beh)
        local relive_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Idle, false)
        behaviour_queue:PushQueueBehaviour(relive_beh)
        local nvwashi = BattleData.IsHasNvWaShiBuff(grid_idx)
        local relive_eff_beh = BattleBehaviourPerformSkillByAssetId.New(nvwashi and BattleHelper.NvWaShiFuHuo or BattleHelper.FengHuangNiePan, grid_idx, {grid_idx}, {grid_idx})
        behaviour_queue:PushSyncBehaviour(relive_eff_beh)
    elseif ch_not_in_pos == false then --角色在自己的位置上的话，直接死亡，否则延迟死亡表现
        if eve_data.die_type == 1 then
            --有不在位置上的情况
            if BattleNodeEvent.not_at_pos[grid_idx] == true then
                local move_beh = BattleBehaviourMove.New(grid_idx, grid_idx)
                behaviour_queue:PushQueueBehaviour(move_beh)
                behaviour_queue:PushCutOff()
            end
            local diedis_beh = BattleDieDisappear.New(grid_idx, audio_cfg)
            if BattleNodeEvent.daxiong_skill_hit_trigger then
                --trigger 导致死亡提前播放（大凶技能播放中断），重新设置
                if grid_idx == BattleNodeEvent.daxiong_skill_hit_trigger.hit_idx then
                    trigger = BattleNodeEvent.daxiong_skill_hit_trigger
                end
                BattleNodeEvent.daxiong_skill_hit_trigger = nil
            end
            diedis_beh:SetTrigger(trigger)
            behaviour_queue:PushSyncBehaviour(diedis_beh)
            BattleData.Instance:SetDataChInfo(grid_idx, nil)
        else
            local event_type = BattleNodeEvent.getParam(ParamKey.EventType)
            if event_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
                local need_move = BattleNodeEvent.checkNeedMove(grid_idx, {skill_id = skill_id})
                if asset_id ~= "" and need_move then
                    local move_beh = BattleBehaviourMove.New(grid_idx, grid_idx)
                    behaviour_queue:PushQueueBehaviour(move_beh)
                    behaviour_queue:PushCutOff()
                end
            elseif event_type == BATTLE_DATA_EVENT.SELF_DESTRUCT then
                local move_beh = BattleBehaviourMove.New(grid_idx, grid_idx)
                behaviour_queue:PushQueueBehaviour(move_beh)
                behaviour_queue:PushCutOff()
            end
            local death_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Death, false, audio_cfg)
            death_beh:SetTrigger(trigger)
            behaviour_queue:PushSyncBehaviour(death_beh)
            BattleData.Instance:SetDataChInfo(grid_idx, nil)
        end
    end

    local die_hdl_beh = BattleDieHandle.New(grid_idx, eve_data.die_type)
    behaviour_queue:PushSyncBehaviour(die_hdl_beh)

    if ch_not_in_pos then
        if eve_data.die_type ~= 2 then --不在自己位置上未被击飞，播放被击
            local be_hit_beh = BattleBehaviourBeHit.New(grid_idx, atker, false, false, audio_cfg)
            be_hit_beh:SetTrigger(trigger)
            -- be_hit_beh.need_wait = true
            behaviour_queue:PushSyncBehaviour(be_hit_beh)
        end
        BattleNodeEvent.setParam(ParamKey.NotAtPosition, not_in_pos_idx + eve_data.die_type + 1)
    end
end
--------------BATTLE_DATA_EVENT.COUNTER_ATTACK 12---------------------

function BattleNodeEvent.parseCounterAttack(eve_type, data_len)
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1 --反击目标
    MsgAdapter.ReadShort() --reserve_sh
    return data
end

function BattleNodeEvent.setDataCounterAttack(grid_idx, eve_data)
    -- BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    BattleNodeEvent.pushParam(ParamKey.AtkBacker, grid_idx)
    local behiter_idxs = {}
    BattleNodeEvent.pushParam(ParamKey.BehiterIdxs, behiter_idxs)

    local hit_trigger = BattleNodeEvent.pushHitTrigger(grid_idx, eve_data.target_grid_idx)
    local data_ch_info = BattleData.Instance:GetDataChInfo(grid_idx)
    local asset_name = BattleHelper.FanJi
    if data_ch_info then
        for buff_key, buff_id in pairs(data_ch_info.buff_list) do
            if BattleHelper.FanJiAsset[buff_id] then
                asset_name = BattleHelper.FanJiAsset[buff_id]
                break
            end
        end
    end
    
    pushAttackAudio(grid_idx)
    
    local beh = BattleBehaviourPerformSkillByAssetId.New(asset_name, grid_idx, {eve_data.target_grid_idx}, behiter_idxs)
    BattleNodeEvent.pushParam(ParamKey.Hitter, beh)
    behaviour_queue:PushQueueBehaviour(beh)
    beh:AddHitTrigger(hit_trigger)
    if asset_name == BattleHelper.FanJiHouFaZhiRen or asset_name == BattleHelper.FanJi then
        -- local hp_beh = BattleHpChange.New(grid_idx, 0, -1, BATTLE_HP_DESC.IN_VAIN, 0, 0, atker, BattleNodeEvent.detail_player)
        local hp_beh = BattleHpChange.New(grid_idx, 0, -1, BATTLE_HP_DESC.ATTACK_BACK, 0, 0, grid_idx, BattleNodeEvent.detail_player)
        hp_beh:SetTrigger(hit_trigger)
        behaviour_queue:PushSyncBehaviour(hp_beh)
    end
end

function BattleNodeEvent.setDataCounterAttackEnd(grid_idx, eve_data)
    BattleNodeEvent.popParam(ParamKey.BehiterIdxs)
    -- BattleNodeEvent.popParam(ParamKey.Atker)
    BattleNodeEvent.popParam(ParamKey.AtkBacker)
    BattleNodeEvent.popParam(ParamKey.Hitter)
    BattleNodeEvent.popHitTrigger(grid_idx, eve_data.target_grid_idx)
    BattleNodeEvent.show_once_zhimingyiji = nil
end

--------------BATTLE_DATA_EVENT.BE_GUARD 13---------------------

function BattleNodeEvent.parseBeGuard(eve_type, data_len)
    local data = {}
    data.guardian_grid_idx = MsgAdapter.ReadShort() + 1 --被谁护卫
    MsgAdapter.ReadShort() --reserve_sh
    return data
end

function BattleNodeEvent.setDataBeGuardBegin(grid_idx, eve_data)
    if grid_idx == eve_data.guardian_grid_idx then
        LogError("护卫数据异常！不能自己护卫自己 ", grid_idx)
    end
    BattleNodeEvent.pushParam(ParamKey.NotAtPosition, eve_data.guardian_grid_idx)
    BattleNodeEvent.pushParam(ParamKey.BeGuarer, grid_idx)
    BattleNodeEvent.pushParam(ParamKey.Guarder, eve_data.guardian_grid_idx)
end

function BattleNodeEvent.setDataBeGuardPreBegin(grid_idx, eve_data)
    -- BattleNodeEvent.pushParam(ParamKey.NotAtPosition,eve_data.guardian_grid_idx)
    local guard_move = BattleMove.New(eve_data.guardian_grid_idx, grid_idx, true, nil, 2)
    behaviour_queue:PushQueueBehaviour(guard_move)
    local guard_buff = BattleBuff.New(eve_data.guardian_grid_idx, {stage_key = "Guard"}, BATTLE_DATA_EVENT.PLAY_ALL_BUFF)
    guard_buff:AddToActorList(grid_idx)
    behaviour_queue:PushSyncBehaviour(guard_buff)
    local dod_move = BattleMove.New(grid_idx, grid_idx, false, Vector3.New(0, 0, -1))
    dod_move:AddToActorList(eve_data.guardian_grid_idx)
    behaviour_queue:PushSyncBehaviour(dod_move)
end

function BattleNodeEvent.setDataBeGuard(grid_idx, eve_data)
end

function BattleNodeEvent.setDataBeGuardEnd(grid_idx, eve_data)
    BattleNodeEvent.popParam(ParamKey.Guarder)
    BattleNodeEvent.popParam(ParamKey.BeGuarer)
    local post_guardian_idx = BattleNodeEvent.popParam(ParamKey.NotAtPosition)
    local die_flag = post_guardian_idx - eve_data.guardian_grid_idx - 1

    if die_flag == 0 then --回位置，死亡倒地
        local guard_move = BattleMove.New(eve_data.guardian_grid_idx, eve_data.guardian_grid_idx, true, nil, 2)
        behaviour_queue:PushQueueBehaviour(guard_move)
        local dod_move = BattleMove.New(grid_idx, grid_idx, false)
        behaviour_queue:PushSyncBehaviour(dod_move)
        local death_beh = BattleBehaviourPlayAnim.New(eve_data.guardian_grid_idx, ChAnimType.Death, false)
        behaviour_queue:PushQueueBehaviour(death_beh)
    elseif die_flag == 1 then --	回位置，死亡消失
        local guard_move = BattleMove.New(eve_data.guardian_grid_idx, eve_data.guardian_grid_idx, true, nil, 2)
        behaviour_queue:PushQueueBehaviour(guard_move)
        local dod_move = BattleMove.New(grid_idx, grid_idx, false)
        behaviour_queue:PushSyncBehaviour(dod_move)
        local diedis_beh = BattleDieDisappear.New(eve_data.guardian_grid_idx)
        behaviour_queue:PushQueueBehaviour(diedis_beh)
    elseif die_flag == 2 then --不回位置，已经被击飞
        local dod_move = BattleMove.New(grid_idx, grid_idx, false)
        behaviour_queue:PushQueueBehaviour(dod_move)
    else --正常回位置
        local guard_move = BattleMove.New(eve_data.guardian_grid_idx, eve_data.guardian_grid_idx, true, nil, 2)
        behaviour_queue:PushQueueBehaviour(guard_move)
        local dod_move = BattleMove.New(grid_idx, grid_idx, false)
        behaviour_queue:PushSyncBehaviour(dod_move)
    end
end

--------------BATTLE_DATA_EVENT.CHANGE_POSITION 14---------------------
function BattleNodeEvent.setDataChangePosition(grid_idx, eve_data)
    local change_pos_beh = BattleChangePosition.New(grid_idx, BattleNodeEvent.detail_player)
    behaviour_queue:PushQueueBehaviour(change_pos_beh)
    --预修改解析数据中的角色info数据，为后续解析需要使用的数据做准备
    local idx1 = grid_idx
    local isFront = BattleData.IsFrontIdx(idx1)
    local idx2 = isFront and (idx1 - 5) or (idx1 + 5)
    local ch_info1 = BattleData.Instance:GetDataChInfo(idx1)
    local ch_info2 = BattleData.Instance:GetDataChInfo(idx2)
    if ch_info1 then
        ch_info1.grid_idx = idx2
    end
    if ch_info2 then
        ch_info2.grid_idx = idx1
    end--存在死亡的情况
    local isFront = BattleData.IsFrontIdx(grid_idx)
    if ch_info1 then
        local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.CHANGE_POSITION][isFront and 2 or 1], ch_info1.name)
        pushDesc(BATTLE_DATA_EVENT.CHANGE_POSITION, desc)
    end

    BattleData.Instance:SetDataChInfo(idx1, ch_info2)
    BattleData.Instance:SetDataChInfo(idx2, ch_info1)
end

--------------BATTLE_DATA_EVENT.USE_ITEM 15---------------------
function BattleNodeEvent.parseUseItem(eve_type, data_len)
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1 --使用道具的目标
    data.item_id = MsgAdapter.ReadUShort() --reserve_sh
    data.is_use_fake_potion = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataUseItemBegin(grid_idx, eve_data)
end

function BattleNodeEvent.setDataUseItem(grid_idx, eve_data)
    BattleNodeEvent.UseItem = eve_data
    BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    local use_beh = BattleBehaviourUseItemSkill.New(grid_idx, eve_data.target_grid_idx, eve_data.item_id)
    behaviour_queue:PushQueueBehaviour(use_beh)
    local hit_trigger = BattleNodeEvent.pushHitTrigger(grid_idx, eve_data.target_grid_idx)
    use_beh:AddHitTrigger(hit_trigger)
    if eve_data.is_use_fake_potion == 1 then
        local list = BattleData.Instance:BaseData().fake_potion_list
        local temp = {}
        for key, value in ipairs(list) do
            if value.item_id == eve_data.item_id then
                value.num = value.num - 1
            end
            if value.num <= 0 then
                list[key] = nil
            end
        end
        --对假药列表进行更新
        BattleData.Instance:SetFakePotion(BattleData.Instance:BaseData().fake_potion_num,list)
    end
end

function BattleNodeEvent.setDataUseItemEnd(grid_idx, eve_data)
    BattleNodeEvent.popHitTrigger(grid_idx, eve_data.target_grid_idx)
    BattleNodeEvent.popParam(ParamKey.Atker)
    BattleNodeEvent.UseItem = nil
end

--------------BATTLE_DATA_EVENT.CHANGE_EQUIPMENT 16---------------------
function BattleNodeEvent.parseChangeEquipment(eve_type, data_len)
    local data = {}
    data.before_equip_id = MsgAdapter.ReadUShort()
    data.after_equip_id = MsgAdapter.ReadUShort()
    return data
end

function BattleNodeEvent.setDataChangeEquipment(grid_idx, eve_data)
    local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.CHANGE_EQUIPMENT], packingName(grid_idx))
    pushDesc(BATTLE_DATA_EVENT.CHANGE_EQUIPMENT, desc)

    local use_beh = BattleBehaviourChangeEquipment.New(grid_idx, eve_data.before_equip_id, eve_data.after_equip_id, BattleNodeEvent.detail_player)
    behaviour_queue:PushQueueBehaviour(use_beh)
end

--*单次合击
--------------BATTLE_DATA_EVENT.COMBINE_ATTACK_PERFORM_ONE_HIT 22---------------------

-- function BattleNodeEvent.setDataCombineAttackPerformOneHitBegin(grid_idx,eve_data)
-- end

function BattleNodeEvent.setDataCombineAttackPerformOneHit(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)

    local cc = BattleNodeEvent.popParam(ParamKey.CombineCount) or 0
    BattleNodeEvent.pushParam(ParamKey.CombineCount, cc + 1)

    local behiter_idxs = {}
    BattleNodeEvent.pushParam(ParamKey.BehiterIdxs, behiter_idxs)

    local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
    local config = AudioMgr:GetAttackName(ch_data)

    local be_hit_idx = BattleNodeEvent.getParam(ParamKey.CombineBehit)
    table.insert(behiter_idxs, be_hit_idx)

    local atk_beh = BattleBehaviourPerformAttack.New(grid_idx, {be_hit_idx}, behiter_idxs, config)
   --  atk_beh:AddHitTrigger(BattleNodeEvent.pushHitTrigger(grid_idx, be_hit_idx))
    behaviour_queue:PushQueueBehaviour(atk_beh)
end

function BattleNodeEvent.setDataCombineAttackPerformOneHitEnd(grid_idx, eve_data)
    BattleNodeEvent.popParam(ParamKey.BehiterIdxs)
    BattleNodeEvent.popParam(ParamKey.Atker)
end

--------------BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER 19---------------------
-- *角色信息
Appearance = function()
    local appearance_info = 
    {
        prof_base = MsgAdapter.ReadChar(), --职业[0,9)
        --进阶次数[0,4)
        advance_time = MsgAdapter.ReadChar(),
        --角色模型[1,6]
        avatar_type = MsgAdapter.ReadChar(),
        --颜色[0,4)
        color = MsgAdapter.ReadChar(),
        --武器id
        weapon_id = MsgAdapter.ReadUShort(),
        --副武器id
        sub_weapon_id = MsgAdapter.ReadUShort(),
        
        avatar_id = MsgAdapter.ReadShort(),--人物头像

        special_appearance_type = MsgAdapter.ReadShort(),
        special_appearance_param = MsgAdapter.ReadInt(),

        --伙伴、宠物皮肤id
        skin_id = MsgAdapter.ReadShort(),
        --幻化时装
        fashion_body_id = IllusionData.Instance:FashionIdToResId(MsgAdapter.ReadShort()) or 0,
        
        --幻化武器
        fashion_weapon_id = IllusionData.Instance:FashionIdToResId(MsgAdapter.ReadShort()) or 0,
        --幻化羽翼
        MsgAdapter.ReadShort(),
        --幻化坐骑
        ride_id = IllusionData.Instance:FashionIdToResId(MsgAdapter.ReadShort()) or 0,
        --幻化保留
        fashion_reserve_s2 = MsgAdapter.ReadShort(),

        -- 灵骑信息
        bicycle_index = MsgAdapter.ReadShort(),     -- -1 未乘坐灵骑
        bicycle_model_type = MsgAdapter.ReadChar(), -- 0 : 普通灵骑形象，1 : 进阶灵骑形象

        -- 变身卡信息 <废弃，新版幻兽>
        model_card_flag = MsgAdapter.ReadChar(),    -- 0 不使用形象，1使用
        model_card_seq = MsgAdapter.ReadShort(),    -- 变身卡seq

        fly_flag = MsgAdapter.ReadShort(),--飞升阶段 0无飞升 1完成初阶飞升（飞升一） 2完成高阶飞升（飞升二）

        zaohua_app_id = MsgAdapter.ReadInt(), -- 造化·罡气

        -- 幻兽信息
        reserve_char = MsgAdapter.ReadChar(),
        is_use_huanshou = MsgAdapter.ReadChar(),    -- 是否使用幻兽形象
        huanshou_id = MsgAdapter.ReadShort(),       -- 幻兽id


        reserve_ll2 = MsgAdapter.ReadLL(),
    }
    appearance_info.fly_flag = appearance_info.fly_flag > FlyUpStageType.High and FlyUpStageType.High or appearance_info.fly_flag
    appearance_info.avatar_quality = appearance_info.fly_flag --角色头像品质
    return appearance_info
end

function BattleNodeEvent.parseCharacterInfo(all)
    local ch = {root_obj = nil, height = 2.5, show_sel = false}
    ch.character_id = MsgAdapter.ReadInt()
    ch.character_type = MsgAdapter.ReadShort() --[0:主角 1:宠物 2:怪物 3:伙伴 7:助战]
    ch.level = MsgAdapter.ReadShort()
    ch.prof = MsgAdapter.ReadInt()
    ch.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    if ch.character_type == BATTLE_CH_TYPE.YOU_MING then
        local cfg = YouMingData.Instance:GetYouMingCfg(ch.character_id)
        if cfg then
            ch.name = cfg.name
        end
    end
    if StringIsEmpty(ch.name) then
       BuglyReport("录像或战斗协议出错")
       PublicPopupCtrl.Instance:Center("解析失败，无法进行该操作")
    end
    ch.weapon_type = MsgAdapter.ReadChar() --武器类型 [0:空手 1:弓 2:剑 3:斧 4:杖 5:枪 6:拳套]
    ch.is_chaos_element = MsgAdapter.ReadChar()
    ch.weapon_id = MsgAdapter.ReadUShort()
    ch.maxhp = MsgAdapter.ReadInt()
    ch.maxmp = MsgAdapter.ReadInt()
    ch.curhp = MsgAdapter.ReadInt()
    ch.curmp = MsgAdapter.ReadInt()
    --ele
    ch.element = {}
    ch.element[BattleAttriElement.Water] = MsgAdapter.ReadShort()
    ch.element[BattleAttriElement.Fire] = MsgAdapter.ReadShort()
    ch.element[BattleAttriElement.Wind] = MsgAdapter.ReadShort()
    ch.element[BattleAttriElement.Earth] = MsgAdapter.ReadShort()
    ch.race = MsgAdapter.ReadChar()
    ch.param = MsgAdapter.ReadChar()
    if ch.character_type == BATTLE_CH_TYPE.PARTNER or ch.character_type == BATTLE_CH_TYPE.PARTNER_ROBOT then
        ch.param = ch.param == 0 and 1 or ch.param
    end
    -- 角色类型为宠物时：宠物强化度
    -- ch.is_auto = MsgAdapter.ReadChar()
    ch.grid_idx = MsgAdapter.ReadShort() + 1
    ch.appearance = Appearance()
    ch.skin_id = MsgAdapter.ReadInt()
    ch.top_level = MsgAdapter.ReadInt()
    ch.hp_shield = MsgAdapter.ReadInt()
    ch.frozen_shield = MsgAdapter.ReadInt()
    ch.frozen_shield_full = MsgAdapter.ReadInt()
    ch.spirit_point = MsgAdapter.ReadInt()          -- -1不显示怒气，>=0 显示怒气值条
    ch.fly_flag = MsgAdapter.ReadInt()
    ch.fly_flag = ch.fly_flag > FlyUpStageType.High and FlyUpStageType.High or ch.fly_flag
    ch.avatar_quality = ch.fly_flag
    ch.name_color = MsgAdapter.ReadShort()--0正常1炼妖紫色
    ch.reserve_sh = MsgAdapter.ReadShort()
    ch.reserve_int = MsgAdapter.ReadInt()

    --LogError("spirit_point",ch.spirit_point)
    -- LogErrorBT("战斗角色信息", ch.grid_idx, ch)
    if FreedomCode.BattleXinMoSum == true and ch.name == Language.Common.XinMo then
        if BattleData.Instance:GetBossGroupId() ~= BattleHelper.XinMoBattleGroup then
            BattleNodeEvent.ConvertToXinMo(ch, all)
        else
            if ch.grid_idx == BattleHelper.BossPos then
                BattleNodeEvent.ConvertToXinMo(ch, all)
            end
        end
    end
    return ch
end
function BattleNodeEvent.ConvertToXinMo(ch, all)
    local player = BattleData.Instance:GetPlayerChInfo()
    if player == nil then
        if all then
            for index, value in ipairs(all) do
                if value and value.character_type == 0 then
                    player = value
                    break
                end
            end
        end
    end
    if player then
        ch.character_type = 0
        ch.temp_character_type = 2
        ch.inputting = false
        ch.appearance = DataHelper.TableCopy(player.appearance)
        ch.appearance.fashion_body_id = 0
        ch.appearance.advance_time = 1
        ch.appearance.color = 100
        ch.prof = player.prof-- RoleData.ProfIdToProfStage(player.prof)
        ch.appearance.weapon_type = 0
        ch.appearance.weapon_id = 0
    end
end
function BattleNodeEvent.parseBuffInfo()
    local data = {}
    data.new_buff_id = MsgAdapter.ReadShort()
    data.new_buff_level = MsgAdapter.ReadShort()
    data.sustain_num = MsgAdapter.ReadShort()
    data.grid_idx = MsgAdapter.ReadShort() + 1
    data.new_buff_key = MsgAdapter.ReadUInt()
    return data
end

function BattleNodeEvent.parseDataSummonCharacter()
    local data = {}
    -- 列表中的宠物index，如果无效就-1
    data.summon_pet_idx = MsgAdapter.ReadInt()
    data.character_info = BattleNodeEvent.parseCharacterInfo()
    return data
end
--------------BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER 19---------------------
function BattleNodeEvent.setDataSummonCharacter(grid_idx, eve_data)
    local summon_reason = BattleNodeEvent.getParam(ParamKey.SummonReason)
    local summon_mutiple = BattleNodeEvent.getParam(ParamKey.SummonMutiple)
    if summon_mutiple == 1 then return end
    --共工召唤
    if BattleHelper.BATTLE_XINGTIAN_JUQING(BattleData.Instance:BaseData().monster_group_id) and eve_data.character_info.name == Language.Common.GongGong then
        local ani_beh = BattleClipStandBy.New("gg_chuchang_cam")
        behaviour_queue:PushQueueBehaviour(ani_beh)
        local sum_idx = eve_data.character_info.grid_idx
        local sum_beh = BattleSummonSpecial.New(eve_data.character_info, BattleNodeEvent.detail_player)
        behaviour_queue:PushQueueBehaviour(sum_beh)
        BattleData.Instance:SetDataChInfo(sum_idx, eve_data.character_info)
        eve_data.character_info.buff_list = SmartData.New({})
        return
    end
    local summer = BattleData.Instance:GetDataChInfo(grid_idx)
    if grid_idx == 0 then
        summer = nil
    end
    if summer or summon_reason == 6 then
        --描述谁召唤了谁
        local temp = {}
        temp.name = summer and summer.name or eve_data.character_info.name
        if not BattleData.IsEnemyIdx(grid_idx) then
            temp.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", temp.name)
        end
        local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER], temp.name, eve_data.character_info.name)
        pushDesc(BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER, desc)
    end
    local sum_idx = eve_data.character_info.grid_idx
    local sum_beh = BattleSummon.New(eve_data.character_info, grid_idx, eve_data.summon_pet_idx, summon_reason, BattleNodeEvent.detail_player, summer == nil, grid_idx == 0)
    sum_beh.pet_summer = (grid_idx == sum_idx)
    sum_beh.real_pos = grid_idx
    if sum_beh.pet_summer == true then
        behaviour_queue:PushCutOff()
        behaviour_queue:PushCutOff()
    end
    behaviour_queue:PushQueueBehaviour(sum_beh)
    BattleData.Instance:SetDataChInfo(sum_idx, eve_data.character_info)
    eve_data.character_info.buff_list = SmartData.New({})
    if eve_data.character_info.name == Language.Common.JiangZiYa 
    and BattleData.Instance:GetBossGroupId() == BattleHelper.JiangZiYaGroup then
        local data = {}
        data.config = SceneEffectConfig.EffectList.JiangZiYaEffect
        data.sign = "jiangziyatexiao"
        data.grid_idx = sum_idx
        data.auto_release = false
        local eff_beh = BattlePlayEffectOnCh.New(data)
        behaviour_queue:PushSyncBehaviour(eff_beh)
    end
end

--------------BATTLE_DATA_EVENT.SUMMON 20---------------------
function BattleNodeEvent.parseDataSummon()
    local data = {}
    data.summon_reason = MsgAdapter.ReadShort()
    data.is_summon_mutiple = MsgAdapter.ReadShort()-- 0/1
    return data
end
function BattleNodeEvent.setdataSummon(grid_idx, eve_data, child_data)
    BattleNodeEvent.pushParam(ParamKey.SummonReason, eve_data.summon_reason)
    BattleNodeEvent.pushParam(ParamKey.SummonMutiple, eve_data.is_summon_mutiple)
    
    if eve_data.is_summon_mutiple == 1 and child_data then
        local sum_beh = BattleSummonMutiple.New(child_data, grid_idx, eve_data.summon_reason)
        behaviour_queue:PushCutOff()
        behaviour_queue:PushCutOff()
        behaviour_queue:PushQueueBehaviour(sum_beh)
        for index, value in ipairs(child_data) do
            if value.data and value.data.character_info then
                BattleData.Instance:SetDataChInfo(value.data.character_info.grid_idx, value.data.character_info)
                value.data.character_info.buff_list = SmartData.New({})
            end
        end
    elseif eve_data.is_summon_mutiple == 1 and child_data == nil then
        LogDG("多人召唤没有召唤对象的节点，解析失败！！！！！！！！！")
    end
end
function BattleNodeEvent:setDataSummonEnd()
    BattleNodeEvent.popParam(ParamKey.SummonReason)
    BattleNodeEvent.popParam(ParamKey.SummonMutiple)
end

--------------BATTLE_DATA_EVENT.RETRIEVE_PET 21---------------------
function BattleNodeEvent.parseDataRetrievePet()
    local data = {}
    data.caller_grid_idx = MsgAdapter.ReadInt() + 1
    return data
end
function BattleNodeEvent.setDataRetrievePet(grid_idx, eve_data)
    -- local atk_beh = BattleBehaviourPlayAnim.New(grid_idx, ChAnimType.Attack)
    -- behaviour_queue:PushQueueBehaviour(atk_beh)
    local is_front = BattleData.IsFrontIdx(grid_idx)
    local retrieve_idx = eve_data.caller_grid_idx --is_front and (grid_idx - 5) or (grid_idx + 5)

    local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.RETRIEVE_PET], packingName(retrieve_idx), packingName(grid_idx))
    pushDesc(BATTLE_DATA_EVENT.RETRIEVE_PET, desc)
    -- local trigger = BattleNodeEvent.pushHitTrigger(grid_idx, retrieve_idx)
    local ret_beh = BattleRetrieve.New(retrieve_idx, grid_idx , BattleNodeEvent.detail_player)
    -- ret_beh:SetTrigger(trigger)
    behaviour_queue:PushQueueBehaviour(ret_beh)

    BattleData.Instance:SetDataChInfo(grid_idx, nil)

    -- BattleNodeEvent.popHitTrigger(grid_idx, retrieve_idx)
end

--------------BATTLE_DATA_EVENT.MODIFY_MP 23---------------------

function BattleNodeEvent.parseModifyMp()
    local data = {}
    data.modify_num = MsgAdapter.ReadInt()
    data.cur_num = MsgAdapter.ReadInt()
    data.modify_desc = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataModifyMp(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    if skill_id and atker then
        local config = SkillData.GetConfig(skill_id)
        local desc =
            string.format(
            Language.BattleDetail[BATTLE_DATA_EVENT.MODIFY_MP][eve_data.modify_desc ~= 7 and 1 or 2],
            packingName(atker),
            config.skill_name,
            packingName(grid_idx),
            eve_data.modify_desc ~= 7 and -eve_data.modify_num or eve_data.modify_num
        )
        pushDesc(BATTLE_DATA_EVENT.MODIFY_MP, desc)
    end

    if eve_data.modify_desc == 1 then --醉酒
        local beh = BattleBuff.New(grid_idx, {stage_key = "Drunk"}, BATTLE_DATA_EVENT.PLAY_ALL_BUFF)
        if trigger then
            beh:SetTrigger(trigger)
        end
        behaviour_queue:PushSyncBehaviour(beh)
    end

    local mp_beh = BattleMpChange.New(grid_idx, eve_data.modify_num, eve_data.cur_num, eve_data.modify_desc, BattleNodeEvent.detail_player)
    if trigger then
        mp_beh:SetTrigger(trigger)
    end
    behaviour_queue:PushSyncBehaviour(mp_beh)
    --[[ if eve_data.modify_desc == 1 then
        behaviour_queue:PushCutOff()
    end ]]
end

--------------BATTLE_DATA_EVENT.COMBINE_ATTACK_TOTAL 24---------------------
function BattleNodeEvent.parseCombineAttackTotal()
    local data = {}
    data.total_damage = -MsgAdapter.ReadInt()
    return data
end

function BattleNodeEvent.setDataCombineAttackTotal(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.CombineLastAtker)
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    local cc = BattleNodeEvent.popParam(ParamKey.CombineCount)
    local hp_beh = BattleHpChange.New(grid_idx, eve_data.total_damage, -1, BATTLE_HP_DESC.COMBINE_ATTACK, cc, BATTLE_BE_HIT_DESC.NORMAL,atker,BattleNodeEvent.detail_player)
    hp_beh:SetTrigger(trigger)
    behaviour_queue:PushSyncBehaviour(hp_beh)
end

--------------BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL 25---------------------

function BattleNodeEvent.parseComboSkill()
    local data = {}
    data.skill_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort() --reserve_sh
    return data
end

function BattleNodeEvent.setDataComboSkillBegin(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.TotalDamage, {})
    BattleNodeEvent.pushParam(ParamKey.ComboSkillId, eve_data.skill_id)
    BattleNodeEvent.pushParam(ParamKey.ComboIdx, grid_idx)
    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, eve_data)
    if not need_move and eve_data.skill_id >= 100 then
        -- local skill_cfg = SkillData.GetConfig(eve_data.skill_id)
        -- local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SKILL_HIT], packingName(grid_idx), skill_cfg.skill_name)
        -- pushDesc(BATTLE_DATA_EVENT.SKILL_HIT, desc)

        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.SKILL, eve_data.skill_id, BattleNodeEvent.detail_player)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
    BattleNodeEvent.ComboPopup = true
    BattleNodeEvent.ComboNum = 0
end
function BattleNodeEvent.setDataComboSkill(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.EventType, BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL)
end

function BattleNodeEvent.setDataComboSkillEnd(grid_idx, eve_data)
    pushTotalDamageBeh()
    BattleNodeEvent.popParam(ParamKey.ComboSkillId)
    BattleNodeEvent.popParam(ParamKey.ComboIdx)

    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)

    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, {skill_id = eve_data.skill_id})
    if data_ch and need_move then
        local move_beh = BattleMove.New(grid_idx, grid_idx, true, Vector3.zero, 2)
        behaviour_queue:PushQueueBehaviour(move_beh)
    end
    BattleNodeEvent.not_at_pos[grid_idx] = false
    BattleNodeEvent.popParam(ParamKey.EventType)
    BattleNodeEvent.ComboNum = 0
    BattleNodeEvent.show_once_zhimingyiji = nil
end

--------------BATTLE_DATA_EVENT.COMBO_SKILL_ONE_HIT 26---------------------

function BattleNodeEvent.parseComboSkillOneHit()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1 --攻击目标
    data.skill_id = MsgAdapter.ReadShort() --reserve_sh
    return data
end

function BattleNodeEvent.setComboSkillOneHitBegin(grid_idx, eve_data)
    local combo_skill_id = BattleNodeEvent.getParam(ParamKey.ComboSkillId)
    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)

    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, {skill_id = combo_skill_id, target_list = {eve_data.target_grid_idx}})
    if data_ch and need_move then
        local move_beh = BattleBehaviourMove.New(grid_idx, eve_data.target_grid_idx)
        behaviour_queue:PushQueueBehaviour(move_beh)
        BattleNodeEvent.not_at_pos[grid_idx] = true
    end

end

function BattleNodeEvent.setComboSkillOneHit(grid_idx, eve_data)
    BattleNodeEvent.ComboNum = BattleNodeEvent.ComboNum + 1
    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch == nil then
        return
    end
    
    local combo_skill_id = BattleNodeEvent.getParam(ParamKey.ComboSkillId)
    local is_miao_sha = false
    local version
    if eve_data.skill_id and eve_data.skill_id > 0 then
        combo_skill_id = eve_data.skill_id
        BattleNodeEvent.setParam(ParamKey.ComboSkillId, combo_skill_id)
        is_miao_sha = true
        --秒杀分成两个二版本
        local ch = BattleData.Instance:GetChInfo(grid_idx)
        if ch and (ch.character_type == BATTLE_CH_TYPE.PET or ch.character_type == BATTLE_CH_TYPE.PET_SHADOW)then
            --默认版本1 青蛙版本 版本2非青蛙版本
            local cfg = PetData.Instance:GetPetCfg(ch.character_id)
            if cfg and cfg.prototype_id ~= 50152 then
                version = 2
            end
        end
    end
    local asset_id = getSkillAssetId(combo_skill_id)

    BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    local behiter_idxs = {}
    BattleNodeEvent.pushParam(ParamKey.BehiterIdxs, behiter_idxs)

    local hit_trigger = BattleNodeEvent.pushHitTrigger(grid_idx, eve_data.target_grid_idx)
    
    local need_move = BattleNodeEvent.checkNeedMove(grid_idx, {skill_id = combo_skill_id, target_list = {}})
    if need_move and combo_skill_id >= 100 and BattleNodeEvent.ComboPopup == true then
        BattleNodeEvent.ComboPopup = false
        -- local skill_cfg = SkillData.GetConfig(combo_skill_id)
        -- local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SKILL_HIT], packingName(grid_idx), skill_cfg.skill_name)
        -- pushDesc(BATTLE_DATA_EVENT.SKILL_HIT, desc)

        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.SKILL, combo_skill_id, BattleNodeEvent.detail_player)
        popup_beh:AddHitTrigger(hit_trigger)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
    if is_miao_sha then
        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.MIAO_SHA)
        popup_beh:AddHitTrigger(hit_trigger)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end

    if BattleHelper.COMBO_SKILL_SP[asset_id] then--焚烧冻结连珠箭
        if BattleNodeEvent.ComboNum == 1 then
            local per_skill_beh = BattleBehaviourPerformSkill.New(combo_skill_id, grid_idx, {eve_data.target_grid_idx}, {eve_data.target_grid_idx}, SKILL_TIMING_TYPE.IN)
            behaviour_queue:PushQueueBehaviour(per_skill_beh)
            per_skill_beh:AddHitTrigger(hit_trigger)
            BattleNodeEvent.pushParam(ParamKey.Hitter, per_skill_beh)
        else
            local hitter = BattleNodeEvent.getParam(ParamKey.Hitter)
            hitter:AddHitTrigger(hit_trigger)
        end
    elseif BattleHelper.COMBO_SKILL_ATTACK[asset_id] then
        local ch_data = BattleData.Instance:GetDataChInfo(grid_idx)
        local config = AudioMgr:GetAttackName(ch_data)
        local per_skill_beh = BattleBehaviourPerformAttack.New(grid_idx, {eve_data.target_grid_idx}, {eve_data.target_grid_idx}, config)
        behaviour_queue:PushQueueBehaviour(per_skill_beh)
        per_skill_beh:AddHitTrigger(hit_trigger)
        BattleNodeEvent.pushParam(ParamKey.Hitter, per_skill_beh)
    else
        local per_skill_beh = BattleBehaviourPerformSkill.New(combo_skill_id, grid_idx, {eve_data.target_grid_idx}, behiter_idxs, SKILL_TIMING_TYPE.IN, nil, version)
        if BattleNodeEvent.ComboNum == BattleNodeEvent.ComboTotalNum then
            if asset_id == BattleHelper.PuGongLuanSheShouJi then
                --最后一下忽略强制结束保证流程
                per_skill_beh.IsIgnoreForceComplete = true
            end
        end
        if asset_id == BattleHelper.SheMingYiJi2 then
            per_skill_beh.binding_behavior = BattlePlayBuffAssetStart.New(grid_idx, BattleHelper.SheShengQuYiBuff)
        end
        behaviour_queue:PushQueueBehaviour(per_skill_beh)
        per_skill_beh:AddHitTrigger(hit_trigger)
        -- behaviour_queue:PushCutOff()--多次同一目标导致卡死
        BattleNodeEvent.pushParam(ParamKey.Hitter, per_skill_beh)
    end

end

function BattleNodeEvent.setComboSkillOneHitEnd(grid_idx, eve_data)
    local combo_skill_id = BattleNodeEvent.getParam(ParamKey.ComboSkillId)
    local asset_id = getSkillAssetId(combo_skill_id)
    if BattleHelper.COMBO_SKILL_SP[asset_id] then
        if BattleNodeEvent.ComboNum == BattleNodeEvent.ComboTotalNum then
            BattleNodeEvent.popParam(ParamKey.Hitter)
        end
    else
        BattleNodeEvent.popParam(ParamKey.Hitter)
    end
    BattleNodeEvent.popParam(ParamKey.BehiterIdxs)
    BattleNodeEvent.popHitTrigger(grid_idx, eve_data.target_grid_idx)
    BattleNodeEvent.popParam(ParamKey.Atker)
end

--------------BATTLE_DATA_EVENT.SEAL_PET 27---------------------
function BattleNodeEvent.parseSealPet()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.is_succ = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    data.seal_card_item_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataSealPet(grid_idx, eve_data)

    local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SEAL_PET][eve_data.is_succ],
        packingName(grid_idx),
        Item.GetQuaName(eve_data.seal_card_item_id),
        packingName(eve_data.target_grid_idx))
    pushDesc(BATTLE_DATA_EVENT.SEAL_PET, desc)

    local seal_beh = BattleSeal.New(grid_idx, eve_data.target_grid_idx, eve_data.is_succ, eve_data.seal_card_item_id, BattleNodeEvent.detail_player)
    behaviour_queue:PushQueueBehaviour(seal_beh)
end

--------------BATTLE_DATA_EVENT.USE_FABAO 28---------------------
function BattleNodeEvent.parseUseFabao()
    local data = {}
    data.fabao_skill_type = MsgAdapter.ReadShort()
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.fabao_id = MsgAdapter.ReadShort()
    data.is_qiling = MsgAdapter.ReadChar()  -- 是否是器灵触发
    MsgAdapter.ReadChar()
    return data
end

function BattleNodeEvent.setDataUseFabao(grid_idx, eve_data, child_data)

    --是器灵触发的，加个飘字
    if eve_data.is_qiling == 1 and grid_idx == BattleData.Instance.base.player_idx then
        local cfg = FabaoData.Instance:FabaoAttrInfo(eve_data.fabao_id)
        if cfg then
            PublicPopupCtrl.Instance:Center(string.format(Language.Main.Battle.QiLingFaBao, cfg.name))
        end
    end

    --根据法宝增加被击事件
    if eve_data.fabao_skill_type == 112 or eve_data.fabao_skill_type == 115 then
        BattleNodeEvent.use_fabao_skill_type = 112
        --该事件被击一个，但是真实被击有可能是多个，为了被击时机对应上必须cache 子事件的触发对象
        local target_list = {}
        if child_data then
            for index, value in ipairs(child_data) do--这里战报如果有变化一定会出错
            table.insert(target_list, value.grid_idx)
            end
        end
        BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
        local beh = BattleUseFabao.New(grid_idx, eve_data, target_list)
        behaviour_queue:PushQueueBehaviour(beh)
        if child_data then
            for index, value in ipairs(child_data) do
                beh:AddHitTrigger(BattleNodeEvent.pushHitTrigger(grid_idx, value.grid_idx))
            end
        end
        --这里要pop掉
    else
        local beh = BattleUseFabao.New(grid_idx, eve_data)
        behaviour_queue:PushQueueBehaviour(beh)
    end 
end
function BattleNodeEvent.setUseFabaoEnd()
    BattleNodeEvent.popParam(ParamKey.Atker)
    BattleNodeEvent.use_fabao_skill_type = nil
end
--------------BATTLE_DATA_EVENT.USE_FABAO 28---------------------
function BattleNodeEvent.parseElementChange()
    local data = {}
    data.element = {}
    data.element[BattleAttriElement.Water] = MsgAdapter.ReadShort()
    data.element[BattleAttriElement.Fire] = MsgAdapter.ReadShort()
    data.element[BattleAttriElement.Wind] = MsgAdapter.ReadShort()
    data.element[BattleAttriElement.Earth] = MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataElementChange(grid_idx, eve_data)
    local beh = BattleBehaviourChangeAttribute.New(grid_idx, "element", eve_data.element)
    behaviour_queue:PushQueueBehaviour(beh)
end

--------------BATTLE_DATA_EVENT.TALKING 38----------------------------
function BattleNodeEvent.parseTalking()
    local data = {}
    data.talk_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataTalking(grid_idx, eve_data)
    --等对话完进行下一步
    local beh = BattleBehaviourTalking.New(eve_data.talk_id)
    behaviour_queue:PushQueueBehaviour(beh)
    if eve_data.talk_id == BattleHelper.XingTianTalk then
        beh = BattleBehaviourXTZaDi.New()
        behaviour_queue:PushQueueBehaviour(beh)
    end
end

--------------BATTLE_DATA_EVENT.SHOT_EFFECT 39------------------------
function BattleNodeEvent.parseShotEffect()
    local data = {}
    data.shot_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataShotEffect(grid_idx, eve_data)
    local beh = BattleBehaviourShotEffect.New(eve_data.shot_id)
    behaviour_queue:PushQueueBehaviour(beh)
end
--------------BATTLE_DATA_EVENT.CLIENT_GUIDE 40------------------------
function BattleNodeEvent.parseClientGuide()
    local data = {}
    data.guide_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataClientGuide(grid_idx, eve_data)
    local beh = BattleBehaviourClientGuide.New(eve_data.guide_id)
    behaviour_queue:PushQueueBehaviour(beh)
end

--------------BATTLE_DATA_EVENT.MONSTER_TRANSFORM 41------------------------
function BattleNodeEvent.parseMonsterTransform()
    local data = {}
    data.character_info = BattleNodeEvent.parseCharacterInfo()
    return data
end
function BattleNodeEvent.setDataMonsterTransform(grid_idx, eve_data)
    local beh = BattleBehaviourMonsterTransform.New(grid_idx,eve_data.character_info)
    behaviour_queue:PushQueueBehaviour(beh)
end

--------------BATTLE_DATA_EVENT.BUFF_MISS 42------------------------
function BattleNodeEvent.parseBuffMiss()
    local data = {}
    data.miss_reason = MsgAdapter.ReadShort()--0未生效1免疫2抵抗成功
    MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataBuffMiss(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local target_list = BattleNodeEvent.getParam(ParamKey.TargetList)
    local skill_id = BattleNodeEvent.getParam(ParamKey.SkillId)
    local trigger = nil
    if atker ~= nil then
        -- 提取当前攻击事件（即攻击者打我）的trigger
        trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    end
    local beh = BattleBehaviourBuffMiss.New(grid_idx,eve_data.miss_reason)
    if target_list and table.nums(target_list) > 1 then
        behaviour_queue:PushSyncBehaviour(beh)
        beh:SetDelayTime(1)
    else
        if skill_id == BattleHelper.ZhongDuGongJiId then
            behaviour_queue:PushSyncBehaviour(beh)
            beh:SetDelayTime(1)
        else
            behaviour_queue:PushQueueBehaviour(beh)
            beh:AddHitTrigger(trigger)
        end
    end
end
--------------BATTLE_DATA_EVENT.USE_MPITEM 43---------------------------
function BattleNodeEvent.parseUseMpItem()
    local data = {}
    data.used_times = MsgAdapter.ReadShort()
    data.max_times = MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataUseMpItem(grid_idx, eve_data)
    if grid_idx == BattleData.Instance:PlayerIdx() then
        BattleData.Instance:SetMpItemInfo(eve_data.used_times, eve_data.max_times)
    end
end
--------------BATTLE_DATA_EVENT.BUFF_SUSTAIN_NUM_CHANG 44---------------------------
function BattleNodeEvent.parseSustainNumChange()
    local data = {}
    data.sustain_num = MsgAdapter.ReadInt()
    data.new_buff_key = MsgAdapter.ReadUInt()
    return data
end
function BattleNodeEvent.setDataSustainNumChange(grid_idx, eve_data)
    local beh = BattleBuff.New(grid_idx, eve_data, BATTLE_DATA_EVENT.BUFF_SUSTAIN_NUM_CHANG)
    behaviour_queue:PushSyncBehaviour(beh)
end
--------------BATTLE_DATA_EVENT.SKILL_PERFORM_FAIL 45---------------------------
function BattleNodeEvent.parseSkillPerformFail()
    local data = {}
    data.skill_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataSkillPerformFail(grid_idx, eve_data)
    if grid_idx > 0 and (grid_idx == BattleData.Instance:PlayerIdx() or grid_idx == BattleData.Instance:PlayerPetIdx()) then
        local beh = BattleSkillFailure.New(grid_idx, eve_data)
        behaviour_queue:PushSyncBehaviour(beh)
    end
end

function BattleNodeEvent.parseModifyRagePoints()
    local data = {}
    data.modify_num = MsgAdapter.ReadInt()
    data.cur_num = MsgAdapter.ReadInt()
    data.modify_desc = MsgAdapter.ReadShort()
    data.reserve_sh = MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataModifyRagePoints(grid_idx, eve_data)
    local beh = BattleRagePointChange.New(grid_idx, eve_data)
    behaviour_queue:PushSyncBehaviour(beh)
end
-------------- 47 --------------------
function BattleNodeEvent.parseNoticeNum()
    local data = {}
    data.notice_num = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataNoticeNum(grid_idx, eve_data)
    if grid_idx > 0 and (grid_idx == BattleData.Instance:PlayerIdx() or grid_idx == BattleData.Instance:PlayerPetIdx()) then
        local beh = BattleNoticeNum.New(grid_idx, eve_data.notice_num)
        behaviour_queue:PushSyncBehaviour(beh)
    end
end

-------------- 48 --------------------
function BattleNodeEvent.parseClientCustom()
    local data = {}
    data.param1 = MsgAdapter.ReadInt()
    data.param2 = MsgAdapter.ReadInt()
    data.param3 = MsgAdapter.ReadInt()
    data.param4 = MsgAdapter.ReadInt()
    data.param5 = MsgAdapter.ReadInt()
    data.param6 = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataClientCustom(grid_idx, eve_data)
    if eve_data.param1 == 1 then
        local beh = BattleBehaviourTalking.New(eve_data.param2)
        behaviour_queue:PushQueueBehaviour(beh)
    end
end
--------------- 49 -----------------------
function BattleNodeEvent.parseSideShowHp()
    local data = {}
    data.side = MsgAdapter.ReadShort()  --0攻1守
    data.is_show = MsgAdapter.ReadShort() -- 0关1开
    return data
end
function BattleNodeEvent.setDataSideShowHp(grid_idx, eve_data)
    -- local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local role_side = BattleData.Instance:BaseData().role_side
    if role_side ~= eve_data.side then
        local beh = BattleSideHpShow.New(eve_data)
        behaviour_queue:PushQueueBehaviour(beh)
    end
end

------------- 50 -----------------------------
function BattleNodeEvent.parseFabaoInfoChange()
    local data = {}
    data.role_id = MsgAdapter.ReadInt()
    data.cd_num =  MsgAdapter.ReadShort()
    data.remain_times = MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataFabaoInfoChange(grid_idx, eve_data)
    if eve_data.role_id > 0 and eve_data.role_id == RoleData.Instance:GetBaseData().role_id then
        BattleData.Instance:FabaoInfoChange(eve_data.remain_times)
        BattleData.Instance:FabaoCD(eve_data.cd_num)
    end
end

function BattleNodeEvent.parseAddHalo()
    local data = {}
    data.halo_id = MsgAdapter.ReadInt()
    data.cover_grid_flag = {}
    for i = 1, 20 do
        data.cover_grid_flag[i] = MsgAdapter.ReadChar()--0否 1是
    end
    return data
end
function BattleNodeEvent.setDataAddHalo(grid_idx, eve_data)
    --直接给需要加上光环的对象加上
    local beh
    for i = 1, 20, 1 do
        if eve_data.cover_grid_flag[i] == 1 then
            beh = BattleAddHalo.New(grid_idx, i, eve_data.halo_id)
            behaviour_queue:PushSyncBehaviour(beh)
        end
    end
end

function BattleNodeEvent.parseRemoveHalo()
    local data = {}
    data.halo_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataRemoveHalo(grid_idx, eve_data)
    --直接循环所有对象判断是否一个阵营后将光环删除
    local beh
    for i = 1, 20, 1 do
        beh = BattleRemoveHalo.New(grid_idx, i, eve_data.halo_id)
        behaviour_queue:PushSyncBehaviour(beh)
    end
end
function BattleNodeEvent.parseHaloChangePosition()
    local data = {}
    data.old_grid_idx = MsgAdapter.ReadInt()
    data.halo_id = MsgAdapter.ReadInt()
    data.old_cover_grid_flag = {}
    for i = 1, 20 do
        data.old_cover_grid_flag[i] = MsgAdapter.ReadChar()--0否 1是
    end
    data.new_cover_grid_flag = {}
    for i = 1, 20 do
        data.new_cover_grid_flag[i] = MsgAdapter.ReadChar()--0否 1是
    end
    -- LogError("change halo", data)
    return data
end
function BattleNodeEvent.setDataHaloChangePosition(grid_idx, eve_data)
    --旧的删除 新的加上
end

function BattleNodeEvent.parseImprisonPerformFailed()
    local data = {}
    data.special_state_type = MsgAdapter.ReadInt()--0默认5沉默7昏睡8混乱9诅咒10石化22休整24嘲讽
    return data
end
function BattleNodeEvent.setDataImprisonPerformFailed(grid_idx, eve_data)
    if eve_data.special_state_type ~= 0 then
        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.ACTION_FAIL, eve_data.special_state_type)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    end
end
function BattleNodeEvent.parseSkillRemainCastTimes()
    local data = {}
    data.skill_id = MsgAdapter.ReadUShort()
    data.remain_times = MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataSkillRemainCastTimes(grid_idx, eve_data)
    if grid_idx == BattleData.Instance:PlayerIdx() then
        local data = BattleData.Instance:PlayerSkillInfo()
        for _, value in ipairs(data) do
            if value.skill_id == eve_data.skill_id then
                value.remain_times = eve_data.remain_times
            end
        end
    end
end
function BattleNodeEvent.parseHeartSkillInfoChange()
    local data = {}
    data.role_id = MsgAdapter.ReadInt()
    data.remain_times = MsgAdapter.ReadShort()--剩余心法使用次数
    data.is_vaild = MsgAdapter.ReadChar()--是否生效 0：失效 1：生效
    MsgAdapter.ReadChar()
    return data
end
function BattleNodeEvent.setDataHeartSkillInfoChang(grid_idx, eve_data)
    if eve_data.role_id == RoleData.Instance:GetRoleId() then
        local skill = BattleData.Instance:PlayerHeartSkill()
        for index, value in ipairs(skill) do
            value.remain_times = eve_data.remain_times
            value.is_vaild = eve_data.is_vaild
        end
    end
end
function BattleNodeEvent.parseLinkTogether()
    local data = {}
    data.link_target_idx = MsgAdapter.ReadShort() + 1 --//链接的目标格点
    data.link_type = MsgAdapter.ReadShort() -- // 链接类型（目前只有类型0：铁索连环)
    return data
end
function BattleNodeEvent.setDataLinkTogether(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.LinkTarget, grid_idx)
    -- LogDG("57", grid_idx, eve_data)
    if eve_data.link_type == 0 or eve_data.link_type == 1 then
        local atker = BattleNodeEvent.getParam(ParamKey.Atker)
        local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        local beh = BattleLinkTogether.New(grid_idx, eve_data)
        behaviour_queue:PushSyncBehaviour(beh)
        beh:SetTrigger(trigger)
    end
end
function BattleNodeEvent.setDataLinkTogetherEnd()
    BattleNodeEvent.popParam(ParamKey.LinkTarget)
end
function BattleNodeEvent.parseLinkBreak()
    local data = {}
    data.link_target_idx = MsgAdapter.ReadShort() + 1 --//链接的目标格点
    data.link_type = MsgAdapter.ReadShort() -- // 链接类型（目前只有类型0：铁索连环) 1心魔锁链 2共生
    return data
end
function BattleNodeEvent.setDataLinkBreak(grid_idx, eve_data)
    if eve_data.link_type == 0 or eve_data.link_type == 1 then
        local beh = BattleLinkBreak.New(grid_idx, eve_data)
        behaviour_queue:PushSyncBehaviour(beh)
    end
end
function BattleNodeEvent.parseSkillInstantKill()
    local data = {}
    data.is_instant_kill = MsgAdapter.ReadShort()--//是否触发了秒杀 0否 1是
    data.instant_kill_grid_idx = MsgAdapter.ReadShort() + 1 -- // 秒杀目标
    return data
end
function BattleNodeEvent.setDataSkillInstantKill(grid_idx, eve_data)
    -- LogDG("秒杀", grid_idx, eve_data)
end
function BattleNodeEvent.parseCharacterSpecialEffect()
    local data = {}
    data.special_effect_num = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    data.special_effect_list = {}
    for i = 1, data.special_effect_num do
        data.special_effect_list[i] = MsgAdapter.ReadInt()
    end
    return data
end
function BattleNodeEvent.setDataCharacterSpecialEffect(grid_idx, eve_data)
    BattleData.Instance:SetSpecialEffectListDelay(grid_idx, eve_data.special_effect_list)
end
function BattleNodeEvent.parseAddSingleSpecialEffect()
    local data = {}
    data.sp_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataAddSingleSpecialEffect(grid_idx, eve_data)
    BattleData.Instance:SetSpecialEffectList(grid_idx, eve_data.sp_id)
end
function BattleNodeEvent.parseRemoveSingleSpecialEffect()
    local data = {}
    data.sp_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataRemoveSingleSpecialEffect(grid_idx, eve_data)
    BattleData.Instance:RemoveSpecialEffectList(grid_idx, eve_data.sp_id)
end

--怒气值变化事件
--reason枚举在battle_config中
--BATTLE_SPIRIT_POINT_CHANGE_REASON = {
-- 	CHANGE_REASON_DIE_REDUCE = 0,		--死亡减少怒气值
-- 	CHANGE_REASON_ROUND_ADD = 1,		--回合结束加
-- CHANGE_REASON_SKILL_COST = 2, 		--技能消耗
-- }
function BattleNodeEvent.parseSpiritPointChange()
    local data = {}
    data.change_spirit_point = MsgAdapter.ReadInt()
    data.remain_spirit_point = MsgAdapter.ReadInt()
    data.reason = MsgAdapter.ReadInt()
    return data
end

function BattleNodeEvent.setDataSpiritPointChange(grid_idx, eve_data)
    -- local ch_info = BattleData.Instance:GetChInfo(grid_idx)
    -- if ch_info ~= nil then
    --     ch_info.spirit_point = eve_data.remain_spirit_point
    -- end
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)

    local sp_beh = BattleSpiritChange.New(grid_idx, eve_data.change_spirit_point, eve_data.remain_spirit_point, eve_data.reason, BattleNodeEvent.detail_player)
    if trigger then
        sp_beh:SetTrigger(trigger)
    end
    behaviour_queue:PushSyncBehaviour(sp_beh)
    --[[ if eve_data.modify_desc == 1 then
        behaviour_queue:PushCutOff()
    end ]]
end
function BattleNodeEvent.parseSeedExplode()
    local data = {}
    data.seed_type = MsgAdapter.ReadShort()
    data.buff_id = MsgAdapter.ReadShort()
    data.buff_key = MsgAdapter.ReadUInt()
    return data
end
function BattleNodeEvent.setDataSeedExplode(grid_idx, eve_data)
    local atker = BattleNodeEvent.getParam(ParamKey.Atker)
    local hit_trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
    local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.SEED_EXPLODE)
    popup_beh:AddHitTrigger(hit_trigger)
    behaviour_queue:PushSyncBehaviour(popup_beh)
end
function BattleNodeEvent.parseSpecialEffectTarget()
    local data = {}
    data.deliver_grid_idx = MsgAdapter.ReadInt() + 1
    return data
end
function BattleNodeEvent.setDataSpecialEffectTarget(grid_idx, eve_data)
    --存储特效作用对象
    --特效触发事件时判断是否是穷奇大凶 释放技能
    --穷奇播放技能后死亡
    --【A攻击穷】-【技能效果】-【穷血=0】【穷触发大凶特效（前提知道目标）】- 【穷死亡】【A死亡/诅咒/无敌XXXX】
    if BattleNodeEvent.not_at_pos[eve_data.deliver_grid_idx] == true then
        local move_beh = BattleBehaviourMove.New(eve_data.deliver_grid_idx, eve_data.deliver_grid_idx)
        behaviour_queue:PushQueueBehaviour(move_beh)
        BattleNodeEvent.not_at_pos[grid_idx] = false
    end
    local targets = {eve_data.deliver_grid_idx}
    local hit_trigger = BattleNodeEvent.pushHitTrigger(grid_idx, eve_data.deliver_grid_idx)
    local skill_beh = BattleBehaviourPerformSkillByAssetId.New("DaXiong", grid_idx, targets, targets)
    BattleNodeEvent.daxiong_skill_hit_trigger = hit_trigger
    skill_beh:AddHitTrigger(hit_trigger)
    behaviour_queue:PushQueueBehaviour(skill_beh)
end
function BattleNodeEvent.setDataSpecialEffectTargetEnd(grid_idx, eve_data)
    BattleNodeEvent.popHitTrigger(grid_idx, eve_data.deliver_grid_idx)
end
function BattleNodeEvent.parseSpreadSkill()
    local data = {}
    data.source_grid_idx = MsgAdapter.ReadInt() + 1--扩散技能释放者
    data.spread_times = MsgAdapter.ReadInt() --扩散次数 起始0
    return data
end
function BattleNodeEvent.setDataSpreadSkill(grid_idx, eve_data)
    -- 根据次数进行不同时间的特效播放
    if eve_data.spread_times > 0 then
        local atk_idx = eve_data.source_grid_idx
        local spread_beh = BattleNodeEvent.spread_skill[eve_data.spread_times]
        local targets = {grid_idx}
        if spread_beh then
            --这里没有攻击者的动画了，所以自己打自己就可以了，否则仍然会堵塞，因为技能是一个一个播放的，攻击者的技能播放器已经占用了
            local skill_beh = BattleBehaviourPerformSkillByAssetId.New("HuoLuanBeiJi", grid_idx, targets, targets)
            local hit_trigger = BattleNodeEvent.pushHitTrigger(grid_idx, grid_idx)
            BattleNodeEvent.spread_skill_hit_trigger = hit_trigger
            skill_beh:AddHitTrigger(hit_trigger)
            spread_beh:AddBehavior(skill_beh)
        else
            local skill_beh = BattleBehaviourPerformSkillByAssetId.New("HuoLuanBeiJi", atk_idx, targets, targets)
            local hit_trigger = BattleNodeEvent.pushHitTrigger(atk_idx, grid_idx)
            skill_beh:AddHitTrigger(hit_trigger)
            behaviour_queue:PushQueueBehaviour(skill_beh)
            BattleNodeEvent.spread_skill[eve_data.spread_times] = skill_beh
        end
    else
        BattleNodeEvent.spread_skill = {}
        BattleNodeEvent.spread_skill_hit_trigger = nil
    end
end
function BattleNodeEvent.setDataSpreadSkillEnd(grid_idx, eve_data)
    if eve_data.spread_times > 0 then
        local atk_idx = eve_data.source_grid_idx
        local spread_beh = BattleNodeEvent.spread_skill[eve_data.spread_times]
        if spread_beh then
            BattleNodeEvent.popHitTrigger(grid_idx, grid_idx)
        else
            BattleNodeEvent.popHitTrigger(atk_idx, grid_idx)
        end
    end
end

function BattleNodeEvent.parseHallowSkill()
    local data = {}
    data.hallow_id = MsgAdapter.ReadShort()
    data.hallow_skill_id = MsgAdapter.ReadShort()
    data.hallow_skill_lv = MsgAdapter.ReadShort()
    data.assist_num = MsgAdapter.ReadShort() + 1
    data.reserve_sh = MsgAdapter.ReadShort()
    data.target_num = MsgAdapter.ReadShort()
    data.target_list = {}
    for i = 1, data.target_num do
        data.target_list[i] = MsgAdapter.ReadShort() + 1
    end
    return data
end
function BattleNodeEvent.setDataHallowSkill(grid_idx, eve_data)
    BattleNodeEvent.pushParam(ParamKey.EventType, BATTLE_DATA_EVENT.HALLOW_PERFORM_SKILL)
    -- BattleNodeEvent.pushParam(ParamKey.Atker, grid_idx)
    eve_data.grid_idx = grid_idx
    BattleNodeEvent.pushParam(ParamKey.HallowSkill, eve_data)
    -- BattleData.Instance:SetSQSkill(eve_data)
    local beh = BattleBehaviourHallowSkillMove.New(grid_idx, eve_data)
    behaviour_queue:PushQueueBehaviour(beh)
    local beh = BattleShengQiSkillShow.New(eve_data)
    behaviour_queue:PushSyncBehaviour(beh)
    beh = BattleBehaviourPerformHallowSkill.New(grid_idx, eve_data)
    behaviour_queue:PushQueueBehaviour(beh)
end
function BattleNodeEvent.setDataHallowSkillEnd(grid_idx, eve_data)
    local beh = BattleBehaviourMove.New(grid_idx, grid_idx)
    behaviour_queue:PushQueueBehaviour(beh)
    BattleNodeEvent.popParam(ParamKey.EventType)
    BattleNodeEvent.popParam(ParamKey.HallowSkill)
end
function BattleNodeEvent.parseHallowSkillHit()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.skill_id = MsgAdapter.ReadShort()
    return data
end
function BattleNodeEvent.setDataHallowSkillHit(grid_idx, eve_data)
    
end
function BattleNodeEvent.setDataHallowSkillHitEnd()
    
end

function BattleNodeEvent.parseHallowSkillEffect()
    
end
function BattleNodeEvent.setDataHallowSkillEffect()
    
end
function BattleNodeEvent.setDataHallowSkillEffectEnd()
    
end
function BattleNodeEvent.parseHallowFlyAway()
    local data = {}
    data.hallow_id = MsgAdapter.ReadShort()
    data.grid_idx = MsgAdapter.ReadShort() + 1
    return data
end
function BattleNodeEvent.setDataHallowFlyAway(grid_idx, eve_data)
    local beh = BattleHallowDiaup.New(eve_data.grid_idx, eve_data)
    behaviour_queue:PushQueueBehaviour(beh)
end
function BattleNodeEvent.setDataHallowFlyAwayEnd()
    
end
-- 圣器飘字
function BattleNodeEvent.parseHallowFlyWords()
    local data = {}
    data.fly_words_type = MsgAdapter.ReadInt()--闪避飘字
    data.param1 = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataHallowFlyWords(grid_idx, eve_data)
    local info = BattleData.Instance:GetSQInfo(grid_idx)
    local hallow_name = ""
    if Cfg and info then
        local config = Cfg.ShengQiCfgById(info.hallow_id)
        hallow_name = config.shengqi_name
    end
    if eve_data.fly_words_type == FLY_WORDS_TYPE.FLY_WORDS_TYPE_DODGE then
        -- local trigger = BattleNodeEvent.getHitTrigger(atker, grid_idx)
        local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.DODGE)
        -- popup_beh:SetTrigger(trigger)
        behaviour_queue:PushSyncBehaviour(popup_beh)
    elseif eve_data.fly_words_type == FLY_WORDS_TYPE.FLY_WORDS_TYPE_USE_FAIL then
        if Cfg then
            local config = Cfg.ShengQiZhuDongSkill(eve_data.param1, false)
            if config then
                local desc = string.format(Language.Main.Battle.ShengQiFail[eve_data.fly_words_type], config.skill_name)
                local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.ACTION_FAIL)
                popup_beh.num2 = desc
                behaviour_queue:PushSyncBehaviour(popup_beh)
            end
        end
    elseif eve_data.fly_words_type == FLY_WORDS_TYPE.FLY_WORDS_TYPE_ALL_IN_CD then
        if Cfg then
            local desc = string.format(Language.Main.Battle.ShengQiFail[eve_data.fly_words_type], hallow_name)
            local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.ACTION_FAIL)
            popup_beh.num2 = desc
            behaviour_queue:PushSyncBehaviour(popup_beh)
        end
    elseif eve_data.fly_words_type == FLY_WORDS_TYPE.FLY_WORDS_TYPE_DEAD_ABOR then
        if Cfg then
            local desc = string.format(Language.Main.Battle.ShengQiFail[eve_data.fly_words_type], hallow_name)
            local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.ACTION_FAIL)
            popup_beh.num2 = desc
            behaviour_queue:PushSyncBehaviour(popup_beh)
        end

    end
end
function BattleNodeEvent.setDataHallowFlyWordsEnd()
    
end
function BattleNodeEvent.parseHallowActivateList()
--     enum STATUS
--   {
--     STATUS_NOT_ACTIVATE = 0,   //不激活
--     STATUS_ACTIVATE_FIGHT = 1, //主战 (本回合要放技能）
--     STATUS_ACTIVATE_ASSIST = 2,//助战（本回合不放技能）
--   };
    local data = {}
    data.hallow_num = MsgAdapter.ReadInt()
    data.hallow_list = {}
    for i = 1, data.hallow_num, 1 do
        data.hallow_list[i] = {}
        data.hallow_list[i].grid_idx = MsgAdapter.ReadShort() + 1
        data.hallow_list[i].status = MsgAdapter.ReadShort()
    end
    return data
end
function BattleNodeEvent.setDataHallowActivateList(grid_idx, eve_data)
    local beh = BattleBehaviourHallowActivate.New(eve_data)
    behaviour_queue:PushQueueBehaviour(beh)
end
function BattleNodeEvent.setDataHallowActivateListEnd(grid_idx, eve_data)
    
end
function BattleNodeEvent.parseHallowIntervene()
    local data = {}
    data.hallow_id = MsgAdapter.ReadInt()
    data.owner_uid = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataHallowIntervene(grid_idx, eve_data)
    local info = {}
    info.grid_idx = grid_idx -- 21-30
    info.hallow_id = eve_data.hallow_id--圣器id
    info.owner_uid = eve_data.owner_uid
    info.owner_idx = info.grid_idx < 26 and info.grid_idx - 20 or info.grid_idx - 15
    local beh = BattleBehaviourHallowIntervene.New(grid_idx, info)
    behaviour_queue:PushQueueBehaviour(beh)
    local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.INTERVENE)
    behaviour_queue:PushSyncBehaviour(popup_beh)
end
function BattleNodeEvent.setDataHallowInterveneEnd(grid_idx, eve_data)
    
end
function BattleNodeEvent.parseHallowGiftTrigger()
    local data = {}
    data.hallow_id = MsgAdapter.ReadInt()
    data.hallow_gift_id = MsgAdapter.ReadInt()
    return data
end
function BattleNodeEvent.setDataHallowGiftTrigger(grid_idx, eve_data)    
    local popup_beh = BattlePopup.New(grid_idx, BATTLE_HP_DESC.HALLOW_GIFT, eve_data.hallow_gift_id)
    behaviour_queue:PushSyncBehaviour(popup_beh)
end
function BattleNodeEvent.setDataHallowGiftTriggerEnd(grid_idx, eve_data)
    
end
-------------- 18 --------------------
function BattleNodeEvent.setRoundEnd(grid_idx, eve_data)
    --处理一些特殊事件

    --107怪物组的火龙行者没回合变大
    -- if BattleData.Instance:BaseData().monster_group_id == 107 and BattleData.Instance:GetDataChInfo(13) then
    --     local beh = BattleBehaviourChangeScale.New(13, 0.2, 1.7)
    --     behaviour_queue:PushSyncBehaviour(beh)
    -- end
end

-------------- 30 --------------------
function BattleNodeEvent.setVanish(grid_idx, eve_data)
    local diedis_beh = BattleChVanish.New(grid_idx)
    behaviour_queue:PushSyncBehaviour(diedis_beh)
    BattleData.Instance:SetDataChInfo(grid_idx, nil)
end

---common empty func
function BattleNodeEvent.parseDataEmpty(eve_type, data_len)
    if data_len ~= nil and data_len > 0 then
        MsgAdapter.ReadStrN(data_len)
    end
    return nil
end

function BattleNodeEvent.setDataEmpty(grid_idx, eve_data)
end



-- =============== 幻兽技能冷却事件 75 ============
function BattleNodeEvent.parseHuanShouSkillCD()
    local data = {}
    data.role_id = MsgAdapter.ReadInt()
    data.skill_cd = MsgAdapter.ReadShort()
    if data.role_id == RoleData.Instance:GetRoleId() and BattleData.Instance.huanshou_skill_info then
        BattleData.Instance.huanshou_skill_info.skill_cd = data.skill_cd
    end
    MsgAdapter.ReadShort()
    --LogError("幻兽技能冷却事件", data)
    return data
end


-- ====================== 神龙 ===================

-- 事件 76 --神龙释放技能
function BattleNodeEvent.parseDragonSkillPerform()
    local info = {}
    info.skill_id = MsgAdapter.ReadUShort()
    info.target_num = MsgAdapter.ReadShort()
    info.target_list = {}
    for i = 1, info.target_num do
        table.insert(info.target_list, MsgAdapter.ReadShort() + 1)  --格子客户端需要+1处理
    end
    --LOG("神龙释放技能事件 parse：", info.skill_id,info)
    return info
end

function BattleNodeEvent.setDataDragonSkillPerform(grid_idx, eve_data)

    --BattleNodeEvent.pushParam(ParamKey.EventType, BATTLE_DATA_EVENT.HALLOW_PERFORM_SKILL)
    beh = BattleBehaviourPerformDragonSkill.New(grid_idx, eve_data, BattleNodeEvent.detail_player)
    behaviour_queue:PushQueueBehaviour(beh)

    local skill_id = eve_data.skill_id
    local skill_cfg = Cfg.FairyDragonSkillCfg(skill_id)
    if skill_cfg then
        local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.DRAGON_SKILL_HIT][3], skill_cfg.skill_name)
        pushDesc(BATTLE_DATA_EVENT.DRAGON_SKILL_PERFORM, desc)
    end
    --LOG("神龙释放技能事件 SetData", grid_idx, eve_data)
end

-- 77 --神龙技能命中
function BattleNodeEvent.parseDragonSkillHit()
    local info = {}
    info.target_grid_idx = MsgAdapter.ReadShort()
    info.skill_id = MsgAdapter.ReadUShort()

    --LOG("神龙技能命中 parse：", info)
    BattleNodeEvent.pushParam(ParamKey.EventType,BATTLE_DATA_EVENT.DRAGON_SKILL_HIT)
    BattleNodeEvent.pushParam(ParamKey.FairyDragonSkill, info)
end

-- 79   神龙被动效果触发
function BattleNodeEvent.parseDragonPassiveEffect()
    local info = {}
    info.skill_id = MsgAdapter.ReadInt()

    --LOG("神龙被动效果触发 parse：", info)
    return info
end

function BattleNodeEvent.setDataDragonPassiveEffect(grid_idx, eve_data)
    --LOG("神龙被动效果触发 SetData", grid_idx, eve_data)
end


-- 80   神龙星体变化
function BattleNodeEvent.parseDragonStarChange()
    local info = {}
    info.rage_value = MsgAdapter.ReadInt()

    --LOG("神龙星体变化 parse：", info)
    return info
end

function BattleNodeEvent.setDataDragonStarChange(grid_idx, eve_data)
    local beh = BattleDragonPointChange.New(grid_idx, eve_data)
    behaviour_queue:PushSyncBehaviour(beh)
    --LOG("神龙星体变化 SetData", grid_idx, eve_data)
end


--81 护盾格子信息变更
function BattleNodeEvent.parseBattleShieldGridInfoChange()
    local info = {}
    info.is_has_shield = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    --LogError("护盾信息变更Parse", info)
    return info
end
function BattleNodeEvent.setDataBattleShieldGridInfoChange(grid_idx, eve_data)
    --LogError("护盾信息变更", grid_idx, eve_data)
    local data = {}
    data.state = eve_data.is_has_shield == 1 and 1 or 2
    local beh = BattleRabbitShiledChange.New(grid_idx, data)
    behaviour_queue:PushSyncBehaviour(beh)
end

-- 偃月兔护盾受击，闪烁效果
function BattleNodeEvent.setDataBattleShieldGridFlicker(grid_idx)
    --LogError("偃月兔护盾受击，闪烁效果", grid_idx)
    local data = {}
    data.state = 3
    local beh = BattleRabbitShiledChange.New(grid_idx, data)
    behaviour_queue:PushSyncBehaviour(beh)
end

--------------BATTLE_DATA_EVENT.SEAL_NETHER_WORLD 83---------------------
function BattleNodeEvent.parseSealYouMing()
    local data = {}
    data.target_grid_idx = MsgAdapter.ReadShort() + 1
    data.is_succ = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    data.seal_card_item_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    return data
end

function BattleNodeEvent.setDataSealYouMing(grid_idx, eve_data)
    local desc = string.format(Language.BattleDetail[BATTLE_DATA_EVENT.SEAL_NETHER_WORLD][eve_data.is_succ],
        packingName(grid_idx),
        Item.GetQuaName(eve_data.seal_card_item_id),
        packingName(eve_data.target_grid_idx))
    pushDesc(BATTLE_DATA_EVENT.SEAL_NETHER_WORLD, desc)

    local seal_beh = BattleSeal.New(grid_idx, eve_data.target_grid_idx, eve_data.is_succ, eve_data.seal_card_item_id, BattleNodeEvent.detail_player)
    behaviour_queue:PushQueueBehaviour(seal_beh)
end