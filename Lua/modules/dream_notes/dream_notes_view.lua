DreamNotesView = DreamNotesView or DeclareView("DreamNotesView", "dream_notes/dream_notes_view")

VIEW_DECLARE_LEVEL(DreamNotesView,ViewLevel.L0)

function DreamNotesView:DreamNotesView()
    self.Currency:SetCurrency(CurrencyType.Gold,CurrencyType.LuckCoin,CurrencyType.PartnerLevelItem)
end

function DreamNotesView:OpenCallback()
    SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.DREAM_NOTES_CHALLENGE,2)
end

function DreamNotesView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[104].desc)
end

function DreamNotesView:OnClickShop()
    ViewMgr:OpenView(MysteryStoreView)
end

function DreamNotesView:OnClickClose()
    ViewMgr:CloseView(DreamNotesView)
    DreamNotesData.Instance.remind_list = {}
    DreamNotesData.Instance:CheckRedPiontData()
end

--===========================DreamNotesPanel===========================
DreamNotesPanel = DreamNotesPanel or DeclareMono("DreamNotesPanel", UIWFlushPanel)

function DreamNotesPanel:DreamNotesPanel()
    self.data = DreamNotesData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushTimesView,init_call = true},
        {data = self.data.select_data, func = self.FlushItemView,init_call = false},
        {data = SocietyData.Instance.rank_all_data,func = self.FlushRankView},
        {data = MysteryStoreData.Instance.anim_data,func = self.FlushAnimView,init_call = false},
    }
    self:RegisterGetGuideUi()
    self:ChekcSimularIphone()
end

function DreamNotesPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:OnWaitLockFunc()
end

function DreamNotesPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushListView()
end

function DreamNotesPanel:FlushListView()
    local pass_list = self.data:GetPassList()
    local guide_index
    --当界面打开时有指引则界面数据展示第一页（目前有指引的梦渊都在第一页（前3关））
    if GuideManager.Instance:IsGuide() then
        guide_index = 1
    end
    local index = self.data:GetIndex(pass_list, guide_index)
    self.List:SetDataList(pass_list,index)
    self:JumpHorizontalView(index or 1)
    self.Currency:SetCurrency(CurrencyType.LuckCoin,CurrencyType.PartnerLevelItem)
end

function DreamNotesPanel:JumpHorizontalView(index)
    local ITEM_MAX_NUM = 146    --单个Item宽度
    local ITEM_OFFSET_NUM = -55 --补足宽度 不一定List都是Item的整数比
    local NEED_ITEM_NUM = 3     --需要放在哪个位置index
    local width = self.ListRect.rect.width
    if (index * ITEM_MAX_NUM - ITEM_OFFSET_NUM) > width then
        self.ShowTimer = Invoke(function()
            local offset = ITEM_MAX_NUM * (index - NEED_ITEM_NUM)
            self.List:ForceJumpHorizontal(-offset)
        end,0.1)
	end
end

function DreamNotesPanel:FlushRankView()
    local rank_list = self.data:GetRankList()
    for i=1,self.RankList:Length() do
        self.RankList[i]:SetData(rank_list[i])
    end
end

function DreamNotesPanel:FlushItemView()
    local data = self.data:GetSelectData()
    local score = RoleData.Instance:GetSumScore()
    UH.SetText(self.RoleScore,data.synopsis)
    UH.SetText(self.Level, Format(Language.DreamNotes.PassLevel2, DataHelper.GetDaXie(data.level), data.dream_notes_name))--DataHelper.GetDaXie(data.level)))
    self.SweepInter.Interactable = self.data:IsFirstKill(data) and self.data:GetSweepTimes() > 0 and score >= data.recommend_score
    self.Grid:SetData(self.data:GetRewardList(data))
    -- local score_text = score >= data.recommend_score and data.recommend_score or ColorStr(data.recommend_score,COLORSTR.Red8)
    -- UH.SetText(self.Score,Format(Language.DreamNotes.ScoreSweepTip,score_text))
    local score_text = score >= data.recommend_score and score or ColorStr(score,COLORSTR.Red8)
    UH.SetText(self.Score,Format(Language.DreamNotes.TeamSweepTip, score_text, data.recommend_score))
    UH.SetText(self.Strength,data.power_expend)
    self:SetHorizontalView()
    self:SetVideoView(data)
    -- self.StrengthGame:SetActive(self.data:IsFirstKill(data))
    -- self.TipsGame:SetActive(not self.data:IsFirstKill(data))
end

