FILE_REQ("modules/battle/battle_scene_data")

BattleData = BattleData or BaseClass()
BattleData.BattleViewData = {}
function BattleData:__init()
    if BattleData.Instance ~= nil then
        Debuger.LogError("[BattleData] attempt to create singleton twice!")
        return
    end
    BattleData.Instance = self

    if Config == nil then
        return
    end
    self.battle_record_list = {}
    self.auto_info_list = {}
    
    self.scenefight_config_auto = Config.scenefight_config_auto
    self.sundries_auto = Config.sundries_auto
    self.element_elf_auto = Config.element_elf_auto

    --server data
    self.ch_info = SmartData.New() --key:grid idx,val info_tab
    self.sheng_qi_list = {}--key:grid idx,val info_tab
    self.dragon_list = SmartData.New()--key:grid idx,val info_tab    神龙
    self.rabbit_shield_list = SmartData.New()
    self.data_ch_info = nil --解析数据包时使用的角色信息
    self.round_data = {} --key:round count,value round tree root
    self.base =
        SmartData.New(
        {
            cur_round = 1,
            round = 0,
            round_phase = BATTLE_ROUND_PHASE.None,
            auto = false,
            command_i = 0, --1,玩家一动指令;2,宠物指令或玩家2动指令
            last_cmd_type = BATTLE_MOVE_TYPE.NONE,
            last_cmd_p1 = nil,
            last_cmd_p2 = nil,
            last_cmd_p3 = nil,
            sel_tar_type = BATTLE_MOVE_TYPE.NONE,
            sel_tar_param1 = nil, --PERFORM_SKILL:技能ID,USE_ITEM:ItemData
            sel_tar_param2 = nil, --PERFORM_SKILL:技能等级
            sel_tar_param3 = nil, --PERFORM_SKILL:自定义存储
            cmd_left_time = -1, --操作剩余时间(s)
            second_move = {}, ---key:grid idx,val:true or false
            play_speed = 1,
            play_speed_type = ANIMATION_SPEED.NORMAL,
            battle_mode = BATTLE_MODE.BATTLE_MODE_NULL,
            server_fighting = false,
            is_boss = false,
            fabao_remain_times = 0,
            fabao_cd = -1,
            is_ob = 0, --是否在观战状态
            ob_side = 0,
            is_pvp = false,
            role_side = 0,
            limit_timestamp = 0,
            player_idx = 0,
            monster_group_id = -1,
            total_damage = 0,
            is_team_up = false,
            bullet_on = true,
            clear_bullets = false,
            pause = false,
            video_playing = false,
            used_mp_item_times = 0,
            max_mp_item_times = 0,
            mp_item_can_use_tiems = 0,
            mp_item_mark_cancel = 0,
            cam_type = 0,
            fake_potion_num = 0,
            fake_potion_list = {},
            embattle_state = 0,--荣誉战状态
            can_modify_speed = 0,--是否可修改速度
            sheng_qi_show = false,
        }
    )
    self.story_data = SmartData.New({
        auto_anim = false,
        hide_ui = false,
    })
    -- self.base:Trace("is_ob")
    -- self.base:Trace("command_i")
    -- self.base:Trace("auto")

    --type===[BATTLE_MOVE_TYPE]
    self.auto_behaviour = {
        SmartData.New({type = 0, skill_id = 0, skill_lv = 0}),
        SmartData.New({type = 0, skill_id = 0, skill_lv = 0})
    }

    --local data
    self.scene_data = nil --DataHolder   Pos:Transform[20]

    --{pos,num,type}
    self.hp_popup = SmartData.New() --掉血弹字
    --
    self.move_popup = SmartData.New() --玩家行为展示

    self.player_idx = 0 --------玩家的idx，根据ch_info计算得出

    self.view_data =
        SmartData.New(
        {
            skill_sel_tab = 1, --当前选择的tab 无效
            skill_sel_cmdi = 1, --1,玩家一动指令;2,宠物指令或玩家2动指令，去base 中的command_i区分开，因为自动设置可以直接设置2
            opened_sub = BattleSubView.None,
            move_camera = false,
            flush_info = false,
            move1_nomp = false,
            move2_nomp = false,
            embattle_watching = false,--观战荣誉战
            skill_ui_effect_id = 0,
            select_strategy = false,--选择战术
            clear_all = false,--清除所有战术
            quick_skip = false,
            skill_sel_type = 1,--1技能 2心法 3绝招仅做界面展示用
        }
    )
    self.real_server_fight = false
    self.record_quick_click_time = 0

    self.player_skill_info = {}
    self.player_heart_skill_count = 0
    self.player_heart_skill_info = {}

    self.pet_list = SmartData.New()

    self.fabao_list = SmartData.New()

    self.other_data =
        SmartData.New(
        {
            play_battle_effect = false,
            battle_effect_play_list = {}
        }
    )
    self.battle_detail = SmartData.New() --战斗过程记录
    self.bullet_comments = SmartData.New() --弹幕装填器
    self.debug_logs = SmartData.New() --调试日志
    self.battle_statistics = SmartData.New() --玩家战斗统计数据
    self.battle_custom_strategy = SmartData.New()--自定义指令
    self.strategy_data = SmartData.New({del = false, del_list = {}, add_show = true})
    self.sheng_qi_list = {}
    self:Clear()
    self:initConfig()
end
function BattleData:initConfig()
    self.purify_effect_list = {}
    local temp = string.split(self.sundries_auto.other[1].purify_effect_type, "|")
    for index, value in ipairs(temp) do
        self.purify_effect_list[tonumber(value)] = true
    end
    if Cfg then--战斗模拟器报错
        Cfg.Register("BoxRewardBattleMode", Cfg.CacheGet(self.sundries_auto.box_reward,"play_id"))
        Cfg.Register("BoxRewardGroupId", Cfg.CacheGet(self.sundries_auto.box_reward,"monster_group_id"))
        Cfg.Register("ElementElfById", Cfg.CacheGet(self.element_elf_auto.elf, "monster_id"))
        Cfg.Register("ElementElfRewardSeq", Cfg.CacheGet(self.element_elf_auto.reward, "reward_seq"))
    end
end

function BattleData:SetDebugLog(k, v)
    self.debug_logs[k] = v
end

function BattleData:PackegeDebugLog()
    local log = "战斗调试信息（出现问题时请把这个也截下来）\n"
    for k, v in pairs(self.debug_logs) do
        log = log .. v .. "\n"
    end
    return log
end

