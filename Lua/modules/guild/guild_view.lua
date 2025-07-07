FILE_REQ("modules/guild/guild_info")
FILE_REQ("modules/guild/guild_member")
FILE_REQ("modules/guild/guild_build")
FILE_REQ("modules/guild/guild_shop")
FILE_REQ("modules/guild/guild_pet")
FILE_REQ("modules/guild/guild_mission_tab")
FILE_REQ("modules/guild/guild_trial_reward_get")
FILE_REQ("modules/guild/guild_jiyuan")

GuildView = GuildView or DeclareView("GuildView", "guild/guild", Mod.Guild.GuildInfo)
VIEW_DECLARE_LEVEL(GuildView,ViewLevel.L1)
----Board=======UIWBoard
----Tabbar======UIWidgetList
function GuildView:GuildView()
    self.lang = Language.Guild
self.guild_data = GuildData.Instance
end
function GuildView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), self.lang.Title)
    self.Board:SetTabbarVisible(true)
    self.Board.Tabbar:SetData(
        {
            {name = self.lang.Tab1, tab_objs = {self.Tab1}, mod = TabMod(Mod.Guild.GuildInfo),open = Mod.Guild.GuildInfo},
            -- {name = self.lang.Tab10, tab_objs = {self.Tab2}, mod = TabMod(Mod.Guild.GuildMission),open = Mod.Guild.GuildMission},
            {name = self.lang.Tab2, tab_objs = {self.Tab3}, mod = TabMod(Mod.Guild.GuildMember),open = Mod.Guild.GuildMember},
            {name = self.lang.Tab14, tab_objs = {self.Tab7}, mod = TabMod(Mod.Guild.GuildSkill),open = Mod.Guild.GuildSkill},
			{name = self.lang.Tab3, tab_objs = {self.Tab4}, mod = TabMod(Mod.Guild.GuildJiYuan),open = Mod.Guild.GuildJiYuan,open_func = BindTool.Bind(self.CheckJiYuanOpenCondition, self),open_tip = string.format(ErrorText[155],self.guild_data.guild_other.pray_family_level)},
            {name = self.lang.Tab4, tab_objs = {self.Tab5}, open = Mod.Guild.GuildShop, mod = TabMod(Mod.Guild.GuildShop)},
            {name = self.lang.Tab5, tab_objs = {self.Tab6}, mod = TabMod(Mod.Guild.GuildPet),open = Mod.Guild.GuildPet},
            -- {name = self.lang.Tab14, tab_objs = {self.Tab8}, mod = TabMod(Mod.Guild.GuildPet),open = Mod.Guild.GuildPet},
        }
    )
    param_t = param_t or {}
    self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Guild,self.Board.Tabbar,param_t.open_param or 1))
	self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.ImmortalCoin, CurrencyType.Gold, CurrencyType.GuildCotribution)
end

function GuildView:CheckJiYuanOpenCondition()
    if self.guild_data:GetGuildInfo().level and self.guild_data.guild_other.pray_family_level then
        if self.guild_data:GetGuildInfo().level >= self.guild_data.guild_other.pray_family_level then
            return true
        end
    end
	return false
end