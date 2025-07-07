local GetGameObjectByIndex = Game.SkillManager.GetGameObjectByIndex
local TriggerSkillEvent = Game.SkillManager.TriggerSkillEvent
local Time = UnityEngine.Time
local UnityProfiler = UnityEngine.Profiling.Profiler
local GetCh
-- local gameSkillMgr = Game.SkillManager.Singleton()
-- local getGameObjectByIndex = Game.SkillManager.GetGameObjectByIndexDel

BattleSkillTriggerKey = {
    -- 冲锋开始、击中、结束
    charge_start = "charge_start",
    charge_hit = "charge_hit",
    charge_end = "charge_end",
    -- 击退后返回开始、结束
    back_start = "back_start",
    back_end = "back_end",
    -- 返回结束
    move_back_end = "move_back_end",
    -- 闪击开始、击中、结束
    strike_start = "strike_start",
    strike_hit = "strike_hit",
    strike_end = "strike_end",
    -- 闪击的第二个和第三个幻影
    phantom_2 = "phantom_2",
    phantom_3 = "phantom_3",
    -- 仙灵祝福击中
    xianlingzhufu_hit = "xianlingzhufu_hit",

    assass_start = "assass_start",
    assass_stand = "assass_stand",
    assass_hit = "assass_hit",
    assass_end = "assass_end",

    center_start = "center_start",
    center_end = "center_end"
}

BattleLuaFunction = {}

function BattleLuaFunction.Init()
    UpdateBeat:Add(BattleLuaFunction.Update)
    FixedUpdateBeat:Add(BattleLuaFunction.FixedUpdate)
    BattleLuaFunction.update_until_true = {}
    BattleLuaFunction.fixedupdate_until_true = {}
    if BattleCtrl then
        GetCh = BindTool.Bind1(BattleCtrl.Instance.displayer.getCh, BattleCtrl.Instance.displayer)
    end
end

function BattleLuaFunction.Update()
    local new_list = {}
    for _, func in pairs(BattleLuaFunction.update_until_true) do
        if not func() then
            table.insert(new_list, func)
        end
    end
    BattleLuaFunction.update_until_true = new_list
end

function BattleLuaFunction.FixedUpdate()
    local new_list = {}
    for _, func in pairs(BattleLuaFunction.fixedupdate_until_true) do
        if not func() then
            table.insert(new_list, func)
        end
    end
    BattleLuaFunction.fixedupdate_until_true = new_list
end

function BattleLuaFunction.UpdateUntilTrue(func)
    if func() then
        return
    end
    table.insert(BattleLuaFunction.update_until_true, func)
end

function BattleLuaFunction.DelayFrame(func, frame)
    local f = 0
    BattleLuaFunction.UpdateUntilTrue(
        function()
            if f == frame then
                func()
                return true
            end
            f = f + 1
        end
    )
end

function BattleLuaFunction.FixedUpdateUntilTrue(func)
    if func() then
        return
    end
    table.insert(BattleLuaFunction.fixedupdate_until_true, func)
end

local function getParam(Params, n)
    if Params.Length > n then
        return Params[n]
    end
end

