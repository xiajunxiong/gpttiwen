FILE_REQ("modules/arena/arena_reward_rule")
FILE_REQ("modules/arena/arena_rank_view")

ArenaView = ArenaView or DeclareView("ArenaView","arena/arena",Mod.Arena.Main)
VIEW_DECLARE_LEVEL(ArenaView,ViewLevel.L0)

function ArenaView:LoadCallback(param_t)
    self.lang = Language.Arena
    self.Tabbar:SetData(
        {
            {name = self.lang.Tab1, tab_objs = {self.TabArena}, mod = TabMod(Mod.Arena.Main)},
            {name = self.lang.Tab2, tab_objs = {self.TabReward}, mod = TabMod(Mod.Arena.Reward)},
            {name = self.lang.Tab3, tab_objs = {self.TabRank}, mod = TabMod(Mod.Arena.Rank)},
        }
    )
    for i=2, 6 do
        ArenaCtrl.Instance:OnCSColosseumReq(i,0)
    end
    local clickIndex = 1
    if param_t and param_t.open_param ~= nil then
        clickIndex = param_t.open_param
    end
    self.Tabbar:ClickItem(clickIndex)
    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COLOSSEUM)

    UH.SetText(self.BattleRewardScoreText, ArenaData.Instance:GetBattleRewardScore())
    self:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.ArenaRank)
end

function ArenaView:CloseCallback()
    self.freshOpponentCD = false
    TimeHelper:CancelTimer(self.btn_flush_time)
end

function ArenaView:ArenaView()
    -- self.name.text = RoleData.Instance:GetBaseData().name
    self.Currency:SetData(CurrencyType.Colosseum)
end

function ArenaView:OnClose()
    ViewMgr:CloseView(ArenaView)
end

function ArenaView:OnClickFightReport()
    self.NewReport:SetActive(false)
    ViewMgr:OpenView(ArenaFightReportView)
end

function ArenaView:OnClickShopBtn()
    StoreData.Instance:SetJumpIndex({conver_type = 2})
    ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function ArenaView:OnClickAchievementBtn()
    if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Error(Language.ArenaAchieve.OpenTip)
        return
    end
    ViewMgr:OpenView(ArenaAchievenmentView)
end

function ArenaView:OnClickRank()
    ViewMgr:OpenView(ArenaRankView)
end

function ArenaView:OnClickReware()
    ViewMgr:OpenView(ArenaRankRewardView)
end

function ArenaView:OnClickLiBao()
    ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_GET_AWARD,0)--领取礼包操作时服务器不处理p1，但是协议必须要有个int参数
end

function ArenaView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[38].desc})
end

function ArenaView:OnClickBuffTip()
    
    -- local buff_list = ArenaData.Instance:GetBuffList()
    --LogError("OnClickBuffTip",buff_list)
    ViewMgr:OpenView(ArenaBuffTip)
end

function ArenaView:OnClickFreshOpponent()
    if not self.freshOpponentCD then
        ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_REFRESH_TARGET,0)
        self.freshOpponentCD = true
        self:FlushTimeView()
    else
        PublicPopupCtrl.Instance:Center(Language.Arena.FreshOpponentCDTip)
    end
end

function ArenaView:OnClickFormation()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COLOSSEUM, {hide_challenge = true})
end

function ArenaView:OnClickBox()
    ViewMgr:OpenView(ArenaRewardRuleView)
    -- ViewMgr:OpenView(ArenaRewardPoolView)
end

function ArenaView:FlushTimeView()
    local end_time = Config.arena_cfg_auto.other[1].refresh_time
    self.btn_flush_time = TimeHelper:AddCountDownTT(function()
        UH.SetText(self.FlushTime,end_time .. "s")
        end_time = end_time - 1
    end,
    function()
        UH.SetText(self.FlushTime,Language.Arena.FlushTime)
        self.freshOpponentCD = false
	end, end_time, 1, true)
end
function ArenaView:SetCompetitionRank(rank_type)
    CompetitionRanklCtrl.Instance:SendCompetitionRankType(rank_type)
    local is_open = CompetitionRanklData.Instance:GetIsOpenEnd(rank_type)
    self.BtnComp:SetActive(is_open)
    UH.SetText(self.CompDesc, Language.CompetitionRank.RankName[rank_type])
