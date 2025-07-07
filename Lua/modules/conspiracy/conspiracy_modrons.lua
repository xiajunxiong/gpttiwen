ConspiracyModrons =
    ConspiracyModrons or DeclareView('ConspiracyModrons', 'conspiracy/conspiracy_modrons', Mod.Modrons.Main)
VIEW_DECLARE_LEVEL(ConspiracyModrons, ViewLevel.L0)
function ConspiracyModrons:OnClickClose()
    ViewMgr:CloseView(ConspiracyModrons)
end
function ConspiracyModrons:CloseCallback()
    ConspiracyData.Instance:SetSelectScene(nil)
end

ConspiracyModronsPanel = ConspiracyModronsPanel or DeclareMono('ConspiracyModronsPanel', UIWFlushPanel)
function ConspiracyModronsPanel:ConspiracyModronsPanel()
    self.data = ConspiracyData.Instance
    self.data_cares = {
        {data = self.data:GetSelectScene(), func = self.FlushReward},
        {data = BagData.Instance.item_grid, func = self.FlushMedalRed},
        {data = self.data.fetch_flag, func = self.onFlush, init_call = false}
    }
end
function ConspiracyModronsPanel:OnClickTip()
    -- PublicPopupCtrl.ConditionTip(Language.Conspiracy.SaoDangTitle)
end
function ConspiracyModronsPanel:FlushReward()
    local data = self.data:GetSelectScene()
    if data:Val() ~= nil then
        self.CellList[1]:SetDataByItemIdWithoutZeroNum(data.reward_show)
        if not data.open and not data.level_open then
            self.InterGo.Interactable = false
        else
            self.InterGo.Interactable = true
        end
    end
end
function ConspiracyModronsPanel:onFlush()
    UH.SetText(self.SaoDang, Language.Conspiracy.SaoDang2)
    self.data.other_info.click_panel = true
    local data = self.data.scene
    local count = table.nums(data)
    for i = 1, count do
        self.ItemList[i]:SetData(data[i])
    end
    self.data:SetSelectScene(data[1])
    -- self.CellList[2]:SetDataByItemIdWithoutZeroNum(CommonItemId.Exp)
    -- self.CellList[3]:SetDataByItemIdWithoutZeroNum(CommonItemId.Coin)
    local total = self.data:GetOffList()[1].time
    local num = total - self.data:JoinTimes()
    num = num < 0 and 0 or num
    self.NumSTex.text = num
    self.DayHeight:SetActive(num ~= 0)
end
function ConspiracyModronsPanel:OnClickGo()
    local info = self.data:GetSelectScene()
    if info:Val() ~= nil then
        if info.open == false then
            if info.level_open == false then
                PublicPopupCtrl.Instance:Center(Language.Conspiracy.TipLevel)
                return
            end

            if info.task_finish == false then
                ViewMgr:CloseView(ConspiracyModrons)
                ViewMgr:OpenView(MedalTaskView)
                PublicPopupCtrl.Instance:Center(string.format(Language.Conspiracy.TipTask, info.task_name))
                return
            end

            return
        end
        BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(info.behavior_id, true))
        ViewMgr:CloseView(ConspiracyModrons)
    end
end
function ConspiracyModronsPanel:FlushMedalRed()
    self.red_point = self.red_point or UiRedPoint.New(self.BuildRed, Vector3.zero)
    self.red_point:SetNum(MedalData.Instance:GetMedalRedNumQuick())
end
function ConspiracyModronsPanel:OnClcikCheck()
    local data = self.data:GetSelectScene()
    local param_t = {}
    param_t.title_name = string.format(Language.Conspiracy.RewardTitle, data.level)
    param_t.tab_list = Language.Conspiracy.TabList
    param_t.show_list = self.data:GetShowList(data.index)
    param_t.empty_lab = Language.Conspiracy.ShowListEmpty
    param_t.is_with = true
    --[[ param_t.cell_func = function (data)
		LogError("click!",data.name)
    end ]]
    ViewMgr:OpenView(MedalEffectShow, param_t)
    -- PublicInfoShowCtrl.Instance:SetPublicPageShow(param_t)
end

function ConspiracyModronsPanel:OnClcikExchange()
    ViewMgr:OpenViewByKey(Mod.Medal.Build)
    ViewMgr:CloseView(ConspiracyModrons)
end

ConspiracyModronsItem = ConspiracyModronsItem or DeclareMono('ConspiracyModronsItem', UIWidgetBaseItem)
function ConspiracyModronsItem:SetData(data)
    if self.NameList then
        local temp = DataHelper.GetStringWordToTable(data.name)
        for i = 1, 4 do
            UH.SetText(self.NameList[i], temp[i] or "")
        end
        UH.SetText(self.LevelTex, data.level)
    else
        UH.SetText(self.NameTex, data.name)
        UH.SetText(self.LevelTex, string.format(Language.Common.Levels, data.level))
    end
    UH.SetText(self.LockLevel, string.format(Language.Conspiracy.LockLevel, data.level))
    UH.SetText(self.LimitTex, data.description)
    UH.SetText(self.LimitTex2, data.description)
    UH.SpriteName(self.IconSp, data.icon_id)
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_finish = TaskData.Instance:GetTaskIsCompleted(data.pre_task)
    local open = (role_level >= data.level and is_finish) == true
    data.task_finish = (is_finish == true)
    data.open = open
    data.level_open = ((role_level >= data.level) == true)
    UIWidgetBaseItem.SetData(self, data)
    --判断等级和任务是否达标
    if ConspiracyData.Instance:OpenTask() == data.pre_task then
        --播放解锁动画
        self.EffectTool:Play('10010860', self.gameObject)
        -- self.Lock:SetActive(true)
        -- self.Inter.Interactable = false
        -- self.Inter2.Interactable = false
        self.handle_delay =
            TimeHelper:AddDelayTimer(
            function()
                self:CheckLock()
            end,
            1
        )
        ConspiracyData.Instance:SetOpenTask(-1)
    else
        self:CheckLock()
    end
    local total = 7
    for i = 1, total do
        if ConspiracyData.Instance:WeekPass(data.index, i) then
            total = total - 1
        end
    end
    UH.SetText(self.TipTex, string.format(Language.Conspiracy.KillTip, total))
    local num = ConspiracyData.Instance:PassAndNotGet(data.index)
    self.RedPoint:SetNum(num)
end
function ConspiracyModronsItem:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    ConspiracyData.Instance:SetOpenTask(-1)
end
function ConspiracyModronsItem:CheckLock()
    --self.Lock:SetActive(not self.data.open)
    self.Inter.Interactable = self.data.open
    self.Inter2.Interactable = self.data.level_open
    self.Lock:SetActive(not self.data.level_open)
    self.Lock2:SetActive(self.data.level_open and not self.data.task_finish)
    self.TipObj:SetActive(self.data.open)
end
function ConspiracyModronsItem:OnClickSelect()
    self.SelectToggle.isOn = true
    ConspiracyData.Instance:SetSelectScene(self.data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end
function ConspiracyModronsItem:OnClickCheck()
    ConspiracyData.Instance:SetSceneID(self.data.scene_id)
    ViewMgr:OpenView(ConspiracyWeekFirstKill)
end
