SummerMazeQuestionView = SummerMazeQuestionView or DeclareView("SummerMazeQuestionView", "serveractivity/summer_first/summer_maze_question")
VIEW_DECLARE_LEVEL(SummerMazeQuestionView, ViewLevel.L2)
VIEW_DECLARE_MASK(SummerMazeQuestionView, ViewMask.BgBlock)

function SummerMazeQuestionView:OnClickClose() 
    ViewMgr:CloseView(SummerMazeQuestionView)
end

SummerMazeQuestionPanel = SummerMazeQuestionPanel or DeclareMono("SummerMazeQuestionPanel", UIWFlushPanel)
function SummerMazeQuestionPanel:SummerMazeQuestionPanel()
    self.Data = SummerMazeData.Instance
    self.is_first = true
    self.data_cares = {
        {data = self.Data.question_info, func = self.FlushPanel, init_call = true},
    }
end 

function SummerMazeQuestionPanel:FlushPanel()
    if self.is_first then
        self.is_first = false
        self:FlushAnswer()
    else
        -- 答错后2秒刷新新题目
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.QuestionTip[1])
        TimeHelper:CancelTimer(self.delay_handler)
        self.delay_handler = TimeHelper:AddDelayTimer(function()
            self:FlushAnswer()
            self.ObjBlock:SetActive(false)
        end, 2)
    end
    
end

function SummerMazeQuestionPanel:FlushAnswer()
    local cfg = AnswerData.Instance:GetDataById(self.Data.question_info.id)
    UH.SetText(self.Quest, cfg.question)
    for i = 1,4 do 
        self.QuestList[i]:SetData(cfg)
    end
end

function SummerMazeQuestionPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.delay_handler)
end

----------------------------SummerMazeQuestionItem----------------------------
SummerMazeQuestionItem = SummerMazeQuestionItem or DeclareMono("SummerMazeQuestionItem", UIWidgetBaseItem)
function SummerMazeQuestionItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.answer.gameObject:SetActive(false)
    UH.SetText(self.select, data["anwser_"..self.index])
end 

function SummerMazeQuestionItem:OnClickQuest()
    -- 答对后延迟2秒关闭界面
    if self.data.answer_id == self.index then
        PublicPopupCtrl.Instance:Center(Language.SummerMaze.QuestionTip[2])
        TimeHelper:CancelTimer(self.delay_handler)
        self.delay_handler = TimeHelper:AddDelayTimer(function()
            ViewMgr:CloseView(SummerMazeQuestionView)
        end, 2)
    end
    self:FlushAnswer()
    self.ObjBlock:SetActive(true)
    SummerMazeData.Instance:SendInfo(6, self.index)
end 

function SummerMazeQuestionItem:FlushAnswer()
    self.answer.gameObject:SetActive(true)
    UH.SpriteName(self.answer, self.data.answer_id == self.index and "Correct" or "Fail")
end

function SummerMazeQuestionItem:OnDestroy()
    TimeHelper:CancelTimer(self.delay_handler)
end