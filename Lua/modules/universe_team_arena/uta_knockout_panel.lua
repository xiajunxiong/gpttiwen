
-- =============== 天下第一比武大会淘汰赛 ==============
UTAKnockoutPanel = UTAKnockoutPanel or DeclareMono("UTAKnockoutPanel", UIWFlushPanel)
function UTAKnockoutPanel:UTAKnockoutPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.remind_data, func = self.FlushRemind, init_call = false}
    }
end

function UTAKnockoutPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTop()
    self:FlushRemind()
end

function UTAKnockoutPanel:FlushTop()
    local self_team_info = self.data:SelfTeamBaseInfo()
    local is_show_toggle1 = self_team_info ~= nil
    self.TabItem1:SetShow(is_show_toggle1)
    self.Toggle1.isOn = is_show_toggle1
    self.Toggle2.isOn = not is_show_toggle1
end

function UTAKnockoutPanel:FlushRemind()
    local remind_num = self.data:OutRemind() or 0
    self.RedPoint:SetNum(remind_num)
end

function UTAKnockoutPanel:OnStoreClick()
    ViewMgr:OpenView(UTAGuessStore)
end



-- ======== 本战队赛程列表 ========
UTASelfTeamInfoPart = UTASelfTeamInfoPart or DeclareMono("UTASelfTeamInfoPart", UIWFlushPanel)
function UTASelfTeamInfoPart:UTASelfTeamInfoPart()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.self_team_base_data, func = self.FlushBaseInfo, init_call = false},
        {data = self.data.self_out_match_list, func = self.FlushMatchList, init_call = false},
        {data = self.data.base_data, func = self.FlushReq, init_call = false},
    }
end

function UTASelfTeamInfoPart:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function UTASelfTeamInfoPart:FlushReq()
    -- 请求我的战队基本信息
    UniverseTeamArenaCtrl.Instance:SendCSUTAMyTeamQualificationScheduleReq()
end

function UTASelfTeamInfoPart:FlushPanel()
    self:FlushBaseInfo()
    self:FlushMatchList()
end

-- 刷新基本信息
function UTASelfTeamInfoPart:FlushBaseInfo()
    local info = self.data.self_team_base_data
    UH.SetText(self.DayCount, string.format(Language.UTAPanel.Knockout.Text1,info.today_reward_num, info.today_total_reward_num))
    UH.SetText(self.WinCount, string.format(Language.UTAPanel.Knockout.Text2,info.win_num))
    -- 参与奖励
    local reward_list_data_1 = DataHelper.FormatItemList(Config.universe_arena_auto.other[1].join_reward)
    self.RewardList1:SetData(reward_list_data_1)
    local max_count = self:MaxWinCount()
    local progress_num = info.win_num / max_count
    self.ProgressBar:SetProgress(progress_num)

    UH.SetText(self.TeamName, info.team_name)
    UH.SetText(self.Score, info.qualification_score)
    UH.SetText(self.Rank, string.format(Language.UTAPanel.Text6, info.cur_rank))
    UH.SetText(self.WinFail, string.format("%s/%s/%s", info.win_num, info.lose_num,info.draw_num))

    self.BoxList:SetData(Config.universe_arena_auto.victory_reward)
end

function UTASelfTeamInfoPart:FlushMatchList()
    local match_list = self.data:SelfOutMatchList() or {}
    self.MatchList:SetData(match_list)
end

function UTASelfTeamInfoPart:OnRewardBoxClick(data)
    local flag = UniverseTeamArenaData.Instance:GetOutBoxFlag(data)
    if flag == 1 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAWinBox(data.sn)
    end
    self.BoxInfo:Open(data.sn)
end

