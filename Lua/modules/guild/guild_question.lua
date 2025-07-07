GuildQuestion = GuildQuestion or DeclareView("GuildQuestion", "guild/guild_question")
VIEW_DECLARE_MASK(GuildQuestion, ViewMask.None)
function GuildQuestion:LoadCallback()
    GuildData.Instance.question_show.show_panel = true
end
GuildQuestionPanel = GuildQuestionPanel or DeclareMono("GuildQuestionPanel", UIWFlushPanel)
function GuildQuestionPanel:GuildQuestionPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data:QuestionInfo(), func = self.FlushQuestionInfo}
    }
end
function GuildQuestionPanel:FlushQuestionInfo()
    local info = self.data:QuestionInfo()
    if info.cur_seq == nil then
        return
    end
    if info.cur_seq and info.cur_seq > self.data:TotalQuestionNum() and info.question_id <= 0 then
        self.QueObj:SetActive(false)
        self.BarObj:SetActive(false)
        self.FinishObj:SetActive(true)
        return
    end
    if info.cur_seq and info.cur_seq == -1 then --缓冲时间
        --显示等待时间
        self.QueObj:SetActive(false)
        self.BarObj:SetActive(true)
        UH.SetText(self.LblCount, Language.Guild.AnswerWait)
        TimeHelper:CancelTimer(self.handle_countdown)
        local total_time = info.next_time - TimeManager.GetServerTime()
        self.cur_time = total_time
        self.handle_countdown =
            TimeHelper:AddCountDownTT(
            function()
                self.cur_time = self.cur_time - 1
                self.cur_time = self.cur_time < 0 and 0 or self.cur_time
                self.TimeBar:SetProgress(self.cur_time / total_time)
                self.TimeBar:SetText(string.format("%ds", self.cur_time))
            end,
            function()
            end,
            total_time,
            1
        )
        return
    end

    local cfg = self.data:QuestionConfig(info.question_id)
    if cfg == nil then
        LogError("无配置 id:", info.question_id)
        return
    end
    self.QueObj:SetActive(true)
    self.BarObj:SetActive(true)
    UH.SetText(self.LblTitle, cfg.question)
    local temp = {}
    for i, v in ipairs(Language.Guild.AnswerList) do
        table.insert(temp, string.format(v, cfg["anwser_" .. i]))
    end
    UH.SetText(self.LblAnswer, string.concat(temp, "\n"))
    UH.SetText(self.LblCount, string.format(Language.Guild.AnswerNum, info.cur_seq + 1, self.data:TotalQuestionNum()))

    for i=1,4 do
        self.AnswerList[i]:SetActive(false)
    end
    TimeHelper:CancelTimer(self.handle_countdown)
    local total_time = info.next_time - TimeManager.GetServerTime() -- self.data:AnswerTime()
    self.cur_time = total_time
    self.handle_countdown =
        TimeHelper:AddCountDownTT(
        function()
            self.cur_time = self.cur_time - 1
            self.cur_time = self.cur_time < 0 and 0 or self.cur_time
            if self.cur_time <= 0.888 then
                for i=1,4 do
                    self.AnswerList[i]:SetActive(i == cfg.answer_id)
                end
            end
            self.TimeBar:SetProgress(self.cur_time / total_time)
            self.TimeBar:SetText(string.format("%ds", self.cur_time))
        end,
        function()
            if info.cur_seq + 1 >= self.data:TotalQuestionNum() then
                self.EndObj:SetActive(true)
                self.FinishObj:SetActive(true)
                self.BarObj:SetActive(false)
                self.QueObj:SetActive(false)
                self.handle_delay =
                    TimeHelper:AddDelayTimer(
                    function()
                        self.EndObj:SetActive(false)
                    end,
                    2
                )
            end
        end,
        total_time,
        1
    )
end
function GuildQuestionPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_countdown)
    TimeHelper:CancelTimer(self.handle_delay)
    GuildData.Instance.question_show.show_panel = false
end
function GuildQuestionPanel:OnClickPreview()
    local reward_list = self.data:GetAnswerTotalPreReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward_list)
end

GuildAnswerItem = GuildAnswerItem or DeclareMono("GuildAnswerItem", UIWFlushPanel)
function GuildAnswerItem:GuildAnswerItem()
    self.enter_answer = false
end
function GuildAnswerItem:OnTriggerEnter()
    self.SelectObj:SetActive(true)
    GuildCtrl.GuildAnswerAnswerQuestion(self.Id)
