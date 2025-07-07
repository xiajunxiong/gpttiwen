
ImmortalRecruitStudentView = ImmortalRecruitStudentView or DeclareView("ImmortalRecruitStudentView", "anecdote/immortal_recruit_student", Mod.Anecdote.ImmortalRecruitStudent)

function ImmortalRecruitStudentView:ImmortalRecruitStudentView()
	self.data = ImmortalRecruitStudentData.Instance
end

function ImmortalRecruitStudentView:OnClickClose()
    local state = self.data.answer_show.state
    if 0 == state then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Anecdote.ImmortalRecruitStudent,
            confirm = {
                func = function ()
                    ViewMgr:CloseView(ImmortalRecruitStudentView)
                    ViewMgr:CloseView(DialogTipsView)
                    AnecdoteCtrl.Instance:StrangerTaskFailTalk(AnecdoteType.ImmortalRecruitStudent)
                end
            }
        }
    else
        ViewMgr:CloseView(ImmortalRecruitStudentView)
        AnecdoteCtrl.Instance:StrangerTaskFailTalk(AnecdoteType.ImmortalRecruitStudent)
    end
end

ImmortalRecruitStudentViewF = ImmortalRecruitStudentViewF or DeclareMono("ImmortalRecruitStudentViewF", UIWFlushPanel)

function ImmortalRecruitStudentViewF:ImmortalRecruitStudentViewF()
    self.data = ImmortalRecruitStudentData.Instance
    self.language = Language.ImmortalRecruitStudent

    self.data:ResetAnswerShow()

    self.data_cares = {
		{data = self.data.answer_show, func = self.FlushState, keys = {"state"}},
		{data = self.data.answer_show, func = self.FlushAnswerNum, keys = {"answer_num"}},
		{data = self.data.answer_show, func = self.FlushResults, keys = {"flush_results"}},
	}
end

function ImmortalRecruitStudentViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushAnswers()
    self:FlushSelections()
end

function ImmortalRecruitStudentViewF:FlushAnswers()
    local selections = self.data:GetSelections()
    for i = 1, self.Answers:Length() do
        self.Answers[i]:SetData(i)
    end
end

function ImmortalRecruitStudentViewF:FlushSelections()
    local selections = self.data:GetSelections()
    for i = 1, self.Selections:Length() do
        self.Selections[i]:SetData(selections[i])
    end
end

function ImmortalRecruitStudentViewF:FlushResults()
    local results = self.data.answer_show.results
    self.ResultList:SetData(results)
end

function ImmortalRecruitStudentViewF:FlushState()
    local state = self.data.answer_show.state
    self.ResultObj:SetActive(state > 0)
    self.SuccObj:SetActive(2 == state)
    self.FailObj:SetActive(1 == state)
    self.Result.text = state > 0 and string.format(self.language.ResultShow, self.data.NumShow(self.data.answer_show.result))  or ""
    -- if 2 == state then AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.ImmortalRecruitStudent) end
end

function ImmortalRecruitStudentViewF:FlushAnswerNum()
    self.AnswerNum.text = string.format(self.language.AnswerNum, self.data.answer_show.answer_num)
end

function ImmortalRecruitStudentViewF:OnClickConfirm()
    if 0 == self.data.answer_show.answer_num then
        PublicPopupCtrl.Instance:Center(self.language.FailTips)
        return
    end
    self.data:ResultConfirm()
end

function ImmortalRecruitStudentViewF:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[138].desc})
end

function ImmortalRecruitStudentViewF:OnClickClose()
    local state = self.data.answer_show.state
    if 2 == state then
        ViewMgr:CloseView(ImmortalRecruitStudentView)
        AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.ImmortalRecruitStudent)
    else
        self.ResultObj:SetActive(false)
    end
end

ImmortalRecruitStudentViewBtnItemA = DeclareMono("ImmortalRecruitStudentViewBtnItemA", UIWidgetBaseItem)

function ImmortalRecruitStudentViewBtnItemA:ImmortalRecruitStudentViewBtnItemA()
    self.irs_data = ImmortalRecruitStudentData.Instance
    self.index = 1
end

function ImmortalRecruitStudentViewBtnItemA:OnDestroy()
    self:uncareData()
end

function ImmortalRecruitStudentViewBtnItemA:SetData(index)
    self.index = index

    self:uncareData()
    self.care_handles = {}

    self:careData(self.irs_data.answer_show, BindTool.Bind(self.FlushSelShow, self), "sel_seq")
    self:careData(self.irs_data.answer_show.selections, BindTool.Bind(self.FlushNum, self))
end

function ImmortalRecruitStudentViewBtnItemA:FlushSelShow()
    self.SelObj:SetActive(self.irs_data.answer_show.sel_seq == self.index)
end

function ImmortalRecruitStudentViewBtnItemA:FlushNum()
    local num = self.irs_data.answer_show.selections[self.index]
    local num_show = num > -1
    self.Num.text = num_show and DataHelper.GetDaXie(num, true) or ""
    self.Num.color = num_show and COLORS.Blue4 or COLORS.Yellow9
    self.BgSp.SpriteName = num_show and "WenZiDiXuanZe" or "WenZiDiWeiXuanZe"
end

function ImmortalRecruitStudentViewBtnItemA:OnClickItem()
    if 0 == self.irs_data.answer_show.answer_num then return end
    self.irs_data:SelectAnswer(self.index)
end

ImmortalRecruitStudentViewBtnItemS = DeclareMono("ImmortalRecruitStudentViewBtnItemS", UIWidgetBaseItem)

function ImmortalRecruitStudentViewBtnItemS:ImmortalRecruitStudentViewBtnItemS()
    self.irs_data = ImmortalRecruitStudentData.Instance
end

function ImmortalRecruitStudentViewBtnItemS:OnDestroy()
    self:uncareData()
end

function ImmortalRecruitStudentViewBtnItemS:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}
    self:careData(self.irs_data.answer_show.selections, BindTool.Bind(self.FlushSelShow, self))

    self.NumShow.text = data.num_show
end

function ImmortalRecruitStudentViewBtnItemS:FlushSelShow()
    local is_sel = self.irs_data:IsNumSelected(self.data.num)
    self.BgSp.SpriteName = is_sel and "WenZiDiXuanZe" or "WenZiDiWeiXuanZe"
    self.NumShow.color = is_sel and COLORS.Blue4 or COLORS.Yellow9
end

function ImmortalRecruitStudentViewBtnItemS:OnClickItem()
    if self.irs_data:IsNumSelected(self.data.num) or 0 == self.irs_data.answer_show.answer_num then return end
    self.irs_data:SelectNum(self.data.num)
end

ImmortalRecruitStudentViewBtnItemR = DeclareMono("ImmortalRecruitStudentViewBtnItemR", UIWidgetBaseItem)

function ImmortalRecruitStudentViewBtnItemR:ImmortalRecruitStudentViewBtnItemR()
    self.irs_data = ImmortalRecruitStudentData.Instance
end

function ImmortalRecruitStudentViewBtnItemR:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Answer.text = self.irs_data.NumShow(data.answer)
    self.AllRight.text = data.all_right
    self.NumRight.text = data.num_right
end
