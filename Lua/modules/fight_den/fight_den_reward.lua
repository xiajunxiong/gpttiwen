FightDenReward = FightDenReward or DeclareView("FightDenReward", "fight_den/fight_den_reward")

function FightDenReward:FightDenReward()
    self.lang = Language.FightDen
    self.data = FightDenData.Instance
end

function FightDenReward:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title, Vector2.New(810,473))
end

function FightDenReward:OnFlush()
    self.data:SetLevel(self.data:ResultLevel())
    local reward_list = self.data:GetShowReward(nil,nil,true)
    --万妖谷特殊处理 写死
    if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_WAVE then
        local result = self.data:ResultLevel()--GetLeaderLevel()
        self.NotCanTip:SetActive(false)--result ~= nil)
        reward_list = self.data:GetShowReward(result, nil, true)
        for k,v in pairs(reward_list) do
            v.not_can_reward = result
        end
    end
    self.ItemList:SetData(reward_list)
end

FightDenItem = FightDenItem or DeclareMono("FightDenItem", UIWidgetBaseItem)

function FightDenItem:FightDenItem()
    self.lang = Language.FightDen
    self.den_data = FightDenData.Instance
end

function FightDenItem:SetData(data)
    UH.SetText(self.Level,string.format( self.lang.Level,data.num))
    local item_list = {}
    for _,v in ipairs(data.reward_item_list) do
        table.insert(item_list, Item.Init(tonumber(v[1]), tonumber(v[2]), tonumber(v[3])))
    end
    self.ItemList:SetData(item_list)
    self.Finish:SetActive(self.den_data:GetWave().num > data.num and data.not_can_reward == nil)
end

FightDenReward2 = FightDenReward2 or DeclareView("FightDenReward2", "fight_den/fight_den_reward2",Mod.WanYaoGu.Mian)
function FightDenReward2:FightDenReward2()
    self.lang = Language.FightDen
    self.data = FightDenData.Instance
    self.data.cur_view_select_mod = 0
    GuideManager.Instance:RegisterGetGuideUi("FightDenBtnEnter", function()
        return self.BtnEnter, function ()
            self:OnClickEnter()
        end
    end)
    self.first_flush = true
    FightDenCtrl.Instance:SendAchievementReq(0)
end

function FightDenReward2:FlushShowList()
    self.ItemList:SetData(self.data:GetShowReward(nil, self.data.cur_view_select_mod))
end

function FightDenReward2:OnFlush()
    local result = self.data:ResultLevel()
    self.data:SetLevel(result)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)

    if self.first_flush then
        self.handle_delay2 = TimeHelper:AddDelayTimer(function ()
            self.first_flush = false
            self:FlushShowList()
        end,0.5)
    else
        self:FlushShowList()
    end
    local cfg =  ActivityData.GetConfig(ActType.FightDen)
    UH.SetText(self.LblTitle, cfg.name_text)
    local level = self.data:GetLeaderLevel()
    UH.SetText(self.LblLevel, string.format(Language.Main.Battle.Ji, level))
    UH.SetText(self.LblMax, string.format(self.lang.MaxWave, self.data:GetMaxWave()))
    UH.SetText(self.LblDesc, self.lang.TaskDesc)
    -- self.handle_delay = TimeHelper:AddDelayTimer(function ()
    --     self:AutoBattle()
    -- end, TeamConfig.AutoTime)
end
function FightDenReward2:AutoBattle()
    local dragon = TeamCtrl.Instance:Dragon()
    if dragon then
        if dragon:Step() > 2 then
            self:OnClickEnter()
        end
    end
end
function FightDenReward2:CloseCallBack()
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
end
function FightDenReward2:OnClickEnter()
    TeamCtrl.Instance:TeamDetectByActType(ActType.FightDen,function ()
        self.data:SetWave(1)--强行设置
        local param_t = {}
        param_t.battle_mode = BATTLE_MODE.BATTLE_MODE_WAVE
        param_t.param1 = self.data.cur_view_select_mod
        BattleCtrl.BattleModeFightReq(param_t)
        ViewMgr:CloseView(FightDenReward2)
    end)
end

function FightDenReward2:OnClickToggle(index)
    self.data.cur_view_select_mod = index
    self:OnFlush()
end

function FightDenReward2:OnClickAchieve()
    ViewMgr:OpenView(FightDenAchieveView)
end

