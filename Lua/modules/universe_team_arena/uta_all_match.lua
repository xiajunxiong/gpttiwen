-- =========== 天下第一比武大会：赛程总览
UTAAllMatch = UTAAllMatch or DeclareView("UTAAllMatch", "universe_team_arena/uta_all_match")
VIEW_DECLARE_LEVEL(UTAAllMatch, ViewLevel.L0)
function UTAAllMatch:OnCloseClick()
    ViewMgr:CloseView(UTAAllMatch)
end

function UTAAllMatch:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[206].desc)
end

function UTAAllMatch:CloseCallback()
    UniverseTeamArenaData.Instance.is_look_last_all_match = false
end


UTAAllMatchPanel = UTAAllMatchPanel or DeclareMono("UTAAllMatchPanel", UIWFlushPanel)
function UTAAllMatchPanel:UTAAllMatchPanel()
    self.data = UniverseTeamArenaData.Instance
    self.cfg = {
        -- 胜者组
        [1] = {
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.TOP32},
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.TOP16},
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.TOP8_TO_FINAL},
        },
        -- 败者组
        [2] = {
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.FAIL_GROUP_1},
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.FAIL_GROUP_2},
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.FAIL_GROUP_3},
            {main_type = UTACfg.MAIN_OVERVIEW_TYPE.FAIL_GROUP_4},
        },
    }

    self.data_cares = {
        {data = self.data.all_match_preview_data, func = self.FlushMatchList, init_call = false}
    }

    self.tab_index = 1
end

function UTAAllMatchPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function UTAAllMatchPanel:FlushPanel()
    local cfg = UTACfg.MAIN_TYPE_CONVERT[UTACfg.MAIN_OVERVIEW_TYPE.FAIL_GROUP_1]
    local is_unlock = UniverseTeamArenaData.Instance.is_look_last_all_match or self.data:IsMatchOpen(cfg.match_type, cfg.round_index)
    self.TabMask:SetActive(not is_unlock)
    self.TabInter2.Interactable = is_unlock
    self:FlushMatchTypeList()
end

function UTAAllMatchPanel:FlushMatchTypeList()
    local list_data = self.cfg[self.tab_index]
    self.MatchTypeList:SetData(list_data)
    local sel_index = 1
    if UniverseTeamArenaData.Instance.is_look_last_all_match then
        sel_index = 3
    end
    self.MatchTypeList:SetectItem(sel_index, true)
end

function UTAAllMatchPanel:FlushMatchList()
    if not self.overview_type then
        return
    end
    local list_data = self.data:GetPreviewAllMatchList(self.overview_type)
    local is_nil = TableIsEmpty(list_data)
    self.Panel1:SetObjActive(not is_nil and self.overview_type ~= UTACfg.MAIN_OVERVIEW_TYPE.TOP8_TO_FINAL)
    self.Panel2:SetObjActive(not is_nil and self.overview_type == UTACfg.MAIN_OVERVIEW_TYPE.TOP8_TO_FINAL)
    if self.overview_type == UTACfg.MAIN_OVERVIEW_TYPE.TOP8_TO_FINAL then
        self.Panel2:SetListData(list_data)
    else
        self.Panel1:SetListData(list_data)
    end
end

function UTAAllMatchPanel:OnTabClick(tab_index)
    self.tab_index = tab_index
    self.MatchTypeList:SetData({})
    self:FlushMatchTypeList()
end

function UTAAllMatchPanel:OnMatchTypeClick(data)
    self.overview_type = data.main_type
    local status = self.data:ActState()
    local is_last = 0
    if status ~= UTACfg.ActStatus.Pause then
        is_last = UniverseTeamArenaData.Instance.is_look_last_all_match and 1 or 0
    end
    UniverseTeamArenaCtrl.Instance:SendCSUTAMainSchedulesOverview(self.overview_type,is_last)
    self:FlushMatchList()
end

function UTAAllMatchPanel:OnTab2MaskClick()
    PublicPopupCtrl.Instance:Center(Language.UTAPanel.AllMatch.Tip1)
end


UTAAllMatchPanelCommon = UTAAllMatchPanelCommon or DeclareMono("UTAAllMatchPanelCommon")
function UTAAllMatchPanelCommon:SetListData(list_data)
    self.List:SetData(list_data)
end

UTAAllMatchPanelSpecial = UTAAllMatchPanelSpecial or DeclareMono("UTAAllMatchPanelSpecial")
function UTAAllMatchPanelSpecial:SetListData(list_data)
    for i = 1, self.ItemList:Length() do
        local is_nil = TableIsEmpty(list_data[i])
        self.ItemList[i]:SetObjActive(not is_nil)
        if not is_nil then
            self.ItemList[i]:SetData(list_data[i])
        end
    end
end



UTAAllMatchTypeItem = UTAAllMatchTypeItem or DeclareMono("UTAAllMatchTypeItem", UIWidgetBaseItem)
function UTAAllMatchTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local main_type = data.main_type
    UH.SetText(self.Text, Language.UTA.MainMatchType[data.main_type])
    local cfg = UTACfg.MAIN_TYPE_CONVERT[main_type]
    local is_lock = UniverseTeamArenaData.Instance.is_look_last_all_match or UniverseTeamArenaData.Instance:IsMatchOpen(cfg.match_type, cfg.round_index)
    self.Mask:SetActive(not is_lock)
    self.Inter.Interactable = is_lock
end

function UTAAllMatchTypeItem:OnMaskClick()
    PublicPopupCtrl.Instance:Center(Language.UTAPanel.AllMatch.Tip2)
end

UTAAllMatchItem = UTAAllMatchItem or DeclareMono("UTAAllMatchItem", UIWidgetBaseItem)
-- data = 5009 协议 SCUTAMainOverviewSchedules
function UTAAllMatchItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_all_null = tonumber(data.def_team_id) == 0 and tonumber(data.atk_team_id) == 0
    if is_all_null then
        data.fight_result = 0
    end

    self.RedWin:SetActive(data.fight_result ~= 2)
    self.BlueWin:SetActive(data.fight_result == 2)
    self.LWinFlag:SetActive(data.fight_result == 1)
    self.RWinFlag:SetActive(data.fight_result == 2)

    UH.SetText(self.LName, data.atk_team_name)
    UH.SetText(self.RName, data.def_team_name)

    self.VFlag2:SetActive(data.fight_result == 3)
    self.Interactor.Interactable = data.fight_result == 3 or tonumber(data.video_id) ~= 0
end

function UTAAllMatchItem:OnVideoClick()
    if self.data.fight_result == 3 then
        -- 实时观战
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.AllMatch.Tip3)
    elseif tonumber(self.data.video_id) ~= 0 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAWatchVideo(self.data.video_id_t)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.AllMatch.Tip4)
    end
end