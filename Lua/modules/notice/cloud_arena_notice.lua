CloudArenaNotice = CloudArenaNotice or DeclareView("CloudArenaNotice", "notice/cloud_arena_notice", Mod.FunOpenNotice.CloudArena)
VIEW_DECLARE_LEVEL(CloudArenaNotice, ViewLevel.L1)
CloudArenaNoticePanel = CloudArenaNoticePanel or DeclareMono("CloudArenaNoticePanel", UIWFlushPanel)
function CloudArenaNoticePanel:onFlush()
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.CloudArena)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
function CloudArenaNoticePanel:OnClickClose()
    ViewMgr:CloseView(CloudArenaNotice)
end
function CloudArenaNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    elseif flag == FunPreNoticeEnum.Rewarded then
        CloudArenaCtrl.Instance:OpenCloudArenaView()
        -- ViewMgr:OpenViewByKey(Mod.CloudArena.Main)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function CloudArenaNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end

    self.GuideUnit:SetData(unit_param)
end