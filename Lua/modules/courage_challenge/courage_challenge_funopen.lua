CourageChallengeFunOpenView = CourageChallengeFunOpenView or DeclareView("CourageChallengeFunOpenView", "courage_challenge/courage_challenge_funopen",Mod.FunOpenNotice.CourageChallenge)
VIEW_DECLARE_LEVEL(CourageChallengeFunOpenView, ViewLevel.L1)
function CourageChallengeFunOpenView:LoadCallback()
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.CourageChallenge)
    self.Grid:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local notice = GuideData.Instance:NoticeInfo(self.config.notice_id)
    self.BtnInter.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level
    UH.SetText(self.Level,Format(Language.Notice.GetLevel,self.config.end_level))
    self.notice_id = self.config.notice_id
end

function CourageChallengeFunOpenView:OnClickClose()
    ViewMgr:CloseView(CourageChallengeFunOpenView)
end

function CourageChallengeFunOpenView:OnClickReward()
    if GuideData.Instance:NoticeInfo(self.notice_id) == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
        return
    end
    FunOpen.Instance:SendFuncPreviewClaimReward(self.notice_id)
    ViewMgr:CloseView(CourageChallengeFunOpenView)
    GuideData.Instance:CheckNext(self.config.sn)
end