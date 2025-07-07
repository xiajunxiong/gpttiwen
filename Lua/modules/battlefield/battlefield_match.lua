BattlefieldMatch = BattlefieldMatch or DeclareView("BattlefieldMatch", "battlefield/battlefield_match",Mod.Battlefield.Main)

VIEW_DECLARE_LEVEL(BattlefieldMatch,ViewLevel.L0)

function BattlefieldMatch:BattlefieldMatch()
    self.data = BattlefieldData.Instance
end

function BattlefieldMatch:OpenCallback()
    BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.WIN)
end

function BattlefieldMatch:CloseCallback()
    RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Society, self.handle)
    if TeamCtrl.Instance:Data():InTeam() and TeamCtrl.Instance:Data():IsLeader()then
        BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NOTICE,0)
    end
end

function BattlefieldMatch:LoadCallback()
    self:FlushTimeView()
    self:FlushRemindView()
end

function BattlefieldMatch:OnClickAnother()
    ViewMgr:OpenView(AnotherView)
end

function BattlefieldMatch:OnClickSociety()
    ViewMgr:OpenView(SocietyView)
end

function BattlefieldMatch:OnClickShop()
    ViewMgr:OpenView(BattlefieldShopView)
end

function BattlefieldMatch:OnClickRank()
    ViewMgr:OpenView(BattlefieldRankView,BattlefieldRankType.Cross)
end

function BattlefieldMatch:OnClickReward()
    ViewMgr:OpenView(BattlefieldRewardView)
end

function BattlefieldMatch:OnClickReport()
    ViewMgr:OpenView(BattlefieldReportView)
end

function BattlefieldMatch:OnClickHelp()
    ViewMgr:OpenView(BattlefieldRankingView)
end

function BattlefieldMatch:OnClickClose()
    if TeamCtrl.Instance:Data():InTeam() and not TeamCtrl.Instance:Data():IsLeader()then
        PublicPopupCtrl.Instance:AlertTip(Language.Battlefield.ExitTeamTip,function()
            TeamCtrl.Instance:SendExitTeam()
            self:CloseMatchView()
        end)
        return
    end
    if self.data:GetMatch() == 1 and self.MatchTip.activeSelf then
        PublicPopupCtrl.Instance:AlertTip(Language.Battlefield.PopupTip,
        function()
            BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NO_MATCH)
            self:CloseMatchView()
        end)
        return
    end
    self:CloseMatchView()
end

function BattlefieldMatch:CloseMatchView()
    ViewMgr:CloseView(BattlefieldMatch)
end

function BattlefieldMatch:FlushTimeView()
    local TimeTips = Language.Battlefield.TimeTips
    local season = BattlefieldData.Instance:GetSeasonNum()
    UH.SetText(self.Season,Format(TimeTips[4],DataHelper.GetDaXie(season)))
    self.Time:SetShowCallBack(function(t)return self.data:GetConditionTimeTip(t)end)
    self.Time:StampTime(self.data:GetStampTime(),nil,TimeTips[3])
end

function BattlefieldMatch:FlushRemindView()
    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.SocietyBtn, Vector3.New(26,26,0))
	end
    self.handle = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Society,function()
        self.red_point:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(Mod.Society))
    end, true)
end

--===========================BattlefieldMatchPanel===========================
BattlefieldMatchPanel = BattlefieldMatchPanel or DeclareMono("BattlefieldMatchPanel", UIWFlushPanel)

function BattlefieldMatchPanel:BattlefieldMatchPanel()
    self.data = BattlefieldData.Instance
    self.data_cares = {
        {data = self.data.match_data, func = self.FlushMatchView,init_call = false},
        {data = self.data.result_data,func = self.FlushPanelView,init_call = true},
        {data = TeamData.Instance.team_info,func = self.FlushTeamView,init_call = true},
        {data = self.data.role_info_data,func = self.FlushTeamListView,init_call = false},
        {data = RoleData.Instance.base_data, func = self.FlushTeamListView, init_call = false,keys = {"is_leader"}},
        -- {data = self.data.result_data,func = self.FlushGoldView,init_call = true, keys = {"day_money_num"}},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,keys = {"act_type", "status"}},
    }
end

function BattlefieldMatchPanel:OnFlush()
    self.LimitTime:SetShowCallBack(function(t)return self.data:GetConditionTopTimeTips(t)end)
    if ActivityData.IsOpen(ActType.Battlefield) then
        self.LimitTime:CreateActTime(ActType.Battlefield,nil,Language.Battlefield.LimitTimeTip)
    else
        self.LimitTime:SetTime(self.data:GetTopConditionTip())
    end
    UH.SetInter(self.BtnInter,ActivityData.IsOpen(ActType.Battlefield))
