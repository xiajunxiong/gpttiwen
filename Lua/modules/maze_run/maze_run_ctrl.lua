FILE_REQ("modules/maze_run/maze_run_data")

VIEW_REQ("modules/maze_run/maze_run_rank_view")
VIEW_REQ("modules/maze_run/maze_run_main_sub")
VIEW_REQ("modules/maze_run/maze_run_reward_view")
VIEW_REQ("modules/maze_run/maze_run_reward_preview")

MazeRunOperType = {
	TRANSFER = 0,		-- 传送
	AMULET = 1,			-- 使用符咒
	ROLE_INFO = 2,		-- 角色信息
}

MazeRunCtrl = MazeRunCtrl or BaseClass(BaseCtrl)


local enter_npc_id = Config.maze_cfg_auto.other[1].room_npc_id
local transfer_npc_id = Config.maze_cfg_auto.other[1].npc_id

function MazeRunCtrl:__init()
	if MazeRunCtrl.Instance ~= nil then
		Debuger.LogError("[MazeRunCtrl] attempt to create singleton twice!")
		return
	end
	MazeRunCtrl.Instance = self
	self.data = MazeRunData.New()


	-- Debuger.LogError("enter_npc_id====" .. tostring(enter_npc_id))
	-- self:RegisterProtocol(CSKnapsackOperaReq)
	self:RegisterProtocol(SCActivityMazeRacerWaitsInfo, "RecvWaitsInfo")
	self:RegisterProtocol(SCActivityMazeRacerCopysInfo, "RecvCopysInfo")
	self:RegisterProtocol(SCActivityMazeRacerPassdInfo, "RecvPassdInfo")
	self:RegisterProtocol(SCActivityMazeRacerAmuletNum, "RecvRuneBag")
	self:RegisterProtocol(SCActivityMazeRacerRankLists, "RecvRankList")
	self:RegisterProtocol(SCActivityMazeRacerActInsert, "RecvRankActInsert")
	self:RegisterProtocol(SCActivityMazeRacerCurseInfo, "RecvCurseInfo")
	self:RegisterProtocol(SCActivityMazeRacerTransportPos, "RecvTransportPos")
	self:RegisterProtocol(SCActivityMazeRoleInfo, "RecvPassCount")
	self:RegisterProtocol(CSActivityMazeRacerHandleReq)

	ActivityData.Instance:CustomClickHandle(ActType.MzeRun,function()
			SceneLogic.Instance:AutoToNpc(enter_npc_id,nil,nil,true)
		end)

	BehaviorData.Instance:CustomNpcTalkBtnsId(enter_npc_id, BindTool.Bind(self.SetupBehaviourBtn, self))
	BehaviorData.Instance:CustomNpcTalkBtnsId(transfer_npc_id, BindTool.Bind(self.SetupBehaviourBtn, self))
	ActivityData.Instance:RegisterRewardGetFunc(ActType.MzeRun,BindTool.Bind(self.data.GetAllHelpInfo,self.data))
end


function MazeRunCtrl:OnUnloadGameLogic()
	self.data:Clear()
end
function MazeRunCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	-- local stu = ActivityCtrl.Instance:GetActivityStatus(ActType.MzeRun)
	local act_state = ActivityData.Instance:GetActivityStatus(ActType.MzeRun)
	if act_state == ActStatusType.Open or  ActStatusType.Standy then
		if obj_vo and obj_vo.id == enter_npc_id then
			local det = NpcDialogData.CreateDetailInfo1{tips = {Language.MazeRun.NpcDetails1, Language.MazeRun.NpcDetails2},
				rewards = self.data:GetDetailInfoRewards(), confirm_show = Language.Common.Enter}

			local button = NpcDialogBehBtnData.BehCfgCreate(
				Language.MazeRun.EnterBtn,
				BehaviorFactory.EnterFb(FB_TYPE.MAZE_RUN,
					function()
						if ActivityData.Instance:GetActivityStatus(ActType.MzeRun) == ActStatusType.Standy then
							PublicPopupCtrl.Instance:Center(Language.Activity.StandyTips)
							return false
						end
						local mem_count = 3
						TeamCtrl.Instance:TeamDetectByActType(ActType.MzeRun,function() end,mem_count)
						if not TeamData.Instance:InTeam() or TeamData.Instance:MemberNumRole() < mem_count then
							-- PublicPopupCtrl.Instance:DialogTips({
							-- 	content = Language.PublicPopup.ResTask,
							-- 	confirm = {
							-- 		func = function ()
										--是否组队
											-- local hint_str = string.format(Language.PublicPopup.TeamMemNotEnough,mem_count)
											-- PublicPopupCtrl.Instance:Center(hint_str)
											-- if TeamData.Instance:InTeam() then
											-- 	-- TeamCtrl.Instance:SendTeamRecruit()
											-- 	ViewMgr:OpenView(TeamView)
											-- else
											-- 	local team_target_cfg = Cfg.TeamTypeTarget(TeamConfig.TeamTargetType.Normal,0)
											-- 	TeamData.Instance.cur_platform_target_id = team_target_cfg.target_id
					                        --     ViewMgr:OpenView(TeamPlatformView)
											-- end
											-- PublicPopupCtrl.Instance:CloseDialogTips()
				                        -- end
								-- 	}
								-- })
							return false
						else
							return true
						end
					end),obj_vo,det)
			button:SetIconType(NpcIconType.Act)
			table.insert(btn_list, button)
			table.insert(btn_list,NpcDialogBehBtnData.BehCfgCreate(Language.MazeRun.RankList,
				BehaviorFactory.OpenView(MazeRunRankView),obj_vo))
		end
		if obj_vo and obj_vo.id == transfer_npc_id then
			table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(
				Language.MazeRun.Transfer,function()
				-- Debuger.LogErrorSD("self:RequestTransfer()====transfer_npc_id====")
					self:RequestTransfer()
				end))
		end
	end
