MainTaskView = MainTaskView or DeclareView("MainTaskView","main_task/main_task",Mod.MainTask.Main)
-- MainPart==============GameObject
-- TaskPart==============GameObject

function MainTaskView:MainTaskView() 
	self.data = MainTaskData.Instance
end 

function MainTaskView:LoadCallback(param_t)
    if param_t ~= nil and param_t.open_param ~= nil and param_t.open_param == 3 then 
        self:ClickTask()
    end 
end 

function MainTaskView:CloseCallback()

end 

function MainTaskView:ClickMainTask()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian) 
    self.MainPart:SetActive(true)
    self.TaskPart:SetActive(false)

    UH.SpriteName(self.ButtonMain,"_Loc_maintask_1")
    UH.SpriteName(self.ButtonTask,"_Loc_mission_2")
end 

function MainTaskView:ClickTask()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.MainPart:SetActive(false)
    self.TaskPart:SetActive(true)

    UH.SpriteName(self.ButtonMain,"_Loc_maintask_2")
    UH.SpriteName(self.ButtonTask,"_Loc_mission_1")
end 

function MainTaskView:OnClickClose() 
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(MainTaskView)
end 
