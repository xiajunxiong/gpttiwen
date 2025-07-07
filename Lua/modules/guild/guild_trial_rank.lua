GuildTrialRank = GuildTrialRank or DeclareView("GuildTrialRank", "guild/guild_trial_rank")
function GuildTrialRank:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function GuildTrialRank:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end
------------------------------------------------------
GuildTrialRankPanel = GuildTrialRankPanel or DeclareMono("GuildTrialRankPanel", UIWFlushPanel)
function GuildTrialRankPanel:GuildTrialRankPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data:BossRankList(), func = self.FlushRankInfo}
    }
end
function GuildTrialRankPanel:FlushRankInfo()
    local list = self.data:BossRankList()
    self.NoneObj:SetActive(table.nums(list) == 0)
    self.RankList:SetData(list)
    local out_rank, my_rank_data = self.data:GetMyRankData()
    if out_rank then
        self.RankObj:SetActive(true)
        UH.SetText(self.LblRank, Language.Guild.TrialNoRank)
    else
        if my_rank_data.index and my_rank_data.index > 0 and my_rank_data.index <=3 then
			self.RankObj:SetActive(false)
            self.NumArray[my_rank_data.index]:SetActive(true)
        else
            self.RankObj:SetActive(true)
            UH.SetText(self.LblRank, my_rank_data.index)
        end
		
		local reward_data_list = GuildData.Instance:GetTrialRankRewardList(my_rank_data.index)
		local reward_item_list = {}
		for k,v in pairs(reward_data_list) do
			table.insert(reward_item_list,Item.Init(v.item_id, v.num, v.is_bind))
		end
       self.RewardList:SetData(reward_item_list)
    end
    UH.SpriteName(self.ProfImg, ProfSettingB[my_rank_data.prof_base])
    UH.SetText(self.LblAttack, my_rank_data.total_damage)
    UH.SetText(self.LblName, my_rank_data.role_name)

end

function GuildTrialRankPanel:OnClickTip()
    ViewMgr:OpenView(GuildTrialRankRewardPreview)
end

function GuildTrialRankPanel:OnClickClose()
	ViewMgr:CloseView(GuildTrialRank)
end

-------------------------------------------------------
GuildTrialRankItem = GuildTrialRankItem or DeclareMono("GuildTrialRankItem", UIWidgetBaseItem)
function GuildTrialRankItem:GuildTrialRankItem()
    -- body
end
function GuildTrialRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

   -- UH.SpriteName(self.ProfImg, ProfSettingB[data.prof_base])
    UH.SetText(self.LblAttack, data.total_damage)
    UH.SetText(self.LblName, data.role_name)
    
    if data.role_id == RoleData.Instance:GetRoleId() then
        UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    else
        UH.SetAvatar(self.IconSp, data.avatar_type,data.avatar_id,data.avatar_quality)
    end
	local sprite_name = "ZhengChangDi"
	for i = 1, 3 do
		self.no_array[i]:SetActive(false)
	end
	self.RankObj:SetActive(false)
    if data.index and data.index > 0 and data.index <=3 then
        self.no_array[data.index]:SetActive(true)
		sprite_name = "PaiMingDi_"..tostring(data.index)
    else
        self.RankObj:SetActive(true)
        UH.SetText(self.LblRank, data.index)
    end
	UH.SpriteName(self.ItemSp,sprite_name)
    local reward_data_list = GuildData.Instance:GetTrialRankRewardList(data.index)
	local reward_item_list = {}
    for k,v in pairs(reward_data_list) do
        table.insert(reward_item_list,Item.Init(v.item_id, v.num, v.is_bind))
    end
    self.RewardList:SetData(reward_item_list)
end
--------------------------------
GuildTrialRankRewardPreview = GuildTrialRankRewardPreview or DeclareView("GuildTrialRankRewardPreview","guild/guild_trial_rank_reward_preview")
function GuildTrialRankRewardPreview:GuildTrialRankRewardPreview()
end

GuildTrialRankRewardPreviewPanel = GuildTrialRankRewardPreviewPanel or DeclareMono("GuildTrialRankRewardPreviewPanel",UIWFlushPanel)
function GuildTrialRankRewardPreviewPanel:GuildTrialRankRewardPreviewPanel()
	local info = GuildData.Instance:GuildBossInfo()
    self:FlushChapterReward(info.guild_boss_chapter_id)
end

function GuildTrialRankRewardPreviewPanel:FlushChapterReward(chapter_id)
    local data_list = GuildData.Instance.guild_trial_rank_reward[chapter_id]
    self.RankList:SetData(data_list)
end

function GuildTrialRankRewardPreviewPanel:OnClickChapter(chapter_id)
    self:FlushChapterReward(chapter_id)
end

function GuildTrialRankRewardPreviewPanel:OnClickClose()
    ViewMgr:CloseView(GuildTrialRankRewardPreview)
end

GuildTrialRankRewardPreviewItem = GuildTrialRankRewardPreviewItem or DeclareMono("GuildTrialRankRewardPreviewItem", UIWidgetBaseItem)
function GuildTrialRankRewardPreviewItem:GuildTrialRankRewardPreviewItem()
    self.guild_data = GuildData.Instance
end

function GuildTrialRankRewardPreviewItem:SetData(data)
    if data.min_rank == data.max_rank then
        UH.SetText(self.LblRank, string.format(Language.Guild.TrialRankFormat, data.min_rank))
    else
        UH.SetText(self.LblRank, string.format(Language.Guild.TrialRankFormat, data.min_rank.."-"..data.max_rank))
    end
	local reward_item_list = {}
    for k,v in pairs(data.reward_show) do
        table.insert(reward_item_list,Item.Init(v.item_id, v.num, v.is_bind))
    end
    self.RewardList:SetData(reward_item_list)
    self.data = data
