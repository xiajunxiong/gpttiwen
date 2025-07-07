GuildEmbattlePreview = GuildEmbattlePreview or DeclareView("GuildEmbattlePreview", "guild_embattle/guild_embattle_preview")
function GuildEmbattlePreview:GuildEmbattlePreview()
    self.data = GuildEmbattleData.Instance
end
function GuildEmbattlePreview:LoadCallback()
    self.List:SetData(self.data:GetPreviewReward())
    UH.SetText(self.Tip, Language.GuildEmbattle.BattleTip)
end
function GuildEmbattlePreview:OnClickClose()
    ViewMgr:CloseView(GuildEmbattlePreview)
end
function GuildEmbattlePreview:OnClickNameList()
    ViewMgr:OpenView(GuildEmbattleNameList)
end