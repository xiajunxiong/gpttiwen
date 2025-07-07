BattleDamageTips = BattleDamageTips or DeclareView("BattleDamageTips", "battle/battle_damage_tips", 0)--, ViewLayer.Top - 1)
VIEW_DECLARE_LEVEL(BattleDamageTips, ViewLevel.Lt)
VIEW_DECLARE_MASK(BattleDamageTips, ViewMask.None)

function BattleDamageTips:BattleDamageTips()
end

function BattleDamageTips:LoadCallback()
end

BattleDamageTipsPanel = BattleDamageTipsPanel or DeclareMono("BattleDamageTipsPanel", UIWFlushPanel)
function BattleDamageTipsPanel:BattleDamageTipsPanel()
	self.data = GuildData.Instance
	self.damage = 0
	-- self.data.guild_trial_total_damage = 0
	self.data_cares = {
		{data = self.data.damage_data, func = self.FlushRankInfo}
	}
end 

function BattleDamageTipsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function BattleDamageTipsPanel:FlushRankInfo()
--[[	local popup_datas= self.data.damage_data
	for _, v in pairs(popup_datas) do
		if BattleData.IsEnemyIdx(v.behiter) then
			if v.num and v.num < 0 and v.type ~= BATTLE_HP_DESC.MP_DECREASE then
				self.damage  = self.damage - v.num
			end
		end
    end--]]
	self.damage  = self.data:GetTrialDamage()
	UH.SetText(self.DamageValue,tostring(self.damage))
	if self.damage > 0 then
		local rank_text = Language.Guild.TrialNoRank
		local rank_index = -1
		local list = GuildData.Instance:BossRankList()
		if table.nums(list) == 0 then
			rank_index = 1
		else
			local min_index = 1000
			for _, v in pairs(list) do
				if v.total_damage < self.damage then
					if v.index  < min_index then
						min_index = v.index 
					end
				end
			end
			if min_index == 1000 then
				rank_index = table.nums(list) + 1
			else
				rank_index = min_index
			end
		end
		if rank_index > 0 and rank_index < 120 then
			rank_text = tostring(rank_index)
		end
		UH.SetText(self.RankValue,rank_text)
	end
end