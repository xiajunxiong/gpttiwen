GuildActiveBoxView = GuildActiveBoxView or DeclareView("GuildActiveBoxView","guild/guild_active_box")
-- Board = UIWSubstitute(UIWBoard3)
function GuildActiveBoxView:GuildActiveRankView()

end

function GuildActiveBoxView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.GuildActiveBoxTitle, Vector2.New(524, 366))
end

function GuildActiveBoxView:OnClickClose()
	ViewMgr:CloseView(GuildActiveBoxView)
end


GuildActiveBoxPanel = GuildActiveBoxPanel or DeclareMono("GuildActiveBoxPanel", UIWFlushPanel)
function GuildActiveBoxPanel:GuildActiveBoxPanel()
	self.data = GuildData.Instance
	self.data_cares = {
		{data = self.data.guild_active_box_seq, func = self.FlushShowPanel,keys = {"seq"},init_call = true},
    }
end 

function GuildActiveBoxPanel:FlushShowPanel()
	local itemdata = DataHelper.FormatItemList(self.data:GetGuildActiveRewardBySeq(self.data.guild_active_box_seq.seq))
	self.ItemList:SetData(itemdata)
	local str = Language.Guild.GuildActiveBoxHasGet
	if not self.data:GetGuildActiveRewardIsGetBySeq(self.data.guild_active_box_seq.seq) then
		local current = self.data.guild_active_info.day_active_val
		local all = self.data:GetGuildActiveLimitBySeq(self.data.guild_active_box_seq.seq)
		str = string.format(Language.Guild.GuildActiveBoxDec,tostring(current),tostring(all))
	end
	UH.SetText(self.ActiveText,str)
end

function GuildActiveBoxPanel:OnClickButton()
	ViewMgr:CloseView(GuildActiveBoxView)
end