function BattleLuaFunction.KnockUp(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local start_time = Time.time

    -- UnityEngine.Profiling.Profiler.BeginSample("StringTest")
    -- for i = 1, 10000 do
    --     local a = "123" .. "456" .. "789"
    -- end
    -- UnityEngine.Profiling.Profiler.EndSample()
    local v_start = tonumber(Params[0])
    local g = tonumber(Params[2])
    local v_angular = tonumber(Params[3])
    local a_angular = tonumber(Params[4])
    if BattleData.Instance:SceneData() == nil then
        return
    end
    local dir_up = BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize()

    FeatureInstance:Start()
    local count = 0
    for i = 0, SkillPlayParam.BehiterIdx.Length - 1 do
        local idx = SkillPlayParam.BehiterIdx[i]
        repeat
            local i_obj = GetGameObjectByIndex(idx)
            if i_obj == nil then
                break
            end
            local transform = i_obj.transform
            local f_original = BattleData.Instance:SceneData():GetForward(idx)
            local p_min = transform.position:Clone()
            local p_max
            local t_up = v_start / g
            local t_suspend = tonumber(Params[1])
            count = count + 1
            local ch
            if GetCh then
                ch = GetCh(idx)
            end
            BattleLuaFunction.FixedUpdateUntilTrue(
                function()
                    if ch then
                        ch.is_knocked_up = true
                        if ch.is_diauped then
                            TriggerSkillEvent(SkillPlayParam.SkillId, SkillPlayParam.PlayerIdx, 1, idx)
                            count = count - 1
                            if count == 0 then
                                FeatureInstance:Terminate()
                            end
                            return true
                        end
                    end
                    local t = Time.time - start_time
                    if t < t_up then
                        local pos = p_min:Clone()
                        pos.y = pos.y + v_start * t - g * math.pow(t, 2) / 2
                        transform.position = pos
                        p_max = pos
                    elseif t < t_up + t_suspend then
                        -- 悬停
                    else
                        local pos = p_max:Clone()
                        pos.y = pos.y - g * math.pow(t - t_up - t_suspend, 2) / 2
                        transform.position = pos
                        if pos.y <= p_min.y then
                            transform.position = p_min
                            transform.forward = f_original
                            TriggerSkillEvent(SkillPlayParam.SkillId, SkillPlayParam.PlayerIdx, 1, idx)
                            if ch then
                                ch.is_knocked_up = false
                            end
                            count = count - 1
                            if count == 0 then
                                FeatureInstance:Terminate()
                            end
                            return true
                        else
                            transform.position = pos
                        end
                    end
                    local angle = v_angular * Time.deltaTime * 10
                    transform:RotateAround(p_min, dir_up, math.min(angle, 180))
                    v_angular = v_angular + a_angular * Time.deltaTime
                end
            )
        until true
    end
end
function BattleLuaFunction.Charge(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local start_time = Time.time
    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_start)

    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    local target = GetGameObjectByIndex(SkillPlayParam.TargetIdxs[Index])
    if target == nil then
        --先这样已测试
        FeatureInstance:Start()
        local behiter = SkillPlayParam.BehiterIdx.Length == 0 and {} or {SkillPlayParam.BehiterIdx[Index]}
        local param = Game.SkillPlayParam(SkillPlayParam.PlayerIdx, {SkillPlayParam.TargetIdxs[Index]}, behiter)
        param.anim = SkillPlayParam.anim
        param.defaultAnimSpeed = SkillPlayParam.defaultAnimSpeed
        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_hit, param)
        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_end)
        FeatureInstance:Terminate()
        return
    end
    local tran_target = target.transform

    local v = tonumber(Params[0])
    local v0 = v
    local d = Vector3.Distance(tran_player.position, tran_target.position)
    local v2 = GameMath.DistancePos(tran_player.position.x, tran_player.position.z, tran_target.position.x, tran_target.position.z, -tonumber(Params[1]))
    local end_pos = Vector3.New(v2.x, tran_player.position.y , v2.y)
    -- local a_brake = tonumber(Params[2])
    local t_hit = (d + tonumber(Params[1])) / v
    local t_pause = tonumber(Params[2])
    -- local t_brake = v / a_brake
    local t_brake = tonumber(Params[3])
    local coef_brake = tonumber(Params[4])
    local has_hit = false
    local behiter = SkillPlayParam.BehiterIdx.Length == 0 and {} or {SkillPlayParam.BehiterIdx[Index]}
    local param = Game.SkillPlayParam(SkillPlayParam.PlayerIdx, {SkillPlayParam.TargetIdxs[Index]}, behiter)
    param.anim = SkillPlayParam.anim
    param.defaultAnimSpeed = SkillPlayParam.defaultAnimSpeed

    FeatureInstance:Start()
    BattleLuaFunction.UpdateUntilTrue(
        -- BattleLuaFunction.FixedUpdateUntilTrue(
        function()
            local t = Time.time - start_time
            if t > (t_hit + t_pause + t_brake) then
                if not has_hit then
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_hit, param)
                    has_hit = true
                end
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_end)
                FeatureInstance:Terminate()
                return true
            elseif t > (t_hit + t_brake) then
            elseif t > t_hit then
                if not has_hit then
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.charge_hit, param)
                    has_hit = true
                end
                -- v = v - a_brake * Time.deltaTime
                -- v = v0 / (coef_brake * (t - t_hit) + 1)
                -- tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v))
                tran_player.position = end_pos
            elseif t == 0 then
                tran_player:LookAt(tran_target.position)
            else
                tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v))
            end
        end
    )