function UTASelfTeamInfoPart:MaxWinCount()
    local num = Config.universe_arena_auto.victory_reward[#Config.universe_arena_auto.victory_reward].victory_times
    return num
end

UTASelfTeamInfoPartRoleInfo = UTASelfTeamInfoPartRoleInfo or DeclareMono("UTASelfTeamInfoPartRoleInfo", UIWidgetBaseItem)
-- data = 5003协议 SCUTAMyTeamQualificationSchedules
function UTASelfTeamInfoPartRoleInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local round_idx = UniverseTeamArenaData.Instance:ConvertRoundIndex(data.round_idx)
    UH.SetText(self.RoundText, string.format(Language.UTAPanel.Knockout.Text3, DataHelper.GetDaXie(round_idx + 1)))
    --UH.SetText(self.Time,)
    UH.SetText(self.Name, data.team_name)
    local server_info = LoginData.Instance:GetServerInfoById(data.server_id) or {}
    local server_name = server_info.name or Language.UTAPanel.Knockout.Text4
    UH.SetText(self.ServerName, string.format(Language.UTAPanel.Knockout.Text5,server_name))

    local match_time_info_1, match_time_info_2= UniverseTeamArenaData.Instance:GetKnockoutTime(round_idx)
    local match_time_info = data.session == 0 and match_time_info_1 or match_time_info_2
    self.match_time_info = match_time_info

    
    local time_stamp = match_time_info.start_time

    local time_info = TimeManager.FormatUnixTime2Date(time_stamp)
    local minute = time_info.minute < 10 and time_info.minute .. "0" or time_info.minute
    local time_str = string.format(Language.UTAPanel.Knockout.Text6,time_info.month,time_info.day, time_info.hour, minute)
    UH.SetText(self.TimeText,time_str)
    self.DayFlag:SetActive(now_day == time_info.day)

    local team_id = tonumber(data.team_id)
    self.Info:SetActive(team_id ~= 0)
    if team_id == 0 then
        return
    end

    self.ResultFlag:SetObjActive(data.fight_result > 0)
    if data.fight_result > 0 then
        local flag_name
        if data.fight_result == 1 then
            flag_name = "_LocSheng"
        elseif data.fight_result == 2 then
            flag_name = "_LocBai"
        elseif data.fight_result == 3 then
            flag_name = "_LocQi"
        else
            flag_name = "_LocPing"
        end
        UH.SpriteName(self.ResultFlag, flag_name)
        local score = UniverseTeamArenaData.Instance:GetOutWinScore(data.fight_result)
        local score_str = data.fight_result == 3 and string.format(Language.UTAPanel.Knockout.Text7, score) or string.format(Language.UTAPanel.Knockout.Text8,score)
        local color = score > 0 and COLORSTR.Green4 or COLORSTR.Red8
        UH.SetText(self.Score, ColorStr(score_str, color))
    else
        UH.SetText(self.Score, "")
    end
    --UH.SetText(self.LastScore, string.format(Language.UTAPanel.Knockout.Text9, "暂无"))

    local now_time_stamp = TimeCtrl.Instance:GetServerTime()
    local now_time_info = TimeManager.FormatUnixTime2Date(now_time_stamp)
    local now_day = now_time_info.day

    local is_can = UniverseTeamArenaData.Instance:IsMatchOpen(UTACfg.MatchType.Out, data.round_idx)
    self.GoToBtn:SetActive(data.fight_result == 0 and is_can == true)

    UH.SetAvatarp(self.HeadIcon,data.avatar_type,data.head_id,0)
end

function UTASelfTeamInfoPartRoleInfo:OnLookInfoClick()
    ViewMgr:OpenView(UTABattleArray,{team_id_t = self.data.team_id_t, team_name = self.data.team_name})
end

function UTASelfTeamInfoPartRoleInfo:OnGotoMatchClick()
    -- local now_time_stamp = TimeCtrl.Instance:GetServerTime()
    -- if now_time_stamp < self.match_time_info.start_time then
    --     PublicPopupCtrl.Instance:Center("比赛时间未到")
    --     return
    -- end
    -- if now_time_stamp > self.match_time_info.end_time then
    --     PublicPopupCtrl.Instance:Center("比赛时间已结束")
    --     return
    -- end
    local is_can = UniverseTeamArenaData.Instance:IsMatchOpen(UTACfg.MatchType.Out, self.data.round_idx)
    if is_can then
        ViewMgr:CloseView(UniverseTeamArenaView)
        ViewMgr:CloseView(UniverseTeamArenaMain)
        SceneLogic.Instance:AutoToNpc(UniverseTeamArenaData.Instance:MainNpcId(), nil, false, true)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Knockout.Tip1)
    end
