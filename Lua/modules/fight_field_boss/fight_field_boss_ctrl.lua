FILE_REQ("modules/fight_field_boss/fight_field_boss_data")
VIEW_REQ("modules/fight_field_boss/fight_field_boss_view")
VIEW_REQ("modules/fight_field_boss/fight_hill_sea_view")

FightFieldBossCtrl = FightFieldBossCtrl or BaseClass(BaseCtrl)

function FightFieldBossCtrl:__init()
	if FightFieldBossCtrl.Instance ~= nil then
		Debuger.LogError("[FightFieldBossCtrl] attempt to create singleton twice!")
		return
	end	
    FightFieldBossCtrl.Instance = self
    self.data = FightFieldBossData.New()
	self:RegisterAllProtocols()
end

function FightFieldBossCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSWildBossReq)
	self:RegisterProtocol(SCWildBossInfo, "OnWildBossInfo")
	self:RegisterProtocol(SCWildBossSingleInfo, "OnWildBossSingleInfo")
	self:RegisterProtocol(SCWildBossPokerInfo, "OnWildBossPokerInfo")

	self:RegisterProtocol(CSShanHaiBossInfoReq)
	self:RegisterProtocol(SCShanHaiBossInfo,"OnShanHaiBossInfo")
	self:RegisterProtocol(SCShanHaiBossRoleInfo,"OnShanHaiBossRoleInfo")
	self:RegisterProtocol(SCShanHaiBossSingleInfo,"OnShanHaiBossSingleInfo")
	self:RegisterProtocol(SCShanHaiBossPokerInfo,"OnShanHaiBossPokerInfo")

	self:RegisterProtocol(SCWildBossPosiInfo, "OnSCWildBossPosiInfo")
end

function FightFieldBossCtrl:OnInit()
	ActivityData.Instance:CustomClickHandle(ActType.FightFieldBoss,function()
        return ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Main)
	end)
	ActivityData.Instance:CustomClickHandle(ActType.ShxyBoss,function()
        return ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Shxy)
	end)
	EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.OnRoleNoticeComplete, self))
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_WORLD_BOSS,function()
		PokerCtrl.Instance:OnOpenPokerView(self.data:GetPokerList(),POKER_REQ_OPER_TYPE.TYPE_WILD_BOSS)
	end)
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_SHXY,function(is_win)
		if is_win == 1 then
			local reward_data = self.data:GetWorldBossRewardData()
			if reward_data ~= nil then
				MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(reward_data),GIFT_OPEN_TYPE.CONGRATULATIONS)
			end
		end
	end)
end

function FightFieldBossCtrl:OnRoleNoticeComplete()
	self:SendWildBossReq(1)
	self:SendShanHaiBossInfoReq()
end

--野外首领请求
function FightFieldBossCtrl:SendWildBossReq(req_type,param)
	local protocol = GetProtocol(CSWildBossReq)
	protocol.req_type = req_type or 1
	protocol.param = param or 0
	SendProtocol(protocol)
end

--野外首领信息
function FightFieldBossCtrl:OnWildBossInfo(protocol)
	local max_times = Config.monster_yewaiboss_auto.other[1].weekly_time
	local cur_times = Valve(protocol.today_times,max_times)
	ActivityData.Instance:SetActivityFinishInfo(ActType.FightFieldBoss,cur_times,max_times)
	ActivityData.Instance:FlushDailyRed()
	self.data:SetWildBossInfo(protocol)
end

--野外首领单独信息
function FightFieldBossCtrl:OnWildBossSingleInfo(protocol)
	self.data:SetWildBossSingleInfo(protocol)
end

--山海降妖请求
function FightFieldBossCtrl:SendShanHaiBossInfoReq(req_type,param)
	local protocol = GetProtocol(CSShanHaiBossInfoReq)
	protocol.req_type = req_type or 0
	protocol.param = param or 0
	SendProtocol(protocol)
end

--野外boss翻牌
function FightFieldBossCtrl:OnWildBossPokerInfo(protocol)
	self.data:SetBossPokerInfo(protocol.info)
end

--替换队伍中的翻牌信息
function FightFieldBossCtrl:OnWildBossPokerChange(protocol)
	PokerData.Instance:GetMypokerByrIndex(self.data:GetBossPokerInfo(),protocol)
end

--山海降妖信息
function FightFieldBossCtrl:OnShanHaiBossInfo(protocol)
	self.data:SetShanHaiBossInfo(protocol)
end

function FightFieldBossCtrl:OnShanHaiBossRoleInfo(protocol)
	local max_times = Config.monster_shxyboss_auto.other[1].day_time
	local cur_times = Valve(protocol.challenge_count,max_times)
	ActivityData.Instance:SetActivityFinishInfo(ActType.ShxyBoss,cur_times,max_times)
	self.data:SetShanHaiBossRoleInfo(protocol)
end

--山海降单个信息
function FightFieldBossCtrl:OnShanHaiBossSingleInfo(protocol)
	self.data:SetShanHaiBossSingleInfo(protocol)
end

--山海降妖翻牌
function FightFieldBossCtrl:OnShanHaiBossPokerInfo(protocol)
	self.data:SetBossPokerInfo(protocol.info)
end
--根据seq获取到的山海降妖单个坐标
function FightFieldBossCtrl:OnSCWildBossPosiInfo(protocol)
	self.data:SetBossPos(protocol)
end