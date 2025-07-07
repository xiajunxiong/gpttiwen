

VIEW_REQ("modules/answer/answer_reward_view")

AnswerCtrl = AnswerCtrl or BaseClass(BaseCtrl)

function AnswerCtrl:__init()
		if AnswerCtrl.Instance ~= nil then
		Debuger.LogError("[AnswerCtrl] attempt to create singleton twice!")
		return
	end	
	AnswerCtrl.Instance = self
	self:RegisterProtocol(SCAnswerOverInfo, "OnRecvAnswerOverInfo")
	self.delay_reward_timer = nil
end

function AnswerCtrl:__delete()
	if self.delay_reward_timer ~= nil then
		TimeHelper:CancelTimer(self.delay_reward_timer)
		self.delay_reward_timer = nil
	end
	AnswerCtrl.Instance = nil
	self:UnRegisterProtocol(SCAnswerOverInfo)
end

function AnswerCtrl:OnUnloadGameLogic()
	if self.delay_reward_timer ~= nil then
		TimeHelper:CancelTimer(self.delay_reward_timer)
		self.delay_reward_timer = nil
	end	
end

function AnswerCtrl:OnRecvAnswerOverInfo(protocol)
	AnswerData.Instance:SetRewardInfo(protocol)
end

function AnswerCtrl:OnOpenRewardInfo()
	local re_list = {}
	local reward_info = AnswerData.Instance:GetRewardInfo()
	if reward_info.add_coin > 0 then
		table.insert(re_list,Item.Create({item_id = CommonItemId.Coin,num = reward_info.add_coin}))
	end
	if reward_info.add_exp > 0 then
		table.insert(re_list,Item.Create({item_id = CommonItemId.Exp ,num = reward_info.add_exp}))
	end
	local reward_func = function ()	
		AnswerCtrl.Instance:OpenReward({
			hint = Language.Answer.TotalRewardNoBuy,
			items = re_list,
		})
		ViewMgr:CloseView(AnswerView)
	end
	if ViewMgr:IsOpen(AnswerView) then
		if self.delay_reward_timer ~= nil then
			TimeHelper:CancelTimer(self.delay_reward_timer)
		end
		self.delay_reward_timer = TimeHelper:AddDelayTimer(reward_func,2)
	end
end

--[[
param = {
	hint = 界面中间的提示文字
	btn1 = {
		text = 按钮文字
		func = 点击按钮执行的方法
		click_close = 点击是否自动关闭界面，默认为true
	},
	btn2 = {			
		text = 按钮文字
		func = 点击按钮执行的方法
		click_close = 点击是否自动关闭界面，默认为true
	},
	items = 奖励的物品表
	time_param = {
		time = 倒计时,
		func = 倒计时和结束回调
		desc = 倒计时的描述带%s
	}
}
]]
function AnswerCtrl:OpenReward(param)
	ViewMgr:OpenView(AnswerRewardView,param)
end