function BattleData:Clear()
    --LogError("!!!!!!!!!!!!!!!!!!!!!!!!!!")
    self.is_show_boss_box = false
    self.change_pet_gudie = nil
    self.battle_halo_list = nil
    self.debug_logs:Clear()
    self.fabao_list:Set({})
    self.ch_info:Set({})
    self.sheng_qi_list = {}
    self.dragon_list:Set({})
    self.rabbit_shield_list:Set({})
    self.data_ch_info = nil
    self.view_data.flush_info = not self.view_data.flush_info
    self.view_data.select_strategy = false
    if self.scene_data ~= nil then
        self.scene_data:DeleteMe()
        self.scene_data = nil
    end
    self.round_data = {}
    self.base.cur_round = 1
    self.base.round = 0
    self.base.round_phase = BATTLE_ROUND_PHASE.None
    self.base.auto = false
    self.base.command_i = 0 --1,玩家一动指令;2,宠物指令或玩家2动指令
    self.base.last_cmd_type = BATTLE_MOVE_TYPE.NONE
    self.base.last_cmd_p1 = nil
    self.base.last_cmd_p2 = nil
    self.base.last_cmd_p3 = nil

    self.base.sel_tar_type = BATTLE_MOVE_TYPE.NONE
    self.base.sel_tar_param1 = nil --PERFORM_SKILL:技能ID,USE_ITEM:ItemData
    self.base.sel_tar_param2 = nil --PERFORM_SKILL:技能等级
    self.base.sel_tar_param3 = nil --PERFORM_SKILL:技能等级

    self.base.cmd_left_time = -1 --操作剩余时间(s)
    self.base.second_move = {} ---key:grid idx,val:true or false
    self.base.battle_mode = BATTLE_MODE.BATTLE_MODE_NULL

    self.base.is_ob = -1
    self.base.is_pvp = false
    self.base.role_side = 0
    self.base.monster_group_id = -1
    self.base.total_damage = 0
    self.base.is_team_up = false
    self.base.server_fighting = false
    self.base.pause = false
    self.base.video_playing = false
    self.base.used_mp_item_times = 0
    self.base.max_mp_item_times = 0
    self.base.mp_item_can_use_tiems = 0
    self.base.fake_potion_num = 0
    self.base.embattle_state = 0
    self.base.can_modify_speed = 0
end

function BattleData:__delete()
    self:Clear()
    if BattleData.Instance == self then
        BattleData.Instance = nil
    end
end

--服务端是否已经进入战斗
function BattleData:IsServerFighting()
    return self.base.server_fighting
end

function BattleData:SetChInfo(grid_i, info, change_pos)
    --[[ if info then
        LogError("SetChInfo", grid_i, info.name)
    end ]]
    change_pos = change_pos or false
    if info == nil then
        self:ClearSpecialEffectList(grid_i)
        self.ch_info[grid_i] = nil
        if self.player_idx == grid_i then
            self.player_idx = 0
            self.base.player_idx = 0
        end
    else
        local sd
        if SmartData.Check(info) == false then
            sd = SmartData.New(info, true)
        else
            sd = info
        end
        self.ch_info[grid_i] = sd
        if change_pos == false then
            sd.buff_list = SmartData.New({})
            sd.ui_buff_list = SmartData.New({}, true)
            sd.attr_buff_list = SmartData.New({})
            self:ClearSpecialEffectList(grid_i)
            self:CheckSpecialEffectListDelay(grid_i)
        end
        if info.character_type == BATTLE_CH_TYPE.ROLE or info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
            if RoleData.Instance:GetBaseData().role_id == info.character_id then
                self.player_idx = grid_i
                self.base.player_idx = grid_i
                self.ch_info[grid_i].is_player = true
            end
        end
    end
    self.view_data.flush_info = not self.view_data.flush_info
end
function BattleData:SetBuffInfo(buff_info)
    for key, value in pairs(self.ch_info) do
        if buff_info[key] then
            value.buff_list:Set(buff_info[key])
        end
    end
end
function BattleData:GetChInfo(grid_i)
    return self.ch_info[grid_i]
end

function BattleData:ChInfo()
    return self.ch_info
end
function BattleData:GetGridIdxByUid(uid)
    for k,v in pairs(self.ch_info) do
        if v.character_id == uid then
            return k
        end
    end
end
function BattleData:GetUidByGridIdx(grid_idx)
    for _,v in pairs(self.ch_info) do
        if v.grid_idx == grid_idx then
            return v.character_id
        end
    end
end
function BattleData:GenerateDataChInfo()
    self.data_ch_info = self.data_ch_info or {}
    -- local temp
    for i, v in pairs(self.ch_info) do
        if self.data_ch_info[i] then
            -- temp = self.data_ch_info[i]
            self.data_ch_info[i] = TableCopy(v:Val())
            self.data_ch_info[i].buff_list = {}
            for key, value in pairs(v.buff_list) do
                if type(value) == "number" then
                    self.data_ch_info[i].buff_list[key] = value
                else
                    self.data_ch_info[i].buff_list[key] = value.new_buff_id
                end
            end
            -- self.data_ch_info[i].buff_list = temp.buff_list
        else
            self.data_ch_info[i] = TableCopy(v:Val())
        end
    end
end

function BattleData:GetDataChInfo(grid_i)
    if self.data_ch_info == nil then
        return nil
    end
    return self.data_ch_info[grid_i]
end

function BattleData:SetDataChInfo(grid_i, info)
    self.data_ch_info[grid_i] = info
    -- self.view_data.flush_info = not self.view_data.flush_info
end


function BattleData:checkPetSeal()
    local catch_type = 4
    for _, value in pairs(self.ch_info) do
        if value.character_type == BATTLE_CH_TYPE.MONSTER then
            local cfg = Cfg.MonsterById(value.character_id)
            if value.level == 1 and cfg.catch_pet_id ~= 0 then
                local pc = Cfg.PetById(cfg.catch_pet_id)
                if pc then
                    catch_type = math.min(catch_type, pc.catch_type)
                end
            end
        end
    end
    return catch_type
end
function BattleData:ViewInfo()
    return self.view_data
end

function BattleData:ChInfoCount()
    if self.ch_info == nil then
        return 0
    end
    local count = 0
    for k, v in pairs(self.ch_info) do
        count = count + 1
    end
    return count
end

function BattleData:PlayerIdx()
    return self.player_idx
end

function BattleData:PlayerPetIdx()
    return self:GetPetIdxByPlayerIdx(self.player_idx)
end

function BattleData:GetPetIdxByPlayerIdx(player_idx)
    if player_idx == 0 then
        return 0
    end
    local is_front = BattleData.IsFrontIdx(player_idx)
    return is_front and (player_idx - 5) or (player_idx + 5)
end

function BattleData:SetSceneData(data_holder)
    self.scene_data = BattleSceneData.New(data_holder)
    return self.scene_data
