FILE_REQ("modules/serveractivity/yun_ze/yun_ze_data")

VIEW_REQ("modules/serveractivity/yun_ze/yun_ze_view")
VIEW_REQ("modules/serveractivity/yun_ze/yun_ze_territory_view")
VIEW_REQ("modules/serveractivity/yun_ze/super_value_enjoy_view")
VIEW_REQ("modules/serveractivity/yun_ze/act_same_flower_view")
VIEW_REQ("modules/serveractivity/yun_ze/yun_ze_nice_gift")
VIEW_REQ("modules/serveractivity/yun_ze/elves_catch_view")
VIEW_REQ("modules/serveractivity/yun_ze/yz_wish_view")

YunZeCtrl = YunZeCtrl or BaseClass(BaseCtrl)
function YunZeCtrl:__init()
    if YunZeCtrl.Instance ~= nil then
        Debuger.LogError("[YunZeCtrl] attempt to create singleton twice!")
    return
    end
    YunZeCtrl.Instance = self
    self.data = YunZeData.New()
    -- 活动开启注册
    ActivityRandData.Instance:Register(Mod.YunZe.Main,function()
        return self.data:IsOpen()
    end)
    
    ActivityRandData.Instance:RegisterActCountDown(Mod.YunZe.Main, function (TimeMeter)
        if TimeMeter ~= nil then
            local act_type = self.data:GetBoosActCfg().act_type
            local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
            TimeMeter:StampTime(end_time or 0,TimeType.Type_Special_9, "")
        end
    end)

    Remind.Instance:Register(Mod.YunZe.SuperValueEnjoy, self.data.super_value_enjoy_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY) then
            return 0
        end
        return self.data:GetSuperValueEnjoyRemind()
    end)
    
    -- 分享同花礼红点
    Remind.Instance:Register(Mod.YunZe.SameFlower,self.data.same_flower_data, function () 
        return self.data:GetSameFlowerRemind() 
    end)
    
    -- 云泽二三四红点
    Remind.Instance:Register(Mod.YunZe.YunZeErSanShi,YunZeErSanShiData.Instance.info_sm.info, function ()
        return self.data:GetErSanSiRemind() 
    end)
    -- 魔族在哪里红点
    Remind.Instance:Register(Mod.YunZe.MoZuZaiNaLi, MoZuZaiNaLiData.Instance.info_sm, function ()
        return self.data:GetMoZuRemind() 
    end)

    -- 云泽画境
    Remind.Instance:Register(Mod.YunZe.Territory, self.data.say_territory_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY) then
            return 0
        end
        return self.data:GetSayTerritoryRemind()
    end)
    -- 精灵大抓捕
    Remind.Instance:Register(Mod.YunZe.ElvesCatch, BagData.Instance:GetColumnGrids(ItemColumnType.Item), function ()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ELVES_CATCH) then
            return 0
        end
        return self.data:GetElvesCatchRemind() 
    end)
    -- 云泽祈愿
    Remind.Instance:Register(Mod.YunZe.YzWish, self.data.yz_wish_data, function ()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YZ_WISH) then
            return 0
        end
        return self.data:GetYZWishRemind() 
    end)
    -- 云泽好礼
    Remind.Instance:Register(Mod.YunZe.YunZeNiceGift, self.data.nice_gift_data, function ()
        return self.data:GetNiceGiftRemind() 
    end)
    BagData.Instance:ListenItems({self.data:GetNiceGiftGoldItemId()},function ()
        self.data.nice_gift_data:Notify()
    end)

    self:RegisterAllProtocols()
end

function YunZeCtrl:__delete()
    YunZeCtrl.Instance = nil
    Delete(self, "data")
end

function YunZeCtrl:RegisterAllProtocols()
    -- 云泽画境
    self:RegisterProtocol(SCRAYunZeTerritoryInfo, "OnSCRAYunZeTerritoryInfo")
    -- 超值纷享
    self:RegisterProtocol(SCRAChaoZhiFenXiangInfo, "OnSCRAChaoZhiFenXiangInfo")
    -- 分享同花礼
    self:RegisterProtocol(SCSameFlowerInfo, "OnSCSameFlowerInfo")
    self:RegisterProtocol(SCSameFlowerResult, "OnSCSameFlowerResult")
    -- 云泽好礼
    self:RegisterProtocol(SCYunZeNiceGiftInfo, "OnSCYunZeNiceGiftInfo")
    -- 精灵大抓捕
    self:RegisterProtocol(SCRAYunZeCatchElvesGetInfo, "OnSCRAYunZeCatchElvesGetInfo")
    -- 云泽祈愿
    self:RegisterProtocol(SCRAYunZeWishInfo, "OnSCRAYunZeWishInfo")
end

function YunZeCtrl:OnUnloadGameLogic()
    self.data.say_territory_data.is_remind = true --重置
    MoZuZaiNaLiData.Instance.login_flag = false
end

--云泽画境
function YunZeCtrl:OnSCRAYunZeTerritoryInfo(protocol)
    self.data:SetSayTerritoryInfo(protocol)
end

function YunZeCtrl:OnSCRAChaoZhiFenXiangInfo(protocol)
    self.data:SetSuperValueEnjoyInfo(protocol)
end

--分享同花礼
function YunZeCtrl:OnSCSameFlowerInfo(protocol)
    self.data.same_flower_data.info = protocol.info
end

function YunZeCtrl:OnSCSameFlowerResult(protocol)
    self.data.same_flower_data.result = protocol.result
end

--云泽好礼
function YunZeCtrl:OnSCYunZeNiceGiftInfo(protocol)
    self.data.nice_gift_data.info = protocol.info
end

-- 精灵大抓捕
function YunZeCtrl:OnSCRAYunZeCatchElvesGetInfo(protocol)
    self.data:SetElvesCatchInfo(protocol)
end

-- 云泽祈愿
function YunZeCtrl:OnSCRAYunZeWishInfo(protocol)
    self.data:SetYZWishInfo(protocol)
end