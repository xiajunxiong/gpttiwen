
FinishPassViewAM = FinishPassViewAM or DeclareView("FinishPassViewAM", "finish/finish_pass_am")

function FinishPassViewAM:OnClickClose()
	ViewMgr:CloseView(FinishPassViewAM)
end

FinishPassViewAMDetail = FinishPassViewAMDetail or DeclareMono("FinishPassViewAMDetail", UIWFlushPanel)

function FinishPassViewAMDetail:FinishPassViewAMDetail()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch

	self.data_cares = {
		{data = self.data.rank_info_list, func = self.FlushRankInfo},
	}
end

function FinishPassViewAMDetail:FlushRankInfo()
	self.SuccObj:SetActive(self.data.finish_is_win)
	self.FailObj:SetActive(not self.data.finish_is_win)

	local rank_info_list = self.data:RankInfoList()
	local role_info = self.data.role_info_list[self.data.cur_sel_arena.seq]
	self.RankList:SetData(rank_info_list)
	-- self.RctObj:SetActive(nil ~= role_info)
	self.FailDesc:SetActive(nil == role_info)
	self.RankCurVal:SetObjActive(nil ~= role_info)
	self.RankCurVal.text = role_info and string.format(self.language.Rank.RankCur, role_info:Rank()) or ""
end

FinishPassViewAMItem = DeclareMono("FinishPassViewAMItem", UIWidgetBaseItem)

function FinishPassViewAMItem:FinishPassViewAMItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Rank
end

function FinishPassViewAMItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local rank = data:Rank()
	self.ItemSp.SpriteName = "Wei_" .. (rank <= 3 and rank or "p")
	self.RankSp.SpriteName = data:RankSp()
	self.Rank.text = rank > 3 and rank or ""
	self.Name.text = data:Name()
	if data.role_info.top_level and data.role_info.top_level > 0 then
        UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, data.role_info.top_level))
	else
		self.Level.text = "Lv." .. data:Level()
	end
	
	self.Cap.text = data:PetsCap() + data:PartnersCap()
	UH.SetAvatar(self.IconSp, data:AvatarType(),data:AvatarId(),data:AvatarQuality())
end