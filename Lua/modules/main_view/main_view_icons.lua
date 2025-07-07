MainViewIconsOpen = MainViewIconsOpen or DeclareMono("MainViewIconsOpen", UIWFlushPanel)
function MainViewIconsOpen:MainViewIconsOpen()
    self.open_config = {
        Mod.Activity,
        Mod.Guild,
        Mod.Bicycle,
        Mod.Store,
        Mod.Reward,
        Mod.Achieve,
        Mod.Partner,
        Mod.Mall,
        Mod.Team,
        Mod.Illustrate,
        Mod.DivineCostume,
        Mod.TreasureHunt,
        Mod.Equip,
        Mod.Fabao,
        Mod.Workshop,
        Mod.Bag,
        Mod.Marbas
    }
    self.hide_list = {[Mod.Reward] = 1}
    self.handle_remind_list = {}
end
function MainViewIconsOpen:FlushRedPoint()
    local total = 0
    for index, value in ipairs(self.open_config) do
        local num = RemindCtrl.Instance:Remind():GetGroupNum(value)
        if self.hide_list[value] == 1 then
            num = 0
        end
        total = total + num
    end
    self.RedPoint:SetActive(total > 0)
end

function MainViewIconsOpen:OnEnable()
    UIWFlushPanel.OnEnable(self)
    for index, value in ipairs(self.open_config) do
        local flag = FunOpen.Instance:CheckOpen(value)
        if self.hide_list[value] == 1 then
            flag = false
        end 
        self.IconObjs[index]:SetActive(flag)
        local handle =
            RemindCtrl.Instance:Remind():RegisterGroup(
            value,
            function()
                local num = RemindCtrl.Instance:Remind():GetGroupNum(value)
                if self.hide_list[value] == 1 then
                    num = 0
                end
                self.RedObjs[index]:SetActive(num > 0)
            end,
            true
        )
        self.handle_remind_list[value] = handle
    end
end
function MainViewIconsOpen:OnDisable()
    for key, value in pairs(self.handle_remind_list) do
        RemindCtrl.Instance:Remind():UnRegisterGroup(key, value)
    end
    self.handle_remind_list = {}
end

MainViewIcons = MainViewIcons or DeclareMono("MainViewIcons", UIWFlushPanel)

function MainViewIcons:MainViewIcons(mod_name)
    self.config = {
        fight = FightFBView,
        illusion = IllusionView,
        illustration = IllustrateView,
        setting = RoleSettingView,
        reward = RewardView,
        fabao = FabaoView,
        achievement = AchieveNewView,
        family = "Family",
        team = TeamView,
        activity = ActiveView,
        workshop = Workshop,
        partner = PartnerView,
        task = TaskView,
        bag = BagView,
        store = StoreView,
        mall = MallView,
        athletics = AthleticsView,
        challenge = ChallengeNaviView,
        bicycle = BicycleView,
        divine_costume = DivineCostumeView,
        knight = KnightlyView,
        equip = EquipView,
        treasure_hunt = TreasureHuntView,
        marbas = MarbasView,
    }
    self.mv_data = MainViewData.Instance
    self.data_cares = {
        {data = self.mv_data:BaseData(), func = self.FlushGameState},
        {data = self.mv_data.view_data, func = self.HideLeftIcons, keys = {"hide_li"}, init_call = false},
        {data = self.mv_data.view_data, func = self.ShowLeftIcons, keys = {"show_all"}, init_call = false},
        {data = self.mv_data.view_data, func = self.ShowLeftIcons, keys = {"show_li"}, init_call = false}
    }
    self.open_config = {
        Mod.Activity,
        Mod.Guild,
        Mod.Bicycle,
        Mod.Store,
        Mod.Reward,
        Mod.Achieve,
        Mod.Partner,
        Mod.Mall,
        Mod.Team,
        Mod.Illustrate,
        Mod.DivineCostume,
        Mod.TreasureHunt,
        Mod.Task,
        Mod.Fabao,
        Mod.Workshop,
        Mod.Bag,
        Mod.Marbas
        -- Mod.Illusion,
        -- Mod.Setting,
    }
    self.hide_list = {[Mod.Reward] = 1}
    EventSys:Bind(GlobalEventKeys.TOUCH_BEGAN, BindTool.Bind(self.HideLeftIcon, self))