end

function BattleData:SceneData()
    return self.scene_data
end
function BattleData:ClearSceneData()
    if self.scene_data ~= nil then
        self.scene_data:DeleteMe()
        self.scene_data = nil
    end
end

function BattleData:SetRoundData(round, root)
    self.round_data[round] = root
end

function BattleData:GetRoundData(round)
    if round == nil then
        round = self.base.cur_round
    end
    --LOG("获取回合数据", round)
    return self.round_data[round] or {}
end
function BattleData:InsertDetail(content)
    self.battle_detail[table.nums(self.battle_detail)] = content
end
function BattleData:BattleDetail()
    return self.battle_detail
end
function BattleData:ClearDetail()
    self.battle_detail:Clear()
end
---------BATTLE_ROUND_PHASE
function BattleData:SetRoundPhase(phase)
    if self.base.round_phase ~= phase then
        if self.base.round_phase == BATTLE_ROUND_PHASE.Command then --exit command phase
            -- self.view_data.opened_sub = BattleSubView.None
            self.base.sel_tar_type = BATTLE_MOVE_TYPE.NONE
            self.base.sel_tar_param1 = nil
            self.base.sel_tar_param2 = nil
            -- self.view_data.select_strategy = false
        end
        self.base.round_phase = phase
        if self.base.round_phase == BATTLE_ROUND_PHASE.Command then --enter command phase
            -- LogErrorHL("command_i turn into 1")
            self.base.command_i = 1
            self.base.last_cmd_type = BATTLE_MOVE_TYPE.NONE
            self.base.last_cmd_p1 = nil
            self.base.last_cmd_p2 = nil
            self.base.last_cmd_p3 = nil
        end
    end
end

---------auto battle--------
function BattleData:SetAuto(auto)
    self.base.auto = auto
    self.story_data.auto_anim = not self.story_data.auto_anim
end

-- function BattleData:SetSecondMove(data)
-- 	self.base.second_move = data
-- end
---1,2
function BattleData:AutoBehaviour(i)
    return self.auto_behaviour[i]
end

function BattleData:BaseData()
    return self.base
end

--战斗模型
function BattleData:BattleMode()
    return self.base.battle_mode
end

function BattleData:Round()
    return self.base.round
end


-- function BattleData:SwitchAllCh()
--     for i = 1, 10 do
--         local switched_idx = BattleData.GetSwitchedIdx(i)
--         local temp = self.ch_info
--     end
-- end

function BattleData:SetRoleInfo(role_info)
    self.player_skill_info = role_info.role_skill_list
    self.player_heart_skill_count = role_info.heart_skill_count
    self.player_heart_skill_info = role_info.heart_skill
    self.live_card_seq = role_info.live_card_seq
    self.huanshou_skill_info =  role_info.huanshou_skill_info
    self.pet_list:Clear()
    for i = 1, #role_info.pet_list do
        self.pet_list[i] = SmartData.New(role_info.pet_list[i])
        local bag_pet_info = PetData.Instance:GetPetInfo(role_info.pet_list[i].bag_idx)
        if bag_pet_info then
            self.pet_list[i].score = bag_pet_info:GetScore()
        else
            self.pet_list[i].score = 0
        end
        if self.pet_list[i].status == BATTLE_PET_STATUS.Zhan then
            self.cur_zhan_pet = i
        end
        self.pet_list[i]:Trace("status")
    end

    self.fabao_list:Clear()
    for i = 1, #role_info.fabao_list do
        self.fabao_list[i] = SmartData.New(role_info.fabao_list[i])
    end

    self.base.fabao_cd = role_info.fabao_cd
    self.base.fabao_remain_times = role_info.fabao_remain_times
    self.base.used_mp_item_times = role_info.used_mp_item_times
    self.base.max_mp_item_times = role_info.max_mp_item_times

    self:SetMpCanUseTime(self.base.max_mp_item_times - self.base.used_mp_item_times)
    self:SetFakePotion(role_info.fake_potion_num, role_info.fake_potion_list)
end
function BattleData:SetFakePotion(num, list)
    self.base.fake_potion_num = num
    self.base.fake_potion_list = list
end
function BattleData:SetMpCanUseTime(time,mark_cancel)
    self.base.mp_item_can_use_tiems = time
    self.base.mp_item_mark_cancel = mark_cancel
end

function BattleData:GetMpCanUseTimeMarkCancel()
    local result = self.base.mp_item_mark_cancel and 1 or 0
    self.base.mp_item_mark_cancel = nil
    return result == 1
end

function BattleData:PlayerSkillInfo()
    return self.player_skill_info
end
function BattleData:PlayerHeartSkillCount()
    return self.player_heart_skill_count
end
function BattleData:PlayerHeartSkill()
    return self.player_heart_skill_info
end

function BattleData:FabaoCD(cd)
    if cd then
        if cd > 0 then
            self.base.fabao_cd = cd
        else
            self.base.fabao_cd = math.max(0, self.base.fabao_cd - 1)
        end
    else
        return self.base.fabao_cd
    end
end

function BattleData:FabaoInfoChange(times)
    if times then 
        if times > 0 then 
            self.base.fabao_remain_times = times
        else 
            self.base.fabao_remain_times = math.max(0, self.base.fabao_remain_times - 1)
        end 
    else 
        return self.base.fabao_remain_times
    end 
end

function BattleData:PetList()
    return self.pet_list
end

function BattleData:FabaoList()
    return self.fabao_list
end

function BattleData:SetMpItemInfo(use, max)
    self.base.used_mp_item_times = use
    self.base.max_mp_item_times = max
end
function BattleData:PopupData()
    return self.hp_popup
end

-- 飘字数据
function BattleData:PushPopupData(data)
    self.hp_popup[self.hp_popup:Length() + 1] = data
end

function BattleData:PopPopupData()
    local val = self.hp_popup:Val()
    if #val > 0 then
        self.hp_popup:Set({})
    end
    return val
end

function BattleData:MovePopupData()
    return self.move_popup
end

-- 飘字数据
function BattleData:PushMovePopupData(data)
    self.move_popup[self.move_popup:Length() + 1] = data
end

function BattleData:PopMovePopupData()
    local val = self.move_popup:Val()
    if #val > 0 then
        self.move_popup:Set({})
    end
    return val
end

function BattleData:SetSkillSelData(is_show, cmd_i, skill_sel_type)
    self.skill_click = cmd_i
    self.view_data.skill_sel_type = 1
    if skill_sel_type ~= nil then
        self.view_data.skill_sel_type = skill_sel_type
     end
    if is_show then
        self.view_data.opened_sub = BattleSubView.Skill
    else
        if self.view_data.opened_sub == BattleSubView.Skill then
            self.view_data.opened_sub = BattleSubView.None
        end
    end
    if is_show and cmd_i == nil then
        cmd_i = self.base.command_i
    end
    if cmd_i ~= nil then
        self.view_data.skill_sel_cmdi = cmd_i
    end
