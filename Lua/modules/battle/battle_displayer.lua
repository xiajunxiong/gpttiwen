FILE_REQ("modules/battle/battle_ch")
FILE_REQ("modules/battle/battle_sv")
FILE_REQ("modules/battle/battle_dragon")
FILE_REQ("modules/battle/battle_rabbit_shield")

BattleDisplayerHitRouter = BattleDisplayerHitRouter or BaseClass()
function BattleDisplayerHitRouter:__init()
    -- self.hit_info = {} --key:hit idx,value:behit idx queue
    self.waits = {} --wait hit listener
end

-- function BattleDisplayerHitRouter:PresetHit(hit_idx, be_hit_idx)
--     Debuger.LogError("BattleDisplayerHitRouter:PresetHit---hit==%s,behit==%s", tostring(hit_idx), tostring(be_hit_idx))
--     -- LogError("frameCount", Time.frameCount)
--     local behits = self.hit_info[hit_idx]
--     if behits == nil then
--         behits = {}
--         self.hit_info[hit_idx] = behits
--     end
--     -- table.insert(behits, be_hit_idx)
--     if not behits[be_hit_idx] then
--         behits[be_hit_idx] = 0
--     end
--     behits[be_hit_idx] = behits[be_hit_idx] + 1
-- end

-- function BattleDisplayerHitRouter:OnHitTrigger(hit_idx, be_hit_idx)
--     -- local behits = self.hit_info[hit_idx]
--     -- Debuger.LogError("BattleDisplayerHitRouter:OnHitTrigger---hit==%s,behit==%s,behits===%d", tostring(hit_idx), tostring(be_hit_idx), behits ~= nil and #behits or 0)
--     self:hitTrigger(hit_idx, be_hit_idx)
--     -- -- LogError("frameCount", Time.frameCount)
--     -- if behits ~= nil and next(behits) then
--     --     if be_hit_idx == nil then --all behit
--     --         LogError("!")
--     --         -- local be_hit_idx = behits[1]
--     --         -- table.remove(behits,1)
--     --         -- if #behits == 0 then
--     --         -- 	self.hit_info[hit_idx] = nil
--     --         -- end
--     --         -- self:hitTrigger(hit_idx,be_hit_idx)
--     --         -- local trggered_idxs = {}
--     --         -- for i=#behits,1,-1 do
--     --         -- 	local idx = behits[i]
--     --         -- 	if trggered_idxs[idx] == nil then
--     --         -- 		trggered_idxs[idx] = true
--     --         -- 		table.remove(behits,i)
--     --         -- 		self:hitTrigger(hit_idx,idx)
--     --         -- 	end
--     --         -- end
--     --         -- if #behits == 0 then
--     --         -- 	self.hit_info[hit_idx] = nil
--     --         -- end
--     --         self.hit_info[hit_idx] = nil
--     --         -- for _, v in ipairs(behits) do
--     --         --     self:hitTrigger(hit_idx, v)
--     --         -- end
--     --         for be_hit_idx, _ in pairs(behits) do
--     --             self:hitTrigger(hit_idx, be_hit_idx)
--     --         end
--     --     else
--     --         LogError("@", behits)
--     --         -- for i, v in ipairs(behits) do
--     --         --     if v == be_hit_idx then
--     --         --         table.remove(behits, i)
--     --         --         if #behits == 0 then
--     --         --             self.hit_info[hit_idx] = nil
--     --         --         end
--     --         --         self:hitTrigger(hit_idx, be_hit_idx)
--     --         --         break
--     --         --     end
--     --         -- end
--     --         if behits[be_hit_idx] and behits[be_hit_idx] then
--     --             behits[be_hit_idx] = behits[be_hit_idx] - 1
--     --             if not next(behits) then
--     --                 self.hit_info[hit_idx] = nil
--     --             end
--     --             self:hitTrigger(hit_idx, be_hit_idx)
--     --         end
--     --     end
--     -- end
-- end

function BattleDisplayerHitRouter:WaitBehit(hit_idx, be_hit_idx, func)
    table.insert(self.waits, {hit_idx = hit_idx, be_hit_idx = be_hit_idx, func = func})
end

function BattleDisplayerHitRouter:hitTrigger(hit_idx, be_hit_idx)
    -- LogError(Time.frameCount, "hitTrigger", hit_idx, be_hit_idx)
    -- Debuger.LogError("BattleDisplayerHitRouter:hitTrigger===hit=%d,behit=%d", hit_idx, be_hit_idx)
    -- local waits = {}
    for i, v in ipairs(self.waits) do
        if be_hit_idx == v.be_hit_idx and (v.hit_idx == nil or v.hit_idx == hit_idx) then
            -- LogError("!!!!!!!!!!!!!!")
            table.remove(self.waits, i)
            v.func()
            break
        -- else
        --     table.insert(waits, v)
        end
    end
    -- self.waits = waits
end

function BattleDisplayerHitRouter:Clear()
    -- self.hit_info = {}
    self.waits = {}
end

BattleDisplayer = BattleDisplayer or BaseClass()

function BattleDisplayer:__init(data)
    if BattleDisplayer.Instance ~= nil then
        Debuger.LogError("[BattleDisplayer] attempt to create singleton twice!")
        return
    end
    BattleDisplayer.Instance = self

    self.chs = {} ---key:grid idx,value:BattleCh
    self.svs = {} ---key:idx,value:BattleSV
    self.ele_box_ch = {}-- key:idx,value:BoxCh
    self.dgns = {}---key:inx,value:BattleDragon
    self.rabbit_shields = {}--key:inx,value:BattleRabbitShield
    self.delay_delete_chs = {} --key:BattleCh,value:true
    self.data = data
    self.hit_router = BattleDisplayerHitRouter.New()
    self.buff_manager = BattleBuffManager.New()
    self.halo_manager = BattleHaloManager.New()

    self.timer_handles = {} --key:handle value:true
    self.dodge_handles = {} --key:idx value:handle
    self.dodge_coms = {} --key:idx value:{func}
    self.base_data = self.data:BaseData()
    self.effonch_handles = {} --key:idx value:{custom_sign = handle}
    self.handle_wait_chains_effect = {}
    self.handle_repeat_chains = {}

    BattleSkillPool.SetHitEvent(BindTool.Bind(self.onSkillHit, self))
    self.command_i_care =
        self.base_data:KeysCare(
        {"command_i", "round_phase"},
        function()
            if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_NULL then
                return
            end
            self:StopEffectOnCh(self.cur_cmd_eff)
            self.cur_cmd_eff = nil
            if BattleData.Instance:IsOB() == 0 and self.base_data.round_phase == BATTLE_ROUND_PHASE.Command then
                local ch = self:getCh(self.data:CurCmdIdx())
                if ch ~= nil then
                    -- if ch.vo.curhp > 0 then
                        self.cur_cmd_eff = self:PlayEffectOnCh(self.data:CurCmdIdx(), SceneEffectConfig.EffectList.CurCmdEffect, true)
                    -- end
                else
                    if self.cur_cmd_eff_runner then
                        Runner.Instance:RemoveRunUntilTrue(self.cur_cmd_eff_runner)
                    end
                    self.cur_cmd_eff_runner =
                        Runner.Instance:RunUntilTrue(
                        function()
                            local ch = self:getCh(self.data:CurCmdIdx())
                            if ch ~= nil then
                                -- if ch.vo.curhp > 0 then
                                    self.cur_cmd_eff = self:PlayEffectOnCh(self.data:CurCmdIdx(), SceneEffectConfig.EffectList.CurCmdEffect, true)
                                -- end
                                return true
                            end
                        end
                    )
                end
            end
        end
    )
    self.login_base_data = LoginData.Instance.base_data
    self.login_care = self.login_base_data:Care(BindTool.Bind(self.LoginStateChange, self), "login_state")
end
function BattleDisplayer:LoginStateChange()
    if self.niutou_story_info then
        self.niutou_story_info:Stop()
        self.niutou_story_info:storyEnd()
        self.niutou_story_info = nil
    end
end

function BattleDisplayer:IsHasBuffId(grid_idx, buff_id)
    local ch = self:getCh(grid_idx)
    if ch == nil then
        return false
    end
    local data_ch = ch.vo
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

function BattleDisplayer:__delete()
    BattleSkillPool.SetHitEvent(nil)
    self.base_data:Uncare(self.command_i_care)
    self.login_base_data:Uncare(self.login_care)
    self:Clear()
end

function BattleDisplayer:RestoreAllCh()
    for grid_idx, ch in pairs(self.chs) do
        ch:SetPos(self:getIdxPos(grid_idx))
        ch:SetForward(self:getIdxForward(grid_idx))
        ch:PlayAnim(ChAnimType.Idle)
    end
end

function BattleDisplayer:CreateCh(grid_ind, ch_info, on_complete, summon, mutiple)
    local ch = BattleCh.New(ch_info)
    self.chs[grid_ind] = ch
    ch.created = false
    if mutiple ~= true then
        ch:Prepare(summon)
    end
    ch:Load(
        function()
            if ch.vo.curhp <= 0 then
                ch:WaitAnimEnd(
                    ChAnimType.Death,
                    function()
                        ch:ClearWaitDieCheck()
                        ch.created = true
                    end
                )
                ch:PlayAnim(ChAnimType.Death)
            else
                ch.created = true
            end
            if on_complete then
                on_complete(grid_ind)
            end
        end
    )
    return ch
end
function BattleDisplayer:CreateSV(grid_idx, sv_info, on_complete)
    local ch = BattleSV.New(sv_info)
    self.svs[grid_idx] = ch
    ch:Load(function ()
        if on_complete then
            on_complete(grid_idx)
        end
    end)
    return ch
end

function BattleDisplayer:SummonCh(grid_ind, ch_info, on_complete, dont_prepare)
    local ch = BattleCh.New(ch_info)
    self.chs[grid_ind] = ch
    ch.created = false
    if not dont_prepare then
        ch:Prepare(true)
    end
    ch:Load(
        function()
            ch.created = true
            --真实模型加载完后回调
            if on_complete then
                on_complete(grid_ind)
            end
        end
    , true)
    return ch
end

function BattleDisplayer:EnableAllObjShadow(enable)
    for _, v in pairs(self.chs) do
        v:EnableShadow(enable)
    end
end

function BattleDisplayer:PlayAnim(idx, anim_type)
    local ch = self:getCh(idx)
    -- if ch == nil then
    --	Debuger.LogError("!!!!BattleDisplayer:PlayAnim===idx=%s,anim=%s",tostring(idx),tostring(anim_type))
    -- end
    if ch == nil then
        return false
    else
        if anim_type == ChAnimType.Death then
            --播个特效
            ch:SetAnimActive(true)
            SE:Play(SceneEffectConfig.EffectList.Death, ch:GetPos())
        end
        ch:PlayAnim(anim_type)
        return true
    end
end

function BattleDisplayer:WaitAnimEnd(idx, anim, func)
    local ch = self:getCh(idx)
    ch:WaitAnimEnd(anim, func)
end

function BattleDisplayer:WaitBehit(hit_idx, be_hit_idx, func)
    self.hit_router:WaitBehit(hit_idx, be_hit_idx, func)
end

function BattleDisplayer:PresetHit(hit_idx, be_hit_idx)
    self.hit_router:PresetHit(hit_idx, be_hit_idx)
end

-- function BattleDisplayer:onAnimHit(battle_ch)
--     local hit_ch_idx = self:chToIdx(battle_ch)
--     self.hit_router:OnHitTrigger(hit_ch_idx, nil)
-- end

-- 技能配置里的打击事件会告诉你要谁谁谁触发了被击事件
function BattleDisplayer:onSkillHit(hiter_idx, behiter_idx)
--    LogError("onSkillHit", hiter_idx, behiter_idx)
    self.hit_router:hitTrigger(hiter_idx, behiter_idx)
end

function BattleDisplayer:Clear()
    for key, value in pairs(self.handle_repeat_chains) do
        Runner.Instance:CancelRepeat(value)
    end
    self.handle_repeat_chains = {}

    for key, value in pairs(self.handle_wait_chains_effect) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_wait_chains_effect = {}

    if self.camera_anim_handle then
        Runner.Instance:RemoveRunUntilTrue(self.camera_anim_handle)
        self.camera_anim_handle = nil
    end
    if self.buff_renew_runner then
        Runner.Instance:RemoveRunUntilTrue(self.buff_renew_runner)
    end
    for _, value in pairs(self.dodge_handles) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.dodge_handles = {}

    self.hit_router:Clear()
    self.halo_manager:Clear()
    for _, v in pairs(self.ele_box_ch) do
        v:DeleteMe()
    end
    self.ele_box_ch = {}
    local deleted_cache = {}
    for _, v in pairs(self.chs) do
        deleted_cache[v] = true
        v:DeleteMe()
    end
    self.chs = {}
    for _, v in pairs(self.svs) do
        v:DeleteMe()
    end
    self.svs = {}
    for _, v in pairs(self.dgns) do
        v:DeleteMe()
    end
    self.dgns = {}
    for ch, _ in pairs(self.delay_delete_chs) do
        if not deleted_cache[ch] then
            ch:DeleteMe()
        end
    end
    self.delay_delete_chs = {}
    deleted_cache = {}

    for _, v in pairs(self.rabbit_shields) do
        v:DeleteMe()
    end
    self.rabbit_shields = {}

    for _, th in pairs(self.timer_handles) do
        TimeHelper:CancelTimer(th)
    end
    self.timer_handles = {}
    BattleSkillPool.Instance:UnloadUnusedAssets()
    self:StopEffectOnCh(self.cur_cmd_eff)
    self.cur_cmd_eff = nil
    if self.cur_cmd_eff_runner then
        Runner.Instance:RemoveRunUntilTrue(self.cur_cmd_eff_runner)
        self.cur_cmd_eff_runner = nil
    end
    for k,v in pairs(self.effonch_handles) do
        for key, value in pairs(v) do
            if type(value) == "table" then
                self:StopEffectOnCh(value.obj)
            else
                self:StopEffectOnCh(value)
            end
        end
    end
    self.effonch_handles = {}
end

function BattleDisplayer:getChByType(type)
    local list = {}
    for idx, ch in pairs(self.chs) do
        if ch.vo.character_type == type then
            list[idx] = ch
        end
    end
    return list
end

function BattleDisplayer:AllChCreated()
    for _, ch in pairs(self.chs) do
        if not ch.created then
            return false
        end
    end
    return true
end
function BattleDisplayer:CheckBuff()
    if self.buff_renew_runner then
        Runner.Instance:RemoveRunUntilTrue(self.buff_renew_runner)
    end
    local halo_list = BattleData.Instance:GetHaloList()
    self.buff_renew_runner = Runner.Instance:RunUntilTrue(function ()
        if self:AllChCreated() then
            for _, ch in pairs(self.chs) do
                if next(ch.vo.buff_list) ~= nil then
                    for _, value in pairs(ch.vo.buff_list) do
                        if type(value) == "table" then
                            self:AddBuff(value.grid_idx, value)
                        else
                            -- LogError(value)
                        end
                    end
                    ch:PlayAllBuff(BuffStageKey.Start)
                end
            end
            if halo_list then
                for caster, list in pairs(halo_list) do
                    for _, data in ipairs(list) do
                        for i = 1, 20 do
                            if data.cover_grid_flag[i] == 1 then
                                 self:AddHalo(i, caster, data.halo_id)
                            end
                        end
                    end
                end
            end
            return true
        end
        return false
    end)
end
function BattleDisplayer:getCh(idx)
    local re = self.chs[idx]
    if idx and idx >= 21 and idx <= 30 then
        re = self.svs[idx]
    elseif idx and idx >= 31 and idx <= 32 then
        re = self.dgns[idx]
    elseif idx and idx >= 41 and idx <= 50 then
        re = self.rabbit_shields[idx]
    end
    if re == nil then
        for ch, _ in pairs(self.delay_delete_chs) do
            if ch.vo and ch.vo.grid_idx == idx then
                return ch
            end
        end
    -- Debuger.LogErrorSD("BattleDisplayer:getCh is NULL idx=%s", idx == nil and "[NULL]" or tostring(idx))
    end
    return re
end

function BattleDisplayer:chToIdx(ch)
    for k, v in pairs(self.chs) do
        if v == ch then
            return k
        end
    end
end

function BattleDisplayer:getACh()
    for idx, ch in pairs(self.chs) do
        return idx, ch
    end
end

function BattleDisplayer:getIdxPos(idx, offset)
    -- local player_idx = self.data:PlayerIdx()
    -- if player_idx > 10 then --PVP交换位置
    --     idx = (idx > 10) and (idx - 10) or (idx + 10)
    -- end
    local scene_data = self.data:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetPos(idx, offset)
end

function BattleDisplayer:getIdxForward(idx)
    -- local player_idx = self.data:PlayerIdx()
    -- if player_idx > 10 then --PVP交换位置
    --     idx = (idx > 10) and (idx - 10) or (idx + 10)
    -- end
    local scene_data = self.data:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetForward(idx)
end

function BattleDisplayer:SwapIdx(idx1, idx2)
    local ch1 = self.chs[idx1]
    local ch2 = self.chs[idx2]
    self.chs[idx2] = ch1
    self.chs[idx1] = ch2
end
function BattleDisplayer:FixedPos(idx, copy_idx)
    local ch = self:getCh(idx)
    if ch then
        if copy_idx then
            idx = copy_idx
        end
        local fw = self:getIdxForward(idx)
        local pos = self:getIdxPos(idx)
        ch:SetPos(pos)
        ch:SetForward(fw)
    end
end
function BattleDisplayer:SetShow(idx, is_show)
    local ch = self:getCh(idx)
    if ch then
        ch:SetIsShow(is_show)
    end
end

--*移动
-------------------ch move-----------
--forward:0 no end forward,1:face,2:idx tran same
function BattleDisplayer:MoveToIdx(idx, to_idx, run_anim, offset, forward, on_com)
    -- LogErrorHL("MoveToIdx",idx,to_idx,run_anim,offset,forward,on_com)
    local tar_pos = self:getIdxPos(to_idx, offset)
    if forward == 1 then
        forward = self:getIdxPos(to_idx) - tar_pos
    elseif forward == 2 then
        forward = self:getIdxForward(idx)
    else
        forward = nil
    end
    local ch = self:getCh(idx)
    if ch == nil then
        if on_com ~= nil then
            on_com()
        end
    else
        ch:MoveToPos(tar_pos, run_anim, forward, on_com)
    end
end
function BattleDisplayer:MoveToCenter(idx, to_idx, on_com)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        if on_com ~= nil then
            on_com()
        end
        return
    end
    local re = scene_data:GetWholeTransform(to_idx)
    local tar_pos = re.position
    local forward = re.forward
    local ch = self:getCh(idx)
    if ch == nil then
        if on_com ~= nil then
            on_com()
        end
    else
        ch:MoveToPos(tar_pos, nil, forward, on_com)
    end
end

--run to to_idx
function BattleDisplayer:RunToIdx2(idx, to_idx, on_com)
    self:MoveToIdx(idx, to_idx, true, Vector3.New(0, 0, 2), nil, on_com)
end

--run to front of to_idx
function BattleDisplayer:RunToIdx(idx, to_idx, on_com)
    -- local ch = self:getCh(idx)
    -- if ch == nil then
    --     if on_com ~= nil then
    --         on_com()
    --     end
    --     return
    -- end
    -- local tar_pos = self:getIdxPos(to_idx)
    -- local sou_pos = ch:GetPos()
    -- local dir = (tar_pos - sou_pos):Normalize()
    -- local move_tar = tar_pos - dir * 1.5
    -- local fd = tar_pos - move_tar
    -- fd.y = 0
    -- ch:MoveToPos(move_tar, true, fd, on_com)
    self:MoveToIdx(idx, to_idx, true, Vector3.New(0, 0, 2), 2, on_com)
end

function BattleDisplayer:RunToIdxWithOffset(idx, to_idx, offset, on_com)
    self:MoveToIdx(idx, to_idx, true, offset, nil, on_com)
end
function BattleDisplayer:RestoreHallowIdx(idx, on_com)
    local ch = self:getCh(idx)
    if ch == nil then
        if on_com then
            on_com()
        end
        return
    end
    if ch.vo.owner_idx == nil then
        return
    end
    local pos = self:getIdxPos(ch.vo.owner_idx, Vector3.New(0, 1, -1))
    local forward = self:getIdxForward(ch.vo.owner_idx)
    ch:MoveToPos(pos, nil, forward, on_com)
end
-------------------ch move END-------------------

function BattleDisplayer:LookAt(idx, to_idx)
    local ch1 = self:getCh(idx)
    local ch2 = self:getCh(to_idx)
    if ch1 and ch2 then
        ch1:LookAtCh(ch2)
    end
end
-- 闪避
--move to step back
function BattleDisplayer:Dodge(idx, atker_idx, combo_idx, on_com)
    local ch = self:getCh(idx)
    local atker_ch = self:getCh(atker_idx)
    local tar_pos
    if not ch then
        if on_com then
            on_com()
        end
        return
    end
    if atker_ch ~= nil then
        local ch_pos = ch:GetPos()
        local move_dir = (ch_pos - atker_ch:GetPos()):Normalize()
        tar_pos = ch_pos + (move_dir * 2)
        ch:SetForward(-move_dir)
    else
        tar_pos = self:getIdxPos(idx, Vector3.New(0, 0, -2))
    end
    ch:MoveToPos(
        tar_pos,
        false,
        nil,
        function()
            --timer里不能套runer否则有bug(时间不准确)
            --本来这里可以节省移动时间的现在加了残影就不行了
            local stay_time = 0.5
            if self.dodge_handles[idx] then
                Runner.Instance:RemoveRunUntilTrue(self.dodge_handles[idx])
                --闪避和连射冲突，策划要连射表现要快，闪避残影又要花时间显示
                if self.dodge_coms[idx] then
                    for _,v in ipairs(self.dodge_coms[idx]) do
                        v()
                    end
                    self.dodge_coms[idx] = {}
                end
                if on_com then
                    on_com()
                end
                tar_pos = self:getIdxPos(idx)
                ch:MoveToPos(tar_pos)
            end
            self.dodge_coms[idx] = self.dodge_coms[idx] or {}
            table.insert(self.dodge_coms[idx], on_com)
            self.dodge_handles[idx] = 
            Runner.Instance:RunUntilTrue(function ()
                stay_time = stay_time - Time.deltaTime
                if stay_time <= 0 then
                    if combo_idx == nil or idx ~= combo_idx then
                        tar_pos = self:getIdxPos(idx)
                        ch:MoveToPos(tar_pos, false, self:getIdxForward(idx), on_com, true)
                    else
                        if on_com then
                            on_com()
                        end
                    end
                    self.dodge_coms[idx] = nil
                    self.dodge_handles[idx] = nil
                    return true
                end
            end)
        end,
        true
    )
end

--move back to normal pos
function BattleDisplayer:RestoreIdx(idx, run_anim, on_com)
    if idx > 20 and idx < 100 then
        self:RestoreHallowIdx(idx, on_com)
        return
    end
    self:MoveToIdx(idx, idx, run_anim, nil, 2, on_com)
end

--待办 查一下Diaup和被击掉血的数据推入顺序
function BattleDisplayer:Diaup(idx, dont_play_behit, end_func)
    local ch = self:getCh(idx)
    if not ch then
        return
    end
    ch:SetAnimActive(true)--石化关闭了动画状态击,死亡的时候打开一下
    self.delay_delete_chs[ch] = true
    self.chs[idx] = nil

    if dont_play_behit then
        ch:Diaup(
            function()
                self.delay_delete_chs[ch] = nil
                ch:DeleteMe()
                if end_func then
                    end_func()
                end
            end
        )
        return
    end
    local suc = self:PlayAnim(idx, ChAnimType.Behit)
    if suc == true then
        --[[ ch:WaitBehitBack(
            function()
                ch:Diaup(
                    function()
                        self.delay_delete_chs[ch] = nil
                        ch:DeleteMe()
                    end
                )
            end
        ) ]]
    local timer_i = #self.timer_handles + 1
    self.timer_handles[timer_i] = TimeHelper:AddDelayTimer(function ()
            ch:Diaup(
                    function()
                        self.delay_delete_chs[ch] = nil
                        ch:DeleteMe()
                        if end_func then
                            end_func()
                        end
                    end
                )
        end, 0.6)
    else
        ch:Diaup(
            function()
                self.delay_delete_chs[ch] = nil
                ch:DeleteMe()
                if end_func then
                    end_func()
                end
            end
        )
    end
end
--清理死亡检查
function BattleDisplayer:ClearWaitDieCheck(idx)
    local ch = self:getCh(idx)
    if not ch then
        return
    end
    ch:ClearWaitDieCheck()
end

--普通死亡消失表现
function BattleDisplayer:Disappear(idx, end_func)
    local ch = self:getCh(idx)
    if not ch then
        return
    end
    self.delay_delete_chs[ch] = true
    self.chs[idx] = nil

    local suc = self:PlayAnim(idx, ChAnimType.Death)
    BattleData.Instance:SetChInfo(idx, nil)
    if suc == true then
        self:WaitAnimEnd(
            idx,
            ChAnimType.Death,
            function()
                ch:ClearWaitDieCheck()
                ch:Disappear(
                    function()
                        self.delay_delete_chs[ch] = nil
                        ch:DeleteMe()
                        if end_func then
                            end_func()
                        end
                    end
                )
            end
        )
    else
        ch:Disappear(
            function()
                self.delay_delete_chs[ch] = nil
                ch:DeleteMe()
                if end_func then
                    end_func()
                end
            end
        )
    end
end

function BattleDisplayer:Vanish(idx)
    local ch = self:getCh(idx)
    if not ch then
        return
    end
    self.chs[idx] = nil
    ch:DeleteMe()
    BattleData.Instance:SetChInfo(idx, nil)
end 
function BattleDisplayer:Behit(idx, atker_idx, defence, beat_back, on_com)
    if defence == true then
        self:PerformDefenceSkill(idx, on_com)
    else
        self:PlayAnim(idx, ChAnimType.Behit)
        local ch = self:getCh(idx)
        if ch ~= nil then
            ch:Repel(on_com)
        else
            on_com()
        end
    end

    --额外逻辑 怪物组1009
    --if BattleData.Instance:BaseData().monster_group_id == 1009 and BattleData.IsEnemyIdx(idx) then
        --self:SetScale(idx, 0.2, 1.7)
    --end
    -- 被击转向
    -- if suc == true then
    --     local hit_forward = self:getIdxForward(idx)
    --     if atker_idx ~= nil then
    --         local atker_ch = self:getCh(atker_idx)
    --         if atker_ch ~= nil then --forward to attacker
    --             local atker_orginal_pos = self:getIdxPos(atker_idx)
    --             local akter_pos = atker_ch:GetPos()
    --             if atker_orginal_pos ~= akter_pos then --在原位置的攻击者不会导致被击角色朝向
    --                 local ch = self:getCh(idx)
    --                 local orginal_forward = hit_forward
    --                 hit_forward = akter_pos - ch:GetPos()
    --                 ch:SetForward(hit_forward)
    --                 self:WaitAnimEnd(
    --                     idx,
    --                     anim,
    --                     function()
    --                         ch:SetForward(orginal_forward)
    --                     end
    --                 )
    --             end
    --         end
    --     end

    -- 被击后退
    -- if beat_back == true then
    --     local ch = self:getCh(idx)
    --     local tar_pos = ch:GetPos() - hit_forward:Normalize() * (defence == true and 0.2 or 0.3)
    --     ch:MoveToPos(tar_pos, false, nil, nil)
    --     self:WaitAnimEnd(idx, anim, function()
    --         self:RestoreIdx(idx)
    --     end)
    -- end

    -- end
end

function BattleDisplayer:Retrieve(idx)
    -- LogError("Retrieve", idx)
    local ch = self:getCh(idx)
    if ch then
        self.chs[idx] = nil
        self.delay_delete_chs[ch] = true
        ch:Retrieve(
            function()
                self.delay_delete_chs[ch] = nil
                ch:DeleteMe()
            end
        )
    end
end

function BattleDisplayer:Summon(idx, onfinish)
    self.halo_manager:CheckHalo(idx)
    local ch = self:getCh(idx)
    if ch then
        ch:Summon(onfinish)
    end
end

function BattleDisplayer:Reinforce(idx, end_func)
    local ch = self:getCh(idx)
    if ch then
        if ch.vo.curhp > 0 then
            local fw = self:getIdxForward(idx)
            ch:SetPos(self:getIdxPos(idx) - fw * 16)
            ch:SetForward(fw)
            ch:PlayAnim(ChAnimType.Run)
            ch:MoveToPos(self:getIdxPos(idx), true, nil, end_func)
        else
            local fw = self:getIdxForward(idx)
            ch:SetPos(self:getIdxPos(idx))
            ch:SetForward(fw)
        end
    end
end

function BattleDisplayer:SelfDestruct(idx, onfinish)
    local ch = self:getCh(idx)
    if ch == nil then
        if onfinish then
            onfinish()
        end
        return
    end
    if ch.vo.character_type == BATTLE_CH_TYPE.MONSTER then
        self.chs[idx] = nil
        self.delay_delete_chs[ch] = true
    end
    ch:SelfDestruct(
        function()
            -- self.hit_router:OnHitTrigger(idx, nil)
            if onfinish ~= nil then
                onfinish(
                    function()
                        if ch.vo and ch.vo.character_type == BATTLE_CH_TYPE.MONSTER then
                            self.delay_delete_chs[ch] = nil
                            ch:DeleteMe()
                        end
                    end
                )
            end
        end
    )
end
function BattleDisplayer:YanHuo(idx, onfinish)
    local ch = self:getCh(idx)
    if ch == nil or ch.vo == nil then
        if onfinish then
            onfinish()
        end
        return
    end
    if ch.vo.character_type == BATTLE_CH_TYPE.MONSTER then
        self.chs[idx] = nil
        self.delay_delete_chs[ch] = true
    end
    ch:YanHuo(function ()
        if onfinish ~= nil then
            onfinish(
                function()
                    if ch.vo.character_type == BATTLE_CH_TYPE.MONSTER then
                        self.delay_delete_chs[ch] = nil
                        ch:DeleteMe()
                    end
                end
            )
        end
    end)
end


function BattleDisplayer:RunAway(idx, suc, onfinish)
    -- LogErrorHL("RunAway", idx)
    local ch = self:getCh(idx)
    if not ch then
        return
    end
    self:PlayAnim(idx, ChAnimType.Run)
    local fw = self:getIdxForward(idx)
    ch:SetForward(-fw)
    if suc then
        self.chs[idx] = nil
        self.delay_delete_chs[ch] = true
    end
    local timer_i = #self.timer_handles + 1
    local t_had =
        TimeHelper:AddDelayTimer(
        function()
            self.timer_handles[timer_i] = nil
            if suc then
                -- if idx == self.data:PlayerIdx() then
                --     local pet_idx = self.data:PlayerPetIdx()
                --     if pet_idx then
                --         local pet_ch = self:getCh(pet_idx)
                --         if pet_ch then
                --             self.chs[pet_idx] = nil
                --             self.delay_delete_chs[pet_ch] = true
                --             pet_ch:MoveToPos(
                --                 run_away_target,
                --                 true,
                --                 nil,
                --                 function()
                --                     self.delay_delete_chs[pet_ch] = nil
                --                     pet_ch:DeleteMe()
                --                     BattleData.Instance:SetChInfo(pet_idx, nil)
                --                 end
                --             )
                --         end
                --     end
                -- end
                local run_away_target = self:getIdxPos(idx) - fw * 8
                -- LogError("RunAway")
                ch:MoveToPos(
                    run_away_target,
                    true,
                    nil,
                    function()
                        -- LogError("RunAway Finished")
                        self.delay_delete_chs[ch] = nil
                        ch:DeleteMe()
                        if onfinish ~= nil then
                            onfinish()
                        end
                    end
                )
            else
                self:PlayAnim(idx, ChAnimType.Idle)
                if ch.vo.character_type == BATTLE_CH_TYPE.ROLE or ch.vo.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
                    self:PlayAnim(idx, ChAnimType.FallDown)
                    self:WaitAnimEnd(
                        idx,
                        ChAnimType.FallDown,
                        function()
                            ch:SetForward(fw)
                            if onfinish ~= nil then
                                onfinish()
                            end
                        end
                    )
                else
                    ch:SetForward(fw)
                    if onfinish ~= nil then
                        onfinish()
                    end
                end
            end
        end,
        1
    )
    self.timer_handles[timer_i] = t_had
end

function BattleDisplayer:Seal(idx, is_succ, seal_type, callback)
    -- LogErrorHL("Seal", idx, is_succ)
    local ch = self:getCh(idx)
    if ch then
        -- local eff = self:PlayEffectOnCh(idx, SceneEffectConfig.EffectList["PetSealEffect" .. seal_type])
        ch:Seal(
            is_succ,
            function()
                -- self:StopEffectOnCh(eff)
                if callback then
                    callback()
                end
                if is_succ then
                    self.delay_delete_chs[ch] = nil
                    ch:DeleteMe()
                    if self.chs[idx] == ch then
                        self.chs[idx] = nil
                    end
                else
                    ch:setScaleSeal(1)
                end
            end
        )
    else
        LogErrorBT("!!!Not Exist!!!")
    end
end
function BattleDisplayer:RecoverSeal(idx)
    local ch = self:getCh(idx)
    if ch then
        ch:RecoverSeal()
    end
end
function BattleDisplayer:ShakeAndDie(idx)
    -- LogErrorHL("Seal", idx, is_succ)

    local ch = self:getCh(idx)
    if ch then
        ch:Shake(
            function()
                self:Disappear(idx)
            end
        )
    else
        LogErrorBT("!!!Not Exist!!!")
    end
end

function BattleDisplayer:ChangeEquipment(idx, before_id, after_id)
    local ch = self:getCh(idx)
    if ch then
        ch:ChangeEquipment(before_id, after_id)
    end
end

function BattleDisplayer:ChangeAttribute(idx, attr_key, attr_val)
    local ch = self:getCh(idx)
    if ch then
        -- BattleData.Instance:GetChInfo(idx):Trace("element")
        -- LogErrorHL("ChangeAttributeBefore", BattleData.Instance:GetChInfo(idx))
        ch:ChangeAttribute(attr_key, attr_val)
    -- LogErrorHL("ChangeAttributeAfter", BattleData.Instance:GetChInfo(idx))
    end
end

function BattleDisplayer:PopHp(idx, type, num, num2, behit_desc, atker)
    local ch = self:getCh(idx)
    if ch ~= nil then
        -- local pos = ch:GetPos()
        -- pos.y = pos.y + ch:Height() / 2
        -- self.data:PushPopupData(ch:GetPos(), type, num, num2, behit_desc, ch:Height(), idx, atker)
        self.data:PushPopupData({pos = ch:GetPos(), type = type, num = num, num2 = num2, behit_desc = behit_desc, height = ch:Height(), atker = atker, behiter = idx, ch = ch})
    end
end

function BattleDisplayer:AddBuff(idx, data, on_remove)
    local ch = self:getCh(idx)
    if ch then
        local buff_id = data.new_buff_id
        local buff_level = data.new_buff_level
        -- local basic = BattleData.BuffConfig(buff_id)
        local cfg = BattleData.BuffLevelConfig(buff_id, buff_level)
        if cfg == nil then
            LogError("buff",buff_id,"获取不到等级",buff_level,"的配置,当前战斗组",BattleData.Instance:BaseData().monster_group_id)
            if on_remove then
                on_remove()
            end
            return
        end
        ch:AddBuff(data, on_remove)
    end
end

function BattleDisplayer:PlayBuff(idx, buff_key, stage_key, on_finished)
    local ch = self:getCh(idx)
    if ch then
        ch:PlayBuff(buff_key, stage_key, on_finished)
    end
end

function BattleDisplayer:PlayAllBuff(idx, stage_key, on_finished)
    local ch = self:getCh(idx)
    local played = false
    if ch then
        played = ch:PlayAllBuff(stage_key, on_finished)
    else
        if on_finished then
            on_finished()
        end
    end
    return played
end

function BattleDisplayer:PlayOneBuff(idx, buff_id, stage_key, on_finished)
    local ch = self:getCh(idx)
    local played = false
    if ch then
        played = ch:PlayOneBuff(buff_id, stage_key, on_finished)
    else
        on_finished()
    end
    return played
end

function BattleDisplayer:StopBuff(idx, buff_key, on_remove)
    local ch = self:getCh(idx)
    if ch then
        ch:StopBuff(buff_key, on_remove)
    else
        on_remove()
    end
end

function BattleDisplayer:ReplaceBuff(idx, data)
    local ch = self:getCh(idx)
    if ch then
        ch:ReplaceBuff(data)
    end
end
function BattleDisplayer:ChangeSustainNum(idx, data)
    local ch = self:getCh(idx)
    if ch then
        ch:ChangeSustainNum(data)
    end
end

function BattleDisplayer:ForceCompleteSkill(atker_idx, target_list, on_complete)
    TimeHelper:AddDelayFrameTimer(
        function()
            if target_list then
                for _, idx in ipairs(target_list) do
                    self:onSkillHit(atker_idx, idx)
                end
            end
            if on_complete ~= nil then
                on_complete()
            end
        end
    )
end

function BattleDisplayer:PerformAttack(atker_idx, target_list, hiter_list, on_complete)
    local ch = self:getCh(atker_idx)
    if ch == nil then
        self:ForceCompleteSkill(atker_idx, target_list, on_complete)
        return
    end
    local ass_id = ch:AttackSkillAssetId()
    BattleSkillPool.Instance:Play(ass_id, atker_idx, target_list, hiter_list, on_complete)
end

SKILL_TIMING_TYPE = {
    PRE = 1,
    IN = 2,
    POST = 3,
    HIT = 4
}

function BattleDisplayer:PerformSkill(skill_id, atker_idx, target_list, behiter_list, skill_timing_type, on_complete, version)
    -- LogErrorBT("PerformSkill", skill_id, atker_idx, target_list, behiter_list, skill_timing_type)
    -- LogErrorHL("PerformSkill", skill_id, atker_idx, behiter_list, skill_timing_type)
    local ch = self:getCh(atker_idx)
    if ch == nil then
        self:ForceCompleteSkill(atker_idx, target_list, on_complete)
        return
    end
    --skill_id to skill_asset_id
    local cfg = SkillData.GetConfig(skill_id)
    local ass_id = cfg and cfg["asset_id_" .. skill_timing_type] or ""
    if skill_timing_type == SKILL_TIMING_TYPE.IN and version == 2 then
        ass_id = cfg and cfg["asset_id_2_"..version]
    end
    if ass_id == "" then
        if skill_timing_type == SKILL_TIMING_TYPE.IN then
            ass_id = "0"
        else
            on_complete()
            return
        end
    end
    --[[ local on_com
    if FreedomCode.NiuYaoSkill == true and ch.vo.character_id == 10022 and BattleData.Instance:BaseData().monster_group_id == 1002 then
        if skill_id == 335 or skill_id == 334 then
            BattleData.Instance.story_data.is_start = true
            on_com = function ()
                BattleData.Instance.story_data.is_start = false
                on_complete()
            end
        end
    else
        on_com = on_complete
    end ]]
    
    -- LogError(ass_id)
    -- LogError(atker_idx)
    -- LogError(target_list)
    -- LogError(behiter_list)
    -- Game.MyTool.Pause()
    BattleSkillPool.Instance:Play(ass_id, atker_idx, target_list, behiter_list, on_complete)
    local skillParam = BattleSkillPool.Instance:GetCurSkillParam()
    if skillParam then
        skillParam.SkillId = skill_id
    end
    return skillParam
    -- else
    -- on_complete()
    -- end
end

function BattleDisplayer:PerformHallowSkill(atker_idx, ass_id, target_list, behiter_list, on_complete)
    BattleSkillPool.Instance:Play(ass_id, atker_idx, target_list, behiter_list, on_complete)
    local skillParam = BattleSkillPool.Instance:GetCurSkillParam()
    return skillParam
end

function BattleDisplayer:PerformSkillByAssetId(asset_id, atker_idx, target_list, behiter_list, on_complete)
    if asset_id ~= nil and asset_id ~= "" then
        BattleSkillPool.Instance:Play(asset_id, atker_idx, target_list, behiter_list, on_complete)
    else
        on_complete()
    end
end

function BattleDisplayer:PerformUseItemSkill(asset_id, user_idx, target_idx, on_complete)
    --item_id to skill_asset_id
    local tar_list = {target_idx}
    BattleSkillPool.Instance:Play(asset_id, user_idx, tar_list, tar_list, on_complete)
end

function BattleDisplayer:PerformSummonSkill(summoner_idx, be_summoner_idx, on_complete)
    local tar_list = {be_summoner_idx}
    BattleSkillPool.Instance:Play(BattleSkillPool.ConstAssetId.Summon, summoner_idx, tar_list, tar_list, on_complete)
end
function BattleDisplayer:PerformSummonSkill2(summoner_idx, tar_list, on_complete)
    BattleSkillPool.Instance:Play(BattleSkillPool.ConstAssetId.Summon, summoner_idx, tar_list, tar_list, on_complete)
end
function BattleDisplayer:PerformSummonSkill3(summoner_skill, summoner_idx, be_summoner_idx, on_complete)
    local tar_list = {be_summoner_idx}
    BattleSkillPool.Instance:Play(summoner_skill, summoner_idx, tar_list, tar_list, on_complete)
end
function BattleDisplayer:PerformRetrieveSkill(retriever_idx, be_retrieve_idx, on_complete)
    local tar_list = {be_retrieve_idx}
    BattleSkillPool.Instance:Play(BattleSkillPool.ConstAssetId.Retrieve, retriever_idx, tar_list, tar_list, on_complete)
end

function BattleDisplayer:PerformSealSkill(sealer_idx, be_seal_idx, on_complete)
    local tar_list = {be_seal_idx}
    BattleSkillPool.Instance:Play(BattleSkillPool.ConstAssetId.Seal, sealer_idx, tar_list, tar_list, on_complete)
end

function BattleDisplayer:PerformDefenceSkill(defencer_idx, on_complete)
    local tar_list = {defencer_idx}
    BattleSkillPool.Instance:Play(BattleSkillPool.ConstAssetId.Defence, defencer_idx, tar_list, tar_list, on_complete)
end

function BattleDisplayer:PerformUseFabaoSkill(asset_id, caster_idx, target_idx, on_complete)
    local tar_list = {target_idx}
    BattleSkillPool.Instance:Play(asset_id, caster_idx, tar_list, tar_list, on_complete)
end
function BattleDisplayer:PerformUseFabaoSkill2(asset_id, caster_idx, tar_list, on_complete)
    BattleSkillPool.Instance:Play(asset_id, caster_idx, tar_list, tar_list, on_complete)
end

function BattleDisplayer:PlayEffectOnCh(idx, config, pin_up, on_end)
    -- LogError("PlayEffectOnCh", idx, config)
    local ch = self:getCh(idx)
    if ch ~= nil then
        return SE:Play(config, ch:GetPos(), on_end, pin_up and ch.root_node or nil)
    end
end

-- function BattleDisplayer:PlayEffectOnCh(idx, eff_id, pin_up, auto_release, on_end)
--     -- LogError("PlayEffectOnCh")
--     local ch = self:getCh(idx)
--     if ch ~= nil then
--         -- LogError("PlayEffectOnCh")
--         local config = {path = string.format("effect/%d.prefab", eff_id), auto_release = auto_release or false}
--         return SE:Play(config, ch:GetPos(), on_end, pin_up and ch.root_node or nil)
--     -- ResMgr:LoadEffect(eff_id, function (obj)
--     --     LogError("PlayEffectOnCh")
--     --     LogError(obj.name)
--     --     if ch ~= nil then
--     --         LogError("PlayEffectOnCh")
--     --         local eff = UnityGameObj.Instantiate(obj)
--     --         eff:SetPosition(ch:GetPos())
--     --     end
--     -- end)
--     end
-- end

function BattleDisplayer:StopEffectOnCh(eff)
    if eff then
        SE:Release(eff)
    end
end
function BattleDisplayer:FlushSpecial(idx, sp_id)
    local ch = self:getCh(idx)
    if ch then
        ch:FlushSpecial(sp_id)
    end
end

--变身
function BattleDisplayer:Transform(idx, data, callback)
    BattleData.Instance:SetChInfo(idx, data)
    local ch = self:getCh(idx)
    if ch ~= nil then
        if ch.vo.character_id == 10020 
        and BattleData.Instance:BaseData().monster_group_id == 1002 then
            AudioMgr:PlayEffect(AudioType.Battle, "NiuYaoBianShen")
            local ncCure = ch:GetRoot():GetComponentInChildren(typeof("NcCurveAnimation"))
            if ncCure then
                ncCure.enabled = true
            end
            local story_info = StoryCtrl.Instance:PlayStoryByStoryId(101)
            if story_info == nil then
                ch:SetVo(BattleData.Instance:GetChInfo(idx))
                ch:FlushDrawer(callback)
                TimeHelper:AddDelayFrameTimer(function ()
                    SE:Play(SceneEffectConfig.EffectList.BianShen,nil,nil,ch.root_node)
                end,1)
                return
            end
            self.niutou_story_info = story_info
            story_info:SetCustomFunc(callback)
            self:PlayAnim(idx, ChAnimType.BianShen)
            self:WaitAnimEnd(idx, ChAnimType.BianShen, function ()
                ch:SetVo(BattleData.Instance:GetChInfo(idx))
                ch:FlushDrawer(function ()
                    TimeHelper:AddDelayFrameTimer(function ()
                        story_info:Resume()
                    end, 1)
                    ch:PlayAnim(ChAnimType.BianShen)
                end)
            end)
        else
            ch:SetVo(BattleData.Instance:GetChInfo(idx))
            ch:FlushDrawer(callback)
            TimeHelper:AddDelayFrameTimer(function ()
                SE:Play(SceneEffectConfig.EffectList.BianShen,nil,nil,ch.root_node)
            end,1)
        end
    end
end
function BattleDisplayer:ForceTransformEnd()
    if self.niutou_story_info == nil then
        return
    end
    if self.niutou_story_info:State() ~= StoryConfig.State.End then
        self.niutou_story_info:Stop()
        self.niutou_story_info:storyEnd()
    end
end
-- 变大
function BattleDisplayer:SetScale(idx, size,limit)
    local ch = self:getCh(idx)
    ch:setScale(ch:curScale() + size, limit)
end

-- function BattleDisplayer:PerformSkill1To1(type, caster_idx, target_idx, on_complete)
--     local tar_list = {target_idx}
--     BattleSkillPool.Instance:Play(type, caster_idx, tar_list, tar_list, on_complete)
-- end

function BattleDisplayer:CameraAnim(clipName, func)
    local vir_cam = BattleSkillPool.Instance:GetVirtualCamera()
    if vir_cam then
        local virCamAnim = vir_cam:GetOrAddComponent(typeof(UnityEngine.Animation))
        local animObj = ClipGeter.GetClip(clipName)
        virCamAnim.playAutomatically = false
        if animObj then
            virCamAnim.clip = animObj
            virCamAnim:AddClip(animObj, animObj.name)
        end
        SceneCtrl.Instance:CurScene():RecoverCamera()
        virCamAnim:Play()
        BattleLuaFunction.UIHide()
        if self.camera_anim_handle then
            Runner.Instance:RemoveRunUntilTrue(self.camera_anim_handle)
            self.camera_anim_handle = nil
        end
        self.camera_anim_handle = Runner.Instance:RunUntilTrue(function ()
            if virCamAnim.isPlaying == false then
                BattleLuaFunction.UIShow()
                SceneCtrl.Instance:CurScene():RecallCamera()
                if func then
                    func()
                end
                return true
            end
            return false
        end)
    else
        if func then
            func()
        end
    end
end
function BattleDisplayer:ForceStopCameraAnim()
    if self.camera_anim_handle then
        Runner.Instance:RemoveRunUntilTrue(self.camera_anim_handle)
        self.camera_anim_handle = nil
    end
    local vir_cam = BattleSkillPool.Instance:GetVirtualCamera()
    if vir_cam then
        local virCamAnim = vir_cam:GetOrAddComponent(typeof(UnityEngine.Animation))
        virCamAnim:Stop()
        local len = vir_cam.m_Lens
        len.FieldOfView = 22
        vir_cam.m_Lens = len
    end
    SceneCtrl.Instance:CurScene():RecallCamera()
    BattleLuaFunction.UIShow()
end
function BattleDisplayer:XTZD(func)
    self:WaitAnimEnd(13, ChAnimType.Skill1, func)
    self:PlayAnim(13, ChAnimType.Skill1)
end

function BattleDisplayer:AddHalo(idx, caster, halo_id)
    self.halo_manager:AddHalo(idx, caster, halo_id)
    local ch = self:getCh(idx)
    if ch then
        ch:AddHalo(halo_id, caster)
    end
end
function BattleDisplayer:RemoveHalo(idx, caster, halo_id)
    self.halo_manager:RemoveHalo(idx, caster, halo_id)
    local ch = self:getCh(idx)
    if ch then
        ch:RemoveHalo(halo_id, caster)
    end
end
function BattleDisplayer:PlayBuffAssetStart(idx, asset_name)
    local ch = self:getCh(idx)
    if ch then
        ch:PlayBuffAssetStart(asset_name)
    end
end
function BattleDisplayer:PlayBuffAssetStage(idx, asset_name, stage_key, on_finished)
    local ch = self:getCh(idx)
    if ch then
        ch:PlayBuffAssetStage(asset_name, stage_key, on_finished)
    end
end
function BattleDisplayer:RemoveBuffAsset(idx, asset_name)
    local ch = self:getCh(idx)
    if ch then
        ch:RemoveBuffAsset(asset_name)
    end
end
function BattleDisplayer:HideHuiYan(idx)
    local ch = self:getCh(idx)
    if ch then
        ch:HideHuiYan()
    end
end

function BattleDisplayer:RegisterSkill(skill_id, type, version)
    local cfg = SkillData.GetConfig(skill_id)
    if cfg then
        local ass_id = cfg and cfg["asset_id_" .. type] or ""
        if type == SKILL_TIMING_TYPE.IN and version == 2 then
            ass_id = cfg and cfg["asset_id_2_"..version] or ass_id
        end
        -- if ass_id ~= nil then
        if ass_id ~= "" then
            BattleSkillPool.Instance:Register(ass_id)
            return ass_id
        end
    -- else
    --     Debuger.LogErrorSD("BattleDisplayer:RegisterSkill ERROR,asset_id get failed!skill_id=%s,type=%s",tostring(skill_id),tostring(type))
    --     return nil
    -- end
    end
end
function BattleDisplayer:RegisterSkillAsset(asset_id)
    BattleSkillPool.Instance:Register(asset_id)
end
function BattleDisplayer:UnregisterSkillAsset(asset_id)
    BattleSkillPool.Instance:Unregister(asset_id)
end
function BattleDisplayer:UnregisterSkill(skill_id, type)
    local cfg = SkillData.GetConfig(skill_id)
    if cfg then
        local ass_id = cfg and cfg["asset_id_" .. type] or ""
        if ass_id ~= "" then
            BattleSkillPool.Instance:Unregister(ass_id)
        end
    end
end

function BattleDisplayer:Test(idx, param)
    local ch = self:getCh(idx)
    ch:SetAnimActive(param)
end
function BattleDisplayer:PlayLink(idx, data)
    self.effonch_handles[idx] = self.effonch_handles[idx] or {}
    self.effonch_handles[idx]["link_together"] = SE:Play(SceneEffectConfig.EffectList.LinkChains)
    self:ShowAndSetupChains(self.effonch_handles[idx]["link_together"], idx, data)
end
function BattleDisplayer:ShowAndSetupChains(handle, idx, data)
    if not VersionTag.HasTag(VersionTag.LineRenderer) then
        return
    end
    self.handle_wait_chains_effect[idx] = Runner.Instance:RunUntilTrue(function ()
        if handle.obj ~= nil then
            local coms = handle.obj:GetComponentsInChildren(typeof('UnityEngine.LineRenderer'), true)
            self:UpdateChains(coms, idx, data)
            return true
        end
    end)
end
function BattleDisplayer:UpdateChains(coms, idx, data)
    self.handle_repeat_chains[idx] = Runner.Instance:RepeatCall(function ()
        if coms.Length > 0 then
            if self:getCh(idx) and self:getCh(data.link_target_idx) then
                local pos1 = self:getCh(idx):GetPos()
                local pos2 = self:getCh(data.link_target_idx):GetPos()
                for i = 0, coms.Length - 1 do
                    LineRenderer.SetPosition(coms[i], 1, pos1)
                    LineRenderer.SetPosition(coms[i], 0, pos2)
                end
            else
                self:LinkBreak(idx)
            end
        end
    end, 0.03)
end
function BattleDisplayer:LinkBreak(idx, data)
    if self.handle_repeat_chains[idx] then
        Runner.Instance:CancelRepeat(self.handle_repeat_chains[idx])
        self.handle_repeat_chains[idx] = nil
    end
    if self.handle_wait_chains_effect[idx] then
        Runner.Instance:RemoveRunUntilTrue(self.handle_wait_chains_effect[idx])
        self.handle_wait_chains_effect[idx] = nil
    end
    if self.effonch_handles[idx] and self.effonch_handles[idx]["link_together"] and self.effonch_handles[idx]["link_together"].obj then
        self:StopEffectOnCh(self.effonch_handles[idx]["link_together"].obj)
        self.effonch_handles[idx]["link_together"] = nil
    end
end
function BattleDisplayer:DeleteCh(idx)
    local ch = self:getCh(idx)
    if ch then
        ch:DeleteMe()
        self.chs[idx] = nil
    end
end
function BattleDisplayer:StatusChange(idx, status)
    local ch = self:getCh(idx)
    if ch then
        if status == 2 then
            ch:StatusUnactive()
        elseif status == 1 then
            ch:StatusActive()
        else
            ch:StatusRecovery()
        end
    end
end
function BattleDisplayer:CreateBox(idx, end_func)
    local pos = self:getIdxPos(idx)
    local forward = self:getIdxForward(idx)
    --在BOSS格子的位置进行创建宝箱并展示打开过程
    BattleData.Instance:CreateRewardBox(pos, forward, end_func)
end
function BattleDisplayer:ClearRewardBox()
    BattleData.Instance:ClearRewardBox()
end

function BattleDisplayer:CreateEleBox(idx, npc_id, end_func)
    local pos = self:getIdxPos(idx)
    local forward = self:getIdxForward(idx)
    if self.ele_box_ch[idx] then
        self.ele_box_ch[idx]:DeleteMe()
        self.ele_box_ch[idx] = nil
    end
    local info = Config.npc_auto.npc_list[npc_id]
    if info == nil then
        return
    end
    info = TableCopy(info)
    local vo = CommonStruct.NpcVo(info)
    vo.obj_type = SceneObjType.Box
    vo.is_open = 0
    vo.end_func = end_func
    vo.is_ele_box = true
    vo.obj_id = nil
    vo.end_func = function()
        if self.ele_box_ch[idx] then
            self.ele_box_ch[idx]:DeleteMe()
            self.ele_box_ch[idx] = nil
        end
    end
    self.ele_box_ch[idx] = SceneObj.CreateObj(vo)
    self.ele_box_ch[idx]:SetForward(forward)
    self.ele_box_ch[idx]:SetPos(pos)
end