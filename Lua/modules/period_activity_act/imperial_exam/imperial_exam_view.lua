ImperialExamView = ImperialExamView or DeclareView("ImperialExamView", "period_activity_act/imperial_exam/imperial_exam", Mod.PeriodActivity.ImperialExam)
VIEW_DECLARE_LEVEL(ImperialExamView, ViewLevel.L1)
VIEW_DECLARE_MASK(ImperialExamView, ViewMask.BgBlock)

function ImperialExamView:ImperialExamView()
    
end

function ImperialExamView:CloseCallback()
    
end

function ImperialExamView:OnCloseClick()
    ViewMgr:CloseView(ImperialExamView)
end

----------------ImperialExamPanel----------------
ImperialExamPanel = ImperialExamPanel or DeclareMono("ImperialExamPanel", UIWFlushPanel)
function ImperialExamPanel:ImperialExamPanel()
    self.Data = ImperialExamData.Instance
    self.language = Language.ImperialExam
    self.data = self.Data.info_data
    self.Data:IsShowRight(false)
    self.Data:IsShowResult(false)
    self.ObjShowUseCard:SetActive(false)
    self.is_first = false
    -- 记录上一次答题次数
    self.last_ans_count = self.data.base.answer_count
    self.data_cares = {
        {data = self.data, func = self.FlushShow, init_call = false},
        {data = self.Data.use_card_data, func = self.ShowCard, init_call = false},
    }
end

function ImperialExamPanel:Awake()
    UIWFlushPanel.Awake(self)
    if not TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(self.language.NoTeamTip)
        ViewMgr:CloseView(ImperialExamView)
        return
    end
    if not self.npc_ui_obj then
        self.npc_ui_obj = UIObjDrawer.New()
        self.ModelUIShow:SetShowData({ui_obj = self.npc_ui_obj, view = self})
    end
    self.npc_ui_obj:SetData(DrawerHelper.GetNpcPath(self.Data:AnswerNpcResId()))
    UH.SetText(self.TxtNpcName, self.Data:AnswerNpcName())
    self.ObjBlock:SetActive(false)
    UH.SetText(self.TxtAnswer, self.Data:GetAnswerName())
    self:FlushShow()
end

function ImperialExamPanel:FlushShow()
    self.Timer:CloseCountDownTime()
    self.Timer:StampTime(self.data.timestamp, TimeType.Type_Special_5, self.language.Time)
    if self.data.base.day_start_flag == 0 then
        -- 没接任务直接关掉界面
        PublicPopupCtrl.Instance:Center(self.language.TaskTip)
        self.Data:ResetOther(true)
        ViewMgr:CloseView(ImperialExamView)
        return
    end
    local question, answer_data = self.Data:QuestionInfo()
    if not question then
        -- 拿不到题目数据直接关闭
        PublicPopupCtrl.Instance:Center(self.language.QuestionError)
        ViewMgr:CloseView(ImperialExamView)
        return
    end
    UH.SetText(self.TxtQuestion, question)
    self.AnswerList:SetData(answer_data)

    local team_info = TeamData.Instance:GetTeamInfo()
	local member_list = team_info:MemberListSort()
    self.TeamList:SetData(member_list or {})
    
    self.TxtResult:SetObjActive(self.data.result.answer >= 0)
    if not self.is_first then
        self.ObjFinish:SetActive(false)
    end

    -- 回答者并且在没有出结果的时候才能求助
    if self.Data:IsCanAnswer() then
        self.ObjHelp:SetActive(self.data.result.answer == -1)
    else
        self.ObjHelp:SetActive(false)
    end
    -- 有人回答了，开始展示回答了什么，这里只会进一次
    if self.data.result.answer >= 0 and not self.is_first then
        self.is_first = true
        self.ObjBlock:SetActive(true)
        -- 判断是无人作答还是有人作答
        if self.data.result.answer > 0 then
            UH.SetText(self.TxtResult, Format(self.Data:GetAnswerName(), self.language.Result[3]))
        else
            UH.SetText(self.TxtResult, self.language.Result[2])
        end
        
        TimeHelper:CancelTimer(self.ht_delay1)
        self.ht_delay1 = Invoke(function()
            -- 展示结果
            UH.SetText(self.TxtResult, self.language.Result[self.data.result.answer == 0 and 2 or self.data.result.result])
            self.Data:IsShowRight(true)
            self.AnswerList:SetData(answer_data)
            TimeHelper:CancelTimer(self.ht_delay2)
            self.ht_delay2 = Invoke(function()
                self.Data:IsShowResult(true)
                self.ItemList:SetData(self.Data:GetQuestionItem())
                self.TeamList:SetData(member_list or {})
                self.ObjFinish:SetActive(true)

                -- 如果次数一致，证明答题奖励已用完
                if self.last_ans_count == self.data.base.answer_count then
                    self.ObjHasItem:SetActive(false)
                else
                    self.ObjHasItem:SetActive(true)
                    -- 这里自己飘道具
                    local item = self.Data:GetQuestionItem()
                    if not TableIsEmpty(item) then
                        for k, v in pairs(item) do
                            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num, color_str = v:ColorStr()})
                            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = v, num = v.num}}
                        end
                    end
                end
                -- 展示结束界面的时候关闭使用卡
                self.ObjShowUseCard:SetActive(false)
                TimeHelper:CancelTimer(self.ht_delay3)
                self.ht_delay3 = Invoke(function()
                    -- 没点确认自动关闭
                    self:OnConfirmClick()
                end, 2)
            end, 2)
        end, self.data.result.answer == 0 and 0 or 3)
    end

    UH.SetText(self.TxtNowSatisfy, Format(self.language.NowSatisfy, self.Data:GetQuestionSatisfy()))
    UH.SpriteName(self.ImgSetResult, self.data.result.result == 0 and "_LocBuJia" or "_LocDingJian")
