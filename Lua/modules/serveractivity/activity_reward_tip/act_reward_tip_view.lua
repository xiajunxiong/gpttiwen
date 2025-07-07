ActRewardTipView = ActRewardTipView or DeclareView("ActRewardTipView","serveractivity/activity_reward_tip")
function ActRewardTipView:ActRewardTipView()   
     self:FlushView()
     ActRewardTipData.Instance.is_open_view = true
end
function ActRewardTipView:FlushView()
    for i = 1,self.TipObjs:Length() do
        self.TipObjs[i]:SetActive(i == ActRewardTipData.Instance.reward_type)
    end
    local state = ActRewardTipData.Instance.is_success and 1 or 2
    for i = 1,self.BgObjs:Length() do
        self.BgObjs[i]:SetActive(i == state)
    end
    UH.SetText(self.FailText,ActRewardTipData.Instance.reward_type == ServeractivityConfig.ActRewerdTip.RewardType.Fail and Language.ActRewardTip.FailText or "")
    local index = ServeractivityConfig.ActRewerdTip.RewardTypeToIcon[ActRewardTipData.Instance.reward_type]
    self.RewardList[index]:SetData(ActRewardTipData.Instance:GetRewardData())
    for i = 1,self.RewardList:Length() do
        self.RewardList[i]:SetObjActive(i == index)
    end
    if index == 1 then
        UH.SetEnabled(self.ListScrollRect1, #ActRewardTipData.Instance:GetRewardData() > 5)
    end
    self.ExtraList:SetObjActive(#ActRewardTipData.Instance.extra_list > 0)
    self.ExtraList:SetData(ActRewardTipData.Instance.extra_list)
end
function ActRewardTipView:OnClickClose()
    ActRewardTipData.Instance.is_open_view = false
    ViewMgr:CloseView(ActRewardTipView)
end
function ActRewardTipView:CloseCallback()
    ActRewardTipData.Instance:FinishOpenView()
end

-----------Item------------
ActRewardTipItem = ActRewardTipItem or DeclareMono("ActRewardTipItem", UIWidgetBaseItem)
function ActRewardTipItem:SetData(data)
    self.num = data.num
    data.num = 0
    self.Cell:SetData(Item.New(data))
    local state = ActRewardTipData.Instance.is_success and 1 or 2
    local index = self.EffectCoror ~= nil and ActRewardTipData.Instance.reward_type or state
    UH.SetText(self.Num,string.format(Language.ActRewardTip.NumText[index],self.num))
    if self.EffectCoror then 
        self.EffectCoror.effectColor = state == 2 and COLORSTR.Yellow9 or COLORSTR.Gray1
    end
    if self.BgObjs then
        for i = 1,self.BgObjs:Length() do 
            self.BgObjs[i]:SetActive(i == state)
        end
    end
    if self.ItemPos then 
        local pos,angle = self:GetPosAndAngle(data.index)
        UH.LocalPos(self.ItemPos,pos)
        UH.LocalEulerAngles(self.ItemPos,angle)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ActRewardTipItem:GetPosAndAngle(index)
    local pos = Vector3.New(0,0,0)
    local angle = pos
    if #ActRewardTipData.Instance:GetRewardData() % 2 == 0 then
        local len = #ActRewardTipData.Instance:GetRewardData() + 1
        index = len - index
        index = #ActRewardTipData.Instance:GetRewardData() == 2 and index + 1 or index
        pos = ServeractivityConfig.ActRewerdTip.ItemPos2[index]
        angle = ServeractivityConfig.ActRewerdTip.ItemAngles2[index]
    else
        local len = #ServeractivityConfig.ActRewerdTip.ItemPos + 1
        index = len - index
        pos = ServeractivityConfig.ActRewerdTip.ItemPos[index]
        angle = ServeractivityConfig.ActRewerdTip.ItemAngles[index]
    end
    return pos,angle
end

