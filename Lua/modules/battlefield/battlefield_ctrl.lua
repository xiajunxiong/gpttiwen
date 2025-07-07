FILE_REQ("modules/battlefield/battlefield_config")
FILE_REQ("modules/battlefield/battlefield_data")
VIEW_REQ("modules/battlefield/battlefield_view")
VIEW_REQ("modules/battlefield/battlefield_match")
VIEW_REQ("modules/battlefield/battlefield_other")
VIEW_REQ("modules/battlefield/battlefield_shop_view")
VIEW_REQ("modules/battlefield/battlefield_rank_view")
VIEW_REQ("modules/battlefield/battlefield_report_view")
VIEW_REQ("modules/battlefield/battlefield_reward_view")

BattlefieldCtrl = BattlefieldCtrl or BaseClass(BaseCtrl)

function BattlefieldCtrl:__init()
	if BattlefieldCtrl.Instance ~= nil then
		Debuger.LogError("[BattlefieldCtrl] attempt to create singleton twice!")
		return
	end	
    BattlefieldCtrl.Instance = self
    self.data = BattlefieldData.New()
	self:RegisterAllProtocols()
end

function BattlefieldCtrl:OnUnloadGameLogic()
	self.data:InitSmartData()
end

function BattlefieldCtrl:OnInit()
	ActivityData.Instance:CustomClickHandle(ActType.Battlefield, BindTool.Bind(self.OnBattlefield, self))
	ActivityData.Instance:CustomClickHandle(ActType.BattlefieldAgain, BindTool.Bind(self.OnBattlefield, self))
	ActivityData.Instance:RegisterTopCondition(ActType.BattlefieldAgain,function()
        return self.data:GetTopConditionTip()
	end)
	ActivityData.Instance:RegisterCondition(ActType.BattlefieldAgain,function()
        return self.data:GetConditionTip()
	end)
	FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_BATTLEFIELD,{
        desc_func = function()
            return Language.Battlefield.PassTips
        end,
		value_func = function()
			return self:GetGetFinishTip(true)
        end,
	})
	FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_BATTLEFIELD,{
        desc_func = function()
            return Language.Battlefield.FailTips
        end,
		value_func = function()
			return self:GetGetFinishTip(false)
        end,
	})
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_BATTLEFIELD,function()
		local last_info = self.data:GetLastInfo()
		if self:IsPopupGain(last_info) then
			if last_info.is_top_race == 0 then
				ViewMgr:OpenView(BattlefieldGainView,{
					old_data = self.data:GetRankingConfig(0),
					new_data = self.data.Instance:GetRankingConfig(self.data:GetRankingNum()),
				})
			else
				ViewMgr:OpenView(BattlefieldGainView,{
					old_data = self.data:GetRankingConfig(last_info.ranking_num),
					new_data = self.data.Instance:GetRankingConfig(self.data:GetRankingNum()),
				})
			end
		else
			if not ViewMgr:IsOpen(BattlefieldMatch) then
				self:OnToBattlefieldMatch()
			end
		end
	end)
	-- self.handle_act = ActivityData.Instance.activity_status_event:KeysCare({"act_type", "status"},BindTool.Bind(self.OnActEvent, self))
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SendRoleInfoReq, self))

	ActivityData.Instance:RegisterRemind(ActType.BattlefieldAgain,function()
        if ActivityData.Instance:IsNoEnoughLevel(ActType.BattlefieldAgain,true) then
            return 0
        end
		if not ActivityData.IsOpen(ActType.Battlefield) then
			return 0
		end
        return self.data:GetRemindNum()
    end)
end

function BattlefieldCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSBattlefieldReq)
	self:RegisterProtocol(CSBattlefieldRoleItemReq)
	self:RegisterProtocol(SCTeamMemberNotice,"OnTeamMemberNotice")
	self:RegisterProtocol(SCTeamMemberOtherNotice,"OnTeamMemberOtherNotice")
	self:RegisterProtocol(SCBattlefieldMatch,"OnBattlefieldMatch")
	self:RegisterProtocol(SCBattlefieldResult,"OnBattlefieldResult")
	self:RegisterProtocol(SCBattlefieldRoleInfo,"OnBattlefieldRoleInfo")
	self:RegisterProtocol(SCBattlefieldOpponentResult,"OnBattlefieldOpponentResult")
	self:RegisterProtocol(SCBattlefieldRankInfo,"OnBattlefieldRankInfo")
	self:RegisterProtocol(SCBattlefieldReportInfo,"OnBattlefieldReportInfo")
	self:RegisterProtocol(SCBattlefieldReportRateInfo,"OnBattlefieldReportRateInfo")
	self:RegisterProtocol(SCBattlefieldRoleItemInfo,"OnBattlefieldRoleItemInfo")
end

function BattlefieldCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSBattlefieldReq)
	self:UnRegisterProtocol(CSBattlefieldRoleItemReq)
	self:UnRegisterProtocol(SCTeamMemberNotice)
	self:UnRegisterProtocol(SCTeamMemberOtherNotice)
	self:UnRegisterProtocol(SCBattlefieldMatch)
	self:UnRegisterProtocol(SCBattlefieldResult)
	self:UnRegisterProtocol(SCBattlefieldRoleInfo)
	self:UnRegisterProtocol(SCBattlefieldOpponentResult)
	self:UnRegisterProtocol(SCBattlefieldRankInfo)
	self:UnRegisterProtocol(SCBattlefieldReportInfo)
	self:UnRegisterProtocol(SCBattlefieldReportRateInfo)
	self:UnRegisterProtocol(SCBattlefieldRoleItemInfo)
end

function BattlefieldCtrl:__delete()
	self:UnRegisterAllProtocols()
	BattlefieldCtrl.Instance = nil
	Delete(self,"data")
end

function BattlefieldCtrl:SendBattlefieldReq(req_type,param1)
	local protocol = GetProtocol(CSBattlefieldReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function BattlefieldCtrl:SendBattlefieldInfoReq(info_list)
	local protocol = GetProtocol(CSBattlefieldRoleItemReq)
	protocol.info_list = info_list or {}
	SendProtocol(protocol)
end

----------------------服务端数据下发----------------------
function BattlefieldCtrl:OnBattlefieldMatch(protocol)
	self.data:SetBattlefieldMatch(protocol)
end

function BattlefieldCtrl:OnBattlefieldResult(protocol)
	self.data:SetBattlefieldResult(protocol)
	if protocol.result ~= 1 then
		FinishCtrl.Instance:Finish({is_win = protocol.result == 0 and 1 or 0,result_type = protocol.result == 2 and 3 or nil},BATTLE_MODE.BATTLE_MODE_BATTLEFIELD)
	end
end

function BattlefieldCtrl:OnBattlefieldRoleInfo(protocol)
	if self.data:GetRankScore() ~= protocol.rank_score or self.data:GetRanksInfo().tolal_times ~= protocol.ranks_info.tolal_times then
		self.data:SetBattlefieldRoleInfo(protocol)
	end
	self.data:SetOtherInfo(protocol)
end

function BattlefieldCtrl:OnBattlefieldRankInfo(protocol)
	self.data:SetBattlefieldRankInfo(protocol)
end

function BattlefieldCtrl:OnBattlefieldReportInfo(protocol)
	self.data:SetBattlefieldReportInfo(protocol)
end

function BattlefieldCtrl:OnBattlefieldReportRateInfo(protocol)
	self.data:SetBattlefieldReportRateInfo(protocol)
end

function BattlefieldCtrl:OnTeamMemberNotice(protocol)
	self.data:SetTeamMemberNotice(protocol.notice)
	if TeamCtrl.Instance:Data():InTeam() and not TeamCtrl.Instance:Data():IsLeader() then
		if not MainViewData.Instance:IsBattle() and not ViewMgr:IsOpen(BattlefieldGainView) then --是否同步打开
			self:OnToTeamMemberNotice(protocol.notice)
		end
	end
end

function BattlefieldCtrl:OnToBattlefieldMatch()
	if TeamCtrl.Instance:Data():InTeam() then
		if TeamCtrl.Instance:Data():IsLeader() then
			self:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NOTICE,1)
			self:OpenBattlefieldMatch()
		else
			self:OnToTeamMemberNotice(self.data:GetTeamMemberNotice())
		end
	else
		self:OpenBattlefieldMatch()
	end
