FILE_REQ("modules/serveractivity/all_soul_cast/all_soul_cast_data")
VIEW_REQ("modules/serveractivity/all_soul_cast/all_soul_cast_view")

AllSoulCastCtrl = AllSoulCastCtrl or BaseClass(BaseCtrl)
function AllSoulCastCtrl:__init()
    if AllSoulCastCtrl.Instance ~= nil then
        Debuger.LogError("[AllSoulCastCtrl] attempt to create singleton twice!")
        return
    end
    AllSoulCastCtrl.Instance = self
    self.data = AllSoulCastData.New()
    self:RegisterAllProtocols()
end

function AllSoulCastCtrl:__delete()
    AllSoulCastCtrl.Instance = nil
    Delete(self, "data")
end

function AllSoulCastCtrl:OnInit()
    Remind.Instance:Register(Mod.MayDay.AllSoulCast, self.data.act_info, function()
        local remind_num = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ALL_SOUL_COST) and self.data:GetRemindNum() or 0
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ALL_SOUL_COST) and self.data:GetRemindNum() or 0
    end)
end

function AllSoulCastCtrl:OnUnloadGameLogic()
end

function AllSoulCastCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAWanLingZhuLiInfo, "OnSCRAWanLingZhuLiInfo")
end

function AllSoulCastCtrl:OnSCRAWanLingZhuLiInfo(protocol)
    self.data:SetActInfo(protocol)
end

--0,信息
function AllSoulCastCtrl:SendInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ALL_SOUL_COST, 0)
end

--1领取每日奖励
function AllSoulCastCtrl:SendFreeFetch()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ALL_SOUL_COST, 1)
end

--2领取天数奖励 p2:是否终极奖励(1是0否)
function AllSoulCastCtrl:SendFetch(seq, param2)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ALL_SOUL_COST, 2, seq, param2 or 0)
end

--3加购校验
function AllSoulCastCtrl:SendBuy()
    local fee = RechargeData.Instance:GoldToMoney(self.data:GetBuyPrice())
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ALL_SOUL_CAST, fee, ACTIVITY_TYPE.RAND_ALL_SOUL_COST, 3)
end
