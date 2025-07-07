BattleBehaviourQueueState = {
    Unplay = 0,
    Playing = 1,
    Played = 2
}

BattleBehaviourQueue = BattleBehaviourQueue or BaseClass()

-- function BattleBehaviourQueue:LogBehaviours()
--     local i = 0
--     for _, v in pairs(self.queue) do
--         i = i + 1
--         Debuger.LogError("behaviour_[%d]=%s", i, type(v) == "table" and Debuger.TableToString(v) or tostring(v))
--     end

--     i = 0
--     for _, v in pairs(self.wait_behs) do
--         i = i + 1
--         Debuger.LogError("wait_beh_[%d]=%s", i, Debuger.TableToString(v))
--     end
-- end

function BattleBehaviourQueue:__init(index)
    self.index = index
    self.queue = {}
    self.state = BattleBehaviourQueueState.Unplay
    self.wait_behs = {}
end

function BattleBehaviourQueue:PushQueueBehaviour(behaviour)
    table.insert(self.queue, false)
    table.insert(self.queue, behaviour)
end

function BattleBehaviourQueue:PushSyncBehaviour(behaviour)
    table.insert(self.queue, behaviour)
end

function BattleBehaviourQueue:PushCutOff()
    table.insert(self.queue, false)
end

function BattleBehaviourQueue:Play()
    if self.state ~= BattleBehaviourQueueState.Unplay then
        return
    end
    --[[ LogError("===================================")
    for index, value in ipairs(self.queue) do
        if value ~= false then
            LogError(GetClassName(value))
            if GetClassName(value) == "BattleSummon" then
               LogError("BattleSummon ", value.summoner_idx)
            elseif GetClassName(value) == "BattleChangePosition" then
                LogError("BattleChangePosition", value.idx1, value.idx2)
            end
        end
    end ]]
    self.state = BattleBehaviourQueueState.Playing
    self:playBehaviour(1)
end

function BattleBehaviourQueue:IsPlaying()
    return self.state == BattleBehaviourQueueState.Playing
end

function BattleBehaviourQueue:IsPlayed()
    return self.state == BattleBehaviourQueueState.Played
end
--修改为当前queue如果出现错误，直接强制停止该queue
function BattleBehaviourQueue:playBehaviour(index)
    if table.nums(self.queue) < index then
        self.state = BattleBehaviourQueueState.Played
        return
    end

    local beh = self.queue[index]
    if beh then
        beh.start_time = Time.time
        table.insert(self.wait_behs, beh)
        beh:Play()
    end
    if beh == false or table.nums(self.queue) == index then ---wait all behaviour
        self.handle_runner = Runner.Instance:RunUntilTrue(
            function()
                if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_NULL then
                    return true
                end
                if BattleData.Instance:BaseData().pause == true then
                    return false
                end
                if BattleData.Instance:SceneData() == nil then
                    return false
                end
                local all_done = true
                for _, v in pairs(self.wait_behs) do
                    if v:Ignored() and #self.queue ~= index then
                        -- LogError("Ignorednnnnnnnnnnnnnnnnnnnnnnnn", GetClassName(v))
                        v.ignored = true
                        all_done = true
                        break
                    else
                        if v:IsDone() == false then
                            --如果卡顿时间超过设定时间10秒那么 保存战报后上传(剧情指引客户端暂停不纳入计算)
                            if not IS_EDITOR and not v:IgnoreTimeoutJudge() and 
                                not GuideManager.Instance:IsGuide() and (Time.time - v.start_time > 15) then
                                BattleSimulator.Instance:UnLoadData()
                                BuglyReport("事件完成超时 ", v._class_name, v.idx, v.skill_id or v.asset_id, v.target_idxs)
                                self:ForceDone()
                            end
                            if IS_EDITOR and not v:IgnoreTimeoutJudge() and 
                                not GuideManager.Instance:IsGuide() and (Time.time - v.start_time > 15) then
                                LogError("事件完成超时 ", v._class_name, v.skill_id, v.idx, v.target_idxs)
                                self:ForceDone()
                            end
                            all_done = false
                            break
                        end
                    end
                end
                if all_done then
                    if #self.queue == index then --play done
                        self.state = BattleBehaviourQueueState.Played
                    else
                        self:playBehaviour(index + 1)
                    end
                end
                return all_done
            end,
            true
        )
    else
        self:playBehaviour(index + 1)
    end
end
function BattleBehaviourQueue:ForceDone()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    --强制结束所有行为
    for _, beh in ipairs(self.queue) do
        if beh and beh:IsDone() == false then
            -- LogError("强制结束行为 ", beh._class_name)
            beh:ForceDone()
        end
    end
    self.state = BattleBehaviourQueueState.Played
end

-- 获取演员名单
function BattleBehaviourQueue:GetActorList()
    if not self.actor_list then
        self.actor_list = {}
        for _, beh in pairs(self.queue) do
            if beh then
                for _, actor in pairs(beh:GetActorList()) do
                    self.actor_list[actor] = true
                end
            end
        end
    end
    return self.actor_list
end
-- 是否在播放队列里
function BattleBehaviourQueue:ActorInQueue(grid_idx)
    if next(self.wait_behs) == nil then
        return false
    end
    for _, v in pairs(self.wait_behs) do
        if v:IsDone() == false then
            for _, actor in pairs(v:GetActorList()) do
                if grid_idx == actor then
                    return true
                end
            end
        end
    end
    return false
end

BattleBehaviour = BattleBehaviour or BaseClass()

BattleBehaviour.displayer = nil

function BattleBehaviour:__init()
    self.battle_data = BattleData.Instance
    self.state = BattleBehaviourQueueState.Unplay
    self.displayer = BattleBehaviour.displayer
    self.actor_list = {}
    self.hit_triggers = {}
    self.binding_behaviors = {}
    self.binding_behavior = nil
end

-- 获取演员名单
function BattleBehaviour:GetActorList()
    return self.actor_list
end

function BattleBehaviour:AddToActorList(...)
    for _, actor in pairs({...}) do
        table.insert(self.actor_list, actor)
    end
end

function BattleBehaviour:SetTrigger(trigger)
    self.trigger = trigger
end

function BattleBehaviour:SetDelayTime(delay_time)
    self.delay_time = delay_time
end
--忽略行为超时判断
function BattleBehaviour:IgnoreTimeoutJudge()
    return false
end

-- 战斗行为
function BattleBehaviour:Play()
    if self.state ~= BattleBehaviourQueueState.Unplay then
        return
    end
    if self.trigger == nil then
        self.state = BattleBehaviourQueueState.Playing
        self:PushWaitForHitTriggers()
        if self.delay_time then
            TimeHelper:AddDelayTimer(
                function()
                    self:playInternal()
                    self:playBinding()
                end,
                self.delay_time
            )
        else
            self:playInternal()
            self:playBinding()
        end
    else
       self.handle_runner = Runner.Instance:RunUntilTrueForQueue(
            function()
                if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_NULL then
                    return true
                end
                local triggered = self.trigger:Triggered()
                -- LogError(GetClassName(self), "is waiting for", self.trigger)
                if triggered then
                    self.handle_runner = nil
                    self.state = BattleBehaviourQueueState.Playing
                    self:PushWaitForHitTriggers()
                    if self.delay_time then
                        TimeHelper:AddDelayTimer(
                            function()
                                self:playInternal()
                                self:playBinding()
                            end,
                            self.delay_time
                        )
                    else
                        self:playInternal()
                        self:playBinding()
                    end
                end
                return triggered
            end
        )
    end
end
--override
function BattleBehaviour:ForceDone()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrueForQueue(self.handle_runner)
        self.handle_runner = nil
    end
    self.state = BattleBehaviourQueueState.Played
    self.is_done = true
end

function BattleBehaviour:IsDone()
    if self.state == BattleBehaviourQueueState.Played then
        return true
    end
    if self.state ~= BattleBehaviourQueueState.Playing then
        return false
    end
    if self:checkDone() == true then
        self.state = BattleBehaviourQueueState.Played
        return true
    else
        return false
    end
end

function BattleBehaviour:AddHitTrigger(hit_trigger)
    table.insert(self.hit_triggers, hit_trigger)
end

function BattleBehaviour:PushWaitForHitTriggers()
    for _, hit_trigger in ipairs(self.hit_triggers) do
        hit_trigger:waitTriggerInternal()
    end
    if self.binding_behavior then
        self.binding_behavior:PushWaitForHitTriggers()
    end
    for index, beh in ipairs(self.binding_behaviors) do
        beh:PushWaitForHitTriggers()
    end
end
--override
function BattleBehaviour:Ignored()
    return false
end
--override
function BattleBehaviour:checkDone()
    return true
end
--override
function BattleBehaviour:playInternal()
end
function BattleBehaviour:playBinding()
    if self.binding_behavior then
        self.binding_behavior:playInternal()
    end
    for index, beh in ipairs(self.binding_behaviors) do
        beh:playInternal()
    end
end
function BattleBehaviour:AddBehavior(behavior)
    table.insert(self.binding_behaviors, behavior)
end
function BattleBehaviour:waitForAsset(wait_ass_id, func)
    if wait_ass_id then
        Runner.Instance:RunUntilTrue(
            function()
                if BattleSkillPool.Instance:CheckAssetRegistered(wait_ass_id) then
                    func()
                    return true
                end
            end,
            true
        )
    else
        func()
    end
end

BattleBehaviourMove = BattleBehaviourMove or BaseClass(BattleBehaviour, "BattleBehaviourMove")

function BattleBehaviourMove:__init(idx, to_idx)
    self.is_done = false
    self.idx = idx
    self.to_idx = to_idx ~= nil and to_idx or self.idx
    self.on_move_end = BindTool.Bind(self.onMoveEnd, self)
    self:AddToActorList(self.idx, self.to_idx)
end

function BattleBehaviourMove:playInternal()
    if self.idx == self.to_idx then
        self.displayer:RestoreIdx(self.idx, true, self.on_move_end)
    elseif self.idx > 100 then--分身
        self.displayer:RunToIdx2(self.idx, self.to_idx, self.on_move_end)
    else
        self.displayer:RunToIdx(self.idx, self.to_idx, self.on_move_end)
    end
end

function BattleBehaviourMove:onMoveEnd()
    self.is_done = true
end

function BattleBehaviourMove:checkDone()
    return self.is_done
end
function BattleBehaviourMove:ForceDone()
    BattleBehaviour.ForceDone(self)
    self.displayer:RestoreIdx(self.idx, true)
end

BattleBehaviourChangeEquipment = BattleBehaviourChangeEquipment or BaseClass(BattleBehaviour, "BattleBehaviourChangeEquipment")

function BattleBehaviourChangeEquipment:__init(idx, before_id, after_id, detail_player)
    self.idx = idx
    self.before_id = before_id
    self.after_id = after_id
    self.detail_player = detail_player
    self:AddToActorList(self.idx)
