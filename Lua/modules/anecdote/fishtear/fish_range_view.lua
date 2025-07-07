FishRangeView = FishRangeView or DeclareView("FishRangeView","anecdote/fish_range")
VIEW_DECLARE_LEVEL(FishRangeView, ViewLevel.Lt)
VIEW_DECLARE_MASK(FishRangeView, ViewMask.None)

function FishRangeView:LoadCallback(param_t)
    if param_t and param_t.is_show_effect and param_t.task_id then 
        self.panel.Pos:SetActive(FishAnswerData.Instance:IsSceneOperate())
        self.panel.BtnIrony:SetActive(FishAnswerData.TaskTag.Inroy == param_t.task_id)
        self.panel.BtnCatch:SetActive(FishAnswerData.TaskTag.Catch == param_t.task_id or FishAnswerData.TaskTag.Storehouse == param_t.task_id)
    end 
end

function FishRangeView:OpenCallback()
    self.panel:OnTaskChanged()
end

function FishRangeView:CloseCallback()

end

function FishRangeView:OnClickClose()
	ViewMgr:CloseView(FishRangeView)
end

FishRangePanel = FishRangePanel or DeclareMono("FishRangePanel", UIWFlushPanel)

function FishRangePanel:FishRangePanel()
    self.data = FishAnswerData.Instance
    self.data_cares = {
        {data = self.data:SMData(), func = self.OnTaskChanged,keys = {"cur_task_id"}},
        {data = self.data:SMData(), func = self.OnTaskChanged,keys = {"is_show_effect"}},
    }

    self.timer = nil
    self.keep_effect = nil
end 

function FishRangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.timer then
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end
end

function FishRangePanel:OnTaskChanged()
    self.Pos:SetActive(self.data:IsSceneOperate() and self.data:SMData().is_show_effect)

    self.BtnIrony:SetActive(self.data:InInroy())
    self.BtnCatch:SetActive(self.data:InCatch() or self.data:InStorehouse())


    if self.EffectTool.isActiveAndEnabled then 
        if self.keep_effect == nil and self.data:IsSceneOperate() then 
            self.keep_effect = self.EffectTool:Play("10170003")
        end 
    end 
end

function FishRangePanel:OnClickCache()
    self.EffectTool:Play("10170004")
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    if self.keep_effect then 
        self.EffectTool:Stop(self.keep_effect)
    end 
end

function FishRangePanel:EndCacheEffect()
    if self.data:InCatch() then 
        AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.FishTear)
    end
    if self.data:InStorehouse() then
        AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.PuzzleStorehouse)
    end
end

function FishRangePanel:OnClickIrony()
    self.EffectTool:Play("10170004")

    if self.data:IronyFinish() then return end 
    local param_t = {
        desc = 7,
        time = 3,
        finish_func = BindTool.Bind(self.Clickfinish, self),
        seq = 0,
    }
    MainOtherCtrl.Instance:OpenGather(param_t)
    if self.keep_effect then 
        self.EffectTool:Stop(self.keep_effect)
        self.keep_effect = nil
    end 
end

function FishRangePanel:Clickfinish()
    self.data:ShowFishEffect(0.5)
    self.data:IronyFinish(true)

    if self.timer then
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end
    self.timer = TimeHelper:AddDelayTimer(function()
        ViewMgr:OpenView(FishAnswerView) 
    end,1.5)
end