-- =========== 天下第一比武大会：赛事竞猜 ==========
UTAGuess = UTAGuess or DeclareView("UTAGuess", "universe_team_arena/uta_guessing")
function UTAGuess:UTAGuess()
    
end

function UTAGuess:LoadCallback(param_t)
    local match_id_t = param_t.match_id_t
    if not TableIsEmpty(match_id_t) then
        UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetInfo, match_id_t)
    end
end

function UTAGuess:OnCloseClick()
    ViewMgr:CloseView(UTAGuess)
end

function UTAGuess:CloseCallback()
    UniverseTeamArenaData.Instance:SetGuessInfo({})
end


UTAGuessPanel = UTAGuessPanel or DeclareMono("UTAGuessPanel", UIWFlushPanel)
function UTAGuessPanel:UTAGuessPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.guess_info, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false},
    }

    -- 虚假数据
    -- local guess_info = {match_id = 10}
	-- guess_info.l_bet_info = GuessBetInfoClass.New({})
	-- guess_info.r_bet_info = GuessBetInfoClass.New({})
	-- self.data:SetGuessInfo(guess_info)

end

function UTAGuessPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function UTAGuessPanel:FlushPanel()
    self:FlushInfo()
    self:FlushBottom()
    self:FlushItem()
end

function UTAGuessPanel:FlushInfo()
    local guess_info = self.data:GetGuessInfo()
    if TableIsEmpty(guess_info) then
        return
    end
    local l_info = guess_info.l_bet_info
    local r_info = guess_info.r_bet_info
    self.LeftInfo:SetData(l_info)
    self.RightInfo:SetData(r_info)
end

function UTAGuessPanel:FlushItem()
    local item_id = self.data:GetGuessItemId()
    UH.SetIcon(self.GuessIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    UH.SetText(self.GuessNum, Item.GetNum(item_id))
end

function UTAGuessPanel:FlushBottom()
    local guess_info = self.data:GetGuessInfo()
    if TableIsEmpty(guess_info) then
        return
    end
    local red_num = guess_info.l_bet_info.bet_num
    local blue_num = guess_info.r_bet_info.bet_num
    
    UH.SetText(self.ScaleTip, string.format(Language.UTAPanel.Guess.Text1, red_num, blue_num))

    if red_num == 0 and blue_num == 0 then
        self:SetProgress(0.5)
    else
        local num = red_num / (red_num + blue_num)
        self:SetProgress(num)
    end
end

function UTAGuessPanel:SetProgress(num)
    local w = 0
    if num > 0 then
        w = 366 * num + 20
    end
    self.ProgressRed:SetSizeWithCurrentAnchors(0,w)
end


UTAGuessPanelTeamInfo = UTAGuessPanelTeamInfo or DeclareMono("UTAGuessPanelTeamInfo", UIWidgetBaseItem)
function UTAGuessPanelTeamInfo:Awake()
    local item_id = UniverseTeamArenaData.Instance:GetGuessItemId()
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end
-- data : GuessBetInfoClass
function UTAGuessPanelTeamInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.ServerName, data:GetServerName())
    UH.SetText(self.WinCount, data.win_count)
    UH.SetText(self.WinRate, data.win_rate .. "%")
    UH.SetText(self.Rate, string.format(Language.UTAPanel.Guess.Text2,data:GetRate()))
    UH.SetText(self.BetNum, data.bet_num)
    UH.SetText(self.TeamName, data:GetTeamName())
    if self.TeamName2 then
        UH.SetText(self.TeamName2, data:GetTeamName())
    end

    local prof_list = data:GetProfList()
    for i = 1, self.ProfList:Length() do
        if prof_list[i] then
            UH.SpriteName(self.ProfList[i], UTACfg.ProfIcon[prof_list[i]])
        end
    end

    local head_id = data:GetHeadId()
    UH.SetAvatarp(self.HeadIcon, data:GetAvatarType(), head_id, 0)

    local match = UniverseTeamArenaData.Instance.req_guess_match
    self.is_end = false
    if math then
        local result = match.fight_result
        -- if result == UTACfg.FightResult.AtkWin or result == UTACfg.FightResult.DefWin or result == UTACfg.FightResult.Draw then
        --     self.is_end = true
        -- end
        self.is_end = result ~= UTACfg.FightResult.Nothing
    end
    
    self.BtnInter.Interactable = self.is_end == false
end

function UTAGuessPanelTeamInfo:OnOpenInfoClick()
    local team_key = self.data:GetTeamKey()
    local team_name = self.data:GetTeamName()
    ViewMgr:OpenView(UTABattleArray, {team_id_t = team_key, team_name = team_name})
end

function UTAGuessPanelTeamInfo:OnOpenBetClick()
    if UniverseTeamArenaData.Instance.req_guess_match then
        local match = UniverseTeamArenaData.Instance.req_guess_match
        if self.is_end == true then
            PublicPopupCtrl.Instance:Center(Language.UTAPanel.Guess.Tip1)
            return
        end
        local self_team_info = UniverseTeamArenaData.Instance:SelfTeamBaseInfo()
        if self_team_info then
            if self_team_info.team_name == match.atk_team.team_name or self_team_info.team_name == match.def_team.team_name then
                PublicPopupCtrl.Instance:Center(Language.UTAPanel.Guess.Tip2)
                return
            end
        end
    end
    ViewMgr:OpenView(UTAGuessBet, {info = self.data})
end