end

---一次指令输入完成时调用，当为第一个指令是指令ID设置为2
--*战斗指令完成
function BattleData:CmdComplete(cmd_type, cmd_p1, cmd_p2, cmd_p3)
    if self.base.command_i == 2 then
        self:SetRoundPhase(BATTLE_ROUND_PHASE.CommandDone)
    else
        -- LogErrorHL("command_i turn into 2")
        self.base.command_i = 2
    end
    if cmd_type ~= nil then
        self.base.last_cmd_type = cmd_type
        self.base.last_cmd_p1 = cmd_p1
        self.base.last_cmd_p2 = cmd_p2
        self.base.last_cmd_p3 = cmd_p3
    end
end

--perhaps die
function BattleData:IsChExist(idx)
    return self:GetChInfo(idx) ~= nil
end

--not exist return false,exist and hp>0
function BattleData:IsChAlive(idx)
    local ch_info = self:GetChInfo(idx)
    if ch_info == nil then
        return false
    end
    return ch_info.curhp > 0
end
function BattleData:IsPetAlive(idx)
    local ch_info = self:GetChInfo(idx)
    if ch_info == nil then
        return self:PlayerIdx()
    end
    return idx
end

function BattleData:IsDataChAlive(idx)
    local ch_info = self:GetDataChInfo(idx)
    if ch_info == nil then
        return false
    end
    return ch_info.curhp > 0
end

--两个idx之间是否符合近战规则
function BattleData:IsFitDisRule(from_idx, to_idx)
    --同一阵营
    if BattleData.AreInSameSide(from_idx, to_idx) then
        return true
    end
    local is_front = BattleData.IsFrontIdx(from_idx)
    if is_front then
        return true
    end
    is_front = BattleData.IsFrontIdx(to_idx)
    if is_front then
        return true
    end
    if from_idx == self:PlayerIdx() then
        local player_ch = self:GetChInfo(from_idx)
        if player_ch and player_ch.weapon_type == WEAPON_TYPE.BOW then
            return true
        end
    end

    if not self:IsChAlive(from_idx + 5) then
        return true
    end
    if not self:IsChAlive(to_idx + 5) then
        return true
    end
    return false
end

--当前执行命令的idx，根据宠物的存在与否和cmd_i决定
function BattleData:CurCmdIdx()
    local re_idx
    if self.base.command_i == 2 then --二动时
        local pet_idx = self:PlayerPetIdx()
        re_idx = self:IsPetAlive(pet_idx)
    else
        re_idx = self:PlayerIdx()
    end
    return re_idx
end

function BattleData:IsOffensive()
    if not self.base then
        return false
    end
    return self.base.role_side == 0
end
-- 互换位置
function BattleData.GetSwitchedIdx(idx)
    if not BattleData.Instance:BaseData() then
        return idx
    end
    if BattleData.IsFrontIdx(idx) then
        return 26 - idx
    else
        return 16 - idx
    end
end

function BattleData.GetSwitchedIdx2(idx)
    if not BattleData.Instance:BaseData() then
        return idx
    end
    local is_front = BattleData.IsFrontIdx(idx)
    return is_front and (idx - 5) or (idx + 5)
end

function BattleData.PowerSkillJudge(idx1, idx2)
    if BattleData.IsFrontIdx(idx1) then
        return (idx1 - idx2 == -1) or (idx1 - idx2 == 5) or ((idx1 - idx2 == 1) and (idx1 ~= 16 and idx1 ~= 6))
    else
        return (idx1 - idx2 == 1) or (idx1 - idx2 == -5) or ((idx1 - idx2 == -1) and (idx1 ~= 15 and idx1 ~= 5))
    end
end

function BattleData.IsEnemyIdx(idx)
    if idx > 20 and idx < 100 then
        return BattleData.IsEnemyHallowIdx(idx)
    end
    if not BattleData.Instance:BaseData() then
        return idx > 10
    end
    --pvp 且是守方
    if BattleData.Instance:IsPVP() and not BattleData.Instance:IsOffensive() then
        return idx <= 10
    --观战 且是守方
    elseif --[[ BattleData.Instance:IsOB() > 0 and ]] BattleData.Instance:OBSide() == 1 then
        return idx <= 10
    else
        return idx > 10
    end
end

function BattleData.IsEnemyHallowIdx(idx)
    if not BattleData.Instance:BaseData() then
        return idx > 25
    end
    --pvp 且是守方
    if BattleData.Instance:IsPVP() and not BattleData.Instance:IsOffensive() then
        return idx <= 25
    --观战 且是守方
    elseif --[[ BattleData.Instance:IsOB() > 0 and ]] BattleData.Instance:OBSide() == 1 then
        return idx <= 25
    else
        return idx > 25
    end
end

function BattleData.AreInSameRow(idx1, idx2)
    return math.floor((idx1 - 1) / 5) == math.floor((idx2 - 1) / 5)
end

function BattleData.AreInSameCol(idx1, idx2)
    local is_front = BattleData.IsFrontIdx(idx1)
    return is_front and (idx1 - 5 == idx2) or (idx1 + 5 == idx2)
end

function BattleData.AreInSameSide(idx1, idx2)
    return math.floor((idx1 - 1) / 10) == math.floor((idx2 - 1) / 10)
end

function BattleData.IsFrontIdx(idx)
    return (idx >= 6 and idx <= 10) or (idx >= 16 and idx <= 20)
end

function BattleData.BuffConfig(buff_id)
    local re

    re = Config.buff_list_auto.buff_list[buff_id]
    if re == nil then
        LogErrorBT("未找到buff配置 ", buff_id)
    end
    return re
end

function BattleData.BuffLevelConfig(buff_id, level)
    -- LogError(buff_id .. " " .. level)
    local level = level or 1
    local basic = Config.buff_list_auto.buff_list[buff_id]
    if basic == nil then
        LogErrorBT("未找到buff配置 ", buff_id)
        return
    end
    local sheet = Config.buff_list_auto[basic.buff_sheet]
    return sheet[level]
end

function BattleData:IsLeader()
    if BattleData.Instance:IsPVP() and not BattleData.Instance:IsOffensive() then
        return self.player_idx == 13 or self.player_idx == 18
    elseif --[[ BattleData.Instance:IsOB() > 0 and ]] BattleData.Instance:OBSide() == 1 then
        return self.player_idx == 13 or self.player_idx == 18
    else
        return self.player_idx == 3 or self.player_idx == 8
    end