end
function MainViewIcons:HideLeftIcon()
    --[[ local is_battle = MainViewData.Instance:IsBattle()
    if is_battle and self.IconsAnim:GetBool("state") then
        self:OnClickArrow2(true, true)
    end ]]
end
--隐藏左侧栏
function MainViewIcons:HideLeftIcons()
    -- self.IconsAnim:SetBool("state", false)
end
--显示左侧栏
function MainViewIcons:ShowLeftIcons()
    -- self.IconsAnim:SetBool("state", true)
end

function MainViewIcons:FlushGameState()
    local is_battle = self.mv_data:IsBattle()
    -- self:OnClickArrow2(is_battle, true)
    -- LogDG(is_battle,"11111111111111111111111")
    self.GameObj:SetActive(not is_battle)
    --self.BattleObj:SetActive(is_battle)       -- 废弃了。改为BattleFuncBtns
    self.ActivityObj:SetActive(is_battle)
    self.BattleFuncBtns:SetActive(is_battle)
    -- self.ActArrow:SetActive(is_battle)

    self:LeftIconNormal()
    self:FlushBattleArrow()
end
function MainViewIcons:LeftIconNormal()
    -- LogDG("LeftIconNormal 0 ")
    self.IconsAnim:SetInteger("state", 0)
    self.battle_icon_state = 0
end

function MainViewIcons:FlushBattleArrow()
    local list = self.mv_data:GetBattleFuncBtnList()
    local num = #list
    local index = Valve(math.floor(num / 4),4)
    for i=1,self.BattlePos:Length() do
        self.BattlePos[i]:SetActive(i == index)
    end
end

function MainViewIcons:OnClickMods(mod_name)
    BehaviorData.Instance.auto_behavior = 0
    local view_name = self.config[mod_name]
    if mod_name == "family" then
        if GuildData.Instance:GetGuildID() > 0 then
            view_name = GuildView
            GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
        else
            -- 请求家族列表
            GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
            view_name = GuildList
        end
        ViewMgr:OpenView(view_name)
    elseif nil ~= view_name then
        ViewMgr:OpenView(view_name)
    end
end

function MainViewIcons:OnClickArrow1()
    local state = self.ChatAnim:GetBool("state")
    self.ChatAnim:SetBool("state", not state)
    local is_battle = MainViewData.Instance:IsBattle()
    if is_battle then
        --判断要隐藏活动还是功能
        -- LogDG(state)
        -- LogDG(self.IconsAnim:GetInteger("state"))
        self:LeftIconNormal()
        --[[ if not state then
            self.IconsAnim:SetBool("state", false)
        elseif self.auto_switch then
            self.IconsAnim:SetBool("state", true)
        end ]]
    end
end
--修改 state  0 1 2 3 隐藏活动，1展开功能 2 隐藏功能 3展开活动
function MainViewIcons:OnClickArrow2(state, skip)
    local is_battle = MainViewData.Instance:IsBattle()
    -- self.GameObj:SetActive(not is_battle)
    -- self.BattleObj:SetActive(not skip and is_battle)
    -- local state = nil == state and self.IconsAnim:GetBool("state") or state
    local state = nil == state and self.battle_icon_state or state
    -- self.IconsAnim:SetBool("state", not state)

    if self.battle_icon_state == 1 then
        state = 2
        self.battle_icon_state = 2
    else
        state = 1
        self.battle_icon_state = 1
    end
    self.IconsAnim:SetInteger("state", state)
    -- self.auto_switch = not state
    --[[ if is_battle and not state then
        self.ChatAnim:SetBool("state", false)
    end ]]
    if is_battle then
        self.ChatAnim:SetBool("state", false)
    end
    
    if is_battle then
        --检查是否显示红点
        if state then
            self:FlushRedPoint(self:GetRedPoint())
        else
            self:FlushRedPoint(false)
        end
    else
        self:FlushRedPoint(false)
    end
