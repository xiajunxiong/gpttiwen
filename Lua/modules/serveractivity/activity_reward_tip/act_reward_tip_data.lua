ActRewardTipData = ActRewardTipData or BaseClass()

function ActRewardTipData:__init()
	if ActRewardTipData.Instance ~= nil then
		Debuger.LogError("[ActRewardTipData] attempt to create singleton twice!")
		return
	end
    ActRewardTipData.Instance = self
	self.get_reward_list = {}
	self.reward_list = {}
	self.result_data = {}
	self.is_open_view = false
end 

function ActRewardTipData:__delete()
    -- body
end

function ActRewardTipData:SetActivityRewardTipData(type,reward_list,extra_list) ---ServeractivityConfig.ActRewerdTip.RewardType
	self.reward_type = type
	self.extra_list = extra_list or {}
	self.is_success = type == ServeractivityConfig.ActRewerdTip.RewardType.Success or type == ServeractivityConfig.ActRewerdTip.RewardType.ChallengeSuccess
	if type == ServeractivityConfig.ActRewerdTip.RewardType.ChallengeSuccess then
		self.reward_list = DataHelper.ReverseList(reward_list)
	else
		self.reward_list = reward_list
	end
end
function ActRewardTipData:GetRewardData()
	return #self.reward_list == 0 and self.list or self.reward_list
end

function ActRewardTipData:ListAddIndexAttr(list)
	for i = 1,#list do
		list[i].index = i
	end
end
--外部调用  不经过服务器下发   奖励类型type  奖励reward_list  概率展示奖励extra_list 可为nil 
function ActRewardTipData:OpenActRewardTipViewByData(type,reward_list,extra_list) ---奖励类型ServeractivityConfig.ActRewerdTip.RewardType 1为默认通用
	self.list = {}
	self.reward_list = {}
	for i,v in ipairs(reward_list) do
		local tb = {}
		tb.num = v.num
		tb.is_bind = v.is_bind
		tb.item_id = v.item_id
		table.insert(self.list,tb)
	end
	self:ListAddIndexAttr(self.list)
	self:SetActivityRewardTipData(type,self.list,extra_list)
	ViewMgr:OpenView(ActRewardTipView)
end

function ActRewardTipData:OpenActRewardTipView()
	if #self.reward_list == 0 then 
		return
	end
	self:ListAddIndexAttr(self.reward_list)
    self:SetActivityRewardTipData(self.result_data.type,self.reward_list,self:GetExtraRewardList()) --完成结果  --得到奖励  --可能获得奖励（可为nil）
    ViewMgr:OpenView(ActRewardTipView)
end
--活动结果下发
function ActRewardTipData:SetResultNotice(protocol)
	self.result_data.activity_type = protocol.activity_type
	self.result_data.param1 = protocol.param1
	self.result_data.param2 = protocol.param2
	self.result_data.count = protocol.count
	self.reward_list = protocol.item_list
	self.result_data.type = self:GetResultType(protocol.result_type)
	self:OpenActRewardTipView()
end

function ActRewardTipData:GetResultType(type)-- 服务端定义type: 1战斗胜利 2战斗失败 3活动奖励
    if type == 3 then
		return ServeractivityConfig.ActRewerdTip.RewardType.Success
	end
	if #self:GetExtraRewardList() > 0 then
        return type == 1 and ServeractivityConfig.ActRewerdTip.RewardType.ChallengeSuccess or ServeractivityConfig.ActRewerdTip.RewardType.ChallengeFail
	end 
	return type == 1 and ServeractivityConfig.ActRewerdTip.RewardType.Success or ServeractivityConfig.ActRewerdTip.RewardType.Fail
end
--可能会获得以下奖励列表
function ActRewardTipData:GetExtraRewardList()
	--self.result_data.activity_type 
	--self.result_data.param1
	--self.result_data.param2
	local list = {}
	return list
end
--活动提示结束  关闭后跳转其他界面
function ActRewardTipData:FinishOpenView()
	if ActivityData.IsOpen(self.result_data.activity_type) then 
		if self.result_data.activity_type == ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE then
			ViewMgr:OpenViewByKey(Mod.SummerFirst.SummerExperience)
		end
		if self.result_data.activity_type == ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO then
			ViewMgr:OpenViewByKey(Mod.MountainSea.SummerExperienceTwo)
		end

		if self.result_data.activity_type == ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET then
			ServerActivityCtrl.Instance:SendShanHaiMeetResetPoker()
		end
	end
end