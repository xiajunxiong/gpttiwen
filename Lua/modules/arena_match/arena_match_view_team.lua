
ArenaMatchViewTeam = ArenaMatchViewTeam or DeclareMono("ArenaMatchViewTeam", UIWFlushPanel)

function ArenaMatchViewTeam:ArenaMatchViewTeam()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch

	self.data_cares = {
		{data = self.data.role_info_list, func = self.FlushRoleInfo},
		{data = self.data.role_info_nums, func = self.FlushArenaState},
	}
	GuideManager.Instance:RegisterGetGuideUi("XiFangQiSuBtn", function()
		if self.Arenas then
			return self.Arenas[1], function()
				ViewMgr:CloseView(ArenaMatchView)
				SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ARENA_MATCH, 1)
			end
		end
	end)
end

function ArenaMatchViewTeam:Awake()
	UIWFlushPanel.Awake(self)

	-- self:FlushArenaPosList()
end

function ArenaMatchViewTeam:FlushArenaPosList()
	local list = self.data:GetArenaPosSeqList()
	for i = 1, self.Arenas:Length() do
		self.Arenas[i]:SetData(list[i])
	end
end

function ArenaMatchViewTeam:FlushRoleInfo()
	local role_info_list = self.data:RoleInfoList()
	self.RoleInfoList:SetData(role_info_list)
end

function ArenaMatchViewTeam:FlushArenaState()
	local role_info_nums = self.data:RoleInfoNums()
	self.ArenaState.text = role_info_nums.arena > 0 and string.format(self.language.ArenaState, role_info_nums.partner, role_info_nums.pet, role_info_nums.arena) or self.language.ArenaStateEmpty
	self.EmptyObj:SetActive(0 == role_info_nums.arena)
end

ArenaMatchViewTeamRoleInfoItem = DeclareMono("ArenaMatchViewTeamRoleInfoItem", UIWidgetBaseItem)

function ArenaMatchViewTeamRoleInfoItem:ArenaMatchViewTeamRoleInfoItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewTeamRoleInfoItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local arena_info = self.data.arena_info
	self.ArenaName.text = arena_info.leitai_name
	self.Rank.text = data:RankL()
	self.PetList:SetData(data:PetList())
	self.PartnerList:SetData(data:PartnerList())
	self.NoPetObj:SetActive(0 == data:PetNum())
	self.NoPartnerObj:SetActive(0 == data:PartnerNum())
	self.BgSp.SpriteName = ArenaMatchConfig.BgSp[data.bg_type]
end

function ArenaMatchViewTeamRoleInfoItem:OnClickManage()
	self.arena_match_data.cur_sel_info:Set(self.data)
	self.arena_match_data.cur_sel_info:BackupPP()
	ViewMgr:OpenView(ArenaMatchInfoView)
end

function ArenaMatchViewTeamRoleInfoItem:OnClickRecall()
	ArenaMatchCtrl.Instance:SendPlatformBattleRemove(self.data:ArenaSeq())
end