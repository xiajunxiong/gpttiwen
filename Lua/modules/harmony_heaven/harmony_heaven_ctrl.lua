
FILE_REQ("modules/harmony_heaven/harmony_heaven_config")
FILE_REQ("modules/harmony_heaven/harmony_heaven_data")

VIEW_REQ("modules/harmony_heaven/harmony_heaven_view")
VIEW_REQ("modules/harmony_heaven/harmony_heaven_shop_view")
VIEW_REQ("modules/harmony_heaven/harmony_heaven_record_view")
VIEW_REQ("modules/harmony_heaven/harmony_heaven_reward_view")

VIEW_REQ("modules/harmony_heaven/harmony_heaven_menu")


HarmonyHeavenCtrl = HarmonyHeavenCtrl or BaseClass(BaseCtrl)

function HarmonyHeavenCtrl:__init()
	if HarmonyHeavenCtrl.Instance ~= nil then
		Debuger.LogError("[HarmonyHeavenCtrl] attempt to create singleton twice!")
		return
	end	
	HarmonyHeavenCtrl.Instance = self
	self.data = HarmonyHeavenData.New()
	self:RegisterAllProtocols()

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_HARMONY_HEAVEN, function(is_win)
		if is_win == 1 then
			if self.data.reward_list_all then
				ViewMgr:OpenView(HarmonyHeavenRewardView)
			else
				self.data:AutoToNpc()
			end
		end
    end)
	ActivityData.Instance:CustomClickHandle(ActType.HarmonyHeaven,function()
		ViewMgr:OpenView(HarmonyHeavenView)
	end)
end

function HarmonyHeavenCtrl:__delete()
	if HarmonyHeavenCtrl.Instance == self then
		HarmonyHeavenCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function HarmonyHeavenCtrl:OnInit()
	ActivityData.Instance:RegisterCondition(ActType.HarmonyHeaven,function()
        return self:TaskChainCondition()
	end)
end
function HarmonyHeavenCtrl:OnUnloadGameLogic()
	DialogTipsData.Instance:SetTodayValue(DTTodayNotRemind.HarmonyFight, false)
	DialogTipsData.Instance:SetTodayValue(DTTodayNotRemind.LeaderChangeSpeed, false)
end

function HarmonyHeavenCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSHongMengTianJieReq)

	self:RegisterProtocol(SCHongMengTianJieSceneInfo, "OnHongMengTianJieSceneInfo")
	self:RegisterProtocol(SCHongMengTianJieRoleInfo, "OnHongMengTianJieRoleInfo")
	self:RegisterProtocol(SCHongMengTianJieWorldInfo, "OnHongMengTianJieWorldInfo")
	self:RegisterProtocol(SCHongMengTianJieTalkInfo, "OnHongMengTianJieTalkInfo")
	self:RegisterProtocol(SCHongMengTianJieRewardInfo, "OnHongMengTianJieRewardInfo")
	self:RegisterProtocol(SCHongMengTianJiePassTeamInfo, "OnHongMengTianJiePassTeamInfo")
end

function HarmonyHeavenCtrl:UnRegisterAllProtocols()
end

function HarmonyHeavenCtrl:OnHongMengTianJieSceneInfo(protocol)
	self.data:SetHongMengTianJieSceneInfo(protocol)	
end

function HarmonyHeavenCtrl:OnHongMengTianJieRoleInfo(protocol)
	self.data:SetHongMengTianJieRoleInfo(protocol)	
end

function HarmonyHeavenCtrl:OnHongMengTianJieWorldInfo(protocol)
	self.data:SetHongMengTianJieWorldInfo(protocol)	
end

function HarmonyHeavenCtrl:OnHongMengTianJieTalkInfo(protocol)
	self.data:SetHongMengTianJieTalkInfo(protocol)	
end

function HarmonyHeavenCtrl:OnHongMengTianJieRewardInfo(protocol)
	self.data:SetHongMengTianJieRewardInfo(protocol)	
end

function HarmonyHeavenCtrl:OnHongMengTianJiePassTeamInfo(protocol)
	self.data:SetHongMengTianJiePassTeamInfo(protocol)	
end

function HarmonyHeavenCtrl:SendHongMengTianJieReq(param_t)
	local protocol = GetProtocol(CSHongMengTianJieReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param_1 = param_t.param_1 or 0
	protocol.param_2 = param_t.param_2 or 0
	SendProtocol(protocol)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqTalk(talk_id, talk_seq)
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.talk,
		param_1 = talk_id,
		param_2 = talk_seq,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqRoleInfo()
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.role_info,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqSendReward()
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.send_reward,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqShop(seq, num)
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.shop,
		param_1 = seq,
		param_2 = num,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqWorldPass()
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.world_pass,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:SendHongMengTianJieReqPassTeam(fb_id)
	local param_t = {
        req_type = HarmonyHeavenConfig.ReqType.pass_team,
		param_1 = fb_id,
    }
	self:SendHongMengTianJieReq(param_t)
end

function HarmonyHeavenCtrl:TaskChainCondition()
    local info = self.data.fb_info.role_info
	if info.init then return end
	return string.format(Language.HarmonyHeaven.Activity.NumShow, info.rewards_times_max or 0, self.data:ConfigOtherBossRewardLimit())
end
