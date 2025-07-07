
ArenaMatchMenuView = ArenaMatchMenuView or DeclareMono("ArenaMatchMenuView", UIWFlushPanel)

function ArenaMatchMenuView:ArenaMatchMenuView()
	self.data = ArenaMatchCtrl.Instance:Data()

	self.data_cares = {
		{data = SceneData.Instance.base, func = self.FlushUp, keys = {"scene_id"}},
		{data = SceneData.Instance.base, func = self.FlushDown, keys = {"scene_id"}},
		{data = self.data.role_info_list, func = self.FlushDown, init_call = false},
		{data = self.data.SelMatch, func = self.FlushDown, init_call = false}
	}
end

function ArenaMatchMenuView:FlushUp()
	--local list = self.data:GetArenaPosListWithoutScene(SceneData.Instance:SceneId())
	local list = self.data:GetArenaPosSeqList()
	self.UpGrid:SetData(list)
end

function ArenaMatchMenuView:FlushDown()
	-- local co = self.data:GetArenaPosSceneInfo(SceneData.Instance:SceneId())
	-- if nil == co then return end
	-- local list = self.data:GetArenaPosSceneInfoSortRoleInfo(co.position)
	local list = self.data:GetArenaPosSceneInfoSortRoleInfo(self.data.SelMatch.pos or 0)
	self.DownList:SetData(list)
end

function ArenaMatchMenuView:OnClickReward()
	ViewMgr:OpenViewByKey(Mod.ArenaMatch.Reward)
end

ArenaMatchMenuUpItem = DeclareMono("ArenaMatchMenuUpItem", UIWidgetBaseItem)

function ArenaMatchMenuUpItem:ArenaMatchMenuUpItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchMenuUpItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data.desc
	self.IconSp.SpriteName = ArenaMatchConfig.MenuPosToIconSp[data.position]
end

function ArenaMatchMenuUpItem:OnClickItem()
	-- PublicPopupCtrl.Instance:DialogTips{
	-- 	content = string.format(Language.ArenaMatch.GoTips, self.data.desc),
	-- 	confirm = {
	-- 		func = function()
	-- 			ArenaMatchCtrl.Instance:SendPlatformBattlePlaformSwitch(self.data.seq)
	-- 			ViewMgr:CloseView(ArenaMatchView)
	-- 			ViewMgr:CloseView(DialogTipsView)
	-- 		end
	-- 	}
	-- }
	self.arena_match_data.SelMatch.pos = self.data.position
end

ArenaMatchMenuDownItem = DeclareMono("ArenaMatchMenuDownItem", UIWidgetBaseItem)

function ArenaMatchMenuDownItem:ArenaMatchMenuDownItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Menu
end

function ArenaMatchMenuDownItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local role_list = self.arena_match_data.role_info_list
	local info = role_list[data.seq]
	if info then
		self.Name.text = RichTextHelper.ColorStr(string.format(self.language.NameRank, data.leitai_name, info:Rank()), COLORSTR.Green3)
	else
		self.Name.text = RichTextHelper.ColorStr(data.leitai_name, COLORSTR.Yellow7)
	end
	self.ItemSp.SpriteName = "LeiTaiDi" .. (nil == info and 1 or 2)
end

function ArenaMatchMenuDownItem:OnClickItem()
	SceneLogic.Instance:AutoToNpc(self.data.npc_seq, nil, false, false)
end