end

function BattleData:SetBattleStart(protocol)
    self.need_boss_come = 1 == protocol.is_boss_fight
    self.is_mine_monster = 1 == protocol.is_mine_monster
    self.boss_group_id = protocol.boss_group_id
    self.can_fast_forward = 1 == protocol.can_fast_forward
end

--获取当前战斗场景的怪物组ID
function BattleData:GetBossGroupId()
    return self.boss_group_id or 0
end
--获取是否可以速战
function BattleData:CanFastForward()
    return self.can_fast_forward or false
end
function BattleData:SetIsBoss()
    self.base.is_boss = not self.base.is_boss
end

local wont_change_cur_zhan_pet = {
    [0] = true,
    -- [2] = true,
}

function BattleData:SetPetStatus(status, idx)
    if idx then
        if status == BATTLE_PET_STATUS.Zhan then
            if self.cur_zhan_pet and self.pet_list[self.cur_zhan_pet] and self.pet_list[self.cur_zhan_pet].status == BATTLE_PET_STATUS.Zhan then
                self.pet_list[self.cur_zhan_pet].status = BATTLE_PET_STATUS.Dai
            end
            if self.pet_list[idx] then
                self.pet_list[idx].status = BATTLE_PET_STATUS.Zhan
                self.cur_zhan_pet = idx
                --修改显示技能(要有技能才行)
                local first_skill = self.pet_list[idx].skill_list[1]
                if first_skill then
                    local sce_move = BattleData.Instance:AutoBehaviour(2)
                    sce_move.type = BATTLE_MOVE_TYPE.PERFORM_SKILL
                    sce_move.skill_id = first_skill.skill_id
                    sce_move.skill_lv = first_skill.skill_level
                end
            end
        else
            if self.cur_zhan_pet == idx and not wont_change_cur_zhan_pet[status] and wont_change_cur_zhan_pet[self.pet_list[idx].status] then
                self.cur_zhan_pet = nil
            end
            if self.pet_list[idx] then
                self.pet_list[idx].status = status
            end
        end
    elseif self.cur_zhan_pet then
        if self.pet_list[self.cur_zhan_pet] then
            self.pet_list[self.cur_zhan_pet].status = status
        end
        if not wont_change_cur_zhan_pet[status] then
            self.cur_zhan_pet = nil
        end
    end
end

BattleEffectKey = {
    Move = 10010247,
    Hit = 10010248
}

function BattleData:GetOtherData()
    return self.other_data
end

-- 播放战斗界面大特效
function BattleData:AddBattleEffect(eff_key, on_begin, on_end)
    table.insert(self.other_data.battle_effect_play_list, {eff_key = eff_key, on_begin = on_begin, on_end = on_end})
    self.other_data.play_battle_effect = true
end

function BattleData:ClearBattleEffect()
    self.other_data.battle_effect_play_list = {}
    self.other_data.play_battle_effect = false
end

function BattleData:IsPVP()
    if not self.base then
        return false
    elseif self:BattleMode() == BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA then
        return false
    end
    return self.base.is_pvp
end
function BattleData:IsVideo()
    if not self.base then
        return false
    end
    return self.base.video_playing
end
function BattleData:IsOB(type)
    if type == nil then
        return self.base.is_ob
    else
        self.base.is_ob = type
    end
end
function BattleData:OBSide(ob_side)
    if self.base == nil then
        return 0
    end
    if ob_side == nil then
        return self.base.ob_side
    else
        self.base.ob_side = ob_side
    end
end

function BattleData:IsBoss()
    if self.base.monster_group_id == -1 then
        return false
    end
    local config = Cfg.MonsterGroupById(self.base.monster_group_id)
    return config and config.is_boss == 1 or false
end

function BattleData.IsStory()
    local battle_mode = BattleData.Instance:BattleMode()
    return battle_mode == BATTLE_MODE.BATTLE_MODE_STORY
end


--获取战斗剩余时间
function BattleData:GetLimitTimeStamp()
    return self.base.limit_timestamp
end

--是否是暗怪战斗
function BattleData:IsMineMonster()
    return self.is_mine_monster or false
end

function BattleData:IsTeamUp()
    return self.base.is_team_up
end

function BattleData:GetCountDownOffset()
    --[[ if self.base.cur_round == 1 then
        return self.base.auto == true and 27 or 0
    else
        return self.base.auto == true and 29 or 0
    end ]]
    if self.base.cur_round == 1 then
        return (self.base.auto == true or self.base.battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE) and 27 or 0
    else
        return (self.base.auto == true or self.base.battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE) and 29 or 0
    end
end
function BattleData:GetPlayerChInfo()
    local ch
    ch = self:GetChInfo(3)
    if ch and ch.character_type == 0 then
        return ch
    end
    ch = self:GetChInfo(8)
    if ch and ch.character_type == 0 then
        return ch
    end
    return nil
end
--修改为默认技能
function BattleData:FlushDefaultSkill(pet_skill_id)
    --当前角色不是主角
    local ch = self:GetChInfo(self:PlayerIdx())
    local id = RoleData.Instance:GetRoleId()
    local guide = GuideManager.Instance:Guide()
    if guide and GuideData.GuideList[guide.id] then
        return--破防指引不需要
    end
    if ch and ch.character_id == id then
        local skill_id = GuideData.Instance:GetSkillId()
        local info = SkillData.Instance:SkillInfo(skill_id)
        local first_move = BattleData.Instance:AutoBehaviour(1)
        first_move.type = BATTLE_MOVE_TYPE.PERFORM_SKILL
        first_move.skill_id = skill_id
        first_move.skill_lv = info ~= nil and info.skill_level or 1
        local param_t = {{}}
        param_t[1].first_move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
        param_t[1].first_skill_id = skill_id
        param_t[1].first_skill_level = info ~= nil and info.skill_level or 1
        if pet_skill_id then
            param_t[2] = {}
            param_t[2].second_move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
            param_t[2].second_skill_id = pet_skill_id
            param_t[2].second_skill_level = 1
        end
        FightCtrl.Instance:SendSetBattleAutoMove(param_t)
    end
end

function BattleData.RecordData()
    if IS_EDITOR and table.nums(BattleData.Instance.battle_record_list) > 0 then
        FileTool.WriteAllText("BattleData/" .. tostring(os.date("%Y%m%d-%H%M%S")) .. ".txt", TableToString(BattleData.Instance.battle_record_list))
        -- FileTool.WriteAllText("Assets/BattleData/"..tostring(os.date("%Y-%m-%d-%H-%M-%S"))..".txt",TableToString(BattleData.Instance.battle_record_list))
        BattleData.Instance.battle_record_list = {}
    end
