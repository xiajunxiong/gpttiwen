
BunComeView = BunComeView or DeclareView("BunComeView", MayDayConfig.ResPath .. "bun_come", Mod.MayDay.BunCome)
-- VIEW_DECLARE_LEVEL(BunComeView, ViewLevel.L0)

function BunComeView:BunComeView()
    PeriodActivityData.Instance:SetRemindData(Mod.MayDay.BunCome)
end

function BunComeView:LoadCallback()
end

BunComeViewF = BunComeViewF or DeclareMono("BunComeViewF", UIWFlushPanel)

function BunComeViewF:BunComeViewF()
    self.data = MayDayCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.MayDay.BunCome

    self.data_cares = {
        {data = self.data.bun_come_info, func = self.FlushInfoShow},
        {data = self.data.bun_come_sm, func = self.FlushStateShow, keys = {"state"}},
        {data = self.data.bun_come_sm, func = self.FlushNumShow, keys = {"num"}, init_call = false}
    }

    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

    self.OffsetX = 0
    self.data:ResetBunComeInfo()

    self.SPAWN_INTERVAL = 1
    self.SPAWN_NUM_MAX = 3
end

function BunComeViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushShow()
end

function BunComeViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_tt)
    TimeHelper:CancelTimer(self.timer_rt)
    if MayDayConfig.BunComeState.Play == self.data.bun_come_sm.state then
        self:GameEnd(false)
    end
end

function BunComeViewF:FlushShow()
    local reward_show = self.data:BunComeRewardShow()
    self.RewardList:SetData(reward_show)

    self.TimeShow.text = string.format(self.language.TimeShow, self.data:ActTimeShow(ACTIVITY_TYPE.RAND_BUN_COME))
    self.BubbleGameShow.text = self.language.BubbleGameShow
    self.BubbleReadyShow.text = self.language.BubbleReadyShow
    self.GameTips.text = self.data:BunComeConfigOtherDesc()
end

function BunComeViewF:FlushInfoShow()
    local count = self.data.bun_come_info.count
    self.StartCount.text = string.format(self.language.StartCountShow, self.data.bun_come_sm.total - count, self.data.bun_come_sm.total)
end

function BunComeViewF:FlushStateShow()
    local state = self.data.bun_come_sm.state
    local bcs = MayDayConfig.BunComeState
    self.BtnStartObj:SetActive(bcs.Ready == state)
    self.CountDownObj:SetActive(bcs.Ready ~= state)
    self.PlateBeginDrag.enabled = bcs.Play == state
    self.PlateDrag.enabled = bcs.Play == state
    self.PlateBuns:SetActive(bcs.Ready ~= state)
    self.BubbleReady:SetActive(bcs.Ready == state)
    self.BubbleGame:SetActive(bcs.Ready ~= state)

    TimeHelper:CancelTimer(self.timer_tt)
    TimeHelper:CancelTimer(self.timer_rt)
    if bcs.Play == state then
        local time = self.data.bun_come_sm.game.time + 1
        self.timer_tt = TimeHelper:AddCountDownTT(function ()
            time = time - 1
            self.CountDownShow.text = string.format(self.language.CountDownShow, time)
        end, function ()
            self.CountDownShow.text = string.format(self.language.CountDownShow, 0)
            self:GameEnd(false)
        end, time)

        -- self:SpawnItems()
        self.timer_rt = TimeHelper:AddRunTimer(BindTool.Bind(self.SpawnItems, self), self.SPAWN_INTERVAL)
    end
end

function BunComeViewF:SpawnItems()
    local spawn_num = math.random(1, self.SPAWN_NUM_MAX)
    for i = 1, spawn_num do
        local item_obj = self.SpawnItem.gameObject:Clone(self.SpawnParent)
        local item_mono = item_obj:GetLuaComponent(self.SpawnItem:GetTypeName())
        item_mono:SetData({type = math.random(1, 2)})
    end
end

function BunComeViewF:FlushNumShow()
    local num = self.data.bun_come_sm.num
    for i = 1, self.BunObjs:Length() do
        self.BunObjs[i]:SetActive(num >= i)
    end
    if num >= self.data.bun_come_sm.game.reward_number then
        self:GameEnd(num >= self.data.bun_come_sm.game.reward_number)
    end
end

function BunComeViewF:GameStart()
    local succ, game = self.data:BunComeGameRandom()
    if succ then
        self.GoalNumShow.text = game.reward_number
        self.data.bun_come_sm.game = game
        self.data.bun_come_sm.state = MayDayConfig.BunComeState.Play
    end
end

function BunComeViewF:GameEnd(is_win)
    if MayDayConfig.BunComeState.Play ~= self.data.bun_come_sm.state then
        return
    end
    TimeHelper:CancelTimer(self.timer_tt)
    TimeHelper:CancelTimer(self.timer_rt)
    self.data.bun_come_sm.state = MayDayConfig.BunComeState.End
    MayDayCtrl.Instance:SendRABunComeGet(is_win)
end

function BunComeViewF:OnBeginDrag()
    local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
    self.OffsetX = localPoint.x - self.PlatePos.localPosition.x
end

function BunComeViewF:OnDrag()
    local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
    local to_x = localPoint.x - self.OffsetX
    to_x = math.min(math.max(-186, to_x), 346)
    self.PlatePos.localPosition = Vector2.New(to_x, -76.5)
end

function BunComeViewF:OnClickStart()
    if self.data.bun_come_info.count < self.data.bun_come_sm.total then
        AudioMgr:PlayEffect(AudioType.Scene, "enter_game")
        self:GameStart()
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        PublicPopupCtrl.Instance:Center(self.language.CountNotEnough)
    end
end

function BunComeViewF:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    if MayDayConfig.BunComeState.Play == self.data.bun_come_sm.state then
        PublicPopupCtrl.Instance:DialogTips{
            content = self.language.GiveUpTips,
            confirm = {
                func = function ()
                    self:GameEnd(false)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    else
        ViewMgr:CloseView(BunComeView)
    end
end


BunComeViewSpawnItem = BunComeViewSpawnItem or DeclareMono("BunComeViewSpawnItem", UIWidgetBaseItem)
function BunComeViewSpawnItem:BunComeViewSpawnItem()
    self.md_data = MayDayCtrl.Instance:Data()
end

function BunComeViewSpawnItem:OnDestroy()
    TimeHelper:CancelTimer(self.timer_dt)
end

function BunComeViewSpawnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local parent_size = self.ParentPos.rect
    self.ItemPos.localPosition = Vector2.New(-parent_size.width / 2 + parent_size.width * (math.random(0, 10) / 10), parent_size.height)
    self.ItemSp.SpriteName = MayDayConfig.BunComeSp[data.type]
    self.ItemPos:SetObjActive(true)
    local anim_type = math.random(1, 3)
    self.ItemAnim:Play("bun_come" .. anim_type)

    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:JudgeAndDisappear()
    end, MayDayConfig.BunComeAnimTime[anim_type])
end

function BunComeViewSpawnItem:JudgeAndDisappear()
    if 1 == self.data.type and math.abs(self.PlatePos.localPosition.x - (self.ItemPos.localPosition.x + 80)) < 100 then
        if MayDayConfig.BunComeState.Play == self.md_data.bun_come_sm.state then
            local num = self.md_data.bun_come_sm.num
            self.md_data.bun_come_sm.num = num + 1
        end
    end
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end
