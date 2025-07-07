GuildEmbattleMsg = GuildEmbattleMsg or DeclareView('GuildEmbattleMsg', 'guild_embattle/guild_embattle_msg')
function GuildEmbattleMsg:GuildEmbattleMsg()
    self.embattle_data = GuildEmbattleData.Instance
end
function GuildEmbattleMsg:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
    -- GuildEmbattleData.Instance:GetRank()
end
function GuildEmbattleMsg:OnClickClose()
    ViewMgr:CloseView(GuildEmbattleMsg)
end
function GuildEmbattleMsg:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    self.embattle_data:ViewData().select = 1
end

GuildEmbattleMsgPanel = GuildEmbattleMsgPanel or DeclareMono('GuildEmbattleMsgPanel', UIWFlushPanel)
function GuildEmbattleMsgPanel:GuildEmbattleMsgPanel()
    self.act_data = ActivityData.Instance
    self.embattle_data = GuildEmbattleData.Instance
    self.data_cares = {
        {data = self.act_data:GetActivityEvent(), func = self.FlushShow},
        {data = self.embattle_data:BattleProcess(), func = self.onFlush},
        {data = self.embattle_data:Base(), func = self.FlushRoundShow}
    }
    self.is_only_show_finish = false
end
function GuildEmbattleMsgPanel:OnClickGo()
    local has_join = self.embattle_data:EntryQualify()
    if has_join then
        local base = self.embattle_data:Base()
        local act_info = ActivityData.Instance:GetActivityStatusInfo(ActType.GuildEmbattle)
        if act_info.status == ActStatusType.Open and base.round_state == 0 then
            GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
            ViewMgr:CloseView(GuildEmbattleMsg)
        else
            PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GotoTip[1])
        end
    else
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GotoTip[2])
    end
end
function GuildEmbattleMsgPanel:FlushShow()
    local is_open = self.embattle_data:IsOpen()
    self.FinishObj:SetActive(not is_open)
    self.SchedObj:SetActive(is_open)
    self.is_only_show_finish = not is_open
    if is_open then
        self.Btns:SetActive(false)
    else
        self.MsgAnim:SetInteger('state', 0)
        self.Btns:SetActive(true)
    end
end
function GuildEmbattleMsgPanel:OnClickRank()
    self.MsgAnim:SetInteger('state', 1)
end
function GuildEmbattleMsgPanel:OnClickBackFinish()
    self.MsgAnim:SetInteger('state', 0)
end
function GuildEmbattleMsgPanel:onFlush()
    self.MsgAnim:SetInteger('state', 0)
    -- self.BtnRank:SetActive(true)
    --[[ local base = self.embattle_data:Base()
    if base.round > 0 and self.is_only_show_finish == false then
        local round = self.embattle_data:CalcRound()
        self.RoundToggles[round].isOn = true
    end ]]
    for i = 1, 4 do
        UH.SetText(self.RoundNums[i], Language.GuildEmbattle.RoundName[i])
    end
end
function GuildEmbattleMsgPanel:FlushRoundShow()
    local base = self.embattle_data:Base()
    local stage = self.embattle_data:Stage()
    -- 如果活动开启了，需要知道当前的阶段是多少
    -- self.embattle_data:ViewData().round = round
    if self.is_only_show_finish then
        self.RoundToggles[stage].isOn = false
        for i = 1, 4 do
            self.GuEffects[i]:SetActive(false)
            self.RoundSelects[i]:SetActive(false)
            self.TimeObjs[i]:SetActive(false)
            if i < 4 then
                self.LineingObjs[i]:SetActive(false)
            end
        end
    else
        for i = 1, 3 do
            self.LineingObjs[i]:SetActive(stage - 1 >= i)
        end
        self.RoundToggles[stage].isOn = true
        for i = 1, 4 do
            self.GuEffects[i]:SetActive(i == stage)
            self.RoundSelects[i]:SetActive(true)
            self.TimeObjs[i]:SetActive(i == stage)
            UH.SetText(self.StateDesc[i], Language.GuildEmbattle.StateDesc1[base.round_state])
        end
    end
    self.RoundTimes[stage]:StampTime(
        base.next_round_state_time,
        TimeType.Type_Special_0,
        Language.GuildEmbattle.TimeEnd
    )
    local index = base.round_state == 0 and 1 or 2
    local final_stage = self.embattle_data:FinalStage()
    if stage == final_stage and base.round_state == 1 then
        index = 3
     --结束倒计时描述
    end
    self.TimeDesc:StampTime(
        base.next_round_state_time,
        TimeType.Type_Special_0,
        Language.GuildEmbattle.TimeDesc[index],
        Language.GuildEmbattle.ActEnd2
    )
    self.embattle_data:ViewData().stage = stage