end
---
function BattleData:SetAutoInfoList(procotol)
    self.auto_info_list = procotol.list
end
function BattleData:SetSingleAutoInfo(procotol)
    self.auto_info_list[procotol.role_id] = procotol.is_auto
end
function BattleData:GetAutoInfo(role_id)
    if role_id == nil then return 0 end
    return self.auto_info_list[role_id] or 0
end
function BattleData:SpecialEffectList(list)
    self.battle_spe_list = list
end
function BattleData:HaloList(halo_list)
    self.battle_halo_list = halo_list
end
function BattleData:GetHaloList()
    return self.battle_halo_list
end
function BattleData:GetSpecialEffectList(idx)
    if self.battle_spe_list == nil then
        return nil
    end
    return self.battle_spe_list[idx]
end
function BattleData:SetSpecialEffectList(idx, sp_id)
    if self.battle_spe_list == nil then
        self.battle_spe_list = {}
    end
    self.battle_spe_list[idx] = self.battle_spe_list[idx] or {}
    table.insert(self.battle_spe_list[idx], sp_id)
end
function BattleData:RemoveSpecialEffectList(idx, sp_id)
    if self.battle_spe_list == nil then
        return
    end
    self.battle_spe_list[idx] = self.battle_spe_list[idx] or {}
    for key, value in pairs(self.battle_spe_list[idx]) do
        if value == sp_id then
            table.remove(self.battle_spe_list[idx], key)
        end
    end
end
function BattleData:SetSpecialEffectListDelay(idx, list)
    if self.battle_spe_list_delay == nil then
        self.battle_spe_list_delay = {}
    end
    self.battle_spe_list_delay[idx] = list
end
function BattleData:CheckSpecialEffectListDelay(idx)
    if self.battle_spe_list_delay == nil then
        return
    end
    if self.battle_spe_list == nil then
        self.battle_spe_list = {}
    end
    if self.battle_spe_list_delay[idx] then
        self.battle_spe_list[idx] = self.battle_spe_list_delay[idx]
        self.battle_spe_list_delay[idx] = nil
    end
end
function BattleData:ClearSpecialEffectList(idx)
    if self.battle_spe_list == nil then
        return
    end
    self.battle_spe_list[idx] = nil
end
--战斗中是否可以使用该物品
function BattleData:PetUseItemInBattle(item)
    local command_i = self.base.command_i
	if command_i == 2 then
	    local player_cmd = self:CurCmdIdx() == self:PlayerIdx()
		if player_cmd == false and self.base.last_cmd_type == BATTLE_MOVE_TYPE.USE_ITEM and 
            self.base.last_cmd_p2 == item:GridColumn() and self.base.last_cmd_p3 == item:GridIndex() and item:Num() == 1 then
            return false
        end
    end
    return true
end

function BattleData:SetBattleStatisticResult(protocol)
    local total_damage_to_other, total_suffer_injure, total_heal_num = 0, 0, 0
    local stat_list = {}
    local function func_push(info)
        table.insert(stat_list, info)
        total_damage_to_other = math.max(total_damage_to_other, info.total_damage_to_other)
        total_suffer_injure = math.max(total_suffer_injure, info.total_suffer_injure)
        total_heal_num = math.max(total_heal_num, info.total_heal_num)
    end
    for i, v in ipairs(protocol.role_stat_list) do
        v.obj_type = 0
        func_push(v)
    end
    for i, v in ipairs(protocol.partner_stat_list) do
        v.obj_type = 1
        func_push(v)
    end
    for i, v in ipairs(protocol.pet_stat_list) do
        v.obj_type = 2
        func_push(v) 
    end
    local all_info = {}
    all_info.total_damage_to_other = total_damage_to_other
    all_info.total_suffer_injure = total_suffer_injure
    all_info.total_heal_num = total_heal_num
    all_info.stat_list = stat_list
    self.battle_statistics:Set(all_info)
end

function BattleData:ClearBattleStatistic()
    self.battle_statistics:Set({})
end

function BattleData:GetBattleStatisticResult()
    return self.battle_statistics
end

function BattleData:GetBattleStatisticResultMax(index)
    local list = {"total_damage_to_other", "total_heal_num", "total_suffer_injure"}
    return math.max(self.battle_statistics[list[index]] or 1,1)
end

function BattleData.FinishAfterBattle()
    if BattleData.Instance:IsServerFighting() then
		PublicPopupCtrl.Instance:Center(ErrorText[157])
	end
end
function BattleData:IsBossAudio()
    return BattleData.Instance:IsBoss() or BattleHelper.BossAudioGroup[self.base.monster_group_id]
end

function BattleData:IsPVPAudio()
    return BattleData.Instance:IsPVP() or BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_STORY
end
function BattleData:BattleMap()
    local map_id = SceneData.Instance:MapId()
    local battle_cfg = Config.world_map_auto.scene_to_scene[map_id]
    local sn = battle_cfg and battle_cfg.map_name or "HCZD_1"
    local mode_cfg = Config.world_map_auto.moshi_to_scene[self:BattleMode()]
    sn = mode_cfg and mode_cfg.map_name or sn
    local group_cfg = Config.world_map_auto.zhandou_to_scene[BattleData.Instance:BaseData().monster_group_id]
    sn = group_cfg and group_cfg.map_name or sn
    sn = self:BattleMode() == BATTLE_MODE.BATTLE_MODE_BATTLEFIELD and Config.illusory_battlefield_auto.activity_cfg[1].battle_map_id or sn
    if AchieveNewData.Instance:GetChanllageMapId() ~= nil then
        sn = Config.world_map_auto.scene_to_scene[AchieveNewData.Instance:GetChanllageMapId()].map_name
        AchieveNewData.Instance:ClearChanllageMapId()
    end
    if HarmonyHeavenData.Instance:GetHHVideoMapId() ~= nil then
        local co = Config.world_map_auto.scene_to_scene[HarmonyHeavenData.Instance:GetHHVideoMapId()]
        sn = co and co.map_name or sn
        HarmonyHeavenData.Instance:ClearHHVideoMapId()
    end
    if self:BattleMode() == BATTLE_MODE.BATTLE_MODE_GUILD_BOSS 
        and GuildData.Instance:GetFamilyBossMapId() ~= nil then

        sn = Config.world_map_auto.scene_to_scene[GuildData.Instance:GetFamilyBossMapId()].map_name
        GuildData.Instance:ClearFamilyBossMapId()
    elseif self:BattleMode() == BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY then
        sn = Config.pet_island_auto.other[1].battle_map
    elseif self:BattleMode() == BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA then
        sn = Config.cloud_battlefield_auto.others[1].scene_battle
    end
    if BattleData.Instance:IsVideo() then
        local video = BattleVideo.Instance:PlayVideoCfg()
        if video then
            sn = video.map_id
        end
    end
    if SchoolExerciseData.Instance:IsSchoolBattle(BattleData.Instance:BaseData().monster_group_id) then
        sn = Config.school_exercise_auto.other[1].battle_map_id
    end
    return sn