end
function MainViewIcons:OnClickArrow3()
    local is_battle = MainViewData.Instance:IsBattle()
    ---LogDG("OnClickArrow3", self.battle_icon_state)
    --展开活动图标列表
    -- self.ActivityObj:SetActive()
    if self.battle_icon_state == 3 then
        self.IconsAnim:SetInteger("state", 4)
        self.battle_icon_state = 4
    else
        self.IconsAnim:SetInteger("state", 3)
        self.battle_icon_state = 3
    end
    if is_battle then
        self.ChatAnim:SetBool("state", false)
    end
end
function MainViewIcons:GetRedPoint()
    -- local total = 0
    -- for index, value in ipairs(self.open_config) do
    --     local num = RemindCtrl.Instance:Remind():GetGroupNum(value)
    --     total = total + num
    -- end
    -- return (total > 0)
    local list = self.mv_data:GetBattleFuncBtnList()
    for i,v in pairs(list) do
        local mod = GetMod(v.mod)
        local num = RemindCtrl.Instance:Remind():GetGroupNum(mod)
        if num > 0 then
            return true
        end
    end
    return false
end

function MainViewIcons:FlushRedPoint(flag)
    for i=1,self.RedPoint:Length() do
        self.RedPoint[i]:SetActive(flag)
    end
end

function MainViewIcons:OnClickTestView(str)
    BehaviorData.Instance.auto_behavior = 0
    ViewMgr:OpenView(MonoTypeOf(str))
end
function MainViewIcons:OnClickFight()
    ViewMgr:OpenView(FightFBView)
end
function MainViewIcons:OnClickMallView()
    BehaviorData.Instance.auto_behavior = 0
    ViewMgr:OpenView(MallView)
end

MainViewIconsEnergy = MainViewIconsEnergy or DeclareMono("MainViewIconsEnergy", UIWFlushPanel)

function MainViewIconsEnergy:MainViewIconsEnergy()
    self.data = FightCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.fight_crystal_sweep, func = self.FlushEnergyValue},
        {data = self.data.energy_effect, func = self.FlushEnergyEffect, init_call = false},
        {data = self.data.fight_crystal_sweep, func = self.FlushEnergyEffect, init_call = true},
        {data = RoleData.Instance.base_data, func = self.FlushEnergyValue,init_call = false,keys = {"profession_id"}},
	}
end

function MainViewIconsEnergy:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.EffectTimer)
end

function MainViewIconsEnergy:FlushEnergyValue()
	local strength_num,max_power = self.data:GetFightStrengthNum()
    UH.SetText(self.EnergyValue,Format(Language.Fight.Sweep.desc,strength_num,max_power))
end

function MainViewIconsEnergy:FlushEnergyEffect()
    TimeHelper:CancelTimer(self.EffectTimer)
    if self.data:GetFightStrengthNum() >= self.data:GetCrystalFbSweepMaxPower() then
        self.EffectTimer = Invoke(function()
            self.effect_handle = UH.PlayEffect(self.EffectTool,[[100108214]])
        end, 8)
    end
end

MainViewIconsRedPoints = MainViewIconsRedPoints or DeclareMono("MainViewIconsRedPoints", UIWFlushPanel)

function MainViewIconsRedPoints:MainViewIconsRedPoints()
    self.config = {
        {btn = "Society", mod = Mod.Society},
        {btn = "FightGame", mod = Mod.FightFB, open = Mod.FightFB},
        {btn = "ActivityGame", mod = Mod.Activity, open = Mod.Activity.Daily},
        {btn = "RewardGame", mod = Mod.Store, open = Mod.Store},
        {btn = "MallGame", mod = Mod.Mall, open = Mod.Mall.Trade}
    }
    self.openes = {
        Society = true,
        FightGame = false,
        ActivityGame = false,
        RewardGame = false,
        MallGame = false
    }
    GuideManager.Instance:RegisterGetGuideUi(
        "ActivityViewBtn",
        function()
            MainViewData.Instance:ShowLeftButton()
            return self.ActivityGame, function()
                MainViewData.Instance:HideRbButton()
                ViewMgr:OpenViewByKey(Mod.Activity.Daily)
            end
        end
    )

    GuideManager.Instance:RegisterGetGuideUi(
        "FightBtn",
        function()
            MainViewData.Instance:ShowLeftButton()
            return self.FightGame, function()
                ViewMgr:OpenView(FightFBView)
            end
        end
    )

    GuideManager.Instance:RegisterGetGuideUi(
        "SocietyBtn",
        function()
            MainViewData.Instance:ShowLeftButton()
            return self.Society, function()
                ViewMgr:OpenView(SocietyView)
            end
        end
    )
