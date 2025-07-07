-- 幽冥分解
YouMingResloveView = YouMingResloveView or DeclareView("YouMingResloveView", "youming/youming_reslove_view",Mod.YouMing.Reslove)
function YouMingResloveView:YouMingResloveView()
    self.Board:SetData(self:GetType(),Language.YouMing.Title2,Vector2.New(1018, 568))
end


YouMingReslovePanel = YouMingReslovePanel or DeclareMono("YouMingReslovePanel", UIWFlushPanel)
function YouMingReslovePanel:YouMingReslovePanel()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data.youming_list, func = self.FlushPanel, init_call = false}
    }
    self.type_list = {
        {qua = ItemColor.None},
        {qua = ItemColor.Blue},
        {qua = ItemColor.Purple},
        {qua = ItemColor.Orange},
        {qua = ItemColor.Red},
    }
    self.type_data = self.type_list[1]
end

function YouMingReslovePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.TypeList:SetData(self.type_list)
    self:FlushPanel()
end

function YouMingReslovePanel:FlushPanel()
    local youming_list = self.data:GetBagList()
    self.youming_list = youming_list
    self.List:SetData(youming_list)
    self.Empty:SetActive(TableIsEmpty(youming_list))
    self.Show:SetActive(not TableIsEmpty(youming_list))
    self:FlushInfo()
end

function YouMingReslovePanel:FlushInfo()
    local select_list = self.List:GetSelectGroupData()
    local count = table.nums(select_list)
    UH.SetText(self.SelectCount, string.format(Language.YouMing.Text3, ColorStr(count, COLORSTR.Yellow1)))

    local color = QualityColorStrShallow[self.type_data.qua]
    local text = self.type_data.qua == 0 and Language.YouMing.QuaTypeDefault or ColorStr(Language.YouMing.QuaType[self.type_data.qua], color)
    UH.SetText(self.SelectText, text)

end

function YouMingReslovePanel:OnAllResloveBtnClick()
    local youming_list = self.List:GetSelectGroupData()
    local list = {}
    for i,v in pairs(youming_list) do
        table.insert(list, v.index)
    end
    YouMingCtrl.Instance:SendResloveList(list)
end

function YouMingReslovePanel:OnTypeSelectClick(data)
    self.type_data = data
    self.TypeSelectToggle.isOn = false
    self.List:CancelSelect()
	for i,v in pairs(self.youming_list) do
		if v:Qua() == data.qua then
			self.List:SetectItem(i,true)
		end
    end
	self:FlushInfo()
end

function YouMingReslovePanel:OnItemSelectClick(value, data)
    self:FlushInfo()
end

function YouMingReslovePanel:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMing.Text2})
end

YouMingResloveItem = YouMingResloveItem or DeclareMono("YouMingResloveItem", UIWidgetBaseItem)
function YouMingResloveItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name, data:Name())
    self.ItemCell:SetData(data)
    UH.SetText(self.Score, string.format(Language.YouMing.Score, data:Score()))
end

function YouMingResloveItem:OnResloveClick()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.DECOMPOSE, self.data.index)
end

function YouMingResloveItem:OnLookClick()
   ViewMgr:OpenView(YouMingInfoView, {youming = self.data})
end

function YouMingResloveItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
end


YouMingResloveTypeItem = YouMingResloveTypeItem or DeclareMono("YouMingResloveTypeItem", UIWidgetBaseItem)
function YouMingResloveTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local color = QualityColorStr[data.qua]
    local text = ColorStr(Language.YouMing.QuaType[data.qua], color)
    UH.SetText(self.Text, text)
end