end
function BattleLuaFunction.Assassinate(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    if BattleData.Instance:SceneData() == nil then
        return
    end
    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_start)
    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    local target = GetGameObjectByIndex(SkillPlayParam.TargetIdxs[Index])
    if target == nil then
        local behiter = SkillPlayParam.BehiterIdx.Length == 0 and {} or {SkillPlayParam.BehiterIdx[Index]}
        local param = Game.SkillPlayParam(SkillPlayParam.PlayerIdx, {SkillPlayParam.TargetIdxs[Index]}, behiter)
        param.anim = SkillPlayParam.anim
        param.defaultAnimSpeed = SkillPlayParam.defaultAnimSpeed
        FeatureInstance:Start()
        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_stand)
        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_hit, param)
        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_end)
        FeatureInstance:Terminate()
        return
    end
    local tran_target = target.transform

    local p_original = BattleData.Instance:SceneData():GetPos(SkillPlayParam.PlayerIdx)
    local f_original = BattleData.Instance:SceneData():GetForward(SkillPlayParam.PlayerIdx)
    local t_pos = BattleData.Instance:SceneData():GetPos(SkillPlayParam.TargetIdxs[Index], Vector3.New(0, 0, tonumber(Params[0])))
    tran_player:LookAt(tran_target.position)

    local d = Vector3.Distance(tran_player.position, tran_target.position)
    local t_start = tonumber(Params[1])
    local t_stand = tonumber(Params[2])
    local t_hit = tonumber(Params[3])
    local t_back = tonumber(Params[4])
    local t_end = tonumber(Params[5])
    local has_hit = false
    local has_flashed = false
    local has_stand = false
    local t = 0
    local behiter = SkillPlayParam.BehiterIdx.Length == 0 and {} or {SkillPlayParam.BehiterIdx[Index]}
    local param = Game.SkillPlayParam(SkillPlayParam.PlayerIdx, {SkillPlayParam.TargetIdxs[Index]}, behiter)
    param.anim = SkillPlayParam.anim
    param.defaultAnimSpeed = SkillPlayParam.defaultAnimSpeed

    FeatureInstance:Start()
    BattleLuaFunction.FixedUpdateUntilTrue(function ()
        t = t + Time.deltaTime

        if t > t_start + t_hit + t_end + t_back + t_stand then
            if not has_hit then
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_hit, param)
                has_hit = true
            end
            tran_player.position = p_original
            tran_player.forward = f_original
            FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_end)
            FeatureInstance:Terminate()
            return true
        elseif t > t_start + t_hit + t_back + t_stand then
            -- tran_player:Translate(Vector3.New(0, 0, -Time.deltaTime * v_back))
        elseif t > t_start + t_hit + t_stand then
            -- 发生攻击
            if not has_hit then
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_hit, param)
                has_hit = true
            end
        elseif t > t_start + t_stand then
            tran_player.position = t_pos
            tran_player:LookAt(tran_target.position)
            if not has_stand then
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.assass_stand)
                has_stand = true
            end
        elseif t > t_start then
            -- 直接闪到该位置
            if not has_flashed then
                tran_player:Translate(Vector3.New(0, -1000, d))
                has_flashed = true
            end
        else
            --等一会后开始冲
        end
    end)
end
function BattleLuaFunction.ChargeCenter(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    -- local f_original = BattleData.Instance:SceneData():GetForward(SkillPlayParam.PlayerIdx)
    local end_pos = BattleData.Instance:SceneData():GetWholeTransform(SkillPlayParam.PlayerIdx).position
    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    local d = Vector3.Distance(tran_player.position, end_pos)
    local t_stop = tonumber(Params[0])
    local v = d / t_stop
    local t = 0
    local has_stop = false
    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.center_start)
    FeatureInstance:Start()
    tran_player:LookAt(end_pos)
    BattleLuaFunction.UpdateUntilTrue(
        function()
            t = t + Time.deltaTime
            if t > t_stop then
                tran_player.position = end_pos
                if not has_stop then
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.center_end)
                    has_stop = true
                end
                FeatureInstance:Terminate()
                return true
            else
                tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v))
            end
        end
    )
