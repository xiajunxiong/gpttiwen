PalacesHistoryRank = PalacesHistoryRank or DeclareView("PalacesHistoryRank", "god_areas_acts/palaces_history_rank")
function PalacesHistoryRank:PalacesHistoryRank()
    
end

function PalacesHistoryRank:LoadCallback()
    
end

PalacesHistoryRankPanel = PalacesHistoryRankPanel or DeclareMono("PalacesHistoryRankPanel", UIWFlushPanel)
function PalacesHistoryRankPanel:PalacesHistoryRankPanel()
    self.data = GodAreasActsData.Instance
    self.data_cares = {
        {data = self.data:GetPalacesInfo(), func = self.FlushRank}
    }
end
function PalacesHistoryRankPanel:TestInfo(index, season_id)
    local t = {}
    t.rank = index
    local appearance = RoleData.Instance:GetApearance()
    t.role_uuid = 0
	t.role_name = RoleData.Instance:GetRoleName()..season_id
	t.space_id = 0
	t.server_id = LoginData.Instance:ServerName()
	t.season_id = season_id--self.data:SeasonId()

	t.level = RoleData.Instance:GetRoleLevel()
	t.top_level = RoleData.Instance:GetTopLevel()
	t.prof_base = appearance.prof_base
	t.advance_time = appearance.advance_time    --		    //!< 进阶次数[0,6]
	t.avatar_type  = appearance.avatar_type         --		    //!< 角色模型[1,6]
	t.color = appearance.color                 --  		//!< 颜色[0,4)
	t.weapon_id = appearance.weapon_id           --		    //!< 武器ID
	t.sub_weapon_id =appearance.sub_weapon_id        --	        //!< 副武器ID
	t.headshot_id = appearance.avatar_id          --		    //!< 头像ID 
    t.avatar_id = t.headshot_id
	t.fly_flag= appearance.fly_flag              --		    //!< 飞升阶段
	t.fashion_body_id = appearance.fashion_body_id or 0-- //!< 幻化时装
	t.fashion_weapon_id = appearance.fashion_weapon_id or 0--	//!< 幻化武器
	t.season_contribute_value = 10000 + index
    return t
end
function PalacesHistoryRankPanel:FlushRank()
    --根据当前赛季显示排行榜
    local season_id = self.data:SeasonId()
    local select_id = self.data.select_season_id
    local data
    if select_id == 0 and select_id ~= season_id then
        data = self.data:GetPalaceRankList(season_id)
        self.data:SetSelectSeason(season_id)
        select_id = season_id
    else
        data = self.data:GetPalaceRankList(select_id)
    end
    --[[ local temp = {}
    for i = 1, 10, 1 do
       temp[i] = self:TestInfo(i,select_id)
    end
    data = temp ]]
    self.List:SetData(data)
    UH.SetText(self.RankName, string.format(Language.GodAreasActs.PalaceSeason, DataHelper.GetDaXie(select_id)))
    --根据当前赛季反推有多少季
    self.SeasonList:SetData(self.data:SeasonList())
end
function PalacesHistoryRankPanel:SetParentActive()
    self.HistoryParent:SetActive(false)
end
function PalacesHistoryRankPanel:OnClicHistory()
    --假如只有1季就提示return
    if self.HistoryParent.activeSelf == false then
        self.HistoryParent:SetActive(true)
    else
        self.HistoryParent:SetActive(false)
    end
end
function PalacesHistoryRankPanel:OnClickColorBtn(data)
    GodAreasActsCtrl.Instance:GodSpaceOperatePalaceRank(data.season_id)
    self.data:SetSelectSeason(data.season_id)
    --请求排行榜
    self:FlushRank()
    self:OnClicHistory()
end
function PalacesHistoryRankPanel:OnClickClose()
    ViewMgr:CloseView(PalacesHistoryRank)
end
function PalacesHistoryRankPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelectSeason(0)
end
PalacesHistoryRankItem = PalacesHistoryRankItem or DeclareMono("PalacesHistoryRankItem", UIWidgetBaseItem)
function PalacesHistoryRankItem:PalacesHistoryRankItem()
    self.rank_sp = {
        "DiYiMing", "DiErMing", "DiSanMing", "DiNMingDi"
    }
end
function PalacesHistoryRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.role_name)
    UH.SetText(self.ZhanGong, data.season_contribute_value)
    if data.rank < 4 then
        UH.SpriteName(self.RankSp, self.rank_sp[data.rank])
        UH.SetText(self.RankNum, "")
    else
        UH.SpriteName(self.RankSp, self.rank_sp[4])
        UH.SetText(self.RankNum, data.rank)
        -- self.RankSp:SetObjActive(false)
    end
    self.RankSp:SetObjActive(true)
end

PalacesHistorySeasonItem = PalacesHistorySeasonItem or DeclareMono("PalacesHistorySeasonItem", UIWidgetBaseItem)
function PalacesHistorySeasonItem:PalacesHistorySeasonItem()
    
end
function PalacesHistorySeasonItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, string.format(Language.GodAreasActs.PalaceSeason, DataHelper.GetDaXie(data.season_id)))
end