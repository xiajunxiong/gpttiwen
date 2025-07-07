ConcentricKnotTeamView = ConcentricKnotTeamView or DeclareView("ConcentricKnotTeamView", "period_activity_act/concentric_knot/concentric_knot_team")
VIEW_DECLARE_LEVEL(ConcentricKnotTeamView, ViewLevel.L1)

function ConcentricKnotTeamView:LoadCallback()
    self.Board:SetData2(self:GetType(), Language.ConcentricKnot.FBName, "", nil)
end

function ConcentricKnotTeamView:OnClickClose()
    local is_agree = ConcentricKnotData.Instance:GetIsAgreeByUID(RoleData.Instance:GetRoleId())
    if not is_agree then
        ConcentricKnotCtrl.Instance:SendAgree(0)
    end
end

ConcentricKnotTeamViewOffset = ConcentricKnotTeamViewOffset or DeclareMono("ConcentricKnotTeamViewOffset", UIWFlushPanel)
function ConcentricKnotTeamViewOffset:ConcentricKnotTeamViewOffset()
    self.data = ConcentricKnotData.Instance
    self.data_cares = {
        {data = self.data.team_info, func = self.FlushView, init_call = true}
    }
    self.wait_time = 15
end

function ConcentricKnotTeamViewOffset:Awake()
    UIWFlushPanel.Awake(self)
    
    local member_list = TeamCtrl.Instance:Data():GetTeamInfo():MemberListSort()
    for i = 1, self.TeamItems:Length() do
        self.TeamItems[i]:SetData(member_list[i])
    end
    UH.SetText(self.LeaveTex, Language.PublicPopup.ReplyType.refuse)
    UH.SetText(self.ConfirmTex, Language.PublicPopup.ReplyType.confirm)

    local timestamp = TimeHelper.GetServerTime() + self.wait_time
    self.play_time = self.wait_time
    self.timer_ct = TimeHelper:AddCountDownCT(BindTool.Bind1(self.TimerCTUpdate, self), BindTool.Bind1(self.TimerCTComplete, self), timestamp, 1)
end

function ConcentricKnotTeamViewOffset:TimerCTUpdate()
    self.CountdownProgress:SetProgress(self.play_time / self.wait_time)
    self.CountdownNum.text = self.play_time .. Language.Team.Second
    self.play_time = self.play_time - 1
end

function ConcentricKnotTeamViewOffset:TimerCTComplete()
    ViewMgr:CloseView(ConcentricKnotTeamView)
end

function ConcentricKnotTeamViewOffset:FlushView()
    for i = 1, self.TeamItems:Length() do
        self.TeamItems[i]:FlushReady()
    end
    local is_agree = ConcentricKnotData.Instance:GetIsAgreeByUID(RoleData.Instance:GetRoleId())
    self.BtnConfirmInter.Interactable = not is_agree
    self.BtnLeaveInter.Interactable = not is_agree

    if ConcentricKnotData.Instance:IsAllAgree() then
        TimeHelper:CancelTimer(self.timer_ct)
        local function func_agree()
            ViewMgr:CloseView(ConcentricKnotTeamView)
            ViewMgr:OpenView(ConcentricKnotView)
        end
        Invoke(func_agree)
    end
    local is_over = ConcentricKnotData.Instance:GetRemainTimes() <= 0
    self.NotCanTip:SetActive(is_over)
    if is_over then
        UH.SetText(self.NotCanText, Language.ConcentricKnot.TeamError2)
    end
end

function ConcentricKnotTeamViewOffset:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function ConcentricKnotTeamViewOffset:OnClickLeave()
    ConcentricKnotCtrl.Instance:SendAgree(0)
end

function ConcentricKnotTeamViewOffset:OnClickConfirm()
    ConcentricKnotCtrl.Instance:SendAgree(1)
end

ConcentricKnotTeamItem = DeclareMono("ConcentricKnotTeamItem", UIWidgetBaseItem)
function ConcentricKnotTeamItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data and data.info or nil
    if nil == info or 1 == info.is_partner then
        self.ItemObj:SetActive(false)
    else
        self.ItemObj:SetActive(true)
        self.Name.alignment = string.len(info.name) > 15 and 0 or 1
        self.Name.text = info.name
        self.Level.text = info.level
        UH.SetAvatar(self.IconSp, info.appearance.avatar_type, info.appearance.avatar_id, info.appearance.avatar_quality)
    end
    self:FlushReady()
end

function ConcentricKnotTeamItem:FlushReady()
    if self.data == nil or self.data.info == nil then
        return
    end
    self.ReadyObj:SetActive(ConcentricKnotData.Instance:GetIsAgreeByUID(self.data.info.uid))
end
