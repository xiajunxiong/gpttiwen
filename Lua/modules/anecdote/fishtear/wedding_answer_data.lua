WeddingAnswerData = WeddingAnswerData or BaseClass()

function WeddingAnswerData:__init()
	if WeddingAnswerData.Instance ~= nil then
		WeddingAnswerData.LogError("[WeddingAnswerData] attempt to create singleton twice!")
		return
	end
    WeddingAnswerData.Instance = self

    self.config = Config.get_married_auto.answer_questions

	self.sm_data = SmartData.New({progress = 1})
end 

function WeddingAnswerData:GetQuestConfig()
    return self.config[self.sm_data.progress]
end 

function  WeddingAnswerData:GetProgressLegth()
	return #self.config
end 

function WeddingAnswerData:SMData()
	return self.sm_data
end

function WeddingAnswerData:ProgressClear()
    self.sm_data.progress = 1
end

function WeddingAnswerData:DestroyCallBack()
	self:ProgressClear()
end 

function WeddingAnswerData:ProgressPlus(index)
	self.sm_data.progress = index == 0 and 1 or index  -- self.sm_data.progress + 1
end 

function WeddingAnswerData:GetIsCorrect(choice)
    local question_config = self:GetQuestConfig()
    return choice == question_config.answer_id
end 

function WeddingAnswerData:IsProgressEnd()
    local is_done = true
    for i = 1,4 do 
        local list = bit:d2b(MarryData.Instance.wedding_fb_zsgz_sm.fetch_flag)
        if list[33-self.sm_data.progress] == 0 then 
            is_done = false
        end 
    end 

    return is_done
end 

function WeddingAnswerData:IsComplete()
    local list = bit:d2b(MarryData.Instance.wedding_fb_zsgz_sm.fetch_flag)
    return list[33-self.sm_data.progress] == 1
end

function WeddingAnswerData:GetCurProgress()
    local list = bit:d2b(MarryData.Instance.wedding_fb_zsgz_sm.fetch_flag)
    local progress = 0
    for i = 1,4 do 
        if list[33-i] == 1 then 
            progress = progress + 1
        end 
    end 

    return progress+1
end 