end

function MazeRunCtrl:__delete()
	-- self:UnRegisterProtocol(SCKnapsackInfoAck)
	self.UnRegisterProtocol(SCActivityMazeRacerWaitsInfo)
	self.UnRegisterProtocol(SCActivityMazeRacerCopysInfo)
	self.UnRegisterProtocol(SCActivityMazeRacerPassdInfo)
	self:UnRegisterProtocol(SCActivityMazeRacerAmuletNum)
	self.UnRegisterProtocol(SCActivityMazeRacerRankLists)
	self.UnRegisterProtocol(SCActivityMazeRacerCurseInfo)


	if MazeRunCtrl.Instance == self then
		MazeRunCtrl.Instance = nil
	end
end

function MazeRunCtrl:ReqProc(op_type, p1)
	local proc = GetProtocol(CSActivityMazeRacerHandleReq)
	proc.op_type = op_type or 2
	proc.param1 = p1 or 0
	SendProtocol(proc)
end

function MazeRunCtrl:RequestTransfer()
	local proc = GetProtocol(CSActivityMazeRacerHandleReq)
    proc.op_type = MazeRunOperType.TRANSFER
    SendProtocol(proc)
	--Debuger.LogErrorSD("!!!!!!!!!!!!!!!")
end

function MazeRunCtrl:RequestUseRune(rune_id)
	--Debuger.LogError("?????ʹ????:%d",rune_id)
	local proc = GetProtocol(CSActivityMazeRacerHandleReq)
    proc.op_type = MazeRunOperType.AMULET
    proc.param1 = rune_id
    SendProtocol(proc)
end



function MazeRunCtrl:RecvWaitsInfo(protocol)
	local wait_data = self.data:WaitData()
	wait_data.begin_timestamp = protocol.next_begin_timestamp
	wait_data.wait_num = protocol.cur_waiting_role_num
	wait_data.min_lv = protocol.min_level
	wait_data.max_level = protocol.max_level
end

function MazeRunCtrl:RecvCopysInfo(protocol)
	--LogError("MazeRunCtrl:RecvCopysInfo",protocol)
	local running_data = self.data:RunningData()
	running_data.is_timing = true
	running_data.begin_timestamp = protocol.begin_timestamp
	running_data.rank_list = protocol.rank_list
	--下面两个要重改一下
	-- running_data.cur_floor = protocol.my_layer
	-- running_data.cur_rank = protocol.my_rank
end

function MazeRunCtrl:RecvPassdInfo(protocol)
	self.data:SetPassData(protocol)
	local running_data = self.data:RunningData()
	running_data.is_timing = false
	if protocol.award_list == nil or  next(protocol.award_list) == nil then
		SceneCtrl.Instance:RequestLeaveFb()
	else
		ViewMgr:OpenView(MazeRunRewardView)
	end
end

function MazeRunCtrl:RecvPassCount(protocol)
	self.data.pass_count.count = protocol.pass_count
end

function MazeRunCtrl:RecvRankList(protocol)
	self.data:SetRankData(protocol.maze_id,protocol.rank_list)
end

function MazeRunCtrl:RecvRankActInsert(protocol)
	self.data:AddRankData(protocol.maze_id,protocol.rank_info)
	-- for _,mem_info in pairs(protocol.rank_info.mem_list) do
	-- 	if mem_info.role_name == RoleData.Instance:GetBaseData().name then
	-- 		self.data:SetMyPassRank(protocol.rank_info.rank,protocol.rank_info.pass_time)
	-- 		return
	-- 	end
	-- end	
end

--受咒信息
function MazeRunCtrl:RecvCurseInfo(protocol)
	-- LogError("MazeRunCtrl:RecvCurseInfo===",protocol,"=self.objid===",RoleData.Instance:GetBaseData().obj_id)
	if protocol.cures_type == CommonItemId.Exp then --服务端的不知道啥数字 反正代表空
		self.data:SetRuneCurseInfo(protocol.obj_id, -1)
	else
		self.data:SetRuneCurseInfo(protocol.obj_id,protocol.cures_type)
	end
end

function MazeRunCtrl:RecvRuneBag(protocol)
	-- LogError("MazeRunCtrl:RecvRuneBag",protocol)
		self.data:SetRuneNum(protocol.amulet_num)
	-- for _,item_info in pairs(protocol.amulet_num) do
	-- 	self.data:SetRuneNum(item_info.item_id,item_info.item_num)
	-- end
end
--门的位置
function MazeRunCtrl:RecvTransportPos(protocol)
	self.data:DoorInfo(protocol)
	local running_data = self.data:RunningData()
	running_data.is_timing = true
	running_data.begin_timestamp = protocol.start_time
	running_data.cur_floor = protocol.layer

	local wait_data = self.data:WaitData()
	wait_data.begin_timestamp = protocol.start_time
end