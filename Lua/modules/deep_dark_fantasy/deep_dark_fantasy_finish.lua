DeepDarkFantasyFinishView = DeepDarkFantasyFinishView or DeclareView("DeepDarkFantasyFinishView", "deep_dark_fantasy/deep_dark_fantasy_finish")
VIEW_DECLARE_MASK(DeepDarkFantasyFinishView,ViewMask.BgBlockClose)

function DeepDarkFantasyFinishView:DeepDarkFantasyFinishView()
	self.data = DeepDarkFantasyData.Instance
end

function DeepDarkFantasyFinishView:LoadCallback(data)
	self.EffectTool:Play(10010204)
	self:FlushMemberInfo()
	self:FlushTimeView()
end

--刷新倒计时
function DeepDarkFantasyFinishView:FlushTimeView()
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(DeepDarkFantasyFinishView)
	end)
	self.ConfirmTime:TotalTime(10,TimeType.Type_Special_2,Language.Finish.Confirm)
end

function DeepDarkFantasyFinishView:OnClickConfirm()
	ViewMgr:CloseView(DeepDarkFantasyFinishView)
end

function DeepDarkFantasyFinishView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	TimeHelper:CancelTimer(self.ShowMemberTimer)
	TimeHelper:CancelTimer(self.ConditionTimer)
	self.ConfirmTime:CloseCountDownTime()
	-- FinishCtrl.Instance:CloseFinishView()

	self.data:OutInitView()
	ViewMgr:OpenView(DeepDarkFantasyView)
end

function DeepDarkFantasyFinishView:FlushMemberInfo()
	local member_data = BattleData.Instance:GetBattleStatisticResult()
	if member_data.stat_list ~= nil then
		for i, v in pairs(member_data.stat_list) do
			v.seq = i
		end
		self.member_list = member_data.stat_list
		self.MemberList:SetData(member_data.stat_list)
		self.ShowMemberTimer = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushMemberView, self), 0.7)
	end
end

--加动态效果
function DeepDarkFantasyFinishView:FlushMemberView()
	if self.member_count == nil then
		self.member_count = 0
	end
	if self.member_count < #self.member_list then
		local item = self.MemberList.item_list[self.member_count + 1]
		if item ~= nil then
			item:SetShow()
		end
		self.member_count = self.member_count + 1
		self.ShowMemberTimer = TimeHelper:AddDelayTimer(BindTool.Bind(self.FlushMemberView, self), 0.2)
	end
end

--===============================MaterialChallengeCondition===============================
DeepDarkFantasyFinishCondition = DeepDarkFantasyFinishCondition or DeclareMono("DeepDarkFantasyFinishCondition", UIWFlushPanel)
function DeepDarkFantasyFinishCondition:DeepDarkFantasyFinishCondition()
	self.data = DeepDarkFantasyData.Instance
	self.data_cares = {
        {data = self.data.finish_data, func = self.onFlush },
    }
end 

function DeepDarkFantasyFinishCondition:onFlush()
	local finish_data = self.data.finish_data
	local challenge = self.data:GetBossInfoByChallengeId(finish_data.challenge_id)
	for i = 1,3 do 
		self.Score_list[i].gameObject:SetActive(challenge.reward_list[i] ~= nil)
		if challenge.reward_list[i] ~= nil then 
		    local vo = {}
		    vo.timer = i
		    vo.condition = challenge.reward_list[i].condition
		    vo.is_success = finish_data.flag_list[i]
		    self.Score_list[i]:SetData(vo)
		end 
	end 
	if finish_data.with_new then 
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
	end 
end

DarkFantasyFinishBoxCell = DarkFantasyFinishBoxCell or DeclareMono("DarkFantasyFinishBoxCell", UIWidgetBaseItem)
function DarkFantasyFinishBoxCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local info = Config.score_condition_auto.condition[data.condition]
    UH.SetText(self.desc, info.desc)
	
	self.inter.Interactable = data.is_success == 1

	if self.data.is_success == 1 then
		self.ShowTimer = TimeHelper:AddDelayTimer(function()
			self.effect:Play("3165005")
	    end, 0.7+(self.data.timer/10))
	end 

    local list = {}
	if info.condition_type ==5 and self.partner_list ~= nil then 
		local vo_1 = {icon = info.param_1,type = 1}
        table.insert(list,vo_1)
	elseif info.condition_type == 12 and self.partner_list ~= nil then 
        local vo_2 = {icon = info.param_1,type = 2}
        table.insert(list,vo_2)
	end 

    if self.partner_list ~= nil then 
        self.partner_list.gameObject:SetActive(#list > 0 )
        if #list > 0 then self.partner_list:SetData(list) end 
    end 
end

function DarkFantasyFinishBoxCell:OnDestroy()
	if self.ShowTimer ~= nil then 
		TimeHelper:CancelTimer(self.ShowTimer)
		self.ShowTimer = nil
	end 
end