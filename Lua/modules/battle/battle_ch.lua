local root_path = "actor/__ActorRoot.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            root_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("__ActorRoot pool register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

BattleCh = BattleCh or BaseClass()

local wait_load_path = "actor/npc/350.prefab"
local move_speed = 15 * 3.5
local step_dis = move_speed * 0.025

function BattleCh:__init(data)
    self.v3_scale = Vector3.one
    self.data = data
    self.vo = data
    self.root_node = GameObjPool:Get(root_path)
    self.root_transform = self.root_node.transform
    self.vo.root_node = self.root_node
    self.vo.show_hp = 0 --0关1开

    self.TopPoint = self.root_node:FindChild("TopPoint")
    if self.TopPoint then
        self:setScaleSeal(1)
    end

    self.move_wait = nil
    self.is_show = true

    self.vo.height = self:Height()

    self.buff_list = {}
    self.special_list = {}
    self.finished_replace = {}
    self.effect_buff_player = {}

    self.child_cache = {}

    self.default_scale = 1
    self.current_scale = 1

    self.ch_data = SmartData.New({flush_buff = false, flush_special = false})
    BattleSkillPool.Instance:Register(self:AttackSkillAssetId())

    self.is_in_shihua = false
    if IS_EDITOR then
        self.root_node.name = string.format("%s[%d]", self.vo.name, self.vo.grid_idx)
    end
end

function BattleCh:__delete()
    TimeHelper:CancelTimer(self.wait_die_check)
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end
    if self.handle_run1 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run1)
        self.handle_run1 = nil
    end
    if self.handle_run2 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run2)
        self.handle_run2 = nil
    end
    if self.handle_run3 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run3)
        self.handle_run3 = nil
    end
    if self.handle_run4 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run4)
        self.handle_run4 = nil
    end
    if self.handle_run5 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run5)
        self.handle_run5 = nil
    end
    if self.handle_run6 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run6)
        self.handle_run6 = nil
    end
    if self.halo_list then
        for _, value in pairs(self.halo_list) do
            for _, v in pairs(value) do
                for _, obj in pairs(v) do
                    GameObjPool:Release(obj)
                end
            end
        end
    end
    if self.effect_buff_player then
        for asset_name, player in pairs(self.effect_buff_player) do
            if player then
                GameBuffManager:RemovePlayer(player, true)
            end
        end
    end
    self.effect_buff_player = {}

    self:ClearBuff()
    if self.fly_eff_handle ~= nil then
        SE:Stop(self.fly_eff_handle.obj)
        self.fly_eff_handle = nil
    end
    BattleSkillPool.Instance:Unregister(self:AttackSkillAssetId())
    GameObjPool:Release(self.root_node)
    self:clearCenterPointFollow()
    
    self.special_list = {}
    self.finished_replace = {}
    if self.body_show then
        self.body_show:DeleteMe()
        self.body_show = nil
    end

    if self.old_body_show then
        self.old_body_show:DeleteMe()
        self.old_body_show = nil
    end
    self.TopPoint = nil
    self.root_node = nil
    if self.vo ~= nil then
        self.vo.root_obj = nil
    end
    self.vo = nil
    self.config = nil

    self.child_cache = {}
end

function BattleCh:SetVo(data)
    self.vo = data
    self.data = data
end
function BattleCh:FlushDrawer(on_complete)
    self.vo.root_obj = self.root_node
    local character_type = self.vo.character_type
    if character_type == BATTLE_CH_TYPE.ROLE or character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        self:FlushRoleDrawer()
    else
        self:FlushOtherDrawer()
    end
    self:WaitLoading(on_complete)
end
function BattleCh:FlushRoleDrawer()
    local handles = nil
    if self.body_show and self.body_show._class_name == "BattleObjDrawer" then
        local handles = self.body_show:GetWaitAnimEventHandle()
        self.body_show:DeleteMe()
        self.body_show = nil
        self.body_show = BattleChDrawer.New(self.root_node)
        --假如存在这种情况应该将wait_anim_event_handle重新赋值给新的drawer
    end
    self.body_show:SetData(self.vo.appearance)
    self.body_show:SetLoadedCallback(function ()
        self.body_show:SetWaitAnimEventHandle(handles)
        self:BodyLoadedCallback()
    end)
    local temp_character_type = self.vo.temp_character_type
    if temp_character_type and temp_character_type == BATTLE_CH_TYPE.MONSTER then
        self.config = Cfg.MonsterById(self.vo.character_id, false)
    end
end
function BattleCh:FlushOtherDrawer(path)
    local handles = nil
    if self.body_show then
        handles = self.body_show:GetWaitAnimEventHandle()
        self.body_show:DeleteMe()
        self.body_show = nil
        --假如存在这种情况应该将wait_anim_event_handle重新赋值给新的drawer
    end
    self.body_show = BattleObjDrawer.New(self.root_node, true)
    local body_path = nil
    local character_type = self.vo.character_type
    local character_id = self.vo.character_id
    local config
    if character_type == BATTLE_CH_TYPE.PET or character_type == BATTLE_CH_TYPE.PET_SHADOW then
        config = Cfg.PetById(character_id, false)
        if config then
            body_path = DrawerHelper.GetPetPath(config.res_id, self.vo.skin_id)
            local battle_mode = BattleData.Instance:BattleMode()
            if battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
                self.vo.name = config.name
            end
            self.config = config
        else
            LogErrorBT("缺失宠物配置", character_id)
        end
    elseif character_type == BATTLE_CH_TYPE.MONSTER then
        config = Cfg.MonsterById(character_id, false)
        if config then
            body_path = DrawerHelper.GetNpcPath(config.res_id)
            self.vo.name = config.name
            self.config = config
        else
            LogErrorBT("缺失怪物配置", character_id)
        end
    elseif self.vo.character_type == BATTLE_CH_TYPE.PARTNER then
        config = Config.partner_cfg_auto.partner_info[character_id]
        if config then
            body_path = DrawerHelper.GetPartnerPath(config.res_id, self.vo.skin_id)
            self.vo.name = config.name
            self.config = config
        else
            if character_id then
                body_path = string.format("actor/npc/%d.prefab", character_id)
            end
            LogErrorBT("缺失伙伴配置", character_id)
        end
    elseif self.vo.character_type == BATTLE_CH_TYPE.NPC_HELPER then
        config = Config.npc_group_auto.npc_group[character_id]
        if config then
            body_path = DrawerHelper.GetNpcPath(config.res_id)
            self.vo.name = config.name
            self.config = config
        else
            LogErrorBT("缺失NPC助战配置", character_id)
        end
    elseif character_type == BATTLE_CH_TYPE.YOU_MING then
        config = YouMingData.Instance:GetYouMingCfg(character_id)
        if config then
            body_path = DrawerHelper.GetNpcPath(config.res_id)
            self.vo.name = config.name
            self.config = config
        else
            LogErrorBT("缺失幽冥配置", character_id)
        end
    end
    body_path = body_path or "actor/npc/508.prefab"
    if not StringIsEmpty(path) then
        body_path = path
    end
    self.body_show:SetData(body_path, nil, function ()
        self.body_show:SetWaitAnimEventHandle(handles)
        self:BodyLoadedCallback()
    end)
    
    --[[ self.body_show:SetLoadedCallback(function ()
        self:BodyLoadedCallback()
    end) ]]
