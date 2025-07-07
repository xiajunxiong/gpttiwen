FILE_REQ("modules/all_souls_storybook/all_souls_storybook_data")
VIEW_REQ("modules/all_souls_storybook/all_souls_storybook")
VIEW_REQ("modules/all_souls_storybook/all_souls_storybook_menu")
VIEW_REQ("modules/all_souls_storybook/all_souls_storybook_start")
VIEW_REQ("modules/all_souls_storybook/all_souls_storybook_reward")

AllSoulsStorybookCtrl = AllSoulsStorybookCtrl or BaseClass(BaseCtrl)
AllSoulsStorybookCtrl.ReqType = {
	TaskFinish = 0,
	FetchSet = 1,
	TaskFight = 2
}
--23 活跃类型
function AllSoulsStorybookCtrl:__init()
    if AllSoulsStorybookCtrl.Instance ~= nil then
		Debuger.LogError("[AllSoulsStorybookCtrl] attempt to create singleton twice!")
		return
	end	
	AllSoulsStorybookCtrl.Instance = self
	self.data = AllSoulsStorybookData.New()
	self:RegisterAllProtocol()

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_ALL_SOULS, function(is_win)
		if is_win == 1 then
			TimeHelper:AddDelayTimer(function ()
				if not self.data:GetIsFinish() then
					local cur_task = self.data:CurTask()
					if cur_task == nil then
						return
					end
					-- LogError("2 cur_task",cur_task, not StringIsEmpty(cur_task.mb_dec))
					if not StringIsEmpty(cur_task.mb_dec) then
						ViewMgr:OpenView(AllSoulsStorybookStart,{use_cur = true})
					else
						self.data:doTask(cur_task)
					end
				end
			end, 0.5)
		end
    end)

    ActivityData.Instance:CustomClickHandle(ActType.AllSoulsFB, function(source_view)
		if source_view == ActiveDailyView then
			GuideManager.Instance:Start(ConstGuideId.DailyActAllSoulsBook)
		end
		ViewMgr:OpenViewByKey(Mod.AllSoulsStorybook.Main)
	end)


end
function AllSoulsStorybookCtrl:RegisterAllProtocol()
	self:RegisterProtocol(CSWangLingFBReq)
	self:RegisterProtocol(SCWangLingFBRoleInfo, "SCWangLingFBRoleInfo")
	self:RegisterProtocol(SCWanLingFBInfo, "SCWanLingFBInfo")
end

function AllSoulsStorybookCtrl:__delete()
end

function AllSoulsStorybookCtrl:SCWangLingFBRoleInfo(protocol)
	-- LogError("wanling Role Info",protocol)
	if self.data:FBInfo().pass == protocol.day_fetch_pass_reward - 1 and protocol.day_fetch_pass_reward == 2 then
		self.just_change = true
	else
		self.just_change = false
	end
	self.data:FBInfo().fetch = protocol.fb_fetch_flag
	self.data:FBInfo().pass = protocol.day_fetch_pass_reward
	self.data:FBInfo().boss_fetch = protocol.boss_reward_times
end
function AllSoulsStorybookCtrl:SCWanLingFBInfo(protocol)
	if protocol.task_seq == 1 then
		self.data:ClearCacheRewards()
	end
	-- LogError("wanling FB Info",protocol)
	local last_is_battle = false
	if self.data:CurTask() and self.data:CurTask().task_type == 1 then
		last_is_battle = true
	end
	self.data:SetCurIsFinish(protocol)
	self.data:SceneInfo().index = protocol.fb_index
	self.data:SceneInfo().seq = protocol.task_seq
	self.data:CheckNpcs()
	if protocol.is_finish == 0 then
		local cur_task = self.data:CurTask()
		if cur_task == nil then
			return
		end
		-- LogError("1 cur_task",cur_task, not StringIsEmpty(cur_task.mb_dec),protocol.task_seq)
		if StringIsEmpty(cur_task.mb_dec) or last_is_battle then
			self.data:doTask(cur_task)
		else
			ViewMgr:OpenView(AllSoulsStorybookStart,{use_cur = true})
		end
	elseif not self.just_change and self.data:FBInfo().pass >= self.data:RewardTime() or not self.data:IsFetch(protocol.fb_index) then
		TimeHelper:AddDelayTimer(function ()
			self.just_change = false
			self.data:doTask(self.data:CurTask())
		end, 1)
	end

end
function AllSoulsStorybookCtrl.CSWangLingFBReq(req_type, param1, param2)
	local protocol = GetProtocol(CSWangLingFBReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end
function AllSoulsStorybookCtrl.TaskFinish()
	AllSoulsStorybookCtrl.CSWangLingFBReq(AllSoulsStorybookCtrl.ReqType.TaskFinish)
end
function AllSoulsStorybookCtrl.FetchSet(param1, param2)
	AllSoulsStorybookCtrl.CSWangLingFBReq(AllSoulsStorybookCtrl.ReqType.FetchSet, param1, param2)
end
function AllSoulsStorybookCtrl.TaskFight()
	AllSoulsStorybookCtrl.CSWangLingFBReq(AllSoulsStorybookCtrl.ReqType.TaskFight)
end

--rewawrd_type = AllSoulsRewardType
--Sample时（list是item的列表）
--Mult时（list下列结构体列表）
-- { 
-- 	title_text = 标题文本（可传空，空则不显示标题）,
-- 	list = item的列表,
-- }
--title_type奖励界面标题文字 传空时默认与reward_type一致（传参对应枚举 AllSoulsRewardTitleType）
--hide_fly_text界面是否飘获得物品文字 true为隐藏 false为飘字
--huobi_desc = {
--	type = 货币类型 不传不显示
--  num = 数量或者描述
--}
function AllSoulsStorybookCtrl:OpenRewardView(list, reward_type, title_type, hide_fly_text, huobi_desc)
	self.data:SetRewardList(list, reward_type, title_type, hide_fly_text, huobi_desc)
	ViewMgr:OpenView(AllSoulsStorybookReward)
end