end
function BattleData.BattleTips()
    local group_id = BattleData.Instance:BaseData().monster_group_id
    local config = Config.strategic_tips_auto.strategic_tips_config[group_id]
    --Cfg.MonsterGroupById(self.base.monster_group_id)
    if config then
        return true, config
    end
    return false
end

function BattleData.IsHasDamageTips()
    local group_id = BattleData.Instance:BaseData().monster_group_id
    local config = Config.family_auto.family_boss
	for key, value in pairs(config) do
		if value.boss_id == group_id then
			return true
		end
	end
    return false
end

function BattleData.IsHasNvWaShiBuff(grid_idx)
    local nvwashi = false
    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch and data_ch.buff_list then
        for key, value in pairs(data_ch.buff_list) do
            if value == 543 then
                nvwashi = true
            end
        end
    end
    return nvwashi
end
function BattleData.IsHasBingFengBuff(grid_idx)
    local nvwashi = false
    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch and data_ch.buff_list then
        for key, value in pairs(data_ch.buff_list) do
            if BattleHelper.BingFeng[value] == true then
                nvwashi = true
            end
        end
    end
    return nvwashi
end
--[[ function BattleData.IsHuiYan(grid_idx, buff_id)
    if buff_id and buff_id == BattleHelper.HuiYan then--or buff_id == BattleHelper.HuiYan2) then
        return  grid_idx == BattleData.Instance:PlayerIdx() or grid_idx == BattleData.Instance:PlayerPetIdx()
    end
    return false
end ]]
function BattleData:HasHuiYan()
    local huiyan = false
    local idx = self:CurCmdIdx()
    for buff_id, _ in pairs(BattleHelper.HuiYanBuff) do
        huiyan = BattleData.IsHasBuffId(idx, buff_id)
        if huiyan == true then
            return huiyan
        end
    end
    --[[ for sp_id, _ in pairs(BattleHelper.HuiYanEffect) do
        huiyan = BattleData.IsHasEffectId(idx, sp_id)
        if huiyan == true then
            return huiyan
        end
    end ]]
    return huiyan
end
function BattleData:HasYinSheng(idx)
    local yinsheng = false
    idx = idx or self:CurCmdIdx()
    for buff_id, _ in pairs(BattleHelper.YinShengBuff) do
        yinsheng = BattleDisplayer.Instance:IsHasBuffId(idx, buff_id)
        if yinsheng == true then
            return yinsheng
        end
    end
    for sp_id, _ in pairs(BattleHelper.YinShengEffect) do
        yinsheng = BattleData.IsHasEffectId(idx, sp_id)
        if yinsheng == true then
            return yinsheng
        end
    end
    return yinsheng
end
function BattleData.IsHasBuffId(grid_idx, buff_id)
    local data_ch = BattleData.Instance:GetDataChInfo(grid_idx)
    if data_ch and data_ch.buff_list then
        for key, value in pairs(data_ch.buff_list) do
            if type(value) =="table" and value.new_buff_id == buff_id then
                return true
            elseif type(value) =="number" and value == buff_id then
                return true
            end
        end
    end
    return false
end
function BattleData.IsHasEffectId(grid_idx, sp_eff_id)
    local spe_list = BattleData.Instance:GetSpecialEffectList(grid_idx)
    local temp
    if spe_list then
        for key, value in pairs(spe_list) do
            temp = BagData.Instance:GetEquipEffectCfg(value)
            if temp and temp.is_hide == 0 and temp.sp_id == sp_eff_id then
                return true
            end
        end
    end
    return false
end

--[[ function BattleData:IsHasInvisibleCh()
    local temp = {}
    for idx, ch in pairs(self.data_ch_info) do
        if ch and ch.buff_list then
            for key, value in pairs(ch.buff_list) do
                if value == BattleHelper.YinSheng then --or value == BattleHelper.YinSheng2 then
                    table.insert(temp, idx)
                end
            end
        end
    end
    return temp
end ]]
function BattleData:IsLiuXingOrHuiXing(idx)
    if self.battle_spe_list == nil then
        return 1
    end
    if self.battle_spe_list[idx] then
        local temp
        for _, value in ipairs(self.battle_spe_list[idx]) do
            temp = BagData.Instance:GetEquipEffectCfg(value)
            if temp and temp.name == Language.BuffName.LiuXing then
                return 1
            elseif temp and temp.name == Language.BuffName.HuiXing then
                return 2
            end
        end
    end
end
function BattleData:IsShowStrategyBtn()
    if self:BattleMode() == BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE then
        return false
    end
    if self:BattleMode() == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY then
        return true
    end
    if BattleData.Instance:IsLeader() and BattleData.Instance:IsOB() == 0 and BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        local player_idx = BattleData.Instance:PlayerIdx()
        for idx, ch_info in pairs(BattleData.Instance:ChInfo()) do
            if ch_info.character_type == BATTLE_CH_TYPE.PARTNER and BattleData.AreInSameSide(idx, player_idx) then
                return true
            end
        end
        if TeamData.Instance:IsLeader() then
            return true
        end
    end
    if TeamData.Instance:InTeam() and BattleData.Instance:IsOB() == 0 then
        return true
    end
    return false
end
function BattleData:InEmbattleWatching()
    return self:ViewInfo().embattle_watching
end

function BattleData:HadPetDie()
    for index, value in ipairs(self.pet_list) do
        if value.status == BATTLE_PET_STATUS.Wang then
            return self.cur_zhan_pet == nil
        end
    end
    return false
end
function BattleData:HasPetWait()
    for index, value in ipairs(self.pet_list) do
        if value.status == BATTLE_PET_STATUS.Dai then
            return true
        end
    end
    return false
end
function BattleData:ChangePetGuide(flag)
    if flag ~= nil then
        self.change_pet_gudie = flag
    end
    return self.change_pet_gudie
end
function BattleData:CustomStrategy()
    return self.battle_custom_strategy
end
function BattleData:IsPurifyEffect(buff_type)
    return self.purify_effect_list[buff_type] == true