function DreamNotesPanel:FlushTimesView()
    DreamNotesCtrl.Instance:OnSweepReard(self.sweep_times,self.data:GetSweepTimes())
    self.sweep_times = self.data:GetSweepTimes()
    local COLOR = self.sweep_times > 0 and COLORSTR.Green7 or COLORSTR.Red7
    UH.SetText(self.Times,Format(Language.DreamNotes.SweepTip,COLOR,self.sweep_times))
end

function DreamNotesPanel:OnClickSweep()
    local data = self.data:GetSelectData()
    if not self.data:IsFirstKill(data) then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotSweepTip)
        return
    end
    if RoleData.Instance:GetSumScore() < data.recommend_score then
        PublicPopupCtrl.Instance:Center(Format(Language.DreamNotes.NeedScoreSweepTip,data.recommend_score))
        return
    end
    if self.data:GetSweepTimes() == 0 then
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = 25186})})
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotTiemsTip[self.data:GetBuyTimes() == 0 and 1 or 2])
        return
    end
    if FightData.Instance:GetFightStrengthNum() < data.power_expend then
        PublicPopupCtrl.Instance:Center(Language.Material.StrengthTip)
        ViewMgr:OpenView(FightCrystalSweepItemView)
        return
    end
    DreamNotesCtrl.Instance:SendChallengeReq(1,data.level)
end

function DreamNotesPanel:OnClickEnter()
    if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
    if FightCtrl.IsActFB() then
        return
    end
    local data = self.data:GetSelectData()
    local level = self.data:GetSelectData().level
    if self.data:GetSweepTimes() == 0 and level <= self.data:GetPassLevel() then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotTiemsTip[self.data:GetBuyTimes() == 0 and 1 or 2])
        return
    end
    if level > self.data:GetPassLevel() + 1 then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotPassTip)
        return
    end
    if RoleData.Instance:GetRoleLevel() < data.recommend_level then
        PublicPopupCtrl.Instance:Center(Format(Language.DreamNotes.LimitLevelTip,data.recommend_level))
        return
    end
    if self.data:IsFirstKill(data) and FightData.Instance:GetFightStrengthNum() < data.power_expend then
        PublicPopupCtrl.Instance:Center(Language.Material.StrengthTip)
        ViewMgr:OpenView(FightCrystalSweepItemView)
        return
    end
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.DreamNotes.TeamChallengeTip,function()
            TeamCtrl.Instance:SendExitTeam()
            self:OnChallenge(level)
        end)
        return
    end
    self:OnChallenge(level)
end

function DreamNotesPanel:OnChallenge(level)
    DreamNotesCtrl.Instance:SendChallengeReq(0,level)
    self.data:SetChallengeLevel(level)
    ViewMgr:CloseView(DreamNotesView)
    ViewMgr:CloseView(FightFBView)
    BehaviorData.Instance.auto_behavior = 0
    AutoInvokerCtrl.Instance:ClearCommands()
end

function DreamNotesPanel:OnClickAdd()
    if self.data:GetBuyTimes() == 0 then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasBuyTimes)
        return
    end
    ViewMgr:OpenView(DreamNotesTipView,{
        content_func = function(num)
            return Format(Language.DreamNotes.BuyGoldTip,self.data:GetTimesPriceNum(num),num)
        end,
        times_func = function(num)
            return Format(Language.DreamNotes.BuyTimesTip,self.data:GetBuyTimes() - num)
        end,
        can_func = function(num)
            return (self.data:GetBuyTimes() - num) >= 0
        end,
        price_func = function(num)
            return self.data:GetTimesPriceNum(num)
        end,
        comfirm_func = function(num)
            for i=1,num do
                DreamNotesCtrl.Instance:SendChallengeReq(3,1)
            end
        end
    })
end

function DreamNotesPanel:OnClickRank()
    SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.DREAM_NOTES_CHALLENGE)
    self.OpenTimer = Invoke(function()
        ViewMgr:OpenView(DreamNotesRankView)
    end,0.5)
end

function DreamNotesPanel:OnClickShop()
    ViewMgr:OpenView(MysteryStoreView)
end

function DreamNotesPanel:OnWaitLockFunc()
    if DreamNotesCtrl.Instance.WaitLockFunc then
        self.MysteryStoreReward:SetActive(true)
    end
end

function DreamNotesPanel:FlushAnimView()
    self.RewardAnim:Play([[MysteryStoreReward_sub]])
end

