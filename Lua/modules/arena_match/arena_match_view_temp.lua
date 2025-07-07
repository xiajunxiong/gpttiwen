
ArenaMatchViewTemp = ArenaMatchViewTemp or DeclareView("ArenaMatchViewTemp", ArenaMatchConfig.ResPath .. "arena_match_temp")
VIEW_DECLARE_LEVEL(ArenaMatchViewTemp,ViewLevel.Ls)
VIEW_DECLARE_MASK(ArenaMatchViewTemp,ViewMask.BgBlockClose)

function ArenaMatchViewTemp:ArenaMatchViewTemp()
	self.data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewTemp:LoadCallback()
	local arena_list = self.data:GetArenaListByPos(self.data.cur_sel_pos.position)
	self.ArenaList:SetData(arena_list)
end

ArenaMatchViewTempItem = DeclareMono("ArenaMatchViewTempItem", UIWidgetBaseItem)

function ArenaMatchViewTempItem:ArenaMatchViewTempItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewTempItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data.leitai_name
end

function ArenaMatchViewTempItem:OnClickItem()
	self.arena_match_data.cur_sel_arena = self.data
	ArenaMatchCtrl.Instance:SendPlatformBattleRankInfo(self.data.seq)
	ViewMgr:CloseView(ArenaMatchViewTemp)
end