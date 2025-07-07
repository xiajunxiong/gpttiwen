MedalView = MedalView or DeclareView("MedalView", "bag/medal", Mod.Medal.Main)

function MedalView:MedalView()
    self.lang = Language.Bag
    self.data = BagData.Instance
end
function MedalView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Medal, Vector2.New(378, 536))
end
function MedalView:OnFlush()
    -- 获取背包中的兑换物品
    self.item = self.data:MedalChangeItem()
    if self.item == nil then
        self.Item1:SetDataByItemId(0)
        self.Item1:NoneMask()
        self.Item2:SetDataByItemId(0)
        self.Item2:NoneMask()
        UH.SetText(self.Name1, Language.Bag.MT1)
        UH.SetText(self.Name2, Language.Bag.MT2)
    else
        self.info = self.data:GetIsMedalChange(self.item.item_id)
        self.Item2:SetData(self.item)
        local temp = Item.Create({item_id = self.info.target_item_id})
        self.Item1:SetData(temp)
        UH.SetText(self.Name1, temp:Name())
        UH.SetText(self.Name2, self.item:Name())
    end
end
function MedalView:OnClickCancel()
    self.item = nil
    ViewMgr:CloseView(MedalView)
    self.data:SetMedalChange(nil)
end
function MedalView:OnClickUse()
    if self.item == nil then
        PublicPopupCtrl.Instance:Center(Language.Bag.ErrorTip)
        return
    end
    BagCtrl.Instance:SendMedalReq({req_type = MEDAL_REQ_TYPE.EXCHANGE, param1 = self.item.item_id})
    self.data:SetMedalChange(nil)
    ViewMgr:CloseView(MedalView)
-- ViewMgr:CloseView(ItemInfoView)
end
