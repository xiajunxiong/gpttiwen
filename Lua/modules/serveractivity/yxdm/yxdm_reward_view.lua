-- 月夕灯谜 恭喜获得界面
YxdmRewardView = YxdmRewardView or DeclareView("YxdmRewardView", "serveractivity/yxdm/yxdm_reward")
VIEW_DECLARE_LEVEL(YxdmRewardView, ViewLevel.Lt)
VIEW_DECLARE_MASK(YxdmRewardView, ViewMask.BgBlockClose)

function YxdmRewardView:YxdmRewardView()
end

function YxdmRewardView:LoadCallback(param_t)
    local item_list = BagData.Instance:ItemListBuild(param_t.item_list or {})
    self.List:SetData(item_list)
end

function YxdmRewardView:CloseCallback()
    if ViewMgr:IsOpen(YxdmCmView) then
        ViewMgr:FlushView(YxdmCmView)
    end
end
----------------------------YxdmRewardPanel----------------------------
YxdmRewardPanel = YxdmRewardPanel or DeclareMono("YxdmRewardPanel", UIWFlushPanel)
function YxdmRewardPanel:YxdmRewardPanel()
end
