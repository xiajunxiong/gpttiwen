--进入活动
GuildEmbattleEnter = GuildEmbattleEnter or DeclareView("GuildEmbattleEnter", "guild_embattle/guild_embattle_enter")
function GuildEmbattleEnter:GuildEmbattleEnter()
    self.data = GuildEmbattleData.Instance
end
function GuildEmbattleEnter:OnFlush()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    UH.SetText(self.TipTex, Language.GuildEmbattle.TimeOut)
    local round = self.data:Stage() -- self.data:CalcRound()
    local desc = string.format(Language.GuildEmbattle.ReadyFight, Language.GuildEmbattle.RoundName[round])
    UH.SetText(self.DescTex, desc)
    local base = GuildEmbattleData.Instance:Base()
    self.Time:StampTime(base.next_round_state_time, TimeType.Type_Special_0)
    local guild_id = GuildData.Instance:GetGuildID()
    local info = self.data:OpponetInfo()
    -- LogError(info)
    --[[ local info = {
        {
            index = 1,
            server_id = 1,
            guild_id = 1,
            name = "家族1",
            banner = "加"
        },
        {
            index = 2,
            server_id = 80,
            guild_id = 2,
            name = "家族2",
            banner = "减"
        }
    } ]]
    local temp
    if info and info[1] then
        temp = info[1]
        UH.SetText(self.LeftInfo[1], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(
            self.LeftInfo[2],
            string.format(Language.GuildEmbattle.ServerName, self.data:ServerName(temp.server_id))
        )
        UH.SetText(self.LeftInfo[3], temp.banner)
    else
        UH.SetText(self.LeftInfo[1], "")
        UH.SetText(self.LeftInfo[2], "")
        UH.SetText(self.LeftInfo[3], "")
    end
    if info and info[2] then
        temp = info[2]
        UH.SetText(self.RightInfo[1], guild_id == temp.guild_id and self.data:GuildName(temp.name) or temp.name)
        UH.SetText(
            self.RightInfo[2],
            string.format(Language.GuildEmbattle.ServerName, self.data:ServerName(temp.server_id))
        )
        UH.SetText(self.RightInfo[3], temp.banner)
        self.NoneBattle:SetActive(false)
        for i = 1, 3 do
            self.RightObjs[i]:SetActive(true)
        end
    else
        self.NoneBattle:SetActive(true)
        for i = 1, 3 do
            self.RightObjs[i]:SetActive(false)
        end
        UH.SetText(self.RightInfo[1], "")
        UH.SetText(self.RightInfo[2], "")
        UH.SetText(self.RightInfo[3], "")

    end
end
function GuildEmbattleEnter:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(GuildEmbattleEnter)
end
function GuildEmbattleEnter:OnClickEnter()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
    ViewMgr:CloseView(GuildEmbattleEnter)
end
