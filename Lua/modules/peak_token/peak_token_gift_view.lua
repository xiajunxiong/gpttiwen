PeakTokenGiftView = PeakTokenGiftView or DeclareView("PeakTokenGiftView", "peak_token/peak_token_gift",ViewLayer.Top)

VIEW_DECLARE_MASK(PeakTokenGiftView,ViewMask.BlockClose)

PeakTokenGiftPanel = PeakTokenGiftPanel or DeclareMono("PeakTokenGiftPanel", UIWFlushPanel)

function PeakTokenGiftPanel:PeakTokenGiftPanel()
    self.data = PeakTokenData.Instance
    self.data_cares = {
        {data = self.data.game_data, func = self.OnFlush,keys = {"num"},init_call = false},
    }
end

function PeakTokenGiftPanel:Awake()
    UIWFlushPanel.Awake(self)
    local data = self.data:GetRoleInfoData()
    self.SendPeaktokenFunc = function()
        if not self.IsOpenGiftPanel then
            PeakTokenCtrl.Instance:SendPeaktokenSeq(2,data.obj_id)
        end
    end
    UH.SetText(self.Desc,Format(Language.PeakToken.GiftTips,data.name or ""))
end

function PeakTokenGiftPanel:OnFlush()
    local data = self.data:GetItemData()
    if data and data.item_id then
        local item = Item.Create(data)
        self.Call:SetData(item)
        UH.SetText(self.Name,item:QuaName())
    end
    self.OpenAnim:Play()
    self.IsOpenGiftPanel = true
end

function PeakTokenGiftPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.SendPeaktokenFunc()
end

function PeakTokenGiftPanel:OnClickOpen()
    self.SendPeaktokenFunc()
end