MSLantrenView = MSLantrenView or DeclareView("MSLantrenView", "period_activity_act/ms_lantern/ms_lantern", Mod.PeriodActivity.MSLantern)
VIEW_DECLARE_LEVEL(MSLantrenView, ViewLevel.L1)
VIEW_DECLARE_MASK(MSLantrenView, ViewMask.BgBlock)

function MSLantrenView:MSLantrenView()
    
end

function MSLantrenView:CloseCallback()
    
end

function MSLantrenView:OnCloseClick()
    ViewMgr:CloseView(MSLantrenView)
end

----------------MSLantrenPanel----------------
MSLantrenPanel = MSLantrenPanel or DeclareMono("MSLantrenPanel", UIWFlushPanel)
function MSLantrenPanel:MSLantrenPanel()
    self.Data = MSLanternData.Instance
    self.language = Language.MSLantern
    self.data = self.Data.info_data
    self.Data:IsShowRight(false)
    self.ObjShowUseCard:SetActive(false)
    self.is_first = false
    -- 记录上一次答题次数
    self.last_ans_count = self.data.base.answer_count
    self.data_cares = {
        {data = self.data, func = self.FlushShow},
        {data = self.Data.use_card_data, func = self.ShowCard, init_call = false},
    }
end

function MSLantrenPanel:Awake()
    UIWFlushPanel.Awake(self)
    if not TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(self.language.NoTeamTip)
        ViewMgr:CloseView(MSLantrenView)
    end
    if not self.npc_ui_obj then
        self.npc_ui_obj = UIObjDrawer.New()
        self.ModelUIShow:SetShowData({ui_obj = self.npc_ui_obj, view = self})
    end
    self.npc_ui_obj:SetData(DrawerHelper.GetNpcPath(1055))
    self.ObjBlock:SetActive(false)
end

function MSLantrenPanel:FlushShow()
    self.Timer:CloseCountDownTime()
    self.Timer:StampTime(self.data.timestamp, TimeType.Type_Special_5, self.language.Time)
    if self.data.base.day_start_flag == 0 then
        PublicPopupCtrl.Instance:Center(self.language.TaskTip)
        self.Data:ResetOther(true)
        ViewMgr:CloseView(MSLantrenView)
        return
    end
    local question, answer_data = self.Data:QuestionInfo()
    if not question then
        PublicPopupCtrl.Instance:Center(self.language.QuestionError)
        ViewMgr:CloseView(MSLantrenView)
        return
    end
    UH.SetText(self.TxtQuestion, question)
    self.AnswerList:SetData(answer_data)

    local team_info = TeamData.Instance:GetTeamInfo()
	local member_list = team_info:MemberListSort()
    self.TeamList:SetData(member_list or {})
    
    self.TxtResult:SetObjActive(self.data.result.answerer_id >= 0)
    self.ObjFinish:SetActive(false)
    -- 有人回答了，开始展示回答了什么，这里只会进一次
    if self.data.result.answerer_id >= 0 and not self.is_first then
        -- 手动将时间戳置为0
        self.data.timestamp = 0
        self.is_first = true
        self.ObjBlock:SetActive(true)
        -- 判断是无人作答还是有人作答
        if self.data.result.answerer_id > 0 then
            local answer_name = self.Data:GetAnswerName()
            UH.SetText(self.TxtResult, Format(self.language.Result[3], answer_name))
        else
            UH.SetText(self.TxtResult, self.language.Result[2])
        end
        
        TimeHelper:CancelTimer(self.ht_delay1)
        self.ht_delay1 = Invoke(function()
            -- 展示结果
            UH.SetText(self.TxtResult, self.language.Result[self.data.result.answerer_id == 0 and 2 or self.data.result.result])
            self.Data:IsShowRight(true)
            self.AnswerList:SetData(answer_data)
            TimeHelper:CancelTimer(self.ht_delay2)
            self.ht_delay2 = Invoke(function()
                self.ItemList:SetData(self.Data:GetQuestionItem())
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
        end, self.data.result.answerer_id == 0 and 0 or 3)
    end

    UH.SetText(self.TxtNowSatisfy, Format(self.language.NowSatisfy, self.Data:GetQuestionSatisfy()))
    UH.SpriteName(self.ImgSetResult, self.data.result.result == 0 and "_LocBuJia" or "_LocDingJian")
end

function MSLantrenPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht_delay1)
    TimeHelper:CancelTimer(self.ht_delay2)
    TimeHelper:CancelTimer(self.ht_delay3)
    TimeHelper:CancelTimer(self.ht_delay4)
    -- 这三个改到收到开始答题
    -- self.Data:ResetResult()
    -- self.Data:ResetPraise()
    -- self.Data:ResetUseCard()
    if TeamData.Instance:IsLeader() then
        self.Data:ResetOther(true)
        MSLanternCtrl.Instance:JudgeStartTask()
    end
    Delete(self, "npc_ui_obj")