end

function BattleCh:EnableShadow(enable)
    if self.body_show ~= nil then
        self.body_show:EnableShadow(enable)
    end
end

function BattleCh:ChangeEquipment(before_id, after_id)
    local equip_keys = {
        "weapon_id",
        "sub_weapon_id"
    }
    for _, key in pairs(equip_keys) do
        if self.vo.appearance[key] == before_id then
            self.vo.appearance[key] = after_id
            break
        end
    end
end

function BattleCh:ChangeAttribute(attr_key, attr_val)
    -- LogErrorHL("ChangeAttributeBefore", attr_key, self.vo[attr_key])
    self.vo[attr_key] = attr_val
    -- LogErrorHL("ChangeAttributeAfter", attr_key, self.vo[attr_key])
end

function BattleCh:Load(on_complete, real_create_end)
    self.vo.root_obj = self.root_node
    self.root_node:SetActive(false)
    local character_type = self.vo.character_type
    -- local character_id = self.vo.character_id
    if character_type == BATTLE_CH_TYPE.ROLE or character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        self.vo.appearance = SmartData.New(self.vo.appearance)
        self.body_show = BattleChDrawer.New(self.root_node)
        self:FlushRoleDrawer()
        self.vo.appearance:Care(BindTool.Bind(self.FlushRoleDrawer, self))
    else
        self:FlushOtherDrawer()
    end
    if real_create_end then
        self.create_complete = on_complete
        on_complete = nil
    end
    self:WaitLoading(on_complete)
    if self.body_show:IsLoading() == false then
        if on_complete == nil and self.create_complete ~= nil then
            self:WaitLoading(self.create_complete)
            self.create_complete = nil
        end
        return
    end
    -- self:CheckLoad()
    self:ReplaceShow()
end
function BattleCh:CheckLoad()
    local check_time = os.time()
    self.handle_run1 =
        Runner.Instance:RunUntilTrue(
        function()
            if self.body_show:IsLoading() == true and os.time() - check_time >= 0.05 then --讲道理是不想执行这段逻辑的
                self:ReplaceShow()
                return true
            elseif self.body_show:IsLoading() == false then
                return true
            end
            return false
        end
    )
end
function BattleCh:BodyLoadedCallback()
    -- TimeHelper:CancelTimer(self.wait_die_check)
    self:FlushCenterPointFollow()
    self:SetFlyEffect()
    self:SpriteBubble()
    if self.henshin_complete_func then
        self.henshin_complete_func()
        self.henshin_complete_func = nil
    end
end

function BattleCh:SetFlyEffect(appearance)
    appearance = appearance or self.vo

    local enable_flag_fly = appearance.fly_flag and appearance.fly_flag > 0
    local enable_flag_marbas = appearance.zaohua_app_id ~= nil 

    local flag_fly_change = enable_flag_fly and (appearance.fly_flag ~=  self.cache_fly_flag or self.fly_eff_handle == nil)
    local flag_marbas_change = enable_flag_marbas and (appearance.zaohua_app_id ~=  self.cache_marbas_flag or self.fly_eff_handle == nil)
    
    if  flag_fly_change and flag_marbas_change then
        local body_show = self.body_show
        if body_show:IsLoading() and self.old_body_show ~= nil then
            body_show = self.old_body_show
        end
        if not body_show:IsLoading() then--body_show and body_show.main_body and body_show.GetMainBodyHeight and body_show:GetMainBodyHeight() then
            if self.fly_eff_handle ~= nil then
                SE:Stop(self.fly_eff_handle)
                self.fly_eff_handle = nil
            end
            -- 赋值
            self.cache_marbas_flag = appearance.zaohua_app_id or 0
            local show_flag = self.cache_marbas_flag > 0 
            self.cache_fly_flag = show_flag and 0 or appearance.fly_flag
            
            local sp_path =  show_flag and MarbasData.Instance:GetVigorousEffId(self.cache_marbas_flag) 
                or SceneEffectConfig.EffectList["ChFlyUpEffect"..appearance.fly_flag]

            local function fly_eff(obj)
                obj:SetLocalScale(Vector3.New(body_show:GetMainBodyHeight() / 2,body_show:GetMainBodyHeight() / 2,body_show:GetMainBodyHeight() / 2))
            end

            if sp_path ~= nil then 
                self.fly_eff_handle = self:EffectInCenter(sp_path, 
                    show_flag and "cache_marbas_flag" or "cache_fly_flag",
                    show_flag and nil or fly_eff,
                    self.fly_eff_handle,body_show)
            end 
        end
    elseif not enable_flag_fly or not enable_flag_marbas then
        if self.fly_eff_handle ~= nil then
            SE:Stop(self.fly_eff_handle)
            self.fly_eff_handle = nil
        end
        self.cache_fly_flag = 0
        self.cache_marbas_flag = 0
    end
end

-- 与eff_handle绑定的简化代码(请勿用作他用)
function BattleCh:EffectInCenter(sp_path,check_key,extral_func,lock_key,body_show)
    return SE:Play(sp_path, nil,function (obj)
        if self[check_key] and self[check_key] > 0 and body_show.main_body ~= nil then
            if extral_func ~= nil then 
                extral_func(obj)
            end  
        else
            SE:Stop(lock_key)
        end
    end, self:CenterPoint())
end

function BattleCh:ReplaceShow()
    local temp = self.body_show
    self.old_body_show = temp
    self.body_show = BattleObjDrawer.New(self.root_node, true)
    self.body_show:SetData(wait_load_path, true)
    self.root_node:SetActive(self.is_show)
    if self.handle_run4 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run4)
        self.handle_run4 = nil
    end
    self.handle_run4 =
        Runner.Instance:RunUntilTrue(
        function()
            if self.old_body_show == nil then
                return true
            end
            if self.old_body_show:IsLoading() == false --[[ and BattleData.Instance:SceneData() ]] then
                --如果模型没加载成功停止替换
                if self.old_body_show.main_body:getObj() == nil then
                    return true
                end
                self.old_body_show:EnableMain(false)
                local queues = BattleCtrl.Instance:GetStream().behaviour_queues
                for _, v in pairs(queues) do
                    if v:ActorInQueue(self:GetIdx()) == true then
                        return false
                    end
                end
                if self.body_show:IsInAnim(ChAnimType.Idle) then
                    self:ReInit()
                    return true
                elseif self.body_show:IsInAnim(ChAnimType.Death) then
                    self:ReInit(true)
                    return true
                end
            end
            return false
        end
    )
