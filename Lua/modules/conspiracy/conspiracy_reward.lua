ConspiracyReward =
    ConspiracyReward or DeclareView("ConspiracyReward", "conspiracy/conspiracy_reward", Mod.Conspiracy.Main)
VIEW_DECLARE_LEVEL(ConspiracyReward, ViewLevel.L1)

function ConspiracyReward:ConspiracyReward()
    self.lang = Language.Conspiracy
end
function ConspiracyReward:LoadCallback()
    self.data = ConspiracyData.Instance
    local total = self.data:GetOffList()[1].time
    local num = self.data:JoinTimes()
    num = num > total and total or num
    local title = string.format(self.lang.Title2, num, total)
    self.Board:SetData(self:GetType(), title, Vector2.New(982, 665))
end

ConspiracyRewardPanel = ConspiracyRewardPanel or DeclareMono("ConspiracyRewardPanel", UIWFlushPanel)
function ConspiracyRewardPanel:ConspiracyRewardPanel()
    self.data = ConspiracyData.Instance
    self.lang = Language.Conspiracy
    self.data_cares = {
        {data = SceneData.Instance.base, func = self.onFlush,keys = {"scene_id"}},
    }
end
function ConspiracyRewardPanel:onFlush()
    local info = self.data:SceneInfo(SceneData.Instance:SceneId())
    if info == nil then
        ViewMgr:CloseView(ConspiracyReward)
        return 
    end

    local data = self.data:BossListByIndex(info.index)

    self.List:SetData(data)
    --[[ local pass = true
    for index, value in ipairs(data) do
        pass = self.data:WeekPass(value.seq + 1, value.index)
        if pass == false then
            self.List:ClickItem(index)
            break
        end
    end
    if pass == true then ]]
        local alive = false
        for index, value in ipairs(data) do
            alive = self.data:KillFlag(value.index)
            if alive == true then
                self.List:ClickItem(index)
                break
            end
        end
        if alive == false then
            self.List:ClickItem(1)
        end
    --[[ end ]]
    --刷新时间
    --剩余时间
    local w = tonumber(os.date("%w", os.time()))
    w = w == 0 and 7 or w
    local h = os.date("%H", os.time())
    UH.SetText(self.Time, string.format(self.lang.Time, 7 - w, 24 - h))

    local join_times = self.data:JoinTimes()
    local off_list = self.data:GetOffList()
    UH.SetText(self.Desc, string.format(self.lang.Desc, join_times))--, off_list[1].time, off_list[2].time))

    self.ItemCell:SetDataByItemIdWithoutZeroNum(info.reward_show)
    --UnityEngine.UI.ClipperRegistry.instance:Register(self.Mask2D);
    --UnityEngine.UI.MaskUtilities:Notify2DMaskStateChanged(self.Mask2D);
