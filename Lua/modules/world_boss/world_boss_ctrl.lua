
FILE_REQ("modules/world_boss/world_boss_config")
FILE_REQ("modules/world_boss/world_boss_data")
VIEW_REQ("modules/world_boss/world_boss_view")
VIEW_REQ("modules/world_boss/world_boss_reward_view")
VIEW_REQ("modules/world_boss/world_boss_map_view")
VIEW_REQ("modules/world_boss/world_boss_menu_view")
VIEW_REQ("modules/world_boss/world_boss_rank")

WorldBossCtrl = WorldBossCtrl or BaseClass(BaseCtrl)

function WorldBossCtrl:__init()
	if WorldBossCtrl.Instance ~= nil then
		Debuger.LogError("[WorldBossCtrl] attempt to create singleton twice!")
		return
	end	
	WorldBossCtrl.Instance = self
	self.data = WorldBossData.New()
	self:RegisterAllProtocols()
	ActivityData.Instance:RegisterRewardGetFunc(ActType.WorldBoss, BindTool.Bind(self.data.GetWorldBossPreReward, self.data, ActType.WorldBoss))
	ActivityData.Instance:RegisterRewardGetFunc(ActType.WorldBoss2,	BindTool.Bind(self.data.GetWorldBossPreReward, self.data, ActType.WorldBoss2))
	ActivityData.Instance:RegisterRewardGetFunc(ActType.WorldBoss3,	BindTool.Bind(self.data.GetWorldBossPreReward, self.data, ActType.WorldBoss3))
end

function WorldBossCtrl:__delete()
	if WorldBossCtrl.Instance == self then
		WorldBossCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function WorldBossCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSWorldBossReq)
	self:RegisterProtocol(CSWorldBoss2Req)
	self:RegisterProtocol(CSWorldBoss3Req)

	self:RegisterProtocol(SCWorldBossInfo, "OnWorldBossInfo")
	self:RegisterProtocol(SCWorldBoss2Info, "OnWorldBoss2Info")
	self:RegisterProtocol(SCWorldBoss3Info, "OnWorldBoss3Info")
	self:RegisterProtocol(SCWorldBossResult, "OnWorldBossResult")
	self:RegisterProtocol(SCWorldBoss2Result, "OnWorldBoss2Result")
	self:RegisterProtocol(SCWorldBoss3Result, "OnWorldBoss3Result")
	self:RegisterProtocol(SCWorldBossPosInfo, "OnWorldBossPosInfo")
	self:RegisterProtocol(SCWorldBoss2PosInfo, "OnWorldBoss2PosInfo")
	self:RegisterProtocol(SCWorldBoss3PosInfo, "OnWorldBoss3PosInfo")
	self:RegisterProtocol(SCWorldBossBattleInfo, "OnWorldBossBattleInfo")
	self:RegisterProtocol(SCWorldBoss2BattleInfo, "OnWorldBoss2BattleInfo")
	self:RegisterProtocol(SCWorldBoss3BattleInfo, "OnWorldBoss3BattleInfo")

	self:RegisterProtocol(SCWildBossNotice, "OnWildBossNotice")
	--[[ self:RegisterProtocol(SCWorldBossRoleInfo, "OnWorldBossRoleInfo")
	self:RegisterProtocol(SCWorldBoss2RoleInfo, "OnWorldBoss2RoleInfo") ]]

	self:RegisterProtocol(SCWorldBoss2RoleInfo, "SCWorldBoss2RoleInfo")
	self:RegisterProtocol(SCWorldBoss2RankInfo, "SCWorldBoss2RankInfo")

	self:RegisterProtocol(SCWorldBossRoleInfo, "SCWorldBossRoleInfo")
	self:RegisterProtocol(SCWorldBossRankInfo, "SCWorldBossRankInfo")

	self:RegisterProtocol(SCWorldBoss3RoleInfo, "SCWorldBoss3RoleInfo")
	self:RegisterProtocol(SCWorldBoss3RankInfo, "SCWorldBoss3RankInfo")
end