end

function MainViewIconsRedPoints:Awake()
    UIWFlushPanel.Awake(self)
    for _, v in pairs(self.config) do
        local handle, red_point = v.btn .. "Handle", v.btn .. "RedPoint"
        if self[v.btn] then
            if self[red_point] == nil then
                self[red_point] = UiRedPoint.New(self[v.btn], Vector3.New(26, 26), false, 1)
                self[red_point]:SetNum(0)
            end
            if v.mod then
                RemindCtrl.Instance:Remind():UnRegisterGroup(v.mod, self[handle])
                self[handle] =
                    RemindCtrl.Instance:Remind():RegisterGroup(
                    v.mod,
                    function()
                        local num = RemindCtrl.Instance:Remind():GetGroupNum(v.mod)
                        self[red_point]:SetNum(num)
                    end,
                    true
                )
            end
            if v.open then
                FunOpen.Instance:Register(
                    v.open,
                    function(opened)
                        self[v.btn]:SetActive(opened)
                        self.openes[v.btn] = opened
                        self:CheckArrow()
                    end
                )
            end
        end
    end

    FunOpen.Instance:Register(
        Mod.FightFB.DreamNotes,	--这里防止与方面的Mod.FightFB注册重复导致覆盖，用里面的一个子系统注册
        function(opened)
            self.FightEnergy:SetActive(opened)
        end
    )

    FunOpen.Instance:Register(
        Mod.Rank.Main,
        function(opened)
            self.RankObj:SetActive(opened)
        end
    )
end