end
function ArenaView:CompetitionRank()
    ViewMgr:OpenViewByKey(Mod.RandActivity.CompetitionRank)
end

ArenaViewFlush = ArenaViewFlush or DeclareMono("ArenaViewFlush",UIWFlushPanel)
---EndTime=====UIWTimeMeter
--BuffTipBtn===GameObject
function ArenaViewFlush:ArenaViewFlush()
    self.data_instance = ArenaData.Instance
    self.data_cares = {
        {data = self.data_instance.target_list,func = self.OpponentLishFlush,keys = {"is_change"}},
        {data = self.data_instance.top_list,func = self.OpponentLishFlush,keys = {"is_change"}},
        {data = self.data_instance.RoleInfo,func = self.RoleInfoFlush,keys = {"is_change"}},
        {data = self.data_instance.RoleInfo,func = self.OnEndTimeChanged,keys = {"next_switch_timestamp"}},
        {data = self.data_instance.RoleInfo, func = self.OnRoleEnterArena, keys = {"is_out_of_rank_list"}},
        {data = self.data_instance.RoleInfo, func = self.FlushLeftChallengeTimes, keys = {"today_fight_count"}},
        {data = self.data_instance.rank_pos_list, func = self.SetLiBaoBtnShow, keys = {"is_change"}, init_call = false},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushRandActState, keys = {"act_type", "status"}},
        {data = self.data_instance.achievement_data, func = self.FlushAchievementRemind, init_call = true},
        {data = self.data_instance.history_highest_info, func = self.FlushHistoryHighestScore, init_call = false},
        {data = self.data_instance.shop_smart, func = self.FlushShopRedPoint, keys = {"is_change"}}
    }

    self.ui_drawers = {}
    for k=1,self.model_array:Length() do
        self.ui_drawers[k] = UIChDrawer.New()
        self.model_array[k]:SetShowData({
            ui_obj = self.ui_drawers[k],
            view = self
        })
    end
end

function ArenaViewFlush:FlushShopRedPoint()
    if self.data_instance:GetShopRedPoint() > 0 then
        if self.shop_red_point == nil then
            self.shop_red_point = UiRedPoint.New(self.ShopBtnObj, Vector3.New(32, 25,0))
        end
        self.shop_red_point:SetNum(1)
    elseif self.shop_red_point ~= nil then
        self.shop_red_point:SetNum(0)
    end
end

function ArenaViewFlush:FlushHistoryHighestScore()
    UH.SetText(self.HistoryHighestText, self.data_instance:GetHistoryHighestRank().cur_rank)
end

function ArenaViewFlush:FlushRandActState()
    local state_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.KF_ATHLETICS)
    if state_info and state_info.status == ActStatusType.Open then
        UH.SetText(self.NewSerTex1, Language.Arena.NewSer1)
        self.NewSerTex2:CloseCountDownTime()
	    self.NewSerTex2:CreateActTime(ACTIVITY_TYPE.KF_ATHLETICS, TimeType.Type_Special_7)
        self.NewSerObj:SetActive(true)
    else
        self.NewSerObj:SetActive(false)
    end
end
function ArenaViewFlush:OnClickGoTo()
    ViewMgr:OpenViewByKey(Mod.ServerActivity.Athletics)
end

function ArenaViewFlush:RoleInfoFlush()
    if self.data_instance.up_rank ~= nil and self.data_instance.up_rank ~= 0 then
        local run_time = math.abs(self.data_instance.up_rank) <= 4 and math.abs(self.data_instance.up_rank) or 4
        local step_rank = math.floor(self.data_instance.up_rank / run_time)
        --界面打开后X秒执行
        if self.delay_show_rank_change == nil then
            self.delay_show_rank_change = TimeHelper:AddDelayTimer(function ()
                self.set_times = run_time
                if self.role_info_rank_run == nil then
                    self.role_info_rank_run = TimeHelper:AddRunTimer(function ()
                        self.rank.text = self.data_instance.RoleInfo.rank + step_rank * self.set_times
                        self.set_times = self.set_times - 1
                        if self.set_times == 0 then
                            self.rank.text = self.data_instance.RoleInfo.rank
                            self.data_instance.up_rank = nil
                        end
                    end, 0.05, run_time, false)
                end
            end,1)
            self.rank.text = self.data_instance.RoleInfo.rank + self.data_instance.up_rank
        end
    else
        self.rank.text = self.data_instance.RoleInfo.rank
    end
    self.ScoreImgText.text = self.data_instance.RoleInfo.capabolity
    local reward_str = self.data_instance:GetMyRankReward()
    reward_str = Language.Arena.CurRankReward .. string.format(Language.Common.Xstr, COLORSTR.Yellow1, reward_str)
    self.CurRewardText.text = reward_str
    self.NewReport:SetActive(self.data_instance.has_new_report)
    self:SetLiBaoBtnShow()
    self:FlushHistoryHighestScore()