end
function GuildAnswerItem:OnTriggerExit()
    self.SelectObj:SetActive(false)
    GuildCtrl.GuildAnswerAnswerQuestion(0)
end
function GuildAnswerItem:PosCheck()
    --3.35
    if MainPlayer == nil then
        TimeHelper:CancelTimer(self.handle_update)
        return
    end
    local p1 = self:RealObjPos()-- MainPlayer:GetPos()
    if p1 == nil then
        return
    end
    local p2 = self.gameObject:GetLocalPosition()
    local distance = GameMath.GetDistance(p1.x,p1.z,p2.x,p2.z)
    if distance <= 15.2225 then
        self.enter_answer = true
        self.SelectObj:SetActive(true)
        GuildCtrl.GuildAnswerAnswerQuestion(self.Id)
    else        
        self.SelectObj:SetActive(false)
        if self.enter_answer == true then
            GuildCtrl.GuildAnswerAnswerQuestion(0)
            self.enter_answer = false
        end
    end
end
function GuildAnswerItem:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.handle_update = TimeHelper:AddRunTimer(function ()
        self:PosCheck()
    end, 0.2)
end
function GuildAnswerItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_update)
end
function GuildAnswerItem:RealObjPos()
    --[[ local inScene = GuildData.Instance:IsInAnswerScene() ]]
    local inTeam = TeamData.Instance:InTeam()
    local leader = TeamData.Instance:GetTeamInfo().leader
    local follow = TeamData.Instance:IsFollow()
    --[[ local v1 = Vector3.New(0.5, 2.5, 0.5)
    local v2 = Vector3.New(1.5, 2.5, 1.5) ]]
    --[[ if inScene then ]]
        if inTeam and follow then
            local uid = leader.info.uid
            local role_id = RoleData.Instance:GetRoleId()
            local obj = SceneCtrl.Instance:Objs():GetCharaByUid(uid)
            if obj then
                return obj:GetPos()
            end
            if MainPlayer and role_id == uid then
               return MainPlayer:GetPos()
            end
        else
            return MainPlayer:GetPos()
            --[[ for _, obj in pairs(self.net_roles) do
                obj:SetTouchAble(false)
            end ]]
            --只有主角可以
            --[[ if MainPlayer then
                MainPlayer:SetTouchAble(true, v1)
            end ]]
        end
    --[[ else
        for _, obj in pairs(self.net_roles) do
            obj:SetTouchAble(true, v2)
        end
        if MainPlayer then
            MainPlayer:SetTouchAble(false)
        end ]]
    --[[ end ]]
end
GuildQuestionChatPanel =GuildQuestionChatPanel or DeclareMono("GuildQuestionChatPanel", UIWFlushPanel)
function GuildQuestionChatPanel:GuildQuestionChatPanel()
    self.data = RoleSettingData.Instance
    self.max_num = 255
end
function GuildQuestionChatPanel:OnEnable()
    local num = self.data:GetData(ROLE_SETTING_TYPE.GUILD_ANSWER_SHOW_ROLE_NUM)
    self.Slider.value = num / self.max_num
    if self.handle_touch == nil then
        self.handle_touch = ViewMgr:CareTouchDownEvent(BindTool.Bind(self.OnTouchEvent, self))
    end
end
function GuildQuestionChatPanel:OnDisable()
    if self.handle_touch then
        ViewMgr:UncareTouchDownEvent(self.handle_touch)
        self.handle_touch = nil
    end
end
function GuildQuestionChatPanel:OnTouchEvent()
    if ViewMgr.touch_down_data.type ~= ViewManager.TouchDownType.GQRN then
        self.LineObj:SetActive(false)
        RoleSettingCtrl.Instance:SaveRoleSettingInfo()
    end
end
function GuildQuestionChatPanel:OnClickPress()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.GQRN)
end
function GuildQuestionChatPanel:OnValueChange()
    local num = math.floor(self.Slider.value * self.max_num)
    self.data:SetData(ROLE_SETTING_TYPE.GUILD_ANSWER_SHOW_ROLE_NUM, num)
    UH.SetText(self.RoleNum, Format(Language.RoleSetting.Role2, num))
end
function GuildQuestionChatPanel:OnClickRoleNum()
    self.LineObj:SetActive(not self.LineObj.activeSelf)
    if not self.LineObj.activeSelf then
        RoleSettingCtrl.Instance:SaveRoleSettingInfo()
    end
end