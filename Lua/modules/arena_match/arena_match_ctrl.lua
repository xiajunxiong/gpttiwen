
FILE_REQ("modules/arena_match/arena_match_config")
FILE_REQ("modules/arena_match/arena_match_info")
FILE_REQ("modules/arena_match/arena_match_data")

VIEW_REQ("modules/arena_match/board_am")
VIEW_REQ("modules/arena_match/arena_match_items")
VIEW_REQ("modules/arena_match/arena_match_view")

VIEW_REQ("modules/arena_match/arena_match_view_temp")
VIEW_REQ("modules/arena_match/arena_match_rank_view")
VIEW_REQ("modules/arena_match/arena_match_info_view")
VIEW_REQ("modules/arena_match/arena_match_winner_view")
VIEW_REQ("modules/arena_match/arena_match_add_view")
VIEW_REQ("modules/arena_match/arena_match_reward_preview_view")
VIEW_REQ("modules/arena_match/arena_match_menu_view")
VIEW_REQ("modules/arena_match/arena_match_fight_report_view")
VIEW_REQ("modules/arena_match/arena_match_achieve_view")
VIEW_REQ("modules/arena_match/arena_match_detail_view")
VIEW_REQ("modules/arena_match/arena_match_last_record")

ArenaMatchCtrl = ArenaMatchCtrl or BaseClass(BaseCtrl)

function ArenaMatchCtrl:__init()
	if ArenaMatchCtrl.Instance ~= nil then
		Debuger.LogError("[ArenaMatchCtrl] attempt to create singleton twice!")
		return
	end	
	ArenaMatchCtrl.Instance = self
	self.data = ArenaMatchData.New()

	self:RegisterAllProtocols()
	ActivityData.Instance:RegisterTopCondition(ActType.ArenaMatch,function()
        return self.data:GetTopConditionTip()
	end)
	self:CheckEndTime()
end

function ArenaMatchCtrl:__delete()
	if ArenaMatchCtrl.Instance == self then
		ArenaMatchCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
end

function ArenaMatchCtrl:OnInit()
    ActivityData.Instance:CustomClickHandle(ActType.ArenaMatch, function ()
		ViewMgr:OpenView(ArenaMatchView)
	end)
end

function ArenaMatchCtrl:OnUnloadGameLogic()
	self.data.EndTime.time = 0
end

function ArenaMatchCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSPlatformBattleReq)
	self:RegisterProtocol(CSPlatformBattleRankFirstInfoReq)
	self:RegisterProtocol(CSPlatformBattlePlaformSwitch)
	self:RegisterProtocol(CSPlatformBattleAchieve)

	self:RegisterProtocol(SCPlatformBattleRoleInfo, "OnPlatformBattleRoleInfo")
	self:RegisterProtocol(SCPlatformBattleRankInfo, "OnPlatformBattleRankInfo")
	self:RegisterProtocol(SCPlatformBattleRankFirstInfo, "OnPlatformBattleRankFirstInfo")
	self:RegisterProtocol(SCPlatformBattleRecord,"OnPlatformBattleRecord")
	self:RegisterProtocol(SCPlatformBattleRecordNotice,"OnPlatformBattleRecordNotice")
	self:RegisterProtocol(SCPlatformBattleAchieveInfo,"OnSCPlatformBattleAchieveInfo")
	self:RegisterProtocol(SCPlatformBattleEndNotice,"OnSCPlatformBattleEndNotice")
	self:RegisterProtocol(SCPlatformBattleLastRankInfo,"OnSCPlatformBattleLastRankInfo")

	ActivityData.Instance:RegisterCondition(ActType.ArenaMatch,function()
        return self.data:GetConditionTip()
	end)
	ActivityData.Instance:RegisterRemind(ActType.ArenaMatch,function()
        if ActivityData.Instance:IsNoEnoughLevel(ActType.ArenaMatch,true) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
end

function ArenaMatchCtrl:UnRegisterAllProtocols()
end

-- 检查擂台倒计时结算预告
function ArenaMatchCtrl:CheckEndTime()
	TimeHelper:AddRunTimer(function()
		if LoginData.Instance:LoginState() == LoginStateType.Logined then
			if self.data.EndTime.time == nil or self.data.EndTime.time <= 0 then
				self.data.EndTime.time = self.data:GetDayEndTime()
			else
				local time = self.data.EndTime.time - 10
				self.data.EndTime:Set({time = time})
				if time < ArenaMatchConfig.RemindEndTime then
					if not self.end_time_flag then
						ActivityData.Instance.activity_status_event:SetDirty("status")
						self.end_time_flag = true
					end
				else
					self.end_time_flag = false
				end
			end
		end
	end, 10)
	--五方擂台结算提醒开启
	ActivityRandData.Instance:Register(Mod.ActivityRemind.ArenaMatch,function()
		if not FunOpen.Instance:IsFunOpen(Mod.ArenaMatch.Arena) then
			return false
		end
		local time = ArenaMatchData.Instance.EndTime.time
		if time == nil or time <= 0 then
			time = ArenaMatchData.Instance:GetDayEndTime()
			ArenaMatchData.Instance.EndTime.time = time
		end
		if time > 0 and time <= ArenaMatchConfig.RemindEndTime then
			self.end_time_flag = true
			return true
		end
		return false
	end,self.data.EndTime)
end

function ArenaMatchCtrl:OnPlatformBattleRoleInfo(protocol)
	PetData.Instance:SetArenaMatchIndexList(protocol.arena_match_index_list)
	PartnerData.Instance:SetArenaMatchIdList(protocol.arena_match_id_list)
    self.data:SetPlatformBattleRoleInfo(protocol)
end

function ArenaMatchCtrl:OnPlatformBattleRankInfo(protocol)
	self.data:SetPlatformBattleRankInfo(protocol)
	if self.data.is_open_rank then
		self.data.is_open_rank = false
		if ViewMgr:IsOpen(ArenaMatchRankView) then
			ViewMgr:FlushView(ArenaMatchRankView)
		elseif self.data.cur_sel_arena.seq == protocol.platform_number then
			ViewMgr:OpenView(ArenaMatchRankView)
		else
			-- Debuger.LogError("error 2782 rank")
		end
	end
	if self.data.is_open_finish then
		self.data.is_open_finish = false
		if ViewMgr:IsOpen(FinishPassViewAM) then
			ViewMgr:FlushView(FinishPassViewAM)
		elseif self.data.cur_sel_arena.seq == protocol.platform_number then
			ViewMgr:OpenView(FinishPassViewAM)
		else
			-- Debuger.LogError("error 2782 finish")
		end
	end
end

function ArenaMatchCtrl:OnPlatformBattleRankFirstInfo(protocol)
	self.data:SetPlatformBattleRankFirstInfo(protocol)
end

function ArenaMatchCtrl:SendPlatformBattleReq(param_t)
	local protocol = GetProtocol(CSPlatformBattleReq)
	protocol.oper_type = param_t.oper_type or 0
	protocol.p1 = param_t.p1 or 0
	protocol.p2 = param_t.p2 or 0
	protocol.pet_list = param_t.pet_list or {}
	protocol.partner_list = param_t.partner_list or {}
	protocol.pet_num = param_t.pet_num or 0
	protocol.partner_num = param_t.partner_num or 0
	protocol.pos_c2s_config = param_t.pos_c2s_config or {}
	SendProtocol(protocol)
end

function ArenaMatchCtrl:SendPlatformBattleRankFirstInfoReq(arena_list)
	local protocol = GetProtocol(CSPlatformBattleRankFirstInfoReq)
	protocol.arena_list = arena_list
	SendProtocol(protocol)
end

function ArenaMatchCtrl:SendPlatformBattlePlaformSwitch(platform_seq)
	local protocol = GetProtocol(CSPlatformBattlePlaformSwitch)
	protocol.platform_seq = platform_seq
	SendProtocol(protocol)
end

function ArenaMatchCtrl:SendPlatformBattleRankFirstInfoReqSceneId(scene_id)
	self.data:ArenaShowListClear()
	local co = self.data:GetArenaPosSceneInfo(scene_id)
	if nil == co then return end
	self:SendPlatformBattleRankFirstInfoReq(self.data:GetArenaListByPos(co.position))
end

function ArenaMatchCtrl:SendPlatformBattleRankFirstInfoReqSeq(seq)
	self:SendPlatformBattleRankFirstInfoReq({{seq = seq}})
end

function ArenaMatchCtrl:SendPlatformBattleRoleInfo()
	self:SendPlatformBattleReq{
		oper_type = ArenaMatchConfig.OperType.role_info,
	}
end

function ArenaMatchCtrl:SendPlatformBattleRankInfo(arena)
	self:SendPlatformBattleReq{
		oper_type = ArenaMatchConfig.OperType.rank_info,
		p1 = arena,
	}
end

function ArenaMatchCtrl:SendPlatformBattleChallenge(pet_list, partner_list)
	self:SendPlatformBattleReq{
		oper_type = ArenaMatchConfig.OperType.challenge,
		p1 = self.data.cur_sel_info:ArenaSeq(),
		p2 = self.data.cur_sel_info:ArenaRank(),
		pet_list = self.data.cur_sel_info:PetList(),
		partner_list = self.data.cur_sel_info:PartnerList(),
		pet_num = self.data.cur_sel_info:PetNum(),
		partner_num = self.data.cur_sel_info:PartnerNum(),
		pos_c2s_config = self.data.cur_sel_info.pos_c2s_config
	}
end

function ArenaMatchCtrl:SendPlatformBattleAdjust(pet_list, partner_list)
	self:SendPlatformBattleReq{
		oper_type = ArenaMatchConfig.OperType.adjust,
		p1 = self.data.cur_sel_info:ArenaSeq(),
		pet_list = self.data.cur_sel_info:PetList(),
		partner_list = self.data.cur_sel_info:PartnerList(),
		pet_num = self.data.cur_sel_info:PetNum(),
		partner_num = self.data.cur_sel_info:PartnerNum(),
		pos_c2s_config = self.data.cur_sel_info.pos_c2s_config
	}
end

function ArenaMatchCtrl:SendPlatformBattleRemove(arena)
	PublicPopupCtrl.Instance:DialogTips{
		content = Language.ArenaMatch.RecallTips,
		confirm = {
			func = function()
				self:SendPlatformBattleReq{
					oper_type = ArenaMatchConfig.OperType.remove,
					p1 = arena,
				}
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end

function ArenaMatchCtrl:FightByNpcSeq(npc_seq)
	local arena = self.data:GetArenaListByNpcSeq(npc_seq)
	if arena then
		self.data.is_open_rank = true
		self.data.cur_sel_arena = arena
		self:SendPlatformBattleRankInfo(arena.seq)
		self:SendPlatformBattleRankFirstInfoReqSeq(arena.seq)
	end
end

function ArenaMatchCtrl:FightFinish(is_win)
	self.data.is_open_finish = true
	self.data.finish_is_win = is_win
	self:SendPlatformBattleRankInfo(self.data.cur_sel_arena.seq)
end

--擂台战报信息回调
function ArenaMatchCtrl:OnPlatformBattleRecord(protocol)
	self.data:SetRecordInfos(protocol)
	--LogError("擂台战报信息",protocol)
end

--擂台红点通知回调
function ArenaMatchCtrl:OnPlatformBattleRecordNotice(protocol)
	self.data:SetNoticeData(protocol)
	local actInfo = ActivityData.Instance:GetActivityInfo(ActType.ArenaMatch)
    if actInfo then
        actInfo:ActRedPoint()
    end
	--LogError("擂台战报通知信息",protocol)
end

-- Req2
function ArenaMatchCtrl:SendReq2(oper_type,p1,p2)
	local protocol = GetProtocol(CSPlatformBattleAchieve)
	protocol.oper_type = oper_type
	protocol.p1 = p1 or 0
	protocol.p2 = p2 or 0
	--LogError("请求擂台成就奖励：",protocol)
	SendProtocol(protocol)
end

function ArenaMatchCtrl:OnSCPlatformBattleAchieveInfo(protocol)
	--LogError("擂台成就协议内容：",protocol)
	self.data:HandleAchievementProto(protocol)
	local actInfo = ActivityData.Instance:GetActivityInfo(ActType.ArenaMatch)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

-- 擂台结算通知，清空数据
function ArenaMatchCtrl:OnSCPlatformBattleEndNotice(protocol)
	-- 清空战报
	self.data.record_infos:Set({})
	-- 清空成就
	self.data.achievement_data:Set({flags = {},type_count_info = {}})
	-- 清空队伍
	self.data.role_info_list:Set({})
	self.data.platform_notice_data.is_first_times = 1
	local actInfo = ActivityData.Instance:GetActivityInfo(ActType.ArenaMatch)
    if actInfo then
        actInfo:ActRedPoint()
	end
	--LogError("擂台结算通知了")
end

-- 擂台上次排名信息
function ArenaMatchCtrl:OnSCPlatformBattleLastRankInfo(protocol)
	local rank_list = protocol.info_list
	ViewMgr:OpenView(ArenaMatchLastRecord, {rank_list = rank_list})
end