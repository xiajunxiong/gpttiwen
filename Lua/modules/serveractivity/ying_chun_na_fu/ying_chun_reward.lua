YingChunReward = YingChunReward or DeclareView("YingChunReward", 'serveractivity/ying_chun_na_fu/ying_chun_reward')
VIEW_DECLARE_LEVEL(YingChunReward, ViewLevel.Lt)
VIEW_DECLARE_MASK(YingChunReward, ViewMask.BgBlockClose)

function YingChunReward:YingChunReward()
end

function YingChunReward:LoadCallback(param_t)
end

function YingChunReward:OnClickClose()
    ViewMgr:CloseView(YingChunReward)
end

YingChunRewardPanel = YingChunRewardPanel or DeclareMono("YingChunRewardPanel", UIWFlushPanel)

function YingChunRewardPanel:YingChunRewardPanel()
    self.data = YingChunData.Instance
end

function YingChunRewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushList()
end

function YingChunRewardPanel:FlushList()
    local notice_info = self.data.ying_chun_notice
    local list = notice_info.info.list
    self.Bg.sizeDelta = Vector2.New(1261, #list > 5 and 406 or 307)
    self.List:SetData(list)
    self.ListEffect:SetData(list)
    for k,v in pairs(list) do
         ChatCtrl.Instance:ChannelChatCustom{item_get = {item = Item.Create(v.item), num = v.item.num}}
    end
    PublicPopupCtrl.Instance:ReleaseShieldParam(true) -- 放开拦截的激活飘字
    AchieveCtrl.Instance:CloseDelayCourseRemind() -- 显示延迟的成就展示
end

-- Item
YingChunRewardItem = YingChunRewardItem or DeclareMono("YingChunRewardItem", UIWidgetBaseItem)

function YingChunRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_data=Item.Create(data.item)
    UH.SetText(self.TextName, item_data:QuaName())
    self.Cell:SetData(item_data)
end

-- Item2 特效用
YingChunRewardItem2 = YingChunRewardItem2 or DeclareMono("YingChunRewardItem2", UIWidgetBaseItem)

function YingChunRewardItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjXiYou:SetActive(data.is_award==1)
end
