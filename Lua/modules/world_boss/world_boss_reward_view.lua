
WorldBossRewardView = WorldBossRewardView or DeclareView("WorldBossRewardView", WorldBossConfig.ResPath .. "world_boss_reward")

function WorldBossRewardView:WorldBossRewardView()
	self.data = WorldBossCtrl.Instance:Data()
	self.language = Language.WorldBoss
end

function WorldBossRewardView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.WorldBossRewardPreview, Vector2.New(698, 473))
	
	if self.data.result_act_type == nil then 
		self.data.result_act_type = self.data:ActType()
	end 
	-- LogError("?",self.data.result_act_type)
	local reward_list = self.data:GetRewardList()
	self.RewardList:SetData(reward_list)
end

function WorldBossRewardView:OnClickClose()
	ViewMgr:CloseView(WorldBossView)
end

WorldBossRewardViewItem = DeclareMono("WorldBossRewardViewItem", UIWidgetBaseItem)

function WorldBossRewardViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.NameSp.SpriteName = data.sp_name
	self.RewardGrid:SetData(data.rewards)
end