end
function ConspiracyRewardPanel:OnClickBoss(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.choose_info = data
    local reward = {}
    local group = self.data:PassReward(data.level_group_id)
    local item
    for _, v in ipairs(group) do
        item = Item.Create({item_id = tonumber(v), num = 1})
        table.insert(reward, item)
    end
    self.RewardList:SetData(reward)
    UH.SetText(self.Title, string.format(self.lang.LevelTitle, DataHelper.GetDaXie(data.seq + 1), data.level))
    --[[ local kill = true
    SceneCtrl.Instance:Objs():ForeachSNPC(function (obj)
        if obj.vo.id == self.choose_info.npc_id then
            kill = false
            return true
        end
    end)
    self.Inter.Interactable = not kill ]]
    local cfg = NpcHelper.NpcCo(data.npc_id)
    UH.SetIcon(self.BossIcon, cfg.npc_icon)
    local pass = self.data:WeekPass(data.seq + 1, data.index)
    self.KillObj:SetActive(pass)
end
function ConspiracyRewardPanel:OnClickGoto()
    if self.choose_info.scene_id ~= SceneData.Instance:SceneId() then
        PublicPopupCtrl.Instance:Center(ErrorText[78])
        return
    end
    --判断是否存在
    SceneLogic.Instance:AutoToNpc(CommonStruct.MonsterVo(self.choose_info), nil, true)
    ViewMgr:CloseView(ConspiracyReward)
    self.data:SetChooseBoss(self.choose_info)
end
function ConspiracyRewardPanel:OnClickChaKan()
    local param_t = {}
    param_t.title_name = string.format(Language.Conspiracy.RewardTitle2, self.choose_info.name)
    param_t.tab_list = Language.Conspiracy.TabList
    param_t.show_list = self.data:GetBossShowList(self.choose_info.sp_group)
    param_t.empty_lab = Language.Conspiracy.ShowListEmpty
    ViewMgr:OpenView(MedalEffectShow,param_t)
    -- PublicInfoShowCtrl.Instance:SetPublicPageShow(param_t)
end

ConspiracyRewardItem = ConspiracyRewardItem or DeclareMono("ConspiracyRewardItem", UIWidgetBaseItem)
function ConspiracyRewardItem:ConspiracyRewardItem()
    self.con_data = ConspiracyData.Instance
end
function ConspiracyRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cfg = NpcHelper.NpcCo(data.npc_id)
    UH.SetText(self.Name, data.name)
    UH.SetIcon(self.Icon, cfg.npc_icon)

    local first = self.con_data:FirstKill(data.seq + 1, data.index)
    local pass = self.con_data:WeekPass(data.seq + 1, data.index)
    local get = self.con_data:CanGet(data.seq + 1, data.index)
    self.Get:SetActive(get)
    self.Kill:SetActive(pass)
    self.CanGet:SetActive(not get and pass) 
    local real_index = self.data.seq * 7 + self.data.index
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_MI_YI, real_index)
    if video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, video))
        self:FlushGray(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
    end
end
function ConspiracyRewardItem:SetupAllComplete()
    local reward = {}
    --[[ local group = string.split(self.data.first_reward_id, "|")
    for _, v in ipairs(group) do
        local list = self.con_data:RewardList(tonumber(v))
        for _, value in ipairs(list) do
            table.insert(reward, value)
        end
    end ]]
    table.insert(reward, Item.Create(self.data.weekly_reward[0]))
    table.insert(reward, Item.Create({item_id = CommonItemId.Exp, num = self.data.exp}))
    table.insert(reward, Item.Create({item_id = CommonItemId.Coin, num = self.data.coin}))
    self.List:SetData(reward)
end
function ConspiracyRewardItem:OnDestroy()
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    TimeHelper:CancelTimer(self.handle_delay)
end
function ConspiracyRewardItem:FlushGray(video)
    local can_play = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0)
    self.Gray.enabled = can_play == 0
end
function ConspiracyRewardItem:OnClickVideo()
    local index = self.data.seq * 7 + self.data.index
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_MI_YI, index)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id)
    end
end
function ConspiracyRewardItem:OnClickGetReward()
    ConspiracyCtrl.CSMiYiReq(0, self.data.scene_id, self.data.npc_id)
end
function ConspiracyRewardItem:Click()
    if self.BossClick then
        self.BossClick:Trigger()
    end
end

--------------------------------ConspiracyPreview--------------------------------
ConspiracyPreview = ConspiracyPreview or DeclareView("ConspiracyPreview", "conspiracy/conspiracy_preview")

VIEW_DECLARE_MASK(ConspiracyPreview, ViewMask.BlockClose)
function ConspiracyPreview:ConspiracyPreview()
    self.data = ConspiracyData.Instance
end

function ConspiracyPreview:OnFlush()
    local index = BattleData.Instance:BaseData().monster_group_id
    local info = self.data:SceneInfo(SceneData.Instance:SceneId())
    local reward = self.data.dexter_reward[index]
    if reward == nil then
        ViewMgr:CloseView(ConspiracyPreview)
        return
    end
    self.choose_info = reward
    --获取当前boss
    local reward_list = {}
    local group = self.data:PassReward(reward.level_group_id)
    local item
    for _, v in ipairs(group) do
        item = Item.Create({item_id = tonumber(v), num = 1})
        table.insert(reward_list, item)
    end
    self.CellList[1]:SetDataByItemIdWithoutZeroNum(info.reward_show)
    for i = 2, 7 do
        self.CellList[i]:SetData(reward_list[i - 1])
    end