FightDenReward2Panel = FightDenReward2Panel or DeclareMono("FightDenReward2Panel", UIWFlushPanel)
function FightDenReward2Panel:FightDenReward2Panel()
    self.data = FightDenData.Instance
    self.data_cares = {
        {data = self.data.achievement_smart, func = self.FlushAchieveRedpoint, keys = {"achieve_state"}}
    }
end

function FightDenReward2Panel:FlushAchieveRedpoint()
    local remind = self.data:RemindAchievement()
    if self.redpoint ~= nil then
        self.redpoint:SetNum(remind)
    elseif remind > 0 and self.redpoint == nil then
        self.redpoint = UiRedPoint.New(self.BtnAchieveObj, Vector3.New(27, 26,0))
        self.redpoint:SetNum(remind)
    end
end

----------------------------------------------------
FightDenItem2 = FightDenItem2 or DeclareMono("FightDenItem2", UIWidgetBaseItem)

function FightDenItem2:FightDenItem2()
    self.lang = Language.FightDen
    self.den_data = FightDenData.Instance
end

function FightDenItem2:SetData(data)
    UH.SetText(self.LblWave, string.format( self.lang.Level,data.num))
    self.ItemList:SetData(data.reward_item_list)
    --小于等于显示已领取
    local temp_max_wave = math.max(self.den_data.pass_wave[0], self.den_data.pass_wave[1])
    -- local pass = self.den_data:GetRewardFlag(self.den_data.cur_view_select_mod, data.num)
    --LogError("data.num",data.num,pass)
    -- self.SuiJiObj:SetActive(pass == 0)
    self.GetObj:SetActive(temp_max_wave >= data.num)
end

function FightDenReward2:OnClickClose()
    ViewMgr:CloseView(FightDenReward2)
end
---------------------------------------
FightDenRewardCellItem = FightDenRewardCellItem or DeclareMono("FightDenRewardCellItem", UIWidgetBaseItem)
function FightDenRewardCellItem:FightDenRewardCellItem()
end

function FightDenRewardCellItem:SetData(data)
    self.data = data
    self.CornerObj:SetActive(not StringIsEmpty(data.rate))
    if not StringIsEmpty(data.rate) then
        UH.SetText(self.RateText, data.rate)
    end
    self.ItemCell:SetData(Item.Init(tonumber(data[1]), tonumber(data[2]), tonumber(data[3])))
end

------------------------------------
FightDenAchieveView = FightDenAchieveView or DeclareView("FightDenAchieveView", "fight_den/fight_den_achieve")
VIEW_DECLARE_MASK(FightDenAchieveView, ViewMask.BgBlock)
function FightDenAchieveView:FightDenAchieveView()
    self.data = FightDenData.Instance
end

function FightDenAchieveView:LoadCallback()
    self.RewardList:SetData(Config.monster_cave_auto.achievement)
end

function FightDenAchieveView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[415].desc})
end

function FightDenAchieveView:OnCloseClick()
    ViewMgr:CloseView(FightDenAchieveView)
end

-----------------------------
FightDenAchieveItem = FightDenAchieveItem or DeclareMono("FightDenAchieveItem", UIWidgetBaseItem)
function FightDenAchieveItem:FightDenAchieveItem()
end

function FightDenAchieveItem:OnDestroy()
    if self.state_care_handle then
        FightDenData.Instance.achievement_smart:Uncare(self.state_care_handle)
        self.state_care_handle = nil
    end
end

function FightDenAchieveItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.ConditionText, Format(data.name, data.num))
    if self.state_care_handle == nil then
        self.state_care_handle = FightDenData.Instance.achievement_smart:Care(function ()
            self:FlushState()
        end,"achieve_state")
    end
    local reward_list = {}
    for i = 0, #data.extra_reward + 1 do
        if data.extra_reward[i] then
            table.insert(reward_list, Item.Init(data.extra_reward[i].item_id, data.extra_reward[i].num, data.extra_reward[i].is_bind))
        end
    end
    self.RewardList:SetData(reward_list)
    self:FlushState()
end
function FightDenAchieveItem:FlushState()
    self.state = FightDenData.Instance:GetRewardState(self.data.index, self.data.num)
    self.Flag1:SetActive(self.state == FightDenData.AchievementState.HasGet)
    self.Flag0:SetActive(self.state == FightDenData.AchievementState.NotEnough)
    self.FlagBtn:SetActive(self.state == FightDenData.AchievementState.CanGet)
    
end

function FightDenAchieveItem:OnGetClick()
    if self.state == FightDenData.AchievementState.CanGet then
        FightDenCtrl.Instance:SendGetAchievenmentReward(self.data.index)
    end
end