end

function ArenaViewFlush:FlushLeftChallengeTimes()
    UH.SetText(self.ChallengeLeftTimes, string.format(Language.Arena.TodayChallengeLeftTimes, COLORSTR.Yellow6, ArenaData.Instance:GetChallengeLeftTimes()))
end

--礼包按钮显示
function ArenaViewFlush:SetLiBaoBtnShow()
    local show = ArenaData.Instance:GetHasLiBao()
    self.BtnLiBaoObj:SetActive(show)
end

function ArenaViewFlush:OnRoleEnterArena()
    local is_out_of_rank_list = self.data_instance.RoleInfo.is_out_of_rank_list == 0
    self.middle:SetActive(is_out_of_rank_list)
    if self.data_instance:GetShowFirstIn() then
        ViewMgr:OpenView(ArenaFirstIn)
    end
end

function ArenaViewFlush:OpponentLishFlush()
    self.BuffTipBtn:SetActive(#self.data_instance:GetBuffList() ~= 0)
    if ArenaData.Instance.target_list.data[1] == nil then
        return
    end

    table.sort(ArenaData.Instance.target_list.data, DataSorter.KeyLowerSorter("rank"))

    for k, v in pairs(ArenaData.Instance.target_list.data) do

        self.ui_drawers[k]:SetData(v.appearance)
    end
    self.opponent_list:SetData(ArenaData.Instance.target_list.data)

    GuideManager.Instance:RegisterGetGuideUi("ArenaBtn", function()
        if self.opponent_list and self.opponent_list:GetItem(4) then
            return self.opponent_list:GetItem(4).BtnChallenge,function()
                if ViewMgr:IsOpen(ArenaView) and self.opponent_list:GetItem(4).ChallengeClick then
                    self.data_instance.guide_fight = true
                    self.opponent_list:GetItem(4):ChallengeClick()
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
	end)
end

function ArenaViewFlush:OnClickBuffIcon()
    ViewMgr:OpenView(BuffInfoTip, {buff_id = self.buff_cfg.buff_id})
end

function ArenaViewFlush:OnEndTimeChanged()
    -- self.EndTime:StampTime(self.data_instance:NextSwitchTS(),TimeType.Type_Special_3)
end

function ArenaViewFlush:OnDestroy()
    GuideManager.Instance:UnRegisterGetGuideUi("ArenaBtn")
    if self.ui_drawers then
        for k, v in pairs(self.ui_drawers)do
            v:DeleteMe()
            v = nil
        end
        self.ui_drawers = nil
    end
    if self.delay_show_rank_change ~= nil then
        TimeHelper:CancelTimer(self.delay_show_rank_change)
        self.delay_show_rank_change = nil
    end
    if self.role_info_rank_run ~= nil then
        TimeHelper:CancelTimer(self.role_info_rank_run)
        self.role_info_rank_run = nil
    end
    -- self.EndTime:CloseCountDownTime()
    UIWFlushPanelGroup.OnDestroy(self)
end

function ArenaViewFlush:FlushAchievementRemind()
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.Arena)
    if actInfo then
        actInfo:ActRedPoint()
    end
    self.AchieveRedPoint:SetNum(self.data_instance:GetAchievementRemind())
end
---------------------------------------------------
ArenaOpponentItem = ArenaOpponentItem or DeclareMono("ArenaOpponentItem",UIWidgetBaseItem)
function ArenaOpponentItem:ArenaOpponentItem()
    self.data_instance = ArenaData.Instance
