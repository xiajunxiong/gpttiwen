VIEW_REQ("modules/serveractivity/sheng_dan_mi_ling/sheng_dan_mi_ling_view")

ShengDanMiLingCtrl = ShengDanMiLingCtrl or BaseClass(BaseCtrl)

ShengDanMiLingCtrl.ReqType = {
    Info = 0,
    BaseReward = 1,     -- 登录奖励 p1 seq
    MiLingReward = 2,   -- 密令奖励 p1 seq
}

function ShengDanMiLingCtrl:__init()
	if ShengDanMiLingCtrl.Instance ~= nil then
		Debuger.LogError("[ShengDanMiLingCtrl] attempt to create singleton twice!")
		return
	end
	ShengDanMiLingCtrl.Instance = self
	self.data = ShengDanMiLingData.New()
    self:RegisterAllProtocols()

    Remind.Instance:Register(Mod.Act_Halloween.ShanDanMiLing,self.data.act_data, function () 
        return self.data:GetRemind() 
    end)
end

function ShengDanMiLingCtrl:__delete()
    ShengDanMiLingCtrl.Instance = nil
end

function ShengDanMiLingCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end


function ShengDanMiLingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAShengDanMiLingInfo, "OnSCRAShengDanMiLingInfo")
end

function ShengDanMiLingCtrl:OnSCRAShengDanMiLingInfo(protocol)
    self.data.act_data:Set(protocol.info)
end


ShengDanMiLingData = ShengDanMiLingData or BaseClass()

function ShengDanMiLingData:__init()
    if ShengDanMiLingData.Instance ~= nil then
        Debuger.LogError("[ShengDanMiLingData] attempt to create singleton twice!")
        return
    end
    ShengDanMiLingData.Instance = self
    self.act_data = SmartData.New()
    self.act_type = ACTIVITY_TYPE.RAND_SHENG_DAN_MI_LING
end

function ShengDanMiLingData:ClearData()
    self.act_data:Set({})
end

function ShengDanMiLingData:GetCfg()
    return ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,Config.shengdanmiling_auto.reward,"seq")
end

-- 基础奖励flag 1可领取，0未完成，-1已领取
function ShengDanMiLingData:GetBaseRewardFlag(seq)
    if TableIsEmpty(self.act_data) then
        return 0
    end
    if self.act_data.receive_flag[seq] == 1 then
        return -1
    end
    local day = TimeCtrl.Instance:GetActOpenDay(self.act_type)
    if day >= seq + 1 then
        return 1
    end
    return 0
end

-- 密令奖励flag 1可领取，0未完成，-1已领取, - 2 已经失效
function ShengDanMiLingData:GetMiLingRewardFlag(seq)
    if TableIsEmpty(self.act_data) then
        return 0
    end
    if self.act_data.miling_receive_flag[seq] == 1 then
        return -1
    end
    if self.act_data.finish_flag[seq] == 1 then
        return 1
    end
    local day = TimeCtrl.Instance:GetActOpenDay(self.act_type)
    if day > seq + 1 then
        return -2
    end
    return 0
end

function ShengDanMiLingData:GetRemind()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
    local cfg = self:GetCfg()
    for i,v in pairs(cfg) do
        if self:GetBaseRewardFlag(v.seq) == 1 then
            return 1
        end
        if self:GetMiLingRewardFlag(v.seq) == 1 then
            return 1
        end
    end 
    return 0
end