end

UTASelfTeamInfoPartBoxItem = UTASelfTeamInfoPartBoxItem or DeclareMono("UTASelfTeamInfoPartBoxItem", UIWidgetBaseItem)
function UTASelfTeamInfoPartBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Text, string.format(Language.UTAPanel.Knockout.Text10, data.victory_times))
    local flag = UniverseTeamArenaData.Instance:GetOutBoxFlag(data)
    self.On:SetActive(flag == 0)
    self.Off:SetActive(flag == -1)
    self.Can:SetActive(flag == 1)
    self.Yuan:SetActive(flag == -1 or flag == 1)
end


UTASelfTeamInfoPartBoxInfo = UTASelfTeamInfoPartBoxInfo or DeclareMono("UTASelfTeamInfoPartBoxInfo")
function UTASelfTeamInfoPartBoxInfo:UTASelfTeamInfoPartBoxInfo()
    self.pos_list = {
        [1] = Vector3.New(75,0,0),
        [2] = Vector3.New(192,0,0),
        [3] = Vector3.New(323,0,0),
        [4] = Vector3.New(350,0,0),
    }
end

function UTASelfTeamInfoPartBoxInfo:Open(index)
    self:SetObjActive(true)
    local cfg = Config.universe_arena_auto.victory_reward[index]
    local reward_list = DataHelper.FormatItemList(cfg.reward)
    self.RewardList:SetData(reward_list)
    local title = string.format(Language.UTAPanel.Knockout.Text11, cfg.victory_times)
    UH.SetText(self.Title, title)
    self.BG:SetLocalPosition(self.pos_list[index])
end

function UTASelfTeamInfoPartBoxInfo:Close()
    self:SetObjActive(false)
end

function UTASelfTeamInfoPartBoxInfo:OnBlockClick()
    self:Close()
end

-- ======== 全服战队赛程列表&竞猜 ========
UTAAllTeamInfoPart = UTAAllTeamInfoPart or DeclareMono("UTAAllTeamInfoPart", UIWFlushPanel)
function UTAAllTeamInfoPart:UTAAllTeamInfoPart()
    self.data = UniverseTeamArenaData.Instance
    self.knockout_list = {{knockout = 0},{knockout = 1},{knockout = 2},{knockout = 3},{knockout = 4}}
    self.sel_knockout = nil

    self.data_cares = {
        {data = self.data.all_out_match_data, func = self.FlushMatchList, init_call = false},
        {data = self.data.base_data, func = self.FlushReq, init_call = false},
        {data = self.data.guedd_profit_data, func = self.FlushBottom, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushBottom, init_call = false},
        {data = self.data.remind_data, func = self.FlushRemind, init_call = false}
    }
end

function UTAAllTeamInfoPart:Awake()
    UIWFlushPanel.Awake(self)
    local other_cfg = self.data:OtherCfg()
    local start_time,end_time = self.data:GetStartAndEndTime()
    start_time = self.data:GuessOpenTime()
    local start_time_info = TimeManager.FormatUnixTime2Date(start_time)
    local end_time_info = TimeManager.FormatUnixTime2Date(end_time)
    local time_str = string.format(Language.UTAPanel.Knockout.Text12, start_time_info.month, start_time_info.day, end_time_info.month, end_time_info.day)
    local tip1 = string.format(Language.UTA.Knockout.Tip1, time_str, other_cfg.need_active,other_cfg.lottery_active[0].num)
    UH.SetText(self.Tip1, tip1)
    UH.SetText(self.Tip2, Language.UTA.Knockout.Tip2)

    self.ToggleList:SetData(self.knockout_list)
    local sel_index = 1
    local state = self.data:ActState()
    if state == UTACfg.ActStatus.Knockout then
        local round_index = self.data:ConvertRoundIndex(self.data.base_data.round_index) or 0
        sel_index = round_index + 1
    end
    self.ToggleList:SetectItem(sel_index,true)

    self:FlushPanel()