end

function ImperialExamPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht_delay1)
    TimeHelper:CancelTimer(self.ht_delay2)
    TimeHelper:CancelTimer(self.ht_delay3)
    TimeHelper:CancelTimer(self.ht_delay4)

    if TeamData.Instance:IsLeader() then
        self.Data:ResetOther(true)
        ImperialExamCtrl.Instance:JudgeStartTask()
    end
    Delete(self, "npc_ui_obj")
end

-- 点击后关闭，或者x秒后自动关闭
function ImperialExamPanel:OnConfirmClick()
    ViewMgr:CloseView(ImperialExamView)
end

-- 点击答案
function ImperialExamPanel:OnAnswerClick(data)
    if not self.Data:IsCanAnswer() then
        PublicPopupCtrl.Instance:Center(self.language.CannotAnswerTip)
        return
    end
    local use_card_info = self.Data.use_card_data.info
    -- 如果有人使用了除错卡，那需要判断他点击的是不是错误答案
    if use_card_info.card_type == ImperialExamCardType.ReduceWrong then
        if use_card_info.param == data.index then
            PublicPopupCtrl.Instance:Center(self.language.ClickFaildTip)
            return
        end
    end
    ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Answer, data.index)
    self.ObjBlock:SetActive(true)
end

-- 点击建议
function ImperialExamPanel:OnSuggestClick(data)
    if self.Data:IsCanAnswer() then
        PublicPopupCtrl.Instance:Center(self.language.CannotSuggestTip)
        return
    end
    local use_card_info = self.Data.use_card_data.info
    -- 如果有人使用了除错卡，那需要判断他点击的是不是错误答案
    if use_card_info.card_type == ImperialExamCardType.ReduceWrong then
        if use_card_info.param == data.index then
            PublicPopupCtrl.Instance:Center(self.language.ClickFaildTip)
            return
        end
    end
    ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Advise, data.index)
end

-- 点击求助
function ImperialExamPanel:OnHelpClick()
    if not self.Data:IsCanAnswer() then
        PublicPopupCtrl.Instance:Center(self.language.CannotHelpTip)
        return
    end
    ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Help)
end

-- 显示谁用了卡
function ImperialExamPanel:ShowCard()
    local use_card_info = self.Data.use_card_data.info
    UH.SpriteName(self.IconUseCard, "_LocCard" .. use_card_info.card_type)
    self.ObjShowUseCard:SetActive(true)
    UH.SetText(self.TxtUseCard, Format(self.language.UseCard, self.Data:GetNameByUid(use_card_info.uid), self.Data:KardName(use_card_info.card_type)))
    -- 换题卡需要自己手动换题
    if use_card_info.card_type == ImperialExamCardType.Change then
        self.Data:SetQuestion(use_card_info.param)
    end
    TimeHelper:CancelTimer(self.ht_delay4)
    self.ht_delay4 = Invoke(function()
        self.ObjShowUseCard:SetActive(false)
    end, 1.5)
    self:FlushShow()
end

--------------ImperialExamAnswerItem--------------
ImperialExamAnswerItem = ImperialExamAnswerItem or DeclareMono("ImperialExamAnswerItem", UIWidgetBaseItem)
function ImperialExamAnswerItem:ImperialExamAnswerItem()
    self.Data = ImperialExamData.Instance
end

function ImperialExamAnswerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtAnswer, data.answer)
    UH.SetText(self.TxtNum, DataHelper.GetDaXie(data.index, true))
    local result = self.Data.info_data.result
    -- 这里是回答了答案
    if result.answer >= 0 then
        self.ObjAnswer:SetActive(result.answer == data.index)
        self.ObjRight:SetActive(self.Data:IsShowRight() and data.right == data.index)
        self.ObjSuggest:SetActive(false)
    else
        self.ObjAnswer:SetActive(false)
        self.ObjRight:SetActive(false)
        self.ObjSuggest:SetActive(not self.Data:IsCanAnswer())
    end
    -- 判断是否用了排错卡
    local use_card_info = self.Data.use_card_data.info
    if use_card_info.card_type == ImperialExamCardType.ReduceWrong and use_card_info.param == data.index then
        UH.SpriteName(self.IconRight, "Cha")
        self.ObjRight:SetActive(true)
        self.ObjSuggest:SetActive(false)
    else
        UH.SpriteName(self.IconRight, "Gou")
    end
end

--------------ImperialExamHeadItem--------------
ImperialExamHeadItem = ImperialExamHeadItem or DeclareMono("ImperialExamHeadItem", UIWidgetBaseItem)
function ImperialExamHeadItem:ImperialExamHeadItem()
    self.Data = ImperialExamData.Instance
    self.language = Language.ImperialExam
    self.is_praise = false
    UH.SpriteName(self.IconPraise, "Zan1")
end

function ImperialExamHeadItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local color = data.is_self and COLORSTR.Green4 or COLORSTR.Yellow2
    UH.SetText(self.TxtName, Format(Language.Common.Xstr, color, info.name))
    UH.SetText(self.TxtLevel, info.level)
    UH.SetAvatar(self.IconSp, info.appearance.avatar_type, info.appearance.avatar_id, info.appearance.avatar_quality)
    local result = self.Data.info_data.result
    local praise_cfg = self.Data:GetInfoPraise(info.uid)
    if TableIsEmpty(praise_cfg) then
        LogError("找不到该队员点赞信息", info.uid, self.Data.info_data.praise)
        return
    end

    local str = ""
    if not self.Data:IsShowResult() then
        -- 各种气泡，有就显示
        self.ObjAnswer:SetActive(praise_cfg.word_id > 0)
        if praise_cfg.word_id > 0 then
            str = RichTextHelper.ParseAllFunc(self.Data:GetWord(praise_cfg.word_id), true, RichTextFormat.FormatNpcBubbleSpecial)
        end
    else
        -- 展示奖励结果的时候不显示了
        self.ObjAnswer:SetActive(false)
    end
    -- 这个richtext需要开关一下才能刷新
    self.TxtSelect.enabled = false
    self.TxtSelect.text = str
    self.TxtSelect.enabled = true

    self.ObjPraise:SetActive(not data.is_self)
    self.ObjCard:SetActive(praise_cfg.card > 0)
    if praise_cfg.card > 0 then
        UH.SetText(self.TxtCard, self.language.CardType[praise_cfg.card])
        UH.SpriteName(self.IconCard, "Card" .. praise_cfg.card)
    end

    self.ObjNotAnswer:SetActive(info.uid ~= self.Data.info_data.answer_role_id)
end

-- 点击点赞
function ImperialExamHeadItem:OnPraiseClick()
    if not self.is_praise then
        self.is_praise = true
        UH.SpriteName(self.IconPraise, "Zan2")
        ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Praise, self.data.info.uid)
    end
end

-- 点击使用卡
function ImperialExamHeadItem:OnCardClick()
    if self.data.is_self then
        if self.Data.info_data.result.answer == -1 then
            if self.Data.use_card_data.info.card_type > 0 then
                PublicPopupCtrl.Instance:Center(self.language.CannotCardTip[3])
            else
                if self.Data.info_data.base.card > 0 then
                    ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.UseCard)
                else
                    PublicPopupCtrl.Instance:Center(self.language.CannotCardTip[1])
                end
            end
        else
            PublicPopupCtrl.Instance:Center(self.language.CannotCardTip[2])
        end
    end
end


---------------------------ImperialExamOverView---------------------------
ImperialExamOverView = ImperialExamOverView or DeclareView("ImperialExamOverView", "period_activity_act/imperial_exam/imperial_exam_over")
function ImperialExamOverView:LoadCallback(param)
    self.WinState:SetActive(param.is_win == 1)
    self.FailState:SetActive(param.is_win ~= 1)
    local reward = ImperialExamData.Instance:GetLastItemList()
    -- 奖励
    if not TableIsEmpty(reward) then
        self.Reward:SetActive(true)
        self.NotGet:SetActive(false)
        self.Geted:SetActive(false)
        self.RewardList:SetData(reward)
    else
        self.Reward:SetActive(false)
        local answer = ImperialExamData.Instance.info_data.base.answer_count
        local total = ImperialExamData.Instance:TotalTimes()
        if answer < total then
            self.NotGet:SetActive(true)
            self.Geted:SetActive(false)
            UH.SetText(self.TxtNowPro, Format(Language.ImperialExam.NowProgress, answer, total))
        else
            self.NotGet:SetActive(false)
            self.Geted:SetActive(true)
        end
    end
