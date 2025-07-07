FunRiddleView = FunRiddleView or DeclareView("FunRiddleView","anecdote/fun_riddle",Mod.Anecdote.FunRiddle)
-- VIEW_DECLARE_LEVEL(MusicStationView, ViewLevel.L0)
VIEW_DECLARE_MASK(FunRiddleView, ViewMask.BgBlock)
function FunRiddleView:LoadCallback()
    FunRiddleData.Instance:GameStart()
end

FunRiddlePanel = FunRiddlePanel or DeclareMono("FunRiddlePanel", UIWFlushPanel)
function FunRiddlePanel:FunRiddlePanel()
    self.data = FunRiddleData.Instance

    self.data_cares = {
        {data = self.data.operate, func = self.FlushPanel,keys = {"flush"},init_call = false},
        {data = self.data.operate, func = self.FlushAnswer,keys = {"answer"},init_call = false},
    }

    -- TxtQuestion
    -- TxtResult
    -- AnswerList
    -- ObjBlock

    self.type = NewAnecdoteType.QWCM    -- 新奇闻类型，当触发新奇闻时，走新奇闻逻辑
    local param = AnecdoteData.Instance:GetNewParam(self.type, false)
    self.is_new = not TableIsEmpty(param)
    self.param = param
end

function FunRiddlePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.show_timer ~= nil then
        TimeHelper:CancelTimer(self.show_timer)
        self.show_timer = nil
    end

    
end

function FunRiddlePanel:OnClickClose()
    if self.is_new then
        ViewMgr:CloseView(FunRiddleView)
        return
    end
    PublicPopupCtrl.Instance:DialogTips{
        content = Language.Anecdote.FunRiddleExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(FunRiddleView)
			end
		}
    }
end

function FunRiddlePanel:FlushPanel()
    local q_show = self.data:CatchQuestionInfo()
    UH.SetText(self.TxtQuestion,q_show.question)
    self.AnswerList:SetData(q_show.answer_list)
end

function FunRiddlePanel:FlushResult(is_right)
    self.ObjBlock:SetActive(true)

    TimeHelper:CancelTimer(self.show_timer)
    self.show_timer = TimeHelper:AddDelayTimer(function()
        if self.data.operate.complete == 3 then
            ViewMgr:CloseView(FunRiddleView)
            -- 成功
            if self.is_new then
                -- 新奇闻
                local seq = self.param.seq
                AnecdoteData.Instance:NewAnecdoteSucc(seq)
            else
                AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.FunRiddle)
            end
        elseif is_right then 
            self.data:NextQuestion()
        else 
            ViewMgr:CloseView(FunRiddleView)
            if not self.is_new then
                NpcDialogCtrl.Instance:NpcDialogView({talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.FunRiddle)})
            end
        end
        self.ObjBlock:SetActive(false)
    end,3)
end

function FunRiddlePanel:OnAnswerClick(data)
    self.data:FlushAnswer(data.index) 
end

function FunRiddlePanel:FlushAnswer()
    if self.data.operate.answer == 0 then 
        -- 归0 全部隐藏
        for k,v in pairs(self.AnswerList.item_list) do
            v:ShowAnswer(false)
        end
    else 
        -- 有答案时展示答案
        local result,right_index= self.data:CheckIsRight()
        for k,v in pairs(self.AnswerList.item_list) do
            local data = v:GetData()
            v:ShowAnswer(data.index == self.data.operate.answer or data.index == right_index)
        end

        self:FlushResult(result)
        if result then 
            PublicPopupCtrl.Instance:Center(Language.FunRiddle.RightAnswer)
        else
            PublicPopupCtrl.Instance:Center(Language.FunRiddle.WrongAnswer)
        end 
    end 
end

FunRiddleAnswerItem = FunRiddleAnswerItem or DeclareMono("FunRiddleAnswerItem", UIWidgetBaseItem)
function FunRiddleAnswerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.TxtNum, DataHelper.GetDaXie(data.index, true))
    UH.SetText(self.TxtAnswer1,data.str)
    UH.SetText(self.TxtAnswer2,data.str)
    UH.SpriteName(self.IconRight, data.is_right and "DuiGou" or "Cha")
    self.ObjAnswer:SetActive(false)
    self.ObjRight:SetActive(false)
end 

function FunRiddleAnswerItem:ShowAnswer(flag)
    self.ObjAnswer:SetActive(flag)
    self.ObjRight:SetActive(flag)
end