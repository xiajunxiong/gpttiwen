DreamNotesRankView = DreamNotesRankView or DeclareView("DreamNotesRankView", "dream_notes/dream_notes_rank")

function DreamNotesRankView:DreamNotesRankView()
    self.data = DreamNotesData.Instance
end

function DreamNotesRankView:LoadCallback()
    local rank_list,role_info = DreamNotesData.Instance:GetRankList()
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)
    self.RoleItem:SetData(self:GetRoleData(role_info.rank))
end

function DreamNotesRankView:GetRoleData(rank)
    return {
        rank = rank or 0,
        rank_value = self.data:GetPassLevel(),
        role = {
            uid = RoleData.Instance:GetRoleId(),
            prof = RoleData.Instance:GetRoleProfId(),
            name = RoleData.Instance:GetRoleName(),
            avatar_type = RoleData.Instance:GetRoleAvatarType(),
            avatar_id = RoleData.Instance:GetRoleAvatarId(),
            avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
        }
    }
end

function DreamNotesRankView:OnClickClose()
    ViewMgr:CloseView(DreamNotesRankView)
end

----------------------------DreamNotesRankItem----------------------------
DreamNotesRankItem = DreamNotesRankItem or DeclareMono("DreamNotesRankItem", UIWidgetBaseItem)

function DreamNotesRankItem:SetData(data)
    if data.rank > 3 or data.rank == 0 then
        UH.SetText(self.RinkNum,data.rank == 0 and Language.CourageChallenge.NotHasRankTips or data.rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..data.rank)
    end
    self.RinkNum:SetObjActive(data.rank > 3 or data.rank == 0)
    UH.SetText(self.Name,data.role.name)
    UH.SetText(self.Level,data.rank_value ~= 0 and Format(Language.DreamNotes.Level,data.rank_value) or "--")
    UH.SetAvatar(self.Head,data.role.avatar_type,data.role.avatar_id,data.role.avatar_quality)
    UH.SetText(self.Prof,Language.Common.Prof[RoleData.Instance:GetRoleProf(data.role.prof)])
    UIWidgetBaseItem.SetData(self, data)
end

function DreamNotesRankItem:OnClickItem()
    if self.data ~= nil then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role.uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role.uid)
    end
end