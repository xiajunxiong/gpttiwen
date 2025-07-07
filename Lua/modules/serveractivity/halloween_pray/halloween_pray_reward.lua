HalloweenPrayReward = HalloweenPrayReward or DeclareView("HalloweenPrayReward", "serveractivity/halloween_pray/halloween_pray_reward")
VIEW_DECLARE_LEVEL(HalloweenPrayReward, ViewLevel.Lt)
VIEW_DECLARE_MASK(HalloweenPrayReward, ViewMask.BgBlockClose)

function HalloweenPrayReward:HalloweenPrayReward()
end

function HalloweenPrayReward:LoadCallback(param_t)
    self.Panels[ HalloweenPrayData.Instance:GetShowStyle()]:SetObjActive(true)
    self.Panels[ HalloweenPrayData.Instance:GetShowStyle()]:InitShow(param_t)
end

HalloweenPrayRewardPanel = HalloweenPrayRewardPanel or DeclareMono("HalloweenPrayRewardPanel", UIWFlushPanel)
function HalloweenPrayRewardPanel:HalloweenPrayRewardPanel()
end

function HalloweenPrayRewardPanel:InitShow(param_t)
    self.List:SetData(param_t.item_list)
    self.ListEffect:SetData(param_t.item_list)
    local style= HalloweenPrayData.Instance:GetShowStyle();
    local pos_big={{bg=-27,list=-28,bg_height=498},{bg=-13,list=-49,bg_height=504}}
    local pos_small={{bg=-18.5,list=-22,bg_height=307},{bg=10,list=-28,bg_height=378}}
    local pos ;
    if #param_t.item_list > 5 then
        pos=pos_big[style];
    else
        pos=pos_small[style];
    end
    UH.SetRectPosition(self.RectBg, {y = pos.bg})
    UH.SetRectPosition(self.RectList, {y = pos.list})
    UH.SetRectPosition(self.RectListEffect, {y =  pos.list})
    self.RectBg.sizeDelta = Vector2.New(1255, pos.bg_height)
    PublicPopupCtrl.Instance:ReleaseShieldParam(true)
    AchieveCtrl.Instance:CloseDelayCourseRemind()
end

--Item
HalloweenPrayRewardItem = HalloweenPrayRewardItem or DeclareMono("HalloweenPrayRewardItem", UIWidgetBaseItem)

function HalloweenPrayRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data.item)
    UH.SetText(self.TextName, data.item:QuaName())
    if self.ImgXiYou ~= null then
         self.ImgXiYou:SetActive(data.is_xiyou)
    end
end

--Item2 特效用
HalloweenPrayRewardItem2 = HalloweenPrayRewardItem2 or DeclareMono("HalloweenPrayRewardItem2", UIWidgetBaseItem)

function HalloweenPrayRewardItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjXiYou:SetActive(data.is_xiyou)
end
