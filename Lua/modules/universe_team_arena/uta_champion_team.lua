-- =========== 天下第一比武大会：冠军队伍 ==========
UtaChampionTeam = UtaChampionTeam or DeclareView("UtaChampionTeam", "universe_team_arena/uta_champion_team")
VIEW_DECLARE_LEVEL(UtaChampionTeam, ViewLevel.L0)
function UtaChampionTeam:OnCloseClick()
    ViewMgr:CloseView(UtaChampionTeam)
end

function UtaChampionTeam:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[312].desc)
end


UtaChampionTeamPanel = UtaChampionTeamPanel or DeclareMono("UtaChampionTeamPanel", UIWFlushPanel)
function UtaChampionTeamPanel:UtaChampionTeamPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.champion_data, func = self.FlushPanel, init_call = false},
    }
end

function UtaChampionTeamPanel:Awake()
    UIWFlushPanel.Awake(self)
    
    local begin_index = 0
    local end_index
    local status = self.data:ActState()
    if status == UTACfg.ActStatus.Pause then
        end_index = self.data.base_data.season_idx - 1
    else
        end_index = self.data.base_data.season_idx - 2
    end
    end_index = end_index < 0 and 0 or end_index

    --UniverseTeamArenaCtrl.Instance:SendCSGetUTAChampionRecord(4, 5)
    UniverseTeamArenaCtrl.Instance:SendCSGetUTAChampionRecord(begin_index, end_index)
    self.cur_sel_data = nil
    
end

function UtaChampionTeamPanel:FlushPanel()
    self:FlushTypeList()
    self:FlushMemberList()
end

function UtaChampionTeamPanel:FlushMemberList()
    if TableIsEmpty(self.cur_sel_data) then
        UH.SetText(self.CurText, "")
        UH.SetText(self.Title, "")
        return
    end
    local index = self.cur_sel_data.index
    local title = string.format(Language.UTAPanel.Text18, DataHelper.GetDaXie(index + 1))
    local str = string.format(Language.UTAPanel.Text19, DataHelper.GetDaXie(index + 1), self.cur_sel_data.team_name)
    UH.SetText(self.CurText, str)
    UH.SetText(self.Title, title)
    local member_list = self.cur_sel_data.team_info.member_list or {}
    -- 按42135顺序排列
    local list_data = {}
    list_data[1] = member_list[4] or {}
    list_data[2] = member_list[2] or {}
    list_data[3] = member_list[1] or {}
    list_data[4] = member_list[3] or {}
    list_data[5] = member_list[5] or {}
    self.MemberList:SetData(list_data)
end

function UtaChampionTeamPanel:FlushTypeList()
    local _list_data = self.data:GetChampionData() or {}
    local list_data = {}
    for i,v in pairs(_list_data) do
        table.insert(list_data,v)
    end
    table.sort(list_data, function (a,b)
        return a.index > b.index
    end)

    local len = table.nums(list_data)
    self.List:SetObjActive(len < 10)
    self.ScrollView:SetActive(len >= 10)

    if len < 10 then
        self.List:SetData(list_data)
    else
        self.List2:SetData(list_data)
    end
    self.cur_sel_data = list_data[1]
end

function UtaChampionTeamPanel:OnOpenClick()

end

function UtaChampionTeamPanel:OnSelClick(data)
    self.List:SetObjActive(false)
    self.cur_sel_data = data
    self.Toggle.isOn = false
    self:FlushMemberList()
end


UtaChampionTeamMemberItem = UtaChampionTeamMemberItem or DeclareMono("UtaChampionTeamMemberItem", UIWidgetBaseItem)
function UtaChampionTeamMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.ModelShow:SetObjActive(not TableIsEmpty(data))
    self.Prof:SetObjActive(not TableIsEmpty(data))
    if TableIsEmpty(data) then
        UH.SetText(self.ServerName, Language.UTAPanel.Text5)
        UH.SetText(self.Name, Language.UTAPanel.Text7)
        return
    end

    local apparance = UniverseTeamArenaCtrl.Instance:ChampionFormatAppearance(data)
    apparance.ride_id = 0
	apparance.bicycle_index = -1
    apparance.model_card_flag = 0
    apparance.is_use_huanshou = 0

    if not self.ui_obj then
        self.ui_obj = UIChDrawer.CreateByAppearance(apparance)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
    else
        self.ui_obj:SetData(apparance)
    end

    UH.SetText(self.Name, data.role_name)
    local server_info = LoginData.Instance:GetServerInfoById(data.server_id, data.plat_id) or {}
    UH.SetText(self.ServerName, server_info.name or "")
    UH.SpriteName(self.Prof, UTACfg.ProfIcon[data.prof_base])
end

function UtaChampionTeamMemberItem:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end


UtaChampionTeamTypeItem = UtaChampionTeamTypeItem or DeclareMono("UtaChampionTeamTypeItem", UIWidgetBaseItem)
function UtaChampionTeamTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local str = string.format(Language.UTAPanel.Text8, DataHelper.GetDaXie(data.index + 1), data.team_name)
    UH.SetText(self.Text, str)
end