end

function BattlefieldMatchPanel:FlushPanelView()
    if self.data:GetTopRace() == 0 then
        self:FlushTopRaceView()
    else
        self:FlushRankView()
    end
    self:FlushWinInfoView()
    self:FlushProgressView()
end

function BattlefieldMatchPanel:OnClickMath()
    if self.data:GetMatch() == 0 then
        if TeamCtrl.Instance:Data():InTeam() and not TeamCtrl.Instance:Data():IsLeader()then
			PublicPopupCtrl.Instance:Center(ErrorText[178])
            return
        end
        BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.MATCH)
    else
        BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NO_MATCH)
        self.MatchTime:CloseCountDownTime()
    end
end

function BattlefieldMatchPanel:FlushRankView()
    local ranking = self.data:GetRankNum() or 1
    local cur_config = self.data:GetRankConfig(ranking)
    local next_config = self.data:GetRankConfig(ranking + 1) or cur_config

    for i=1,self.StarList:Length() do
        self.StarList[i]:SetActive(i <= cur_config.level)
    end
    self.StarParent:SetActive(cur_config.level ~= 0)
    UH.SpriteName(self.Rank,"Rank" .. cur_config.rank)
    UH.SetText(self.Ranking,BattlefieldData.QuaName(cur_config))

    local cur_score = self.data:GetRankScore()
    local curr_score = cur_score - cur_config.score
    local next_scroe = next_config.score - cur_config.score
    next_scroe = next_scroe <= 0 and curr_score or next_scroe
    local IntegralTip = Language.Battlefield.IntegralTip
    self.Progress:SetProgress(curr_score / next_scroe)
    self.Progress:SetText(Format(IntegralTip,cur_score,next_config.score))
    -- self:FlushRewardView(cur_config.ranking,next_config.ranking)
end

--刷新匹配
function BattlefieldMatchPanel:FlushMatchView()
    local is_match = self.data:GetMatch()
    if is_match == 1 then
        self.MatchTime:CreateRunTime(1)
    end
    self.MatchTip:SetActive(is_match ~= 0)
    UH.SetText(self.MatchName,Language.Battlefield.MatchList[is_match])
end

--刷新战绩
function BattlefieldMatchPanel:FlushWinInfoView()
    local win_info = self.data:GetRanksInfo()
    local win_times = self.data:GetWinStreakTimes()
    local tolal_times = win_info.tolal_times or 0
    local win_info_times = win_info.win_times or 0
    local draw_times = win_info.draw_times or 0
    local win_rate = tolal_times == 0 and 0 or math.floor(win_info_times / tolal_times * 100)
    UH.SetText(self.WinNum,Format(Language.Battlefield.WinNumTip,win_times))
    UH.SetText(self.WinRate,Format(Language.Battlefield.WinRateTip,win_rate))
    local lose_times = tolal_times - win_info_times - draw_times
    UH.SetText(self.WinDesc,Format(Language.Battlefield.WinDescTip,win_info_times,lose_times,draw_times))
    local is_top_race = self.data:GetTopRace()
    self.BottomRect.sizeDelta = Vector2.New(692,is_top_race == 0 and 156 or 128)
end

--刷新奖励
-- function BattlefieldMatchPanel:FlushRewardView(ranking,next_ranking)
--     local reward_config = self.data:GetRewardConfig(next_ranking)
--     if reward_config ~= nil then
--         self.Grid:SetData(reward_config.reward_list)
--         if #reward_config.reward_list > 3 then
--             self.Content.pivot = Vector2.New(0,0.5)
--         end
--         UH.SetText(self.Title,reward_config.ranking_list .. Language.Battlefield.RewardTip)
--     end
--     self.Reward:SetActive(reward_config ~= nil)
-- end

--刷新幻界古币
-- function BattlefieldMatchPanel:FlushGoldView()
--     local gold_num = self.data:GetDayMoneyNum()
--     local limit_num = self.data:GetOtherConfig().day_money_limit
--     gold_num = Valve(gold_num,limit_num)
--     self.GoldProgress:SetProgress(gold_num / limit_num)
--     self.GoldProgress:SetText(Format("%s/%s",ColorStr(gold_num,COLORSTR.Yellow1),limit_num))
-- end