function WorldBossCtrl:OnInit()
	ActivityData.Instance:CustomClickHandle(ActType.WorldBoss, function()
		if ActivityData.Instance:GetActivityStatus(ActType.WorldBoss) == ActStatusType.Standy then
			PublicPopupCtrl.Instance:Center(Language.WorldBoss.ActivityTip[1])
			return true
		end
		local pos_info = self.data:GetPosInfo()
		function autotowb1()
			if pos_info ~= nil then
				pos_info.obj_type = SceneObjType.Monster
				SceneCtrl.Instance:Logic():AutoToNpc(pos_info, nil, true, true)
			end
		end
		if not LoginData.Instance:IsOnCrossSever() then
			LoginCtrl.Instance:SendCrossStartReq()
			if nil ~= self.handle_wb1 then
				LoginData.Instance:BaseData():Uncare(self.handle_wb1)
				self.handle_wb1 = nil
			end
			self.handle_wb1 = LoginData.Instance:BaseData():Care(function ()
				LoginData.Instance:BaseData():Uncare(self.handle_wb1)
				self.handle_wb1 = nil
				TimeHelper:AddDelayTimer(autotowb1, 1)
			end, "is_on_crosssever")
		else
			autotowb1()
		end
	end)

	ActivityData.Instance:CustomClickHandle(ActType.WorldBoss2, function ()
		if ActivityData.Instance:GetActivityStatus(ActType.WorldBoss2) == ActStatusType.Standy then
			PublicPopupCtrl.Instance:Center(Language.WorldBoss.ActivityTip[2])
			return true
		end
		local pos_info = self.data:GetPosInfo2()
		function autotowb2()
			if pos_info ~= nil then
				pos_info.obj_type = SceneObjType.Monster
				SceneCtrl.Instance:Logic():AutoToNpc(pos_info, nil, true, true)
			end
		end
		if not LoginData.Instance:IsOnCrossSever() then
			LoginCtrl.Instance:SendCrossStartReq()
			if nil ~= self.handle_wb2 then
				LoginData.Instance:BaseData():Uncare(self.handle_wb2)
				self.handle_wb2 = nil
			end
			self.handle_wb2 = LoginData.Instance:BaseData():Care(function ()
				LoginData.Instance:BaseData():Uncare(self.handle_wb2)
				self.handle_wb2 = nil
				TimeHelper:AddDelayTimer(autotowb2, 1)
			end, "is_on_crosssever")
		else
			autotowb2()
		end
	end)

	ActivityData.Instance:CustomClickHandle(ActType.WorldBoss3, function ()
		if ActivityData.Instance:GetActivityStatus(ActType.WorldBoss3) == ActStatusType.Standy then
			PublicPopupCtrl.Instance:Center(Language.WorldBoss.ActivityTip[3])
			return true
		end
		local pos_info = self.data:GetPosInfo3()
		function autotowb3()
			if pos_info ~= nil then
				pos_info.obj_type = SceneObjType.Monster
				SceneCtrl.Instance:Logic():AutoToNpc(pos_info, nil, true, true)
			end
		end
		if not LoginData.Instance:IsOnCrossSever() then
			LoginCtrl.Instance:SendCrossStartReq()
			if nil ~= self.handle_wb3 then
				LoginData.Instance:BaseData():Uncare(self.handle_wb3)
				self.handle_wb3 = nil
			end
			self.handle_wb3 = LoginData.Instance:BaseData():Care(function ()
				LoginData.Instance:BaseData():Uncare(self.handle_wb3)
				self.handle_wb3 = nil
				TimeHelper:AddDelayTimer(autotowb3, 1)
			end, "is_on_crosssever")
		else
			autotowb3()
		end
	end)
end

function WorldBossCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCWorldBossInfo)
	self:UnRegisterProtocol(SCWorldBossResult)
	self:UnRegisterProtocol(SCWorldBossPosInfo)
	self:UnRegisterProtocol(SCWorldBossBattleInfo)
	self:UnRegisterProtocol(SCWildBossNotice)
	self:UnRegisterProtocol(SCWildBossPosiInfo)
end

function WorldBossCtrl:OnWorldBossInfo(protocol)
    self.data:SetWorldBossInfo(protocol)
end

function WorldBossCtrl:OnWorldBoss2Info(protocol)
    self.data:SetWorldBoss2Info(protocol)
end

function WorldBossCtrl:OnWorldBoss3Info(protocol)
    self.data:SetWorldBoss3Info(protocol)
