IllusionAllAttrView = IllusionAllAttrView or DeclareView("IllusionAllAttrView", IllusionConfig.ResPath .. "illusion_all_attr")
VIEW_DECLARE_MASK(IllusionAllAttrView, ViewMask.Block)
function IllusionAllAttrView:IllusionAllAttrView()
    self.Board:SetData(self:GetType(), Language.IllusionUp.Title2, Vector2.New(752, 618))
end

function IllusionAllAttrView:LoadCallback(param_t)
    
end

function IllusionAllAttrView:CloseCallback()

end

function IllusionAllAttrView:OnCloseClick()
    ViewMgr:CloseView(IllusionAllAttrView)
end

----------------------------IllusionAllAttrPanel----------------------------
IllusionAllAttrPanel = IllusionAllAttrPanel or DeclareMono("IllusionAllAttrPanel", UIWFlushPanel)
function IllusionAllAttrPanel:IllusionAllAttrPanel()
    self.Data = IllusionData.Instance
    self.language = Language.IllusionUp
    self.data = self.Data.illusion_lv_info
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
end

function IllusionAllAttrPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function IllusionAllAttrPanel:FlushAll()
    local base_data = self:DealBaseAttr()
    local sp_data = self:DealSpAttr()
    self.BaseList:SetData(base_data)
    self.ObjEmptyBase:SetActive(TableIsEmpty(base_data))
    self.SpList:SetData(sp_data)
    self.ObjEmptySp:SetActive(TableIsEmpty(sp_data))
end

function IllusionAllAttrPanel:DealBaseAttr()
    local list = self.Data:IllusionLvAllAttr()
    return self:DealAttrList(list)
end

function IllusionAllAttrPanel:DealSpAttr()
    local list = {}
    local cfg = self.Data:GetSpLvUpAttr()
    for k, v in pairs(cfg) do
		table.insert(list, v)
	end
	table.sort(list, function(a, b)
		return a.attr_type < b.attr_type
    end)
    return self:DealAttrList(list)
end

function IllusionAllAttrPanel:DealAttrList(list)
    local list2 = {}
    if not TableIsEmpty(list) then
        for k, v in pairs(list) do
            local t = {}
            local k_1 = k - 1
            local t1, t2 = math.floor(k_1 / 3) + 1, (k_1 % 3) + 1
            if not list2[t1] then
                list2[t1] = {}
                list2[t1].my_index = t1
                list2[t1].attr = {}
            end
		    table.insert(list2[t1].attr, v)
        end
    end
    return list2
end

function IllusionAllAttrPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------IllusionAllAttrRowItem--------------
IllusionAllAttrRowItem = IllusionAllAttrRowItem or DeclareMono("IllusionAllAttrRowItem", UIWidgetBaseItem)
function IllusionAllAttrRowItem:IllusionAllAttrRowItem()
    self.Data = IllusionData.Instance
end

function IllusionAllAttrRowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjBg:SetActive(data.my_index % 2 == 0)
    self.AttrList:SetData(data.attr)
end

--------------IllusionAllAttrAttrItem--------------
IllusionAllAttrAttrItem = IllusionAllAttrAttrItem or DeclareMono("IllusionAllAttrAttrItem", UIWidgetBaseItem)
function IllusionAllAttrAttrItem:IllusionAllAttrAttrItem()
    self.language = Language.IllusionUp
end

function IllusionAllAttrAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDesc, Format(self.language.AttrDesc, data.attr_name, data.attr_percent_value))
end