end

function BattleBehaviourChangeEquipment:playInternal()
    self.displayer:ChangeEquipment(self.idx, self.before_id, self.after_id)
    self.detail_player:Play(BATTLE_DATA_EVENT.CHANGE_EQUIPMENT)
    -- self.battle_data:InsertDetail(content)
end

-- 通用的角色属性改变行为
BattleBehaviourChangeAttribute = BattleBehaviourChangeAttribute or BaseClass(BattleBehaviour, "BattleBehaviourChangeAttribute")

function BattleBehaviourChangeAttribute:__init(idx, attr_key, attr_val)
    self.idx = idx
    self.attr_key = attr_key
    self.attr_val = attr_val
    self:AddToActorList(self.idx)
end

function BattleBehaviourChangeAttribute:playInternal()
    self.displayer:ChangeAttribute(self.idx, self.attr_key, self.attr_val)
end

BattleBehaviourMoveCombineHit = BattleBehaviourMoveCombineHit or BaseClass(BattleBehaviourMove, "BattleBehaviourMoveCombineHit")

function BattleBehaviourMoveCombineHit:__init(idx, to_idx, i, num)
    self.i = i
    self.num = num
end

function BattleBehaviourMoveCombineHit:playInternal()
    local i = self.i
    local num = self.num
    local dis = 1.5
    local max_deg = 35
    local interval = math.min(180 / num, max_deg)
    local rad = math.rad(interval * (i - (num + 1) / 2))
    local offset = Vector3.New(math.sin(rad) * dis, 0, math.cos(rad) * dis)
    self.displayer:RunToIdxWithOffset(self.idx, self.to_idx, offset, self.on_move_end)
end

function BattleBehaviourMoveCombineHit:onMoveEnd()
    self.displayer:LookAt(self.idx, self.to_idx)
    self.is_done = true
end

BattleBehaviourPlayAnim = BattleBehaviourPlayAnim or BaseClass(BattleBehaviour, "BattleBehaviourPlayAnim")

function BattleBehaviourPlayAnim:__init(idx, anim, wait_end, audio)
    -- LogErrorHL("BattleBehaviourPlayAnim", anim)
    self.anim = anim
    self.idx = idx
    self.is_done = false
    if wait_end ~= nil then
        self.wait_end = wait_end
    else
        self.wait_end = true
    end
    self:AddToActorList(self.idx)
    if audio and anim == ChAnimType.Death then
        self.audio_beh = BattleBehaviourAudioPlayer.New(audio["death_" .. GameMath.Ramdon(5)])
    end
    -- if self.anim == ChAnimType.Behit then
    -- 	Debuger.LogErrorSD("BattleBehaviourPlayAnim:__init===idx=%s,anim=%s,wait_end=%s",
    -- 		tostring(self.idx),tostring(self.anim),tostring(self.wait_end))
    -- end
end

function BattleBehaviourPlayAnim:playInternal()
    -- if self.anim == ChAnimType.Behit then
    -- 	Debuger.LogErrorSD("BattleBehaviourPlayAnim:playInternal===idx=%s,anim=%s,wait_end=%s",
    -- 		tostring(self.idx),tostring(self.anim),tostring(self.wait_end))
    -- end
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    local suc = self.displayer:PlayAnim(self.idx, self.anim)
    if self.wait_end and suc == true then
        self.displayer:WaitAnimEnd(
            self.idx,
            self.anim,
            function()
                if self.anim == ChAnimType.Death then
                    self.displayer:ClearWaitDieCheck(self.idx)
                end
                self.is_done = true
            end
        )
    else
        self.is_done = true
    end
end

function BattleBehaviourPlayAnim:checkDone()
    return self.is_done
end

BattleBehaviourPerformAttack = BattleBehaviourPerformAttack or BaseClass(BattleBehaviour, "BattleBehaviourPerformAttack")

function BattleBehaviourPerformAttack:__init(idx, target_idxs, behiter_idxs, audio_cfg)
    self.idx = idx
    self.target_idxs = target_idxs
    self.behiter_idxs = behiter_idxs
    self.is_done = false
    self:AddToActorList(self.idx)
    for _, i in pairs(target_idxs) do
        self:AddToActorList(i)
    end
    self.detail_player = nil
    if audio_cfg then
        self.attack_audio = BattleBehaviourAudioPlayer.New(audio_cfg.attack_1)
        self.voice_audio = BattleBehaviourAudioPlayer.New(audio_cfg["voice_" .. GameMath.Ramdon(5)])
    end
end

function BattleBehaviourPerformAttack:playInternal()
    if self.attack_audio then
        self.attack_audio:playInternal()
        self.attack_audio = nil
    end
    if self.voice_audio then
        self.voice_audio:playInternal()
        self.voice_audio = nil
    end
    local suc =
        self.displayer:PerformAttack(
        self.idx,
        self.target_idxs,
        self.behiter_idxs,
        function()
            self.is_done = true
        end
    )
    if self.detail_player then
        self.detail_player:Play(BATTLE_DATA_EVENT.SKILL_HIT)
    end
end

function BattleBehaviourPerformAttack:checkDone()
    return self.is_done
end

BattleBehaviourPerformSkill = BattleBehaviourPerformSkill or BaseClass(BattleBehaviour, "BattleBehaviourPerformSkill")

function BattleBehaviourPerformSkill:__init(skill_id, idx, target_idxs, behiter_idxs, skill_timing_type, audio_cfg, version)
    self.skill_id = skill_id
    self.idx = idx
    self.target_idxs = target_idxs
    self.behiter_idxs = behiter_idxs
    self.skill_timing_type = skill_timing_type
    self.is_done = false
    self.version = version
    self.wait_ass_id = self.displayer:RegisterSkill(self.skill_id, self.skill_timing_type, version)
    self:AddToActorList(self.idx)
    for _, i in pairs(target_idxs) do
        self:AddToActorList(i)
    end
    if audio_cfg then
        self.audio_beh = BattleBehaviourAudioPlayer.New(audio_cfg["voice_" .. GameMath.Ramdon(5)])
    end
end
function BattleBehaviourPerformSkill:Ignored()
    --拉弓连击技能忽略抬手过程
    return (self.skill_id == 501 or self.skill_id == 507 or self.skill_id == 511) and self:IsDone() and not self.ignored
end
function BattleBehaviourPerformSkill:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    self:waitForAsset(
        self.wait_ass_id,
        function()
            local skillParam =
                self.displayer:PerformSkill(
                self.skill_id,
                self.idx,
                self.target_idxs,
                self.behiter_idxs,
                self.skill_timing_type,
                function()
                    self.displayer:UnregisterSkill(self.skill_id, self.skill_timing_type, self.version)
                    self.is_done = true
                end,
                self.version
            )
            if skillParam then
                if self.IsIgnoreForceComplete then
                    skillParam.IsIgnoreForceComplete = true
                end
            end
        end
    )
end

function BattleBehaviourPerformSkill:checkDone()
    return self.is_done
end

BattleBehaviourPerformSkillByAssetId = BattleBehaviourPerformSkillByAssetId or BaseClass(BattleBehaviour, "BattleBehaviourPerformSkillByAssetId")

function BattleBehaviourPerformSkillByAssetId:__init(asset_id, attacker_idx, target_idxs, behiter_idxs)
    self.asset_id = asset_id
    self.idx = attacker_idx
    self.target_idxs = target_idxs
    self.behiter_idxs = behiter_idxs
    self.is_done = false

    BattleSkillPool.Instance:Register(self.asset_id)
    self:AddToActorList(self.idx)
    for _, i in pairs(target_idxs) do
        self:AddToActorList(i)
    end
end

function BattleBehaviourPerformSkillByAssetId:playInternal()
    self:waitForAsset(
        self.asset_id,
        function()
            self.displayer:PerformSkillByAssetId(
                self.asset_id,
                self.idx,
                self.target_idxs,
                self.behiter_idxs,
                function()
                    BattleSkillPool.Instance:Unregister(self.asset_id)
                    self.is_done = true
                end
            )
        end
    )
end

function BattleBehaviourPerformSkillByAssetId:checkDone()
    return self.is_done
end

BattleBehaviourUseItemSkill = BattleBehaviourUseItemSkill or BaseClass(BattleBehaviour, "BattleBehaviourUseItemSkill")

function BattleBehaviourUseItemSkill:__init(idx, target_idx, item_id)
    self.idx = idx
    self.target_idx = target_idx
    self.is_done = false
    self.item_id = item_id

    self.ch_info = self.battle_data:GetChInfo(self.idx)
    self.target_info = self.battle_data:GetChInfo(self.target_idx)
    self:AddToActorList(self.idx)
    self:AddToActorList(self.target_idx)

    self.asset_id = BattleSkillPool.ConstAssetId.UseItem
    local config = Item.GetConfig(self.item_id)
    if config == nil and self.item_id ~= 0 then
        BuglyReport("config is nil ", self.item_id)
    end
    local use_type = config and config.use_type or ItemUseType.HPMedicine
    if use_type == ItemUseType.MPMedicine then
        self.asset_id = BattleSkillPool.ConstAssetId.UseMp
    elseif config and config.param2 == 0 and config.param3 > 0 then
        self.asset_id = BattleSkillPool.ConstAssetId.UseUnseal
    end
    BattleSkillPool.Instance:Register(self.asset_id)
end

function BattleBehaviourUseItemSkill:playInternal()
    -- BattleDetail
    -- local content = string.format(Language.BattleDetail[15], self.ch_info.name, self.target_info.name, Item.Create({item_id = self.item_id}):QuaName())
    -- self.battle_data:InsertDetail(content)

    local suc =
        self.displayer:PerformUseItemSkill(
        self.asset_id,
        self.idx,
        self.target_idx,
        -- self.item_id,
        function()
            self.is_done = true
        end
    )
end

function BattleBehaviourUseItemSkill:checkDone()
    return self.is_done
end

BattleBehaviourBeHit = BattleBehaviourBeHit or BaseClass(BattleBehaviour, "BattleBehaviourBeHit")

function BattleBehaviourBeHit:__init(idx, atker_idx, defence, beat_back, audio, ignore)
    self.idx = idx
    self.atker_idx = atker_idx
    self.defence = defence
    self.beat_back = beat_back
    if ignore ~= true then
        self:AddToActorList(self.idx, self.atker_idx)
    end
    self.is_done = false
    if audio then
        self.audio_beh = BattleBehaviourAudioPlayer.New(audio.hit)
    end
    BattleSkillPool.Instance:Register(BattleSkillPool.ConstAssetId.Defence)
end

function BattleBehaviourBeHit:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    self.displayer:Behit(
        self.idx,
        self.atker_idx,
        self.defence,
        self.beat_back,
        function()
            self.is_done = true
        end
    )
