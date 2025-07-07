
WorldBossView = WorldBossView or DeclareView("WorldBossView", WorldBossConfig.ResPath .. "world_boss")

function WorldBossView:WorldBossView()
	self.data = WorldBossCtrl.Instance:Data()
	self.language = Language.WorldBoss
end

function WorldBossView:LoadCallback()
	local rank_type_list = self.data:GetRankTypeList()
	self.Title.text = self.language.TitleNames[self.data.result_act_type]
	self.RankList:SetData(rank_type_list)
end

function WorldBossView:OnClickClose()
	ViewMgr:CloseView(WorldBossView)
end

function WorldBossView:OnClickReward()
	ViewMgr:OpenView(WorldBossRewardView)
end

function WorldBossView:OnClickConfirm()
	ViewMgr:CloseView(WorldBossView)
end

WorldBossViewRankItem = DeclareMono("WorldBossViewRankItem", UIWidgetBaseItem)

function WorldBossViewRankItem:WorldBossViewRankItem()
	self.wb_data = WorldBossCtrl.Instance:Data()
end

function WorldBossViewRankItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Title.text = data.title
	local rank_list = self.wb_data:GetRankListWithType(data.type)
	self.EmptyObj:SetActive(0 == #rank_list)
	self.NameGrid:SetData(rank_list)
end

WorldBossViewRankNameItem = DeclareMono("WorldBossViewRankNameItem", UIWidgetBaseItem)

function WorldBossViewRankNameItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data.is_self and RichTextHelper.ColorStr(data.name, COLORSTR.Yellow20) or data.name
end