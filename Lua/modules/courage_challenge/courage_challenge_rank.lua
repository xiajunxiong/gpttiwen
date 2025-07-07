CourageChallengeRankView = CourageChallengeRankView or DeclareView("CourageChallengeRankView", "courage_challenge/courage_challenge_rank")

function CourageChallengeRankView:CourageChallengeRankView()
    self.data = CourageChallengeData.Instance
    self.handle_care = self.data.rank_data:Care(BindTool.Bind(self.FlushRank, self))
end
function CourageChallengeRankView:FlushRank()
    if self.rank_type == nil then
        return
    end
    local rank_list = CourageChallengeData.Instance:GetRankList(self.rank_type)
    if self.pagoda_type == CourageChallengeData.PagodaType.ShiShen then
        if self.rank_type == CourageChallengeRankType.Main then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 0)
            rank_list = CourageChallengeData.Instance:GetRankList2(0)
        elseif self.rank_type == CourageChallengeRankType.Friends then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 1)
            rank_list = CourageChallengeData.Instance:GetRankList2(1)
        elseif self.rank_type == CourageChallengeRankType.Guild then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 2)
            rank_list = CourageChallengeData.Instance:GetRankList2(2)
        end
    end
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)
    self:FlushRoleItemView(self:GetRankData(rank_list) or {})
end
function CourageChallengeRankView:CloseCallback()
    if self.handle_care then
        self.data.rank_data:Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function CourageChallengeRankView:OnClickClose()
    self.rank_type = nil
    ViewMgr:CloseView(CourageChallengeRankView)
end

function CourageChallengeRankView:LoadCallback(param_t)
    self.pagoda_type = param_t.type or CourageChallengeData.PagodaType.GuMo
    self:OnClickType(0)
end

function CourageChallengeRankView:OnClickType(type)
    local rank_list = CourageChallengeData.Instance:GetRankList(type)
    if self.pagoda_type == CourageChallengeData.PagodaType.ShiShen then
        if type == CourageChallengeRankType.Main then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 0)
            rank_list = CourageChallengeData.Instance:GetRankList2(0)
        elseif type == CourageChallengeRankType.Friends then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 1)
            rank_list = CourageChallengeData.Instance:GetRankList2(1)
        elseif type == CourageChallengeRankType.Guild then
            SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE, nil, 2)
            rank_list = CourageChallengeData.Instance:GetRankList2(2)
        end
    end
    self.rank_type = type
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)
    self:FlushRoleItemView(self:GetRankData(rank_list) or {})
end

function CourageChallengeRankView:GetRankData(list)
    for k,v in pairs(list or {}) do
        if v.role_id == RoleId() then
            return v
        end
    end
    if self.pagoda_type == CourageChallengeData.PagodaType.ShiShen then
        return CourageChallengeData.Instance:GetRoleRinkInfo2()
    else
        return CourageChallengeData.Instance:GetRoleRinkInfo()
    end
end

function CourageChallengeRankView:FlushRoleItemView(info)
    self.RoleItem:SetData({
        role_id = RoleId(),
        star_num = info.star_num or 0,
        layer_level = info.layer_level or 0,
        rank = info.rank_num or info.rank or 0,
        name = RoleData.Instance:GetRoleName(),
        avatar_type = RoleData.Instance:GetRoleAvatarType(),
        avatar_id = RoleData.Instance:GetRoleAvatarId(),
        avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
    })
end

----------------------------CourageChallengeRankItem----------------------------
CourageChallengeRankItem = CourageChallengeRankItem or DeclareMono("CourageChallengeRankItem", UIWidgetBaseItem)

function CourageChallengeRankItem:SetData(data)
    local rank = data.rank_num or data.rank
    if rank > 3 or rank == 0 then
        UH.SetText(self.RinkNum,rank == 0 and Language.CourageChallenge.NotHasRankTips or rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..rank)
    end
    self.RinkNum:SetObjActive(rank > 3 or rank == 0)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.StarNum,data.star_num)
    UH.SetText(self.Level,data.layer_level .. Language.Common.Level)
    UH.SetAvatar(self.Head, data.avatar_type,data.avatar_id,data.avatar_quality)
    UIWidgetBaseItem.SetData(self, data)
end

function CourageChallengeRankItem:OnClickItem()
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role_id)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role_id)
end