end

function UTAAllTeamInfoPart:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UniverseTeamArenaCtrl.Instance:SendCSUTAGuessReq(UTACfg.GuessReqType.GetProfit)
end

function UTAAllTeamInfoPart:FlushReq()
    if not self.sel_knockout then
        return
    end
    UniverseTeamArenaCtrl.Instance:SendCSUTAQualificationSchedules(self.sel_knockout)
    self:FlushRemind()
end

function UTAAllTeamInfoPart:FlushPanel()
    self:FlushBottom()
    self:FlushSelfInfo()
    self:FlushRemind()
end

function UTAAllTeamInfoPart:FlushSelfInfo()
    local self_team_info = self.data:SelfTeamBaseInfo()
    self.SelfInfoObj:SetActive(self_team_info ~= nil)
    UH.SetText(self.NothingText, self_team_info ~= nil and "" or Language.UTA.Knockout.Tip5)
    if self_team_info then
        UH.SetText(self.TeamName, self_team_info.team_name)
        UH.SetText(self.Score, self_team_info.qualification_score)
        UH.SetText(self.Rank, string.format(Language.UTAPanel.Text6, self_team_info.cur_rank))
        UH.SetText(self.WinFail, string.format("%s/%s", self_team_info.win_num, self_team_info.lose_num))
    end
end

function UTAAllTeamInfoPart:FlushBottom()
    local item_id = self.data:GetGuessItemId()
    local item_icon = Item.GetIconId(item_id)
    UH.SetIcon(self.ItemIcon1, item_icon, ICON_TYPE.ITEM)
    UH.SetIcon(self.ItemIcon2, item_icon, ICON_TYPE.ITEM)

    UH.SetText(self.ItemNum1, Item.GetNum(item_id))
    UH.SetText(self.ItemNum2, self.data.guedd_profit_data.num)
end

function UTAAllTeamInfoPart:FlushRemind()
    local remind_num = self.data:OutRemind()
    self.RedPoint:SetNum(remind_num)
end

function UTAAllTeamInfoPart:FlushMatchList()
    if not self.sel_knockout then
        return
    end
    local list_data = self.data:GetKnockoutAllMatchList(self.sel_knockout)
    self.MatchList:SetData(list_data)
end

function UTAAllTeamInfoPart:OnRecordClick()
    ViewMgr:OpenView(UTAGuessRecord)
end

function UTAAllTeamInfoPart:OnToggleClick(data)
    self.sel_knockout = data.knockout
    UniverseTeamArenaCtrl.Instance:SendCSUTAQualificationSchedules(self.sel_knockout)
    self:FlushMatchList()
end

function UTAAllTeamInfoPart:OnItemIconClick()
    UniverseTeamArenaCtrl.Instance:ShowGuessItem()
end

-- 比赛时间信息
UTAAllTeamInfoPartTime = UTAAllTeamInfoPartTime or DeclareMono("UTAAllTeamInfoPartTime", UIWidgetBaseItem)
function UTAAllTeamInfoPartTime:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, string.format(Language.UTAPanel.Knockout.Text3, DataHelper.GetDaXie(data.knockout + 1)))
    local time_info1, time_info2 = UniverseTeamArenaData.Instance:GetKnockoutTime(data.knockout)
    local start_info1 = TimeManager.FormatUnixTime2Date(time_info1.start_time)
    local start_info2 = TimeManager.FormatUnixTime2Date(time_info2.start_time)
    local end_info1 = TimeManager.FormatUnixTime2Date(time_info1.end_time)
    local end_info2 = TimeManager.FormatUnixTime2Date(time_info2.end_time)
    local time_str1 = string.format(Language.UTAPanel.Knockout.Text13,
    start_info1.month,start_info1.day,start_info1.hour,self:FormatMinute(start_info1.minute),end_info1.hour,self:FormatMinute(end_info1.minute))
    local time_str2 = string.format(Language.UTAPanel.Knockout.Text13,
    start_info2.month,start_info2.day,start_info2.hour,self:FormatMinute(start_info2.minute),end_info2.hour,self:FormatMinute(end_info2.minute))
    UH.SetText(self.Time1, time_str1)
    UH.SetText(self.Time2, time_str2)