end

function WorldBossCtrl:OnWorldBossResult(protocol)
	self.data.result_act_type = ActType.WorldBoss
	self.data:SetWorldBossResult(protocol)
	self:OpenWorldBossView()
end

function WorldBossCtrl:OnWorldBoss2Result(protocol)
	self.data.result_act_type = ActType.WorldBoss2
	self.data:SetWorldBossResult(protocol)
	self:OpenWorldBossView()
end

function WorldBossCtrl:OnWorldBoss3Result(protocol)
	self.data.result_act_type = ActType.WorldBoss3
	self.data:SetWorldBossResult(protocol)
	self:OpenWorldBossView()
end

function WorldBossCtrl:OnWorldBossPosInfo(protocol)
	self.data:SetWorldBossPosInfo(protocol)
end

function WorldBossCtrl:OnWorldBoss2PosInfo(protocol)
	self.data:SetWorldBoss2PosInfo(protocol)
end

function WorldBossCtrl:OnWorldBoss3PosInfo(protocol)
	self.data:SetWorldBoss3PosInfo(protocol)
end

function WorldBossCtrl:OnWorldBossBattleInfo(protocol)
	local battle_info = protocol.battle_info
	FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_SERVER_BOSS,{desc_func = function()
		return string.format(Language.WorldBoss.BattleFinish, battle_info.team_total_damage, battle_info.team_total_heal)
    end,value_func = function()
		return {{damage = battle_info.team_total_damage , heal = battle_info.team_total_heal}}
	end})
end

function WorldBossCtrl:OnWorldBoss2BattleInfo(protocol)
	local battle_info = protocol.battle_info
	FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2,{desc_func = function()
		return string.format(Language.WorldBoss.BattleFinish, battle_info.team_total_damage, battle_info.team_total_heal)
    end,value_func = function()
		return {{damage = battle_info.team_total_damage , heal = battle_info.team_total_heal}}
	end})
end

function WorldBossCtrl:OnWorldBoss3BattleInfo(protocol)
	local battle_info = protocol.battle_info
	FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3,{desc_func = function()
		return string.format(Language.WorldBoss.BattleFinish, battle_info.team_total_damage, battle_info.team_total_heal)
    end,value_func = function()
		return {{damage = battle_info.team_total_damage , heal = battle_info.team_total_heal}}
	end})
end

function WorldBossCtrl:OnWildBossNotice(protocol)
	local npc_vo = SceneLogic.Instance:CurTarget()
	BehaviorCtrl.ExeByCfgVo({
		behavior_type=1,
		param1="",
		param2="",
		param3="",
        talk_list = {
			[1] = {
				talk_seq=1,
				talker=npc_vo.id,
				npc_res_id = Cfg.NpcById(npc_vo.id).res_id,
				talk_text=Language.WorldBoss.WildBossNoticeContent[protocol.notice_type]
			}
		}
	})
end

--[[ function WorldBossCtrl:OnWorldBossRoleInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WorldBoss, protocol.play_times, Config.monster_taofaboss_auto.other[1].max_time)
end ]]

--[[ function WorldBossCtrl:OnWorldBoss2RoleInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WorldBoss2, protocol.play_times, Config.monster_taofaboss_auto.other[1].max_time)
end ]]

function WorldBossCtrl:SendWorldBossReq(param_t)
	local protocol = GetProtocol(CSWorldBossReq)
	protocol.req_type = param_t.req_type
	SendProtocol(protocol)
end

function WorldBossCtrl:SendWorldBossInfo()
	self:SendWorldBossReq{
		req_type = WorldBossConfig.OperType.info
	}
end

function WorldBossCtrl:SendWorldBossCancelInfo()
	self:SendWorldBossReq{
		req_type = WorldBossConfig.OperType.cancel_info
	}
end

function WorldBossCtrl:SendWorldBoss2Req(param_t)
	local protocol = GetProtocol(CSWorldBoss2Req)
	protocol.req_type = param_t.req_type
	SendProtocol(protocol)
end

function WorldBossCtrl:SendWorldBoss2Info()
	self:SendWorldBoss2Req{
		req_type = WorldBossConfig.OperType.info
	}
end