end
-------------------------------------------------------------------
GuildTrialReward = GuildTrialReward or DeclareView("GuildTrialReward","guild/guild_trial_reward")
-- Board = UIWBoard2
function GuildTrialReward:GuildTrialReward()
end

function GuildTrialReward:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.TrialRewardTitle, Vector2.New(752, 482))
    self.GuildTrialRewardPanel:FlushRewardPanel()
end
-----------------------------------------------------------------
GuildTrialRewardPanel = GuildTrialRewardPanel or DeclareMono("GuildTrialRewardPanel", UIWFlushPanel)
-- RewardList = UIWidgetList
-- LeftTime = Text
function GuildTrialRewardPanel:GuildTrialRewardPanel()
    self.data = GuildData.Instance
end

function GuildTrialRewardPanel:FlushRewardPanel()
    --奖励
    local boss_info = self.data:GuildBossInfo()
    local data_list = self.data:GetGuildBossReward(boss_info.guild_boss_chapter_id, boss_info.guild_boss_seq - 1)
    self.RewardList:SetData(data_list)
    self.count_down = TimeHelper:AddRunTimer(BindTool.Bind(self.SetTime, self), 60)
end

function GuildTrialRewardPanel:OnDestroy()
    if self.count_down ~= nil then
        TimeHelper:CancelTimer(self.count_down)
        self.count_down = nil
    end
    UIWFlushPanel.OnDestroy(self)
end

function GuildTrialRewardPanel:SetTime()
    --倒计时
    local time = Split(os.date("%d-%H-%M", tonumber(TimeHelper.GetServerTime())),"-")
    local leftTime = 0
    if tonumber(time[2]) < 6 then
        leftTime = 6 * 3600 - (tonumber(time[2]) * 3600 + tonumber(time[3]) * 60)
    else
        leftTime = 30 * 3600 - (tonumber(time[2]) * 3600 + tonumber(time[3]) * 60)
    end
    local timeStr = string.format(Language.Guild.LeftTime, tostring(math.modf(leftTime / 3600)), tostring((leftTime % 3600) / 60))
    UH.SetText(self.LeftTime, timeStr)
end

-----------------------------------------------------------------
GuildTrialRewardItem = GuildTrialRewardItem or DeclareMono("GuildTrialRewardItem", UIWidgetBaseItem)

function GuildTrialRewardItem:GuildTrialRewardItem()
end

function GuildTrialRewardItem:SetData(data)
    local item_data = Item.Init(data.item_id, data.num, data.is_bind)
    self.data = item_data
    self.data.Dark = true
    CellFlushs.SetIcon(self, item_data:IconId())
    CellFlushs.SetIconQuality(self, item_data.item_id)
    CellFlushs.CheckSomething(self, item_data)
    CellFlushs.SetName(self, item_data, true, true)
end

----------------家族试炼公会排行------------------------------------------
GuildTrialTotalRank = GuildTrialTotalRank or DeclareView("GuildTrialTotalRank", "guild/guild_trial_total_rank")
function GuildTrialTotalRank:GuildTrialTotalRank()
end

function GuildTrialTotalRank:LoadCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function GuildTrialTotalRank:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end
------------------------------------------------------
GuildTrialTotalRankPanel = GuildTrialTotalRankPanel or DeclareMono("GuildTrialTotalRankPanel", UIWFlushPanel)
function GuildTrialTotalRankPanel:GuildTrialTotalRankPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data:GetTotalRankList(), func = self.FlushTotalRankInfo}
    }
end

function GuildTrialTotalRankPanel:FlushTotalRankInfo()
    local data_list = self.data:GetTotalRankList()
	local boss_info = self.data:GuildBossInfo()
	local guild_info = self.data:GetGuildInfo()
    self.RankList:SetData(data_list)
	local my_data = {}
	my_data.rank = -1
	my_data.guild_name = guild_info.guild_name
	my_data.guild_banner = guild_info.arms
	my_data.kill_num = 0--boss_info.guild_boss_seq
	my_data.boss_index = 0--boss_info.guild_boss_seq
	
	for k,v in pairs(data_list) do
		if v.guild_name == my_data.guild_name then
			my_data = v
			break
		end
	end
	self.MyRank:SetData(my_data)
end

function GuildTrialTotalRankPanel:OnClickClose()
	ViewMgr:CloseView(GuildTrialTotalRank)
end

-------------------------------------------------------
GuildTrialTotalRankItem = GuildTrialTotalRankItem or DeclareMono("GuildTrialTotalRankItem", UIWidgetBaseItem)
function GuildTrialTotalRankItem:GuildTrialTotalRankItem()
end

function GuildTrialTotalRankItem:SetData(data)
    if data.rank and data.rank > 0 and data.rank <=3 then
        self.no_array[data.rank]:SetActive(true)
    else
        self.RankOj:SetActive(true)     
		if  data.rank < 1 then
			 self.Rank.text = Language.Guild.TrialNoRank
		else
			 self.Rank.text = data.rank
		end
    end
    self.GuildName.text = data.guild_name
    self.Banner.text = data.guild_banner
    self.KillNum.text = data.kill_num
	local index = data.boss_index
	if data.boss_index == 0 then
		index = 1
	end
    self.Chapter.text = Language.Guild.GuildTrialNewBossStar[index][1]
end