FILE_REQ("modules/activity/activity_config")
FILE_REQ("modules/activity/activity_info")
FILE_REQ("modules/activity/activity_data")
FILE_REQ("modules/activity/activity_data_acts")
FILE_REQ("modules/activity/activity_rand_data")
VIEW_REQ("modules/activity/board_act")
VIEW_REQ("modules/activity/athletics_view")
VIEW_REQ("modules/activity/activity_week_view")
VIEW_REQ("modules/activity/activity_push_view")
VIEW_REQ("modules/activity/activity_view_challenge")
VIEW_REQ("modules/activity/activity_view_time_limit_reward_preview")
VIEW_REQ("modules/activity/activity_reward_btn_item")
VIEW_REQ("modules/activity/activity_contest_notice")
ActivityCtrl = ActivityCtrl or BaseClass(BaseCtrl)

function ActivityCtrl:__init()
	if ActivityCtrl.Instance ~= nil then
		Debuger.LogError("[ActivityCtrl] attempt to create singleton twice!")
		return
	end	
	ActivityCtrl.Instance = self
	self.data = ActivityData.New()
	ActivityRandData.New()
	self:RegisterAllProtocols()
	RemindCtrl.Instance:Remind():Register(Mod.Activity.Daily, self.data.daily_red_point, function ()
		return self.data.daily_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Athletics.Main, self.data.challenge_red_point, function ()
		return self.data.challenge_red_point.val and 1 or 0
	end)
	BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data.acts_npc_seqs.trevi_fountain, BindTool.Bind(self.SetupBehaviourBtn, self))

	ActivityRandData.Instance:Register(Mod.ActivityRemind.ChiefElection,function()
		if self.data:IsNoEnoughLevel(ActType.ChiefElection,true) then
			return false
		end
		if self.data:IsNotRemindRange(ActType.ChiefElection,2) then
			return false
		end
		if self.data:IsOpenEnd(ActType.ChiefElection) then
			return false
		end
		return self.data:GetActivityStatus(ActType.ChiefElection) == ActStatusType.Close
	end,self.data:GetActivityEvent())
	ActivityRandData.Instance:Register(Mod.ActivityRemind.GuildMelee,function()
		if self.data:IsNoEnoughLevel(ActType.GuildMelee,true) then
			return false
		end
		if self.data:IsNotRemindRange(ActType.GuildMelee,2) then
			return false
		end
		if self.data:IsOpenEnd(ActType.GuildMelee) then
			return false
		end
		if self.data:IsRangeEnd(ActType.GuildMelee,ActType.GuildEmbattle) then
			return false
		end
		return self.data:GetActivityStatus(ActType.GuildMelee) == ActStatusType.Close
	end,self.data:GetActivityEvent())
	ActivityRandData.Instance:Register(Mod.ActivityRemind.GuildEmbattle,function()
		if self.data:IsNoEnoughLevel(ActType.GuildEmbattle,true) then
			return false
		end
		if self.data:IsRangeEnd(ActType.GuildMelee,ActType.GuildEmbattle) then
			return self.data:GetActivityStatus(ActType.GuildEmbattle) == ActStatusType.Close
		end
		return false
	end,self.data:GetActivityEvent())
end

function ActivityCtrl:__delete()
	if ActivityCtrl.Instance == self then
		ActivityCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end
function ActivityCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function ActivityCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSTreviFountainOp)
	self:RegisterProtocol(CSActiveSystemReq)
	self:RegisterProtocol(CSActivitySignUpReq)
	self:RegisterProtocol(CSActivityAdvanceFetchReq)
	self:RegisterProtocol(SCActivityStatus, "OnActivityStatus")
	self:RegisterProtocol(SCTreviFountainInfo,"OnTreviFountainInfo")
	self:RegisterProtocol(SCActiveSystemInfo, "OnActiveSystemInfo")
	self:RegisterProtocol(SCActiveSystemSingleInfo, "OnActiveSystemSingleInfo")
	self:RegisterProtocol(SCActivitySignUpInfo, "OnActivitySignUpInfo")
	self:RegisterProtocol(SCActivityEnterInfo, "OnActivityEnterInfo")
	self:RegisterProtocol(SCActivityAdvanceFetchInfo, "OnActivityAdvanceFetchInfo")
	self:RegisterProtocol(SCSpecialActiveInfo, "OnSpecialActiveInfo")
	self:RegisterProtocol(SCNoticeClientRestart, "OnActivityConfigReloadNotice")
