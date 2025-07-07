
FILE_REQ("modules/vitality_flop/vitality_flop_data")
VIEW_REQ("modules/vitality_flop/vitality_flop")
VIEW_REQ("modules/vitality_flop/vitality_flop_exchange")
VitalityFlopCtrl = VitalityFlopCtrl or BaseClass(BaseCtrl)
function VitalityFlopCtrl:__init()
    if VitalityFlopCtrl.Instance ~= nil then
        Debuger.LogError("[VitalityFlopCtrl] attempt to create singleton twice!")
        return
    end
    VitalityFlopCtrl.Instance = self
    self.data = VitalityFlopData.New()

    self:RegisterProtocol(SCRAYuanQiFanPaiInfo,"SCRAYuanQiFanPaiInfo")
    local remind_func = BindTool.Bind(self.GetRemingCount, self)
    Remind.Instance:Register(Mod.MayDay.VitalityFlop, self.data:Info(), remind_func)
    Remind.Instance:Register(Mod.MayDay.VitalityFlop, BagData.Instance:GetColumnGrids(ItemColumnType.Material), remind_func)
end
function VitalityFlopCtrl:__delete()
    
end
function VitalityFlopCtrl:GetRemingCount()
    return self.data:GetRemindCount()
end
function VitalityFlopCtrl:SCRAYuanQiFanPaiInfo(protocol)
    self.data:Info().count = protocol.count
    self.data:Info().times = protocol.is_receive_times

    MayDayData.Instance:SetIsFetchFayReward(protocol.is_fetch_day_reward)
end

function VitalityFlopCtrl.ReqInfo()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.VITALITY_FLOP
	param_t.opera_type = 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end
function VitalityFlopCtrl.Lottery()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.VITALITY_FLOP
	param_t.opera_type = 1
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function VitalityFlopCtrl.ReceiveTimes(seq)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.VITALITY_FLOP
	param_t.opera_type = 2
    param_t.param_1 = seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function VitalityFlopCtrl.Exchange(seq)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.VITALITY_FLOP
	param_t.opera_type = 3
    param_t.param_1 = seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end