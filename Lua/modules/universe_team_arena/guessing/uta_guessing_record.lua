-- =========== 天下第一比武大会：赛事竞猜记录 ==========
UTAGuessRecord = UTAGuessRecord or DeclareView("UTAGuessRecord", "universe_team_arena/uta_guessing_record")
function UTAGuessRecord:OnCloseClick()
    ViewMgr:CloseView(UTAGuessRecord)
end

function UTAGuessRecord:CloseCallback()
    UniverseTeamArenaData.Instance:SetGuessInfo({})
end

UTAGuessRecordPanel = UTAGuessRecordPanel or DeclareMono("UTAGuessRecordPanel", UIWFlushPanel)
function UTAGuessRecordPanel:UTAGuessRecordPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.guess_bet_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false},
    }
end

function UTAGuessRecordPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item_id = self.data:GetGuessItemId()
    local icon = Item.GetIconId(item_id)
    UH.SetIcon(self.ItemIcon1, icon)
    UH.SetIcon(self.ItemIcon2, icon)
    -- 打开先请求下注信息
    UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetRecord)
    self:FlushPanel()
end

function UTAGuessRecordPanel:FlushPanel()
    self:FlushItem()
    local list_data = self.data:GetMatchBetRecordList()
    self.List:SetData(list_data)
    local is_nothing = TableIsEmpty(list_data)
    self.NothingObj:SetActive(is_nothing)
    local all_num = self:GetNum(list_data)
    self.all_num = all_num
    UH.SetText(self.ItemNum2, all_num)
    self.AllBtnInter.Interactable = self.all_num > 0
    --self.RedPoint:SetNum(self.all_num > 0 and 1 or 0)
    if self.all_num > 0 then
        self.data.remind_data.can_guess_num = 1
    else
        self.data.remind_data.can_guess_num = 0
    end
end

function UTAGuessRecordPanel:FlushItem()
    local item_id = self.data:GetGuessItemId()
    UH.SetText(self.ItemNum1, Item.GetNum(item_id))
end

-- 未领取的总竞猜币
function UTAGuessRecordPanel:GetNum(list_data)
    local num = 0
    for i,v in pairs(list_data or {}) do
        if v.is_get == 0 then
            local _num = UTAGuessRecordPanelItem.GetWinNum(v)
            num = num + _num
        end
    end
    return num
end

function UTAGuessRecordPanel:OnAllGetClick()
    if self.all_num > 0 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetReward, -1)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.BetRecord.Tip1)
    end
end

function UTAGuessRecordPanel:OnItemIconClick()
    UniverseTeamArenaCtrl.Instance:ShowGuessItem()
end

UTAGuessRecordPanelItem = UTAGuessRecordPanelItem or DeclareMono("UTAGuessRecordPanelItem", UIWidgetBaseItem)
function UTAGuessRecordPanelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Index, data.index)
    local result = data.fight_result
    local state_str = ""
    if result == UTACfg.FightResult.Nothing then
        state_str = Language.UTAPanel.Text1
    elseif result == UTACfg.FightResult.Fighting then
        state_str = Language.UTAPanel.Text2
    else
        state_str = Language.UTAPanel.Text3
    end
    UH.SetText(self.StateText, state_str)
    local round_index = data.round_index
    if data.match_type == UTACfg.MatchType.Out then
        round_index = UniverseTeamArenaData.Instance:ConvertRoundIndex(round_index)
    end
    local match_name = UniverseTeamArenaData.Instance:FormatMatchName(data.match_type, round_index)
    UH.SetText(self.MatchName, match_name)
    local flag = self:IsCanGet()
    UH.SetText(self.BtnText, flag == -1 and Language.UTAPanel.Guess.Btn1 or Language.UTAPanel.Guess.Btn2)
    self.Inter.Interactable = flag == 1

    self.State1:SetActive(flag == 0)
    local win_num = UTAGuessRecordPanelItem.GetWinNum(self.data)
    self.State2:SetActive(win_num == 0 and flag ~= 0)
    self.State3:SetActive(win_num > 0)
    if win_num > 0 then
        local item_id = UniverseTeamArenaData.Instance:GetGuessItemId()
        UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
        UH.SetText(self.WinNum, win_num)
    end

    self.LTeamInfo:SetData(data.atk_team_info)
    self.RTeamInfo:SetData(data.def_team_info)
end

function UTAGuessRecordPanelItem:IsCanGet()
    if self.data.is_get == 1 then
        return -1
    end
    local result = self.data.fight_result
    if result == UTACfg.FightResult.Nothing or result == UTACfg.FightResult.Fighting then
        return 0
    end
    local win_num = UTAGuessRecordPanelItem.GetWinNum(self.data)
    if win_num < 1 then
        return -2
    end
    return 1
end

function UTAGuessRecordPanelItem.GetWinNum(data)
    local result = data.fight_result
    local num = 0
    if result == UTACfg.FightResult.AtkWin then
        num = data.atk_team_info.bet_num * (data.atk_team_info.rate / 100)
    elseif result == UTACfg.FightResult.DefWin then
        num = data.def_team_info.bet_num * (data.def_team_info.rate / 100)
    elseif result == UTACfg.FightResult.Draw then
        num = data.atk_team_info.bet_num + data.def_team_info.bet_num
    end
    return math.floor(num)
end

function UTAGuessRecordPanelItem:OnGetClick()
    local flag = self:IsCanGet()
    if flag == 1 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetReward, self.data.match_id_t)
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.BetRecord.Tip2)
    elseif flag == -2 then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.BetRecord.Tip4)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.BetRecord.Tip3)
    end
end


UTAGuessRecordPanelItemInfo = UTAGuessRecordPanelItemInfo or DeclareMono("UTAGuessRecordPanelItemInfo", UIWidgetBaseItem)
function UTAGuessRecordPanelItemInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetAvatarp(self.HeadIcon,data.avatar_type,data.head_id,0)
    local flag_name
    if data.fight_result == 1 then
        flag_name = data._type == 1 and "_LocSheng" or "_LocBai"
    elseif data.fight_result == 2 then
        flag_name = data._type == 1 and "_LocBai" or "_LocSheng"
    elseif data.fight_result == 4 then
        flag_name = "_LocPing"
    end
    UH.SpriteName(self.WinFlag, flag_name)
    UH.SetText(self.TeamName, data.team_name)
    local item_id = UniverseTeamArenaData.Instance:GetGuessItemId()
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    UH.SetText(self.CurrNum, data.bet_num)
    UH.SetText(self.Rate, data.rate / 100 .. Language.UTAPanel.BetRecord.Text1)
end