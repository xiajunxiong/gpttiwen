CombatView = CombatView or DeclareView("CombatView","combat/combat_view")
----Board=========UIWBoard
----TabWild======GameObject
----TabDungeon===GameObject
----TabGather===GameObject
CombatView.vct_pos = {
	[1] = Vector2.New(768,378),
	[2] = Vector2.New(760,223),
}

function CombatView:CombatView()
end

function CombatView:LoadCallback()
	self.Board:SetData(self:GetType(),Language.Combat.Title)
	self.Board:SetTabbarVisible(true)
	local tabbar = self.Board.Tabbar
	tabbar:SetData({
			{name = Language.Combat.Tab1,tab_objs = {self.TabWild}},
            {name = Language.Combat.Tab2,tab_objs = {self.TabDungeon}},
            {name = Language.Combat.Tab3,tab_objs = {self.TabGather}},
			})
	tabbar:ClickItem(1)
	self.Currency:DefaultSet()
end

function CombatView:Flush(data)
	if self.Bottom1 then
		self.Bottom1.sizeDelta = CombatView.vct_pos[1]
	end

	if self.Bottom2 then
		self.Bottom2.sizeDelta = CombatView.vct_pos[2]
	end
end

-- 战斗公用界面
CombatPublicFlushView = CombatPublicFlushView or DeclareMono("CombatPublicFlushView",UIWFlushPanel)

function CombatPublicFlushView:CombatPublicFlushView()
	self.combat_view_data = CombatData.Instance:GetCombatViewData()
	self.data_cares = {
		{data = self.combat_view_data,keys = {"select_scene_type"}, func = self.FlushLeftList,init_call = false},
		{data = self.combat_view_data,keys = {"select_scene_data"}, func = self.FlushView,init_call = false},
	}
end

function CombatPublicFlushView:FlushLeftList()
	local scene_type = self.combat_view_data.select_scene_type
	local data_list = CombatData.Instance:GetSceneFightListData(scene_type)
	self.SceneList:SetData(data_list)
	self.SceneList:ClickItem(1)
end

function CombatPublicFlushView:FlushView()
	self.SceneName.text = self.combat_view_data.select_scene_data.scene_name
	local reward_data = CombatData.Instance:GetShowRewardList()
	for i=1,3 do
		if self["Tab"..i].FlushRewardList then
			self["Tab"..i]:FlushRewardList(reward_data)
		end
	end
end

function CombatPublicFlushView:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.combat_view_data.select_scene_type = -1
end

function CombatPublicFlushView:SetBottomSize(vct1,vct2)
	if self.Bottom1 then
		self.Bottom1.sizeDelta = vct1
	end

	if self.Bottom2 then
		self.Bottom2.sizeDelta = vct2
	end
end

-- 战斗左边列表Item
CombatLeftItem = CombatLeftItem or DeclareMono("CombatLeftItem",UIWidgetBaseItem)

function CombatLeftItem:CombatLeftItem()
	self.combat_view_data = CombatData.Instance:GetCombatViewData()
end

function CombatLeftItem:SetData(data)
	self.data = data
	self.SceneName.text = self.data.scene_name
end

function CombatLeftItem:OnClick()
	self.combat_view_data.select_scene_data = self.data
end

function CombatLeftItem:Click()
	self:SetSelected(not self.is_selected)
	self:OnClick()
end