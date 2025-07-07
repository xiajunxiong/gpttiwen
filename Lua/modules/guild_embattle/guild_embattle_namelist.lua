-- 参数名单
GuildEmbattleNameList =
    GuildEmbattleNameList or DeclareView("GuildEmbattleNameList", "guild_embattle/guild_embattle_namelist")
function GuildEmbattleNameList:GuildEmbattleNameList()
    self.Board:SetData(self:GetType(), Language.GuildEmbattle.NameList)
end
function GuildEmbattleNameList:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    GuildEmbattleCtrl.CSGuildHonorReq2(GUILD_HONOR_REQ_TYPE_2.SCORE_RANK)
end
function GuildEmbattleNameList:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[123].desc})
end
function GuildEmbattleNameList:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end
GuildEmbattleNameListPanel = GuildEmbattleNameListPanel or DeclareMono("GuildEmbattleNameListPanel", UIWFlushPanel)
function GuildEmbattleNameListPanel:GuildEmbattleNameListPanel()
    self.embattle_data = GuildEmbattleData.Instance
    self.guild_data = GuildData.Instance
    self.data_cares = {
        {data = self.embattle_data.score_rank_info, func = self.onFlush},
        {data = self.guild_data:GetGuildInfo(), func = self.FlushDown},
    }
end
function GuildEmbattleNameListPanel:onFlush()
    local config = ActivityData.GetConfig(ActType.GuildEmbattle) or {}
    self.Time:SetShowCallBack(function(t)
        return Format(Language.Activity.TimeLimitRewardTip,config.name_text, t.day, t.hour ,t.min)
    end)
    self.Time:StampTime(ActivityData.GetRemindTimeStamp(ActType.GuildEmbattle))

    local list = self.embattle_data:ScoreRankInfo().list
    self.NoneObj:SetActive(table.nums(list) == 0)
    self.List:SetData(list)
    --显示自己的
    local data = GuildEmbattleData.Instance:MyGuildScoreInfo()
    local guild_id = GuildData.Instance:GetGuildID()
    self.NoGuild:SetActive(guild_id <= 0)
    self.HasGuild:SetActive(guild_id > 0)
    if data ~= nil then
        UH.SetText(self.LevelTex, data.index or Language.GuildEmbattle.NoneRank)
        UH.SetText(self.NameTex, data.name or GuildData.Instance:GetGuildName())
        UH.SetText(self.BannerTex, data.banner or GuildData.Instance:GetGuildInfo().arms)
        local server_name = self.embattle_data:ServerName(data.server_id or LoginData.Instance:GetLastLoginServer())
        UH.SetText(self.ServerTex, server_name)
        if data.join_guild_fight_day_flag[14] == 0 then
            UH.SetText(self.Score1, data.score[2] == 0 and Language.GuildEmbattle.TempNone or data.score[2])
        else
            UH.SetText(self.Score1, data.score[2])
        end
        --16-4
        if data.join_guild_fight_day_flag[12] == 0 then
            UH.SetText(self.Score2, data.score[4] == 0 and Language.GuildEmbattle.TempNone or data.score[4])
        else
            UH.SetText(self.Score2, data.score[4])
        end
        --[[ UH.SetText(self.Score1, data.score[2] == 0 and Language.GuildEmbattle.TempNone or data.score[2])
        UH.SetText(self.Score2, data.score[4] == 0 and Language.GuildEmbattle.TempNone or data.score[4]) ]]
        UH.SetText(self.TotalTex, data.score[2] + data.score[4])
    elseif guild_id > 0 then
        UH.SetText(self.LevelTex, Language.GuildEmbattle.NoneRank)
        UH.SetText(self.NameTex, GuildData.Instance:GetGuildName())
        UH.SetText(self.BannerTex, GuildData.Instance:GetGuildInfo().arms)
        local server_name = self.embattle_data:ServerName(LoginData.Instance:GetLastLoginServer())
        UH.SetText(self.ServerTex, server_name)
        UH.SetText(self.Score1, Language.GuildEmbattle.TempNone)
        UH.SetText(self.Score2, Language.GuildEmbattle.TempNone)
        UH.SetText(self.TotalTex, "0")
    end
end
function GuildEmbattleNameListPanel:FlushDown()
    local data = GuildEmbattleData.Instance:MyGuildScoreInfo()
    local guild_id = GuildData.Instance:GetGuildID()
    self.NoGuild:SetActive(data == nil and guild_id <= 0)
    self.HasGuild:SetActive(data ~= nil or guild_id > 0)
end
function GuildEmbattleNameListPanel:OnClickJoin()
    ViewMgr:OpenView(GuildList)
end
EmbattleNamelistItem = EmbattleNamelistItem or DeclareMono("EmbattleNamelistItem", UIWidgetBaseItem)
function EmbattleNamelistItem:EmbattleNamelistItem()
    self.embattle_data = GuildEmbattleData.Instance
end
function EmbattleNamelistItem:SetData(data)
    local guild_id = GuildData.Instance:GetGuildID()
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.BgSp, data.index % 2 == 0 and "DiTiao2" or "DiTiao1")
    UH.SpriteName(self.LevelSp, data.index)
    self.LevelSp:SetObjActive(data.index < 4)
    self.LevelObj:SetActive(data.index > 3)
    UH.SetText(self.LevelTex, data.index)
    UH.SetText(self.NameTex, guild_id == data.guild_id and self.embattle_data:GuildName(data.name) or data.name)
    UH.SetText(self.BannerTex, data.banner)
    local server_name = self.embattle_data:ServerName(data.server_id)
    UH.SetText(self.ServerTex, server_name)
    --这个要判断没有参加过 才显示暂无
    --16-2
    if data.join_guild_fight_day_flag[14] == 0 then
        UH.SetText(self.Score1, data.score[2] == 0 and Language.GuildEmbattle.TempNone or data.score[2])
    else
        UH.SetText(self.Score1, data.score[2])
    end
    --16-4
    if data.join_guild_fight_day_flag[12] == 0 then
        UH.SetText(self.Score2, data.score[4] == 0 and Language.GuildEmbattle.TempNone or data.score[4])
    else
        UH.SetText(self.Score2, data.score[4])
    end
    UH.SetText(self.TotalTex, data.score[2] + data.score[4])
end