end

function ActivityCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCNoticeClientRestart)
	self:UnRegisterProtocol(CSTreviFountainOp)
	self:UnRegisterProtocol(CSTreviFountainOp)
	self:UnRegisterProtocol(CSActivitySignUpReq)
	self:UnRegisterProtocol(CSActivityAdvanceFetchReq)
	self:UnRegisterProtocol(SCActivityStatus)
	self:UnRegisterProtocol(SCActiveSystemInfo)
	self:UnRegisterProtocol(SCActiveSystemSingleInfo)
	self:UnRegisterProtocol(SCActivitySignUpInfo)
	self:UnRegisterProtocol(SCActivityEnterInfo)
	self:UnRegisterProtocol(SCActivityAdvanceFetchInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Activity.Daily)
end

function ActivityCtrl:SendTreviFountainOp()
	local protocol = GetProtocol(CSTreviFountainOp)
	SendProtocol(protocol)
end

function ActivityCtrl:OnActivityStatus(protocol)
	if LoginData.Instance:BaseData().is_on_crosssever then --跨服屏蔽拍卖行
		if protocol.act_status_info.act_type == ACTIVITY_TYPE.RAND_PAI_MAI_HANG then
			protocol.act_status_info.status = ActStatusType.Close
		end
	end
	self.data:SetActivityStatus(protocol)
end

function ActivityCtrl:OnTreviFountainInfo(protocol)
	self.data:SetTreviFountainInfo(protocol)
	if self.data.trevi_fountain_switch then
		self.data.trevi_fountain_switch = false
		self.data:GetActivityInfo(ActType.TreviFountain):DoClickHandle()
	end
end

function ActivityCtrl:GetActivityBackRewardListWish()
	return self.data:GetActivityBackRewardListWish()
end

function ActivityCtrl:GetWSTRewardList()
	return self.data:GetWSTRewardList()
end

function ActivityCtrl:GetSilkRoadBackRewardList()
	return self.data:GetSilkRoadBackRewardList()
end

function ActivityCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if nil == obj_vo then return end
	local acts_npc_seqs = self.data.acts_npc_seqs
	if obj_vo.seq == acts_npc_seqs.trevi_fountain and FunOpen.Instance:GetFunIsOpened(Mod.Acts.trevi_fountain) then
		local cur_num = self.data.act_wish_info.use_count or 0
		local dis_str = string.format(Language.Activity.Acts.TreviFountain.ndv_dis_str, cur_num, self.data.act_wish_max_num)
		local button = NpcDialogBehBtnData.FuncCreate(dis_str, function ()
			self:SendTreviFountainOp()
			if self.data.act_wish_max_num - cur_num > 0 then
				self.data.trevi_fountain_switch = true
			end
		end,
		true, NpcDialogData.CreateDetailInfo1{tips = Language.Activity.Acts.TreviFountain.DetailTips, rewards = self.data:GetDetailInfoWish()})
		button:SetIconType(NpcIconType.Act)
		table.insert(btn_list, button)
	end
end

--活跃系统
-- op = 0 请求信息
-- op = 1 请求领取活跃度宝箱奖励 index
-- op = 2 周常领取奖励 seq
-- op = 3 领取特殊活跃奖励 index 填 active_special_type
-- op = 4 请求特殊活跃信息
function ActivityCtrl:SendActiveSystemReq(op_type,index)
	local protocol = GetProtocol(CSActiveSystemReq)
	protocol.op_type = op_type or 0
	protocol.index = index or 0
	SendProtocol(protocol)
end

function ActivityCtrl:OnActiveSystemInfo(protocol)
	self.data:SetActiveSystemInfo(protocol)
