
SocietyShiTuFBGuideView = SocietyShiTuFBGuideView or DeclareView("SocietyShiTuFBGuideView", SocietyConfig.ResPath .. "society_shitu_fb_guide")

function SocietyShiTuFBGuideView:SocietyShiTuFBGuideView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyShiTuFBGuideView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyShiTuFB, Vector2.New(860, 450))
end

SocietyShiTuFBGuideViewF = SocietyShiTuFBGuideViewF or DeclareMono("SocietyShiTuFBGuideViewF", UIWFlushPanel)

function SocietyShiTuFBGuideViewF:SocietyShiTuFBGuideViewF()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuFB
end

function SocietyShiTuFBGuideViewF:Awake()
    UIWFlushPanel.Awake(self)

    self.GuideTips.text = self.language.GuideTips
    self.RewardList:SetData(self.data:GetShiTuFBRewardShow())
end

function SocietyShiTuFBGuideViewF:OnClickInvite()
    -- SocietyCtrl.Instance:SendShiTuReqInvite()
    ViewMgr:OpenView(TeamInviteShiTuView)
end

function SocietyShiTuFBGuideViewF:OnClickFight()
    -- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SHITU_FB)
    -- ViewMgr:CloseView(SocietyShiTuFBGuideView)
    SceneLogic.Instance:AutoToNpc(self.data:GetShiTuFBNpcSeq(), nil, nil, true)
    ViewMgr:CloseView(SocietyShiTuFBGuideView)
    ViewMgr:CloseView(SocietyView)
end

SocietyShiTuFBGuideViewItem = DeclareMono("SocietyShiTuFBGuideViewItem", UIWidgetBaseItem)

function SocietyShiTuFBGuideViewItem:SocietyShiTuFBGuideViewItem()
	self.society_data = SocietyData.Instance
end

function SocietyShiTuFBGuideViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    self.GetObj:SetActive(self.society_data.is_get_shi_tu_reward)
end
