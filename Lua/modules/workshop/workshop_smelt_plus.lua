WorkshopSmeltPlus = WorkshopSmeltPlus or DeclareView("WorkshopSmeltPlus", "workshop/workshop_smelt_plus")
function WorkshopSmeltPlus:WorkshopSmeltPlus()

end 

function WorkshopSmeltPlus:OnClickClose()
    ViewMgr:CloseView(WorkshopSmeltPlus)
end

WorkshopSmeltPlusPanel = WorkshopSmeltPlusPanel or DeclareMono("WorkshopSmeltPlusPanel", UIWFlushPanel)
function WorkshopSmeltPlusPanel:WorkshopSmeltPlusPanel()
    self.workshop_data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        -- {data = BagData.Instance.item_grid, func = self.FlushEquip, init_call = false},
        {data = self.workshop_data.smelt_select, func = self.FlushEquip, init_call = false},
    }
    self.send_data = {}

    self:FlushEquip()
end 


function WorkshopSmeltPlusPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

function WorkshopSmeltPlusPanel:FlushEquip()
    if self.workshop_data:GetSmeltSelect() == nil then 
        ViewMgr:CloseView(WorkshopSmeltPlus)
        return 
    end 

    local attr = self.workshop_data:GetSmeltSelectAttr()
    table.sort(attr, DataSorter.KeyLowerSorter("index"))
    self.AttrList1:SetData(attr)
    self.EquipCell:SetData(self.workshop_data:GetSmeltSelect())

    if self.send_data.data == nil then 
        self.EmptyRight:SetActive(true)
        self.SelectCell:SetData(Item.New())
    else 
        self.EmptyRight:SetActive(false)

        local cur_data = BagData.Instance:GetItemByIndex(ItemColumnType.Item,self.send_data.data.index)
        self.send_data.data = cur_data

        local attr = self.workshop_data:GetSmeltSelectAttr(self.send_data.data)
        table.sort(attr, DataSorter.KeyLowerSorter("index"))

        self.AttrList2:SetData(attr)
        self.SelectCell:SetData(Item.Create(self.send_data.data))
        self.BtnReplace:SetActive(true)
    end 
end

function WorkshopSmeltPlusPanel:OnClickSelectItem(data)
    self.send_data.data = data
end

function WorkshopSmeltPlusPanel:OnClickReplace()
    self.PartSelect:SetActive(true)

    local list = self.workshop_data:GetSmeltPlusEquipList()
    self.SelectList:SetData(list)

    self.SelectListEmpty:SetActive(#list == 0)
    
    self.select_toggle:SetAllTogglesOff()
end

function WorkshopSmeltPlusPanel:OnClickInherit()
    if self.send_data.data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltEmptyInHeritTips)
        return
    end 

    local select_data = self.workshop_data:GetSmeltSelect()
    local param_t = {}
    param_t.change_voc_type = select_data:IsEquip() and 0 or 1
    param_t.role_type = AnotherData.Instance:CheckEquipmentUniqueId(select_data, select_data:IsEquip()) and 0 or 1
    param_t.replacer_posi = select_data.in_bag and 1 or 0
    param_t.replacer_grid = select_data.index
    param_t.replacer_voc_index = 0
    param_t.bereplacer_posi = self.send_data.data.in_bag and 1 or 0
    param_t.bereplacer_grid = self.send_data.data.index
    param_t.bereplacer_voc_index = 0

    WorkshopCtrl.Instance:ChangeEquipVocReq(param_t)


    self.send_data.data = nil 
    self.AttrList2:SetData({})
    self.BtnReplace:SetActive(false)
    -- ViewMgr:CloseView(WorkshopSmeltPlus)
end

function WorkshopSmeltPlusPanel:OnClickSelect()
    self.PartSelect:SetActive(false)
    self:FlushEquip()
end

