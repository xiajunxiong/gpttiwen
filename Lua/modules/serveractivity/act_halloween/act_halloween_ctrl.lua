FILE_REQ("modules/serveractivity/act_halloween/act_halloween_data")
VIEW_REQ("modules/serveractivity/act_halloween/act_halloween_view")
VIEW_REQ("modules/serveractivity/act_halloween/halloween_lei_chong")
VIEW_REQ("modules/serveractivity/act_halloween/halloween_shop_view")
VIEW_REQ("modules/serveractivity/rabbit_year/rabbit_year_charge/rabbit_year_charge_view")

WAN_SHENG_LEI_CHONG_REQ_TYPE={
    INFO=0,--信息下发
    FETCH=1--领取奖励
}

WAN_SHENG_SHOP_REQ_TYPE={
    INFO=0,--信息下发
    BUY_ZHIGOU=1,--购买 p1 奖励reward_seq
    BUY=2,--非直购购买 p1 配置seq
}

WAN_SHENG_SHOP_MAX_BUY_ITEM=20  --万圣商店物品上限

ActHalloweenCtrl = ActHalloweenCtrl or BaseClass(BaseCtrl)

function ActHalloweenCtrl:__init()
    if ActHalloweenCtrl.Instance ~= nil then
        Debuger.LogError("[ActHalloweenCtrl] attempt to create singleton twice!")
        return
    end
    ActHalloweenCtrl.Instance = self
    self.data = ActHalloweenData.New()
    self:RegisterAllProtocols()
    self:RegisterRemind()
end

function ActHalloweenCtrl:RegisterRemind()
    RemindCtrl.Instance:Remind():Register(Mod.Act_Halloween.LeiChong, self.data.lei_chong_data, BindTool.Bind(self.RemindLeiChong, self))
    RemindCtrl.Instance:Remind():Register(Mod.RabbitYear.HongYunLeiChong, self.data.lei_chong_data, BindTool.Bind(self.RemindLeiChong, self))
    RemindCtrl.Instance:Remind():Register(Mod.Act_Halloween.Shop, self.data.shop_data, BindTool.Bind(self.RemindShop, self))
end

function ActHalloweenCtrl:OnInit()
    EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SetRecommendSuit, self))
end

function ActHalloweenCtrl:SetRecommendSuit()
    BagData.Instance:ListenItems({[1] = CommonItemId.HalloweenCoinId}, BindTool.Bind(self.FlushCurr, self))
end

function ActHalloweenCtrl:FlushCurr()
    self.data:onFlushCurr()
end

function ActHalloweenCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActHalloweenCtrl.Instance = nil
end

function ActHalloweenCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAWanShengLeiChongInfo, "RecvLeiChongInfo")
    self:RegisterProtocol(SCRAWanShengShopInfo, "RecvShopInfo")

end

function ActHalloweenCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRAWanShengLeiChongInfo)
    self:UnRegisterProtocol(SCRAWanShengShopInfo)
end

function ActHalloweenCtrl:RecvShopInfo(protocol)
    self.data:SetShopData(protocol)
end

function ActHalloweenCtrl:RecvLeiChongInfo(protocol)
    self.data:SetLeiChongData(protocol)
end

function ActHalloweenCtrl:RemindLeiChong()
    local num = self.data:RemindLeiChong()
    return num
end

function ActHalloweenCtrl:RemindShop()
    local num = self.data:RemindShop()
    return num
end