end
function BattleLuaFunction.Strike(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    -- local start_time = Time.time

    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.strike_start)
    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    local tran_target = GetGameObjectByIndex(SkillPlayParam.TargetIdxs[Index]).transform

    BattleLuaFunction.DelayFrame(
        function()
            tran_player:Translate(Vector3.New(0, 1000, 0))
        end,
        1
    )

    local p_original = BattleData.Instance:SceneData():GetPos(SkillPlayParam.PlayerIdx)
    local f_original = BattleData.Instance:SceneData():GetForward(SkillPlayParam.PlayerIdx)

    tran_player:LookAt(tran_target.position)

    local v_foward = tonumber(Params[0])
    local d = Vector3.Distance(tran_player.position, tran_target.position)
    local d_flush = tonumber(Params[1])
    local d_hit = tonumber(Params[2])
    local v_back = tonumber(Params[3])
    local t_start = 0.15
    local t_hit = (d_hit - d_flush) / v_foward
    local t_back = (d + d_hit) / v_back

    local has_hit = false
    local has_flashed = false

    -- local t_phantom_1 = (tonumber(Params[4]) - d_flush) / v_foward
    -- local t_phantom_2 = (tonumber(Params[5]) - d_flush) / v_foward
    -- local t_phantom_3 = (tonumber(Params[6]) - d_flush) / v_foward

    local t_phantom_2 = tonumber(Params[4])
    local t_phantom_3 = tonumber(Params[5])

    local has_gen_2 = false
    local has_gen_3 = false

    local t_brake = tonumber(Params[6])
    local coef_brake = tonumber(Params[7])
    local t_pause = tonumber(Params[8])

    -- Runner.Instance:RunUntilTrue(
    local t = 0
    FeatureInstance:Start()
    -- BattleLuaFunction.UpdateUntilTrue(
    BattleLuaFunction.FixedUpdateUntilTrue(
        function()
            t = t + Time.deltaTime
            -- local t = Time.time - start_time

            if t > t_brake + t_pause + t_hit + t_back + t_start then
                tran_player.position = p_original
                tran_player.forward = f_original
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.strike_end)
                FeatureInstance:Terminate()
                return true
            elseif t > t_hit + t_pause + t_brake + t_start then
                -- 暂停后返回
                tran_player:Translate(Vector3.New(0, 0, -Time.deltaTime * v_back))
            elseif t > t_hit + t_brake + t_start then
                -- 减速后暂停
                -- tran_player:Translate(Vector3.New(0, 0, -Time.deltaTime * v_back))
            elseif t > t_hit + t_start then
                -- 打中后减速
                if not has_hit then
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.strike_hit)
                    has_hit = true
                end
                tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v_foward / (coef_brake * (t - t_hit - t_start) + 1)))
            elseif t > t_start then
                -- 闪现并前冲
                if not has_flashed then
                    tran_player:Translate(Vector3.New(0, -1000, d + d_flush))
                    has_flashed = true
                end
                tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v_foward))
            else
                -- 等一段时间然后闪现
            end
            if not has_gen_2 and t > t_phantom_2 + t_start then
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.phantom_2)
                has_gen_2 = true
            end
            if not has_gen_3 and t > t_phantom_3 + t_start then
                FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.phantom_3)
                has_gen_3 = true
            end
        end
    )
end

function BattleLuaFunction.MoveBack(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local start_time = Time.time

    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    local p_original = BattleData.Instance:SceneData():GetPos(SkillPlayParam.PlayerIdx)
    local f_original = BattleData.Instance:SceneData():GetForward(SkillPlayParam.PlayerIdx)

    tran_player:LookAt(p_original)

    local v = tonumber(Params[0])
    local d = Vector3.Distance(tran_player.position, p_original)
    local t_end = d / v
    local delay_frame = 4
    local triggered_end = false
    FeatureInstance:Start()
    BattleLuaFunction.UpdateUntilTrue(
        function()
            local t = Time.time - start_time
            if delay_frame == 0 then
                tran_player.forward = f_original
                FeatureInstance:Terminate()
                return true
            end
            if t > t_end then
                if not triggered_end then
                    tran_player.position = p_original
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.move_back_end)
                    triggered_end = true
                end
                delay_frame = delay_frame - 1
            end
            if not triggered_end then
                tran_player:Translate(Vector3.New(0, 0, Time.deltaTime * v))
            end
        end
    )
end

