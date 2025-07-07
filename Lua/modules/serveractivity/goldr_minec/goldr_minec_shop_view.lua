GoldrMinecShopView = GoldrMinecShopView or DeclareView("GoldrMinecShopView", "serveractivity/goldr_minec/goldr_minec_shop")
-- VIEW_DECLARE_LEVEL(GoldrMinecShopView, ViewLevel.L0)

function GoldrMinecShopView:GoldrMinecShopView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function GoldrMinecShopView:LoadCallback(param_t)
    -- GoldRMinecCtrl.Instance:SendInfo()
end

function GoldrMinecShopView:OnClickClose()
    ViewMgr:CloseView(GoldrMinecShopView)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end


--===========================GoldrMinecShopPanel===========================
GoldrMinecShopPanel = GoldrMinecShopPanel or DeclareMono("GoldrMinecShopPanel", UIWFlushPanel)
function GoldrMinecShopPanel:GoldrMinecShopPanel()
    self.data = GoldRMinecData.Instance

    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
        {data = self.data.act_info, func = self.MainFlush, keys = {"flush"}},
    }
    
    self.send_info = {}

end 

function GoldrMinecShopPanel:MainFlush()
    -- LogError("!MainFlush?@~~~",self.data:GetShopCfg())
    self.list:SetData(self.data:GetShopList())    
end

function GoldrMinecShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self) 
end

function GoldrMinecShopPanel:OnClickBuy(data)
    -- LogError("?",data)
    self.send_info.price = data.price

    if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_JIN_LONG_JU_BAO, data.price/10) then
        ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_JIN_LONG_JU_BAO, data.price/10 ,data.seq)
    else
        GoldRMinecCtrl.Instance:SendBuy(data.seq)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldrMinecShopPanel:SendBuy()
    -- LogDG("平哥哥救救我?直购成功了哦！")
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_JIN_LONG_JU_BAO, self.send_info.price)
end

GoldrMinecShopCell = GoldrMinecShopCell or DeclareMono("GoldrMinecShopCell", UIWidgetBaseItem)
function GoldrMinecShopCell:GoldrMinecShopCell()
    self.gm_data = GoldRMinecData.Instance
end
function GoldrMinecShopCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.call:SetData(Item.Create(data.reward_item[0]))
    local limits = data.limit_type == 1 and data.limit_times or data.day_times
    local flag = limits - self.gm_data:GetShopLimit(data.limit_type,data.seq) > 0 
    UH.SetText(self.limit, string.format(Language.GoldRMinec.LimitShow[data.limit_type],
        limits - self.gm_data:GetShopLimit(data.limit_type,data.seq)))
    UH.SetText(self.price_o,data.value / 10)
    UH.SetText(self.price_n,data.price / 10)

    self.btn_buy:SetActive(flag)
    self.btn_done:SetActive(not flag)
end 