end
local replay_buff = {
    [3] = true,
    [12] = true,
    [383] = true,
    [425] = true,
    [332] = true,
    [376] = true,
    [381] = true,
    [372] = true,
    [640]  = true,
    [653]  = true,
    [4] = true,
    [9] = true,
    [423] = true,
    [466] = true,
}
function BattleCh:ReInit(dia)
    self.child_cache = {}
    local temp = self.body_show
    self.body_show = self.old_body_show
    self.old_body_show = temp
    self.old_body_show:EnableMain(false)
    self.body_show:EnableMain(true)
    if self.create_complete then
        self.create_complete()
        self.create_complete = nil
    end
    if dia == true then
        self:PlayAnim(ChAnimType.Death)
        return
    end
    if self.body_show == nil then
        return
    end
    for k, v in pairs(self.buff_list) do --这里可能其他buff也要
        if replay_buff[v.buff_id] == true then
            self:PlayBuff(
                k,
                BuffStageKey.Start,
                function()
                    self:PlayBuff(k, BuffStageKey.Persistence)
                end
            )
        end
    end
    --可能就死了
    if self.is_in_shihua == true and self.body_show then
        self:SetAnimActive(false)
        self:SetEffectAttacherVisible(false)
    end
    --在这里同步动画
    --[[ if self.body_show and self.body_show.main_body then
        local body = self.body_show.main_body:getObj()
        if body then
            local animators = body:GetComponentsInChildren(typeof(UnityAnim)):ToTable()
            for k, v in pairs(animators) do            
                if v.name == "2034" then
                    self.vice_animator = v
                end
            end
        end
    end ]]
end
function BattleCh:WaitLoading(on_complete)
    if self.handle_run3 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run3)
        self.handle_run3 = nil
    end
    self.handle_run3 =
        Runner.Instance:RunUntilTrue(
        function()
            if self.body_show then
                local flag = self.body_show:IsLoading()
                if flag == false then
                    if self.config and self.config.res_size and self.config.res_size ~= "" then
                        self.default_scale = self.config.res_size
                    else
                        self.default_scale = 1.1
                    end
                    self:setScale(self.default_scale)
                    self.root_node:SetActive(self.is_show)
                    if on_complete then
                        on_complete()
                    end
                end
                return not flag
            end
        end
    )
end
function BattleCh:LookAtCh(ch)
    self:LookAt(ch.root_node.transform)
end

function BattleCh:LookAt(transform)
    self.root_node.transform:LookAt(transform, Vector3.up)
end

function BattleCh:LookAtPos(pos)
    self.root_node.transform:LookAt(pos)
end

function BattleCh:SetPos(pos)
    self.root_node:SetPosition(pos)
end

function BattleCh:GetPos()
    return self.root_node:GetPosition()
end

function BattleCh:GetRoot()
    return self.root_node
end

function BattleCh:SetForward(forward)
    forward.y = 0
    self.root_node:SetForward(forward)
end

function BattleCh:PlayAnim(anim)
    if self.body_show then
        self.body_show:PlayAnim(anim)
    end
end

function BattleCh:SetAnimActive(bool)
    if self.body_show then
        self.body_show:SetAnimActive(bool)
    end
end

function BattleCh:SetEffectAttacherVisible(bool)
    if not self.EffectAttachers then
        self.EffectAttachers = self.root_node:GetComponentsInChildren(typeof(Game.EffectAttacher))
    end
    if self.EffectAttachers then
        for i = 0, self.EffectAttachers.Length - 1 do
            if self.EffectAttachers[i] then
                self.EffectAttachers[i]:SetAllVisible(bool)
            end
        end
    end
end

function BattleCh:WaitAnimEnd(anim, func)
    if self.body_show then
        self.body_show:WaitAnimEvent(anim, "end", func)
    end
end
function BattleCh:ClearWaitDieCheck()
    if self.vo.grid_idx == 11 then
        LogDG("ClearWaitDieCheckClearWaitDieCheck")
    end
    TimeHelper:CancelTimer(self.wait_die_check)
end

function BattleCh:WaitBehitBack(func)
    if self.body_show then
        self.body_show:WaitAnimEvent(ChAnimType.Behit, "behit_back", func)
    end
end

function BattleCh:SetOnAnimHit(func)
    if self.body_show then
        self.body_show:SetOnAnimHit(func)
    end
end

function BattleCh:GetIdx()
    return self.vo.grid_idx
end

function BattleCh:AddBuff(data, on_remove, on_add)
    local buff_key = data.new_buff_key
    local buff_id = data.new_buff_id
    local buff_level = data.new_buff_level
    local basic = BattleData.BuffConfig(buff_id)
    local cfg = BattleData.BuffLevelConfig(buff_id, buff_level)
    self.vo.buff_list[data.new_buff_key] = data
    self.buff_list[buff_key] = {
        buff_id = buff_id,
        buff_level = buff_level,
        sustain_num = data.sustain_num,
        buff_layer = data.buff_layer
    }
    self.ch_data.flush_buff = not self.ch_data.flush_buff
    if cfg and cfg.asset_id ~= "" then
        local asset_name = cfg.asset_id
        GameBuffManager:AddPlayer(
            asset_name,
            self.root_node,
            function(player)
                player.OnRemoveAction = function()
                    self:OnBuffRemove(player, asset_name)
                    if on_remove then
                        on_remove()
                    end
                end
                if self.buff_list[buff_key] then
                    self.buff_list[buff_key].player = player
                else
                    self.buff_list[buff_key] = nil
                    return
                end
                self.buff_list[buff_key].attr_buff_list = BattleBuffManager.Instance:AttrBuff(self.vo.grid_idx, data)
                if basic.ui_type ~= "" then
                    self.buff_list[buff_key].ui_stage_key = BuffStageKey.Persistence
                end
                self:PlayBuff(
                    buff_key,
                    BuffStageKey.Start,
                    function()
                        self:PlayBuff(buff_key, BuffStageKey.Persistence)
                    end
                )
                self:OnBuffStart(player, asset_name)
            end)
    else
        self.buff_list[buff_key].attr_buff_list = BattleBuffManager.Instance:AttrBuff(self.vo.grid_idx, data)
        if basic.ui_type ~= "" then
            self.vo.ui_buff_list[buff_key] = buff_id
        end
    end
    if basic and not StringIsEmpty(basic.extra_logic) then
        local logics = string.split(basic.extra_logic, "|")
        self[logics[1]](self, logics[2], logics[3])
    end
