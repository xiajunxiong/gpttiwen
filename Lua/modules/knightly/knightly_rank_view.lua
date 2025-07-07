KnightlyRankView = KnightlyRankView or DeclareView("KnightlyRankView", KnightlyConfig.ResPath .. "knightly_rank", Mod.Knight.Rank)

function KnightlyRankView:OnClickClose()
    ViewMgr:CloseView(KnightlyRankView)
end

function KnightlyRankView:LoadCallback()
end

KnightlyRankViewF = KnightlyRankViewF or DeclareMono("KnightlyRankViewF", UIWFlushPanel)

function KnightlyRankViewF:KnightlyRankViewF()
	self.data = KnightlyCtrl.Instance:Data()
    self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Knightly

	self.data_cares = {
		{data = self.data.rank_info, func = self.FlushShow},
	}

    self:OnClickType(0)
end

function KnightlyRankViewF:FlushShow(type)
    local rank_list = self.data.rank_info:Val()
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)

    local rank,info = self:GetRankData(rank_list)
    self:FlushRoleItemView(rank,info or {})
end

function KnightlyRankViewF:GetRankData(list)
    local role_id = RoleData.Instance:GetRoleId()
    for k, v in pairs(list or {}) do
        if v.role_id == role_id then
            return k, v
        end
    end
end

function KnightlyRankViewF:FlushRoleItemView(rank,info)
    local knightly_info = self.team_data:GetKnightlyInfo()
    self.RoleItem:SetData({
        rank = rank or 0,
        role_id = RoleData.Instance:GetRoleId(),
        name = RoleData.Instance:GetRoleName(),
        avatar_type = RoleData.Instance:GetRoleAvatarType(),
        avatar_id = RoleData.Instance:GetRoleAvatarId(),
        avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
        prof = RoleData.Instance:GetRoleProfId(),
        week_chivalrous = knightly_info.today or 0,
    })
end

function KnightlyRankViewF:OnClickType(type)
    self.data.rank_type = type
    KnightlyCtrl.Instance:SendChivalrousRankListReq()
end

----------------------------KnightlyRankViewItem----------------------------
KnightlyRankViewItem = KnightlyRankViewItem or DeclareMono("KnightlyRankViewItem", UIWidgetBaseItem)

function KnightlyRankViewItem:SetData(data)
    if data.rank > 3 or data.rank == 0 then
        UH.SetText(self.RinkNum,data.rank == 0 and Language.CourageChallenge.NotHasRankTips or data.rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..data.rank)
    end
    self.RinkNum:SetObjActive(data.rank > 3 or data.rank == 0)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.StarNum, Language.Common.Prof[RoleData.ProfIdToProfType(data.prof)])
    UH.SetText(self.Level,data.week_chivalrous)
    UH.SetAvatar(self.Head, data.avatar_type,data.avatar_id,data.avatar_quality)
    UIWidgetBaseItem.SetData(self, data)
end

function KnightlyRankViewItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role_id)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role_id)
end