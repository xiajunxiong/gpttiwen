--================================FlyUpMenuPanel================================
FlyUpMenuPanel = FlyUpMenuPanel or DeclareMono("FlyUpMenuPanel", UIWFlushPanel)

function FlyUpMenuPanel:FlyUpMenuPanel()
    self.data = FlyUpData.Instance
    self.data_cares = {
        {data = self.data.add_data, func = self.FlushMenuView, init_call = false},
        {data = self.data.info_data, func = self.OnFlush, init_call = false,keys = {"stage"}},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = TaskData.Instance.task_id_list, func = self.OnFlush, init_call = false,keys = {"finish_list"}},
    }
    GuideManager.Instance:RegisterGetGuideUi("MainRoleFlyUpDescBtn", function()
		return self.BaseDesc
	end)
end

function FlyUpMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:OnFlush()
    UnRegisterRemind(Mod.FlyUp.Main,self.handle)
    self.handle = RegisterRemind(Mod.FlyUp.Main,function(num)
        self.RedPoint:SetNum(num, true)
    end,true)
end

function FlyUpMenuPanel:OnFlush()
    self:FlushItemView()
    self:FlushMenuView()
end

function FlyUpMenuPanel:FlushItemView()
    local is_open = self.data:IsShowed()
    local item_active = GetActive(self.BaseItem)
    if item_active ~= is_open then
        if is_open == true then
            if self:IsFlyUpStageHigh() then
                self:OnFlyUpPopupView()
            end
            self.BaseGame:SetActive(false)
        end
        TaskData.Instance:CheckMenuGuideFlush()
    end
    if item_active == false and self:GetIsOpened() then
        self.ShowTimer = Invoke(function()
            VideoCtrl.Instance:StartVideoById(8,BindTool.Bind(self.OnFlyUpPopupView, self))
        end,0.5)
    end
    self.BaseItem:SetActive(is_open)
end

function FlyUpMenuPanel:FlushMenuView()
    if not self.data:IsShowed() then return end
    local stage = self.data:GetNextStage()
    local task_num,target_num = self.data:GetTaskNum(stage)
    local ProgressTips = Language.OpenAct.SevenDay.ProgressTips
    self.Progress:SetText(Format(ProgressTips,task_num,target_num))
    self.Progress:SetProgress(task_num / target_num)
    UH.SetText(self.Name,Language.FlyUp.Name[stage])
end
function FlyUpMenuPanel:IsFlyUpStageHigh()
    return self.data:GetNextStage() == FlyUpStageType.High and self.data:GetIsOpenNum() == 0
end

function FlyUpMenuPanel:GetIsOpened()
    return self.data:IsCompletedTask() and PrefsInt([[RoleFlyUpPopupAnim]] .. RoleId()) == 0 and not self.data:IsFinish()
end

function FlyUpMenuPanel:OnFlyUpPopupView()
    if self.data:GetIsOpenNum() == 0 then
        FlyUpCtrl.Instance:SendFlyUpReq(3)
    end
    if PrefsInt([[RoleFlyUpPopupAnim]] .. RoleId()) == 0 then
        PrefsInt([[RoleFlyUpPopupAnim]] .. RoleId(),1)
        ViewMgr:OpenView(FlyUpPopupView,{guide_func = function()
            GuideManager.Instance:Start(111)
        end})
    else
        ViewMgr:OpenView(FlyUpPopupView)
    end
end

function FlyUpMenuPanel:OnClickItem()
    ViewMgr:OpenView(FlyUpPopupView)
end