end

function UTAAllTeamInfoPartTime:FormatMinute(minute)
    minute = minute < 10 and minute .. "0" or minute
    return minute
end

--比赛对阵信息
UTAAllTeamInfoPartMatchItem = UTAAllTeamInfoPartMatchItem or DeclareMono("UTAAllTeamInfoPartMatchItem", UIWidgetBaseItem)
-- data :::: UTAMatchClass
function UTAAllTeamInfoPartMatchItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.AtkTeam:SetData(data.atk_team)
    self.DefTeam:SetData(data.def_team)
    UH.SetText(self.State, Language.UTA.FightResult[data.fight_result])
    self.AtkTeam:SetWinFlag(data.fight_result == UTACfg.FightResult.AtkWin)
    self.DefTeam:SetWinFlag(data.fight_result == UTACfg.FightResult.DefWin)
    self.AtkTeam:SetDrawFlag(data.fight_result == UTACfg.FightResult.Draw)
    self.DefTeam:SetDrawFlag(data.fight_result == UTACfg.FightResult.Draw)
    UH.SetText(self.Index, data.show_index or "")

    self.VideoInteractor.Interactable = tonumber(data.video_id) ~= 0
    self.VideoBtn1:SetActive(data.fight_result ~= UTACfg.FightResult.Fighting)
    self.VideoBtn2:SetActive(data.fight_result == UTACfg.FightResult.Fighting)
end

function UTAAllTeamInfoPartMatchItem:OnGuessClick()
    -- 有轮空不允许竞猜
    if not self.data:IsCanGuess() then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip4)
        return
    end
    UniverseTeamArenaCtrl.Instance:ReqMatchGuessInfo(self.data)
    --ViewMgr:OpenView(UTAGuess, {match_id_t = self.data.match_id_t})
end

function UTAAllTeamInfoPartMatchItem:OnVideoClick()
    if tonumber(self.data.video_id) ~= 0 then
        UniverseTeamArenaCtrl.Instance:SendCSUTAWatchVideo(self.data.video_id_t)
    else
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip5)
    end
end

function UTAAllTeamInfoPartMatchItem:OnWatchClick()
    UniverseTeamArenaCtrl.Instance:SendCSUTAStartOB(self.data.info.match_id_t)
end

--比赛对阵的队伍信息
UTAAllTeamInfoPartMatchTeamItem = UTAAllTeamInfoPartMatchTeamItem or DeclareMono("UTAAllTeamInfoPartMatchTeamItem")
-- data :::: UTAMatchTeamClass
function UTAAllTeamInfoPartMatchTeamItem:SetData(data)
    if data:IsNull() then
        self.Info:SetActive(false)
        return
    else
        self.Info:SetActive(true)
    end
    UH.SetText(self.Name, data.team_name)
    UH.SetText(self.ServerName, data:ServerName())
    UH.SetText(self.Score, data.cur_score)

    if self.Score then
        UH.SetText(self.Score, data.cur_score)
    end

    if self.Rate then
        UH.SetText(self.Rate, data.support_rate .. "%")
    end

    UH.SetAvatarp(self.HeadIcon,data.avatar_type,data.head_id,0)
end

function UTAAllTeamInfoPartMatchTeamItem:SetWinFlag(value)
    self.WinFlag:SetActive(value)
end

function UTAAllTeamInfoPartMatchTeamItem:SetDrawFlag(value)
    self.DrawFlag:SetActive(value)
end