end
--点击哪个阶段
function GuildEmbattleMsgPanel:OnClickChangeRound(stage)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local init_stage = self.embattle_data:InitStage()
    LogDG("胜利组起始阶段", init_stage)
    if stage < init_stage then
        local desc = Language.GuildEmbattle.ErrorRound
        local name = Language.GuildEmbattle.RoundName[stage]
        PublicPopupCtrl.Instance:Center(string.format(desc, name))
        self.RoundToggles[self.embattle_data:ViewData().stage].isOn = true
        -- self.embattle_data:ViewData().stage = stage
        return
    end
    if self.is_only_show_finish then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.BattleEnd)
        return
    end
    local curr_stage = self.embattle_data:Stage()
    if stage > curr_stage then
        self.RoundToggles[curr_stage].isOn = true
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.RoundTip)
        return
    end
    self.embattle_data:ViewData().stage = stage
end
function GuildEmbattleMsgPanel:OnClickCondition()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:OpenView(GuildEmbattleCondition)
end
function GuildEmbattleMsgPanel:OnClickPreReward()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local reward = GuildData.Instance:GetEmbattleReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward)
end
function GuildEmbattleMsgPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[122].desc})
end
--赛程
function GuildEmbattleMsgPanel:OnClickSchedule()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.is_only_show_finish then
        self.BtnToggle.isOn = true
    end
    self.embattle_data:ViewData().select = 1
end
--竞猜
function GuildEmbattleMsgPanel:OnClickQuiz()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.is_only_show_finish then
        self.BtnToggle.isOn = true
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.ActEnd)
        return
    end
    self.embattle_data:ViewData().select = 2
end

GuildEmbattleMsgFinish = GuildEmbattleMsgFinish or DeclareMono('GuildEmbattleMsgFinish', UIWFlushPanel)
function GuildEmbattleMsgFinish:GuildEmbattleMsgFinish()
    self.embattle_data = GuildEmbattleData.Instance
    self.data_cares = {
        {data = self.embattle_data:LastMatchInfo(), func = self.onFlush, init_call = false}
    }
end
function GuildEmbattleMsgFinish:onFlush()

    local rank = self.embattle_data:GetLastRank()
    --这里要修改 获取前三名的方式可能改变了
    --获取上次前三
    --[[ local last_info = self.embattle_data:LastInfo() ]]
    local num = table.nums(rank)
    if num == 0 then
        local temp2
        for i = 1, 3 do
            temp2 = self['Item' .. i]
            UH.SetText(temp2[1], Language.GuildEmbattle.RankDesc[1])
            UH.SetText(temp2[2], Language.GuildEmbattle.RankDesc[2])
            UH.SetText(temp2[3], Language.GuildEmbattle.RankDesc[1])
        end
    else
        local guild_id = GuildData.Instance:GetGuildID()
        local temp1, temp2, temp3
        temp3 = rank
        for i = 1, 3 do
            temp1 = temp3[i]
            temp2 = self['Item' .. i]
            if temp1 and temp1.guild_id > 0 then
                UH.SetText(
                    temp2[1],
                    guild_id == temp1.guild_id and self.embattle_data:GuildName(temp1.name) or temp1.name
                )
                UH.SetText(temp2[2], temp1.banner)
                UH.SetText(temp2[3], self.embattle_data:ServerName(temp1.server_id))
            else
                UH.SetText(temp2[1], Language.GuildEmbattle.RankDesc[1])
                UH.SetText(temp2[2], Language.GuildEmbattle.RankDesc[2])
                UH.SetText(temp2[3], Language.GuildEmbattle.RankDesc[1])
            end
        end
    end
    local res
    for i = 1, 3 do
        res = GuildData.Instance:GetRongYaoDesc(i)
        if res and res[1] and res[2] then
            UH.SetText(
                self.DescTexs[i],
                string.format(Language.Guild.RYBuffDesc, res[1][1], res[1][2], res[2][1], res[2][2])
            )
        else
            UH.SetText(self.DescTexs[i], '配置有问题')
        end
    end