--===========================显示左右按钮相关===========================
function DreamNotesPanel:OnClickDrag()
    if self.horizontal == nil then return end
    local num = math.abs(self.Content.localPosition.x)
    local width = self.ListRect.rect.width / 2
    self.BtnLeft:SetActive(num > (self.horizontal + width))
    self.BtnRight:SetActive(num < (self.horizontal - width))
end

function DreamNotesPanel:OnClickHorizontal()
    if self.horizontal == nil then return end
    self.List:ForceJumpHorizontal(-self.horizontal)
    self:SetHorizontalView()
end

function DreamNotesPanel:SetHorizontalView()
    self.horizontal = math.abs(self.Content.localPosition.x)
    self.BtnLeft:SetActive(false)
    self.BtnRight:SetActive(false)
end
--===============================录像相关===============================
function DreamNotesPanel:SetVideoView(data)
    local npc_config = Cfg.NpcById(data.boss_id)
	local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE,npc_config.param1)
	if video_config then
		if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGrayView, self, video_config))
        self:FlushGrayView(video_config)
        BattleCtrl.BattleReportSimpleInfo(video_config.monster_group_id, 0)
	end
end

function DreamNotesPanel:FlushGrayView(video_config)
	self.ViewGray.enabled = BattleVideo.Instance:GetSimpleInfo(video_config.monster_group_id, 0) == 0
end

function DreamNotesPanel:OnClickPlayVideo()
	local data = self.data:GetSelectData()
    local npc_config = Cfg.NpcById(data.boss_id)
    local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE,npc_config.param1)
	if video_config ~= nil then
		BattleVideo.Instance:PlayVideoCfg(video_config)
        BattleCtrl.BattleReportDetailInfo(video_config.monster_group_id)
    else
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasPlayVideo)
	end
end
--===========================指引相关===========================
function DreamNotesPanel:RegisterGetGuideUi()
    for i=1,DreamNotesConfig.GuideUiMaxNum do
        GuideManager.Instance:RegisterGetGuideUi("DreamNotesViewLevel"..i, function()
            if self.List.index_item and self.List.index_item[i] then
                self.List:ForceJumpHorizontal(0)
                self.List:SetectItem(i,true)
                return self.List.index_item[i],function()
                    self.List.index_item[i]:Click()
                end
            end
        end)
    end
    GuideManager.Instance:RegisterGetGuideUi("DreamNotesViewLevel", function()
        if GuideManager.Instance:GuideView().step_cfg.step_param_2 ~= "" then
            local i = GuideManager.Instance:GuideView().step_cfg.step_param_2
            if self.List.index_item and self.List.index_item[i] then
                self:JumpHorizontalView(i)
                self.List:SetectItem(i,true)
                return self.List.index_item[i],function()
                    self.List.index_item[i]:Click()
                end
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("DreamNotesChallengeBtn", function()
        return self.ChallengeBtn,function ()
            self:OnClickEnter()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("DreamNotesSweepBtn", function()
        return self.SweepBtn,function ()
            self:OnClickSweep()
        end
    end)
end

function DreamNotesPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.OpenTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.WaitLockTimer)
    self.data:Initialization()
	if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    self.data:ClearRemindList()
end

function DreamNotesPanel:ChekcSimularIphone()
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        self.PanelRect:SetSizeWithCurrentAnchors(0, self.PanelRect.rect.width - 100)
    end
end
----------------------------DreamNotesItem----------------------------
DreamNotesItem = DreamNotesItem or DeclareMono("DreamNotesItem", UIWidgetBaseItem)

function DreamNotesItem:SetData(data)
    local is_recommend = RoleData.Instance:GetRoleLevel() < data.recommend_level
    UH.SpriteName(self.ImgSet,DreamNotesConfig.ImgName[data.type])
    UH.LocalPos(self.PanelRect,DreamNotesConfig.ImgPos[data.seq % 2])
    UH.SetText(self.Level,RichTextHelper.StringToVertical(data.dream_notes_name))--Format(Language.DreamNotes.Level,DataHelper.GetDaXie(data.level))))
    UH.SetText(self.Difficult,RichTextHelper.StringToVertical(data.dream_notes_name))--Format(Language.DreamNotes.Level,DataHelper.GetDaXie(data.level))))
    UH.SetText(self.LimitLevel,data.recommend_level .. Language.Common.Level)
    self.Difficult:SetObjActive(data.type == 1)
    self.Remind:SetNum(DreamNotesData.Instance:GetItemRemind(data.level))
    self.RemindGame:SetActive(data.recommend_level ~= 0)
    self.ItemInter.Interactable = (not is_recommend) and DreamNotesData.Instance:GetPassLevel()+1 >= data.level
    self.Block:SetActive(false)
    UIWidgetBaseItem.SetData(self, data)
