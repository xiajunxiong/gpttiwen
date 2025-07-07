GuildInbornResetView = GuildInbornResetView or DeclareView("GuildInbornResetView", "guild/guild_inborn_reset", nil, ViewLayer.Top + 2)
VIEW_DECLARE_MASK(GuildInbornResetView, ViewMask.BgBlockNoAnim)

function GuildInbornResetView:GuildInbornResetView()
    self.data = GuildData.Instance
    self.language = Language.Guild.GuildInbornReset
end

function GuildInbornResetView:LoadCallback(param_t)
    self.select_data = param_t.data
    local item_list = self.data:GetInbornResetPoints(self.select_data.inborn_type, self.select_data.type_seq)
    -- LogD("item_list = ", item_list)

    self.Content.text = self.language.content_show
    -- self.ResetVal.text = string.format(self.language.back_show, points)
    self.CostVal.text = self.select_data.reset_consume
end

function GuildInbornResetView:OnClickConfirm()
    if not MallCtrl.IsNotCurrency(CurrencyType.GuildCotribution, self.select_data.reset_consume) then
        ViewMgr:CloseView(GuildInbornResetView)
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    GuildCtrl.Instance:SendGuildInbornReq(GUILD_INBORN_REQ_TYPE.TYPE_RESET_SKILL, self.select_data.inborn_type, self.select_data.type_seq)
	ViewMgr:CloseView(GuildInbornResetView)
end

function GuildInbornResetView:OnClickClose()
    ViewMgr:CloseView(GuildInbornResetView)
end