end

GuildEmbattleMsgSchedule = GuildEmbattleMsgSchedule or DeclareMono('GuildEmbattleMsgSchedule', UIWFlushPanel)
function GuildEmbattleMsgSchedule:GuildEmbattleMsgSchedule()
    self.embattle_data = GuildEmbattleData.Instance
    self.data_cares = {
        {data = self.embattle_data:ViewData(), func = self.FlushMatchList, keys = {'stage'}},
        {data = self.embattle_data:Base(), func = self.FlushMatchList, keys = {'round_state'}},
    }
end
function GuildEmbattleMsgSchedule:onFlush()
end
function GuildEmbattleMsgSchedule:FlushMatchList()
    LogDG("刷新对战表=============================")
    local data = self.embattle_data:MatchList() or {}
    -- LogDG(data)
    --[[ for index, value in ipairs(data) do
        if value[1]then
            value[1]:DebugMsg(1)
        else
            LogDG("====================轮空1")
        end
        if value[2] then
            value[2]:DebugMsg(2)
        else
            LogDG("====================轮空2")
        end
    end ]]
    --获取某个阶段要显示的对战列表
    self.List:SetData(data or {})
end

GuildEmbattleMsgScheduleItem =
    GuildEmbattleMsgScheduleItem or DeclareMono('GuildEmbattleMsgScheduleItem', UIWidgetBaseItem)
function GuildEmbattleMsgScheduleItem:GuildEmbattleMsgScheduleItem()
    self.embattle_data = GuildEmbattleData.Instance
    self.view_state = 1
    self.is_can_view = false
end
function GuildEmbattleMsgScheduleItem:FlushBase()
    self:FlushLeftBase(self.data[1])
    self:FlushRightBase(self.data[2])
end
function GuildEmbattleMsgScheduleItem:FlushLeftBase(data)
    if data == nil then
        return
    end
    local guild_id = GuildData.Instance:GetGuildID()
    UH.SetText(self.LeftInfo[1], guild_id == data.guild_id and self.embattle_data:GuildName(data.name) or data.name)
    UH.SetText(
        self.LeftInfo[2],
        string.format(Language.GuildEmbattle.ServerName, self.embattle_data:ServerName(data.server_id))
    )
    UH.SetText(self.LeftInfo[3], data.banner)
    --上届标识
    if data.last_match_rank > 0 and data.last_match_rank < 4 then
        local info = Language.GuildEmbattle.LastRank[data.last_match_rank]
        UH.SpriteName(self.LeftRank[1], info[1])
        UH.SetText(self.LeftRank[2], info[2])
        self.LeftRank[1]:SetObjActive(true)
    else
        self.LeftRank[1]:SetObjActive(false)
    end
    self.LeftObj:SetActive(true)
    self.LeftNoObj:SetActive(false)
end
function GuildEmbattleMsgScheduleItem:FlushRightBase(data)
    local is_nil = data.guild_id == 0
    if not is_nil then
        local guild_id = GuildData.Instance:GetGuildID()
        UH.SetText(
            self.RightInfo[1],
            guild_id == data.guild_id and self.embattle_data:GuildName(data.name) or data.name
        )
        UH.SetText(
            self.RightInfo[2],
            string.format(Language.GuildEmbattle.ServerName, self.embattle_data:ServerName(data.server_id))
        )
        UH.SetText(self.RightInfo[3], data.banner)
        --上届标识
        if data.last_match_rank > 0 and data.last_match_rank < 4 then
            local info = Language.GuildEmbattle.LastRank[data.last_match_rank]
            UH.SpriteName(self.RightRank[1], info[1])
            UH.SetText(self.RightRank[2], info[2])
            self.RightRank[1]:SetObjActive(true)
        else
            self.RightRank[1]:SetObjActive(false)
        end
        self.RightObj:SetActive(true)
        self.RightNoObj:SetActive(false)
    else
        self.RightObj:SetActive(false)
        self.RightNoObj:SetActive(true)
    end
end

function GuildEmbattleMsgScheduleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local index = self.gameObject.transform:GetSiblingIndex()
    self.BgObj:SetActive(index % 2 ~= 0)
    if data[1] then
        UH.SetText(self.IndexTex, data[1].match_info.win_rank == 1 and Language.GuildEmbattle.GroupName[1] or Language.GuildEmbattle.GroupName[2])
        UH.SpriteName(self.GroupSp, data[1].match_info.win_rank == 1 and 'MoDiSheng' or 'MoDiBai')
        UH.Color(self.IndexTex, data[1].match_info.win_rank == 1 and COLORS.Yellow16 or COLORS.Blue13)
    end

    --根据状态显示
    -- 1显示是否可观战等状态
    -- 2显示竞猜内容还要根据竞猜协议进行刷新
    self:FlushBase()
    self:FlushResult()
    self:FlushGuess()
    self:FlushShow()
    self:FlushViewBtn()

    self:uncareData()
    self:careData(self.embattle_data:ViewData(), BindTool.Bind(self.FlushShow, self), 'select')
    self:careData(self.embattle_data:ViewData(), BindTool.Bind(self.FlushResult, self), 'select')
    self:careData(self.embattle_data:ViewData(), BindTool.Bind(self.FlushViewBtn, self))
    -- self:careData(self.embattle_data:Base(), BindTool.Bind(self.FlushViewBtn, self))
    self:careData(
        self.embattle_data:Base(),
        BindTool.Bind(self.FlushGuess, self),
        'champion_guild_id',
        'guess_item',
        'state'
    )
    self:careData(self.embattle_data:ViewData(), BindTool.Bind(self.FlushGuess, self), 'select')
end
function GuildEmbattleMsgScheduleItem:FlushResult()
    local result = 0
    if self.data[1] then
        result = self.data[1]:ResultType()
    end
    if result == 1 then
        self.LeftSuccObj:SetActive(true)
        self.RightSuccObj:SetActive(false)
        self.LeftInter.Interactable = true
        self.RightInter.Interactable = false
    elseif result == 2 then
        self.LeftSuccObj:SetActive(false)
        self.RightSuccObj:SetActive(true)
        self.LeftInter.Interactable = false
        self.RightInter.Interactable = true
    else
        self.LeftSuccObj:SetActive(false)
        self.RightSuccObj:SetActive(false)
        self.LeftInter.Interactable = true
        self.RightInter.Interactable = true
    end
end
function GuildEmbattleMsgScheduleItem:IsCanView()
    self.is_can_view = false
    for i = 1, 3 do
        if self.data[1] and self.data[1].battle_result[i] == 0 then
            self.is_can_view = true
            self.view_battle_id = self.data[1].battle_id[i]
        end
    end
    if self.data[2] == nil then
        self.is_can_view = false
    end
end
function GuildEmbattleMsgScheduleItem:FlushViewBtn()
    self:IsCanView()
    local base = self.embattle_data:Base()
    local round = self.embattle_data:RoundByStage()
    local curr_round = self.embattle_data:Round()
    if base.round_state == 0 and round == curr_round then
        UH.SetText(self.ViewBtnTex, Language.GuildEmbattle.ViewBtnDesc[1])
        self.ViewGray[1].enabled = true
        self.ViewGray[2].enabled = true
        self.view_state = 1
    elseif base.round_state == 1 and round == curr_round and self.is_can_view then
        UH.SetText(self.ViewBtnTex, Language.GuildEmbattle.ViewBtnDesc[2])
        self.ViewGray[1].enabled = false
        self.ViewGray[2].enabled = false
        self.view_state = 2
    else
        UH.SetText(self.ViewBtnTex, Language.GuildEmbattle.ViewBtnDesc[3])
        self.ViewGray[1].enabled = true
        self.ViewGray[2].enabled = true
        self.view_state = 3
    end
end
function GuildEmbattleMsgScheduleItem:OnClickView()
    if self.view_state == 2 then
        self.embattle_data:WatchBattle(self.data)
    else
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.ClickViewTip[self.view_state])
    end
end
function GuildEmbattleMsgScheduleItem:FlushShow()
    local index = self.embattle_data:ViewData().select
    self.NormalObj:SetActive(index == 1)
    self.GuessObj:SetActive(index == 2)
