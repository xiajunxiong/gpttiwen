SplaceAnecQuestView = SplaceAnecQuestView or DeclareView("SplaceAnecQuestView","serveractivity/splace_anec_quest_view")
function SplaceAnecQuestView:OnClickClose() 
    ViewMgr:CloseView(SplaceAnecQuestView)
    if TeamCtrl.Instance:Data():IsLeader() then
        ServerActivityCtrl.Instance:SendCloseWindosReq({reason = 1})
    end 
end

SplaceAnecQuestPanel = SplaceAnecQuestPanel or DeclareMono("SplaceAnecQuestPanel", UIWFlushPanel)
function SplaceAnecQuestPanel:SplaceAnecQuestPanel()
    self.data = SplaceAnecdoteData.Instance

    self.data_cares = {
        {data = self.data.quest_info, func = self.FlushPanel},
    }
end 

function SplaceAnecQuestPanel:FlushPanel() 
    local cfg = AnswerData.Instance:GetDataById(self.data.quest_info.question_id)

    UH.SetText(self.quest,cfg.question)

    for i = 1,4 do 
        self.quest_list[i]:SetData(cfg)
        if self.data.quest_info.answer_index >0 then 
            self.quest_list[self.data.quest_info.answer_index]:FlushAnswer(self.data.quest_info)
        end 
    end 
end 

----------------------------SplaceQuestItem----------------------------
SplaceQuestItem = SplaceQuestItem or DeclareMono("SplaceQuestItem", UIWidgetBaseItem)
function SplaceQuestItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.select,data["anwser_"..self.index])
end 

function SplaceQuestItem:OnClickQuest()
    if SplaceAnecdoteData.Instance.quest_info.is_true > 0 then 
        PublicPopupCtrl.Instance:Center(Language.SplaceAnecdote.QuestComplete)
        return 
    end 
    if SceneData.Instance:GetMode() == SCENE_TYPE.SOULS_ANECDOTE then
        SoulsAnecdoteCtrl.Instance:SendSplaceQuestionReq({
            answer_id = self.index,
            npc_obj_id = SplaceAnecdoteData.Instance.quest_info.npc_id,
        })
    else
        ServerActivityCtrl.Instance:SendSplaceQuestionReq({
            answer_id = self.index,
            npc_obj_id = SplaceAnecdoteData.Instance.quest_info.npc_id,
        })
    end

    if TeamCtrl.Instance:Data():InTeam() and not TeamCtrl.Instance:Data():IsLeader() then
        PublicPopupCtrl.Instance:Center(ErrorText[57])
    end
end 

function SplaceQuestItem:FlushAnswer(info)
    if info.answer_index > 0 then 
        self.answer.gameObject:SetActive(true)
        UH.SpriteName(self.answer, info.is_true == 1 and "correct" or "fail" )
    end 
end 