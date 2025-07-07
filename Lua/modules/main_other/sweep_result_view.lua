SweepResultView = SweepResultView or DeclareView("SweepResultView", "main_other/sweep_result")
VIEW_DECLARE_MASK(SweepResultView,ViewMask.BgBlockClose)
function SweepResultView:OnClickClose()
    ViewMgr:CloseView(SweepResultView)
    BagData.Instance:ClearGetWaySweepMark()
end

SweepResultPanel = SweepResultPanel or DeclareMono("SweepResultPanel", UIWFlushPanel)
function SweepResultPanel:SweepResultPanel()
    self.operate_data = BagData.Instance.get_way_sweep
	self.data_cares = {
        {data = BagData.Instance.sweep_mark, func = self.FlushResult },
        {data = BagData.Instance.item_grid, func = self.OnFlushProgress},
    }

    self.target_item =Item.Create({item_id = self.operate_data.item_id}) 
    self.Cell:SetData(self.target_item)
end 

function SweepResultPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self) 
end

function SweepResultPanel:FlushResult()
    UH.SetText(self.exp,MaterialData.Instance:GetExpNum())
    self.result_list:SetData(BagData.Instance.sweep_mark.item_list)

    local result_num = #BagData.Instance.sweep_mark.item_list
    if result_num <= 5 then
        self.center_ele.preferredHeight = 60
        self.list_rect.rect.height = 70
    else 
        self.center_ele.preferredHeight = 100
        self.list_rect.rect.height = 110
    end 
end

function SweepResultPanel:OnFlushProgress()
    local cost_num = self.operate_data.need or 1
    local cur_num = Item.GetNum(self.operate_data.item_id)
    local color = cost_num <= cur_num and COLORSTR.Green3 or COLORSTR.Red7
    local str = string.format(Language.Common.StuffNumColorFirst, color, cur_num, cost_num)
    UH.SetText(self.item_num,str)
    UH.SetText(self.item_name,self.target_item:QuaName()) 
    local rate = cur_num/cost_num > 1 and 1 or cur_num/cost_num
    self.prog:SetProgress(rate)
    self.is_complete:SetActive(rate == 1)
end

function SweepResultPanel:OnClickAgain()
    if self.operate_data.func ~= nil then 
        self.operate_data:func()
    end 
end

function SweepResultPanel:OnClickBack()
    ViewMgr:CloseView(SweepResultView)
    BagData.Instance:ClearGetWaySweepMark()
    -- ViewMgr:OpenView(GetWayView, {item = self.target_item})
end


SweepResultCell = DeclareMono("SweepResultCell", UIWidgetBaseItem)
function SweepResultCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end