-- ======== 战队当前积分排名 ========
UTATeamScoreRankPart = UTATeamScoreRankPart or DeclareMono("UTATeamScoreRankPart", UIWFlushPanel)
function UTATeamScoreRankPart:UTATeamScoreRankPart()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.out_rank_data, func = self.FlushList, init_call = false}
    }
end

function UTATeamScoreRankPart:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.Tip1, Language.UTA.Knockout.Tip3)
    self:FlushSelfInfo()
end

function UTATeamScoreRankPart:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushReq()
end

function UTATeamScoreRankPart:FlushReq()
    -- 请求积分排行
    UniverseTeamArenaCtrl.Instance:SendCSUTATeamScoreRankListReq()
end

function UTATeamScoreRankPart:FlushSelfInfo()
    local self_team_info = self.data:SelfTeamBaseInfo()
    self.SelfInfo:SetActive(self_team_info ~= nli)
    UH.SetText(self.NothingText, self_team_info ~= nil and "" or Language.UTA.Knockout.Tip4)
    if self_team_info then
        UH.SetText(self.Name, self_team_info.team_name)
        local rank_str = self_team_info.cur_rank < 1 and Language.UTAPanel.Text5 or self_team_info.cur_rank
        UH.SetText(self.Rank, rank_str)
        UH.SetText(self.Score, self_team_info.qualification_score)
        local sum = self_team_info.all_num
        local rate = sum == 0 and "100%" or math.floor(self_team_info.win_num / sum * 100) .. "%"
        UH.SetText(self.Rate, string.format("%s (%s/%s)", rate, self_team_info.win_num, sum))
        local reward_list = UniverseTeamArenaData.Instance:GetRankReward(0, self_team_info.cur_rank)
        self.RewardList:SetData(reward_list)
        self.PassToggle.isOn = self.data:IsPass(self_team_info.cur_rank)
    end 
end

function UTATeamScoreRankPart:FlushList()
    local list_data = self.data:GetOutRankList()
    self.RankTeamList:SetData(list_data)
end

UTATeamScoreRankPartItem = UTATeamScoreRankPartItem or DeclareMono("UTATeamScoreRankPartItem", UIWidgetBaseItem)
-- data 来源 decode_uta_team_info_func
function UTATeamScoreRankPartItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank = data.rank
    self.BG:SetObjActive(rank <= 3)
    self.RankImg:SetObjActive(rank <= 3)
    if rank <= 3 then
        local bg_img = "PaiMingDi" .. rank
        UH.SpriteName(self.BG, bg_img)
        local rank_img = string.format("PaiMing_%sXiao", rank)
        UH.SpriteName(self.RankImg, rank_img)
        UH.SetText(self.RankText, "")
    else
        UH.SetText(self.RankText, rank)
    end
    local team_name
    if data.team_name and data.team_name == UniverseTeamArenaData.Instance:GetSelfTeamName() then
        team_name = ColorStr(data.team_name, COLORSTR.Yellow1)
    else
        team_name = data.team_name
    end
    UH.SetText(self.Name, team_name)
    local server_name = UniverseTeamArenaData.Instance:GetServerName(data.server_id, data.plat_type)
    UH.SetText(self.ServerName, server_name)
    local score1 = data.qualification_score < 0 and Language.UTAPanel.Text5 or data.qualification_score
    UH.SetText(self.Score1, score1)

    local rate = "100%"
    if data.battle_times > 0 then
        rate = math.floor(data.win_times / data.battle_times * 100) .. "%"
    end
    UH.SetText(self.Rate,string.format("(%s)%s/%s",rate,data.win_times,data.battle_times))

    local reward_list = UniverseTeamArenaData.Instance:GetRankReward(0, data.rank)
    self.RewardList:SetData(reward_list)
    self.PassToggle.isOn = UniverseTeamArenaData.Instance:IsPass(data.rank)
end

function UTATeamScoreRankPartItem:OnLookInfo()
    ViewMgr:OpenView(UTABattleArray,{team_id_t = self.data.team_id_t, team_name = self.data.team_name})
end