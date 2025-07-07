--祈愿孔明灯 祈愿天灯
FILE_REQ("modules/serveractivity/sky_lantern/sky_lantern_data")
VIEW_REQ("modules/serveractivity/sky_lantern/sky_lantern_view")

SkyLanternCtrl = SkyLanternCtrl or BaseClass(BaseCtrl)
function SkyLanternCtrl:__init()
    if SkyLanternCtrl.Instance ~= nil then
        Debuger.LogError("[SkyLanternCtrl] attempt to create singleton twice!")
        return
    end
    SkyLanternCtrl.Instance = self
    self.data = SkyLanternData.New()
    self:RegisterAllProtocols()
    self.last_item_num = nil
end

function SkyLanternCtrl:__delete()
    SkyLanternCtrl.Instance = nil
    Delete(self, "data")
end

function SkyLanternCtrl:OnInit()
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.SkyLantern, self.data.remind_info, function()
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SKY_LANTERN) and self.data:GetRemindNum() or 0
    end)
    EventSys:Bind(GlobalEventKeys.TimeMinuteChange, BindTool.Bind(self.OnMinuteChange, self))
	EventSys:Bind(GlobalEventKeys.ServerOpenDayChange, BindTool.Bind1(self.OnDayChange, self))
end

function SkyLanternCtrl:OnUnloadGameLogic()
    self.last_item_num = nil
end

function SkyLanternCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAQiYuanKongMingDengInfo, "OnSCRAQiYuanKongMingDengInfo")
end

function SkyLanternCtrl:OnMinuteChange()
    self.data:CheckRemind()
end
function SkyLanternCtrl:OnDayChange()
    if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SKY_LANTERN) then
        ActivityData.Instance:ResetHasSign(ACTIVITY_TYPE.RAND_SKY_LANTERN)
        self.data:CheckRemind()
    end
end

-- 0|发送信息 1|点灯 10|gm加火种
function SkyLanternCtrl:SendInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SKY_LANTERN, 0)
end
function SkyLanternCtrl:SendPlay()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SKY_LANTERN, 1)
end
function SkyLanternCtrl:GMAddHuoZhong(num)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SKY_LANTERN, 10, num)
end

function SkyLanternCtrl:OnSCRAQiYuanKongMingDengInfo(protocol)
    self.data:SetActInfo(protocol)
    if self.last_item_num ~= nil and self.last_item_num < protocol.huozhong then
        --todo 飘字预留 虽然现在说是不需要飘字
    end
    self.last_item_num = protocol.huozhong
end

function SkyLanternCtrl:ShowGetItems()
    local show_list = self.data:LoadNeedDelayItems()
    if show_list ~= nil and #show_list > 0 then
        local show_list = DataHelper.FormatItemList(self.data:GetRewardList().reward_item)
        for k, v in ipairs(show_list) do
            if Item.GetUseType(v.item_id) == ItemUseType.CurrencyItem then
                local co = Item.GetConfig(v.item_id)
                PublicPopupCtrl.Instance:CenterV {type = co.param1, num = co.param2 * v.num}
            else
                PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num, color_str = v:ColorStr()})
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = v, num = v.num}}
            end
        end
        ViewMgr:OpenView(GiftOpenView, {gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS, item_list = show_list, is_not_show = true})
        self.data:ClearNeedDelayItems()
    end
end
