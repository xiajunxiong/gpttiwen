GuildActiveRankView = GuildActiveRankView or DeclareView("GuildActiveRankView","guild/guild_active_rank")
-- Board = UIWSubstitute(UIWBoard3)
function GuildActiveRankView:GuildActiveRankView()
    self.data = GuildData.Instance
end

function GuildActiveRankView:LoadCallback()
	GuildCtrl.Instance:SendCrossGuildActiveGuildRankInfo()
    self.Board:SetData(self:GetType(), Language.Guild.GuildActiveRankTitle, Vector2.New(948, 558))
end

function GuildActiveRankView:CloseCallback()

end


GuildActiveRankPanel = GuildActiveRankPanel or DeclareMono("GuildActiveRankPanel", UIWFlushPanel)
function GuildActiveRankPanel:GuildActiveRankPanel()
	self.data = GuildData.Instance
	self.data_cares = {
		{data = self.data.guild_active_total_rank, func = self.FlushShowPanel, init_call = true},
    }
end 

function GuildActiveRankPanel:FlushShowPanel()
	local is_show = table.nums(self.data.guild_active_total_rank) == 0 and true or false
	self.nodataobj:SetActive(is_show)
	self.guildranklist:SetDataList(self.data.guild_active_total_rank)
	self:FlushMyRankData()
end

function GuildActiveRankPanel:FlushMyRankData()
	local my_data ={}
	if self.data.guild_active_total_rank_my_index ~= -1 then
		for k, v in pairs(self.data.guild_active_total_rank) do
			if v.rank_index == self.data.guild_active_total_rank_my_index then
				my_data = v
			end
		end
	else
		my_data.rank_index = 999
		my_data.guild_name = self.data:GetGuildName()
		my_data.guild_banner = self.data:GetGuildInfo().arms
		my_data.rank_val = self.data.guild_active_info.week_active_val
	end
	self.myranklist:SetData(my_data)
end

GuildActiveRankItem = GuildActiveRankItem or DeclareMono("GuildActiveRankItem", UIWidgetBaseItem)
function GuildActiveRankItem:GuildActiveRankItem()
    -- body
end
function GuildActiveRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.RankObj:SetActive(true)
	local rank_img = "guildrankother"
	if data.rank_index <= 2 then
		rank_img = "guildrank"..tostring(data.rank_index + 1)
		self.RankObj:SetActive(false)
	end
	
    UH.SpriteName(self.RankBgImg,rank_img)
	if self.ItemBgImg then
		local tiao_show = data.rank_index % 2
		if tiao_show == 1 then
			UH.SpriteName(self.ItemBgImg,"guildactivebottom2")
		else
			UH.SpriteName(self.ItemBgImg,"guildactivebottom1")
		end
	end
    UH.SetText(self.LblRank, tostring(data.rank_index + 1))
	if data.rank_index == 999 then
		UH.SetText(self.LblRank, Language.Guild.GuildActiveNoRand)
		UH.SpriteName(self.RankBgImg,"")
	end
    UH.SetText(self.LblName, data.guild_name)
	UH.SetText(self.LblBanner, data.guild_banner)
	UH.SetText(self.LblActive, tostring(data.rank_val))

    local reward_data_list = GuildData.Instance:GetGuildActiveRankRewardByIndex(data.rank_index + 1)
	local itemdata = DataHelper.FormatItemList(reward_data_list)
	self.RewardList:SetData(itemdata)
end