end

function BattleBehaviourBeHit:checkDone()
    if self.need_wait then
        return self.is_done
    end
    return true
end

BattleBehaviourRunAway = BattleBehaviourRunAway or BaseClass(BattleBehaviour, "BattleBehaviourRunAway")

function BattleBehaviourRunAway:__init(idx, suc)
    self.idx = idx
    self.success = suc
    self.is_done = false
    self.pet_idx = BattleData.Instance:GetPetIdxByPlayerIdx(self.idx)
    self:AddToActorList(self.idx, self.pet_idx)
end

function BattleBehaviourRunAway:playInternal()
    -- BattleData.Instance:SetChInfo(self.idx, nil)
    if false == BattleData.Instance:IsChAlive(self.idx) then
        if self.success then
            BattleData.Instance:SetChInfo(self.idx, nil)
            if self.idx == BattleData.Instance:PlayerIdx() then
                BattleCtrl.Instance:RoundPlayFinish()
            end
            self.is_done = true
            return
        end
    end
    self.displayer:RunAway(
        self.idx,
        self.success,
        function()
            if self.success then
                BattleData.Instance:SetChInfo(self.idx, nil)
                if self.idx == BattleData.Instance:PlayerIdx() then
                    BattleCtrl.Instance:RoundPlayFinish()
                end
            end
            self.is_done = true
        end
    )
end

function BattleBehaviourRunAway:checkDone()
    return self.is_done
end
function BattleBehaviourRunAway:ForceDone()
    BattleBehaviour.ForceDone(self)
    if self.success then
        BattleData.Instance:SetChInfo(self.idx, nil)
        if self.idx == BattleData.Instance:PlayerIdx() then
            BattleCtrl.Instance:RoundPlayFinish()
        end
    end
end

BattleHpChange = BattleHpChange or BaseClass(BattleBehaviour, "BattleHpChange")

function BattleHpChange:__init(idx, change_hp, cur_hp, desc, injure_num, behit_desc, atker, detail_player, shield_val, frozen_shield_val)
    self.idx = idx
    self.change_hp = change_hp
    self.cur_hp = cur_hp
    self.desc = desc
    self.injure_num = injure_num
    self.behit_desc = behit_desc
    self.atker = atker
    self.detail_player = detail_player
    self.shield_val = shield_val
    self.frozen_shield_val = frozen_shield_val
    -- LogErrorHL("atker", atker)
    self:AddToActorList(self.idx)
end

function BattleHpChange:playInternal()
    -- LogError("BattleHpChange", Time.frameCount)
    if
        self.change_hp ~= 0 or self.desc == BATTLE_HP_DESC.DAMAGE_ABSORB_HP or self.desc == BATTLE_HP_DESC.IN_VAIN or
            self.desc == BATTLE_HP_DESC.ATTACK_BACK or self.desc == BATTLE_HP_DESC.REAL_DAMAGE or self.desc == BATTLE_HP_DESC.DESPERATE
        or self.behit_desc == BATTLE_BE_HIT_DESC.BLOCK_1
    then
        self.displayer:PopHp(self.idx, self.desc, self.change_hp, self.injure_num, self.behit_desc, self.atker)
        if self.desc ~= BATTLE_HP_DESC.ADD_HP and self.cur_hp ~= 0
        and self.desc ~= BATTLE_HP_DESC.DAMAGE_ABSORB_HP then
            self.displayer:PlayAllBuff(self.idx, BuffStageKey.HPChange)
        end
    end
    self.detail_player:Play(BATTLE_DATA_EVENT.MODIFY_HP)

    if self.cur_hp >= 0 or self.shield_val ~= nil or self.frozen_shield_val ~= nil then --cur_hp < 0时 连击总血量表现，不设置当前血量
        local ch_info = BattleData.Instance:GetChInfo(self.idx)
        if ch_info ~= nil then
            -- LogDG(self.idx, self.cur_hp)
            if self.cur_hp  >= 0 then
                ch_info.curhp = self.cur_hp
            end
            if self.shield_val ~= nil and self.desc == BATTLE_HP_DESC.SHIELD then
                ch_info.hp_shield = self.shield_val
            end
            if self.frozen_shield_val ~= nil then
                ch_info.frozen_shield = self.frozen_shield_val
            end
        end
        if self.idx == BattleData.Instance:PlayerPetIdx() then --player pet be retrieve
            BattleData.Instance:SetPetStatus(BATTLE_PET_STATUS.Zhan)
            local pet_list = BattleData.Instance:PetList()
            for k, v in pairs(pet_list) do
                if v.status == BATTLE_PET_STATUS.Zhan then
                    if self.cur_hp  >= 0 then
                        v.curhp = self.cur_hp
                    end
                    if self.shield_val ~= nil then
                        v.hp_shield = self.shield_val
                    end
                    if self.frozen_shield_val ~= nil then
                        v.frozen_shield = self.frozen_shield_val
                    end
                    break
                end
            end
        end
    end
end
function BattleHpChange:ForceDone()
    BattleBehaviour.ForceDone(self)
    self:playInternal()
end

BattleMpChange = BattleMpChange or BaseClass(BattleBehaviour, "BattleMpChange")

function BattleMpChange:__init(idx, change_mp, cur_mp, modify_desc, detail_player)
    -- LogError("BattleMpChange", modify_desc)
    self.idx = idx
    self.change_mp = change_mp
    self.cur_mp = cur_mp
    self.modify_desc = modify_desc
    self.is_done = self.modify_desc ~= 1
    self.detail_player = detail_player
    self:AddToActorList(self.idx)
end

function BattleMpChange:playInternal()
    local ch_info = BattleData.Instance:GetChInfo(self.idx)
    -- Debuger.LogError("Set Mp===From==%d,To===%d",ch_info.curmp,self.cur_mp)
    self.detail_player:Play(BATTLE_DATA_EVENT.MODIFY_MP)
    if ch_info ~= nil then
        ch_info.curmp = self.cur_mp
    end
    if self.modify_desc ~= 2 and self.modify_desc ~= 5
    and self.modify_desc ~= 0 then
        self.displayer:PopHp(self.idx, BATTLE_HP_DESC.MP_DECREASE, self.change_mp)
    end
    if self.modify_desc == 1 then
        TimeHelper:AddDelayTimer(function()
            self.is_done = true
        end, 1)
    end
    if self.idx == BattleData.Instance:PlayerPetIdx() then --player pet be retrieve
        local pet_list = BattleData.Instance:PetList()
        for _, v in pairs(pet_list) do
            if v.status == BATTLE_PET_STATUS.Zhan then
                v.curmp = self.cur_mp
                break
            end
        end
    end
end

function BattleMpChange:checkDone()
    return self.is_done
end
------------------------------
BattleSpiritChange = BattleSpiritChange or BaseClass(BattleBehaviour, "BattleSpiritChange")
function BattleSpiritChange:__init(idx, change_spirit, cur_spirit, modify_desc, detail_player)
    self.idx = idx
    self.change_spirit = change_spirit
    self.cur_spirit = cur_spirit
    self.modify_desc = modify_desc
    self.detail_player = detail_player
    self.is_done = self.modify_desc ~= BATTLE_SPIRIT_POINT_CHANGE_REASON.CHANGE_REASON_ROUND_ADD
    -- LogError("SpiritChange")
    self:AddToActorList(self.idx)
end

function BattleSpiritChange:playInternal()
    -- LogError("spiritchange playInternal")
    local ch_info = BattleData.Instance:GetChInfo(self.idx)
    self.detail_player:Play(BATTLE_DATA_EVENT.SPIRIT_POINT_CHANGE)
    if ch_info ~= nil then
        ch_info.spirit_point = self.cur_spirit
    end
    if self.modify_desc == BATTLE_SPIRIT_POINT_CHANGE_REASON.CHANGE_REASON_ROUND_ADD then
        TimeHelper:AddDelayTimer(function ()
            self.is_done = true
        end,1)
    end
end

function BattleSpiritChange:checkDone()
    return self.is_done
end
------------------------------
BattleBuff = BattleBuff or BaseClass(BattleBehaviour, "BattleBuff")

function BattleBuff:__init(idx, data, event)
    self.idx = idx
    self.data = data
    self.event = event
    self:AddToActorList(self.idx)
    self.is_done = false
end

function BattleBuff:playInternal()
    if self.event == BATTLE_DATA_EVENT.ADD_BUFF_TO_LIST then
        self.displayer:PopHp(self.idx, BATTLE_HP_DESC.BUFF, self.data.new_buff_id)
        self.displayer:AddBuff(self.idx, self.data)
        -- if self.data.new_buff_id == 352 and self.idx == BattleData.Instance:PlayerIdx() then
            -- local info = BattleData.BuffLevelConfig(self.data.new_buff_id, self.data.new_buff_level)
            -- if info then
            --     BattleData.Instance:FabaoCD(info.sustain_num)
            -- end
        -- end
        self.is_done = true
    elseif self.event == BATTLE_DATA_EVENT.PLAY_ALL_BUFF then
        -- if self.data.fallback then
        --     self.using_fallback = true
        --     self.is_done = self.data.fallback.is_done
        --     self.data.fallback:playInternal()
        -- end
        local played =
            self.displayer:PlayAllBuff(
            self.idx,
            self.data.stage_key,
            function()
                self.is_done = true
                if self.data.trigger then
                    self.data.trigger:trigger()
                end
            end
        )
        if not played and self.data.fallback then
            self.using_fallback = true
            self.is_done = self.data.fallback.is_done
            self.data.fallback:playInternal()
        end
    elseif self.event == BATTLE_DATA_EVENT.PLAY_ONE_BUFF then
        self.displayer:PlayOneBuff(
            self.idx,
            self.data.buff_id,
            self.data.stage_key,
            function()
                self.is_done = true
                if self.data.trigger then
                    self.data.trigger:trigger()
                end
            end
        )
    elseif self.event == BATTLE_DATA_EVENT.REMOVE_BUFF then
        self.displayer:PopHp(self.idx, BATTLE_HP_DESC.BUFF_REMOVE, self.data.removed_buff_key)
        self.displayer:StopBuff(
            self.idx,
            self.data.removed_buff_key,
            function()
                self.is_done = true
            end
        )
    elseif self.event == BATTLE_DATA_EVENT.REPLACE_BUFF then
        self.displayer:ReplaceBuff(self.idx, self.data)--self.data.new_buff_key, self.data.replaced_buff_key)
        self.displayer:ChangeSustainNum(self.idx, self.data)
        self.displayer:PopHp(self.idx, BATTLE_HP_DESC.BUFF_REPLACE, self.data.new_buff_id)
        self.is_done = true
    elseif self.event == BATTLE_DATA_EVENT.MERGE_BUFF then
        self.displayer:ChangeSustainNum(self.idx, self.data)
        self.displayer:PopHp(self.idx, BATTLE_HP_DESC.BUFF_MERGE, self.data.new_buff_id)
        self.is_done = true
    elseif self.event == BATTLE_DATA_EVENT.BUFF_SUSTAIN_NUM_CHANG then
        self.displayer:ChangeSustainNum(self.idx, self.data)
        self.is_done = true
    end
    -- Debuger.LogErrorSD("BattleBuff===idx=%d,buff_id=%d",self.idx,self.buff_id)
