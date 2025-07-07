-- =========== 天下第一比武大会：赛事竞猜投注 ==========
UTAGuessBet = UTAGuessBet or DeclareView("UTAGuessBet", "universe_team_arena/uta_guessing_bet")
function UTAGuessBet:UTAGuessBet()
    self.Board:SetData(self:GetType(),Language.UTA.Title1, Vector2.New(638, 424))
    self.data = UniverseTeamArenaData.Instance
end

function UTAGuessBet:LoadCallback(param_t)
    param_t = param_t or {}
    self.info = param_t.info
    self.has_num = self.data:GetGuessCoinNum()
    self.num = 0
    UH.SetText(self.CoinTip, "/" .. self.has_num)
    self:SetNum(self.num)

    local item_id = self.data:GetGuessItemId()
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end

function UTAGuessBet:SetNum(num)
    num = num or 0
    if num < 0 then
        num = 0
    end

    if num > self.has_num then
        num = self.has_num
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Bet.Tip1)
    end

    local player_bet_num = self.info.player_bet_num or 0
    local max_num = Config.universe_arena_auto.other[1].once_lottery_limit
    if max_num and (num + player_bet_num) > max_num then
        num = max_num - player_bet_num
        --PublicPopupCtrl.Instance:Center(string.format(Language.UTAPanel.Bet.Tip3, max_num))
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Bet.Tip4)
    end

    self.num = num
    UH.SetText(self.Input, self.num)
end

function UTAGuessBet:OnAddClick(num)
    num = self.num + num
    self:SetNum(num)
end

function UTAGuessBet:OnClearClick()
    self:SetNum(0)
end

function UTAGuessBet:OnInput()
    local value = tonumber(self.Input.text)
    self:SetNum(value)
end

function UTAGuessBet:OnBtnClick()
    if self.num > 0 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.Bet, self.info.match_id_t, self.info.team_id_t, self.num)
        ViewMgr:CloseView(UTAGuessBet)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Bet.Tip2)
    end
    if self.has_num == 0 then
        UniverseTeamArenaCtrl.Instance:ShowGuessItem()
    end
end