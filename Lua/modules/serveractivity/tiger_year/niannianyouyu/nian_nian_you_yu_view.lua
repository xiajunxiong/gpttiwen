NianNianYouYuView = NianNianYouYuView or DeclareView("NianNianYouYuView", "serveractivity/tiger_year/niannianyouyu", Mod.TigerYear.NianNianYouYu)

function NianNianYouYuView:OnClickClose()
    ViewMgr:CloseView(NianNianYouYuView)
end

NianNianYouYuPanel = NianNianYouYuPanel or DeclareMono("NianNianYouYuPanel", UIWFlushPanel)
function NianNianYouYuPanel:NianNianYouYuPanel()
    self.data_cares = {
        {data = NianNianYouYuData.Instance.act_info, func = self.FlushView}
    }
end

function NianNianYouYuPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ActTimer:CreateActTime(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, TimeType.Type_Special_4)
end

function NianNianYouYuPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function NianNianYouYuPanel:FlushView()
    local is_accept = NianNianYouYuData.Instance:IsAcceptTask()
    self.TaskEmpty:SetActive(not is_accept)
    self.TaskList.gameObject:SetActive(is_accept)
    if is_accept then
        local task_list = NianNianYouYuData.Instance:GetTaskList()
        self.TaskList:SetData(task_list)
    else
        local accept_npc = NianNianYouYuData.Instance:GetAcceptNpc()
        UH.SetText(self.TaskName, Format(Language.NianNianYouYu.AcceptTaskStr, accept_npc.city_name or ""))
    end

    local reward_list = NianNianYouYuData.Instance:GetRewardList()
    self.RewardList:SetData(reward_list)

    UH.SetText(self.ItemNum, NianNianYouYuData.Instance:CurFishNum())
end

function NianNianYouYuPanel:OnClickGo()
    local accept_npc = NianNianYouYuData.Instance:GetAcceptNpc()
    if accept_npc == nil then
        return
    end
    SceneLogic.Instance:AutoToNpc(accept_npc.task_npc, nil, false, true)
    ViewMgr:CloseView(NianNianYouYuView)

    TigerYearTotalCtrl.Instance:AutoCloseTotal(Mod.TigerYear.NianNianYouYu)
end

NianNianYouYuTaskItem = NianNianYouYuTaskItem or DeclareMono("NianNianYouYuTaskItem", UIWidgetBaseItem)
function NianNianYouYuTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local item_list = DataHelper.FormatItemList(data.reward_item)
    table.insert(item_list, 1, Item.Create({item_id = NianNianYouYuData.Instance:FishItemId(), num = self.data.reward_fish_num}))
    self.ItemList:SetData(item_list)

    local is_finish = NianNianYouYuData.Instance:IsTaskFinished(self.data.task_type)
    local is_fetched = NianNianYouYuData.Instance:IsTaskRewardFetched(self.data.task_type)
    self.Fetched:SetActive(is_fetched)
    self.BtnFetch:SetActive(is_finish and not is_fetched)
    self.BtnGo:SetActive(not is_finish and not is_fetched)

    local task_prog = NianNianYouYuData.Instance:GetTaskProg(self.data.task_type)
    local task_name = data.task_desc .. Format("(%s/%s)", math.min(task_prog, self.data.parameter_1), self.data.parameter_1)
    UH.SetText(self.NameText, task_name)
end

function NianNianYouYuTaskItem:OnClickFetch()
    if NianNianYouYuData.Instance:IsTaskFinished(self.data.task_type) then
        NianNianYouYuCtrl.Instance:SendFetchTaskReward(self.data.seq)
    end
end

function NianNianYouYuTaskItem:OnClickGo()
    ViewMgr:OpenViewByKey(self.data.jump_mod_key)
    ViewMgr:CloseView(NianNianYouYuView)

    TigerYearTotalCtrl.Instance:AutoCloseTotal(Mod.TigerYear.NianNianYouYu)
end

NianNianYouYuRewardItem = NianNianYouYuRewardItem or DeclareMono("NianNianYouYuRewardItem", UIWidgetBaseItem)
function NianNianYouYuRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local cur_value = NianNianYouYuData.Instance:CurFishNum()
    local color = cur_value >= data.fish_num and COLORSTR.Green9 or COLORSTR.Red8
    local show_num = math.min(cur_value, data.fish_num)
    UH.SetText(self.NameText, Format(Language.NianNianYouYu.RewardFormat, data.fish_num, color, show_num, data.fish_num))

    self.ItemList:SetData(DataHelper.FormatItemList(data.reward_item))

    local is_fetched = NianNianYouYuData.Instance:IsRewardFetched(self.data.seq)
    local is_can_fetch = NianNianYouYuData.Instance:IsRewardCanFetch(self.data.seq)
    self.Fetched:SetActive(is_fetched)
    self.BtnGet:SetActive(not is_fetched)
    if not is_fetched then
        self.BtnGetInter.Interactable = is_can_fetch
        self.BtnGetEffect:SetActive(is_can_fetch)
    end
end

function NianNianYouYuRewardItem:OnClickFetch()
    if NianNianYouYuData.Instance:IsRewardFetched(self.data.seq) then
        return
    end
    if NianNianYouYuData.Instance:IsRewardCanFetch(self.data.seq) then
        NianNianYouYuCtrl.Instance:SendFetchRewardReward(self.data.seq)
    else
        PublicPopupCtrl.Instance:Center(Language.NianNianYouYu.ItemNotEnought)
    end
end