end

function ActivityCtrl:OnSpecialActiveInfo(protocol)
	self.data:SetSpecialActiveInfo(protocol)
end

function ActivityCtrl:OnActiveSystemSingleInfo(protocol)

	local old_activity_flag = self.data:GetActiveFlag(protocol.type)
	local one_done = old_activity_flag and old_activity_flag ~= protocol.flag and protocol.flag == 1
	self.data:SetActiveSystemSingleInfo(protocol)
	--完成了某个日常后检查是否还有其他未完成的日常任务，如果有弹出提示
	if one_done then
		local config = self.data:GetActiveDataByType(protocol.type)
		if config == nil or config.recom_order < 0 then
			return
		end
		if protocol.type == ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG then
			MainOtherCtrl.Instance:SetWaitViewFunc(function()self:CheckAndOpenNextNotice()end)
		elseif protocol.type ~= ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_ESCORT then
			self:CheckAndOpenNextNotice()
		end
	end
end
--完成某个日常任务（不一定是日常完成的条件）后，检查弹出是否还有其他的日常任务没有做
function ActivityCtrl:CheckAndOpenNextNotice()
	local server_time_t = TimeCtrl.Instance:GetServerDate()
	if UnityPlayerPrefs.HasKey(PrefKeys.ResearchDailyDegree(RoleData.Instance:GetRoleId())) then
		local pref_value = UnityPlayerPrefs.GetString(PrefKeys.ResearchDailyDegree(RoleData.Instance:GetRoleId()))
		if pref_value == server_time_t.year..server_time_t.month..server_time_t.day then
			return
		end
	end
	local activity_info = self.data:GetTopNextUncompleteDegree()
	if activity_info ~= nil then
		local param_t = {
			talk_list = {
				[1] = {
					talker = Config.activedegree_auto.other[1].NPCid or 0, 
					talk_text = string.format(Language.Activity.ActiveDegreeNpcTipText, activity_info.act_name),
				},
			},
			beh_btn_list = {
				[2] = NpcDialogBehBtnDataFuncExe.New(Language.Activity.ActiveDegreeNpcTipBtn[2],function ()
					UnityPlayerPrefs.SetString(PrefKeys.ResearchDailyDegree(RoleData.Instance:GetRoleId()), server_time_t.year..server_time_t.month..server_time_t.day)
				end,true)
			},
			not_block_close = true
		}
		if activity_info and activity_info.click_handle then
			param_t.beh_btn_list[1] = NpcDialogBehBtnDataFuncExe.New(Language.Activity.ActiveDegreeNpcTipBtn[1], activity_info.click_handle, true)
		else
			param_t.beh_btn_list[1] = NpcDialogBehBtnDataFuncExe.New(Language.Activity.ActiveDegreeNpcTipBtn[1], nil, true)
		end

		NpcDialogCtrl.Instance:NpcDialogView(param_t, nil, true)
	end
end

--活动报名
function ActivityCtrl:SendActivitySignUpReq(seq)
	local protocol = GetProtocol(CSActivitySignUpReq)
	protocol.seq = seq or 0
	SendProtocol(protocol)
end

function ActivityCtrl:OnActivitySignUpInfo(protocol)
	self.data:SetActivitySignUpInfo(protocol)
end

function ActivityCtrl:OnActivityEnterInfo(protocol)
	self.data:SetActivityEnterInfo(protocol)
	local enter_info = protocol.enter_info
	if enter_info.enter_end_timestamp ~= 0 then
		local act_info = ActivityData.Instance:GetActivityInfo(enter_info.act_type)
		if act_info ~= nil then
			PublicPopupCtrl.Instance:ActTips(act_info,enter_info.enter_end_timestamp)
		else
			LogError(Format("活动类型=%s不存在,请检查配置",enter_info.act_type))
		end
	end
end

function ActivityCtrl:IsEndOtherTip(act_type)
	local enter_info = ActivityData.Instance:GetActivityEnterInfo(act_type)
	local enter_act_type = enter_info.act_type or 0
    if enter_act_type ~= 0 then
        return enter_info.enter_end_timestamp ~= 0
    end
    return false
