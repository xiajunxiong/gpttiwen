GuildApply = GuildApply or DeclareView("GuildApply", "guild/guild_apply")
function GuildApply:GuildApply()
    self.lang = Language.Guild
end

function GuildApply:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab8, Vector2.New(905, 498))
    --自动批准
    local guild_info = GuildData.Instance:GetGuildInfo()
    self.CheckItem:SetData(guild_info.is_need_approval == 0)
end

function GuildApply:CloseCallback()
    local check = self.CheckItem:GetCheck()
    local guild_info = GuildData.Instance:GetGuildInfo()
    local is_need_approval = check and 0 or 1
    if guild_info.is_need_approval ~= is_need_approval then
        GuildCtrl.SendGuildNeedApproveOp(is_need_approval)
    end
end

GuildApplyPanel = GuildApplyPanel or DeclareMono("GuildApplyPanel", UIWFlushPanel)
function GuildApplyPanel:GuildApplyPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.apply_list, func = self.FlushApplyList}
    }
end

function GuildApplyPanel:FlushApplyList()
    self.data.apply_count.count = 0
    self.ApplyList:SetData(self.data:GetApplyList())
end

function GuildApplyPanel:OnClickOneKey(result)
    local param_t = {}
    local guild_id = self.data:GetGuildID()
    for k, v in pairs(self.data:GetApplyList()) do
        param_t = {
            guild_id = guild_id,
            uid = v.uid,
            result = result
        }
        GuildCtrl.SendApplyJoinGuildAck(param_t)
    end
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_APPLY_LIST})
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
end

----------------------------GuildApplyItem----------------------------
GuildApplyItem = GuildApplyItem or DeclareMono("GuildApplyItem", UIWidgetBaseItem)
function GuildApplyItem:GuildApplyItem()
    self.guild_data = GuildData.Instance
end

function GuildApplyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    self.AvatarCell:SetAvatar(data.avatar_type,data.avatar_id,data.avatar_quality)
    if data.top_level and data.top_level > 0 then
		UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, data.top_level))
		self.PeakLevel:SetActive(true)
	else
		UH.SetText(self.Level, string.format("Lv.%d", data.level))
		self.PeakLevel:SetActive(false)
	end
	AvatarEffect.SetImageEnabled(self.PeakImg,data.avatar_quality)
end

function GuildApplyItem:OnClickOk(result)
    local param_t = {
        guild_id = self.guild_data:GetGuildID(),
        uid = self.data.uid,
        result = result
    }
    GuildCtrl.SendApplyJoinGuildAck(param_t)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_APPLY_LIST})
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
end

----------------------------GuildApplyCheckItem----------------------------
GuildApplyCheckItem = GuildApplyCheckItem or DeclareMono("GuildApplyCheckItem", UIWidgetBaseItem)

function GuildApplyCheckItem:SetData(data)
    self.Check:SetActive(data)
end

function GuildApplyCheckItem:OnClickCheck()
    local info = GuildData.Instance:GetJobInfo()
    if info.approval_auto == 0 then
        PublicPopupCtrl.Instance:Center(Language.Guild.GuildApplyTip)
        return
    end
    local active = self.Check.activeSelf
    self.Check:SetActive(not active)
end

function GuildApplyCheckItem:GetCheck()
    return self.Check.activeSelf
end