end
function BattleCh:ChangeSustainNum(data)    
    if data == nil then
        return
    end
    if self.buff_list[data.new_buff_key] then
        self.buff_list[data.new_buff_key].sustain_num = data.sustain_num
        --顺便把层数也赋值了
        if data.buff_layer then
            self.buff_list[data.new_buff_key].buff_layer = data.buff_layer
        end
    end
    self.ch_data.flush_buff = not self.ch_data.flush_buff
end

function BattleCh:OnBuffStart(player, asset_name)
    if asset_name == "ShiHua" then
        self:SetAnimActive(false)
        self:SetEffectAttacherVisible(false)
        self.is_in_shihua = true
    elseif asset_name == "BingFeng" then
        self:SetAnimActive(false)
        self:SetEffectAttacherVisible(false)
        self.is_in_shihua = true
    end
end

function BattleCh:OnBuffRemove(player, asset_name)
    if asset_name == "ShiHua" then
        self:SetAnimActive(true)
        self:SetEffectAttacherVisible(true)
        self.is_in_shihua = false
    elseif asset_name == "BingFeng" then
        self:SetAnimActive(true)
        self:SetEffectAttacherVisible(true)
        self.is_in_shihua = false
    end
end
-- ADD PLAY RP REMOVE
function BattleCh:PlayBuff(buff_key, stage_key, on_finished)
    local buff = self.buff_list[buff_key]
    if not buff then
        if self.finished_replace[buff_key] then
            self:PlayBuff(self.finished_replace[buff_key], stage_key, on_finished)
            self.finished_replace[buff_key] = nil
            return
        end
        return
    end
    local has_ui_buff = buff.ui_stage_key and buff.ui_stage_key == stage_key
    if has_ui_buff then
        self.vo.ui_buff_list[buff_key] = buff.buff_id
    end
    if buff.player and buff.player:HasStage(stage_key) then
        buff.player:Play(
            stage_key,
            function()
                if self.vo ~= nil then
                    -- self.vo.buff_list[buff_key] = nil
                    if has_ui_buff then
                        self.vo.ui_buff_list[buff_key] = nil
                    end
                end
                if on_finished then
                    on_finished()
                end
            end
        )
        return true
    else
        if on_finished then
            on_finished()
        end
        return false
    end
end

function BattleCh:PlayAllBuff(stage_key, on_finished)
    local played = false
    if #self.buff_list == 0 then
        if on_finished then
            on_finished()
        end
    end
    for key, buff in pairs(self.buff_list) do
        if self:PlayBuff(key, stage_key, on_finished) then
            played = true
        end
    end
    return played
end

function BattleCh:PlayOneBuff(buff_id, stage_key, on_finished)
    local played = false
    for key, buff in pairs(self.buff_list) do
        if buff_id == buff.buff_id then
            if self:PlayBuff(key, stage_key, on_finished) then
                played = true
            end
        end
    end
    return played
end

function BattleCh:ReplaceBuff(buff) --new_buff_key, old_buff_key)
    local old_buff_key = buff.replaced_buff_key
    local new_buff_key = buff.new_buff_key
    local attr_buff_list
    if self.buff_list[old_buff_key] then
        self.finished_replace[old_buff_key] = new_buff_key
        attr_buff_list =  self.buff_list[old_buff_key].attr_buff_list
    end

    self.buff_list[new_buff_key] = {
        buff_id = buff.new_buff_id,
        buff_level = buff.new_buff_level,
        sustain_num = buff.sustain_num,
        buff_layer = buff.buff_layer,
        attr_buff_list = attr_buff_list
    }
    -- self.buff_list[new_buff_key] = self.buff_list[old_buff_key]
    if self.buff_list[old_buff_key] then
        self.buff_list[new_buff_key].player = self.buff_list[old_buff_key].player
    end
    self.buff_list[old_buff_key] = nil

    -- self.vo.buff_list[new_buff_key] = self.vo.buff_list[old_buff_key]
    self.vo.buff_list[new_buff_key] = buff
    self.vo.buff_list[old_buff_key] = nil

    self.vo.ui_buff_list[new_buff_key] = self.vo.ui_buff_list[old_buff_key]
    self.vo.ui_buff_list[old_buff_key] = nil
end
--这里是石化完成的判断 后面可以改成逻辑提前
function BattleCh:StopBuff(buff_key, on_remove)
    self.vo.buff_list[buff_key] = nil
    local buff = self.buff_list[buff_key]
    if not buff then
        on_remove()
        return
    end
    local basic = BattleData.BuffConfig(buff.buff_id)
    local cfg = BattleData.BuffLevelConfig(buff.buff_id, buff.buff_level)
    if basic and not StringIsEmpty(basic.extra_logic) then
        local logics = string.split(basic.extra_logic, "|")
        self[logics[1]](self, logics[2], logics[3], true)
    end
    if buff.player and buff.player:HasStage(BuffStageKey.End) then
        if buff.player:HasStage(BuffStageKey.Persistence) then
            buff.player:Stop(BuffStageKey.Persistence)
        end
        self:PlayBuff(
            buff_key,
            BuffStageKey.End,
            function()
                GameBuffManager:RemovePlayer(buff.player)
                if on_remove then
                    on_remove()
                end
                if self.body_show and not self.body_show.main_body:Anim().animator.enabled then
                    if cfg and cfg.asset_id == "ShiHua" or cfg.asset_id == "BingFeng" then
                        self:SetAnimActive(true)
                        self:SetEffectAttacherVisible(true)
                        self.is_in_shihua = false
                    end
                end
                if self.vo then --观战可随时退出战斗
                    self.buff_list[buff_key] = nil
                    self.vo.ui_buff_list[buff_key] = nil
                    self.ch_data.flush_buff = not self.ch_data.flush_buff
                    BattleBuffManager.Instance:RemoveAttrBuff(self.vo.grid_idx, buff.attr_buff_list)
                end
            end
        )
    else
        if buff.player then
            GameBuffManager:RemovePlayer(buff.player)
        end
        if on_remove then
            on_remove()
        end
        self.buff_list[buff_key] = nil
        self.vo.ui_buff_list[buff_key] = nil
        BattleBuffManager.Instance:RemoveAttrBuff(self.vo.grid_idx, buff.attr_buff_list)
        self.ch_data.flush_buff = not self.ch_data.flush_buff
    end
end

