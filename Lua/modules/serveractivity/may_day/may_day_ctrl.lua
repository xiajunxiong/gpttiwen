
FILE_REQ("modules/serveractivity/may_day/may_day_config")
FILE_REQ("modules/serveractivity/may_day/may_day_data")

VIEW_REQ("modules/serveractivity/may_day/may_day_view")
VIEW_REQ("modules/serveractivity/may_day/bun_come_view")
VIEW_REQ("modules/serveractivity/may_day/vitality_note_view")

MayDayCtrl = MayDayCtrl or BaseClass(BaseCtrl)

function MayDayCtrl:__init()
	if MayDayCtrl.Instance ~= nil then
		Debuger.LogError("[MayDayCtrl] attempt to create singleton twice!")
		return
	end	
	MayDayCtrl.Instance = self
	self.data = MayDayData.New()
	self:RegisterAllProtocols()
end

function MayDayCtrl:__delete()
	if MayDayCtrl.Instance == self then
		MayDayCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function MayDayCtrl:OnInit()
	ActivityRandData.Instance:RegisterActCountDown(Mod.MayDay.Main, function (obj)
        local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.VITALITY_FLOP)
		if obj then
			obj:StampTime(end_time,TimeType.Type_Special_9, "")
		end
    end)

	Remind.Instance:Register(Mod.MayDay.Main, self.data.main_info, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.VITALITY_FLOP) then
			return 0
		end
		return (0 == self.data.main_info.is_fetch_day_reward) and 1 or 0
	end)

	PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.BunCome, self.data.bun_come_info, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BUN_COME) then
			return 0
		end
		if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.BunCome) then
            return 0
        end
		return self.data:BunComeRemind()
	end)

	PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.VitalityNote, self.data.vitality_note_info, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_VITALITY_NOTE) then
			return 0
		end
		if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.VitalityNote) then
            return 0
        end
		return 1
		-- return self.data:VitalityNoteRemind()
	end)
end

function MayDayCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCBaoZiLaiLeInfo, "OnBaoZiLaiLeInfo")
	self:RegisterProtocol(SCRAYuanQiJiShiBuInfo, "OnRAYuanQiJiShiBuInfo")
end

function MayDayCtrl:UnRegisterAllProtocols()
end

function MayDayCtrl:OnBaoZiLaiLeInfo(protocol)
	self.data:SetBaoZiLaiLeInfo(protocol)
end

function MayDayCtrl:OnRAYuanQiJiShiBuInfo(protocol)
	self.data:SetRAYuanQiJiShiBuInfo(protocol)
end


-- p1:难度seq p2:是否完成挑战(1胜利0失败)
function MayDayCtrl:SendRABunComeGet(is_win)
    local param_t = {}
	local game = self.data.bun_come_sm.game
	if game.init then return end
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_BUN_COME
    param_t.opera_type = 1
    param_t.param_1 = game.seq
    param_t.param_2 = is_win and 1 or 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function MayDayCtrl:SendMainDayRewardGet()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.VITALITY_FLOP
    param_t.opera_type = 4
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function MayDayCtrl:SendRAVitalityNoteGet1(seq)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_VITALITY_NOTE
    param_t.opera_type = 1
    param_t.param_1 = seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function MayDayCtrl:SendRAVitalityNoteGet2(seq)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_VITALITY_NOTE
    param_t.opera_type = 2
    param_t.param_1 = seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function MayDayCtrl:SendRAVitalityNoteBuy()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_VITALITY_NOTE
    param_t.opera_type = 3
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end