end

function BattleBuff:checkDone()
    if self.using_fallback then
        return self.data.fallback.is_done
    else
        return self.is_done
    end
end
BattleSpecial = BattleSpecial or BaseClass(BattleBehaviour, "BattleSpecial")
function BattleSpecial:__init(idx, sp_id, detail_player)
    self.idx = idx
    self.sp_id = sp_id
    self.detail_player = detail_player
end
function BattleSpecial:playInternal()
    self.displayer:FlushSpecial(self.idx, self.sp_id)
    if self.detail_player then
        --部分合击技能没显示
        self.detail_player:Play(BATTLE_DATA_EVENT.SKILL_SPECIAL_EFFECT)
    end
end

BattlePopup = BattlePopup or BaseClass(BattleBehaviour, "BattlePopup")

function BattlePopup:__init(idx, type, num, detail_player)
    self.idx = idx
    self.type = type
    self.num = num
    self.detail_player = detail_player
    self:AddToActorList(self.idx)
end

function BattlePopup:playInternal()
    self.displayer:PopHp(self.idx, self.type, self.num, self.num2)
    if self.detail_player then
        self.detail_player:Play(BATTLE_DATA_EVENT.SKILL_HIT)
    end
end

BattleDodge = BattleDodge or BaseClass(BattleBehaviour, "BattleDodge")

function BattleDodge:__init(idx, atker_idx, combo_idx, detail_player)
    self.idx = idx
    self.atker_idx = atker_idx
    self.is_done = false
    self.detail_player = detail_player
    self:AddToActorList(self.idx, self.atker_idx)
    self.combo_idx = combo_idx
end

--override
function BattleDodge:playInternal()
    self.displayer:Dodge(
        self.idx,
        self.atker_idx,
        self.combo_idx,
        function()
            self.is_done = true
        end
    )
    self.displayer:PopHp(self.idx, BATTLE_HP_DESC.DODGE)
    self.detail_player:Play(BATTLE_DATA_EVENT.DODGE)
end

function BattleDodge:checkDone()
    return self.is_done
end

BattleMove = BattleMove or BaseClass(BattleBehaviour, "BattleMove")

--forward:0 no end forward,1:face,2:idx tran same
function BattleMove:__init(idx, to_idx, run_anim, offset, forward)
    self.idx = idx
    self.to_idx = to_idx ~= nil and to_idx or self.idx
    self.run_anim = run_anim ~= nil and run_anim or false
    self.offset = offset ~= nil and offset or nil
    self.forward = forward ~= nil and forward or nil
    self.is_done = false
    self:AddToActorList(self.idx, self.to_idx)
end

--override
function BattleMove:playInternal()
    self.displayer:MoveToIdx(
        self.idx,
        self.to_idx,
        self.run_anim,
        self.offset,
        self.forward,
        function()
            self.is_done = true
        end
    )
end

function BattleMove:checkDone()
    return self.is_done
end

-- 处理一些公用的死亡事件
BattleDieHandle = BattleDieHandle or BaseClass(BattleBehaviour, "BattleDieHandle")

function BattleDieHandle:__init(idx, die_type)
    self.idx = idx
    self.die_type = die_type
    self:AddToActorList(self.idx)
end

--override
function BattleDieHandle:playInternal()
    local pet_idx = BattleData.Instance:PlayerPetIdx()
    if self.idx == pet_idx and self.die_type ~= 3 then
        BattleData.Instance:SetPetStatus(BATTLE_PET_STATUS.Wang)
    end

    local ch_info = BattleData.Instance:GetChInfo(self.idx)
    if ch_info and ch_info.strategy then
        local temp = {}
        for k, v in pairs(ch_info.strategy) do
            if v == BATTLE_STRATEGY.REALIVE then
                temp[1] = temp
                break
            end
        end
        ch_info.strategy = temp
    end
end

BattleDiaup = BattleDiaup or BaseClass(BattleBehaviour, "BattleDiaup")

function BattleDiaup:__init(idx, audio)
    -- self.type = "Diaup"
    self.idx = idx
    self.pet_idx = BattleData.Instance:GetPetIdxByPlayerIdx(self.idx)
    self:AddToActorList(self.idx, self.pet_idx)
    if audio then
        self.audio_beh = BattleBehaviourAudioPlayer.New(audio.hit)
        self.audio_dia = BattleBehaviourAudioPlayer.New(audio["death_" .. GameMath.Ramdon(5)])
    end
    self.is_done = false
end

--override
function BattleDiaup:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    if self.audio_dia then
        self.audio_dia:playInternal()
        self.audio_dia = nil
    end
    local battle_data = BattleData.Instance
    local ch_info = battle_data:GetChInfo(self.idx)
    battle_data:SetChInfo(self.idx, nil)
    
    if self.idx == battle_data:PlayerIdx() then
        local pet_idx = battle_data:PlayerPetIdx()
        battle_data:SetChInfo(pet_idx, nil)
        self.displayer:ShakeAndDie(pet_idx)
        self.displayer:Diaup(self.idx, self.dont_play_behit)
        self.is_done = true
    else
        --假如是怪物且在杂项配置表里有配置,且是BOSS 就显示一次宝箱
        local is_show_boss_box = false
        if ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
            local cfg = Cfg.MonsterById(ch_info.character_id)
            if cfg.is_boss == 1 then
                is_show_boss_box = battle_data:CheckIsCanShowBox()
            end
        end
        if is_show_boss_box == true then
            self.displayer:Diaup(self.idx, self.dont_play_behit)
            self.displayer:CreateBox(self.idx, function ()
                self.displayer:ClearRewardBox()
            end)
            self.is_done = true
        else
            if Cfg and ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
                local info = Cfg.ElementElfById(ch_info.character_id, false)
                if info then
                    self.displayer:CreateEleBox(self.idx, info.npc_id)
                end
            end
            self.displayer:Diaup(self.idx, self.dont_play_behit)
            self.is_done = true
        end
    end
end

function BattleDiaup:checkDone()
    return self.is_done
end
function BattleDiaup:ForceDone()
    BattleBehaviour.ForceDone(self)
    local battle_data = BattleData.Instance
    battle_data:SetChInfo(self.idx, nil)
    if self.idx == battle_data:PlayerIdx() then
        local pet_idx = battle_data:PlayerPetIdx()
        battle_data:SetChInfo(pet_idx, nil)
        self.displayer:Diaup(pet_idx)
        self.displayer:Diaup(self.idx, true)
    else
        self.displayer:Diaup(self.idx, true)
    end
    self.displayer:ClearRewardBox()
end
BattleHallowDiaup = BattleHallowDiaup or BaseClass(BattleBehaviour, "BattleHallowDiaup")
function BattleHallowDiaup:__init(idx, eve_data)
    self.idx = idx
    self:AddToActorList(self.idx)
end
function BattleHallowDiaup:playInternal()
    local battle_data = BattleData.Instance
    battle_data:SetSQInfo(self.idx, nil)
    self.displayer:Diaup(self.idx, true)
end

BattleDieDisappear = BattleDieDisappear or BaseClass(BattleBehaviour, "BattleDieDisappear")

function BattleDieDisappear:__init(idx, audio)
    self.idx = idx
    self:AddToActorList(self.idx)
    if audio then
        self.audio_beh = BattleBehaviourAudioPlayer.New(audio.hit)
        self.audio_die = BattleBehaviourAudioPlayer.New(audio["death_" .. GameMath.Ramdon(5)])
    end
    self.is_done = false
end

--override
function BattleDieDisappear:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    if self.audio_die then
        self.audio_die:playInternal()
        self.audio_die = nil
    end
    local battle_data = BattleData.Instance
    local ch_info = battle_data:GetChInfo(self.idx)
    battle_data:SetChInfo(self.idx, nil)
    --假如是怪物且在杂项配置表里有配置,且是BOSS 就显示一次宝箱
    local is_show_boss_box = false
    if ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
        local cfg = Cfg.MonsterById(ch_info.character_id)
        if cfg.is_boss == 1 then
            is_show_boss_box = battle_data:CheckIsCanShowBox()
        end
    end
    if is_show_boss_box then
        self.displayer:Disappear(self.idx, function ()
            self.displayer:CreateBox(self.idx, function ()
                self.displayer:ClearRewardBox()
            end)
        end)
        self.is_done = true
    else
        if Cfg and ch_info and ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
            local info = Cfg.ElementElfById(ch_info.character_id, false)
            if info then
                self.displayer:CreateEleBox(self.idx, info.npc_id)
            end
        end
        self.displayer:Disappear(self.idx)
        self.is_done = true
    end
end
function BattleDieDisappear:checkDone()
    return self.is_done
end
function BattleDieDisappear:ForceDone()
    BattleBehaviour.ForceDone(self)
    local battle_data = BattleData.Instance
    battle_data:SetChInfo(self.idx, nil)
    self.displayer:Disappear(self.idx)
    self.displayer:ClearRewardBox()
end

BattleChangePosition = BattleChangePosition or BaseClass(BattleBehaviour, "BattleChangePosition")

function BattleChangePosition:__init(idx, detail_player)
    self.idx1 = idx
    self.idx2 = BattleData.IsFrontIdx(idx) and (idx - 5) or (idx + 5)
    self.detail_player = detail_player

    self.move_end1 = false
    self.move_end2 = false
    self:AddToActorList(self.idx1, self.idx2)
end

function BattleChangePosition:playInternal()
    local battle_data = BattleData.Instance
    local ch_info1 = battle_data:GetChInfo(self.idx1)
    local ch_info2 = battle_data:GetChInfo(self.idx2)

    self.detail_player:Play(BATTLE_DATA_EVENT.CHANGE_POSITION)
    --存在死亡情况
    if ch_info1 ~= nil then
        ch_info1.grid_idx = self.idx2
    end
    if ch_info2 ~= nil then
        ch_info2.grid_idx = self.idx1
    end
    --[[ ch_info1.grid_idx = self.idx2
    ch_info2.grid_idx = self.idx1 ]]
    battle_data:SetChInfo(self.idx2, ch_info1, true)
    battle_data:SetChInfo(self.idx1, ch_info2, true)

    self.displayer:SwapIdx(self.idx1, self.idx2)
    if ch_info1 ~= nil then
        self.displayer:RestoreIdx(
            self.idx2,
            false,
            function()
                self.move_end1 = true
            end
        )
    else
        self.move_end1 = true
    end

    if ch_info2 ~= nil then
        self.displayer:RestoreIdx(
            self.idx1,
            false,
            function()
                self.move_end2 = true
            end
        )
    else
        self.move_end2 = true
    end
