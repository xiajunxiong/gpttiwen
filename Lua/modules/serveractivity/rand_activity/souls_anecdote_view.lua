SoulsAnecdoteView = SoulsAnecdoteView or DeclareView("SoulsAnecdoteView","serveractivity/souls_anecdote_view",Mod.RandActivity.SoulsAnecdote)

VIEW_DECLARE_LEVEL(SoulsAnecdoteView, ViewLevel.L0)

function SoulsAnecdoteView:SoulsAnecdoteView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.SoulsAnecdote.TimeTip,t.day,t.min,t.s)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SOULS_ANECDOTE)
    self.Currency:SetCurrency(CurrencyType.Anecdote)
end

function SoulsAnecdoteView:LoadCallback()
    local config = ServerActivityData.Instance:GetActivityOpenCfg(nil,nil,Config.secret_area_auto.gift_show)
    self.Grid:SetData(DataHelper.FormatItemList(config[1].item_show))
    local item_data = ServerActivityData.Instance:GetActivityOpenCfg(nil,nil,Config.secret_area_auto.event_show)
    for i=1,self.ItemList:Length() do
        if item_data[i] then
            self.ItemList[i]:SetData(item_data[i])
        end
    end
end

function SoulsAnecdoteView:OnClickClose()
    ViewMgr:CloseView(SoulsAnecdoteView)
end

function SoulsAnecdoteView:OnClickEnter()
    local item_id = Config.secret_area_auto.anecdote_configure[1].consume_item
    PublicPopupCtrl.Instance:AlertTip(Format(Language.SoulsAnecdote.EnterTip,Item.GetQuaName(item_id)),function()
        if Item.GetNum(item_id) == 0 then
            MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = item_id})})
            return
        end
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOULS_ANECDOTE)
        ActivityRandData.Instance:CheckRandChildData()
        ViewMgr:CloseView(SoulsAnecdoteView)
    end)
end

----------------------------SoulsAnecdoteItem----------------------------
SoulsAnecdoteItem = SoulsAnecdoteItem or DeclareMono("SoulsAnecdoteItem", UIWidgetBaseItem)

function SoulsAnecdoteItem:SetData(data)
    UH.SpriteName(self.Icon,data.npcid)
    UIWidgetBaseItem.SetData(self, data)
end

function SoulsAnecdoteItem:OnClickItem()
    PublicPopupCtrl.Instance:HelpTip(self.data.event_desc)
end