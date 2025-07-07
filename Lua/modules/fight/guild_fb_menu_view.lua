
GuildFBMenuView = GuildFBMenuView or DeclareMono("GuildFBMenuView", UIWFlushPanel)

function GuildFBMenuView:GuildFBMenuView()
	self.data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetGuildFBInfo(), func = self.FlushMenu},
	}
	self.language = Language.Fight.GuildFB
end

function GuildFBMenuView:FlushMenu()
	local guild_fb_info = self.data:GetGuildFBInfo()
	local guild_fb_config = self.data:GuildFbConfig()
	self.Layer.text = string.format(self.language.progress, guild_fb_info.layer, guild_fb_config.layer)

	self.MonsterNum.text = guild_fb_info.monster_num
	self.TreasureBoxNum.text = guild_fb_info.box_num
	self.Goal.text = guild_fb_config.describe
end

function GuildFBMenuView:OnClickLeave()
	SceneCtrl.Instance:RequestLeaveFb()
end

function GuildFBMenuView:OnClickItem(item_id)
	local info = {
		item_list = {Item.Init(item_id)},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end