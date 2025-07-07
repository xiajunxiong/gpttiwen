GuildPersonRankView = GuildPersonRankView or DeclareView("GuildPersonRankView","guild/guild_person_rank")
-- Board = UIWSubstitute(UIWBoard3)
function GuildPersonRankView:GuildActiveRankView()
    self.data = GuildData.Instance
end

function GuildPersonRankView:LoadCallback()
	GuildCtrl.Instance:SendGuildActivePersonRank()
    self.Board:SetData(self:GetType(), Language.Guild.GuildPersonRankTitle, Vector2.New(948, 558))
end

function GuildPersonRankView:CloseCallback()

end


GuildPersonRankPanel = GuildPersonRankPanel or DeclareMono("GuildPersonRankPanel", UIWFlushPanel)
function GuildPersonRankPanel:GuildPersonRankPanel()
	self.data = GuildData.Instance
	self.data_cares = {
		{data = self.data.guild_active_person_rank, func = self.FlushShowPanel, init_call = true},
    }
end 

function GuildPersonRankPanel:FlushShowPanel()
	local is_show = table.nums(self.data.guild_active_person_rank) == 0 and true or false
	self.nodataobj:SetActive(is_show)
	for k, v in pairs(self.data.guild_active_person_rank) do
		v.rank_index = k
	end
	self.guildranklist:SetDataList(self.data.guild_active_person_rank)	
	self:FlushMyRankData()
end

function GuildPersonRankPanel:FlushMyRankData()
	local my_data ={}
	if self.data.guild_active_person_rank_my_index ~= -1 then
		for k, v in pairs(self.data.guild_active_person_rank) do
			if k == self.data.guild_active_person_rank_my_index  + 1 then
				v.rank_index = k
				my_data = v
				break
			end
		end
	end
	
	if not my_data.rank_index then
		my_data.rank_index = 999
		my_data.name = RoleData.Instance:GetRoleName()
		my_data.level = RoleData.Instance:GetRoleLevel()
		my_data.prof = RoleData.Instance:GetRoleProfId()
		my_data.week_acitve_val = 0
		my_data.post = GuildData.Instance:GetGuildInfo().post
	end
	self.myranklist:SetData(my_data)
end

GuildPersonRankItem = GuildPersonRankItem or DeclareMono("GuildPersonRankItem", UIWidgetBaseItem)
function GuildPersonRankItem:GuildPersonRankItem()
    -- body
end
function GuildPersonRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.RankObj:SetActive(true)
	local rank_img = "guildrankother"
	if data.rank_index <= 3 then
		rank_img = "guildrank"..tostring(data.rank_index)
		self.RankObj:SetActive(false)
	end
	
    UH.SpriteName(self.RankBgImg,rank_img)
	if self.ItemBgImg then
		local tiao_show = data.rank_index % 2
		if tiao_show == 1 then
			UH.SpriteName(self.ItemBgImg,"guildactivebottom1")
		else
			UH.SpriteName(self.ItemBgImg,"guildactivebottom2")
		end
	end
    UH.SetText(self.LblRank, tostring(data.rank_index))
	if data.rank_index == 999 then
		UH.SetText(self.LblRank, Language.Guild.GuildActiveNoRand)
		UH.SpriteName(self.RankBgImg,"")
	end
    UH.SetText(self.LblName, data.name)
	UH.SetText(self.LblLevel, data.level)
	UH.SetText(self.LblActive, tostring(data.week_acitve_val))
	
	local job_info = GuildData.Instance:GetJobInfo(data.post)
    UH.SetText(self.LblPos, job_info.member_name)
	UH.SpriteName(self.ProImg,RoleData.GetProfSpByProfId(data.prof, false))

    local reward_data_list = GuildData.Instance:GetGuildPersonRankRewardByIndex(data.rank_index)
	local itemdata = DataHelper.FormatItemList(reward_data_list)
	self.RewardList:SetData(itemdata)
end