end
function GuildEmbattleMsgScheduleItem:FlushGuess()
    --没有家族和正在参与的家族成员无法进行竞猜
    --直接显示支持人数
    --左右两个家族有一个被支持，显示人数
    --冠军家族有选择 显示人数
    local base = self.embattle_data:Base()
    UH.SpriteName(self.ButtonSp[1], 'XiaoLvAnNiuAn')
    UH.SpriteName(self.ButtonSp[3], 'XiaoHongAnNiuAn')
    UH.SpriteName(self.ButtonSp[2], 'XiaoLvAnNiuAn')
    UH.SpriteName(self.ButtonSp[4], 'XiaoHongAnNiuAn')
    for i = 1, 4 do
        UH.SetText(self.GuessDesc[i], '')
        UH.SetText(self.GuessNum[i], '')
    end
    --这个支持人数也要按照
    local round = self.embattle_data:RoundByStage()--胜利组的round

    if base.round_state == 1 or base.round_state == 2 or base.curr_round ~= round then
        local desc = Language.GuildEmbattle.GuessDesc
        local temp
        if self.data[1] then
            temp = string.format(desc, self.data[1]:GuessNum())
            UH.SetText(self.GuessNum[1], self.embattle_data:FormatColor(temp, 2))
            temp = string.format(desc, self.data[1].champion_guess_num)
            UH.SetText(self.GuessNum[3], self.embattle_data:FormatColor(temp, 4))
        end
        if self.data[2] then
            temp = string.format(desc, self.data[2]:GuessNum())
            UH.SetText(self.GuessNum[2], self.embattle_data:FormatColor(temp, 2))
            temp = string.format(desc, self.data[2].champion_guess_num)
            UH.SetText(self.GuessNum[4], self.embattle_data:FormatColor(temp, 4))
        else
            UH.SetText(self.GuessNum[2], self.embattle_data:FormatColor(Language.GuildEmbattle.GuessBtn[3], 2))
            UH.SetText(self.GuessNum[4], self.embattle_data:FormatColor(Language.GuildEmbattle.GuessBtn[3], 4))
        end
    elseif base.round_state == 0 then
        -- desc 1 2 显示胜者 3 4 显示冠军
        if self.data[1] then
            local is_win = self.data[1]:IsWin()--self.embattle_data:IsWinner(round, self.data[1].guild_id)
            UH.SpriteName(self.ButtonSp[1], is_win and 'XiaoLvAnNiuLiang' or 'XiaoLvAnNiuAn')
            local is_champion = self.data[1]:IsChampion()--self.embattle_data:IsChampion(self.data[1].guild_id)
            UH.SpriteName(self.ButtonSp[3], is_champion and 'XiaoHongAnNiuLiang' or 'XiaoHongAnNiuAn')
            local temp = Language.GuildEmbattle.GuessBtn[2]
            UH.SetText(self.GuessDesc[1], self.embattle_data:FormatColor(temp, is_win and 1 or 2))
            temp = Language.GuildEmbattle.GuessBtn[1]
            UH.SetText(self.GuessDesc[3], self.embattle_data:FormatColor(temp, is_champion and 3 or 4))
        end
        if self.data[2] then
            local is_win = self.data[2]:IsWin()--self.embattle_data:IsWinner(round, self.data[2].guild_id)
            UH.SpriteName(self.ButtonSp[2], is_win and 'XiaoLvAnNiuLiang' or 'XiaoLvAnNiuAn')
            local is_champion = self.data[2]:IsChampion()--self.embattle_data:IsChampion(self.data[2].guild_id)
            UH.SpriteName(self.ButtonSp[4], is_champion and 'XiaoHongAnNiuLiang' or 'XiaoHongAnNiuAn')
            local temp = Language.GuildEmbattle.GuessBtn[2]
            UH.SetText(self.GuessDesc[2], self.embattle_data:FormatColor(temp, is_win and 1 or 2))
            temp = Language.GuildEmbattle.GuessBtn[1]
            UH.SetText(self.GuessDesc[4], self.embattle_data:FormatColor(temp, is_champion and 3 or 4))
        else
            UH.SetText(self.GuessNum[2], self.embattle_data:FormatColor(Language.GuildEmbattle.GuessBtn[3], 2))
            UH.SetText(self.GuessNum[4], self.embattle_data:FormatColor(Language.GuildEmbattle.GuessBtn[3], 4))
        end
    end