--===========================定位赛阶段===========================
function BattlefieldMatchPanel:FlushTopRaceView()
    self.RankTip:SetActive(true)
    local win_info = self.data:GetRanksInfo()
    UH.SetText(self.Ranking,ColorStr(Language.Battlefield.TopRaceTip,COLORSTR.Green4))
    local max_race = self.data:GetOtherConfig().positioning_race
    local tolal_times = win_info.tolal_times or 0
    self.Progress:SetProgress(tolal_times / max_race)
    self.Progress:SetText(Format(Language.Battlefield.ProgressTip,tolal_times,max_race))
end

function BattlefieldMatchPanel:FlushTeamView()
    BattlefieldCtrl.Instance:CheckMemberList(true)
    self:FlushTeamListView()
end

--刷新队伍信息
function BattlefieldMatchPanel:FlushTeamListView()
    local member_list = self.data:GetMemberList()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(member_list[i])
    end
    if self.member_num and self.member_num ~= #member_list then
        if TeamCtrl.Instance:Data():InTeam() and TeamCtrl.Instance:Data():IsLeader()then
            BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NOTICE,1)
        end
        if self.data:GetMatch() == 1 and self.MatchTip.activeSelf then
            BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.NO_MATCH)
            self.MatchTime:CloseCountDownTime()
        end
    end
    self.member_num = #member_list
end

function BattlefieldMatchPanel:OnClickInvite()
	ViewMgr:OpenView(TeamInviteView)
end

--刷新宝箱奖励
function BattlefieldMatchPanel:FlushProgressView()
    local win_times = self.data:GetTodayWinStreakTimes()
    local reward_list,list = self.data:GetRewardData()
    local amount_num = self:GetProgressNum(win_times,list)
    self.RewardProgress:SetProgress(amount_num)
    self.RewardProgress:SetText(win_times)
    self.RewardGrid:SetData(reward_list)
    self.ShowTimer = Invoke(function()
        for k,item in pairs(self.RewardGrid.item_list) do
            if item then
                item:SetParent()
            end
        end
    end,0.5)
end

function BattlefieldMatchPanel:GetProgressNum(num,list)
    local amount_num = (DataHelper.GetFillAmount(num,list))
    return amount_num <= 0.25 and 0 or amount_num
end

function BattlefieldMatchPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
end

----------------------------BattlefieldMatchRewardItem----------------------------
BattlefieldMatchRewardItem = BattlefieldMatchRewardItem or DeclareMono("BattlefieldMatchRewardItem", UIWidgetBaseItem)

function BattlefieldMatchRewardItem:SetData(data)
    local win_times = BattlefieldData.Instance:GetTodayWinStreakTimes()
    UH.SetText(self.Num,data.victory_field .. Language.Battlefield.MatchTitle)
    local reward_flag = BattlefieldData.Instance:GetRewardFlag(data.seq)
    self.Block:SetActive(reward_flag == 0 and win_times >= data.victory_field)
    UH.SpriteName(self.Image,data.seq >= 3 and "final_box" or "excellent_box")
    UH.SetInter(self.Inter,reward_flag == 0)
    if data.victory_field == 1 then
        UH.SetText(self.Num,Language.Battlefield.MatchFirstWin)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldMatchRewardItem:SetParent()
    if GetActive(self.Block) then
        self.Effect:SetParent(self.Parent)
    end
    self.Effect:SetActive(GetActive(self.Block))
end

function BattlefieldMatchRewardItem:OnClickItem()
    local pos = UH.GetRootPosition(self.gameObject)
    ViewMgr:OpenView(SilkRoadPreview,{
        is_finish = BattlefieldData.Instance:GetRewardFlag(self.data.seq) == 1,
        reward_list = self:GetRewardList(self.data),
        pos = Vector3.New(pos.x - 350,pos.y,0)
    })
end

function BattlefieldMatchRewardItem:OnClickReward()
    BattlefieldCtrl.Instance:SendBattlefieldReq(BattlefieldConfig.BATTLEFIELD_REQ_TYPE.REWARD,self.data.seq)
end

function BattlefieldMatchRewardItem:GetRewardList(data)
    local list = {}
    for i,v in ipairs(ActiveConfig.RewardKey) do
        local value = data[v.key] or 0
        if value ~= 0 then
            table.insert(list,{item_id = v.item_id,num = value})
        end
    end
    for k,v in pairs(data.reward) do
        table.insert(list,v)
    end
    if data.coin and data.coin ~= 0 then
        table.insert(list,{item_id = CommonItemId.Coin,num = data.coin})
    end
    return DataHelper.FormatItemList(list)
end