
ArenaMatchRewardPreviewView = ArenaMatchRewardPreviewView or DeclareView("ArenaMatchRewardPreviewView", ArenaMatchConfig.ResPath .. "arena_match_reward_preview")

function ArenaMatchRewardPreviewView:ArenaMatchRewardPreviewView()
	self.data = ArenaMatchCtrl.Instance:Data()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function ArenaMatchRewardPreviewView:LoadCallback(param_t)
	param_t = param_t or {}
	self.sel_index = param_t.sel_index or 1
	self.EnterBtnObj:SetActive(param_t.show_enter_btn == true)
	self:FlushPanel()
end

function ArenaMatchRewardPreviewView:FlushPanel()
	local list = self.data:GetArenaPosSeqList()
	for i = 1, self.Arenas:Length() do
		if list[i] then
			self.Arenas[i]:SetData(list[i])
		end
	end
	if self.sel_index then
		self.Arenas[self.sel_index].Toggle.isOn = true
	end
end

function ArenaMatchRewardPreviewView:CloseCallback()
	self.data.cur_sel_preview.val = 0
end

function ArenaMatchRewardPreviewView:OnClickClose()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	ViewMgr:CloseView(ArenaMatchRewardPreviewView)
end

function ArenaMatchRewardPreviewView:OnEnterClick()
	-- local data = self.data.arena_pos_list_p[self.data.cur_sel_preview.val]
	-- if data then
	-- 	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
	-- 	SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ARENA_MATCH, data.seq)
	-- 	ViewMgr:CloseView(ArenaMatchView)
	-- 	ViewMgr:CloseView(ArenaMatchRewardPreviewView)
	-- end
	ViewMgr:CloseView(ArenaMatchRewardPreviewView)
	ViewMgr:OpenView(ArenaMatchView)
end

ArenaMatchRewardPreviewViewDetail = ArenaMatchRewardPreviewViewDetail or DeclareMono("ArenaMatchRewardPreviewViewDetail",UIWFlushPanel)
function ArenaMatchRewardPreviewViewDetail:ArenaMatchRewardPreviewViewDetail()
	self.data = ArenaMatchCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.cur_sel_preview, func = self.FlushReward}
    }
end

function ArenaMatchRewardPreviewViewDetail:FlushReward()
	self.RewardList:SetData({})
	local reward_list = self.data:GetRewardList()
	self.RewardList:SetData(reward_list)
end

ArenaMatchRewardPreviewViewArenaItem = DeclareMono("ArenaMatchRewardPreviewViewArenaItem", UIWidgetBaseItem)

function ArenaMatchRewardPreviewViewArenaItem:ArenaMatchRewardPreviewViewArenaItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchRewardPreviewViewArenaItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function ArenaMatchRewardPreviewViewArenaItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data.desc
end

function ArenaMatchRewardPreviewViewArenaItem:OnClickItem()
	if self.arena_match_data.cur_sel_preview.val then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
	end
	self.arena_match_data.cur_sel_preview.val = self.data.position
end

ArenaMatchRewardPreviewViewItem = DeclareMono("ArenaMatchRewardPreviewViewItem", UIWidgetBaseItem)

function ArenaMatchRewardPreviewViewItem:ArenaMatchRewardPreviewViewItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.RewardPreview
end

function ArenaMatchRewardPreviewViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Rank.text = string.format(self.language.RankDesc[data.max_rank - data.min_rank] or self.language.RankDesc.default, data.min_rank, data.max_rank - 1)
	self.Rewards:SetData(data.rewards)
end