end
function GuildEmbattleMsgScheduleItem:DialogInfo(content, confirm_func)
    local info = {
        content = content,
        confirm = {
            func = confirm_func
        }
    }
    return info
end
function GuildEmbattleMsgScheduleItem:OnClickGuessL1()
    if self.embattle_data:EntryQualify() then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTip)
        return
    end
    local base = self.embattle_data:Base()
    if base.round_state ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[5])
        return
    end
    if self.data[2] == nil then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[7])
        return
    end
    if self.data[1] then
        local is_win = self.data[1]:IsWin()
        if is_win then
            PublicPopupCtrl.Instance:DialogTips(
                self:DialogInfo(
                    Language.GuildEmbattle.GuessTips[1],
                    function()
                        GuildEmbattleCtrl.CSGuildHonorReq2(
                            GUILD_HONOR_REQ_TYPE_2.CANCEL_GUESSING,
                            self.data[1].guild_id,
                            self.data[1].match_info.win_rank
                        )
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                )
            )
        else
            is_win = self.data[2]:IsWin()
            if is_win then
                PublicPopupCtrl.Instance:DialogTips(
                    self:DialogInfo(
                        Language.GuildEmbattle.GuessTips[2],
                        function()
                            GuildEmbattleCtrl.CSGuildHonorReq2(
                                GUILD_HONOR_REQ_TYPE_2.GUESSING,
                                self.data[1].guild_id,
                                self.data[1].match_info.win_rank
                            )
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    )
                )
            else
                GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING, self.data[1].guild_id, self.data[1].match_info.win_rank)
            end
        end
    end
end
function GuildEmbattleMsgScheduleItem:IsWin(guild_id)
    local view_data = self.embattle_data:ViewData()
    --stage和round一致时才能竞猜
    return self.embattle_data:IsWinner(view_data.stage, guild_id)
end
function GuildEmbattleMsgScheduleItem:IsChampion(guild_id)
    return self.embattle_data:IsChampion(guild_id)
end
function GuildEmbattleMsgScheduleItem:OnClickGuessL2()
    if self.embattle_data:EntryQualify() then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTip)
        return
    end
    local base = self.embattle_data:Base()
    if base.round_state ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[5])
        return
    end
    if base.curr_round > 1 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[6])
        return
    end
    if self.data[1] then
        local is_champion = self.data[1]:IsChampion()
        if is_champion then
            PublicPopupCtrl.Instance:DialogTips(
                self:DialogInfo(
                    Language.GuildEmbattle.GuessTips[3],
                    function()
                        GuildEmbattleCtrl.CSGuildHonorReq2(
                            GUILD_HONOR_REQ_TYPE_2.CANCEL_GUESSING_CHAMPION,
                            self.data[1].guild_id,
                            self.data[1].match_info.win_rank
                        )
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                )
            )
        else
            local champion_id = self.embattle_data:GuessChampion()
            if champion_id ~= 0 then
                PublicPopupCtrl.Instance:DialogTips(
                    self:DialogInfo(
                        Language.GuildEmbattle.GuessTips[4],
                        function()
                            GuildEmbattleCtrl.CSGuildHonorReq2(
                                GUILD_HONOR_REQ_TYPE_2.GUESSING_CHAMPION,
                                self.data[1].guild_id,
                                self.data[1].match_info.win_rank
                            )
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    )
                )
            else
                GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING_CHAMPION, self.data[1].guild_id, self.data[1].match_info.win_rank)
            end
        end
    end
end
function GuildEmbattleMsgScheduleItem:OnClickGuessR1()
    local view_data = self.embattle_data:ViewData()
    if self.embattle_data:EntryQualify() then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTip)
        return
    end
    local base = self.embattle_data:Base()
    if base.round_state ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[5])
        return
    end
    if self.data[2] == nil then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[7])
        return
    end
    if self.data[2] then
        local is_win = self.data[2]:IsWin()
        if is_win then
            PublicPopupCtrl.Instance:DialogTips(
                self:DialogInfo(
                    Language.GuildEmbattle.GuessTips[1],
                    function()
                        GuildEmbattleCtrl.CSGuildHonorReq2(
                            GUILD_HONOR_REQ_TYPE_2.CANCEL_GUESSING,
                            self.data[2].guild_id,
                            self.data[2].match_info.win_rank
                        )
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                )
            )
        else
            is_win = self.data[1]:IsWin()
            if is_win then
                PublicPopupCtrl.Instance:DialogTips(
                    self:DialogInfo(
                        Language.GuildEmbattle.GuessTips[2],
                        function()
                            GuildEmbattleCtrl.CSGuildHonorReq2(
                                GUILD_HONOR_REQ_TYPE_2.GUESSING,
                                self.data[2].guild_id,
                                self.data[2].match_info.win_rank
                            )
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    )
                )
            else
                GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING, self.data[2].guild_id, self.data[2].match_info.win_rank)
            end
        end
    end