end

-- 点击后关闭，或者x秒后自动关闭
function MSLantrenPanel:OnConfirmClick()
    ViewMgr:CloseView(MSLantrenView)
end

-- 点击答案
function MSLantrenPanel:OnAnswerClick(data)
    local use_card_info = self.Data.use_card_data.info
    -- 如果有人使用了除错卡，那需要判断他点击的是不是错误答案
    if use_card_info.card_type == MSLanternCardType.ReduceWrong then
        if use_card_info.param == data.index then
            PublicPopupCtrl.Instance:Center(self.language.ClickFaildTip)
            return
        end
    end
    MSLanternCtrl.Instance:SendReq(MSLanternReqType.Answer, data.index)
    self.ObjBlock:SetActive(true)
end

-- 显示谁用了卡
function MSLantrenPanel:ShowCard()
    local use_card_info = self.Data.use_card_data.info
    UH.SpriteName(self.IconUseCard, "_LocCard" .. use_card_info.card_type)
    self.ObjShowUseCard:SetActive(true)
    UH.SetText(self.TxtUseCard, Format(self.language.UseCard, self.Data:GetNameByUid(use_card_info.uid), self.Data:KardName(use_card_info.card_type)))
    -- 换题卡需要自己手动换题
    if use_card_info.card_type == MSLanternCardType.Change then
        self.Data:SetQuestion(use_card_info.param)
    end
    TimeHelper:CancelTimer(self.ht_delay4)
    self.ht_delay4 = Invoke(function()
        self.ObjShowUseCard:SetActive(false)
    end, 1.5)
    self:FlushShow()
end

--------------MSLantrenAnswerItem--------------
MSLantrenAnswerItem = MSLantrenAnswerItem or DeclareMono("MSLantrenAnswerItem", UIWidgetBaseItem)
function MSLantrenAnswerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtAnswer1, data.answer)
    UH.SetText(self.TxtAnswer2, data.answer)
    UH.SetText(self.TxtNum, DataHelper.GetDaXie(data.index, true))
    local result = MSLanternData.Instance.info_data.result
    -- 这里是回答了答案
    if result.answerer_id >= 0 then
        self.ObjAnswer:SetActive(result.answerer_id == data.index)
        self.ObjRight:SetActive(MSLanternData.Instance:IsShowRight() and data.right == data.index)
    else
        self.ObjAnswer:SetActive(false)
        self.ObjRight:SetActive(false)
    end
    -- 判断是否用了排错卡
    local use_card_info = MSLanternData.Instance.use_card_data.info
    if use_card_info.card_type == MSLanternCardType.ReduceWrong and use_card_info.param == data.index then
        UH.SpriteName(self.IconRight, "Cha")
        self.ObjRight:SetActive(true)
    else
        UH.SpriteName(self.IconRight, "DuiGou")
    end
end

--------------MSLantrenHeadItem--------------
MSLantrenHeadItem = MSLantrenHeadItem or DeclareMono("MSLantrenHeadItem", UIWidgetBaseItem)
function MSLantrenHeadItem:MSLantrenHeadItem()
    self.is_praise = false
    UH.SpriteName(self.IconPraise, "Zan1")
end

function MSLantrenHeadItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local color = data.is_self and COLORSTR.Green4 or COLORSTR.Yellow2
    UH.SetText(self.TxtName, Format(Language.Common.Xstr, color, info.name))
    UH.SetText(self.TxtLevel, info.level)
    UH.SetAvatar(self.IconSp, info.appearance.avatar_type, info.appearance.avatar_id, info.appearance.avatar_quality)
    local result = MSLanternData.Instance.info_data.result
    local praise_cfg = MSLanternData.Instance:GetInfoPraise(info.uid)
    if TableIsEmpty(praise_cfg) then
        LogError("找不到该队员点赞信息", info.uid, MSLanternData.Instance.info_data.praise)
        return
    end
    -- 有回答就飘回答，否则飘点赞，否则不显示
    self.ObjAnswer:SetActive(true)
    local str = ""
    if result.answerer_answer == info.uid then
        str = DataHelper.GetDaXie(result.answerer_id, true)
    elseif praise_cfg.is_praise > 0 then
        str = RichTextHelper.ParseAllFunc(MSLanternData.Instance:GetWord(praise_cfg.word_id), true, RichTextFormat.FormatNpcBubbleSpecial)
    elseif result.answerer_id == -1 or praise_cfg.is_praise == 0 then
        self.ObjAnswer:SetActive(false)
    end
    self.TxtSelect.text = str

    self.ObjPraise:SetActive(not data.is_self)
    self.ObjCard:SetActive(praise_cfg.card > 0)
    if praise_cfg.card > 0 then
        UH.SetText(self.TxtCard, Language.MSLantern.CardType[praise_cfg.card])
        UH.SpriteName(self.IconCard, "Card" .. praise_cfg.card)
    end
