ArenaNotice = ArenaNotice or DeclareView("ArenaNotice", "notice/arena_notice", Mod.FunOpenNotice.Arena)
VIEW_DECLARE_LEVEL(ArenaNotice, ViewLevel.L1)
ArenaNoticePanel = ArenaNoticePanel or DeclareMono("ArenaNoticePanel", UIWFlushPanel)
function ArenaNoticePanel:onFlush()
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Arena)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    -- self.Tip1.text = Language.Notice.ArenaTip
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num,self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
function ArenaNoticePanel:OnClickClose()
    ViewMgr:CloseView(ArenaNotice)
end
function ArenaNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else
        ViewMgr:OpenViewByKey(Mod.Arena.Main) 
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function ArenaNoticePanel:FlushGuideUnit()
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