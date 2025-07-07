ActEquipSmeltView = ActEquipSmeltView or DeclareView("ActEquipSmeltView", "serveractivity/act_equip_smelt/act_equip_smelt", Mod.RandActivity.ActEquipSmelt)

function ActEquipSmeltView:OnClickClose()
    ViewMgr:CloseView(ActEquipSmeltView)
end

ActEquipSmeltPanel = ActEquipSmeltPanel or DeclareMono("ActEquipSmeltPanel", UIWFlushPanel)
function ActEquipSmeltPanel:ActEquipSmeltPanel()
    self.data_cares = {
        {data = ActEquipSmeltData.Instance.act_info, func = self.FlushView, init_call = false}
    }
end

function ActEquipSmeltPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ActTimer:CreateActTime(ACTIVITY_TYPE.RAND_EQUIP_SMELT, TimeType.Type_Special_4)

    ActivityData.Instance:SetHasSign(ACTIVITY_TYPE.RAND_EQUIP_SMELT)
    ActEquipSmeltData.Instance.act_info:Notify()
end

function ActEquipSmeltPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ActEquipSmeltPanel:FlushView()
    local show_list = ActEquipSmeltData.Instance:GetTaskList()
    local function func_sort(data)
        if ActEquipSmeltData.Instance:GetTaskFetchedTimes(data.seq) >= data.reward_times then
            return 2000
        end
        if ActEquipSmeltData.Instance:GetTaskProg(data.seq) >= data.parameter then
            return 0
        end
        return 1000
    end
    table.sort(show_list, DataHelper.WeightSort("seq", func_sort))
    self.TaskList:SetData(show_list)

    local show_info, is_over = ActEquipSmeltData.Instance:GetShowInfo()
    local finish_num = ActEquipSmeltData.Instance:GetFinishTaskNum()
    local can_fetch = not is_over and finish_num >= show_info.task_num
    self.BoxEffect:SetActive(can_fetch)
    self.RedPoint:SetNum(can_fetch and 1 or 0)
    UH.SetText(self.StageText, Format(Language.ActEquipSmelt.StageProg, finish_num, show_info.task_num))
    UH.SetInter(self.StageInter, not is_over)
    self.StageGet:SetActive(is_over)
end

function ActEquipSmeltPanel:OnClickGo()
    ViewMgr:OpenViewByKey(Mod.Equip.Smelt)
end

function ActEquipSmeltPanel:OnClickReward()
    local show_info, is_over = ActEquipSmeltData.Instance:GetShowInfo()
    if is_over then
        PublicPopupCtrl.Instance:Center(Language.ActEquipSmelt.AllFinish1)
        return
    end
    if ActEquipSmeltData.Instance:GetFinishTaskNum() >= show_info.task_num then
        ActEquipSmeltCtrl.Instance:SendFetchStage()
    else
        ViewMgr:OpenView(ActEquipSmeltShowView)
    end
end

ActEquipSmeltTaskItem = ActEquipSmeltTaskItem or DeclareMono("ActEquipSmeltTaskItem", UIWidgetBaseItem)
function ActEquipSmeltTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.DescText, data.task_desc)
    local prog_val = ActEquipSmeltData.Instance:GetTaskProg(data.seq)
    UH.SetText(self.ProgText, Format(Language.ActEquipSmelt.TaskProg, prog_val, data.parameter))

    self.ItemCell:SetData(Item.Create(data.reward_item))
    local fetch_times = ActEquipSmeltData.Instance:GetTaskFetchedTimes(data.seq)
    UH.SetText(self.Times, Format(Language.ActEquipSmelt.RewardProg, fetch_times, data.reward_times))

    local is_over = fetch_times >= data.reward_times
    self.Fetched:SetActive(is_over)
    self.BtnFetchInter.gameObject:SetActive(not is_over)
    if not is_over then
        UH.SetInter(self.BtnFetchInter, prog_val >= data.parameter)
        self.BtnFetchEffect:SetActive(prog_val >= data.parameter)
    end
end

function ActEquipSmeltTaskItem:OnClickFetch()
    ActEquipSmeltCtrl.Instance:SendFetchTaskReward(self.data.seq)
end