end

-- 点击点赞
function MSLantrenHeadItem:OnPraiseClick()
    if not self.is_praise then
        self.is_praise = true
        UH.SpriteName(self.IconPraise, "Zan2")
        MSLanternCtrl.Instance:SendReq(MSLanternReqType.Praise, self.data.info.uid)
    end
end

-- 点击使用卡
function MSLantrenHeadItem:OnCardClick()
    if self.data.is_self and MSLanternData.Instance.info_data.result.answerer_id == -1 then
        MSLanternCtrl.Instance:SendReq(MSLanternReqType.UseCard)
    end
end


---------------------------MSLantrenOverView---------------------------
MSLantrenOverView = MSLantrenOverView or DeclareView("MSLantrenOverView", "period_activity_act/ms_lantern/ms_lantern_over")
function MSLantrenOverView:LoadCallback(param)
    self.WinState:SetActive(param.is_win == 1)
    self.FailState:SetActive(param.is_win ~= 1)
    local reward = MSLanternData.Instance:GetLastItemList()
    -- 奖励
    if not TableIsEmpty(reward) then
        self.Reward:SetActive(true)
        self.NotGet:SetActive(false)
        self.Geted:SetActive(false)
        self.RewardList:SetData(reward)
    else
        self.Reward:SetActive(false)
        local answer = MSLanternData.Instance.info_data.base.answer_count
        local total = MSLanternData.Instance:TotalTimes()
        if answer < total then
            self.NotGet:SetActive(true)
            self.Geted:SetActive(false)
            UH.SetText(self.TxtNowPro, Format(Language.MSLantern.NowProgress, answer, total))
        else
            self.NotGet:SetActive(false)
            self.Geted:SetActive(true)
        end
    end
end

function MSLantrenOverView:OnCloseClick()
    ViewMgr:CloseView(MSLantrenOverView)
end

function MSLantrenOverView:CloseCallback()
    MSLanternData.Instance:SetLastItemList({})
end

----------------MSLantrenOverPanel----------------
MSLantrenOverPanel = MSLantrenOverPanel or DeclareMono("MSLantrenOverPanel", UIWFlushPanel)
function MSLantrenOverPanel:MSLantrenOverPanel()
    self.Data = MSLanternData.Instance
    self.language = Language.MSLantern
    self.data = self.Data.last_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow},
        {data = self.Data.info_data, func = self.FlushShow, "base"},
    }
end

function MSLantrenOverPanel:Awake()
    UIWFlushPanel.Awake(self)

end

function MSLantrenOverPanel:FlushShow()
    UH.SetText(self.TxtTodaySatisfy, Format(self.language.TotalSatisfy, self.Data.info_data.base.satisfy))
    local team_info = TeamData.Instance:GetTeamInfo()
	local member_list = team_info:MemberListSort()
    self.TeamList:SetData(member_list or {})
end


--------------MSLantrenOverItem--------------
MSLantrenOverItem = MSLantrenOverItem or DeclareMono("MSLantrenOverItem", UIWidgetBaseItem)
function MSLantrenOverItem:MSLantrenOverItem()
    self.is_praise = false
    self.is_friend = false
    self.is_my_friend = false
    UH.SpriteName(self.IconPraise, "Zan1")
end

function MSLantrenOverItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local index = data.index % 2 == 0 and 2 or 1
	UH.SpriteName(self.BgSp, "RoleHeadBtm" .. index)
    UH.SizeDelta(self.BgSp, Vector3.New(800, 86))

    local info = data.info
    UH.SetAvatar(self.IconSp, info.appearance.avatar_type, info.appearance.avatar_id, info.appearance.avatar_quality)
    local color = data.is_self and COLORSTR.Green7 or COLORSTR.Yellow1
    local praise_cfg = MSLanternData.Instance:GetLastResultInfo(info.uid)
    if TableIsEmpty(praise_cfg) then
        -- LogError("找不到该队员结算信息", info.uid, MSLanternData.Instance.last_data)
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

function MSLantrenOverItem:OnPraiseClick()
    if not self.is_praise then
        self.is_praise = true
        UH.SpriteName(self.IconPraise, "Zan2")
        MSLanternCtrl.Instance:SendReq(MSLanternReqType.Praise, self.data.info.uid)
    end
end

function MSLantrenOverItem:OnFriendClick()
    if self.is_my_friend then
        PublicPopupCtrl.Instance:Center(Language.MSLantern.IsMyFriendTip)
    else
        if not self.is_friend then
            SocietyCtrl.Instance:SendFriendAdd(self.data.info.uid)
            MSLanternCtrl.Instance:SendReq(MSLanternReqType.Friend, self.data.info.uid)
            self.ObjImgFriend:SetActive(false)
            self.is_friend = true
        else
            PublicPopupCtrl.Instance:Center(Language.MSLantern.FriendTip)
        end
    end
end