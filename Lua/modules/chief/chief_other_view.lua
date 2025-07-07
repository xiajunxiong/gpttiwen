--===================================ChiefWaitRespondView===================================
ChiefWaitRespondView = ChiefWaitRespondView or DeclareView("ChiefWaitRespondView", "chief/ChiefWaitRespondView")

function ChiefWaitRespondView:LoadCallback(param_t)
    BuglyReport("玩家首席对战打开了界面 role_name=",RoleData.Instance:GetRoleName())
end

--===================================ChiefWaitRespondPanel===================================
ChiefWaitRespondPanel = ChiefWaitRespondPanel or DeclareMono("ChiefWaitRespondPanel", UIWFlushPanel)

function ChiefWaitRespondPanel:ChiefWaitRespondPanel()
    self.data = ChiefCtrl.Instance:GetData()
end

function ChiefWaitRespondPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:SetCallBack(function()
        ViewMgr:CloseView(ChiefWaitRespondView)
        ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,5)
        BuglyReport("玩家首席对战时间到了自动关闭界面 玩家自己没点进去 role_name=",RoleData.Instance:GetRoleName())
    end)
    self.Time:StampTime(self.data.info_data.end_time,TimeType.Type_Special_0)
    UH.SetText(self.Prof,Language.Common.Prof[RoleData.Instance:GetRoleProf()])
end

function ChiefWaitRespondPanel:OnClickEnter()
    ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.READY_REQ)
    ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,6)
    BuglyReport("首席 服务端说没有发送请求 role_name=",RoleData.Instance:GetRoleName())
    ViewMgr:CloseView(ChiefWaitRespondView)
end

function ChiefWaitRespondPanel:OnClickCancel()
    MainOtherData.Instance:SetRightMenuData({key = "Chief",is_active = true})
    ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,7)
    ViewMgr:CloseView(ChiefWaitRespondView)
end

--观战界面
--===================================ChiefWatchView===================================
ChiefWatchView = ChiefWatchView or DeclareView("ChiefWatchView", "chief/ChiefWatchView")

VIEW_DECLARE_MASK(ChiefWatchView,ViewMask.BgBlock)

function ChiefWatchView:LoadCallback(info)
    if info ~= nil and #info > 0 then
        self:FlushWatchView(info)
    end
    self:FlushWatchRound()
end

function ChiefWatchView:FlushWatchRound()
    local match_round = ChiefCtrl.Instance:GetData():GetCurrRound()
    local watch_tip = ChiefCtrl.Instance:IsElection()and Language.ChiefElection.WatchTip or Language.ChiefPeakBattle.WatchTip
    UH.SetText(self.WatchTip,Format(watch_tip[0],watch_tip[match_round]))
    if ChiefCtrl.Instance:IsElection() then
        UH.SpriteName(self.Title,ChiefConfig.MatchTitle .. RoleData.Instance:GetRoleProf())
    else
        UH.SpriteName(self.Title,"DianFengDuiZhan")
    end
end

function ChiefWatchView:FlushWatchView(info)
    for i=1,2 do
        self.CellList[i]:SetData(info[i]) 
    end
    local info = info[GameMath.Ramdon(1,2)]--随机
    self.send_uid = info.role_id or 0
end

function ChiefWatchView:OnClickExit()
    self.Anim:Play("hided")
    ViewMgr:CloseView(ChiefWatchView)
end

function ChiefWatchView:OnClickSend()
    BattleCtrl.Instance:SendWatchBattleReq(self.send_uid)
    ViewMgr:CloseView(ChiefWatchView)
    if ChiefCtrl.Instance:IsElection() then
        ViewMgr:CloseView(ChiefElectionMatch)
    else
        ViewMgr:CloseView(ChiefPeakBattleMatch)
    end
end

----------------------------ChiefWatchInfoItem----------------------------
ChiefWatchInfoItem = ChiefWatchInfoItem or DeclareMono("ChiefWatchInfoItem", UIWidgetBaseItem)

function ChiefWatchInfoItem:SetData(data)
    UH.SetText(self.Name,data.role_name)
    UH.SpriteName(self.Prof, ProfSettingB[data.prof])
    UH.SetAvatar(self.Head, data.avatar_type,data.avatar_id,data.avatar_quality)
    UH.SetText(self.Level,data.level)
end

--==================================ChiefFinishView===================================
ChiefFinishView = ChiefFinishView or DeclareView("ChiefFinishView", "chief/ChiefFinishView")

function ChiefFinishView:OnClickClose()
    ViewMgr:CloseView(ChiefFinishView)
end

function ChiefFinishView:OnClickMail()
    ViewMgr:OpenViewByKey(Mod.Society.Mail)
    ViewMgr:CloseView(ChiefFinishView)
end

--首席图标
--===================================ChiefTimeFlushView===================================
ChiefTimeFlushView = ChiefTimeFlushView or DeclareMono("ChiefTimeFlushView", UIWFlushPanel)

function ChiefTimeFlushView:OnEnable()
    if not ChiefCtrl.Instance:GetData():IsLeaveFlag() then
        local end_time = ChiefCtrl.Instance:GetData().info_data.end_time
        if end_time ~= 0 then
            self.Time:SetCallBack(function()self.Root:SetActive(false)end)
            self.Time:StampTime(end_time,TimeType.Type_Special_0)
        else
            self.Root:SetActive(false)
        end
    else
        self.Root:SetActive(false)
    end
end

function ChiefTimeFlushView:OnClickEnter()
    ViewMgr:OpenView(ChiefWaitRespondView)
end