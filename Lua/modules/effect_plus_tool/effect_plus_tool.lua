

FlyStarEffectCell = FlyStarEffectCell or DeclareMono("FlyStarEffectCell")
---NodePos====obj
---EndPos====obj
---StartPos====obj
---CrossPos====obj
function FlyStarEffectCell:FlyStarEffectCell()

end

function FlyStarEffectCell:SetData(param_t)
    self.mono = param_t.mono
    self.offset = param_t.offset or 0.4
    self.end_func = param_t.end_func
    self.end_func_time = param_t.end_func_time or 0.2

    self.start_effect = param_t.start_effect
    self.start_wait_time = param_t.start_wait_time or 0
    self.complete_effect = param_t.complete_effect
    
    self.pre_pause = param_t.pre_pause or 0
    self.speed_time = param_t.speed_time or 3
    self.data_complete = true
end

function FlyStarEffectCell:SetCompleteEffect(effect_id)
    self.complete_effect = effect_id
end

function FlyStarEffectCell:SetStartEffect(effect_id)
    self.start_effect = effect_id
end

function FlyStarEffectCell:OnDestroy()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
    end

    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_delay)
    end

    if self.pause_handle_delay then 
        TimeHelper:CancelTimer(self.pause_handle_delay)
    end 

    if self.start_handle_delay then 
        TimeHelper:CancelTimer(self.start_handle_delay)
    end 
end

-- 一般不需要使用这个系列的功能（值能够完整执行完整个特效且有一定时间残留）
-- 当出现需要强行停止特效的情况下需要使用这个系列的功能
function FlyStarEffectCell:AllHandleCancel()
    self:CancelFlyStarEffect()
    self:CancelStartEffect()
    self:CancelCompleteEffect()
    self:CancelHandleRun()
end

function FlyStarEffectCell:CancelHandleRun()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
    end
end

function FlyStarEffectCell:CancelFlyStarEffect()
    if self.fly_star then
        self.mono.EffectTool:Stop(self.fly_star)
        self.fly_star = nil
    end 
end

function FlyStarEffectCell:CancelStartEffect()
    if self.start_handle then
        self.mono.EffectTool:Stop(self.start_handle)
        self.start_handle = nil
    end 
end

function FlyStarEffectCell:CancelCompleteEffect()
    if self.complete_handle then
        self.mono.EffectTool:Stop(self.complete_handle)
        self.complete_handle = nil
    end 
end 

function FlyStarEffectCell:DataCheck()
    if not self.data_complete and IS_EDITOR then 
        LogError("Please SetData Before")
        return false
    end 
    if self.mono == nil and IS_EDITOR then 
        LogError("Cant Play Effect WITHOUT mono") 
        return false
    end 
    return true
end

function FlyStarEffectCell:AdjectPos(pos,target)
    self[target].transform.localPosition = pos
end 

function FlyStarEffectCell:Play(effect_id)
    if tonumber(effect_id) == nil then return end 
    if not self:DataCheck() then return end 

    local time = 0
    local startPos = self.StartPos.transform.position
    local endPos = self.EndPos.transform.position
    local crossPos = self.CrossPos.transform.position
    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, self.offset)
    UH.Pos(self.NodePos.transform, startPos)

    self:CancelFlyStarEffect()
    if self.mono.EffectTool.isActiveAndEnabled and self.fly_star == nil then 
        self.fly_star = self.mono.EffectTool:Play(effect_id, self.NodePos)
    end 

    local run_func = function()
        if shoot_curve then
            time = time + Time.deltaTime * self.speed_time
            if time >= 1 then
                self.handle_delay = TimeHelper:AddDelayTimer(
                    function()
                        if self.end_func ~= nil then 
                            self.end_func()
                        end
                        self:CancelCompleteEffect()
                        if self.complete_effect ~= nil and self.mono.EffectTool.isActiveAndEnabled and self.complete_handle == nil then 
                            self.complete_handle = self.mono.EffectTool:Play(self.complete_effect, self.CompletePos)
                        end 
                    end,
                    self.end_func_time
                )
                return true
            end
            if self.NodePos then UH.Pos(self.NodePos.transform, shoot_curve:GetPos(time)) end 
        end
        return false
    end

    local start_func = function ()
        self:CancelStartEffect()
        if self.start_effect ~= nil and self.mono.EffectTool.isActiveAndEnabled and self.start_handle == nil then 
            self.start_handle = self.mono.EffectTool:Play(self.start_effect, self.StartPos)
        end 
    end

    if self.start_wait_time ~= nil then 
        self.start_handle_delay = TimeHelper:AddDelayTimer(start_func,self.start_wait_time)
    else 
        start_func()
    end 

    if self.pre_pause ~= nil then 
        self.pause_handle_delay = TimeHelper:AddDelayTimer(
            function()
                self.handle_run = Runner.Instance:RunUntilTrue(run_func)
            end,self.pre_pause)
    else 
        self.handle_run = Runner.Instance:RunUntilTrue(run_func)
    end
end

----------------------------简易版抛物线----------------------------
--NodePos========gameObject 需要移动的节点
----------------------------ParabolaBoard----------------------------
ParabolaBoard = ParabolaBoard or DeclareMono("ParabolaBoard", UIWidgetBaseItem)
--speed_time : 移动速度
--complete_func 结束回调
--x_offset : x方向偏移 或存在CrossPos或cross_pos时可不传
--y_offset : y方向偏移 或存在CrossPos或cross_pos时可不传
function ParabolaBoard:SetData(data)
    self.x_offset = data.x_offset
    self.y_offset = data.y_offset
    self.offset = data.offset or 0.4
    self.speed_time = data.speed_time or 3
    self.complete_func = data.complete_func
end
--start_pos 起始位置 或固定的可在预设 StartPos
--target_pos目标位置 或固定的可在预设 TargetPos
--cross_pos 中点位置 或固定的可在预设 CrossPos 可不传
function ParabolaBoard:Play(start_pos,target_pos,cross_pos)
    start_pos = start_pos or self.StartPos.localPosition
    target_pos = target_pos or self.TargetPos.localPosition
    cross_pos = cross_pos or self:GetCrossPos(start_pos,target_pos)

    local curve = Game.CurveParabola(start_pos, target_pos, cross_pos, self.offset)

    local time = 0
    self.NodePos:SetObjActive(false)
    UH.LocalPos(self.NodePos,start_pos)
    Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    self.handle_runner = Runner.Instance:RunUntilTrue(function()
        time = time + Time.deltaTime * self.speed_time
        if time >= 1 then
            if self.complete_func then
                self.complete_func()
            end
            self.NodePos:SetObjActive(false)
            if self.EndPos then
                UH.LocalPos(self.NodePos,self.EndPos.localPosition)
            end
            return true
        else
            self.NodePos:SetObjActive(true)
            UH.LocalPos(self.NodePos,curve:GetPos(time))
            return false
        end
    end)
end

function ParabolaBoard:GetCrossPos(start_pos,target_pos)
    if self.CrossPos then
        return self.CrossPos.localPosition
    end
    local pos = GameMath.CenterPos(start_pos,target_pos)
    pos.x = self.x_offset or pos.x
	pos.y = self.y_offset or pos.y
    return pos
end

function ParabolaBoard:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    self.handle_runner = nil
end