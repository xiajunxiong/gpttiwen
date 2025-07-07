PCWebView = PCWebView or DeclareView("PCWebView", "pc_web_view/PCWebView")
VIEW_DECLARE_LEVEL(PCWebView,ViewLevel.Lt)
VIEW_DECLARE_MASK(PCWebView, ViewMask.BgBlock)
function PCWebView:PCWebView()
end

function PCWebView:LoadCallback(param_t)
    self.param_t = param_t
    if not TableIsEmpty(param_t) then
        self.Board:SetData(PCWebView, param_t.title_str or Language.Recharge.Recharge, Vector2.New(1228, 598))
        self.RawImgObj:SetActive(not StringIsEmpty(param_t.qrcode))
        self.WebPlaneObj:SetActive(not StringIsEmpty(param_t.url_str))
        if param_t.url_str then
            self.WebAgent:SetAndOpenUrl(param_t.url_str)
        elseif param_t.qrcode then
            Nirvana.QRCode.SetQRTexture(self.RawImg, param_t.qrcode)
            UH.SetText(self.Amount, Format(Language.PCNotice.ChongZhiJinE, param_t.amount))
            UH.SetText(self.TipsText, Language.PCNotice.PCRechargeAliPayAndWechatPay)
        end
    end
end

function PCWebView:CloseCallback()
    -- self.WebAgent:Hide()
    if not TableIsEmpty(self.param_t) then
        if self.param_t.url_str then
            self.WebAgent:Stop()
        elseif self.param_t.qrcode then
            self.RawImg.texture = nil
        end
    end
end