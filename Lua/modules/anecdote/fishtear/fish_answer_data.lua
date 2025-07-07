FishAnswerData = FishAnswerData or BaseClass()

FishAnswerData.TaskTag = {
    Inroy = 65121,
    Catch = 65122,
    Submit = 65123,
    Storehouse = 65184,--神秘宝藏任务id
}

function FishAnswerData:__init()
	if FishAnswerData.Instance ~= nil then
		FishAnswerData.LogError("[FishAnswerData] attempt to create singleton twice!")
		return
	end
    FishAnswerData.Instance = self

    self.config = Config.fish_tear_auto
    self.sm_data = SmartData.New({cur_task_id = 0,is_show_effect = false})

    self.effect_1 = nil 
    self.effect_2 = nil 
    self.effect_3 = nil 

    self.effect_1_timer = nil
    self.effect_2_timer = nil 
end 

function FishAnswerData:__delete()
	if FishAnswerData.Instance == self then
		FishAnswerData.Instance = nil
	end	
end

function FishAnswerData:SMData()
	return self.sm_data
end

function FishAnswerData:SetCurTask(task_id)
    self.sm_data.cur_task_id  = task_id
    self:ShowFishCatchEffect()
end

function FishAnswerData.CanSceneOperate(task_id)
    if task_id == nil then return false end 
    return task_id == FishAnswerData.TaskTag.Inroy or task_id == FishAnswerData.TaskTag.Catch or task_id == FishAnswerData.TaskTag.Storehouse  
end

function FishAnswerData:IsSceneOperate()
    return self.sm_data.cur_task_id == FishAnswerData.TaskTag.Inroy 
        or self.sm_data.cur_task_id == FishAnswerData.TaskTag.Catch
        or self.sm_data.cur_task_id == FishAnswerData.TaskTag.Storehouse  
end

function FishAnswerData:InInroy()
    return self.sm_data.cur_task_id == FishAnswerData.TaskTag.Inroy 
end

function FishAnswerData:InCatch()
    return self.sm_data.cur_task_id == FishAnswerData.TaskTag.Catch
end

-- 神秘宝藏
function FishAnswerData:InStorehouse(task_id)
    task_id = task_id or self.sm_data.cur_task_id
    return task_id == FishAnswerData.TaskTag.Storehouse
end

function  FishAnswerData:Submit()
    return self.sm_data.cur_task_id == FishAnswerData.TaskTag.Submit
end

function FishAnswerData:ShowFishEffect(phase_1,phase_2)
    self:ClearFishEffectTimer()
    self.effect_1_timer = TimeHelper:AddDelayTimer(function()
        self.effect_1 = SE:Play(SceneEffectConfig.EffectList.FishTearShowEffect,  Vector3.zero)
        self.effect_2_timer = TimeHelper:AddDelayTimer(function()
            SE:Stop(self.effect_1)
            self.effect_2 = SE:Play(SceneEffectConfig.EffectList.FishTearKeepEffect,  Vector3.zero)
        end,phase_2 or 0.26)
    end,phase_1 or 1)
end

function FishAnswerData:ShowFishCatchEffect()
    if self:InCatch() then
        if self.effect_3 == nil then
            self.effect_3 = SE:Play(SceneEffectConfig.EffectList.FishTearcatchEffect, Vector3.zero)
        end 
    else
        SE:Stop(self.effect_3)
        self.effect_3 = nil
    end
    if self:InStorehouse() then
        
    end
end

function FishAnswerData:CancelFishEffect()
    SE:Stop(self.effect_2)
end

function FishAnswerData:ClearFishEffectTimer()
    if self.effect_1_timer ~= nil then 
        TimeHelper:CancelTimer(self.effect_1_timer)
        self.effect_1_timer = nil
    end
    
    if self.effect_2_timer ~= nil then 
        TimeHelper:CancelTimer(self.effect_2_timer)
        self.effect_2_timer = nil
    end
end


function FishAnswerData:ClearData()
    self:CancelFishEffect()
    SE:Stop(self.effect_3)
    self.effect_3 = nil
end

function FishAnswerData:IronyFinish(status)
    if status ~= nil then 
        self.irony_finish = status
    else 
        return self.irony_finish or false
    end 
end
