BattlefieldView = BattlefieldView or DeclareView("BattlefieldView", "battlefield/battlefield_view")

VIEW_DECLARE_LEVEL(BattlefieldView,ViewLevel.L0)

function BattlefieldView:BattlefieldView()
    self.data = BattlefieldData.Instance
end

function BattlefieldView:OpenCallback()
    BattlefieldCtrl.Instance:SendAllInfoReq()
    AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.FIRST_ENTER)
end

function BattlefieldView:CloseCallback()
    RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Society, self.handle)
    TimeHelper:CancelTimer(self.effect_time)
end

function BattlefieldView:LoadCallback()
    self:FlushTimeView()
    self:FlushRedpiontView()
    self:FlushMatchState()
end

function BattlefieldView:OnFlush()
	self:FlushMatchState()
end

function BattlefieldView:FlushTimeView()
    local TimeTips = Language.Battlefield.TimeTips
    local season_str = Format(TimeTips[1],DataHelper.GetDaXie(self.data:GetSeasonNum()))
    self.Time:SetShowCallBack(function(t)return self.data:GetConditionTimeTip(t)end)
    self.Time:StampTime(self.data:GetStampTime(),nil,season_str .. TimeTips[2])
end

function BattlefieldView:FlushRedpiontView()
    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.SocietyBtn, Vector3.New(26,26,0))
    end
    self.handle = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Society,function()
        self.red_point:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(Mod.Society))
    end, true)
end

function BattlefieldView:FlushMatchState()
    local is_open = ActivityData.IsOpen(ActType.Battlefield)
    self.LeftInter.Interactable = is_open
    if not is_open then
        local config = ActivityData.GetConfig(ActType.BattlefieldAgain)
        UH.SetText(self.TipsText,Language.Battlefield.MatchOpenTip .. config.time_text)
    end
    self.Tips:SetActive(not is_open)
end

function BattlefieldView:OnClickClose()
    for i=1,2 do
        self.Effect[i]:SetActive(false)
    end
    ViewMgr:CloseView(BattlefieldView)
end

function BattlefieldView:OnClickSociety()
    ViewMgr:OpenView(SocietyView)
end

function BattlefieldView:OnClickShop()
    ViewMgr:OpenView(BattlefieldShopView)
end

function BattlefieldView:OnClickAnother()
    ViewMgr:OpenView(AnotherView)
end

function BattlefieldView:OnClickReward()
    ViewMgr:OpenView(BattlefieldRewardView)
end

function BattlefieldView:OnClickToggle(type)
end

function BattlefieldView:OnClickRank()
    ViewMgr:OpenView(BattlefieldRankView,BattlefieldRankType.Cross)
end

function BattlefieldView:OnClickHelp()
    ViewMgr:OpenView(AnimalRaceRuleView,{desc = Config.language_cfg_auto.textdesc[65].desc})
end

function BattlefieldView:OnClickTeamMath()
    PublicPopupCtrl.Instance:Center(Language.Battlefield.HotOpenTip)
end

function BattlefieldView:OnClickMath()
    if not ActivityData.IsOpen(ActType.Battlefield) then
		PublicPopupCtrl.Instance:Center(Language.Battlefield.MatchNotOpenTip)
		return
    end
    if not BattlefieldCtrl.Instance:IsHasTeam() then
        return
    end
    self.EffectTool:Play(10010890)
    self.effect_time = TimeHelper:AddDelayTimer(function()
        ViewMgr:OpenView(BattlefieldMatch)
    end, 0.5)
end

--===========================BattlefieldPanel===========================
BattlefieldPanel = BattlefieldPanel or DeclareMono("BattlefieldPanel", UIWFlushPanel)

function BattlefieldPanel:BattlefieldPanel()
    self.data = BattlefieldData.Instance
    self.data_cares = {
        {data = self.data.guide_data, func = self.FlushMaskView,init_call = false},
    }
end

function BattlefieldPanel:Awake()
    UIWFlushPanel.Awake(self)
    if AnotherData.IsGuide() then
        ViewMgr:OpenView(GuideAnother)
    end
end

function BattlefieldPanel:FlushMaskView()
    self.Mask:SetActive(self.data:GetGuideData())
end

function BattlefieldPanel:OnClickMask()
    self.data:SetGuideData(false)
    if UnityPlayerPrefs.GetInt(PrefKeys.BattlefieldMatch()) == 0 and self.data:GetTopRace() == 0 then
        self.MaskItem:SetActive(true)
    end
end

function BattlefieldPanel:OnClickEndMask()
    self.MaskItem:SetActive(false)
    UnityPlayerPrefs.SetInt(PrefKeys.BattlefieldMatch(),1)
end

--===========================BattlefieldChatPanel===========================
BattlefieldChatPanel = BattlefieldChatPanel or DeclareMono("BattlefieldChatPanel", UIWFlushPanel)

function BattlefieldChatPanel:BattlefieldChatPanel()
	self.data = ChatCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetChatChannelList(), func = self.FlushRecordList},
	}
end

function BattlefieldChatPanel:FlushRecordList()
    self.RecordList:SetDataList(self.data:GetMainViewChatList())
    self.RecordList:JumpToDown()
end

function BattlefieldChatPanel:OnClickSociety()
    ViewMgr:OpenView(SocietyView)
end

function BattlefieldChatPanel:OnClickSetting()
	ViewMgr:OpenView(ChatViewChannelSel)
end

function BattlefieldChatPanel:OnClickList()
    ViewMgr:OpenView(ChatView)
end