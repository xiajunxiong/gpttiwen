
MarryPartyAnswerView = MarryPartyAnswerView or DeclareView("MarryPartyAnswerView", MarryPartyConfig.ResPath .. "marry_party_answer")
function MarryPartyAnswerView:MarryPartyAnswerView()
end

function MarryPartyAnswerView:OnClickClose()
    -- local param_t = {
    --     content = Language.Anecdote.WeddingAnswerCloseDialog,
    --     confirm = {
    --         func =  function()
    --             ViewMgr:CloseView(DialogTipsView)
    --             ViewMgr:CloseView(WeddingAnswerView)
    --         end 
    --     },
    --     cancel = {
    --         func = function()
    --             ViewMgr:CloseView(DialogTipsView)

    --             AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
    --         end 
    --     }
    -- }

    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    -- PublicPopupCtrl.Instance:DialogTips(param_t)
    MarryPartyCtrl.Instance:SendQingYuanShengHuiQuestionClose()
end

MarryPartyAnswerViewF = MarryPartyAnswerViewF or DeclareMono("MarryPartyAnswerViewF", UIWFlushPanel)
function MarryPartyAnswerViewF:MarryPartyAnswerViewF()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Answer

    self.data_cares = {
		{data = self.data.marry_party_question_info.info, func = self.FlushShow},
    }

    MarryPartyAnswerViewF.CanAnswer = true
end

function MarryPartyAnswerViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_ct)
end

function MarryPartyAnswerViewF:FlushShow()
    MarryPartyAnswerViewF.CanAnswer = true

    local info = self.data.marry_party_question_info.info
    if info.init then return end
    local co = Cfg.LoveMeetingAnswerById(info.question_id)
    if co then
        self.TitleShow.text = string.format(self.language.TitleShow, DataHelper.GetDaXie(info.quest_num))
        self.QuestionShow.text = co.question
        self.ProgressShow.text = string.format(self.language.ProgressShow, info.quest_num, self.data:ConfigOtherQuestionMount())

        for i = 1, self.ShowOptions:Length() do
            self.ShowOptions[i]:SetData(co)
        end
    end

    TimeHelper:CancelTimer(self.timer_ct)
    if info.end_time - TimeHelper.GetServerTime() > 0 then
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local last_time = info.end_time - TimeHelper.GetServerTime()
            self.TimeShow.text = string.format(self.language.TimeShow, math.floor(last_time))
        end, function ()
            self.TimeShow.text = string.format(self.language.TimeShow, 0)
        end, info.end_time)
    else
        self.TimeShow.text = string.format(self.language.TimeShow, 0)
    end
end

MarryPartyAnswerViewItem = MarryPartyAnswerViewItem or DeclareMono("MarryPartyAnswerViewItem", UIWidgetBaseItem)
function MarryPartyAnswerViewItem:MarryPartyAnswerViewItem()
    self.marry_data = MarryPartyCtrl.Instance:Data()
    self.langugae = Language.MarryParty.Answer
end

-- function MarryPartyAnswerViewItem:OnDestroy()
--     self:uncareData()
-- end

function MarryPartyAnswerViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    -- self:uncareData()
    -- self.care_handles = {}
    -- self:careData(self.marry_data.marry_party_question_info.result, BindTool.Bind(self.ShowResult, self))

    self.OptionShow.text = data["anwser_" .. self.ShowIndex] or ""
    self.GouObj:SetActive(false)
end

function MarryPartyAnswerViewItem:OnClickAnswer()
    if MarryPartyAnswerViewF.CanAnswer then
        MarryPartyAnswerViewF.CanAnswer = false
        self.GouObj:SetActive(true)
        MarryPartyCtrl.Instance:SendQingYuanShengHuiQuestionAnswer(self.ShowIndex)
    else
        PublicPopupCtrl.Instance:Center(self.langugae.ResultWaiting)
    end
end