end

function BattleChangePosition:checkDone()
    return self.move_end1 and self.move_end2
end

BattleRetrieve = BattleRetrieve or BaseClass(BattleBehaviour, "BattleRetrieve")

function BattleRetrieve:__init(retriever_idx, idx, detail_player)
    self.idx = idx
    self.retriever_idx = retriever_idx
    self.is_done = false
    self.detail_player = detail_player
    self:AddToActorList(self.idx)
    self:AddToActorList(self.retriever_idx)
    BattleSkillPool.Instance:Register(BattleSkillPool.ConstAssetId.Retrieve)
end

--override
function BattleRetrieve:playInternal()
    if self.idx == BattleData.Instance:PlayerPetIdx() then
        BattleData.Instance:SetPetStatus(BATTLE_PET_STATUS.Dai)
    end
    self.detail_player:Play(BATTLE_DATA_EVENT.RETRIEVE_PET)

    self.displayer:PerformRetrieveSkill(
        self.retriever_idx,
        self.idx,
        function()
            local battle_data = BattleData.Instance
            battle_data:SetChInfo(self.idx, nil)
            self.displayer:Retrieve(self.idx)
            if self.idx == battle_data:PlayerPetIdx() then --player pet be retrieve
                local pet_list = battle_data:PetList()
                for _, v in pairs(pet_list) do
                    if v.status == BATTLE_PET_STATUS.Zhan then
                        v.status = BATTLE_PET_STATUS.Dai
                        break
                    end
                end
            end
            self.is_done = true
        end
    )
end

function BattleRetrieve:checkDone()
    return self.is_done
end

BattleSummon = BattleSummon or BaseClass(BattleBehaviour, "BattleSummon")

function BattleSummon:__init(ch_info, summoner_idx, pet_idx_in_list, summon_reason, detail_player, summer_nil, sys_play)
    self.ch_info = ch_info
    self.summoner_idx = summoner_idx
    self.is_done = false
    self.pet_idx_in_list = pet_idx_in_list
    self.summon_reason = summon_reason
    self.summoner_info = self.battle_data:GetChInfo(self.summoner_idx)
    self.detail_player = detail_player
    self.summer_nil = summer_nil
    self.sys_play = sys_play
    self.pet_summer = false
    self.real_pos = ch_info.grid_idx
    -- if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT and summoner_idx <= 0 then--临时改一下，处理初始战斗召唤时不会执行其它队列，之后考虑用通用数据逻辑解决
    --     for i=1,20 do
    --         self:AddToActorList(i)
    --     end
    -- else
        self:AddToActorList(self.ch_info.grid_idx)
        self:AddToActorList(self.summoner_idx)
    -- end
    if self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_YOU_MING then
        self.summon_skil = BattleSkillPool.ConstAssetId.YouMingSummon
    else
        self.summon_skil = BattleSkillPool.ConstAssetId.Summon
    end
    BattleSkillPool.Instance:Register(self.summon_skil)
end

--override
function BattleSummon:playInternal()
    local idx = self.ch_info.grid_idx
    local battle_data = BattleData.Instance
    if idx ~= self.real_pos and self.pet_summer then
        idx = self.real_pos
        self.ch_info.grid_idx = idx
    end
    -- LogDG(idx, self.real_pos, self.pet_summer)
    battle_data:SetChInfo(idx, self.ch_info)
    self.detail_player:Play(BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER)

    self.summon_ch =
        self.displayer:CreateCh(
        idx,
        battle_data:GetChInfo(idx),
        function()
            if self.summoner_idx > 0 or self.sys_play == true then
                if self.summer_nil == true and self.sys_play ~= true then
                    self.summon_ch:SetIsShow(true)
                    self.displayer:Summon(
                        idx,
                        function()
                            self.summon_ch:SetIsShow(true)
                            self.is_done = true
                        end
                    )
                    if self.pet_summer == false then
                        self.is_done = true
                    end
                else
                    self.displayer:PerformSummonSkill3(
                        self.summon_skil,
                        self.summoner_idx,
                        idx,
                        function()
                            self.summon_ch:SetIsShow(true)
                            self.displayer:Summon(
                                idx,
                                function()
                                    self.summon_ch:SetIsShow(true)
                                end
                            )
                            self.is_done = true
                        end
                    )
                end
            else
                self.summon_ch:SetIsShow(true)
                if BattleSimulator.Instance:IsPlaying() then
                    self.is_done = true
                    return
                end
                if
                    (BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE and
                        (self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_ROLE_REINFORCE or
                            self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_PET_REINFORCE)) or
                        self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_MONSTER_REINFORCE
                 then
                    --在移动完结束否则会出现 无血条情况
                    self.displayer:Reinforce(
                        idx,
                        function()
                            self.is_done = true
                        end
                    )
                else
                    self.is_done = true
                end
            end
        end,
        true
    )
    self.summon_ch:SetIsShow(false)
    if (self.summoner_idx == BattleData.Instance:PlayerIdx() or self.summoner_idx == BattleData.Instance:PlayerPetIdx()) and self.pet_idx_in_list >= 0 then
        BattleData.Instance:SetPetStatus(BATTLE_PET_STATUS.Zhan, self.pet_idx_in_list + 1)
    end
end
function BattleSummon:checkDone()
    return self.is_done
end

BattleSummonMutiple = BattleSummonMutiple or BaseClass(BattleBehaviour, "BattleSummonMutiple")
function BattleSummonMutiple:__init(ch_info_list, summoner_idx, summon_reason)
    self.ch_info_list = ch_info_list
    self.summoner_idx = summoner_idx
    self.summon_reason = summon_reason
    self.summoner_info = self.battle_data:GetChInfo(self.summoner_idx)
    for _, value in ipairs(self.ch_info_list) do
        if value.data and value.data.character_info then
            self:AddToActorList(value.data.character_info.grid_idx)
        end
    end
    self:AddToActorList(self.summoner_idx)
    self.is_done = false
end

function BattleSummonMutiple:playInternal()
    local battle_data = BattleData.Instance
    self.summon_ch_list = {}
    self.target_list = {}
    local idx
    local end_index = 0--table.nums(self.ch_info_list)
    for index, value in ipairs(self.ch_info_list) do
        if value.data and value.data.character_info then
            end_index = end_index + 1
        end
    end
    local index = 0
    for _, value in ipairs(self.ch_info_list) do
        if value.data and value.data.character_info then
            index = index + 1
            idx = value.data.character_info.grid_idx
            table.insert(self.target_list, idx)
            battle_data:SetChInfo(idx, value.data.character_info)
            self.summon_ch_list[idx] =
                self.displayer:CreateCh(
                idx,
                battle_data:GetChInfo(idx),
                function(grid_idx)
                    if self.summoner_idx <= 0 then
                        self.summon_ch_list[grid_idx]:SetIsShow(true)
                        if BattleSimulator.Instance:IsPlaying() then
                            if end_index == index then
                                self.is_done = true
                            end
                            return
                        end
                        if
                            (BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE and
                                (self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_ROLE_REINFORCE or
                                    self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_PET_REINFORCE)) or
                                self.summon_reason == BATTLE_SUMMON_REASON.BATTLE_SUMMON_REASON_MONSTER_REINFORCE
                        then
                            self.displayer:Reinforce(
                                grid_idx,
                                function()
                                    if end_index == index then
                                        self.is_done = true
                                    end
                                end
                            )
                        else
                            self.displayer:FixedPos(grid_idx)
                            if end_index == index then
                                self.is_done = true
                            end
                        end
                    end
                end,
                true,
                self.summoner_idx <= 0
            )
            self.summon_ch_list[idx]:SetIsShow(false)
        end
    end
    if self.summoner_idx > 0 then
        self.displayer:PerformSummonSkill2(
            self.summoner_idx,
            self.target_list,
            function()
                for _, grid_idx in ipairs(self.target_list) do
                    self.summon_ch_list[grid_idx]:SetIsShow(true)
                    self.displayer:Summon(
                        grid_idx,
                        function()
                            self.summon_ch_list[grid_idx]:SetIsShow(true)
                        end
                    )
                end
                self.is_done = true
            end
        )
    end
end
function BattleSummonMutiple:checkDone()
    return self.is_done
end

BattleSeal = BattleSeal or BaseClass(BattleBehaviour, "BattleSeal")

local PetSealCardAsset = {
    [26078] = "FengYinTong",
    [26079] = "FengYinYin",
    [26080] = "FengYinJin",
    [56625] = "FengYinJin",
    [56626] = "FengYinJin",
}

function BattleSeal:__init(caster_idx, target_idx, is_succ, card_item_id, detail_player)
    self.caster_idx = caster_idx
    self.target_idx = target_idx
    self.is_succ = is_succ
    self.card_item_id = card_item_id
    self.is_done = false
    self.detail_player = detail_player
    self.asset_id = PetSealCardAsset[card_item_id]
    BattleSkillPool.Instance:Register(self.asset_id)
    self:AddToActorList(self.caster_idx, self.target_idx)
end

--override
function BattleSeal:playInternal()
    -- self.displayer:PresetHit(self.caster_idx, self.target_idx)
    self.displayer:WaitBehit(
        self.caster_idx,
        self.target_idx,
        function()
            local ch_info = BattleData.Instance:GetChInfo(self.target_idx)
            if ch_info ~= nil then
                ch_info.dont_show = true
            end

            self.displayer:Seal(
                self.target_idx,
                self.is_succ == 1,
                PetSealCard[self.card_item_id],
                function()
                    self.detail_player:Play(BATTLE_DATA_EVENT.SEAL_PET)
                    self.displayer:PopHp(BattleData.Instance:PlayerIdx(), BATTLE_HP_DESC.SEAL_PET, self.is_succ)
                    self.is_done = true
                    if self.caster_idx == BattleData.Instance:PlayerIdx() then
                        PublicPopupCtrl.Instance:Center(ErrorText[self.is_succ == 1 and 95 or 96])
                        if self.is_succ == 1 then
                            FightCtrl.Instance:CheckAutoSealAccomplish()
                        end
                    end
                    if self.is_succ == 0 then
                        local ch_info = BattleData.Instance:GetChInfo(self.target_idx)
                        if ch_info ~= nil then
                            ch_info.dont_show = false
                        end
                    else
                        -- ChatCtrl.Instance:ChannelChatCustom {seal_get = {name = ch_info.name}}
                    end
                    BattleCtrl.Instance:DelayUnlock(BATTLE_DELAY_REASON.SEAL_PET)
                end
            )
        end
    )
    self:waitForAsset(
        self.asset_id,
        function()
            self.displayer:PerformSkillByAssetId(self.asset_id, self.caster_idx, {self.target_idx}, {self.target_idx})
        end
    )