end
function GuildEmbattleMsgScheduleItem:OnClickGuessR2()
    if self.embattle_data:EntryQualify() then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTip)
        return
    end
    local base = self.embattle_data:Base()
    if base.round_state ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[5])
        return
    end
    if base.curr_round > 1 then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[6])
        return
    end
    if self.data[2] == nil then
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GuessTips[7])
        return
    end
    if self.data[2] then
        local is_champion = self.data[2]:IsChampion()
        if is_champion then
            PublicPopupCtrl.Instance:DialogTips(
                self:DialogInfo(
                    Language.GuildEmbattle.GuessTips[3],
                    function()
                        GuildEmbattleCtrl.CSGuildHonorReq2(
                            GUILD_HONOR_REQ_TYPE_2.CANCEL_GUESSING_CHAMPION,
                            self.data[2].guild_id,
                            self.data[2].match_info.win_rank
                        )
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                )
            )
        else
            local champion_id = self.embattle_data:GuessChampion()
            if champion_id ~= 0 then
                PublicPopupCtrl.Instance:DialogTips(
                    self:DialogInfo(
                        Language.GuildEmbattle.GuessTips[4],
                        function()
                            GuildEmbattleCtrl.CSGuildHonorReq2(
                                GUILD_HONOR_REQ_TYPE_2.GUESSING_CHAMPION,
                                self.data[2].guild_id,
                                self.data[2].match_info.win_rank
                            )
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    )
                )
            else
                GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.GUESSING_CHAMPION, self.data[2].guild_id, self.data[2].match_info.win_rank)
            end
        end
    end
end
function GuildEmbattleMsgScheduleItem:OnDestroy()
    self:uncareData()
end
function GuildEmbattleMsgScheduleItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
end

GuildEmbattleMsgRank = GuildEmbattleMsgRank or DeclareMono('GuildEmbattleMsgRank', UIWFlushPanel)
function GuildEmbattleMsgRank:GuildEmbattleMsgRank()
    self.act_data = ActivityData.Instance
    self.embattle_data = GuildEmbattleData.Instance
    self.data_cares = {
        {data = self.act_data:GetActivityEvent(), func = self.onFlush},
        {data = self.embattle_data:BattleProcess(), func = self.onFlush},
    }
end
function GuildEmbattleMsgRank:onFlush()
    local rank = self.embattle_data:GetLastRank()--GetRank()
    self.NoneRank:SetActive(table.nums(rank) == 0)
    self.List:SetData(rank)
end
GuildEmbattleMsgRankItem = GuildEmbattleMsgRankItem or DeclareMono('GuildEmbattleMsgRankItem', UIWidgetBaseItem)
function GuildEmbattleMsgRankItem:GuildEmbattleMsgRankItem()
    self.sp_list = {"DiYiMing","DiErMing","DiSanMing"}
    self.embattle_data = GuildEmbattleData.Instance
end
function GuildEmbattleMsgRankItem:SetData(data)
    local rank = data.match_info.win_rank or 0
    self.RankSp:SetObjActive(rank < 4)
    self.OtherRank:SetActive(rank > 3)
    if rank <4 then
        UH.SpriteName(self.RankSp, self.sp_list[rank])
    end
    UH.SetText(self.RankTex, data.match_info.win_rank)
    local guild_id = GuildData.Instance:GetGuildID()
    UH.SetText(self.Name, guild_id == data.guild_id and self.embattle_data:GuildName(data.name) or data.name)
    UH.SetText(
        self.Sever,
        self.embattle_data:ServerName(data.server_id)
    )
    UH.SetText(self.RoleName, data.role_name)
    local reward = self.embattle_data:GetRankReward(rank)
    self.List:SetData(reward)
end
