RongYaoView = RongYaoView or DeclareView("RongYaoView", "guild/rongyao_view")
-- VIEW_DECLARE_MASK(RongYaoView, ViewMask.BgBlockClose)
function RongYaoView:RongYaoView()
    self.lang = Language.Guild
    self.data = GuildData.Instance
end
function RongYaoView:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end
function RongYaoView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(RongYaoView)
end
function RongYaoView:OnFlush()
    local top_info = self.data.guild_top_info
    for i = 1, 3 do
        if top_info[i] then
            UH.SetText(self.GuildNames[i], top_info[i].guild_id > 0 and top_info[i].guild_name or self.lang.RYNoGuildName)
        end
    end
    local info = self.data:GetGuildInfo()
    -- self.data:GetGuildBuff()
    local not_ry = (info.guild_buff_rank == nil or info.guild_buff_rank <= 0)
    UH.SetText(
        self.Desc,
        not_ry and self.lang.RYNoRank or
            string.format(self.lang.RYRank, info.guild_buff_rank, self.lang.RYRankLevel[info.guild_buff_rank])
    )
    for i = 1, 4 do
        UH.SetText(self.DescTexs[i], self.lang["RYDesc" .. i])
    end
    local res
    for i = 1, 3 do
        res = self.data:GetRongYaoDesc(i)
        if res and res[1] and res[2] then
            UH.SetText(
                self.BuffValues[i],
                string.format(self.lang.RYBuffDesc, res[1][1], res[1][2], res[2][1], res[2][2])
            )
        else
            UH.SetText(self.BuffValues[i], "配置有问题")
        end
    end
end
