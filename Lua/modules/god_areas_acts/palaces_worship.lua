PalacesWorship = PalacesWorship or DeclareView("PalacesWorship", "god_areas_acts/palaces_worship", Mod.GodAreasActs.PalacesHall)
function PalacesWorship:PalacesWorship()
    self.data = GodAreasActsData.Instance
end

function PalacesWorship:LoadCallback()
    GodAreasActsCtrl.Instance:GodSpaceOperatePalaceRank(self.data:SeasonId())
end

PalacesWorshipPanel = PalacesWorshipPanel or DeclareMono("PalacesWorshipPanel", UIWFlushPanel)
function PalacesWorshipPanel:PalacesWorshipPanel()
    self.data = GodAreasActsData.Instance
    self.data_cares = {
        {data = self.data:GetPalacesInfo(), func = self.FlushWorship},
        {data = self.data:GetPalacesInfo(), func = self.FlushModel, keys = {"flush_flag"}},
    }
end
function PalacesWorshipPanel:TestInfo()
    local t = {}
    local appearance = RoleData.Instance:GetApearance()
    t.role_uuid = 0
	t.role_name = RoleData.Instance:GetRoleName()
	t.space_id = 0
	t.server_id = LoginData.Instance:ServerName()
	t.season_id = self.data:SeasonId()

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
	t.season_contribute_value = 10000
    return t
end
function PalacesWorshipPanel:FlushModel()
    local season_id = self.data:SeasonId()
    -- local info = self:TestInfo()
    -- 根据赛季显示当前排第一的B
    local rank_list = self.data:GetPalaceRankList(season_id)
    if rank_list[1] then
        local info = rank_list[1]
        if not self.ui_obj then
            self.ui_obj = UIChDrawer.New()-- UIChDrawer.CopyMainRoleData()
            self.ModelShow:SetShowData(
                {
                    ui_obj = self.ui_obj,
                    view = self
                }
            )
        else 
            self.ui_obj:SetData(info)--RoleData.Instance:GetApearance())
        end
        UH.SetText(self.Name, info.role_name)
        UH.SetText(self.Title, string.format(Language.GodAreasActs.PalaceTitle, DataHelper.GetDaXie(season_id)))
        UH.SetText(self.Server, string.format(Language.GodAreasActs.PalaceServer, RuinsData.Instance:ServerName(info.server_id)))
    else
        UH.SetText(self.Name, "")--Language.GodAreasActs.NoName)
        UH.SetText(self.Title, Language.GodAreasActs.NoName)
        UH.SetText(self.Server, "")
        LogDG("当前排行榜没有数据模型不显示")
    end
end
function PalacesWorshipPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function PalacesWorshipPanel:FlushWorship()
    local is_worship = self.data:GetWorshipFlag()
    self.IsGet:SetActive(is_worship)
    self:FlushModel()
end

function PalacesWorshipPanel:OnClickClose()
    ViewMgr:CloseView(PalacesWorship)
end
function PalacesWorshipPanel:OnClickRank()
    GodAreasActsCtrl.Instance:GodSpaceOperatePalaceRank(self.data:SeasonId())
    ViewMgr:OpenView(PalacesHistoryRank)
end
function PalacesWorshipPanel:OnClickGoto()
    GodAreasActsCtrl.Instance:SendEnterPalace()
end
function PalacesWorshipPanel:OnClickWorship()
    local is_worship = self.data:GetWorshipFlag()
    if is_worship == false then
        GodAreasActsCtrl.Instance:GodSpaceOperateWorship()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.WorshipTip)
    end
end


MainRightPalacesHistory = MainRightPalacesHistory or DeclareMono("MainRightPalacesHistory", UIWFlushPanel)
function MainRightPalacesHistory:MainRightPalacesHistory()
    self.data = GodAreasActsData.Instance
    self.data_cares = {
        {data = self.data:GetPalacesInfo(), func = self.OnEnable}
    }
end

function MainRightPalacesHistory:TestInfo(id)
    local t = {}
    local appearance = RoleData.Instance:GetApearance()
    t.role_uuid = 0
	t.role_name = RoleData.Instance:GetRoleName()
	t.space_id = 0
	t.server_id = LoginData.Instance:ServerName()
	t.season_id = id

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
	t.season_contribute_value = 10000
    return t
end

function MainRightPalacesHistory:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local data = self.data:GetStatusList()
    if table.nums(data) == 0 then
        LogDG("当前雕像列表为空")
    end
    --[[ local temp = {}
    for i = 1, 10, 1 do
        temp[i] = self:TestInfo(i)
    end
    data = temp ]]
    --当前雕像列表
    self.List:SetData(data)
end
function MainRightPalacesHistory:OnClickHide()
    self.Anim:Play("main_hide")
    self:OnEnable()
end
function MainRightPalacesHistory:OnClickShow()
    self.Anim:Play("main_show")
    self:OnEnable()
end


MainRightPalacesHistoryItem = MainRightPalacesHistoryItem or DeclareMono("MainRightPalacesHistoryItem", UIWidgetBaseItem)
function MainRightPalacesHistoryItem:MainRightPalacesHistoryItem()
    self.act_data = GodAreasActsData.Instance
end
function MainRightPalacesHistoryItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Head:SetData(data)
    UH.SetText(self.Name, data.role_name)
    self.Head:SetData(data)
    self.Head:SetClickFunc(BindTool.Bind(self.OnClickItem, self))
end

function MainRightPalacesHistoryItem:OnClickItem()
    --移动到对应位置即可
    local cfg = self.act_data:GetPalaceStatus(self.data.season_id)
    if cfg then
        SceneLogic.Instance:AutoToPos(SceneData.Instance:SceneId(), {x = cfg.pos_x, y = cfg.pos_z})
    else
        LogDG("获取不到赛季为", self.data.season_id,"的配置")
    end
end

