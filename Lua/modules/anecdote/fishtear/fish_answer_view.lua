FishAnswerView = FishAnswerView or DeclareView("FishAnswerView","anecdote/fish_answer", Mod.Anecdote.FishAnswer)
function FishAnswerView:FishAnswerView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    -- self.AudioClose = CommonAudio.ItemInfoClose
end
function FishAnswerView:LoadCallback()
end

function FishAnswerView:CloseCallback()
end

function FishAnswerView:OnClickClose()
    local param_t = {
        content = AnerQuestionData.Instance:GetCloseDialog(),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(FishAnswerView)

                AnerQuestionData.Instance:FailCallBack()
            end 
        },
        cancel = {
            func = function()
                ViewMgr:CloseView(DialogTipsView)

                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
            end 
        }
    }

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

FishAnswerViewPanel = FishAnswerViewPanel or DeclareMono("FishAnswerViewPanel", UIWFlushPanel)

function FishAnswerViewPanel:FishAnswerViewPanel()
    self.data = AnerQuestionData.Instance
    self.data_cares = {
		{data = self.data:SMData(), func = self.OnProgressChanged,keys = {"progress"}},
    }
    self.wait_habdle = nil
    self.close_habdle = nil

    self.data:PanelEnd()

    UH.SetText(self.Title, self.data:GetQuestionTitle())
    UH.SetText(self.Desc, self.data:GetQuestionDesc())
    UH.SpriteName(self.pic,self.data:GetQuestionPic())
end 

function FishAnswerViewPanel:OnProgressChanged()
    local config = self.data:GetQuestConfig()
    UH.SetText(self.Question,config.question)
    for i = 1,4 do 
        UH.SetText(self.TextList[i],config["anwser_"..i])
    end 

    UH.SetText(self.Progress,config.order.."/3")

    self:HideAnswer()
end

function FishAnswerViewPanel:HideAnswer()
    for i = 1,4 do 
        self.FailList[i]:SetActive(false)
        self.CorrectList[i]:SetActive(false)
    end 
end

function FishAnswerViewPanel:ShowAnswer(choice)
    self.is_showing = true
    local config = self.data:GetQuestConfig()
    for i = 1,4 do 
        self.CorrectList[i]:SetActive( i == config.answer_id )
        self.FailList[i]:SetActive(choice == i and choice ~= config.answer_id)
    end 
end

function FishAnswerViewPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:DestroyCallBack()
end 

function FishAnswerViewPanel:OnClickCheck(index)
    if self.is_showing then return end  

    local answer_pop = self.data:GetIsCorrect(index) and Language.Anecdote.FishTearCorrect or Language.Anecdote.FishTearFail
    PublicPopupCtrl.Instance:Center(answer_pop)

    self:ShowAnswer(index)

    if self.wait_habdle ~= nil then
        TimeHelper:CancelTimer(self.wait_habdle)
        self.wait_habdle = nil
    end    

    if self.close_habdle ~= nil then
        TimeHelper:CancelTimer(self.close_habdle)
        self.close_habdle = nil
    end    


    if self.data:GetIsCorrect(index) then 
        if self.data:IsProgressEnd() then 
            ViewMgr:CloseView(FishAnswerView)
            
            CutChapterCtrl.Instance:OpenCutChapter(self.data:GetMainConfig().other[1].success,function()
                self.close_habdle = TimeHelper:AddDelayTimer(function()
                    AnecdoteCtrl.Instance:SendTaskAction(self.data:GetSendType())
                end,0.5)
            end)
        else 
            self.wait_habdle = TimeHelper:AddDelayTimer(function()
                self.wait_habdle = nil
                self.is_showing = false
                self.data:ProgressPlus()
            end,3)
        end 

        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
    else
        self.close_habdle = TimeHelper:AddDelayTimer(function()
            self.close_habdle = nil
            CutChapterCtrl.Instance:OpenCutChapter(self.data:GetMainConfig().other[1].fail,function()
                ViewMgr:CloseView(FishAnswerView)

                AnerQuestionData.Instance:FailCallBack()
            end)
        end,0.5)

        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end 
end 