end

function ImperialExamOverView:OnCloseClick()
    ViewMgr:CloseView(ImperialExamOverView)
end

function ImperialExamOverView:CloseCallback()
    ImperialExamData.Instance:SetLastItemList({})
end

----------------ImperialExamOverPanel----------------
ImperialExamOverPanel = ImperialExamOverPanel or DeclareMono("ImperialExamOverPanel", UIWFlushPanel)
function ImperialExamOverPanel:ImperialExamOverPanel()
    self.Data = ImperialExamData.Instance
    self.language = Language.ImperialExam
    self.data = self.Data.last_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow},
        {data = self.Data.info_data, func = self.FlushShow, "base"},
    }
end

function ImperialExamOverPanel:Awake()
    UIWFlushPanel.Awake(self)

end

function ImperialExamOverPanel:FlushShow()
    UH.SetText(self.TxtTodaySatisfy, Format(self.language.TotalSatisfy, self.Data.info_data.base.satisfy))
    local team_info = TeamData.Instance:GetTeamInfo()
    -- 这里加一层判断，如果在服务端下发的信息里面找不到该角色，就不显示出来
    local member_list = {}
    for k, v in pairs(team_info:MemberListSort()) do
        local praise_cfg = ImperialExamData.Instance:GetLastResultInfo(v.info.uid)
        if not TableIsEmpty(praise_cfg) then
            table.insert(member_list, v)
        end
    end
    self.TeamList:SetData(member_list or {})
end


--------------ImperialExamOverItem--------------
ImperialExamOverItem = ImperialExamOverItem or DeclareMono("ImperialExamOverItem", UIWidgetBaseItem)
function ImperialExamOverItem:ImperialExamOverItem()
    self.is_praise = false
    self.is_friend = false
    self.is_my_friend = false
    self.Data = ImperialExamData.Instance
    self.language = Language.ImperialExam
    UH.SpriteName(self.IconPraise, "Zan1")
end

function ImperialExamOverItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local index = data.index % 2 == 0 and 2 or 1
	UH.SpriteName(self.BgSp, "RoleHeadBtm" .. index)
    UH.SizeDelta(self.BgSp, Vector3.New(800, 86))

    local info = data.info
    UH.SetAvatar(self.IconSp, info.appearance.avatar_type, info.appearance.avatar_id, info.appearance.avatar_quality)
    local color = data.is_self and COLORSTR.Green7 or COLORSTR.Yellow1
    local praise_cfg = self.Data:GetLastResultInfo(info.uid)
    if TableIsEmpty(praise_cfg) then
        return
    end

    UH.SetText(self.TxtSatisfy, Format(Language.Common.Xstr, color, praise_cfg.satisfy))
    UH.SetText(self.TxtTotalNum, Format(Language.Common.Xstr, color, praise_cfg.total_num))
    local rate = 0
    if praise_cfg.total_num ~= 0 then
        rate = math.floor((praise_cfg.true_num / praise_cfg.total_num) * 100)
    end
    UH.SetText(self.TxtRate, Format(Language.Common.Xstr, color, rate .. "%"))
    UH.SetText(self.TxtPraise, Format(Language.Common.Xstr, color, praise_cfg.praise_num))
    self.ObjFriend:SetActive(not data.is_self)
    self.ObjPraise:SetActive(not data.is_self)
    
    self.is_my_friend = SocietyData.Instance:IsFriend(info.uid)
    if self.is_friend then
        -- 点击过就显示绿的
        self.ObjImgFriend:SetActive(false)
    else
        self.ObjImgFriend:SetActive(not self.is_my_friend)
    end
end

function ImperialExamOverItem:OnPraiseClick()
    if not self.is_praise then
        self.is_praise = true
        UH.SpriteName(self.IconPraise, "Zan2")
        ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Praise, self.data.info.uid)
    end
end

function ImperialExamOverItem:OnFriendClick()
    if self.is_my_friend then
        PublicPopupCtrl.Instance:Center(self.language.IsMyFriendTip)
    else
        if not self.is_friend then
            SocietyCtrl.Instance:SendFriendAdd(self.data.info.uid)
            ImperialExamCtrl.Instance:SendReq(ImperialExamReqType.Friend, self.data.info.uid)
            self.ObjImgFriend:SetActive(false)
            self.is_friend = true
        else
            PublicPopupCtrl.Instance:Center(self.language.FriendTip)
        end
    end
end