end

function BattlefieldCtrl:OnToTeamMemberNotice(notice)
	if notice == 1 and ViewMgr:IsOpen(MainView) then --登录会下发 服务端问题
		self:OpenBattlefieldMatch()
	else
		ViewMgr:CloseView(BattlefieldMatch)
	end
end

function BattlefieldCtrl:OnBattlefieldOpponentResult(protocol)
	MainOtherCtrl.Instance:OnTeamSport(protocol)
end

function BattlefieldCtrl:OnBattlefield()
	if ActivityData.Instance:IsNoEnoughLevel(ActType.Battlefield,true) then
		return true
	end
	if AnotherData.IsGuide() and not ActivityData.IsOpen(ActType.Battlefield)then
		PublicPopupCtrl.Instance:Center(Language.Battlefield.PopupNotOpenTip)
		return true
	end
	if not BattlefieldCtrl.Instance:IsHasTeam() then
        return
    end
    AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.FIRST_ENTER)
	self:OpenBattlefieldMatch()
end

function BattlefieldCtrl:OpenBattlefieldMatch()
	BattlefieldCtrl.Instance:SendAllInfoReq()
	ViewMgr:OpenView(BattlefieldMatch)
end

function BattlefieldCtrl:OnActEvent()
	if not FunOpen.Instance:GetFunIsOpened(Mod.Athletics.Main) then
        return
	end
	if ActivityData.Instance:IsNoEnoughLevel(ActType.Battlefield,true) then
		return
	end
	if ActivityData.Instance:GetActivityEventType() == ActType.Battlefield then
		if ActivityData.Instance:GetActivityStatus(ActType.Battlefield) == ActStatusType.Close then
			ViewMgr:CloseView(BattlefieldMatch)
		end
	end
end

function BattlefieldCtrl:SendRoleInfoReq()
	if self.data:GetStampTime() == 0 then
		BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.ROLE)
	end
end

function BattlefieldCtrl:SendAllInfoReq()
	local server_id = MainProber.server_id or 0
	BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.RINK)
    BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.ROLE)
    BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.RINK,server_id)
end

--是否有段位改变
function BattlefieldCtrl:IsPopupGain(info)
	if info.is_top_race == 0 and self.data:GetTopRace() == 1 then
		return true
	end
	if self.data:GetTopRace() == 1 and info.ranking_num < self.data:GetRankingNum() then
		return true
	end
	return false
end

--获取结算奖励
function BattlefieldCtrl:GetGetFinishTip(is_pass)
	local value_list = {}
	if self.data:GetFinishNum() ~= 0 then
		table.insert(value_list,{item_id = CommonItemId.BattlefieldPoint,name = Language.Battlefield.ShopGoldTip,num = " +" .. self.data:GetFinishNum()})
	end
	local last_info = self.data:GetLastInfo()
	if last_info.is_top_race == 0 then
		local tolal_times = self.data:GetRanksInfo().tolal_times
		local max_race = self.data:GetOtherConfig().positioning_race
		table.insert(value_list,{name = Language.Battlefield.TopRaceProgress,num = Format("%s/%s",tolal_times,max_race)})
	else
		local rank_score = self.data:GetRankScore()
		local value_show = (is_pass and "+" or "-") .. math.abs(last_info.change_val)
		if last_info.cur_score == 0 or last_info.change_val == 0 then
			value_show = ""
		end
		table.insert(value_list,{name = Language.Battlefield.Integral[1],num = Format(Language.Battlefield.Integral[2],rank_score,value_show)})
		
		if last_info.cur_score == 0 or last_info.change_val == 0 then
			if is_pass == false then
				table.insert(value_list,{name = ColorStr(Language.Battlefield.ProtectTip,COLORSTR.Red12),num = ""})
			end
		end
	end
	return value_list
