ArenaMatchViewRewardTab = ArenaMatchViewRewardTab or DeclareMono("ArenaMatchViewRewardTab", UIWFlushPanel)
function ArenaMatchViewRewardTab:ArenaMatchViewRewardTab()
	self.data = ArenaMatchData.Instance
	self.data_cares = {
		{data = self.data.role_info_list, func = self.FlushDown, init_call = false},
	}
end

function ArenaMatchViewRewardTab:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushDown()
end

function ArenaMatchViewRewardTab:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushToggle()
end

function ArenaMatchViewRewardTab:OnDisable()
	UIWFlushPanel.OnDisable(self)
	self.data.cur_sel_preview.val = -1
end

function ArenaMatchViewRewardTab:FlushToggle()
	local role_info_list = self.data:RoleInfoList()
	local index = 1
	if not self.data.cur_sel_preview.val or self.data.cur_sel_preview.val < 0 then
		index = #role_info_list > 0 and 2 or 1
	end
	for i = 1, self.Tabs:Length() do
		self.Tabs[i].isOn = i == index
	end
end

function ArenaMatchViewRewardTab:FlushDown()
	local role_info_list = self.data:RoleInfoList()
	local empty = 0 == #role_info_list
	self.DownRObj:SetActive(not empty)
	self.DownNObj:SetActive(empty)
	local rewards_show = self.data.RewardsShow(role_info_list)
	self.RewardList:SetData(rewards_show)
end


-- ======= 擂台收益 ========
ArenaMatchViewEarnings = ArenaMatchViewEarnings or DeclareMono("ArenaMatchViewEarnings", UIWFlushPanel)

function ArenaMatchViewEarnings:ArenaMatchViewEarnings()
	self.data = ArenaMatchCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.role_info_list, func = self.FlushRewardInfo},
	}
end

function ArenaMatchViewEarnings:FlushRewardInfo()
	local role_info_list = self.data:RoleInfoList()
	self.RoleInfoList:SetData(role_info_list)

	local empty = 0 == #role_info_list
	self.EmptyObj:SetActive(empty)
end

function ArenaMatchViewEarnings:OnClickPreview()
	ViewMgr:OpenView(ArenaMatchRewardPreviewView)
end

ArenaMatchViewEarningsItem = DeclareMono("ArenaMatchViewEarningsItem", UIWidgetBaseItem)

function ArenaMatchViewEarningsItem:ArenaMatchViewEarningsItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewEarningsItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local arena_info = self.data.arena_info
	self.ArenaName.text = arena_info.leitai_name
	self.Rank.text = data:RankL()
	self.Rewards:SetData(data:Rewards())
	self.BgSp.SpriteName = ArenaMatchConfig.BgSp[data.bg_type]
	self.EffectObj:SetActive(data.index <= 2)
end

ArenaMatchViewEarningsShowItem = DeclareMono("ArenaMatchViewEarningsShowItem", UIWidgetBaseItem)

function ArenaMatchViewEarningsShowItem:ArenaMatchViewEarningsShowItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewEarningsShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Cell:SetData(data)
end



-- ====================== 奖励预览 ===============
ArenaMatchViewReward = ArenaMatchViewReward or DeclareMono("ArenaMatchViewReward", UIWFlushPanel)
function ArenaMatchViewReward:ArenaMatchViewReward()
	self.data = ArenaMatchData.Instance
	self.data_cares = {
        {data = self.data.cur_sel_preview, func = self.FlushReward}
    }
end

function ArenaMatchViewReward:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushPanel()
end

function ArenaMatchViewReward:GetSelIndex()
	local index = 1
	local list_data = self.data:GetArenaPosSeqList()
	local cur_pos = self.data.cur_sel_preview.val
	for i,v in pairs(list_data) do
		if cur_pos == v.position then
			index = i
			return index
		end
	end
	return index
end

function ArenaMatchViewReward:FlushPanel()
	self:FlushBtn()
	self:FlushReward()
end

function ArenaMatchViewReward:FlushBtn()
	if not TableIsEmpty(self.BtnList:GetData()) then
		self.BtnList:SelectAll(false)
	end
	local list_data = self.data:GetArenaPosSeqList()
	self.BtnList:SetData(list_data)
	self.BtnList:SetectItem(self:GetSelIndex(), true)
end

function ArenaMatchViewReward:FlushReward()
	self.RewardList:SetData({})
	local cur_pos = self.data.cur_sel_preview.val < 0 and 0 or self.data.cur_sel_preview.val
	local list_data = self.data:GetRewardList(cur_pos)
	self.RewardList:SetData(list_data)
end


ArenaMatchViewRewardBtnItem = ArenaMatchViewRewardBtnItem or DeclareMono("ArenaMatchViewRewardBtnItem", UIWidgetBaseItem)
function ArenaMatchViewRewardBtnItem:ArenaMatchViewRewardBtnItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchViewRewardBtnItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.Name, data.desc)
	local img_on = data.seq <= 2 and "AnNiu6" or "AnNiu4"
	local img_off = data.seq <= 2 and "AnNiu5" or "AnNiu3"
	UH.SpriteName(self.ImgOn, img_on)
	UH.SpriteName(self.ImgOff, img_off)
end

function ArenaMatchViewRewardBtnItem:OnToggleClick()
	if self.arena_match_data.cur_sel_preview.val then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
	end
	self.arena_match_data.cur_sel_preview.val = self.data.position
end


ArenaMatchViewRewardItem = DeclareMono("ArenaMatchViewRewardItem", UIWidgetBaseItem)

function ArenaMatchViewRewardItem:ArenaMatchViewRewardItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.RewardPreview
end

function ArenaMatchViewRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	--self.Rank.text = string.format(self.language.RankDesc[data.max_rank - data.min_rank] or self.language.RankDesc.default, data.min_rank, data.max_rank - 1)
	UH.SetText(self.Rank, data.max_rank - data.min_rank == 1 and data.min_rank or string.format("%s ~ %s",data.min_rank, data.max_rank))
	if data.min_rank <= 3 then
		self.Rank:SetObjActive(false)
		self.RankImg:SetObjActive(true)
		UH.SpriteName(self.RankImg, string.format("paiming%s",data.min_rank))
	else
		self.Rank:SetObjActive(true)
		self.RankImg:SetObjActive(false)
	end
	self.Rewards:SetData(data.rewards)
end