function BattleCh:ClearBuff()
    for key, buff in pairs(self.buff_list) do
        if buff.player then
            GameBuffManager:RemovePlayer(buff.player, true)
            buff.player.OnRemoveAction = nil
        end
    end
    self.buff_list = {}
    if self.vo ~= nil then
        if self.vo.ui_buff_list then
            if self.vo.ui_buff_list.Clear then
                self.vo.ui_buff_list:Clear()
            else
                self.vo.ui_buff_list = nil
            end
        end
        if self.vo.attr_buff_list then
            if self.vo.attr_buff_list.Clear then
                self.vo.attr_buff_list:Clear()
            else
                self.vo.attr_buff_list = nil
            end
        end
    end
    self.ch_data.flush_buff = not self.ch_data.flush_buff
end

function BattleCh:SetIsShow(is_show)
    self.is_show = is_show
    if self.root_node then
        self.root_node:SetActive(is_show)
    end
    self.vo.dont_show = not is_show
end

function BattleCh:MoveToPos(pos, run_anim, end_forward, move_end, dodge, custom_move_speed)
    if self.root_node == nil then return end
    local move_speed_real = custom_move_speed or move_speed
    local step_dis_real = custom_move_speed and (custom_move_speed * 0.025)  or step_dis
    local start_pos = self.root_node:GetPosition()
    local st_forward = pos - start_pos
    local tot_dis = st_forward:Magnitude()
    if tot_dis <= step_dis_real then --frame pos .set pos and forward
        self:SetPos(pos)
        if end_forward ~= nil then
            self:SetForward(end_forward)
        end
        if move_end ~= nil then
            move_end()
        end
        return
    end
    --[[ if self.ghosting == true then
        self:StopGhost()
        self.ghosting = false
    end ]]
    if dodge then
        self:PlayGhost()
        self.ghosting = true
    end
    local forward = Vector3.Normalize(st_forward)
    local ms = forward * move_speed_real
    if run_anim then
        self:PlayAnim(ChAnimType.BattleRun)
        self:SetForward(st_forward)
    end
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
    end
    self.move_wait =
        Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            if self.is_diauped == true then
                if move_end ~= nil then
                    move_end()
                end
                return true
            end
            if self.repeling then
                return
            end
            local dis = Time.deltaTime * move_speed_real
            tot_dis = tot_dis - dis
            if tot_dis < 0 then --move end
                self.move_wait = nil
                self:SetPos(pos)
                if run_anim or custom_move_speed then
                    self:PlayAnim(ChAnimType.Idle)
                end
                if end_forward ~= nil then
                    self:SetForward(end_forward)
                end
                if self.ghosting then
                    self:StopGhost()
                    self.ghosting = false
                end
                if move_end ~= nil then
                    move_end()
                end
                return true
            else
                local offset = ms * Time.deltaTime
                self:SetPos(self:GetPos() + offset)
                return false
            end
        end
    )
end

local summon_cost = 0.4

--召唤
function BattleCh:Summon(onfinish)
    local start_time = Time.time
    self:setScale(0)
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            local cost_time = Time.time - start_time
            local sc = math.min(cost_time / summon_cost, self.default_scale)
            self:setScale(sc)
            if sc >= self.default_scale then
                if onfinish ~= nil then
                    onfinish()
                end
                return true
            else
                return false
            end
        end
    )
end

--回收(召回)
function BattleCh:Retrieve(onfinish)
    local start_time = Time.time
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            local cost_time = Time.time - start_time
            local sc = math.min(cost_time / summon_cost, 1)
            self:setScale(1 - sc)
            if sc >= 1 then
                if onfinish ~= nil then
                    onfinish()
                end
                return true
            else
                return false
            end
        end
    )
end

local pi = math.pi
--封印
function BattleCh:Seal(is_succ, onfinish)
    -- self.TopPoint:SetLocalPosition(Vector3.New(0, self:GetHeight(), 0))
    self.body_show:SetParent(self.TopPoint)
    local start_time = Time.time
    local scale = 1.1
    local sin_scale = 0.2
    local sin_speed = 6
    local sin_cycle = 2
    local trenshold = 0.5
    local slope = 1
    local time1 = (1 - trenshold) / slope
    --0.5
    local time2 = time1 + sin_cycle * 2 * pi / sin_speed
    --大概是0.5+2.1=2.6
    -- 不知道以后有没有一开始scale就超过1的，先这样
    self:setScale(scale)
    local finished = false
    local cost_time = Time.time - start_time
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            finished = false
            cost_time = Time.time - start_time
            if cost_time < time1 then
                scale = 1 - slope * cost_time
            elseif cost_time < time2 then
                scale = sin_scale * math.sin(sin_speed * (cost_time - time1 - pi)) + trenshold
            else
                if is_succ then
                    if scale > 0 then
                        local s = trenshold - slope * (cost_time - time2)
                        if s < 0 then
                            scale = 0
                            finished = true
                        else
                            scale = s
                        end
                    end
                else
                    if scale < 1 then
                        local s = trenshold + slope * (cost_time - time2)
                        if s > 1 then
                            scale = 1
                            finished = true
                        else
                            scale = s
                        end
                    end
                end
            end
            self:setScaleSeal(scale)
            if finished then
                self.body_show:SetParent(self.root_node)
                if onfinish then
                    onfinish()
                end
                return true
            else
                return false
            end
        end
    )
end
function BattleCh:RecoverSeal()
    if self.body_show and self.root_node then
        self.body_show:SetParent(self.root_node)
    end
end
--摇晃死亡
function BattleCh:Shake(onfinish)
    local frame = 15
    local frequency = 3
    local offset = Vector3.New(0.2, 0.2, 0.2)
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            local pos = self.root_node.transform.localPosition
            if math.floor(frame / frequency) % 2 == 1 then
                pos = pos + offset
            else
                pos = pos - offset
            end
            self.root_node.transform.localPosition = pos
            frame = frame - 1
            if frame < 0 then
                if onfinish then
                    onfinish()
                end
                return true
            end
        end
    )
end

local self_destruct_rot_speed = 540
local self_destruct_cost = 0.7

--自爆
function BattleCh:SelfDestruct(onfinish)
    local start_time = Time.time
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            local cost_time = Time.time - start_time
            local per = math.min(cost_time / self_destruct_cost, 1)
            local sc = per * 0.8 + 1
            self:setScale(sc)
            self.root_node:Rotate(Vector3.up, self_destruct_rot_speed * Time.deltaTime)
            if per >= 1 then
                if onfinish ~= nil then
                    onfinish()
                end
                return true
            else
                return false
            end
        end
    )
end

function BattleCh:setScale(scale, limit)
    limit = limit or 10
    scale = scale > limit and limit or scale
    self.current_scale = scale
    self.v3_scale:Set(scale, scale, scale)
    self.root_node:SetLocalScale(self.v3_scale)--Vector3.New(scale, scale, scale))
end
function BattleCh:setScaleSeal(scale)
    self.v3_scale:Set(scale, scale, scale)
    self.TopPoint:SetLocalScale(self.v3_scale)--Vector3.New(scale, scale, scale))