end

--组队判断逻辑
function BattlefieldCtrl:IsHasTeam()
	--判断是否组队
	if TeamCtrl.Instance:Data():InTeam() then
		--判断是否是队长
		if not TeamCtrl.Instance:Data():IsLeader() then
			PublicPopupCtrl.Instance:Center(ErrorText[178])
			return false
		end
		if TeamData.Instance:GetRealGoalData().target_id ~= self.data:GetOtherConfig().target_id then
			PublicPopupCtrl.Instance:Center(ErrorText[180])
			return false
		end
		--判断等级是否满足条件
		for k,v in pairs(TeamCtrl.Instance:Data():GetMemberRoleList()) do
			if v.info.is_partner == 0 and v.info.level < BattlefieldData.Instance:GetOtherConfig().level_limit then
				PublicPopupCtrl.Instance:Error(ErrorText[179])
				return false
			end
			if v.info.is_online == 0 then
				PublicPopupCtrl.Instance:Error(ErrorText[181])
				return false
			end
		end
		--判断人数
		if TeamData.Instance:MemberNumRole() > 3 then
			PublicPopupCtrl.Instance:DialogTips({
				content = Language.Battlefield.PopupTeamTip,
				confirm = {name = Language.Battlefield.BtnTeamTip,func = function()
					ViewMgr:CloseView(DialogTipsView)
					ViewMgr:OpenView(TeamView)
				end
			}})
			return false
		end
	else
		TeamData.Instance:SetCurSelTargetByAct(ActType.BattlefieldAgain)
	end
	if TeamCtrl.Instance:Data():InTeam() and TeamCtrl.Instance:Data():IsLeader() then
		self:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NOTICE,1)
	end
	return true
end

function BattlefieldCtrl:CheckMemberList(is_match)
	local info_list = {}
	for k,v in pairs(self.data:GetMemberList()) do
		if v.info.uid ~= RoleData.Instance:GetRoleId() then
			if is_match == true and v.info.is_follow == 0 then
				TeamCtrl.Instance:SendKickOutOfTeamReq(v.info.member_index)
			end
			table.insert(info_list,v.info.uid)
		end
	end
	if #info_list > 0 then
		self:SendBattlefieldInfoReq(info_list)
	end
end

--点击头像
function BattlefieldCtrl:OnRoleInfo(data)
	if data == nil or data.is_self then
		return
	end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
			return RoleInfoData.RoleInfoNew(data.info)
		end,
		funcs = function()
			local is_friend = SocietyData.Instance:IsFriend(data.info.uid)
			local config = TableCopy(TeamCtrl.Instance:Data():IsLeader() and RoleInfoConfig.LeaderInfo or RoleInfoConfig.SimpleInfo)
			table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
            return config
		end,
		hide_base = true
	}
end

function BattlefieldCtrl:OnRoleSimpleInfo(data)
	if data == nil or data.is_self then
		return
	end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
			return RoleInfoData.RoleInfoNew(data)
		end,
		funcs = function()
			local config = TableCopy(RoleInfoConfig.CommonInfo)
			local is_friend = SocietyData.Instance:IsFriend(data.uid)
			table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
			return config
		end,
	}
end

function BattlefieldCtrl:OnTeamMemberOtherNotice(protocol)
	if TeamCtrl.Instance:Data():InTeam() then
		local role_id,reson = protocol.role_id,protocol.reson
		if role_id ~= RoleData.Instance:GetRoleId() then
			PublicPopupCtrl.Instance:Center(Format(Language.Battlefield.ErrorText[reson],self:GetTeamName(role_id)))
		else
			PublicPopupCtrl.Instance:Center(Language.Battlefield.RoleErrorText[reson])
		end
	end
end

function BattlefieldCtrl:GetTeamName(role_id)
	for k,v in pairs(TeamData.Instance:GetMemberList()) do
		if v.info.uid == role_id then
			return v.info.name
		end
	end
end

function BattlefieldCtrl:OnBattlefieldRoleItemInfo(protocol)
	self.data:SetBattlefieldRoleItemInfo(protocol)
end