end

function ActivityCtrl:OnActivityAdvanceFetchInfo(protocol)
	if protocol.activity_type == ActType.OnlyFight then
		OnlyFightCtrl.Instance:RecvGetReward(protocol)
	elseif protocol.activity_type == ActType.GuildMelee then
		GuildCtrl.Instance:RecvGetMeleeReward(protocol)
	elseif protocol.activity_type == ActType.GuildAnswer then
		GuildCtrl.Instance:RecvGetAnswerReward(protocol)
	elseif protocol.activity_type == ActType.TeamSport then
		TeamSportCtrl.Instance:RecvGetReward(protocol)
	end
	self.data:SetTimeLimitRewardGetFlag(protocol)
end

--op_type 0:参与奖励，1:存活奖励（p1 存活任务）
function ActivityCtrl:SendActivityAdvanceFetchReq(param)
	local protocol = GetProtocol(CSActivityAdvanceFetchReq)
	protocol.activity_type = param.activity_type or 0
	protocol.op_type = param.op_type or -1
	protocol.param1 = param.param1 or 0
	SendProtocol(protocol)
end

-- reward_list示例
-- reward_list = {
-- 	[1] = {
-- 		toggle_name = "",界面左侧标签页名称
--		reward_item_list = {  --这个list是在界面上每一个奖励项的数据，包括具体奖励的item列表、回调方法、title文字等
-- 			[1] = {

	-- 			itemlist和child_list二选一

	--			item_list = {[1] = item实例...},
	-- 			child_list = {
	-- 				[1] = {
	-- 					child_title = "",
	-- 					item_list = {[1] = item实例...},
	-- 					click_func = 
	-- 					func_param = 
	-- 				},
	-- 			}
	-- 			title_type = 一级标题样式（空或1为默认，2为排行奖励使用类型）
	-- 			title_name = ""
	-- 			使用了child_list的以下内容酌情可不写
	--      	btn_flag = (number类型，1为按钮已领取（置灰）状态, 为空时不显示按钮)，
	--      	click_func = (如果有点击领取按钮需要通过此接口进行回调，为空时不显示按钮),
	--      	func_param = (点击回调参数如果有则会回传，可为空),
	--			tip_str = (提示文字 例:"(活动结束后邮件发放)"，可为空), 
	--			一般来讲点击回调和提示文字最多有一个即要么显示按钮，要么显示提示文字或者都不显示
	--		}
	--		...
-- 		},
-- 	},
-- [2] = {
--   ...
-- 	}
-- }
function ActivityCtrl:OpenTimeLimitReward(reward_list,act_type)
	self:FlushTimeLimitReward(reward_list,act_type)
end

function ActivityCtrl:FlushTimeLimitReward(reward_list,act_type)
	if reward_list ~= nil and next(reward_list) ~= nil then
		ActivityData.Instance:SetTimeLimitRewardList(reward_list)
		if ViewMgr:IsOpen(TimeLimitRewardPreview) then
			ActivityData.Instance.time_limit_reward_list_index:SetDirty("index")
		else
			ViewMgr:OpenView(TimeLimitRewardPreview,{act_type = act_type})
		end
	end
end

function ActivityCtrl:OnActivityConfigReloadNotice()
	if LoginData.Instance:AssetVersionUnmatch() then return end
	if not SIMULATE_ASSETBUNDLE and not IS_AUDIT_VERSION then
		local url = GameStateInit.GetQueryURL(true)
		WebRequester:Request(url,function(data,error)
			if not StringIsEmpty(error) then
				return
			end
			if data ~= GLOBAL_CONFIG:PkgInfo().assets_version then
				LoginCtrl.Instance:unloadGameLogicBeforeRelogin();
				LoginData.Instance:MarkAssetVersionUnmatch()
				LoginCtrl.Instance:HintToReturnLogin(Language.Login.NewAssetVersion)
			end
		end)
	end
end