end
function BattleCh:curScale()
    return self.current_scale
end

function BattleCh:Height()
    return 2.5
end

function BattleCh:GetHeight()
    return self.body_show:GetHeight()
end

--1:左,2:上,3:右,4:下
local function rnd_vp_dir(edge, cam)
    local negative_x, negative_y
    if edge == 1 then
        negative_x = false
    elseif edge == 2 then
        negative_y = true
    elseif edge == 3 then
        negative_x = true
    elseif edge == 4 then
        negative_y = false
    end
    local rnd_an = 120
    local start_angle = -rnd_an / 2
    start_angle = start_angle + (edge - 1) * 90
    local end_angle = start_angle + rnd_an

    local vp_angle = GameMath.FloatRamdon(start_angle, end_angle)
    local tan_angle = math.tan(math.pi * vp_angle / 180)
    local vp_dir = Vector3.New(1, tan_angle, 0)
    if negative_x ~= nil then
        if (negative_x == true and vp_dir.x > 0) or (negative_x == false and vp_dir.x < 0) then
            vp_dir.x = -vp_dir.x
            vp_dir.y = -vp_dir.y
        end
    end
    if negative_y ~= nil then
        if (negative_y == true and vp_dir.y > 0) or (negative_y == false and vp_dir.y < 0) then
            vp_dir.y = -vp_dir.y
            vp_dir.x = -vp_dir.x
        end
    end
    -- Debuger.LogError("vp_dir=====[%s]",tostring(vp_dir))
    local world_vp_dir = cam.transform:TransformDirection(vp_dir)
    local re = world_vp_dir:ProjectOnPlane(Vector3.up):Normalize()
    return re
end

local diaup_speed = 30
local rotate_speed = 1440

function BattleCh:Diaup(on_com)
    if self.vo == nil then
        return
    end
    self.is_diauped = true
    local reach_edge_count = 0
    local cam = SceneMgr:MainCamera()
    local total_reach_edge = GameMath.Ramdon(2)
    --男左女右
    local diaup_dir = rnd_vp_dir(BattleData.IsEnemyIdx(self.vo.grid_idx) and 3 or 1, cam)
    AudioMgr:PlayEffect(AudioType.Battle,"JiNengJiFei")
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            self.root_node:Rotate(Vector3.up, rotate_speed * Time.deltaTime)
            local offset = diaup_dir * diaup_speed * Time.deltaTime
            local target_pos = self:GetPos() + offset
            local vp_pos =
                cam:WorldToViewportPoint(Vector3.New(target_pos.x, target_pos.y + self:Height() / 2, target_pos.z))
            if reach_edge_count >= total_reach_edge then
                self:SetPos(target_pos)
                --judge diaup end
                if (vp_pos.x < -0.3 or vp_pos.x > 1.3 or vp_pos.y < -0.3 or vp_pos.y > 1.3) and not self.is_in_shihua then
                    if on_com ~= nil then
                        self.is_diauped = false
                        on_com()
                    end
                    return true
                end
            else
                local edge
                if vp_pos.x < 0.05 then
                    edge = 1
                elseif vp_pos.x > 0.95 then
                    edge = 3
                elseif vp_pos.y < 0.05 then
                    edge = 4
                elseif vp_pos.y > 0.95 then
                    edge = 2
                end
                if edge ~= nil then --change diaupdir
                    diaup_dir = rnd_vp_dir(edge, cam)
                    reach_edge_count = reach_edge_count + 1
                else
                    self:SetPos(target_pos)
                end
            end
            return false
        end
    )
end
--击退
function BattleCh:Repel(on_com)
    if self.handle_run5 ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run5)
        self.handle_run5 = nil
        if self.repel_on_com then
            self.repel_on_com()
            self.repel_on_com = nil
        end
        if self.repel_original then
            self.root_transform.position = self.repel_original
            self.repel_original = nil
        end
    end
    self.repel_on_com = on_com
    local start_time = Time.time
    local v0_repel = 7
    local a_repel = -70
    local t_repel = v0_repel / -a_repel
    local t_stand = 0.4
    local v_back = 5
    -- 默认可以减到0
    local d_repel = a_repel * t_repel * t_repel / 2 + v0_repel * t_repel
    local t_back = d_repel / v_back

    local trigger_back = false

    local v_repel = v0_repel
    local cur_pos = self:GetPos()
    local p_original = self:getIdxPos(self.vo.grid_idx)
    -- LogError(self.vo.grid_idx,GameMath.GetDistance(cur_pos.x,cur_pos.z,p_original.x,p_original.z,false))
    if GameMath.GetDistance(cur_pos.x, cur_pos.z, p_original.x, p_original.z, false) > 1.2 then
        p_original = cur_pos
    end
    self.repel_original = p_original
    self.handle_run5 =
        Runner.Instance:RunUntilTrue(
        function()
            if self.is_knocked_up or self.root_node == nil then
                return true
            end
            self.repeling = true
            local t = Time.time - start_time
            if t < t_repel then
                -- 击退
                self.root_transform:Translate(Vector3.New(0, 0, -Time.deltaTime * v_repel))
            elseif t < t_repel + t_stand then
                -- 僵直
            elseif t < t_repel + t_stand + t_back then
                -- 返回
                if not trigger_back then
                    trigger_back = true
                end
                self.root_transform:Translate(Vector3.New(0, 0, Time.deltaTime * v_back))
            else
                self.root_transform.position = p_original
                self.repel_original = nil
                if self.repel_on_com then
                    self.repel_on_com()
                    self.repel_on_com = nil
                end
                self.repeling = false
                return true
            end
            v_repel = v_repel + Time.deltaTime * a_repel
        end
    )
end

--普通死亡消失表现
function BattleCh:Disappear(on_com)
    self:PlayBuffAssetStart("DieEff",function ()
        on_com()
    end, nil, true)
end

--查找子孙
function BattleCh:FindChildByName(name)
    if self.body_show == nil then return nil end
    return self.body_show:FindChildByName(name)
end

function BattleCh:CenterPoint()
    if self.center_point == nil then
        self.center_point = self.root_node:FindChild("CenterPoint")
    end
    return self.center_point
end

function BattleCh:clearCenterPointFollow()
    if self.center_point then
        self:CenterPoint():SetLocalPosition(Vector3.New(0,1.25,0))
    end
    if self.center_point_follow then
        self.center_point_follow:SetTarget(nil)
        self.center_point_follow.PositionOffset = Vector3.zero
    end
end