end
function ArenaOpponentItem:SetData(data)
    self.data = data
    -- self.avatar:SetAvatar(data.avatar_type)
    if string.find(data.name,"%a") then
        UH.SetText(self.role_name, "")
        UH.SetText(self.ChaRoleName, data.name)
    else
        UH.SetText(self.role_name, data.name)
        UH.SetText(self.ChaRoleName, "")
    end
    data.profession = data.appearance.prof_base <= 8 and data.appearance.prof_base >= 0 and data.appearance.prof_base or 0
    local prof = Language.Common.Prof[data.profession]
    local blank_count = ((data.level >= 100 and 4) or (data.level >= 10 and 1)) or 0
    local blank = "\t"
    for i=1,blank_count do
        blank = blank .. " "
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    if data.profession ~= 0 then prof = blank .. prof end
    local text = prof .. data.level .. Language.Arena.level
    UH.SetText(self.LevelText, data.level)
    UH.SpriteName(self.prof_img,RoleData.GetProfSp(data.profession))
    data.capabolity = data.capabolity or 0
    UH.SetText(self.ScoreText,string.format(Language.Arena.Score,data.capabolity > 0 and data.capabolity or Language.Arena.NotScore))
    self.role_rank.text = string.format(Language.Arena.rank,data.rank)

    self.PeakBg:SetActive(data.top_level > 0)
    UH.SetText(self.PeakLevel, data.top_level)
    
    -- UH.SpriteName(self.BtnChallengeImg, role_level < data.level and "hongtiaozhan" or "lvtiaozhan")
    local score_better = data.capabolity > 0 and self.data_instance.RoleInfo.capabolity < data.capabolity or false
    UH.SpriteName(self.BtnChallengeImg, score_better and "hongtiaozhan" or "lvtiaozhan")
    UH.SpriteName(self.ImgChallenge, data.is_can_sweep == 1 and "_LocSaoDang1" or "_LocTiaoZhan1")
end

function ArenaOpponentItem:MobaiClick()
    ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_FETCH_EXP,self.data.rank - 1)
    ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_TOP_INFO,0)
end

function ArenaOpponentItem:ChallengeClick()
    -- GuideManager.Instance:ForceStop()
    if ArenaData.Instance:GetChallengeLeftTimes() > 0 then
        -- 可以扫荡就扫荡，否则挑战
        if self.data.is_can_sweep == 1 then
            ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_SWEEP, self.data.index)
            BagData.Instance:SetHideCurrencyCenterFlag(true)
        else
            ArenaData.Instance.index = self.data.index
            ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COLOSSEUM)
            -- ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_FIHGT,self.data.index)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Arena.NotEnoughTimes)
    end
end

-------------------------------------------
ArenaFirstIn = ArenaFirstIn or DeclareView("ArenaFirstIn","arena/arena_first_in")
VIEW_DECLARE_MASK(ArenaFirstIn,ViewMask.BgBlockClose)
function ArenaFirstIn:ArenaFirstIn()
    self.data = ArenaData.Instance
end

function ArenaFirstIn:LoadCallback()
    self.data.is_showed_first_in = true
end
-------------------------------------------
ArenaBuffTip = ArenaBuffTip or DeclareView("ArenaBuffTip", "arena/arena_buff_tip")
VIEW_DECLARE_MASK(ArenaBuffTip, ViewMask.BlockClose)
function ArenaBuffTip:ArenaBuffTip()
    self.data = ArenaData.Instance
end

function ArenaBuffTip:LoadCallback()
    local buff_list = self.data:GetBuffList()
    if next(buff_list) then
        self.BuffList:SetData(buff_list)
    end 
end

-----------------------------------------
ArenaBuffTipItem = ArenaBuffTipItem or DeclareMono("ArenaBuffTipItem", UIWidgetBaseItem)
function ArenaBuffTipItem:ArenaBuffTipItem()
end

function ArenaBuffTipItem:SetData(data)
    UH.SetIcon(self.Icon, data.icon_id)
    UH.SetText(self.Descr, data.desc)
    UH.SetText(self.Name, data.buff_name)
    self.data = data
end

-----------------------------------------
ArenaHistoryHighestScoreChange = ArenaHistoryHighestScoreChange or DeclareView("ArenaHistoryHighestScoreChange","arena/arena_history_highest_score_change")
function ArenaHistoryHighestScoreChange:ArenaHistoryHighestScoreChange()
    self.data_instance = ArenaData.Instance
    self.data_instance.history_highest_info.changed = false