function BattleLuaFunction.SheHunTongJi(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local start_time = Time.time
    local imposter = sourceFeatureInstance.Imposter
    local transform = imposter.transform

    local t_withdraw = tonumber(Params[0])
    local v_withdraw = tonumber(Params[1])
    local alpha = 1
    local v_alpha = tonumber(Params[2])
    sourceFeatureInstance:Start()
    FeatureInstance:Start()
    BattleLuaFunction.UpdateUntilTrue(
        function()
            local t = Time.time - start_time
            alpha = alpha - 0.01
            if t > t_withdraw then
                sourceFeatureInstance:Terminate()
                FeatureInstance:Terminate()
                return true
            end
            sourceFeatureInstance:ForeachBlock(
                function(block)
                    block:SetFloat("_Alpha", alpha)
                end
            )
            -- sourceFeatureInstance:SetFloat("_Alpha", alpha)
            transform:Translate(Vector3.New(0, 0, -Time.deltaTime * v_withdraw))
        end
    )
end

function BattleLuaFunction.Repel(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local start_time = Time.time

    local v0_repel = tonumber(Params[0])
    local a_repel = tonumber(Params[1])
    local t_repel = v0_repel / -a_repel
    local t_stand = tonumber(Params[2])
    local v_back = tonumber(Params[3])
    -- 默认可以减到0
    local d_repel = a_repel * t_repel * t_repel / 2 + v0_repel * t_repel
    local t_back = d_repel / v_back

    FeatureInstance:Start()
    for i = 0, SkillPlayParam.BehiterIdx.Length - 1 do
        local v_repel = v0_repel
        local idx = SkillPlayParam.BehiterIdx[i]
        local go = GetGameObjectByIndex(idx)
        local transform = go.transform
        -- local f_original = BattleData.Instance:SceneData():GetForward(idx)
        local p_original = BattleData.Instance:SceneData():GetPos(idx)
        local trigger_back = false
        BattleLuaFunction.UpdateUntilTrue(
            function()
                local t = Time.time - start_time
                if t < t_repel then
                    -- 击退
                    transform:Translate(Vector3.New(0, 0, -Time.deltaTime * v_repel))
                elseif t < t_repel + t_stand then
                    -- 僵直
                elseif t < t_repel + t_stand + t_back then
                    -- 返回
                    if not trigger_back then
                        trigger_back = true
                        local param = Game.SkillPlayParam(SkillPlayParam.PlayerIdx, {idx}, {idx})
                        param.anim = go:GetComponent(typeof(UnityEngine.Animator))
                        FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.back_start, param)
                    end
                    transform:Translate(Vector3.New(0, 0, Time.deltaTime * v_back))
                else
                    transform.position = p_original
                    FeatureInstance:ThrowTrigger(BattleSkillTriggerKey.back_end)
                    FeatureInstance:Terminate()
                    return true
                end
                v_repel = v_repel + Time.deltaTime * a_repel
            end
        )
    end
end

function BattleLuaFunction.UIHide(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    BattleData.Instance.story_data.hide_ui = true
end

function BattleLuaFunction.UIShow(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    BattleData.Instance.story_data.hide_ui = false
end
function BattleLuaFunction.EnemyHide(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    local ch
    local player_idx = SkillPlayParam.PlayerIdx
    local v3 = Vector3.New(-1000,-1000,0)
    for i = 1, 20 do
        if BattleData.IsEnemyIdx(i) and i ~= player_idx then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetPos(v3)--SetIsShow(false)
            end
        end
    end
end
function BattleLuaFunction.EnemyShow()
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    local ch
    for i = 1, 20 do
        if BattleData.IsEnemyIdx(i) then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetPos(ch:getIdxPos())
                -- ch:SetIsShow(true)
            end
        end
    end
end

function BattleLuaFunction.PlayerHide(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    local ch
    local player_idx = SkillPlayParam.PlayerIdx
    local v3 = Vector3.New(-1000,-1000,0)
    for i = 1, 20 do
        if not BattleData.IsEnemyIdx(i) and i ~= player_idx then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetPos(v3)--SetIsShow(false)
            end
        end
    end
end
function BattleLuaFunction.PlayerShow()
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    local ch
    for i = 1, 20 do
        if not BattleData.IsEnemyIdx(i) then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetPos(ch:getIdxPos())
                -- ch:SetIsShow(true)
            end
        end
    end
end

function BattleLuaFunction.ChangePos(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    local tran_player = GetGameObjectByIndex(SkillPlayParam.PlayerIdx).transform
    if SkillPlayParam.TargetIdxs[0] == nil then
        return 
    end
    local tran_target = GetGameObjectByIndex(SkillPlayParam.TargetIdxs[0]).transform
    local changeType = tonumber(Params[0])
    if changeType == 0 then
        tran_player.position = tran_target.position
    else
        tran_player.localPosition = Vector3.zero
    end
    -- local p_original = BattleData.Instance:SceneData():GetPos(SkillPlayParam.PlayerIdx)
    -- local f_original = BattleData.Instance:SceneData():GetForward(SkillPlayParam.PlayerIdx)
end

function BattleLuaFunction.PlayStory(SkillPlayParam, Params, sourceFeatureInstance, FeatureInstance, Index)
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    StoryCtrl.Instance:PlayStoryByStoryId(tonumber(Params[0]))
end
function BattleLuaFunction.PlaySkillUiEffect(SkillPlayParam, Params)
    if BattleCtrl == nil or BattleData == nil then
        return
    end
    BattleData.Instance:ViewInfo().skill_ui_effect_id = tonumber(Params[0])
end