end

function ConspiracyPreview:OnClickChaKan()
    local param_t = {}
    param_t.title_name = string.format(Language.Conspiracy.RewardTitle2, self.choose_info.name)
    param_t.tab_list = Language.Conspiracy.TabList
    param_t.show_list = self.data:GetBossShowList(self.choose_info.sp_group)
    param_t.empty_lab = Language.Conspiracy.ShowListEmpty
    -- PublicInfoShowCtrl.Instance:SetPublicPageShow(param_t)
    ViewMgr:OpenView(MedalEffectShow,param_t)
end
function ConspiracyPreview:OnClickClose()
    ViewMgr:CloseView(ConspiracyPreview)
end

ConspiracyWeekFirstKill = ConspiracyWeekFirstKill or DeclareView("ConspiracyWeekFirstKill", "conspiracy/conspiracy_wfk")
function ConspiracyWeekFirstKill:ConspiracyWeekFirstKill()
    
end
function ConspiracyWeekFirstKill:LoadCallback()
    self.Board:SetData(self:GetType(),Language.Conspiracy.WFKTitle,Vector2.New(672, 742))
end

ConspiracyWeekFirstKillPanel = ConspiracyWeekFirstKillPanel or DeclareMono("ConspiracyWeekFirstKillPanel", UIWFlushPanel)
function ConspiracyWeekFirstKillPanel:ConspiracyWeekFirstKillPanel()
    self.data = ConspiracyData.Instance
    self.lang = Language.Conspiracy
    self.data_cares = {
        {data = SceneData.Instance.base, func = self.onFlush, keys = {"scene_id"}, init_call = false},
        {data = self.data.fetch_flag, func = self.onFlush},
    }
end
function ConspiracyWeekFirstKillPanel:onFlush()
    local scene_id = self.data:SceneID() or SceneData.Instance:SceneId()
    local info = self.data:SceneInfo(scene_id)
    if info == nil then
        ViewMgr:CloseView(ConspiracyWeekFirstKill)
        return 
    end
    local data = self.data:BossListByIndex(info.index)
    self.List:SetData(data)
    if info.can_use_pass == 0 then
        self.BtnInter:SetObjActive(false)
        self.TipObj:SetActive(false)
        return
    end
    self.BtnInter:SetObjActive(true)
    self.TipObj:SetActive(true)
    local all = self.data:AllWeekKill()
    self.RedPoint:SetNum(0)
    if all then
        UH.SetText(self.SaoDang, self.lang.AllWeekKill)
        self.BtnInter.Interactable = false
    else
        local can , value = self.data:CanSweep()
        if can then
            self.TipObj:SetActive(false)
            self.BtnInter.Interactable = true
            self.RedPoint:SetNum(1)
        else
            self.BtnInter.Interactable = false
            UH.SetText(self.SaoDang, value.tip)
            self.TipObj:SetActive(true)
        end
    end
    -- UH.SetText(self.SaoDang, string.format(self.lang.SaoDang1, info.pass_score))
end
function ConspiracyWeekFirstKillPanel:OnClickSaoDang()
    local scene_id = self.data:SceneID() or SceneData.Instance:SceneId()
    if self.BtnInter.Interactable == true then
        ConspiracyCtrl.CSMiYiReq(1, scene_id)
        self.Block:SetActive(true)
        self.EffectTool:Play(5161006)
    else
        local list =  self.data:SweepTip()
        PublicPopupCtrl.ConditionTip(Language.Conspiracy.SaoDangTitle, list)
    end
end
function ConspiracyWeekFirstKillPanel:OnEffectEvent(handle)
    self.EffectTool:Stop(handle)
    self.Block:SetActive(false)
end