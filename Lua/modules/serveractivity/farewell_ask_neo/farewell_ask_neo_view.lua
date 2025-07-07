FarewellAskNeoView = FarewellAskNeoView or DeclareView("FarewellAskNeoView","serveractivity/farewell_ask_neo_view",Mod.SpringFestival.FarewellAskNeo)
function FarewellAskNeoView:OnClickClose() ViewMgr:CloseView(FarewellAskNeoView) end

FarewellAskNeoPanel = FarewellAskNeoPanel or DeclareMono("FarewellAskNeoPanel", UIWFlushPanel)
function FarewellAskNeoPanel:FarewellAskNeoPanel()
    self.data = FarewellAskNeoCtrl.Instance
    self.data_cares = { {data = self.data.farewell_info, func = self.FlushPanel},}
end 

function FarewellAskNeoPanel:OnDestroy( )
    if self.timer ~= nil then 
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end 
end

function FarewellAskNeoPanel:FlushPanel()
    local quest_info = self.data:CatchQuestInfo()
    UH.SetText(self.QuesTitle,quest_info.question_title)
    for i = 1,3 do 
        self.BtnList[i]:SetData(quest_info.answer_list[i])
    end 

    TimeHelper:CancelTimer(self.timer)
    self.timer = TimeHelper:AddDelayFrameTimer(function ()
        if self.click ~= nil and self.BtnList[self.click].Interactor.Interactable then 
            local sp = self.click == quest_info.answer_id and Language.FarewellAskNeo.AnswerRight or Language.FarewellAskNeo.AnswerFalse
            PublicPopupCtrl.Instance:Center(sp)
        end 
        if quest_info.today_answer == 1 and self.BtnList[quest_info.answer_id].Interactor.Interactable then
            PublicPopupCtrl.Instance:Center(Language.FarewellAskNeo.HasFinish)
            self.BtnList[quest_info.answer_id]:ShowAnswer()
            for i =1,3 do self.BtnList[i].Interactor.Interactable = false end 
        end
    end)

    for k,v in pairs(quest_info.has_answer_list) do 
        if self.BtnList[v] ~= nil then 
            self.BtnList[v]:ShowAnswer()
        end 
    end 

    self.RewardList:SetData(quest_info.reward_item)
end

function FarewellAskNeoPanel:OnClickItem(data)
    local quest_info = self.data:CatchQuestInfo()
    if quest_info.today_answer == 1 then return end 

    self.click = data.index
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FAREWELL_ASK_NEO,1,data.index)
end

function FarewellAskNeoPanel:AutoClose(time)
    if self.CloseTimer ~= nil then 
        TimeHelper:CancelTimer(self.CloseTimer)
        self.CloseTimer = nil
    end 
    self.CloseTimer = TimeHelper:AddDelayTimer(function()
        ViewMgr:CloseView(FarewellAskNeoView)
    end, time)
end
---------------------------------FarewellAskItem-------------------------------
FarewellAskItem = FarewellAskItem or DeclareMono("FarewellAskItem", UIWidgetBaseItem)
function FarewellAskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Str,data.str)
    local sp = data.index == data.answer and "mid_true" or "mid_false"
    UH.SpriteName(self.Answered,sp)

    self.Interactor.Interactable = true
end

function FarewellAskItem:ShowAnswer()
    self.Answered.gameObject:SetActive(true)
    self.Interactor.Interactable = false
    self.Selected:SetActive(true)
end