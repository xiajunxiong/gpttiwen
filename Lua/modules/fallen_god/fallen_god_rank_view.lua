FallenGodRankView = FallenGodRankView or DeclareView("FallenGodRankView", "fallen_god/fallen_god_rank")
VIEW_DECLARE_MASK(FallenGodRankView, ViewMask.Block)

function FallenGodRankView:LoadCallback()
    local rank_list = FallenGodData.Instance:GetRankList()
    self.List:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)
    local rank, config = DataHelper.SimpleCondIndex(rank_list, function(data)
        return data.role.uid == RoleId()
    end)
    self.RoleItem:SetData(self:GetRoleData(config))
    UH.SetIcon(self.TitleImg, FallenGodData.Instance:GetTitleName())
end

function FallenGodRankView:GetRoleData(config)
    if config then
        return config
    end
    local info = {
        show_rank = 0,
        role = {
            uid = RoleData.Instance:GetRoleId(),
            prof = RoleData.Instance:GetRoleProfId(),
            name = RoleData.Instance:GetRoleName(),
            avatar_type = RoleData.Instance:GetRoleAvatarType(),
            avatar_id = RoleData.Instance:GetRoleAvatarId(),
            avatar_quality = RoleData.Instance:GetRoleAvatarQuality()
        }
    }
    info.boss_sn = FallenGodData.Instance:GetPassBossSn()
    if info.boss_sn ~= -1 then
        local boss_info = FallenGodData.Instance:GetBossInfo(info.boss_sn)
        info.round_num = boss_info.pass_round_num
    end
    return info
end

function FallenGodRankView:OnClickClose()
    ViewMgr:CloseView(FallenGodRankView)
end

FallenGodRankItem = FallenGodRankItem or DeclareMono("FallenGodRankItem", UIWidgetBaseItem)
function FallenGodRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local rank = data.show_rank
    local is_str = rank > 3 or rank == 0
    self.RinkNum:SetObjActive(is_str)
    self.ImgNum:SetObjActive(not is_str)
    if is_str then
        UH.SetText(self.RinkNum, rank == 0 and Language.CourageChallenge.NotHasRankTips or rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..rank)
    end

    UH.SetText(self.Name, data.role.name)
    if data.boss_sn == -1 then
        UH.SetText(self.BossName, "--")
        UH.SetText(self.Round, "--")
    else
        local boss_cfg = FallenGodData.Instance:GetBossCfg(data.boss_sn)
        UH.SetText(self.BossName, boss_cfg.name)
        UH.SetText(self.Round, data.round_num)
    end
end
