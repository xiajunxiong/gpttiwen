MountainSeaQuestionView = MountainSeaQuestionView or DeclareView("MountainSeaQuestionView", "serveractivity/mountain_sea/mountain_sea_question", Mod.MountainSea.Question)
VIEW_DECLARE_LEVEL(MountainSeaQuestionView, ViewLevel.L1)
VIEW_DECLARE_MASK(MountainSeaQuestionView, ViewMask.BgBlock)

function MountainSeaQuestionView:MountainSeaQuestionView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_QUESTION, TimeType.Type_Special_4)
end

function MountainSeaQuestionView:OnClickClose() 
    ViewMgr:CloseView(MountainSeaQuestionView)
end

----------------------------MountainSeaQuestionPanel----------------------------
MountainSeaQuestionPanel = MountainSeaQuestionPanel or DeclareMono("MountainSeaQuestionPanel", UIWFlushPanel)
function MountainSeaQuestionPanel:MountainSeaQuestionPanel()
    self.Data = ServerActivityData.Instance
    self.data = self.Data.mountain_sea_question_data
    self.language = Language.MountainSeaQuestion
    self.question_data = {}
    self.is_first = true
    self.data_cares = {
        {data = self.data, func = self.FlushPanel, init_call = true},
        {data = self.Data.mountain_sea_question_answer, func = self.ShowIsTrue, init_call = false},
    }
    self:NormalShow()
end

function MountainSeaQuestionPanel:FlushPanel()
    self.RedPoint:SetNum(self.Data:MountainSeaQuestionRemind())
    UH.SetText(self.TxtTimes, Format(self.language.Times, self.data.times))
    if not self.data.is_can_answer and not self.is_first then
        PublicPopupCtrl.Instance:Center(self.language.QuestionTimeEnd)
        self.ObjBlock:SetActive(false)
        TimeHelper:CancelTimer(self.delay_handler)
        self.ObjShowPanel:SetActive(true)
    end
    local question_cfg = DataHelper.GetSearch(Config.time_limit_answer_auto.question_bank, "question_id", self.data.question_id)
    if not TableIsEmpty(question_cfg) then
        self.question_data.question = question_cfg.question
        self.question_data.answer_info = {}
        for i = 1, 3 do
            local t = {}
            t.index = i
            t.info = question_cfg
            table.insert(self.question_data.answer_info, t)
        end
    else
        self.question_data = {}
    end
    self.is_first = false
end

function MountainSeaQuestionPanel:NormalShow()
    local start_time = Config.time_limit_answer_auto.other[1].start_times
    local end_time = Config.time_limit_answer_auto.other[1].end_times
    local get_time = function(time)
        local hour = math.floor(time / 100)
        local min = time % 100
        if hour < 10 then
            hour = "0" .. hour
        end
        if min < 10 then
            min = "0" .. min
        end
        return hour, min
    end
    local start1, start2 = get_time(start_time)
    local end1, end2 = get_time(end_time)
    UH.SetText(self.TxtQuestionTime, Format(self.language.QuestionTime, start1, start2, end1, end2))
    local reward = self.Data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_QUESTION, Config.time_limit_answer_auto.reward_show)
    self.QuestionRewardList:SetData(DataHelper.FormatItemList(reward[1].show_item))
    self.ObjShowPanel:SetActive(true)
end

function MountainSeaQuestionPanel:ShowIsTrue()
    if self.Data.mountain_sea_question_answer.is_true then
        PublicPopupCtrl.Instance:Center(self.language.QuestionTip[2])
    else
        PublicPopupCtrl.Instance:Center(self.language.QuestionTip[1])
        local item = self.QuestionList:GetItem(self.Data.mountain_sea_question_answer.true_id)
        if item then
            item:ShowIsTrue(true)
        end
    end
    self.ObjBlock:SetActive(true)
    TimeHelper:CancelTimer(self.delay_handler)
    self.delay_handler = Invoke(function()
        self.ObjBlock:SetActive(false)
        if self.data.is_can_answer then
            if self.data.times > 0 then
                self:FlushQuestionPanel()
            else
                self.ObjShowPanel:SetActive(true)
            end
        else
            PublicPopupCtrl.Instance:Center(self.language.QuestionTimeEnd)
            self.ObjShowPanel:SetActive(true)
        end
    end, 2)
end

function MountainSeaQuestionPanel:OnStartClick()
    if self.data.is_can_answer then
        if self.data.times > 0 then
            self:FlushQuestionPanel()
            self.ObjShowPanel:SetActive(false)
        else
            PublicPopupCtrl.Instance:Center(self.language.TimesNotEnought)
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.QuestionTimeNotStart)
    end
end

function MountainSeaQuestionPanel:FlushQuestionPanel()
    self.QuestionList:SetData(self.question_data.answer_info or {})
    UH.SetText(self.TxtQuestion, self.question_data.question or "")
    local reward_cfg = DataHelper.GetSearch(Config.time_limit_answer_auto.reward_group, "type", self.data.reward_id)
    if not TableIsEmpty(reward_cfg) then
        local reward = {}
        for k, v in pairs(reward_cfg.reward_item) do
            table.insert(reward, v)
        end
        -- if reward_cfg.exp > 0 then
        --     table.insert(reward, {item_id = CommonItemId.Exp, is_bind = 1, num = reward_cfg.exp})
        -- end
        if reward_cfg.coin > 0 then
            table.insert(reward, {item_id = CommonItemId.Coin, is_bind = 1, num = reward_cfg.coin})
        end
        self.NowQuestionRewardList:SetData(DataHelper.FormatItemList(reward))
    end
end

function MountainSeaQuestionPanel:OnQuestionCloseClick()
    self.ObjShowPanel:SetActive(true)
end

function MountainSeaQuestionPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.delay_handler)
end

----------------------------MountainSeaQuestionItem----------------------------
MountainSeaQuestionItem = MountainSeaQuestionItem or DeclareMono("MountainSeaQuestionItem", UIWidgetBaseItem)
function MountainSeaQuestionItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.ObjTrue:SetActive(false)
    self.ObjFalse:SetActive(false)
    UH.SetText(self.TxtAnswer, Format(Language.MountainSeaQuestion.Answer, data.index, data.info["anwser_"..data.index]))
end

function MountainSeaQuestionItem:OnClickQuest()
    self:ShowIsTrue(self.data.info.answer_id == self.data.index)
    ServerActivityData.Instance.mountain_sea_question_answer.is_true = self.data.info.answer_id == self.data.index
    ServerActivityData.Instance.mountain_sea_question_answer.true_id = self.data.info.answer_id
    ServerActivityData.Instance.mountain_sea_question_answer:Notify()
    -- 这里需要发送题目id和回答的id过去校验
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_QUESTION, 1, self.data.info.question_id, self.data.index)
end

function MountainSeaQuestionItem:ShowIsTrue(value)
    self.ObjTrue:SetActive(value)
    self.ObjFalse:SetActive(not value)
end

function MountainSeaQuestionItem:OnDestroy()
    
end