end
function BattleData:CanQuickBattle()
    return self:CanFastForward()
    -- local battle_mode = self.base.battle_mode
    -- local config = Config.rapid_strike_auto.quick_battle
    -- local level = RoleData.Instance:GetRoleLevel()
    -- for k, v in pairs(config) do
    --     if v.type ~= 0 and battle_mode == v.type
    --     and level >= v.open_level then
    --         return true
    --     end
    -- end
    -- local id = self.base.monster_group_id
    -- local cfg = Config.monster_group_auto.monster_group[id]
    -- if cfg and cfg.is_challage == 1 then
    --     return true
    -- end
end
function BattleData:CanSkipBattle()
    local battle_mode = self.base.battle_mode
    local level = RoleData.Instance:GetRoleLevel()
    if BATTLE_SHOW_SKIP[battle_mode] then
        if battle_mode == BATTLE_MODE.BATTLE_MODE_ARENA then
            return level >= Config.arena_cfg_auto.other[1].skip_open_level
        elseif battle_mode == BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA then
            local is_can = self.base.cur_round >= Config.cloud_battlefield_auto.others[1].accept_round
            local is_tip = true
            return is_can,is_tip
        elseif battle_mode == BATTLE_MODE.BATTLE_MODE_BATTLEFIELD then
            local is_can = self.base.cur_round >= 2 and (TeamData.Instance:IsLeader() or not TeamData.Instance:InTeam())
            local is_tip = true
            return is_can, is_tip
        else
            return true
        end
        return 
    end
    return false
end
function BattleData:QuickBattleRound()
    local battle_mode = self.base.battle_mode
    local config = Config.rapid_strike_auto.quick_battle
    for k, v in pairs(config) do
        if v.type ~= 0 and battle_mode == v.type then
            return v.skip
        end
    end
    return 4
end
function BattleData:SetSQList(list)
    self.sheng_qi_list = list
end
function BattleData:GetSQList()
    return self.sheng_qi_list or {}
end
function BattleData:SetSQSkill(data)
    self.cur_shengqi_skill = data
end
function BattleData:GetSQSkill()
    return self.cur_shengqi_skill
end
function BattleData:SetSQInfo(idx, info)
    self.sheng_qi_list[idx] = info
end
function BattleData:GetSQInfo(idx)
    return self.sheng_qi_list[idx]
end
--获取圣器拥有人信息
function BattleData:SQOwnerInfo(grid_idx)
    local info = self.sheng_qi_list[grid_idx]
    if info == nil then
        return nil
    end
    local owner_idx = info.owner_idx
    local ch_info = self:GetDataChInfo(owner_idx)
    if ch_info and
        (ch_info.character_type == BATTLE_CH_TYPE.ROLE or ch_info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW) then
        return ch_info
    end
    owner_idx = BattleData.GetSwitchedIdx2(owner_idx)
    ch_info = self:GetDataChInfo(owner_idx)
    if ch_info and
        (ch_info.character_type == BATTLE_CH_TYPE.ROLE or ch_info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW) then
        return ch_info
    end
    LogDG("没有获取的圣器主人的信息，不显示圣器技能展示UI")
    return nil
end
function BattleData:CreateRewardBox(pos, forward, end_func)
    -- local battle_mode = self:BattleMode()
    -- battle_mode = BATTLE_MODE.BATTLE_MODE_FALLEN_GOD
    local info = self:GetBoxInfo()
    if info then
        local vo = Config.npc_auto.npc_list[info.box_type]
        vo = TableCopy(vo)
        vo.box_color = info.box_color or ItemColor.Purple
        self:RealCreateBox(vo, pos, forward, end_func)
    else
        if end_func then
            end_func()
        end
    end
    --[[ RunUntilTrue(function ()
        if SceneCtrl.Instance:CurScene():GetClassType() == GameScene and SceneCtrl.Instance:CurScene():LoadingComplete() then
            self:RealCreateBox(vo, pos)
            return true
        end
    end) ]]
end

function BattleData:RealCreateBox(info, pos, forward, end_func)
    if self.reward_box then
        self.reward_box:DeleteMe()
        self.reward_box = nil
    end
    local vo = CommonStruct.NpcVo(info)
    vo.obj_type = SceneObjType.Box
    vo.not_affect_yinyang = true
    vo.is_open = 0
    vo.box_color = info.box_color
    vo.end_func = end_func
    vo.obj_id = nil
    self.reward_box = SceneObj.CreateObj(vo)
    self.reward_box:SetForward(forward)
    self.reward_box:SetPos(pos)
end
function BattleData:ClearRewardBox()
    -- ViewMgr:CloseView(GuideBlock)
    if self.reward_box then
        self.reward_box:DeleteMe()
        self.reward_box = nil
    end
end
function BattleData:GetBoxInfo()
    local battle_mode = self:BattleMode()
    local groud_id = self:GetBossGroupId()
    local info = nil
    if battle_mode ~= 0 then
        info = Cfg.BoxRewardBattleMode(battle_mode, false)
    end
    if info == nil then
        if groud_id ~= 0 then
            info = Cfg.BoxRewardGroupId(groud_id, false)
        end
    end
    return info
end
function BattleData:CheckIsCanShowBox()
    local info = self:GetBoxInfo()
    if info == nil then
        return false
    end
    if self.is_show_boss_box == true then
        return false
    end
    if not self.is_show_boss_box then
        self.is_show_boss_box = true
    end
    return true
end

-- idx == 41~50列
function BattleData:SetRabbitShield(idx, has_shield)
    if not has_shield then
        self.rabbit_shield_list[idx] = nil
    else
        self.rabbit_shield_list[idx] = true
    end
end
function BattleData:GetRabbitShield(idx)
    return self.rabbit_shield_list[idx]
end

-- =============== 神龙专区 =============
function BattleData:SetDragonList(info_list)
    for i,info in pairs(info_list) do
        info.is_bottom_info = true
        info.rage_point = BattleDragon.ShowRagePointSkin[info.skin_seq]
        info.rage_value = 0
        if info.skin_seq > 0 then
            local cfg = FairyDragonData.Instance:GetSkinCfgBySeq(info.skin_seq)
            info.name = cfg.skin_name
        else
            info.name = Language.FairyDragon.Name
        end
        local sd
        if SmartData.Check(info) == false then
            sd = SmartData.New(info, true)
        else
            sd = info
        end
        self.dragon_list[info.grid_idx] = sd
    end
end

function BattleData:GetDragonList()
    return self.dragon_list or {}
end

function BattleData:GetDragonCh(idx)
    return self.dragon_list[idx]
end


-- =====================================



DataHolder = DataHolder or DeclareMono("DataHolder")

function DataHolder:Awake()
    local mgi = BattleData.Instance:BaseData().monster_group_id
    if Config.promeval_paradise_auto.special_boss[mgi] then
        self.CameraTran:SetObjActive(false)
        self.CamBoss:SetActive(true)
    end
end
