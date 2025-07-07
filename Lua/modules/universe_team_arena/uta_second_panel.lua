-- ==================== 天下第一比武大会晋级赛 =============

UtaSecondPanel = UtaSecondPanel or DeclareMono("UtaSecondPanel", UIWFlushPanel)
function UtaSecondPanel:UtaSecondPanel()
    self.data = UniverseTeamArenaData.Instance
    self.second_list = {{second = 0},{second = 1},{second = 2},{second = 3},{second = 4}}
    self.data_cares = {
        {data = self.data.all_main_match_data, func = self.FlushMatchList, init_call = false},
        {data = self.data.base_data, func = self.FlushSecondList, init_call = false},
        {data = self.data.guedd_profit_data, func = self.FlushInfo, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushInfo, init_call = false},
        {data = self.data.remind_data, func = self.FlushRemind, init_call = false}
    }
end

function UtaSecondPanel:Awake()
    UIWFlushPanel.Awake(self)
    local other_cfg = self.data:OtherCfg()
    local start_time,end_time = self.data:GetStartAndEndTime()
    local start_time_info = TimeManager.FormatUnixTime2Date(start_time)
    local end_time_info = TimeManager.FormatUnixTime2Date(end_time)
    local time_str = string.format(Language.UTAPanel.Second.Text4, start_time_info.month, start_time_info.day, end_time_info.month, end_time_info.day)
    local tip1 = string.format(Language.UTA.Knockout.Tip1, time_str, other_cfg.need_active,other_cfg.lottery_active[0].num)
    UH.SetText(self.Tip1, tip1)
    UH.SetText(self.Tip2, Language.UTA.Knockout.Tip2)
    self:FlushPanel()
end

function UtaSecondPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetProfit)
end

function UtaSecondPanel:FlushPanel()
    self:FlushSecondList()
    self:FlushInfo()
    self:FlushMatchList()
    self:FlushRemind()
end

function UtaSecondPanel:FlushSecondList()
    self.SecondList:SetData(self.second_list)
    if not self.is_flush_second_sel then
        local sel_index = 1
        local state = self.data:ActState()
        if state == UTACfg.ActStatus.Second then
            local round_index = self.data.base_data.round_index or 0
            sel_index = round_index + 1
        end
        self.SecondList:SetectItem(sel_index, true)
        self.is_flush_second_sel = true
    end
end

function UtaSecondPanel:FlushMatchList()
    if not self.round_index then
        return
    end
    local match_list = self.data:GetMainAllMatchList(self.round_index)
    self.MatchList:SetData(match_list)
end

function UtaSecondPanel:FlushInfo()
    local item_id = self.data:GetGuessItemId()
    local item_icon = Item.GetIconId(item_id)
    UH.SetIcon(self.Icon1, item_icon, ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon2, item_icon, ICON_TYPE.ITEM)

    UH.SetText(self.Num1, Item.GetNum(item_id))
    UH.SetText(self.Num2, self.data.guedd_profit_data.num)
end

function UtaSecondPanel:FlushRemind()
    local remind_num = self.data:MainRemind()
    self.RedPoint:SetNum(remind_num)
end

function UtaSecondPanel:OnToggleClick(data)
    local round_index = data.second
    self.round_index = round_index
    UniverseTeamArenaCtrl.Instance:SendCSUTAMainSchedules(round_index)
end

function UtaSecondPanel:OnOpenPreviewClick()
    ViewMgr:OpenView(UTAAllMatch)
end

function UtaSecondPanel:OnOpenStoreClick()
    ViewMgr:OpenView(UTAGuessStore)
end

function UtaSecondPanel:OnOpenRewardClick()
    ViewMgr:OpenView(UniverseTeamArenaReward)
end

function UtaSecondPanel:OnOpenGuessRecordClick()
    ViewMgr:OpenView(UTAGuessRecord)
end

function UtaSecondPanel:OnItemIconClick()
    UniverseTeamArenaCtrl.Instance:ShowGuessItem()
end

UtaSecondPanelTabBtn = UtaSecondPanelTabBtn or DeclareMono("UtaSecondPanelTabBtn", UIWidgetBaseItem)
function UtaSecondPanelTabBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Language.UTA.Second.SecondName[data.second])
    local match_time = UniverseTeamArenaData.Instance:GetSecondTime(data.second)
    local start_time
    if data.second == 0 then
        local _,time_info = UniverseTeamArenaData.Instance:GetKnockoutTime(4)
        start_time = time_info.end_time
    else
        local _,end_time = UniverseTeamArenaData.Instance:GetSecondTime(data.second - 1)
        start_time = end_time
    end
    local time_info = TimeManager.FormatUnixTime2Date(match_time)
    local minute = time_info.minute < 10 and time_info.minute .. "0" or time_info.minute
    local time_str = string.format(Language.UTAPanel.Second.Text1, time_info.month, time_info.day, time_info.hour, minute)
    self.time_str = time_str
    UH.SetText(self.Time, time_str)

    local cur_time = TimeCtrl.Instance:GetServerTime()
    --self:SetShow(cur_time >= start_time)
    self:SetShow(UniverseTeamArenaData.Instance:IsMatchOpen(UTACfg.MatchType.MainWin, data.second))
end

function UtaSecondPanelTabBtn:SetShow(value)
    self.Interactor.Interactable = value
    self.Mask:SetActive(not value)
end

function UtaSecondPanelTabBtn:OnMaskClick()
    --PublicPopupCtrl.Instance:Center(string.format("%s时间未到，%s开启",Language.UTA.Second.SecondName[self.data.second], self.time_str))
    PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip3)
end

--比赛对阵信息
UtaSecondPanelMatchItem = UtaSecondPanelMatchItem or DeclareMono("UtaSecondPanelMatchItem", UIWidgetBaseItem)
-- data :::: UTAMatchClass
function UtaSecondPanelMatchItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.AtkTeam:SetData(data.atk_team)
    self.DefTeam:SetData(data.def_team)
    UH.SetText(self.State, Language.UTA.FightResult[data.fight_result])
    self.AtkTeam:SetWinFlag(data.fight_result == UTACfg.FightResult.AtkWin)
    self.DefTeam:SetWinFlag(data.fight_result == UTACfg.FightResult.DefWin)
    UH.SetText(self.Index, data.show_index or "")

    self.VideoInteractor.Interactable = tonumber(data.video_id) ~= 0
    self.VideoBtn1:SetActive(data.fight_result ~= UTACfg.FightResult.Fighting)
    self.VideoBtn2:SetActive(data.fight_result == UTACfg.FightResult.Fighting)

    self.MatchTypeToggle.isOn = data.match_type == UTACfg.MatchType.MainWin
    local match_text = data.match_type == UTACfg.MatchType.MainWin and Language.UTAPanel.Second.Text2 or Language.UTAPanel.Second.Text3
    UH.SetText(self.MatchTypeText, match_text)
end

function UtaSecondPanelMatchItem:OnGuessClick()
    -- 有轮空不允许竞猜
    if not self.data:IsCanGuess() then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip4)
        return
    end
    UniverseTeamArenaCtrl.Instance:ReqMatchGuessInfo(self.data)
    --ViewMgr:OpenView(UTAGuess, {match_id_t = self.data.match_id_t})
end

function UtaSecondPanelMatchItem:OnVideoClick()
    if tonumber(self.data.video_id) ~= 0 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAWatchVideo(self.data.video_id_t)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip5)
    end
end

function UtaSecondPanelMatchItem:OnWatchClick()
    UniverseTeamArenaCtrl.Instance:SendCSUTAStartOB(self.data.info.match_id_t)
end