--必须在main_body load完成后刷新
function BattleCh:FlushCenterPointFollow()
    if self.center_point_follow == nil then
        self.center_point_follow = self:CenterPoint():GetComponent(typeof(Game.TransformByTarget))
    end
    --这里body_show获取有误
    local body_show = self.body_show
    if body_show:IsLoading() and self.old_body_show ~= nil then
        body_show = self.old_body_show
    end
    if not body_show:IsLoading() and body_show.main_body:getObj() ~= nil then
        self.center_point_follow:SetTarget(body_show.main_body:getObj().transform)
        if body_show.GetMainBodyHeight and body_show:GetMainBodyHeight() then
            self.center_point_follow.PositionOffset = self.center_point_offset_cache == nil and Vector3.New(0, body_show:GetMainBodyHeight() / 2, 0) or Vector3.zero
        end
    else
        self.center_point_follow:SetTarget(self.root_node.transform)
    end
end

function BattleCh:SkillPlayer()
    return self.body_show:SkillPlayer()
end

function BattleCh:AttackSkillAssetId()
    if self.vo == nil then
        return
    end
    local character_type = self.vo.character_type
    local character_id = self.vo.character_id
    if character_type == BATTLE_CH_TYPE.ROLE then
        local weapon = Config.equipment_auto[self.vo.appearance.weapon_id]
        local weapon_type = weapon and weapon.weapon_type or WEAPON_TYPE.None
        return BattleSkillPool.MainChAttAssId(self.vo.appearance.avatar_type, weapon_type)
    else
        local cfg
        if character_type == BATTLE_CH_TYPE.PET then
            cfg = Config.pet_cfg_auto.pet_info_2[character_id]
        elseif character_type == BATTLE_CH_TYPE.MONSTER then
            cfg = Cfg.MonsterById(character_id, false)
        elseif character_type == BATTLE_CH_TYPE.PARTNER then
            cfg = Config.partner_cfg_auto.partner_info[character_id]
            if cfg.job == 5 then
                return "1002"
            end
        end
        return cfg and cfg.attack_skill_asset or "1000002"
    end
end

function BattleCh:FlushSpecial(sp_id)
    self.special_list[sp_id] = 1
    --没有移除
    self.ch_data.flush_special = not self.ch_data.flush_special
end

function BattleCh.Config(ch_data)
    local character_type = ch_data.character_type
    local character_id = ch_data.character_id
    local config
    if character_type == BATTLE_CH_TYPE.PET or character_type == BATTLE_CH_TYPE.PET_SHADOW then
        config = Cfg.PetById(character_id, false)
        if config then
            return config
        end
    elseif character_type == BATTLE_CH_TYPE.MONSTER then
        config = Cfg.MonsterById(character_id, false)
        if config then
            return config
        end
    elseif character_type == BATTLE_CH_TYPE.PARTNER then
        config = Config.partner_cfg_auto.partner_info[character_id]
        if config then
            return config
        end
    elseif character_type == BATTLE_CH_TYPE.NPC_HELPER then
        config = Config.npc_group_auto.npc_group[character_id]
        if config then
            return config
        end
    end
    -- LogDG("配置缺失，角色类型=", character_type, "id = ", character_id)
    return config
end

function BattleCh:Prepare()
    local fw = self:getIdxForward(self.vo.grid_idx)
    local pos = self:getIdxPos(self.vo.grid_idx)
    self:SetPos(pos)
    self:SetForward(fw)
end
function BattleCh:getIdxForward(idx)
    idx = idx == nil and self.vo.grid_idx or idx
    local scene_data = BattleData.Instance:SceneData()
    return scene_data:GetForward(idx)
end
function BattleCh:getIdxPos(idx, offset)
    idx = idx == nil and self.vo.grid_idx or idx
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetPos(idx, offset)
end
function BattleCh:CmdErrorCheck()
    local battle_data = BattleData.Instance
    local base_data = battle_data:BaseData()
    local move_type = base_data.sel_tar_type
    local state_list = self:BuffState()
    -- 当前操作为普攻时提示 石化，昏睡，诅咒，怯战
    if move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK or move_type == BATTLE_MOVE_TYPE.NONE then
        if state_list[10] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[2][1])
        elseif state_list[7] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[2][2])
        elseif state_list[6] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[2][3])
        elseif state_list[9] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[2][4])
        end
    elseif move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
        -- 7 8 9 10 昏睡 混乱 诅咒 石化
        if state_list[10] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[1][1])
        elseif state_list[7] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[1][2])
        elseif state_list[8] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[1][3])
        elseif state_list[9] then
            PublicPopupCtrl.Instance:Center(Language.Main.CmdErrorTip[1][4])
        end
    end
end
--buff_type 去策划表里查
function BattleCh:BuffState()
    local state_list = {}
    for key, value in pairs(self.buff_list) do
        local basic = BattleData.BuffConfig(value.buff_id)
        if basic and (basic.buff_type == 2 or basic.buff_type == 13) then
            local config = BattleData.BuffLevelConfig(value.buff_id, value.buff_level)
            local params = string.split(config.param_list, ",")
            if params[1] then
                local state = tonumber(params[1])
                state_list[state] = true
            end
        end
    end
    -- LogError("当前状态列表", state_list)
    return state_list
end

function BattleCh:GhostInit(interval, duration)
    self.body_show:MeshGhostInit(interval, duration)
end
function BattleCh:PlayGhost()
    local com = self.body_show:GhostCom()
    if com ~= nil then
        com:Play()
    end
end
function BattleCh:StopGhost()
    local com = self.body_show:GhostCom()
    if com ~= nil then
        xpcall(
            function()
                com:Stop()
            end,
            function(err)
                Debuger.LogError("BattleCh:StopGhost ERROR:" .. tostring(err))
            end
        )
    end
end
function BattleCh:YanHuo(onfinish)
    if self.is_in_shihua then
        self:SetAnimActive(true)
    end
    if onfinish ~= nil then
        onfinish()
    end
end

function BattleCh:AddHalo(sp_id, caster)
    self.special_list[sp_id] = 1
    self.ch_data.flush_special = not self.ch_data.flush_special
    if self.vo.grid_idx ~= caster then
        return
    end
    local cfg = BagData.Instance:GetEquipEffectCfg(sp_id)
    if cfg and cfg.effect_id_1 and cfg.effect_id_1 > 0 then
        self:PlayHalo(
            cfg.effect_id_1,
            function(obj)
                obj:SetParent(self.root_node, false)
                UH.LocalPosG(obj, Vector3.zero)
                self.halo_list = self.halo_list or {}
                self.halo_list[caster] = self.halo_list[caster] or {}
                self.halo_list[caster][sp_id] = self.halo_list[caster][sp_id] or {}
                table.insert(self.halo_list[caster][sp_id], obj)
            end
        )
    else
        -- LogError("光环",sp_id,"特效配置不存在")
    end