function WorldBossCtrl:SendWorldBoss2CancelInfo()
	self:SendWorldBoss2Req{
		req_type = WorldBossConfig.OperType.cancel_info
	}
end

function WorldBossCtrl:SendWorldBoss3Req(param_t)
	local protocol = GetProtocol(CSWorldBoss3Req)
	protocol.req_type = param_t.req_type
	SendProtocol(protocol)
end

function WorldBossCtrl:SendWorldBoss3Info()
	self:SendWorldBoss3Req{
		req_type = WorldBossConfig.OperType.info
	}
end

function WorldBossCtrl:SendWorldBoss3CancelInfo()
	self:SendWorldBoss3Req{
		req_type = WorldBossConfig.OperType.cancel_info
	}
end

function WorldBossCtrl:OpenWorldBossView()
	if MainViewData.Instance:IsBattle() then
		if nil ~= self.handle_wbv then
			MainViewData.Instance:BaseData():Uncare(self.handle_wbv)
			self.handle_wbv = nil
		end
		self.handle_wbv = MainViewData.Instance:BaseData():Care(function ()
			MainViewData.Instance:BaseData():Uncare(self.handle_wbv)
			self.handle_wbv = nil
			ViewMgr:OpenView(WorldBossView)
		end, "is_battle")
	else
		ViewMgr:OpenView(WorldBossView)
	end
end
function WorldBossCtrl:SCWorldBoss2RoleInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WorldBoss2, protocol.play_times, Config.monster_taofaboss_auto.other[1].max_time)
	self.data:MyRankInfo().shanghai = protocol.shanghai
	self.data:MyRankInfo().zhiliao = protocol.zhiliao
end
function WorldBossCtrl:SCWorldBoss3RoleInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WorldBoss3, protocol.play_times, Config.monster_tianshen_auto.other[1].max_time)
	self.data:MyRankInfo().shanghai = protocol.shanghai
	self.data:MyRankInfo().zhiliao = protocol.zhiliao
end
function WorldBossCtrl:SCWorldBoss2RankInfo(protocol)
	if protocol.type == 0 then
		self.data:BossRankInfo().list0 = protocol.hurt_rank
		self.data:MyRankInfo().rank0 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank0 = index
			end
		end
	elseif protocol.type == 1 then
		self.data:BossRankInfo().list1 = protocol.hurt_rank
		self.data:MyRankInfo().rank1 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank1 = index
			end
		end
	end
end
function WorldBossCtrl:SCWorldBoss3RankInfo(protocol)
	if protocol.type == 0 then
		self.data:BossRankInfo().list0 = protocol.hurt_rank
		self.data:MyRankInfo().rank0 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank0 = index
			end
		end
	elseif protocol.type == 1 then
		self.data:BossRankInfo().list1 = protocol.hurt_rank
		self.data:MyRankInfo().rank1 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank1 = index
			end
		end
	end
end
function WorldBossCtrl:SCWorldBossRoleInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WorldBoss, protocol.play_times, Config.monster_taofaboss_auto.other[1].max_time)
	self.data:MyRankInfo().shanghai = protocol.shanghai
	self.data:MyRankInfo().zhiliao = protocol.zhiliao
end
function WorldBossCtrl:SCWorldBossRankInfo(protocol)
	if protocol.type == 0 then
		self.data:BossRankInfo().list0 = protocol.hurt_rank
		self.data:MyRankInfo().rank0 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank0 = index
			end
		end
	elseif protocol.type == 1 then
		self.data:BossRankInfo().list1 = protocol.hurt_rank
		self.data:MyRankInfo().rank1 = 0
		for index, value in ipairs(protocol.hurt_rank) do
			if value.role_id == RoleData.Instance:GetRoleId() then
				self.data:MyRankInfo().rank1 = index
			end
		end
	end
end

function WorldBossCtrl:Subscribe()
	if self.data:ActType() == ActType.WorldBoss then
		LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.World_Boss_1)
	elseif self.data:ActType() == ActType.WorldBoss2 then
		LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.World_Boss_2)
	else 
		LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.World_Boss_3)
	end
end
function WorldBossCtrl:SubscribeCancel()
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.World_Boss_1)
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.World_Boss_2)
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.World_Boss_3)
end