end
function BattleSeal:ForceDone()
    BattleBehaviour.ForceDone(self)
    if self.is_succ == 0 then
        self.displayer:RecoverSeal(self.target_idx)
    end
end
function BattleSeal:checkDone()
    return self.is_done
end

BattleUseFabao = BattleUseFabao or BaseClass(BattleBehaviour, "BattleUseFabao")

function BattleUseFabao:__init(caster_idx, eve_data, target_list)
    self.caster_idx = caster_idx
    self.target_idx = eve_data.target_grid_idx
    self.skill_type = eve_data.fabao_skill_type
    self.is_done = false
    self:AddToActorList(self.caster_idx, self.target_idx)
    self.asset_id, self.default = FabaoData.Instance:GetSkillAsset(self.skill_type)
    -- if self.default == false then
        BattleSkillPool.Instance:Register(self.asset_id)
    -- end
    self.target_list = target_list
end

--override
function BattleUseFabao:playInternal()
    -- if self.caster_idx == BattleData.Instance:PlayerIdx() then
    --     BattleData.Instance:FabaoCD(2)
    -- end
    self:waitForAsset(
        self.asset_id,
        function()
            if self.target_list then
                self.displayer:PerformUseFabaoSkill2(
                self.asset_id,
                self.caster_idx,
                self.target_list,
                function()
                    self.is_done = true
                    if self.default == false then
                        BattleSkillPool.Instance:Unregister(self.asset_id)
                    end
                end
            )
            else
            self.displayer:PerformUseFabaoSkill(
                self.asset_id,
                self.caster_idx,
                self.target_idx,
                function()
                    self.is_done = true
                    if self.default == false then
                        BattleSkillPool.Instance:Unregister(self.asset_id)
                    end
                end
            )
            end
        end
    )
end

function BattleUseFabao:checkDone()
    return self.is_done
end

BattleSkillHit = BattleSkillHit or BaseClass(BattleBehaviour, "BattleSkillHit")

function BattleSkillHit:__init(event_list, log, behiter_idxs)
    self.event_list = event_list
    self.log = log
    self.behiter_idxs = behiter_idxs
end

--override
function BattleSkillHit:playInternal()
end

BattleSelfDestruct = BattleSelfDestruct or BaseClass(BattleBehaviour, "BattleSelfDestruct")

function BattleSelfDestruct:__init(idx, target_idxs, data)
    self.idx = idx
    self.target_idxs = target_idxs
    self.is_done = false
    self:AddToActorList(self.idx)
    for _, index in ipairs(self.target_idxs) do
        self:AddToActorList(index)
    end
    self.asset_name = BattleHelper.ZiBao
    if data.param1 > 0 then
        local config = Cfg.SpecialEffect(data.param1)
        if config.effect_id_1 == BattleHelper.YanHuo or config.effect_id_1 == BattleHelper.JueDiFanJi then
            self.asset_name = config.effect_id_1
        end
    end
    BattleSkillPool.Instance:Register(self.asset_name)
end

--override
function BattleSelfDestruct:playInternal()
    if self.asset_name == BattleHelper.YanHuo or self.asset_name == BattleHelper.JueDiFanJi then
        self.displayer:YanHuo(self.idx,function (on_finish)
            self:playSkill(on_finish) 
        end)
        return
    end
    self.displayer:SelfDestruct(
        self.idx,
        function(on_finish)
            self:playSkill(on_finish)
        end
    )
end
function BattleSelfDestruct:playSkill(on_finish)
    self:waitForAsset(self.asset_name, function ()
        self.displayer:PerformSkillByAssetId(
        self.asset_name,
        self.idx,
        self.target_idxs,
        self.target_idxs,
        function()
            -- BattleSkillPool.Instance:Unregister(self.asset_name)
            if on_finish then
                on_finish()
            end
            if self.asset_name ~= BattleHelper.YanHuo and self.asset_name ~= BattleHelper.JueDiFanJi then
                BattleData.Instance:SetChInfo(self.idx, nil)
            end
            self.is_done = true
        end)
    end)
end
function BattleSelfDestruct:ForceDone()
    BattleBehaviour.ForceDone(self)
    BattleData.Instance:SetChInfo(self.idx, nil)
end
function BattleSelfDestruct:checkDone()
    return self.is_done
end

BattleTotalDamage = BattleTotalDamage or BaseClass(BattleBehaviour, "BattleTotalDamage")

function BattleTotalDamage:__init(total_damage)
    self.total_damage = total_damage
end

function BattleTotalDamage:playInternal()
    local total_damage = 0
    for _, v in ipairs(self.total_damage) do
        total_damage = total_damage + v
    end
    BattleData.Instance:BaseData().total_damage = total_damage
end

BattleBehaviourTalking = BattleBehaviourTalking or BaseClass(BattleBehaviour, "BattleBehaviourTalking")
function BattleBehaviourTalking:__init(talk_id)
    self.talk_id = talk_id
    self.is_done = false
end
function BattleBehaviourTalking:playInternal()
    BehaviorData.Instance.auto_behavior = 12
    BattleData.Instance:BaseData().sel_tar_type = BATTLE_MOVE_TYPE.NONE
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    NpcDialogCtrl.Instance:NpcDialogView(
        {talk_id = self.talk_id},
        function()
            self.is_done = true
            BehaviorData.Instance.auto_behavior = 0
        end
    )
end
function BattleBehaviourTalking:ForceDone()
    BattleBehaviour.ForceDone(self)
    ViewMgr:CloseView(NpcDialogView)
end
function BattleBehaviourTalking:checkDone()
    return self.is_done
end

function BattleBehaviourTalking:IgnoreTimeoutJudge()
    return true
end
BattleBehaviourXTZaDi = BattleBehaviourXTZaDi or BaseClass(BattleBehaviour, "BattleBehaviourXTZaDi")
function BattleBehaviourXTZaDi:__init()
    self.is_done = false
    self:AddToActorList(13)
    BattleSkillPool.Instance:Register("XTZaDi")
end
function BattleBehaviourXTZaDi:playInternal()
    self.displayer:PerformSkillByAssetId(
                "XTZaDi",
                13,
                {},
                {},
                function()
                    BattleSkillPool.Instance:Unregister("XTZaDi")
                    self.is_done = true
                    SE:Play(SceneEffectConfig.EffectList.LuoShiEffect)
                    BattleSkillPool.Instance:ShowShakeCamera()
                end
            )
end
function BattleBehaviourXTZaDi:checkDone()
    return self.is_done
end
function BattleBehaviourXTZaDi:ForceDone()
    BattleBehaviour.ForceDone(self)
    BattleSkillPool.Instance:Unregister("XTZaDi")
    SE:Play(SceneEffectConfig.EffectList.LuoShiEffect)
    BattleSkillPool.Instance:ShowShakeCamera()
end

BattleBehaviourShotEffect = BattleBehaviourShotEffect or BaseClass(BattleBehaviour, "BattleBehaviourShotEffect")
function BattleBehaviourShotEffect:__init(shot_id)
    self.shot_id = shot_id
end
function BattleBehaviourShotEffect:playInternal()
    self.is_done = true
end

function BattleBehaviourShotEffect:checkDone()
    return self.is_done
end

BattleBehaviourClientGuide = BattleBehaviourClientGuide or BaseClass(BattleBehaviour, "BattleBehaviourClientGuide")
function BattleBehaviourClientGuide:__init(guide_id)
    self.guide_id = guide_id
    self.is_done = false
end
function BattleBehaviourClientGuide:playInternal()
    BattleCtrl.Instance:SendAutoBattle(false)
    GuideManager.Instance:Start(self.guide_id)
    self.shot_effect =
        BattleShotEffect.New(
        2,
        function()
            self.is_done = true
            self.shot_effect:DeleteMe()
            self.shot_effect = nil
        end
    )
    --监听指引结束
    Runner.Instance:RunUntilTrue(
        function()
            if not GuideManager.Instance:IsGuide() then
                BattleCtrl.Instance:SendAutoBattle(true)
                BattleData.Instance:BaseData().sel_tar_type = BATTLE_MOVE_TYPE.NONE
                --开始一段TL之后再结束
                self.shot_effect:Play()
                return true
            end
            return not GuideManager.Instance:IsGuide()
        end
    )
end

function BattleBehaviourClientGuide:checkDone()
    return self.is_done
end

BattleBehaviourMonsterTransform = BattleBehaviourMonsterTransform or BaseClass(BattleBehaviour, "BattleBehaviourMonsterTransform")
function BattleBehaviourMonsterTransform:__init(tr_idx, info)
    self.info = info
    self.idx = tr_idx
    self.id_done = false
    self:AddToActorList(self.idx)
end
function BattleBehaviourMonsterTransform:playInternal()
    self.displayer:Transform(
        self.idx,
        self.info,
        function()
            self.is_done = true
        end
    )
end
function BattleBehaviourMonsterTransform:checkDone()
    return self.is_done
end
function BattleBehaviourMonsterTransform:ForceDone()
    BattleBehaviour.ForceDone(self)
    self.displayer:ForceTransformEnd()
end
--------------播放音效-----------------
BattleBehaviourAudioPlayer = BattleBehaviourAudioPlayer or BaseClass(BattleBehaviour, "BattleBehaviourAudioPlayer")
function BattleBehaviourAudioPlayer:__init(eff_name)
    self.eff_name = eff_name
end
function BattleBehaviourAudioPlayer:playInternal()
    -- LogError("play audio ",self.eff_name)
    if tostring(self.eff_name) == "0" then
        return
    end
    AudioMgr:PlayEffect(AudioType.Battle, self.eff_name)
end

-------------体型变化-------------------
BattleBehaviourChangeScale = BattleBehaviourChangeScale or BaseClass(BattleBehaviour, "BattleBehaviourChangeScale")
function BattleBehaviourChangeScale:__init(idx, size, limit)
    self.idx = idx
    self.size = size
    self.limit = limit
    self:AddToActorList(self.idx)
end
function BattleBehaviourChangeScale:playInternal()
    self.displayer:SetScale(self.idx, self.size, self.limit)
end
------------BUFF未生效或者免疫------------------
BattleBehaviourBuffMiss = BattleBehaviourBuffMiss or BaseClass(BattleBehaviour, "BattleBehaviourBuffMiss")
function BattleBehaviourBuffMiss:__init(idx, miss_reason)
    self.idx = idx
    self.miss_reason = miss_reason
    self:AddToActorList(self.idx)
end
function BattleBehaviourBuffMiss:playInternal()
    self.displayer:PopHp(self.idx, BUFF_MISS_REASON[self.miss_reason])
end

