
ArenaMatchRankView = ArenaMatchRankView or DeclareView("ArenaMatchRankView", ArenaMatchConfig.ResPath .. "arena_match_rank")
VIEW_DECLARE_LEVEL(ArenaMatchRankView,ViewLevel.L1)

function ArenaMatchRankView:OnClickClose()
	ViewMgr:CloseView(ArenaMatchRankView)
end

ArenaMatchRankViewDetail = ArenaMatchRankViewDetail or DeclareMono("ArenaMatchRankViewDetail", UIWFlushPanel)

function ArenaMatchRankViewDetail:ArenaMatchRankViewDetail()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch

	self.data_cares = {
		{data = self.data.rank_info_list, func = self.FlushRankInfo},
	}
end

function ArenaMatchRankViewDetail:FlushRankInfo()
	self.Title.text = self.data.cur_sel_arena.leitai_name

	local rank_info_list = self.data:RankInfoList()
	self.RankList:SetData(rank_info_list)

	local rank_info = self.data:RoleInfoBySeq(self.data.cur_sel_arena.seq)
	local own_info = rank_info and rank_info_list[rank_info:Rank()] or nil
	self.OwnItem:SetObjActive(nil ~= own_info)
	self.OwnItem:SetData(own_info)
end

ArenaMatchRankViewItem = DeclareMono("ArenaMatchRankViewItem", UIWidgetBaseItem)

function ArenaMatchRankViewItem:ArenaMatchRankViewItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Rank
end

function ArenaMatchRankViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local rank = data:Rank()
	self.RankSp.SpriteName = data:RankSp()
	self.Rank.text = rank > 3 and rank or ""
	self.Name.text = data:Name()
	if data.role_info.top_level and data.role_info.top_level > 0 then
		self.Level.text = string.format(Language.PeakRoad.Level2, data.role_info.top_level)
	else
		self.Level.text = "Lv." .. data:Level()
	end
	self.Cap.text = data:PetsCap() + data:PartnersCap()
	UH.SetAvatar(self.IconSp, data:AvatarType(),data:AvatarId(),data:AvatarQuality())
	self.BtnFightObj:SetActive(not data:IsOwn())
end

function ArenaMatchRankViewItem:OnClickFight()
	local rank_info = self.arena_match_data:RoleInfoBySeq(self.arena_match_data.cur_sel_arena.seq)
	if rank_info and rank_info:Rank() < self.data:Rank() then
		PublicPopupCtrl.Instance:Center(self.language.Tips)
		return
	end
	self.arena_match_data.cur_sel_info:Set(ArenaMatchReadyInfo.New(ArenaMatchInfo.CreateInfo{per_info = self.data}))
	ViewMgr:OpenView(ArenaMatchInfoView)
end

function ArenaMatchRankViewItem:OnClickInfo()
	self.arena_match_data.cur_sel_info:Set(ArenaMatchReadyInfo.New(ArenaMatchInfo.CreateInfo{per_info = self.data}))
	ViewMgr:OpenView(ArenaMatchWinnerView)
end

ArenaMatchRankViewOwnItem = DeclareMono("ArenaMatchRankViewOwnItem", UIWidgetBaseItem)

function ArenaMatchRankViewOwnItem:ArenaMatchRankViewOwnItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Rank
end

function ArenaMatchRankViewOwnItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local no_rank = nil == data
	self.RankSp.SpriteName = no_rank and ArenaMatchConfig.RankSp.default or data:RankSp()
	if no_rank then
		self.NoRankObj:SetActive(true)
		self.Rank:SetObjActive(false)
	else
		local rank = data:Rank()
		self.NoRankObj:SetActive(false)
		self.Rank.text = rank > 3 and rank or ""
		self.Rank:SetObjActive(true)
	end
	self.Name.text = self.language.Wo
	if RoleData.Instance:GetTopLevel() and RoleData.Instance:GetTopLevel() > 0 then
		self.Level.text = string.format(Language.PeakRoad.Level2, RoleData.Instance:GetTopLevel())
	else
		self.Level.text = "Lv." .. RoleData.Instance:GetRoleLevel()
	end
	-- self.Level.text = "Lv." .. RoleData.Instance:GetRoleLevel()
	self.Cap.text = no_rank and 0 or (data:PetsCap() + data:PartnersCap())
	UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
	self.BtnManageObj:SetActive(not no_rank)
end

function ArenaMatchRankViewOwnItem:OnClickManage()
	self.arena_match_data.cur_sel_info:Set(self.data)
	self.arena_match_data.cur_sel_info:BackupPP()
	ViewMgr:OpenView(ArenaMatchInfoView)
end

function ArenaMatchRankViewOwnItem:OnClickRecall()
	ArenaMatchCtrl.Instance:SendPlatformBattleRemove(self.data:ArenaSeq())
end