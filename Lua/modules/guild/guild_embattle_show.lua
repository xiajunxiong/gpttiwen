GuildEmbattleShow = GuildEmbattleShow or DeclareView("GuildEmbattleShow", "guild/guild_embattle_show")
VIEW_DECLARE_MASK(GuildEmbattleShow, ViewMask.Block)
function GuildEmbattleShow:GuildEmbattleShow()
end
function GuildEmbattleShow:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.Board:SetData(self:GetType(), Language.Guild.EmbattleShow,Vector2.New(1378, 659))
end
function GuildEmbattleShow:OnFlush()
    
end

GuildEmbattleShowPanel = GuildEmbattleShowPanel or DeclareMono("GuildEmbattleShowPanel", UIWFlushPanel)
function GuildEmbattleShowPanel:GuildEmbattleShowPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.embattle, func = self.FlushZoneInfo, keys = {"flushview"}}
    }
end
function GuildEmbattleShowPanel:FlushZoneInfo()
    local data1 = self.data:GetHonorRoleInfo(1)
    data1 = data1 or {}
    self.List1:SetData(data1)
    local data2 = self.data:GetHonorRoleInfo(2)
    data2 = data2 or {}
    self.List2:SetData(data2)
    local data3 = self.data:GetHonorRoleInfo(3)
    data3 = data3 or {}
    self.List3:SetData(data3)
    local zone = self.data:GetMyZone()
    UH.SetText(self.LblTip,Language.Guild.EmbattleZoneDesc[zone]) 
end
function GuildEmbattleShowPanel:OnClickOk()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:CloseView(GuildEmbattleShow)
end