BattleChVanish = BattleChVanish or BaseClass(BattleBehaviour, "BattleChVanish")
function BattleChVanish:__init(idx)
    self.idx = idx
    self:AddToActorList(self.idx)
end
function BattleChVanish:playInternal()
    self.displayer:Vanish(self.idx)
end

BattleRagePointChange = BattleRagePointChange or BaseClass(BattleBehaviour, "BattleRagePointChange")
function BattleRagePointChange:__init(idx, data)
    self.idx = idx
    self.data = data
    self:AddToActorList(self.idx)
    self.is_done = false
end
function BattleRagePointChange:playInternal()
    local ch_info = BattleData.Instance:GetChInfo(self.idx)
    if ch_info then
        ch_info.rage = self.data.cur_num
    end
    TimeHelper:AddDelayTimer(
        function()
            self.is_done = true
        end,
        0.3
    )
end
function BattleRagePointChange:checkDone()
    return self.is_done
end
BattleSkillFailure = BattleSkillFailure or BaseClass(BattleBehaviour, "BattleSkillFailure")
function BattleSkillFailure:__init(idx, data)
    self.idx = idx
    self.data = data
    self:AddToActorList(self.idx)
end
function BattleSkillFailure:playInternal()
    local skill_id = self.data.skill_id
    local skill_cfg = SkillData.GetConfig(skill_id)
    if skill_cfg then
        PublicPopupCtrl.Instance:Center(string.format(Language.Main.SkillFail2, skill_cfg.skill_name))
    else
        PublicPopupCtrl.Instance:Center(Language.Main.SkillFail)
    end
end
BattleNoticeNum = BattleNoticeNum or BaseClass(BattleBehaviour, "BattleNoticeNum")
function BattleNoticeNum:__init(idx, num)
    self.idx = idx
    self.notice_num = num
    self:AddToActorList(self.idx)
end
function BattleNoticeNum:playInternal()
    PublicPopupCtrl.Instance:Center(ErrorInfo[self.notice_num] or "ErrorCode No Generate " .. self.notice_num)
end
BattleClipStandBy = BattleClipStandBy or BaseClass(BattleBehaviour, "BattleClipStandBy")
function BattleClipStandBy:__init(clipName)
    self.clipName = clipName
    self.is_done = false
end
function BattleClipStandBy:playInternal()
    ClipGeter.GetClip(self.clipName)
    TimeHelper:AddDelayTimer(
        function()
            self.is_done = true
        end,
        1
    )
end
function BattleClipStandBy:checkDone()
    return self.is_done
end

BattleSummonSpecial = BattleSummonSpecial or BaseClass(BattleBehaviour, "BattleSummonSpecial")

function BattleSummonSpecial:__init(ch_info, detail_player)
    self.is_done = false
    self.ch_info = ch_info
    self.detail_player = detail_player
end
function BattleSummonSpecial:playInternal()
    local idx = self.ch_info.grid_idx
    local battle_data = BattleData.Instance
    battle_data:SetChInfo(idx, self.ch_info)
    self.detail_player:Play(BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER)
    --召唤模型然后播出场
    self.summon_ch =
        self.displayer:SummonCh(
        idx,
        battle_data:GetChInfo(idx),
        function()
            local vir_cam = BattleSkillPool.Instance:GetVirtualCamera()
            local temp
            local len = vir_cam.m_Lens
            temp = len.FieldOfView
            len.FieldOfView = 34
            vir_cam.m_Lens = len
            --在角色脚底播特效
            SE:Play(SceneEffectConfig.EffectList.GGInitEffect, nil, nil, self.summon_ch:GetRoot())
            self.displayer:CameraAnim("gg_chuchang_cam",function ()
                self.summon_ch:PlayAnim(ChAnimType.Idle)
                temp = (temp ~= 22 and 22 or temp)
                len.FieldOfView = temp
                vir_cam.m_Lens = len
                self.is_done = true
            end)
            self.summon_ch:SetForward(self.summon_ch:getIdxForward())
            self.summon_ch:SetPos(self.summon_ch:getIdxPos())
            self.summon_ch:SetIsShow(true)
            self.summon_ch:PlayAnim(ChAnimType.BianShen)
            AudioMgr:PlayEffect(AudioType.Battle, "GongGongChuChang")
        end,
        true
    )
    self.summon_ch:SetPos(Vector3.New(-1000,0,0))
end
function BattleSummonSpecial:checkDone()
    return self.is_done
end
function BattleSummonSpecial:ForceDone()
    BattleBehaviour.ForceDone(self)
    --检查召唤对象是否存在不存在再召唤一个
    local ch = self.displayer:getCh(self.ch_info.grid_idx)
    if not ch then
        self.displayer:CreateCh(self.ch_info.grid_idx, self.ch_info)
    end
    self.displayer:ForceStopCameraAnim()
end
BattleSideHpShow = BattleSideHpShow or BaseClass(BattleBehaviour, "BattleSideHpShow")
function BattleSideHpShow:__init(data)
    self.data = data
end
function BattleSideHpShow:playInternal()
    local battle_data = BattleData.Instance
    local ch_info
    for i = 1, 20  do
        if BattleData.IsEnemyIdx(i) then
            ch_info = nil
            ch_info = battle_data:GetChInfo(i)
            if ch_info then
                ch_info.show_hp = self.data.is_show
            end
        end
    end
end

BattlePlayEffectOnCh = BattlePlayEffectOnCh or BaseClass(BattleBehaviour, "BattlePlayEffectOnCh")
function BattlePlayEffectOnCh:__init(data)
    self.data = data
    self.idx = self.data.grid_idx
    self:AddToActorList(self.idx)
    self.auto_release = self.data.auto_release or false
end
function BattlePlayEffectOnCh:playInternal()
    self.displayer.effonch_handles[self.idx] = self.displayer.effonch_handles[self.idx] or {}
    if self.displayer.effonch_handles[self.idx][self.data.sign] == nil then
        self.displayer.effonch_handles[self.idx][self.data.sign] = self.displayer:PlayEffectOnCh(self.idx, self.data.config, true)
        if self.auto_release then
            self.displayer.effonch_handles[self.idx][self.data.sign] = nil
        end
    end
end

BattleStopEffectOnCh = BattleStopEffectOnCh or BaseClass(BattleBehaviour, "BattleStopEffectOnCh")
function BattleStopEffectOnCh:__init(data)
    self.data = data
    self.idx = self.data.grid_idx
    self:AddToActorList(self.idx)
end
function BattleStopEffectOnCh:playInternal()
    if self.displayer.effonch_handles[self.idx] then
        if self.displayer.effonch_handles[self.idx][self.data.sign] then
            self.displayer:StopEffectOnCh(self.displayer.effonch_handles[self.idx][self.data.sign])
            self.displayer.effonch_handles[self.idx][self.data.sign] = nil
        end
    end
end

BattleAddHalo = BattleAddHalo or BaseClass(BattleBehaviour, "BattleAddHalo")
function BattleAddHalo:__init(caster, target, halo_id)
    self.caster = caster
    self.target = target
    self.halo_id = halo_id
end
function BattleAddHalo:playInternal()
    self.displayer:AddHalo(self.target, self.caster, self.halo_id)
end
BattleRemoveHalo = BattleRemoveHalo or BaseClass(BattleBehaviour, "BattleRemoveHalo")
function BattleRemoveHalo:__init(caster, target, halo_id)
    self.caster = caster
    self.target = target
    self.halo_id = halo_id
end
function BattleRemoveHalo:playInternal()
    self.displayer:RemoveHalo(self.target, self.caster, self.halo_id)
end

BattlePlayBuffAssetStart = BattlePlayBuffAssetStart or BaseClass(BattleBehaviour, "BattlePlayBuffAssetStart")
function BattlePlayBuffAssetStart:__init(idx, asset_name)
    self.idx = idx
    self.asset_name = asset_name
end
function BattlePlayBuffAssetStart:playInternal()
    --播放start特效，自动回收
    self.displayer:PlayBuffAssetStart(self.idx, self.asset_name)
end
BattleLinkTogether = BattleLinkTogether or BaseClass(BattleBehaviour)
function BattleLinkTogether:__init(idx, data)
    self.idx = idx
    self.data = data
end
function BattleLinkTogether:playInternal()
    --播放特效然后控制特效上的LineRenderer
    self.displayer:PlayLink(self.idx, self.data)
end
BattleLinkBreak = BattleLinkBreak or BaseClass(BattleBehaviour)
function BattleLinkBreak:__init(idx, data)
    self.idx = idx
    self.data = data
end
function BattleLinkBreak:playInternal()
    --播放特效然后控制特效上的LineRenderer
    self.displayer:LinkBreak(self.idx, self.data)
end

BattlePlayBuffAssetStage = BattlePlayBuffAssetStage or BaseClass(BattleBehaviour, "BattlePlayBuffAssetStage")
function BattlePlayBuffAssetStage:__init(idx, asset_name, stage_key, on_finished)
    self.idx = idx
    self.asset_name = asset_name
    self.stage_key = stage_key
    self.on_finished = on_finished
end
function BattlePlayBuffAssetStage:playInternal()
    self.displayer:PlayBuffAssetStage(self.idx, self.asset_name, self.stage_key, self.on_finished)
end

BattleRemoveBuffAsset = BattleRemoveBuffAsset or BaseClass(BattleBehaviour, "BattleRemoveBuffAsset")
function BattleRemoveBuffAsset:__init(idx, asset_name)
    self.idx = idx
    self.asset_name = asset_name
end
function BattleRemoveBuffAsset:playInternal()
    self.displayer:RemoveBuffAsset(self.idx, self.asset_name)
end

BattleCreateFenShen = BattleCreateFenShen or BaseClass(BattleBehaviour, "BattleCreateFenShen")
function BattleCreateFenShen:__init(idx,copy_idx)
    self.idx = idx
    self.copy_idx = copy_idx
end
function BattleCreateFenShen:playInternal()
    local info = BattleData.Instance:GetChInfo(self.copy_idx)
    info = DataHelper.TableCopy(info)
    info.grid_idx = self.idx
    if info.character_type == BATTLE_CH_TYPE.PET or info.character_type == BATTLE_CH_TYPE.PET_SHADOW then
        local config = Cfg.PetById(info.character_id, false)
        if config and config.fenshen > 0 then
            info.character_id = config.fenshen
        end
    end
    BattleCtrl.Instance:GetStream():CreateCh(self.idx, info, function ()
        -- self.displayer:SetShow(self.idx, false)
        self.displayer:FixedPos(self.idx, self.copy_idx)
        self.is_done = true
    end)
end
function BattleCreateFenShen:checkDone()
    return self.is_done
end
BattleDeleteBattleCh = BattleDeleteBattleCh or BaseClass(BattleBehaviour, "BattleDeleteBattleCh")
function BattleDeleteBattleCh:__init(idx)
    self.idx = idx
