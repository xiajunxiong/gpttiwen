
BountyTaskArrayView = BountyTaskArrayView or DeclareView("BountyTaskArrayView", TaskConfig.ResPathBT .. "bounty_task_array", Mod.BountyTask.Array)
-- VIEW_DECLARE_LEVEL(BountyTaskArrayView, ViewLevel.L0)
VIEW_DECLARE_MASK(BountyTaskArrayView, ViewMask.BgBlock)
function BountyTaskArrayView:BountyTaskArrayView()
end

BountyTaskArrayViewF = BountyTaskArrayViewF or DeclareMono("BountyTaskArrayViewF", UIWFlushPanel)

function BountyTaskArrayViewF:BountyTaskArrayViewF()
    -- self.data = TreasureTaskData.Instance
    self.language = Language.Task.BountyTaskArray

    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

    self.cur_index = 0
    self.is_lighted = false
    self.auto_lighting = false
    self.WAIT_TIME = 7.1
    self.WAIT_TIME2 = 3.1
end

function BountyTaskArrayViewF:Awake()
    UIWFlushPanel.Awake(self)

    -- self.RemainTimeObj:SetActive(true)
    -- self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.AutoLighting, self), self.WAIT_TIME)
    TimeHelper:CancelTimer(self.timer_ct)
    local end_time = TimeHelper.GetServerTime() + self.WAIT_TIME
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local ft = TimeHelper.FormatDHMS(end_time - TimeHelper.GetServerTime())
        self.RemainTime.text = string.format(self.language.RemainTime1, ft.s)
        if 2 == ft.s then self.CompleteObj:SetActive(false) end 
    end, BindTool.Bind(self.AutoLighting, self), end_time)
end

function BountyTaskArrayViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_dt)
    TimeHelper:CancelTimer(self.timer_rt)
    TimeHelper:CancelTimer(self.timer_ct)
end

function BountyTaskArrayViewF:OnClickClose()
    ViewMgr:CloseView(BountyTaskArrayView)
    if self.is_lighted then
        TaskCtrl.Instance:SendTaskGameReq(TaskConfig.TaskGameList.RepairArray)
        BountyTaskData.Instance:PlayEffectArray(true)
    end
end

function BountyTaskArrayViewF:AutoLighting()
    if self.is_lighted then return end
    self.auto_lighting = true
    TimeHelper:CancelTimer(self.timer_rt)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        self:Lighting(true)
    end, 0.2)
end

function BountyTaskArrayViewF:OnPressDown()
    self:OnDrag()
end

function BountyTaskArrayViewF:OnPressUp()
    self:PutOut()
end

function BountyTaskArrayViewF:OnDrag()
    if self.auto_lighting then return end
    self:Lighting()
end

function BountyTaskArrayViewF:Lighting(is_auto)
    local to_index = self.cur_index + 1
    local to_item = self.ItemPosList[to_index]
    if to_item then
        local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
        if GameMath.Distance(to_item.localPosition, localPoint, true) < to_item.sizeDelta.x or is_auto then
            self.ItemTogList[to_index].isOn = true
            self.cur_index = to_index
            
            self:Lighted()
        end
    end
end

function BountyTaskArrayViewF:PutOut()
    if self.auto_lighting then return end
    local to_index = self.cur_index + 1
    local to_item = self.ItemPosList[to_index]
    if to_item then
        self.cur_index = 0
        for i = 1, self.ItemTogList:Length() do
            self.ItemTogList[i].isOn = false
        end
    end
end

function BountyTaskArrayViewF:Lighted()
    local to_index = self.cur_index + 1
    local to_item = self.ItemPosList[to_index]
    if to_item then return end

    TimeHelper:CancelTimer(self.timer_rt)
    for i = 1, self.RoundComs:Length() do
        self.RoundComs[i].enabled = true
    end
    self.is_lighted = true

    
    self.CompleteObj:SetActive(true)
    -- self.RemainTimeObj:SetActive(true)

    TimeHelper:CancelTimer(self.timer_ct)
    local end_time = TimeHelper.GetServerTime() + self.WAIT_TIME2
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local ft = TimeHelper.FormatDHMS(end_time - TimeHelper.GetServerTime())
        self.RemainTime.text = string.format(self.language.RemainTime2, ft.s)
        if 2 == ft.s then self.CompleteObj:SetActive(false) end 
    end, BindTool.Bind(self.OnClickClose, self), end_time)
end