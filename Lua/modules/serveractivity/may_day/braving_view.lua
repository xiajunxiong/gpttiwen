BravingView = BravingView or DeclareView("BravingView", "serveractivity/may_day/braving", Mod.MayDay.Braving)
VIEW_DECLARE_LEVEL(BravingView, ViewLevel.L1)
VIEW_DECLARE_MASK(BravingView, ViewMask.BgBlock)
function BravingView:BravingView()
    self.Data = BravingData.Instance
    self.Board:SetData(self:GetType(), "_LocChengFengPoLang")
    self.language = Language.Braving
end

function BravingView:LoadCallback()
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_BRAVING)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_BRAVING) - 1)
        UH.SetText(self.TxtActTime, Format(self.language.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.TxtActTime, Format(self.language.ActTime, 1, 1, 1, 1))
    end

    UH.SetText(self.TxtDesc, Format(self.language.Desc, self.Data:Other().time))
end

function BravingView:CloseCallback()

end

----------------------------BravingPanel----------------------------
BravingPanel = BravingPanel or DeclareMono("BravingPanel", UIWFlushPanel)
function BravingPanel:BravingPanel()
    self.Data = BravingData.Instance
    self.language = Language.Braving
    self.data = self.Data.info_data
    self.other = self.Data:Other()
    self.Data:SetTodayRemind()
    self.need_delay = false
    self.is_first = true
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = true},
    }
end

function BravingPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ObjStartShow:SetActive(true)
    self.RewardList:SetData(DataHelper.FormatItemList(self.Data:ShowReward().reward_item))
    self.is_first = false
end

function BravingPanel:FlushAll()
    if not self.need_delay then
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BRAVING) then
            PublicPopupCtrl.Instance:Center(self.language.ActTimeTip[1])
            ViewMgr:CloseView(BravingView)
            return
        elseif not self.Data:IsOpen() then
            if not self.is_first then
                PublicPopupCtrl.Instance:Center(self.language.ActTimeTip[2])
            end
        end
        self.ObjBlock:SetActive(false)
        self.Data:NowAnswer(0)
        local times1, times2 = self.Data:Times()
        if self.Data:Remind() > 0 then
            -- 有的话刷新题目
            local question_cfg = self.Data:QuestionCfg()
            UH.SetText(self.TxtQuestion, question_cfg.problem)
            self.AnswerList:SetData(self.Data:QuestionData())
        else
            self.ObjStartShow:SetActive(true)
        end
        self.Remind:SetNum(self.Data:Remind())
        UH.SetText(self.TxtTimes, Format(self.language.Times, times1, times2))
    end
end

function BravingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
end

function BravingPanel:OnStatrClick()
    if not self.Data:IsOpen() then
        PublicPopupCtrl.Instance:Center(self.language.ActTimeTip[2])
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    else
        local times, _ = self.Data:Times()
        if times > 0 then
            self.ObjStartShow:SetActive(false)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
        else
            PublicPopupCtrl.Instance:Center(self.language.CountTip)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        end
    end
end

function BravingPanel:OnAnswerClick(data)
    BravingCtrl.Instance:SendReq(BravingReqType.Answer, data.index)
    self.Data:NowAnswer(data.index)
    self.need_delay = true
    self.ObjBlock:SetActive(true)
    self.AnswerList:SetData(self.Data:QuestionData())
    
    local is_right = data.index == data.info.correct
    PublicPopupCtrl.Instance:Center(self.language.AnswerTip[is_right and 1 or 0])

    TimeHelper:CancelTimer(self.ht)
    self.ht = Invoke(function()
        self.need_delay = false
        self:FlushAll()
    end, 3)
end

--------------BravingItem--------------
BravingItem = BravingItem or DeclareMono("BravingItem", UIWidgetBaseItem)
function BravingItem:BravingItem()
    self.Data = BravingData.Instance
end

function BravingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtIndex, Language.Braving.Index[data.index])
    UH.SetText(self.TxtDesc, data.info["answer" .. data.index])
    local now_answer = self.Data:NowAnswer()
    if now_answer > 0 then
        local is_right = data.index == data.info.correct
        self.ObjRight:SetActive(data.index == now_answer and is_right)
        self.ObjFaild:SetActive(data.index == now_answer and not is_right)
    else
        self.ObjRight:SetActive(false)
        self.ObjFaild:SetActive(false)
    end
end