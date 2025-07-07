GuildPost = GuildPost or DeclareView("GuildPost", "guild/guild_post")
function GuildPost:GuildPost()
    self.lang = Language.Guild
end

function GuildPost:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab11)
end

function GuildPost:CloseCallback()
end

GuildPostPanel = GuildPostPanel or DeclareMono("GuildPostPanel", UIWFlushPanel)
function GuildPostPanel:GuildPostPanel()
    self.data = GuildData.Instance
end
function GuildPostPanel:onFlush()
    local data =  self.data:GetPostList()
    self.List:SetData(data)
    self.select_data = self.data:GetSelectMember()
    UH.SetText(self.Title,string.format(Language.Guild.PostName, self.select_data.name))
end
function GuildPostPanel:OnClickApply()
    self.post_info = self.data:GetSelectPost()
    if self.post_info == nil then
        return
    end
    self.select_data = self.data:GetSelectMember()
    if self.select_data == nil then
        return
    end
    local param_t = {
        guild_id = self.data:GetGuildID(),
        uid = self.select_data.uid,
        post = self.post_info.member_type
    }
    if self.post_info.member_type == 0 then
        PublicPopupCtrl.Instance:DialogTips({
            content = Language.Guild.ZhuanRang,
            confirm = {
                func = function ()
                    GuildCtrl.SendGuildAbdicationOp(self.select_data.uid)
                    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        })
        ViewMgr:CloseView(GuildPost)
        return
    else
        GuildCtrl.SendGuildAppointmentOp(param_t)
    end
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
    ViewMgr:CloseView(GuildPost)
end

GuildPostItem = GuildPostItem or DeclareMono("GuildPostItem", UIWidgetBaseItem)
function GuildPostItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.member_name)
end
function GuildPostItem:OnClickSet()
    -- LogError(self.data)
    GuildData.Instance:SetSelectPost(self.data)
end