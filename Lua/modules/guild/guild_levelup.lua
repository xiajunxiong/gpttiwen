GuildLevelUp = GuildLevelUp or DeclareView("GuildLevelUp", "guild/guild_levelup")
VIEW_DECLARE_MASK(GuildLevelUp,ViewMask.Block)
function GuildLevelUp:GuildLevelUp()
    -- body
end
function GuildLevelUp:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.Board:SetData(self:GetType(), Language.Guild.LevelTitle, Vector2.New(602,520))
end
function GuildLevelUp:OnFlush()
    local info = GuildData.Instance:GetGuildInfo()
    local level_info = GuildData.Instance:GetLevelInfo()
    local next_info = GuildData.Instance:GetLevelInfo(info.level + 1)
    local result = info.exp - level_info.exp
    UH.SetText(self.LblExp, info.exp)
    UH.SetText(self.LblNextExp, level_info.exp)-- result < 0 and 0 or result)
    UH.SetText(self.LblLevel, string.format(Language.Guild.LevelDesc, info.level))
    UH.SetText(self.LblNextLevel, string.format(Language.Guild.LevelDesc, info.level + 1))
    UH.SetText(self.LblWeiHu, string.format( Language.Guild.WeiHuDesc,level_info.cost ))
    UH.SetText(self.LblNextWeiHu, string.format( Language.Guild.WeiHuDesc, next_info.cost ))
    UH.SetText(self.LblNeed, level_info.exp)
    if result < 0 then
        self.UpInter.Interactable = false
        UH.SetText(self.LblLevelUp, Language.Guild.ExpNone)
    end
end
function GuildLevelUp:OnClickClose()
    ViewMgr:CloseView(GuildLevelUp)
end
function GuildLevelUp:OnClickLevelUp()
    GuildCtrl.SendGuildLevelUpReq(GuildData.Instance:GetGuildID())
    ViewMgr:CloseView(GuildLevelUp)
end