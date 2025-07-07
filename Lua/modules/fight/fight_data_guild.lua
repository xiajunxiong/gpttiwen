
function FightData:FightDataGuild()
	self.guild_fb_info = SmartData.New()
end

function FightData:SetGuildFBInfo(protocol)
	self.guild_fb_info:Set(protocol.guild_fb_info)
end

function FightData:GetGuildFBInfo(protocol)
	return self.guild_fb_info
end

function FightData:GuildFbConfig()
	return self.guild_fb_config
end
