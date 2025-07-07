
SocietyShiTuAchieveGuideView = SocietyShiTuAchieveGuideView or DeclareView("SocietyShiTuAchieveGuideView", SocietyConfig.ResPath .. "society_shitu_achieve_guide")

function SocietyShiTuAchieveGuideView:SocietyShiTuAchieveGuideView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyShiTuAchieveGuideView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyShiTuAchieve, Vector2.New(860, 450))
end

SocietyShiTuAchieveGuideViewF = SocietyShiTuAchieveGuideViewF or DeclareMono("SocietyShiTuAchieveGuideViewF", UIWFlushPanel)

function SocietyShiTuAchieveGuideViewF:SocietyShiTuAchieveGuideViewF()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuInfoAchieve
end

function SocietyShiTuAchieveGuideViewF:Awake()
    UIWFlushPanel.Awake(self)

    self.GuideTips.text = self.language.GuideTips
    self.RewardList:SetData(self.data:GetShiTuAchieveRewardShow())
end

function SocietyShiTuAchieveGuideViewF:OnClickInvite()
    SocietyCtrl.Instance:SendShiTuReqInvite()
end

function SocietyShiTuAchieveGuideViewF:OnClickFight()
    -- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SHITU_FB)
    -- ViewMgr:CloseView(SocietyShiTuFBGuideView)
    SceneLogic.Instance:AutoToNpc(self.data:GetShiTuFBNpcSeq(), nil, nil, true)
    ViewMgr:CloseView(SocietyShiTuFBGuideView)
    ViewMgr:CloseView(SocietyView)
end

SocietyShiTuAchieveGuideViewItem = DeclareMono("SocietyShiTuAchieveGuideViewItem", UIWidgetBaseItem)

function SocietyShiTuAchieveGuideViewItem:SocietyShiTuAchieveGuideViewItem()
	self.society_data = SocietyData.Instance
end

function SocietyShiTuAchieveGuideViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    -- self.GetObj:SetActive(self.society_data.is_get_shi_tu_reward)
end