end
function BattleCh:RemoveHalo(sp_id, caster)
    self.special_list[sp_id] = nil
    self.ch_data.flush_special = not self.ch_data.flush_special
    if self.halo_list and self.halo_list[caster] and self.halo_list[caster][sp_id] then
        for _, obj in ipairs(self.halo_list[caster][sp_id]) do
            GameObjPool:Release(obj)
        end
    end
end
function BattleCh:PlayHalo(effect_id, func)
    local path = string.format("effect/%s.prefab", effect_id)
    ResMgr:LoadGameObject(
        path,
        function(obj)
            if func then
                func(obj)
            end
        end,
        true,
        false
    )
end

function BattleCh:GetSkin0()
    if self.body_show and self.body_show.main_body then
        return self.body_show.main_body.obj
    end
end

function BattleCh:GetSkin0Animator()
    if self.body_show and self.body_show.main_body then
        return self.body_show.main_body.anim.animator
    end
end

function BattleCh:HideWeapon()
    if self.body_show then
        self.body_show:SetWeapon()
    end
end

function BattleCh:PlayBuffAssetStart(asset_name, on_start_finish, on_remove, dont_remove)
    GameBuffManager:AddPlayer(
        asset_name,
        self.root_node,
        function(player)
            player.OnRemoveAction = function()
                if on_remove then
                    on_remove()
                end
                self:OnBuffRemove(player, asset_name)
            end
            self.effect_buff_player[asset_name] = player
            self:PlayBuffAsset(
                player,
                BuffStageKey.Start,
                function()
                    if on_start_finish then
                        on_start_finish()
                    end
                    if not dont_remove then
                        GameBuffManager:RemovePlayer(player)
                        self.effect_buff_player[asset_name] = nil
                    end
                end
            )
        end)
end
function BattleCh:PlayBuffAsset(player, stage_key, on_finished)
    if player and player:HasStage(stage_key) then
        player:Play(
            stage_key,
            function()
                if on_finished then
                    on_finished()
                end
            end
        )
        return true
    else
        if on_finished then
            on_finished()
        end
        return false
    end
end
--特效没加TimeEvent的特效预设 buff就是持续显示
function BattleCh:PlayBuffAssetStage(asset_name, stage_key, on_finished)
    if self.effect_buff_player[asset_name] == nil then
        self:PlayBuffAssetStart(asset_name, function ()
            self:PlayBuffAsset(self.effect_buff_player[asset_name], stage_key, on_finished)
        end, nil, true)
    else
        self:PlayBuffAsset(self.effect_buff_player[asset_name], stage_key, on_finished)
    end
end
function BattleCh:RemoveBuffAsset(asset_name)
    if self.effect_buff_player[asset_name] then
        GameBuffManager:RemovePlayer(self.effect_buff_player[asset_name])
       self.effect_buff_player[asset_name] = nil
    end
end

function BattleCh:EnableNCA(enabled)
    if self.root_node == nil then return end
    local ncas = self.root_node:GetComponentsInChildren(typeof("NcCurveAnimation"))
    for i = 0, ncas.Length - 1 do
        ncas[i].enabled = enabled
    end
end

--buff恶心逻辑
function BattleCh:ChangeScale(scale, wait, stop)
    if stop == true then
        self:setScale(self.default_scale)
        return
    end
    scale = tonumber(scale)
    local current_scale = self.default_scale or 1.1
    local wait_time = tonumber(wait) or 0
    --需要一个变化过程
    Runner.Instance:RunUntilTrue(function ()
        wait_time = wait_time - Time.deltaTime
        if wait_time <= 0 then
            current_scale = current_scale + 0.05
            self:setScale(current_scale)
            if current_scale >= scale then
                return true
            end
        end
    end)
    -- self:setScale(scale)
end
function BattleCh:TigerWeaponEffect(bool, _, stop)
    if bool == "true" then
        bool = true
    end
    if stop == true then
        bool = false
    end
    if not self.EffectAttachers then
        self.EffectAttachers = self.root_node:GetComponentsInChildren(typeof(Game.EffectAttacher))
    end
    if self.EffectAttachers then
        for i = 0, self.EffectAttachers.Length - 1 do
            if self.EffectAttachers[i] then
                self.EffectAttachers[i]:SetVisible("weapon", bool)
            end
        end
    end
    local effect_weapon = self:FindChildByName("WeaponEffect")
    if effect_weapon then
        effect_weapon.gameObject:SetActive(bool)
    end
end
function BattleCh:HenshinMonkey(res_id, _, stop)
    if stop == true then
        if self.body_show then
           if self.body_show:IsInAnim(ChAnimType.Death) == true then
                self.henshin_complete_func = function()
                    self:WaitAnimEnd(ChAnimType.Death, function () end)
                    self:PlayAnim(ChAnimType.Death)
                end
           end
        end
        local character_type = self.vo.character_type
        if character_type == BATTLE_CH_TYPE.ROLE or character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
            self.vo.appearance.henshin_monkey_res = nil
            self.vo.appearance.henshin_monkey = false  
            self:FlushRoleDrawer()
        else
            self:FlushOtherDrawer()
        end
        return
    end
    local character_type = self.vo.character_type
    if character_type == BATTLE_CH_TYPE.ROLE or character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        self.vo.appearance.henshin_monkey_res = res_id
        self.vo.appearance.henshin_monkey = true
        self:FlushRoleDrawer()
    else
        local res_path = string.format("actor/npc/%s.prefab", res_id)
        self:FlushOtherDrawer(res_path)
    end
end

function BattleCh:SpriteBubble()
    --根据配表配的顺序，显示不同的气泡内容
    if RoleData.Instance:GetRoleLevel() > Config.element_elf_auto.other[1].bubble_level_max then
        return
    end
    local config = Cfg.ElementElfById(self.vo.character_id, false)
    if config and self.vo then
        local bubble_interval = Config.element_elf_auto.other[1].bubble_interval
        local bubble_time = Config.element_elf_auto.other[1].bubble_time
        local descs = string.split(config.bubble_id, "|")
        self:ShowSpriteBubble(1, descs, bubble_time, bubble_time + bubble_interval)
    end
end

function BattleCh:ShowSpriteBubble(index, descs, bubble_time, next_time)
    if BattleBehaviour.displayer and self.vo then
        local cfg = Config.element_elf_auto.bubble[tonumber(descs[index])]
        if cfg == nil then
            return
        end
        BattleBehaviour.displayer:PopHp(self.vo.grid_idx, BATTLE_HP_DESC.BUBBLE, cfg.word, bubble_time)
        TimeHelper:AddDelayTimer(function ()
            index = index + 1
            if index > #descs then
                index = 1
            end
            self:ShowSpriteBubble(index, descs, bubble_time, next_time)
        end, next_time)
    end
end