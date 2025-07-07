GuildLetterView = GuildLetterView or DeclareView("GuildLetterView", "guild/guild_letter")
VIEW_DECLARE_MASK(GuildLetterView, ViewMask.BgBlock)

function GuildLetterView:LoadCallback()
    local recommend_info = GuildData.Instance:GetRecommendInfo()
    UH.SetText(self.NameText, string.format(Language.Guild.GuildLetterName, recommend_info.guild_name or "", recommend_info.role_name or ""))
end

function GuildLetterView:OnClickClose()
    ViewMgr:CloseView(GuildLetterView)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.REFUSE_GUILD_RECOMMEND})
end

function GuildLetterView:OnClickOk()
    ViewMgr:CloseView(GuildLetterView)
    local recommend_info = GuildData.Instance:GetRecommendInfo()
    if recommend_info.guild_id == nil then
        return
    end
    --先退出当前公会
    if GuildData.Instance:GetGuildID() ~= 0 then
        local member_vo = GuildData.Instance:GetMemberVo()
        local member_count = GuildData.Instance:GetMemberInfo().member_count
        if member_vo ~= nil and member_vo.post == 0 and member_count > 1 then
            PublicPopupCtrl.Instance:Center(ErrorText[30])
            return
        end
        if GuildData.Instance:HasPetOnTraining() then
            PublicPopupCtrl.Instance:Center(ErrorText[163])
            return
        end
        GuildCtrl.SendQuitGuild(GuildData.Instance:GetGuildID())
    end
    GuildCtrl.SendApplyJoinGuildReq(recommend_info.guild_id)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
end
