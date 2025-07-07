FunOpenNoticeGuideTaskUnit = FunOpenNoticeGuideTaskUnit or DeclareMono("FunOpenNoticeGuideTaskUnit", UIWFlushPanel)
function FunOpenNoticeGuideTaskUnit:FunOpenNoticeGuideTaskUnit()
    self.data = {}
end
function FunOpenNoticeGuideTaskUnit:SetData(data)
    self.data = data
    self.RewardList:SetData(data.reward_list)
    local param = ColorStr(data.task_param,data.complete and COLORSTR.Green4 or COLORSTR.Red8)
    UH.SetText(self.TaskParam,data.task_desc..string.format(Language.Notice.TaskShow,param, data.task_total ))
    self.BtnInter.Interactable = data.complete
    UH.SetText(self.OperText,not self.data.complete and Language.Notice.Goto or Language.Notice.CanForReward)
end
function FunOpenNoticeGuideTaskUnit:OnClickOper()
    if self.data.complete then
        if not BagData.Instance:TryFindEmptyInTempBagMulit(#self.data.reward_list) then 
            PublicPopupCtrl.Instance:Error(Language.Bag.BagFetchWithoutEmpty)
            return 
        end 

        FunOpen.Instance:SendFuncPreviewClaimReward(self.data.notice_id,2)
    else 
        ViewMgr:OpenViewByKey(self.data.mod_key)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end 