end

function ArenaHistoryHighestScoreChange:LoadCallback()
    local score = self.data_instance:GetHistoryHighestRankScore()
    UH.SetText(self.RankChangeText, string.format(Language.Arena.HistoryHighestRankChange, self.data_instance:GetHistoryHighestRank().change_rank_num))
    UH.SetText(self.ScoreChangeText, string.format(Language.Arena.HistoryHighestRankChangeScore, score))
    local item = Item.Init(CommonItemId.ArenaPoint, score)
    self.ItemList:SetData({item})
    PublicPopupCtrl.Instance:CenterV {type = CurrencyType.Colosseum, num = score}
end

function ArenaHistoryHighestScoreChange:CloseCallback()
    ViewMgr:OpenView(ArenaView)
end

function ArenaHistoryHighestScoreChange:OnClickBlock()
    ViewMgr:CloseView(ArenaHistoryHighestScoreChange)
end

---------------------ArenaSweepSuccessShowView---------------------
ArenaSweepSuccessShowView = ArenaSweepSuccessShowView or DeclareView("ArenaSweepSuccessShowView","arena/arena_sweep_success_show")
VIEW_DECLARE_LEVEL(ArenaSweepSuccessShowView, ViewLevel.Ls)
function ArenaSweepSuccessShowView:ArenaSweepSuccessShowView()
end

function ArenaSweepSuccessShowView:LoadCallback()
end

function ArenaSweepSuccessShowView:CloseCallback()
    ViewMgr:OpenView(ArenaView)
end

function ArenaSweepSuccessShowView:OnClickBlock()
    ViewMgr:CloseView(ArenaSweepSuccessShowView)
end

---------------------ArenaSweepSuccessShowPanel---------------------
ArenaSweepSuccessShowPanel = ArenaSweepSuccessShowPanel or DeclareMono("ArenaSweepSuccessShowPanel",UIWFlushPanel)
function ArenaSweepSuccessShowPanel:ArenaSweepSuccessShowPanel()
    self.data_instance = ArenaData.Instance
    self.data_cares = {
        {data = self.data_instance.target_list, func = self.FlushShow, keys = {"is_change"}},
    }
end

function ArenaSweepSuccessShowPanel:FlushShow()
    if self.data_instance:HistoryHighestRankChanged() then
        self.data_instance.history_highest_info.changed = false
        local score = self.data_instance:GetHistoryHighestRankScore()
        UH.SetText(self.RankChangeText, Format(Language.Arena.SweepUp[1], self.data_instance:GetHistoryHighestRank().change_rank_num))
        UH.SetText(self.ScoreChangeText, Format(Language.Arena.HistoryHighestRankChangeScore, score))
        local item = Item.Init(CommonItemId.ArenaPoint, score)
        self.ItemList:SetData({item})
        PublicPopupCtrl.Instance:CenterV {type = CurrencyType.Colosseum, num = score}
    else
        local score = self.data_instance:GetBaseBattleRewardScore()
        UH.SetText(self.RankChangeText, Language.Arena.SweepUp[2])
        UH.SetText(self.ScoreChangeText, Format(Language.Arena.HistoryHighestRankChangeScore, score))
        -- local item = Item.Init(CommonItemId.ArenaPoint, score)
        local list = self.data_instance:FinishBattleReward()
        -- self.ItemList:SetData({item})
        self.ItemList:SetData(DataHelper.FormatItemList(list))
    end

    if self.data_instance.target_list.data[1] == nil then
        return
    end
    table.sort(self.data_instance.target_list.data, DataSorter.KeyLowerSorter("rank"))
    self.index = nil
    for i, v in ipairs(self.data_instance.target_list.data) do
        if v.is_can_sweep == 1 then
            self.index = v.index
            break
        end
    end
    self.ObjContinue:SetActive(self.index ~= nil)
end

function ArenaSweepSuccessShowPanel:OnContinueClick()
    if self.data_instance:GetChallengeLeftTimes() > 0 then
        if self.index then
            ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_SWEEP, self.index)
            BagData.Instance:SetHideCurrencyCenterFlag(true)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Arena.NotEnoughTimes)
        ViewMgr:CloseView(ArenaSweepSuccessShowView)
    end
end