WorkshopSmeltPlusItem = WorkshopSmeltPlusItem or DeclareMono("WorkshopSmeltPlusItem", UIWidgetBaseItem)
function WorkshopSmeltPlusItem:WorkshopSmeltPlusItem( )

end

function WorkshopSmeltPlusItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:InitData()

    local pass = self.data.info.lock == 1 or data.info.attr_value ~= 0
    -- 针对老装备
    self.LockObj:SetActive( self.data.is_role_limited )
    if self.data.is_role_limited then 
        self.LblLimited.text = self.data.role_limit_text
        return 
    end 
    -- 检查是否装备等级限制
    self.LockObj:SetActive( self.data.is_limited )
    if self.data.is_limited then 
        self.LblLimited.text = self.data.limit_text
        return 
    end 
    self.Total:SetActive(data.info.attr_value > 0)
    self.Empty:SetActive(data.info.attr_value == 0)

    if data.info.attr_value > 0 then 
        local str_ = ""
        if data.info.color == nil then  
            str_ = string.format(Language.Workshop.IncAttr, 
            Language.Common.AttrList[data.info.attr_type], 
            DataHelper.IsPercent(data.attr_type) and Percent(data.info.attr_value) or data.info.attr_value)
        else 
            str_ = string.format(Language.Workshop.IncAttr2, 
            QualityColorStrSmelt[data.info.color],Language.Common.AttrList[data.info.attr_type],
            DataHelper.IsPercent(data.info.attr_type) and Percent(data.info.attr_value) or data.info.attr_value)
        end 
        self.LblAttr.text = str_
        -- str_  = Language.Workshop.SmeltEmptyAttr
    end 

--  string.format(Language.Common.Xstr,QualityColorStrSmelt[data.info.color],
--         string.format(Language.Common.IncAttr, Language.Common.AttrList[data.info.attr_type],
--         DataHelper.IsPercent(data.info.attr_type) and Percent(data.info.attr_value) or data.info.attr_value))

    self.PointObj:SetActive(DataHelper.IsPoint(data.info.attr_type))
end


function WorkshopSmeltPlusItem:InitData()
    self.Total:SetActive(false)
    self.LockObj:SetActive(false)
end

WorkshopSmeltPlusEquipItem = WorkshopSmeltPlusEquipItem or DeclareMono("WorkshopSmeltPlusEquipItem", UIWidgetBaseItem)
function WorkshopSmeltPlusEquipItem:WorkshopSmeltPlusEquipItem( )

end

function WorkshopSmeltPlusEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.EquipCell:SetData(data)
    self.EquipInfo.text = data:QuaName().." "..data:LimitLv()..Language.Common.Level
    self.Limited.text = data:LimitProfDesc()

    local base_list = {}
    for k,v in pairs(data.param.attr_list) do 
        if v.attr_value >0 then table.insert( base_list, v ) end 
    end 
    self.BaseAttr:SetData(base_list)
    local _,inc_list = data:IncAttr()
    
    local smelt_list = {}
    for k,v in pairs(inc_list) do 
        if v.attr_value >0 then table.insert( smelt_list, v ) end 
    end 
    self.SmeltAttr:SetData(smelt_list)
end 


WorkshopSmeltPlusStr = WorkshopSmeltPlusStr or DeclareMono("WorkshopSmeltPlusStr", UIWidgetBaseItem)
function WorkshopSmeltPlusStr:WorkshopSmeltPlusStr( )

end

function WorkshopSmeltPlusStr:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local str_ = ""

    if data.attr_value > 0 then 
        if data.color == nil then  
            str_ = string.format(Language.Workshop.IncAttr, 
            Language.Common.AttrList[data.attr_type], 
            DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
        else 
            str_ = string.format(Language.Workshop.IncAttr2, 
            QualityColorStr[data.color],Language.Common.AttrList[data.attr_type],
            DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
        end 
    else 
        str_  = Language.Workshop.SmeltEmptyAttr
    end 

    self.str.text = str_
end 