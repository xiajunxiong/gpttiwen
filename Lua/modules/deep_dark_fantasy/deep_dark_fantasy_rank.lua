DeepDarkFantasyRankView = DeepDarkFantasyRankView or DeclareView("DeepDarkFantasyRankView", "deep_dark_fantasy/deep_dark_fantasy_rank")
function DeepDarkFantasyRankView:DeepDarkFantasyRankView()
	self.AudioOpen = CommonAudio.view_open_l2
    self.AudioClose = CommonAudio.ViewCloseL1
end

function DeepDarkFantasyRankView:OnClickClose()
    ViewMgr:CloseView(DeepDarkFantasyRankView)
end

function DeepDarkFantasyRankView:LoadCallback()
    self:OnClickType(0)
end

function DeepDarkFantasyRankView:OnClickType(type)
    SocietyCtrl.Instance:SendGetPersonRankList(DeepDarkFantasyData.RANK_TYPE,nil,type)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

DeepDarkFantasyRankPanel = DeepDarkFantasyRankPanel or DeclareMono("DeepDarkFantasyRankPanel", UIWFlushPanel)
function DeepDarkFantasyRankPanel:DeepDarkFantasyRankPanel()
    self.data = DeepDarkFantasyData.Instance
    self.ctrl = DeepDarkFantasyCtrl.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.onFlush ,keys = {"flush_num"},init_call = false},
    }

end

function DeepDarkFantasyRankPanel:onFlush()
    local is_had_guild =  GuildData.Instance:GetGuildID() > 0
    local list = self.data.rank_data.rank_infos
    local is_guild = self.data.rank_data.special_type > 1

    self.List:SetData(list)
    self.NotHasTip:SetActive(#list == 0 and (not is_guild or (is_guild and is_had_guild)))
    self.NotGuildTip:SetActive(not is_had_guild and is_guild)

    -- self.RoleItem.gameObject:SetActive(#list > 0)
    local rank,info = self:GetRankData(list)
    self:FlushRoleItemView(info)
end

function DeepDarkFantasyRankPanel:GetRankData(list)
    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(list or {}) do
        if v.role.uid == role_id then
            return k,v
        end
    end
end

function DeepDarkFantasyRankPanel:FlushRoleItemView(info)
    if info == nil then 
        self.RoleItem:SetData({
            rank = 0,
            rank_value = self.data:GetMyAllBox(),
            flexible_int = self.data.challenge_data.pass,
            role = RoleData.Instance:GetRoleData()
        })
    else 
        self.RoleItem:SetData(info)
    end 
end

----------------------------DeepDarkFantasyRankItem----------------------------
DeepDarkFantasyRankItem = DeepDarkFantasyRankItem or DeclareMono("DeepDarkFantasyRankItem", UIWidgetBaseItem)

function DeepDarkFantasyRankItem:SetData(data)
    if data.rank > 3 or data.rank == 0 then
        UH.SetText(self.RinkNum,data.rank == 0 and Language.CourageChallenge.NotHasRankTips or data.rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..data.rank)
    end
    self.RinkNum:SetObjActive(data.rank > 3 or data.rank == 0)
    UH.SetText(self.Name,data.role.name)
    UH.SetText(self.StarNum,data.rank_value)
    UH.SetText(self.Level,data.flexible_int .. Language.DeepDarkFantasy.Chapter)
    UH.SetAvatar(self.Head, data.role.avatar_type,data.role.avatar_id,data.role.avatar_quality)
    UIWidgetBaseItem.SetData(self, data)
end

function DeepDarkFantasyRankItem:OnClickItem()
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role.uid)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role.uid)
end