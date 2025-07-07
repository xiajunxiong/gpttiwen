FILE_REQ("modules/active/active_data")
FILE_REQ("modules/active/active_config")
VIEW_REQ("modules/active/active_view")
VIEW_REQ("modules/active/active_daily_view")
VIEW_REQ("modules/active/active_time_limit_view")
VIEW_REQ("modules/active/active_week_reward_view")
VIEW_REQ("modules/active/active_roller_view")
VIEW_REQ("modules/active/active_daily_all_finish")
VIEW_REQ("modules/active/active_daily_all_finish_reward")

ActiveCtrl = ActiveCtrl or BaseClass(BaseCtrl)

function ActiveCtrl:__init()
	if ActiveCtrl.Instance ~= nil then
		Debuger.LogError("[ActiveCtrl] attempt to create singleton twice!")
		return
	end
	ActiveCtrl.Instance = self
	self.data = ActiveData.New()
	self:RegisterAllProtocols()
end

function ActiveCtrl:__delete()
	self:UnRegisterAllProtocols()
	ActiveCtrl.Instance = nil
end
function ActiveCtrl:OnUnloadGameLogic()
	self.data:Clear()
end
function ActiveCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSActiveSignUpReq)
	self:RegisterProtocol(CSActiveSystemExtraRewarrdFetch)
	self:RegisterProtocol(SCActiveSignUpInfo, "OnActiveSignUpInfo")
	self:RegisterProtocol(SCWeekActiveDegreeInfo, "OnWeekActiveDegreeInfo")
	self:RegisterProtocol(CSActiveTurntableReq)
	self:RegisterProtocol(SCActiveTurntableInfo, "OnActiveTurntableInfo")
	self:RegisterProtocol(SCActiveTurntableRewardInfo, "OnActiveTurntableRewardInfo")
	self:RegisterProtocol(SCActiveDailyAllFinishInfo, "OnSCActiveDailyAllFinishInfo")
	ActivityData.Instance:CustomClickHandle(ActType.Strength,function()
		ViewMgr:OpenView(FightFBView)
	end)
	local time_limit_func = function()
		return self.data:GetTimeLimitNum()-- + AnecdoteHintData.Instance:OnRemind()
	end
	local anecdote_hint_rp_func = function()
		return AnecdoteHintData.Instance:OnRemind()
	end
	RemindCtrl.Instance:Remind():Register(Mod.Activity.TimeLimit,self.data.time_limit_data,time_limit_func)
	RemindCtrl.Instance:Remind():Register(Mod.Activity.TimeLimit,self.data.click_goto_limit_act,time_limit_func)
	RemindCtrl.Instance:Remind():Register(Mod.Activity.TimeLimit,ActivityData.Instance:GetActivityEvent(),time_limit_func)

	RemindCtrl.Instance:Remind():Register(Mod.Activity.Daily,AnecdoteHintData.Instance:NetData(),anecdote_hint_rp_func)
	RemindCtrl.Instance:Remind():Register(Mod.Activity.WeekReward,self.data.weed_reward_data,function()
		return self.data:GetWeekRewardNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Activity.ActiveRoller, self.data.active_roller_data, function()
		return self.data:GetRollerRemind()
	end)
end

function ActiveCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSActiveSignUpReq)
	self:UnRegisterProtocol(CSActiveSystemExtraRewarrdFetch)
	self:UnRegisterProtocol(SCActiveSignUpInfo)
	self:UnRegisterProtocol(SCWeekActiveDegreeInfo)
	self:UnRegisterProtocol(CSActiveTurntableReq)
	self:UnRegisterProtocol(SCActiveTurntableInfo)
	self:UnRegisterProtocol(SCActiveTurntableRewardInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Activity.TimeLimit)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Activity.WeekReward)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Activity.ActiveRoller)
end

function ActiveCtrl:SendSignUpReq(req_type,act_type)
	local protocol = GetProtocol(CSActiveSignUpReq)
	protocol.req_type = req_type or 0
	protocol.act_type = act_type or 0
	SendProtocol(protocol)
end

function ActiveCtrl:SendExtraRewarrdFetch(type)
	local protocol = GetProtocol(CSActiveSystemExtraRewarrdFetch)
	protocol.type = type or 0
	SendProtocol(protocol)
end

function ActiveCtrl:OnActiveSignUpInfo(protocol)
	self.data:SetActiveSignUpInfo(protocol)
end

function ActiveCtrl:OnWeekActiveDegreeInfo(protocol)
	self.data:SetWeekActiveDegreeInfo(protocol)
end

function ActiveCtrl:SendActiveRollerReq(req_type)
	local protocol = GetProtocol(CSActiveTurntableReq)
	-- 0请求数据信息，1转盘抽奖
	protocol.req_type = req_type or 0
	SendProtocol(protocol)
end

function ActiveCtrl:OnActiveTurntableInfo(protocol)
	self.data:SetActiveRollerAllInfo(protocol)
end

function ActiveCtrl:OnActiveTurntableRewardInfo(protocol)
	self.data:SetActiveRollerRewardInfo(protocol)
end

function ActiveCtrl:OnSCActiveDailyAllFinishInfo(protocol)
	local param = {}
	param.task_num = 0
	for k, v in pairs(protocol.act_types) do
		if v > 0 then
			param.task_num = param.task_num + 1
		end
	end
	param.item_list = protocol.item_list
	param.exp = protocol.exp
	ViewMgr:OpenView(ActiveDailyAllFinishRewardView, param)
end