end
function BattleDeleteBattleCh:playInternal()
   self.displayer:DeleteCh(self.idx)
end

BattleShengQiSkillShow = BattleShengQiSkillShow or BaseClass(BattleBehaviour, "BattleShengQiSkillShow")
function BattleShengQiSkillShow:__init(data)
    self.data = data
end
function BattleShengQiSkillShow:playInternal()
    BattleData.Instance:SetSQSkill(self.data)
    BattleData.Instance:BaseData().sheng_qi_show = true
end
BattleBehaviourHallowSkillMove = BattleBehaviourHallowSkillMove or BaseClass(BattleBehaviour, "BattleBehaviourHallowSkillMove")
function BattleBehaviourHallowSkillMove:__init(idx, eve_data)
    self.idx = idx
    self.target_idxs = eve_data.target_list
    self:AddToActorList(self.idx)
    for _, i in pairs(self.target_idxs) do
        self:AddToActorList(i)
    end
    self.is_done = false
end
function BattleBehaviourHallowSkillMove:playInternal()
    self.displayer:MoveToCenter(self.idx, self.idx, function ()
        self.is_done = true
    end)
end
function BattleBehaviourHallowSkillMove:checkDone()
    return self.is_done
end

BattleBehaviourPerformHallowSkill = BattleBehaviourPerformHallowSkill or BaseClass(BattleBehaviour, "BattleBehaviourPerformHallowSkill")

function BattleBehaviourPerformHallowSkill:__init(idx, eve_data)
    self.idx = idx
    self.skill_id = eve_data.hallow_skill_id
    self.skill_level = eve_data.hallow_skill_lv
    self.target_idxs = eve_data.target_list
    self.behiter_idxs = eve_data.target_list
    self.config = Cfg.ShengQiSkillCfg(self.skill_id, self.skill_level)
    self.asset_id = self.config.asset_id_2
    self.displayer:RegisterSkillAsset(self.asset_id)
    self:AddToActorList(self.idx)
    for _, i in pairs(self.target_idxs) do
        self:AddToActorList(i)
    end
end

function BattleBehaviourPerformHallowSkill:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    if StringIsEmpty(self.asset_id) then
        self.is_done = true
        return
    end
    self:waitForAsset(self.asset_id, function()
        LogDG("释放圣器技能", self.asset_id)
        local skillParam = self.displayer:PerformHallowSkill(self.idx, self.asset_id, self.target_idxs,
            self.behiter_idxs, function()
                self.displayer:UnregisterSkillAsset(self.asset_id)
                self.is_done = true
            end)
        if skillParam then
            skillParam.SkillId = self.skill_id
        end
    end)
end

function BattleBehaviourPerformHallowSkill:checkDone()
    return self.is_done
end
BattleBehaviourHallowIntervene = BattleBehaviourHallowIntervene or BaseClass(BattleBehaviour, "BattleBehaviourHallowIntervene")

function BattleBehaviourHallowIntervene:__init(idx, info)
    self.idx = idx
    self.info = info
    self:AddToActorList(self.idx)
end
function BattleBehaviourHallowIntervene:playInternal()
    BattleData.Instance:SetSQInfo(self.idx, self.info)
    self.displayer:CreateSV(self.idx, self.info, function ()
        self.is_done = true
    end)
end
function BattleBehaviourHallowIntervene:checkDone()
    return self.is_done
end

BattleBehaviourHallowActivate = BattleBehaviourHallowActivate or BaseClass(BattleBehaviour, "BattleBehaviourHallowActivate")
function BattleBehaviourHallowActivate:__init(data)
    self.data = data
    for i, v in ipairs(self.data.hallow_list) do
        self:AddToActorList(v.grid_idx)
    end
    self.is_done = false
end
function BattleBehaviourHallowActivate:playInternal()
    for i, v in ipairs(self.data.hallow_list) do
        self.displayer:StatusChange(v.grid_idx, v.status)
    end
    TimeHelper:AddDelayTimer(function ()
        self.is_done = true
    end,1)
end
function BattleBehaviourHallowActivate:checkDone()
    return self.is_done
end
--------------Triggers-----------------

BattleBehaviourTrigger = BattleBehaviourTrigger or BaseClass()

function BattleBehaviourTrigger:__init()
    self.displayer = BattleBehaviour.displayer
    self.state = 0 ---0:disable,1:wait trigger,2:trggered
end

function BattleBehaviourTrigger:Triggered()
    return self.state == 2
end

function BattleBehaviourTrigger:WaitTrigger()
    -- if self.state == 0 then
    --     self.state = 1
    --     self:waitTriggerInternal()
    -- end
end

--override
function BattleBehaviourTrigger:waitTriggerInternal()
    self:trigger()
end

--child call
function BattleBehaviourTrigger:trigger()
    -- if self.state ~= 1 then
    --     return
    -- end
    self.state = 2
end
--被击触发器
BattleBehitTrigger = BattleBehitTrigger or BaseClass(BattleBehaviourTrigger)

function BattleBehitTrigger:__init(hit_idx, behit_idx)
    self.hit_idx = hit_idx
    self.behit_idx = behit_idx
    self.pre_set = false
end

function BattleBehitTrigger:waitTriggerInternal()
    if not self.pre_set then
        self.pre_set = true
        -- self.displayer:PresetHit(self.hit_idx, self.behit_idx)

        -- Debuger.LogError("BattleBehitTrigger:waitTriggerInternal!!!!!!!!!---hit===%d,behit===%d",self.hit_idx,self.behit_idx)
        self.displayer:WaitBehit(
            self.hit_idx,
            self.behit_idx,
            function()
                -- LogError("BattleBehitTrigger", Time.frameCount, self.hit_idx, self.behit_idx)
                -- if BattleData.Instance:PlayerIdx() == self.hit_idx then
                --     BattleData.Instance:AddBattleEffect(BattleEffectKey.Hit)
                -- end
                -- Debuger.LogError("BattleBehitTrigger:waitTriggerInternal:HitTrigger!!!!!!!!!---hit===%d,behit===%d",self.hit_idx,self.behit_idx)
                self:trigger()
            end
        )
    end
end



-- 神龙释放技能
BattleBehaviourPerformDragonSkill = BattleBehaviourPerformDragonSkill or BaseClass(BattleBehaviour, "BattleBehaviourPerformDragonSkill")

function BattleBehaviourPerformDragonSkill:__init(idx, eve_data, battle_detail)
    self.idx = idx
    self.skill_id = eve_data.skill_id
    self.target_idxs = eve_data.target_list
    self.behiter_idxs = eve_data.target_list
    self.config = Cfg.FairyDragonBattleSkillCfg(self.skill_id)
    self.asset_id = self.config.resources
    self.displayer:RegisterSkillAsset(self.asset_id)
    self.battle_detail = battle_detail
    self:AddToActorList(self.idx)
    for _, i in pairs(self.target_idxs) do
        self:AddToActorList(i)
    end
end

function BattleBehaviourPerformDragonSkill:playInternal()
    if self.audio_beh then
        self.audio_beh:playInternal()
        self.audio_beh = nil
    end
    if StringIsEmpty(self.asset_id) then
        self.is_done = true
        return
    end
    self.battle_detail:Play(BATTLE_DATA_EVENT.DRAGON_SKILL_PERFORM)
    self:waitForAsset(self.asset_id, function()
        LogDG("释放神龙技能", self.asset_id)
        local skillParam = self.displayer:PerformHallowSkill(self.idx, self.asset_id, self.target_idxs,
            self.behiter_idxs, function()
                self.displayer:UnregisterSkillAsset(self.asset_id)
                self.is_done = true
            end)
        if skillParam then
            skillParam.SkillId = self.skill_id
        end
    end)
end

function BattleBehaviourPerformDragonSkill:checkDone()
    return self.is_done
end



-- 神龙星体变化
BattleDragonPointChange = BattleDragonPointChange or BaseClass(BattleBehaviour, "BattleDragonPointChange")
function BattleDragonPointChange:__init(idx, data)
    self.idx = idx
    self.data = data
    self:AddToActorList(self.idx)
    self.is_done = false
end
function BattleDragonPointChange:playInternal()
    local ch_info = BattleData.Instance:GetDragonCh(self.idx)
    if ch_info then
        ch_info.rage_value = self.data.rage_value
    end
    TimeHelper:AddDelayTimer(
        function()
            self.is_done = true
        end,
        0.3
    )
end
function BattleDragonPointChange:checkDone()
    return self.is_done
end


-- 偃月兔护盾变化
BattleRabbitShiledChange = BattleRabbitShiledChange or BaseClass(BattleBehaviour, "BattleRabbitShiledChange")
-- data == {state} state : 1生盾，2毁盾，3盾受击
function BattleRabbitShiledChange:__init(idx, data)
    self.idx = idx
    self.data = data
    self:AddToActorList(self.idx)
    self.is_done = false
end

function BattleRabbitShiledChange:playInternal()
    local ch = BattleCtrl.Instance.displayer:getCh(self.idx)
    local state = self.data.state
    if not ch and state == 2 then
        self.is_done = true
        return
    end
    if not ch then
        ch = BattleCtrl.Instance.stream:CreateRabbitShield(self.idx)
    end

    if state == 1 then
        ch:CreateShield(function ()
            self.is_done = true
        end)
    elseif state == 2 then
        ch:DestroyShiled(function ()
            self.is_done = true
        end)
    elseif state == 3 then
        ch:BeAttacked(function ()
            self.is_done = true
        end)
    end
end

function BattleRabbitShiledChange:checkDone()
    return self.is_done
end

-- 偃月兔释放护盾移动
BattleRabbitShiledMove = BattleRabbitShiledMove or BaseClass(BattleBehaviour, "BattleRabbitShiledMove")

function BattleRabbitShiledMove:__init(idx, to_idx)
    self.is_done = false
    self.idx = idx
    self.to_idx = to_idx ~= nil and to_idx or self.idx
    self.on_move_end = BindTool.Bind(self.onMoveEnd, self)
    self:AddToActorList(self.idx, self.to_idx)
end

function BattleRabbitShiledMove:playInternal()
    if self.idx == self.to_idx then
        self.displayer:RestoreIdx(self.idx, true, self.on_move_end)
    else
        local t_idx
        if self.to_idx <= 5 or (self.to_idx >= 11 and self.to_idx <= 15) then
            t_idx = self.to_idx + 5
        else
            t_idx = self.to_idx
        end
        self.displayer:RunToIdx(self.idx, t_idx, self.on_move_end)
    end
end

function BattleRabbitShiledMove:onMoveEnd()
    self.is_done = true
end

function BattleRabbitShiledMove:checkDone()
    return self.is_done
end
function BattleRabbitShiledMove:ForceDone()
    BattleBehaviour.ForceDone(self)
    self.displayer:RestoreIdx(self.idx, true)
end