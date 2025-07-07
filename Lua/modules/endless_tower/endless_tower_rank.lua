EndlessTowerRankView = EndlessTowerRankView or DeclareView("EndlessTowerRankView", "endless_tower/endless_tower_rank")

function EndlessTowerRankView:EndlessTowerRankView()
    self.data = EndlessTowerData.Instance
end

function EndlessTowerRankView:LoadCallback(param_t)
    for i=1,self.BtnList:Length() do
        UH.SetToggle(self.BtnList[i],(param_t.open_param or 1) == i)
    end
    self:OnClickType(CourageChallengeRankType.Cross)
end

function EndlessTowerRankView:FlushRankView(type)
    local rank_list = self.data:GetRankInfo(type)
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)
    local rank,config = DataHelper.SimpleCondIndex(rank_list,function(data)
        return data.role.uid == RoleId()
    end)
    self.RoleItem:SetData(self:GetRoleData(rank,config))
end

function EndlessTowerRankView:FlushCrossView()
    local rank_list = self.data:GetRankCrossList()
    self.CrossList:SetData(rank_list)
    self.CrossNotHasTip:SetActive(#rank_list == 0)
    local rank,config = DataHelper.SimpleCondIndex(rank_list,function(data)
        return data.role.uid == RoleId()
    end)
    self.CrossItem:SetData(self:GetRoleData(rank,config))
end

function EndlessTowerRankView:OnClickType(type)
    self.data.select_rank_type = type
    if type == CourageChallengeRankType.Cross then
        self:FlushCrossView()
    else
        self:FlushRankView(type)
    end
    self.CrossGame:SetActive(type == CourageChallengeRankType.Cross)
    self.RankGame:SetActive(type ~= CourageChallengeRankType.Cross)
end

function EndlessTowerRankView:GetRoleData(rank,config)
    return {
        rank = rank or 0,
        rank_value = config and config.rank_value or self.data:GetBattleLevel(),
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

function EndlessTowerRankView:OnClickClose()
    ViewMgr:CloseView(EndlessTowerRankView)
end

----------------------------EndlessTowerRankItem----------------------------
EndlessTowerRankItem = EndlessTowerRankItem or DeclareMono("EndlessTowerRankItem", UIWidgetBaseItem)

function EndlessTowerRankItem:SetData(data)
    local rank = data.rank_num or data.rank
    if rank > 3 or rank == 0 then
        UH.SetText(self.RinkNum,rank == 0 and Language.CourageChallenge.NotHasRankTips or rank)
    else
        UH.SpriteName(self.ImgNum,"paiming_"..rank)
    end
    self.RinkNum:SetObjActive(rank > 3 or rank == 0)
    UH.SetText(self.Name,data.role.name)
    UH.SetText(self.Level,data.rank_value ~= 0 and Format(Language.RushDungeon.LevelName,data.rank_value) or "--")
    UH.SetAvatar(self.Head,data.role.avatar_type,data.role.avatar_id,data.role.avatar_quality)
    UH.SetText(self.Prof,Language.Common.Prof[RoleData.Instance:GetRoleProf(data.role.prof)])

    if self.Grid and self.RewardTip then
        local reward_data = EndlessTowerData.Instance:GetRewardRankData(rank)
        self.Grid:SetData(DataHelper.FormatItemList(reward_data or {}))
        self.RewardTip:SetActive(reward_data == nil)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function EndlessTowerRankItem:OnClickItem()
    if self.data ~= nil then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role.uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role.uid)
    end
end

--===========================EndlessTowerRewardPanel===========================
EndlessTowerRewardPanel = EndlessTowerRewardPanel or DeclareMono("EndlessTowerRewardPanel", UIWFlushPanel)

function EndlessTowerRewardPanel:EndlessTowerRewardPanel()
    self.data = EndlessTowerData.Instance
end

function EndlessTowerRewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetData(self.data:GetRewardConfig())
    self.Item:SetData(self.data:GetCrossRoleRankData())
end

----------------------------EndlessTowerRewardItem----------------------------
EndlessTowerRewardItem = EndlessTowerRewardItem or DeclareMono("EndlessTowerRewardItem", UIWidgetBaseItem)

function EndlessTowerRewardItem:SetData(data)
    local rank = data.rank
    if rank ~= nil then
        if rank > 3 or rank == 0 then
            UH.SetText(self.RinkNum,rank == 0 and Language.CourageChallenge.NotHasRankTip or rank)
        else
            UH.SpriteName(self.ImgNum,"paiming_"..rank)
        end
        self.RinkNum:SetObjActive(rank > 3 or rank == 0)
    else
        if data.min_rank == data.max_rank then
            UH.SpriteName(self.ImgNum,"paiming_".. data.min_rank)
        else
            UH.SetText(self.RinkNum,Format(Language.EndlessTower.RankName,data.min_rank,data.max_rank))
        end
        self.RinkNum:SetObjActive(data.min_rank ~= data.max_rank)
    end
    self.Grid:SetData(DataHelper.FormatItemList(data.reward or {}))
    self.RewardTip:SetActive(data.reward == nil)
end