function MainViewIconsRedPoints:CheckArrow()
    local num = 0
    for _, opened in pairs(self.openes) do
        if opened then
            num = num + 1
        end
    end
    -- self.ArrowRep:SetActive(num < #self.config)
    -- self.ArrowPos:SetActive(num == #self.config)
    self.ArrowPos:SetActive(false)
    self.ArrowRep:SetActive(false)
end

--===========================MainViewIconRemindPanel===========================
MainViewIconRemindPanel = MainViewIconRemindPanel or DeclareMono("MainViewIconRemindPanel", UIWFlushPanel)
--ShowItemPar=====GameObject
function MainViewIconRemindPanel:MainViewIconRemindPanel()
    self.data = TopLayerActTipsData.Instance
    self.data_cares = {
        {data = RoleData.Instance.base_data, func = self.FlushIconRemind,init_call = false,keys = {"level"}},
        {data = MainViewData.Instance.base, func = self.FlushIconRemind, init_call = false,keys = {"is_battle"}},
        {data = TreasureMapData.Instance.view_data,func = self.FlushIconRemind,init_call = false, keys = {"val"}},
        {data = GuideData.Instance.notice_info, func = self.FlushIconRemind, init_call = false},
        {data = ActivityData.Instance.activity_status_event, func = self.FlushActStatus, keys = {"act_type", "status"},init_call = false},
        {data = GuideData.Instance.notice_info, func = self.FlushActStatus,init_call = true},
        {data = TaskData.Instance.task_list, func = self.FlushActEffect, init_call = false},
        {data = ActivityData.Instance.active_data, func = self.FlushActEffect, init_call = false},
        {data = GuideData.Instance.open_active_guide, func = self.FlushActGuideEffect, init_call = false},
        {data = ServerActivityData.Instance.login_reward_data, func = BindTool.Bind(self.FlushShowItemsPos,self),keys = {"reward_flag"}},
        {data = LunJianData.Instance:MyInfo(), func = self.FlushActStatus, keys = {"flush_act"}, init_call = true},
    }
end

function MainViewIconRemindPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:AddDelayFrameTimer(function ()
        self:FlushActEffect()
        self:FlushActTips()
    end,2)
end

function MainViewIconRemindPanel:FlushIconRemind()
    if not IS_AUDIT_VERSION or not GLOBAL_CONFIG:PkgInfo().is_youyan then
        self.RemindGame:SetActive(not ViewMgr:IsOpen(GuildQuestion) and
            not TreasureMapData.Instance.view_data.val and
            not MainViewData.Instance:IsBattle() and
            FunOpen.Instance:GetFunIsOpened(Mod.Activity.TimeLimit)
        )
    else
        self.RemindGame:SetActive(false)
    end
    self:FlushActEffect()
end

function MainViewIconRemindPanel:FlushActGuideEffect()
    if GuideData.Instance.open_active_guide.open == true then
        self.handle_act_guide = self.EffectTool:Play(2161049)
    else
        if self.handle_act_guide then
            self.EffectTool:Stop(self.handle_act_guide)
            self.handle_act_guide = nil
        end
    end
end

--活跃特效
function MainViewIconRemindPanel:FlushActEffect()
    if RoleData.Instance:GetBaseData().level == 31 then
        local mainline = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
        if mainline then
            local lock = mainline:TaskMenuCondition()
            if lock then
                local all_finish = true
                for index, value in ipairs(Config.activedegree_auto.active) do
                   if value.is_remind == 1 then
                        local num = ActivityData.Instance:GetProgressNum(value.act_type)
                        if num < value.integral_active then
                            all_finish = false
                        end
                   end
                end
                if all_finish == false then
                    self.ActObj:SetActive(true)
                     if self.handle_act_effect == nil then
                        self.handle_act_effect = self.EffectTool:Play(30000870)
                    end
                end
            end
        end
    else
        self.ActObj:SetActive(false)
    end
end

--刷新活动状态
function MainViewIconRemindPanel:FlushActStatus()
    self.Block:SetActive(#FunOpen.Instance:GetNoticeList() ~= 0)
    self:FlushIconRemind()
    -- if ActivityData.GetConfig(ActivityData.Instance:GetActivityEventType()) ~= nil then
        self:FlushActTips()
    -- end
end

--刷新限时活动提醒内容
function MainViewIconRemindPanel:FlushActTips()
    local param_t = ActivityRemindData.Instance:GetRemindList()
    local notice_show = false
    for i=1,self.ShowItem:Length() do
        if param_t[i] then
            self.ShowItem[i]:SetData(param_t[i])
        end
        if notice_show == false then
            notice_show = param_t[i] ~= nil
        end
        self.ShowItem[i]:SetObjActive(param_t[i] ~= nil)
    end
    if #param_t == 0 then
        ViewMgr:CloseView(ActivityRemindTip)
    end
    self.TipsGame:SetActive(#param_t ~= 0)
    self:FlushShowItemsPos(nil,notice_show)

end

local showitem_par_orginal_pos = nil
local showitem_par_sp_pos = nil
--左上角模型展示与活动提示按钮打架问题
function MainViewIconRemindPanel:FlushShowItemsPos(keys,notice_show)
    if notice_show == nil then
        local param_t = ActivityRemindData.Instance:GetRemindList()
        notice_show = param_t ~= nil and #param_t ~= 0
    end
    if notice_show then
        if showitem_par_orginal_pos == nil then
            showitem_par_orginal_pos = self.ShowItemPar:GetLocalPosition()
            showitem_par_sp_pos = Vector3.New(
                showitem_par_orginal_pos.x,showitem_par_orginal_pos.y-95,showitem_par_orginal_pos.z)
        end
        local list,list_type = FunOpen.Instance:GetNoticeList()
        local is_list = not TableIsEmpty(list)
        if is_list and list_type == 1 then      --模型展示界面出来了，按钮需要下移
            self.ShowItemPar:SetLocalPosition(showitem_par_sp_pos)
        else
            self.ShowItemPar:SetLocalPosition(showitem_par_orginal_pos)
        end
    end
end

----------------------------MainViewIconRemindItem----------------------------
MainViewIconRemindItem = MainViewIconRemindItem or DeclareMono("MainViewIconRemindItem", UIWidgetBaseItem)

function MainViewIconRemindItem:MainViewIconRemindItem()
    self.Time:SetCallBack(function()
        ActivityRemindData.Instance:DeleteRemindItem(self.param_t.act_info.act_type)
        self:SetObjActive(false)
    end)
end

function MainViewIconRemindItem:SetData(param_t)
    local is_open = ActivityRemindData.Instance:IsRemindShow(param_t.act_info,param_t.stamp_time)
    if is_open == true then
        if param_t.act_info.info.act_type == ActType.WorldArena and LunJianData.Instance:SeasonStatus() == LunJianData.Status.Signup then
            self.Time:SetTime(Language.Activity.LunJianBaoMingTime)
        else
            self.Time:StampTime(param_t.stamp_time,TimeType.Type_Special_11)
        end
        --[[ self.Time:StampTime(param_t.stamp_time,TimeType.Type_Special_11) ]]
        self:FlushRemindItem(param_t.act_info.info)
    end
    self:SetObjActive(is_open)
    self.param_t = param_t
end

function MainViewIconRemindItem:FlushRemindItem(data)
    local status = ActivityData.Instance:GetActivityStatus(data.act_type)
    if data.act_type == ActType.WorldArena and LunJianData.Instance:SeasonStatus() == LunJianData.Status.Signup then
        UH.SetText(self.State,Language.Activity.LunJianBaoMing)
    else
        UH.SetText(self.State,Language.Activity.ActRemindTips[status])
    end
    --[[ UH.SetText(self.State,Language.Activity.ActRemindTips[status]) ]]
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.ACT)
    self.Effect:SetActive(status == ActStatusType.Open)
    self:FlushBtnName(data.name_text)
end

function MainViewIconRemindItem:FlushBtnName(btn_name)
    if string.len(btn_name) > 9 then
        UH.SetText(self.Label,btn_name)
        UH.SetText(self.Title)
    else
        UH.SetText(self.Title,btn_name)
        UH.SetText(self.Label)
    end
end

function MainViewIconRemindItem:OnClickItem()
    if self.param_t.act_info.act_type == ActType.GhostNight then
        GhostNightCtrl.Instance:OnGhostNight()
        return
    end
    if self.param_t.act_info.act_type == ActType.WorldArena and LunJianData.Instance:SeasonStatus() == LunJianData.Status.Signup then
        ViewMgr:OpenViewByKey(Mod.LunJian.Signup)
        return
    end
    if not ActivityData.Instance:DoClickRemindHandle(self.param_t.act_info.act_type) then
        ViewMgr:OpenView(ActivityRemindTip, self.param_t)
    end
end

MainViewBattleActIconPanel = MainViewBattleActIconPanel or DeclareMono("MainViewBattleActIconPanel", UIWFlushPanel)
function MainViewBattleActIconPanel:MainViewBattleActIconPanel()
    self.mv_data = MainViewData.Instance
    self.rand_data = ActivityRandData.Instance
    self.data_cares = {
        {data = self.mv_data:BaseData(), func = self.FlushGameState},
        {data = self.rand_data.rand_open_data, func = self.OnFlush, init_call = false},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
    }
    self.act_main_auto = Config.activity_main_auto
    self.battle_show = self.act_main_auto.battle_show
end
function MainViewBattleActIconPanel:OnFlush()
    self:FlushGameState()
end
function MainViewBattleActIconPanel:FlushGameState()
    local is_battle = self.mv_data:IsBattle()
    self.ActArrow:SetActive(is_battle)
    if is_battle then--一开始切换到战斗时最多显示两个
        --显示最多两个GetRandInBattle
        local list = self.rand_data:GetRandInBattle()
        local index = Valve(math.ceil(#list / 4), 4)
        for i=1,self.ActPos:Length() do
            self.ActPos[i]:SetActive(i == index)
        end
        self.List:SetData(list)
        self.List2:SetData({list[1], list[2]})
    end
end
function MainViewBattleActIconPanel:OnDisable()
    self.ActArrow:SetActive(false)
end

MainViewBattleActIconItem = MainViewBattleActIconItem or DeclareMono("MainViewBattleActIconItem", UIWidgetBaseItem)
function MainViewBattleActIconItem:MainViewBattleActIconItem()
    self.rand_data = ActivityRandData.Instance
end
function MainViewBattleActIconItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushBtnItem(data)
end
function MainViewBattleActIconItem:FlushBtnItem(data)
    local base_func = self.rand_data:GetBaseItemData(data.mod_key)
    data.text = data.name
    local base_data = base_func and base_func(data) or nil
    local base_name = base_data and base_data.text or data.name
    local sprite_name = base_data and base_data.sprite or data.sprite

    if string.len(data.name) > 9 then
        UH.SetText(self.Space, base_name or data.name)
        UH.SetText(self.Label)
    else
        UH.SetText(self.Label, base_name or data.name)
        UH.SetText(self.Space)
    end
    UH.SpriteName(self.Icon, sprite_name or data.sprite)
end

function MainViewBattleActIconItem:OnClick()
    --活动自己的前往方式用ActivityRandData.Instance:CustomClickHandle注册
    if self.rand_data:OnClickHandle(self.data.act_type) then
        return
    end
    -- LogDG(self.data.mod_key)
    ViewMgr:OpenViewByKey(self.data.mod_key)
end





MainViewBattleFuncPanel = MainViewBattleFuncPanel or DeclareMono("MainViewBattleFuncPanel", UIWFlushPanel)
function MainViewBattleFuncPanel:MainViewBattleFuncPanel()
    self.config = Config.sundries_auto.fight_screen_button
end

function MainViewBattleFuncPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function MainViewBattleFuncPanel:FlushPanel()
    local list = MainViewData.Instance:GetBattleFuncBtnList()
    table.sort(list,function (a,b)
        return a.sort < b.sort
    end)
    self.List:SetData(list)
end

-- 战斗功能按钮
MainViewBattleFuncIcon = MainViewBattleFuncIcon or DeclareMono("MainViewBattleFuncIcon", UIWidgetBaseItem)
function MainViewBattleFuncIcon:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.text)
    local mod = GetMod(data.mod)
    self.mod = mod
    self:FlushRedNum()

    if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan and not StringIsEmpty(data.youyan_icon) then
		local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset(data.youyan_icon)
        if streaming_has and self.AuditRawIcon then
            elf.Icon:SetObjActive(false)
            self.RawIcon:SetObjActive(true)
			ChannelAgentConsts.SetRawImg(self.RawIcon, path)
		else
			self.Icon:SetObjActive(true)
            self.RawIcon:SetObjActive(false)
            UH.SpriteName(self.Icon, data.sprite)
        end
    else
        self.Icon:SetObjActive(true)
        self.RawIcon:SetObjActive(false)
        UH.SpriteName(self.Icon, data.sprite)
	end
end

function MainViewBattleFuncIcon:FlushRedNum()
    self:UnRegisterGroup()
    self.Red:SetActive(false)
    if self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(self.mod, function()
            self.Red:SetActive(Remind.Instance:GetGroupNum(self.mod) > 0)
        end, true)
    end
end

function MainViewBattleFuncIcon:OnDestroy()
    self:UnRegisterGroup()
end

function MainViewBattleFuncIcon:UnRegisterGroup()
    if self.handle then
        Remind.Instance:UnRegisterGroup(self.mod, self.handle)
        self.handle = nil
    end
end

function MainViewBattleFuncIcon:OnClick()
    if self.data.view == "GuildList" then
        local guild_id = GuildData.Instance:GetGuildID()
        if guild_id and guild_id > 0 then
            ViewMgr:OpenView(GuildView)
            return
        end
    end
    ViewMgr:OpenView(GetView(self.data.view))
end