end

function DreamNotesItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    DreamNotesData.Instance:SetSelectData(self.data)
    if DreamNotesData.Instance:GetItemRemind(self.data.level) == 1 then
        UnityPlayerPrefs.SetInt(RoleData.Instance:GetRoleId() .. "DreamNotes",0)
        DreamNotesData.Instance:SetItemRemind(self.data.level)
        self.Remind:SetNum(0)
    end
end

function DreamNotesItem:OnClickBlock()
    PublicPopupCtrl.Instance:Center(Format(Language.DreamNotes.LimitLevelTip,self.data.recommend_level))
end

----------------------------DreamNotesRankCall----------------------------
DreamNotesRankCall = DreamNotesRankCall or DeclareMono("DreamNotesRankCall", UIWidgetBaseItem)

function DreamNotesRankCall:SetData(data)
    if data ~= nil then
        UH.SetText(self.Name,data.role.name)
        UH.SetAvatar(self.Head,data.role.avatar_type,data.role.avatar_id,data.role.avatar_quality)
        UH.SetText(self.Level,Format(Language.DreamNotes.RankTip,data.rank_value))
    end
    self.NotRankTip:SetActive(data == nil)
    UIWidgetBaseItem.SetData(self, data)
end

function DreamNotesRankCall:OnClickItem()
    if self.data ~= nil then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.role.uid)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.role.uid)
    end
end

--==============================DreamNotesTipView===============================
DreamNotesTipView = DreamNotesTipView or DeclareView("DreamNotesTipView", "dream_notes/dream_notes_tip")

function DreamNotesTipView:LoadCallback(param_t)
    self.make_num = 1
    self.param_t = param_t
    self:FlushNumView()
end

function DreamNotesTipView:FlushNumView()
    UH.SetText(self.Num,self.make_num)
    self.price = self.param_t.price_func(self.make_num)
    UH.SetText(self.Content,self.param_t.content_func(self.make_num))
    UH.SetText(self.Times,self.param_t.times_func(self.make_num))
end

function DreamNotesTipView:OnClickAddNum(num)
    if self.make_num + num <= 0 then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.BuyTimesMinTip)
        TimeHelper:CancelTimer(self.time_func)
        return
    end
    if not self.param_t.can_func(self.make_num + num) then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.BuyTimesNotTip)
        TimeHelper:CancelTimer(self.time_func)
        return
    end
    self.make_num = self.make_num + num
    self:FlushNumView()
end

function DreamNotesTipView:OnClickCancel()
    ViewMgr:CloseView(DreamNotesTipView)
end

function DreamNotesTipView:OnClickConfirm()
    if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.price) then
        return
    end
    self.param_t.comfirm_func(self.make_num)
    ViewMgr:CloseView(DreamNotesTipView)
end

--长按
function DreamNotesTipView:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickAddNum(num)
        end
	end, 0.1)
end

function DreamNotesTipView:OnClickUp()
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function DreamNotesTipView:CloseCallback()
    TimeHelper:CancelTimer(self.time_func)
end

--===============================DreamNotesBattlePanel===============================
DreamNotesBattlePanel = DreamNotesBattlePanel or DeclareMono("DreamNotesBattlePanel", UIWFlushPanel)

function DreamNotesBattlePanel:DreamNotesBattlePanel()
    self.data = DreamNotesData.Instance
    self.data_cares = {
        {data = BattleData.Instance:BaseData(),func = self.OnBattleView,keys = {"round"}},
    }
end

function DreamNotesBattlePanel:OnBattleView()
    local round = BattleData.Instance:Round()
    local battle_mode = BattleData.Instance:BattleMode()
    local config = Language.BattleTopRightConfig[battle_mode]
    if config then
        UH.SetText(self.Title,self:GetTitle(config))
        if config.round_func then
            UH.SetText(self.Tips,self:GetTips(config,(config.round_func() or 0) - round))
        else
            UH.SetText(self.Tips,self:GetTips(config,(config.round or 0) - round))
        end
    end
end

function DreamNotesBattlePanel:GetTitle(config)
    if config.title_func then
        return Format(config.title,config.title_func())
    end
    return config.title
end

function DreamNotesBattlePanel:GetTips(config,remain_round)
    local Tips = config.tips or Language.DreamNotes.BattleTip
    return remain_round >= 0 and Format(Tips, remain_round) or Language.DreamNotes.BattleTip2
end