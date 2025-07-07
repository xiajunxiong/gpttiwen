WeddingAnswerView = WeddingAnswerView or DeclareView("WeddingAnswerView","anecdote/wedding_fb_answer", Mod.Love.Answer)
function WeddingAnswerView:WeddingAnswerView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    -- self.AudioClose = CommonAudio.ItemInfoClose
end
function WeddingAnswerView:LoadCallback()
    if WeddingAnswerData.Instance:IsComplete() then 
        PublicPopupCtrl.Instance:Center(Language.Anecdote.WeddingAnswerComplete)
        ViewMgr:CloseView(WeddingAnswerView)
    end 
end

function WeddingAnswerView:CloseCallback()
end

function WeddingAnswerView:OnClickClose()
    local param_t = {
        content = Language.Anecdote.WeddingAnswerCloseDialog,
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(WeddingAnswerView)
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

-- 本答题不绑定任何任务，故采用独立的data来处理逻辑
WeddingAnswerViewPanel = WeddingAnswerViewPanel or DeclareMono("WeddingAnswerViewPanel", UIWFlushPanel)
function WeddingAnswerViewPanel:WeddingAnswerViewPanel()
    self.data = WeddingAnswerData.Instance
    self.data_cares = {
		{data = self.data:SMData(), func = self.OnProgressChanged,keys = {"progress"}},
    }
    self.wait_habdle = nil
    self.close_habdle = nil

    UH.SetText(self.Title, Language.Anecdote.WeddingQueTitle)
    UH.SetText(self.Desc, Language.Anecdote.WeddingQueDesc)
end 

function WeddingAnswerViewPanel:OnProgressChanged()
    local config = self.data:GetQuestConfig()
    -- LogError("?!?!",config)
    UH.SetText(self.Question,config.question)
    for i = 1,4 do 
        UH.SetText(self.TextList[i],config["anwser_"..i])
    end 

    UH.SetText(self.Progress,self.data:GetCurProgress().."/"..self.data:GetProgressLegth())

    self:HideAnswer()
end

function WeddingAnswerViewPanel:HideAnswer()
    for i = 1,4 do 
        self.FailList[i]:SetActive(false)
        self.CorrectList[i]:SetActive(false)
    end 
end

function WeddingAnswerViewPanel:ShowAnswer(choice)
    self.is_showing = true
    local config = self.data:GetQuestConfig()
    for i = 1,4 do 
        self.CorrectList[i]:SetActive( i == config.answer_id )
        self.FailList[i]:SetActive(choice == i and choice ~= config.answer_id)
    end 
end

function WeddingAnswerViewPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:DestroyCallBack()
end 

function WeddingAnswerViewPanel:OnClickCheck(index)
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
    -- if self.data:GetIsCorrect(index) then 
        -- 完成答题则请求领奖    
        local param_t = {
            req_type = MarryConfig.FBReqType.sub_act,
            param1 = MarryConfig.FBSubActType.zao_sheng_gui_zi,
            param2 = self.data:GetQuestConfig().question_id-1,
        }
        MarryCtrl.Instance:SendWeddingFBReq(param_t)
    -- end 
    -- if self.data:IsProgressEnd() then 
        self.wait_habdle = TimeHelper:AddDelayTimer(function()
            ViewMgr:CloseView(WeddingAnswerView)
        end,3)
    -- else 
    --     self.wait_habdle = TimeHelper:AddDelayTimer(function()
    --         self.wait_habdle = nil
    --         self.is_showing = false
    --         self.data:ProgressPlus()
    --     end,3)
    -- end 
    if self.data:IsProgressEnd() then 
        MarryCtrl.Instance:PlayAnswer(2)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
end 
