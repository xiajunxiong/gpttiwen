-- 追月商城 恭喜获得界面
ZyscRewardView = ZyscRewardView or DeclareView("ZyscRewardView", "serveractivity/act_zysc/zysc_reward")
VIEW_DECLARE_LEVEL(ZyscRewardView, ViewLevel.Lt)
-- VIEW_DECLARE_MASK(ZyscRewardView, ViewMask.BgBlockClose)

function ZyscRewardView:ZyscRewardView()

end

function ZyscRewardView:LoadCallback(param_t)
    if ActZyscCtrl.Instance.ToggleSkip then
        self:OnEffectEnd(nil)
    else
        self.AnimationGP:Play("zysc_reward_801_O")
        UH.PlayEffect(self.UiEffect, 6165328)
    end
    self.List:SetData(param_t)
    self.List:SetCompleteCallBack(function()
        self.ListName:SetData(param_t)
    end)
end

function ZyscRewardView:OnClose()
    ViewMgr:CloseView(ZyscRewardView)
end

function ZyscRewardView:OnEffectEnd(param)
    self.ViewBlock:SetActive(true)
end

function ZyscRewardView:PlayBigReward(obj)
    return UH.PlayEffect(self.UiEffect, 6165329, obj)
end

function ZyscRewardView:StopBigReward(handle)
    UH.StopEffect(self.UiEffect, handle)
end

function ZyscRewardView:CloseCallback()
end

function ZyscRewardView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data = nil
end
----------------------------ZyscRewardItem----------------------------
ZyscRewardItem = ZyscRewardItem or DeclareMono('ZyscRewardItem', UIWidgetBaseItem)
function ZyscRewardItem:SetData(data)
    -- local reward = nil
    -- if data == -1 then
    --     reward = ActZyscData.Instance.SC_ZqzyInfo.data_Reward_Big.reward_item

    -- else
    --     reward = data.reward_item
    -- end
    local Ut_Item = data.item
    -- 屏蔽推荐标签
    -- Ut_Item.IsGem = false
    self.Cell:SetData(Ut_Item)
    UH.SetText(self.Text, Ut_Item:QuaName())
end

function ZyscRewardItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
end

----------------------------ZyscRewardItem----------------------------
ZyscRewardNameItem = ZyscRewardNameItem or DeclareMono('ZyscRewardNameItem', UIWidgetBaseItem)
function ZyscRewardNameItem:SetData(data)
    if data.reward_index == -1 and self.EffectHandle == nil then -- 处理大奖特效
        self.EffectHandle = self.Parent:PlayBigReward(self.ObjSelf)
    end
end

function ZyscRewardNameItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.EffectHandle ~= nil then
        self.Parent:StopBigReward(self.EffectHandle)
    end
end
