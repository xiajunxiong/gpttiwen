AnerQuestionData = AnerQuestionData or BaseClass()

function AnerQuestionData:__init()
    if AnerQuestionData.Instance ~= nil then
		AnerQuestionData.LogError("[AnerQuestionData] attempt to create singleton twice!")
		return
	end
    AnerQuestionData.Instance = self  
	
	self.fish_data = FishAnswerData.New()
	self.children_data = ChildrenAnswerData.New()
	self.wedding_data = WeddingAnswerData.New()
    self.sm_data = SmartData.New({progress = 1})

	self.config = {
		["fish"] = self.fish_data.config,
		["children"] = self.children_data.config,
	}
end

function AnerQuestionData:__delete()
	if AnerQuestionData.Instance == self then
		AnerQuestionData.Instance = nil
	end	
end

function AnerQuestionData:SMData()
	return self.sm_data
end

function AnerQuestionData:GetIsCorrect(choice)
    local question_config = self.config[self.operate_type].question[self.sm_data.progress]
    return choice == question_config.answer_id
end

function AnerQuestionData:GetQuestConfig()
	if self.operate_type == nil then return self.config["fish"].question[self.sm_data.progress] end 
    return self.config[self.operate_type].question[self.sm_data.progress]
end

function AnerQuestionData:GetMainConfig(  )
	if self.operate_type == nil then return self.config["fish"] end 
    return self.config[self.operate_type]
end

function AnerQuestionData:GetQuestionTitle()
	if self.operate_type == "fish" then 
		return Language.Anecdote.FishTearTitle
	elseif self.operate_type == "children" then 
		return Language.Anecdote.ChildrenQueTitle
	end 

	return Language.Anecdote.FishTear
end

function AnerQuestionData:GetQuestionDesc()
	if self.operate_type == "fish" then 
		return Language.Anecdote.FishTearDesc
	elseif self.operate_type == "children" then 
		return Language.Anecdote.ChildrenQueDesc
	end 

	return Language.Anecdote.FishTear
end

function AnerQuestionData:GetQuestionPic()
	return self.operate_type
end


function AnerQuestionData:ProgressPlus()
    self.sm_data.progress = self.sm_data.progress + 1
end

function AnerQuestionData:GetSendType()
	if self.operate_type == "fish" then 
		return AnecdoteType.FishTear
	elseif self.operate_type == "children" then 
		return AnecdoteType.QuestionTongBao
	end 

	return AnecdoteType.FishTear
end 

function AnerQuestionData:IsProgressEnd()
    if self.sm_data.progress == 3 then 
        return true
    else 
        return false
    end 
end

function AnerQuestionData:ProgressClear()
    self.sm_data.progress = 1
end

function AnerQuestionData:SetCurTask(task_id)
	self.operate_type = self:CheckAnswerType(task_id)

	if self.operate_type == "fish" then 
		self.fish_data:SetCurTask(task_id)
	elseif self.operate_type == "children" then 

	end 
end

function AnerQuestionData:GetCloseDialog()
	if self.operate_type == "fish" then 
		return Language.Anecdote.FishAnswerCloseDialog
	elseif self.operate_type == "children" then 
		return Language.Anecdote.ChildrenAnswerCloseDialog
	end 
end

function AnerQuestionData:ClearOperateType()
	self.operate_type = nil
end

function AnerQuestionData:CheckAnswerType(task_id)
	for k,v in pairs(FishAnswerData.TaskTag) do 
		if task_id == v then 
			return "fish"
		end 
	end 

	for k,v in pairs(ChildrenAnswerData.TaskTag) do 
		if task_id == v then 
			return "children"
		end 
	end 

	return "nil"
end

function AnerQuestionData:ClearData()
    self.fish_data:ClearData()
end

function AnerQuestionData:DestroyCallBack()
	self:ProgressClear()
	if self.operate_type == "fish" then 
	    self.fish_data:CancelFishEffect()
	end 
end

function AnerQuestionData:PanelEnd()
	if self.operate_type == "fish" then 
	    self.fish_data:IronyFinish(false)
	elseif self.operate_type == "children" then  

	end 
end 

function AnerQuestionData:FailCallBack()
	if self.operate_type == "fish" then 
		
	elseif self.operate_type == "children" then
		local flag = AnecdoteData.GetConfig(AnecdoteType.QuestionTongBao)
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = flag.fail_talk})
	end 
end