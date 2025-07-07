MaterialChallengeFinishView = MaterialChallengeFinishView or DeclareView("MaterialChallengeFinishView", "material/material_challenge_finish")

VIEW_DECLARE_MASK(MaterialChallengeFinishView,ViewMask.BgBlockClose)

function MaterialChallengeFinishView:MaterialChallengeFinishView()
	self.data = MaterialData.Instance
end

function MaterialChallengeFinishView:LoadCallback(data)
	local info = self.data:GetChapterInfo(data)
	if info ~= nil then
		self.condition_index = 0
		self.flag_list = info.flag_list or {}
		self.Condition:SetEffect(info.flag_num or 0)
		self.Condition:SetData({desc = self.data:GetConditionList(data)})
		self.ConditionTimer = Invoke(function()
			self:FlushCondition()
		end,0.5)
	end
	self.cell_list = self.data:GetFinishRewardData(data)
	local list = FinishData.Instance:GetEleRewards()
	FinishData.Instance:SetEleRewards(nil)
	if list then
		for i, v in ipairs(list) do
			table.insert(self.cell_list, v)
		end
	end
	self.Title:SetActive(self.cell_list[1] and self.cell_list[1].first_kill == true)
	self.Rewards:SetActive(#self.cell_list ~= 0)
	self.ExpItem:SetData({exp_num = MaterialData.Instance:GetExpNum()})
	self:FlushCallListView()
	self:FlushMemberInfo()
	self:FlushTimeView()
end

function MaterialChallengeFinishView:FlushCondition()
	if self.condition_index >= 3 then
		return
	end
	local condition_time = 0.5
	self.condition_index = self.condition_index + 1
	local flag = self.flag_list[self.condition_index] or 0
	if flag == 1 and self.data:GetOldInfoFlag(self.condition_index) == 0 then
		self.Condition:ShowEffect(self.condition_index,true)
	else
		condition_time = 0
	end
	self.ConditionTimer = Invoke(function()
		self:FlushCondition()
	end,condition_time)
end

--刷新倒计时
function MaterialChallengeFinishView:FlushTimeView()
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(MaterialChallengeFinishView)
	end)
	self.ConfirmTime:TotalTime(10,TimeType.Type_Special_2,Language.Finish.Confirm)
end

function MaterialChallengeFinishView:OnClickConfirm()
	if self.condition_index < 3 then
		return
	end
	ViewMgr:CloseView(MaterialChallengeFinishView)
end

--物品动画逻辑
function MaterialChallengeFinishView:FlushCallListView()
	if #self.cell_list == 0 then return end
	self.GridList:SetData(self.cell_list)
	self.ShowTimer = TimeHelper:AddDelayTimer(function()
		self:FlushShowView()
	end, 0.7)
end

function MaterialChallengeFinishView:FlushShowView()
	if self.count == nil then
		self.count = 0
	end
	if self.count < #self.cell_list then
		self:SetItem(self.GridList.item_list[self.count + 1])
		self.count = self.count + 1
		self.ShowItemTime = TimeHelper:AddDelayTimer(function()
			self:FlushShowView()
        end, 0.2)
	end
end

function MaterialChallengeFinishView:SetItem(item)
	if item then
		item:SetShow()
	end
end

function MaterialChallengeFinishView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.ShowItemTime)
	TimeHelper:CancelTimer(self.ShowMemberTimer)
	TimeHelper:CancelTimer(self.ConditionTimer)
	self.ConfirmTime:CloseCountDownTime()
	FinishCtrl.Instance:CloseFinishView()
end

function MaterialChallengeFinishView:FlushMemberInfo()
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
function MaterialChallengeFinishView:FlushMemberView()
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
MaterialChallengeCondition = DeclareMono("MaterialChallengeCondition", UIWidgetBaseItem)

function MaterialChallengeCondition:SetData(data)
	for i=1,self.DescList:Length() do
		local flag = MaterialData.Instance:GetOldInfoFlag(i)
		UH.SetText(self.DescList[i],data.desc[i])
		self.GrayList[i].enabled = flag == 0
		self.GrayListImg[i].enabled = flag == 0
	end
end

function MaterialChallengeCondition:SetEffect(index)
	self.Effect:Play(index == 3 and 10010835 or 10010204)
end

function MaterialChallengeCondition:ShowEffect(index,flag)
	self.EffectList[index]:SetActive(flag)
	self.GrayList[index].enabled = not flag
	self.GrayListImg[index].enabled = not flag
end