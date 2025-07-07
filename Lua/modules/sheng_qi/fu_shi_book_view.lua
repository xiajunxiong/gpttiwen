FuShiBookView = FuShiBookView or DeclareView("FuShiBookView", "sheng_qi/fu_shi_book_view")
function FuShiBookView:FuShiBookView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title1,Vector2.New(868, 603))
end

FuShiBookPanel = FuShiBookPanel or DeclareMono("FuShiBookPanel", UIWFlushPanel)
function FuShiBookPanel:FuShiBookPanel()
    self.data = FuShiData.Instance
    self.data_cares = {
        {data = self.data.sel_book_data, func = self.FlushPanel, init_call = false},
    }
end

function FuShiBookPanel:Awake()
    UIWFlushPanel.Awake(self)
    local list_data = self.data:GetBookList()
    self.List:SetData(list_data)
    self:FlushPanel()
end

function FuShiBookPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetBookSelData({})
end

function FuShiBookPanel:FlushPanel()
    local info = self.data:GetBookSelData()
    if TableIsEmpty(info) then
        return
    end
    local item = Item.Create({item_id = info.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName())
    -- local jt_list = Split(info.jingtong, "|")
    -- local attri_name = ""
    -- for i,v in pairs(jt_list) do
    --     if Language.ShengQi.AttriName[tonumber(v)] then
    --         attri_name = attri_name .. Language.ShengQi.AttriName[tonumber(v)]
    --         if i ~= #jt_list then
    --             attri_name = attri_name .. "、"
    --         end
    --     end
    -- end
    -- UH.SetText(self.Desc, string.format(Language.ShengQi.Text3,attri_name))
    UH.SetText(self.Desc, "")

    local preview_attri_list = item:AttriPreviewList() or {}
    self.AttriList:SetData(preview_attri_list)

    local role_attri = item:RoleAttri()
    local attri_info = CommonStruct.AttrStruct(role_attri.attri_type, role_attri.attri_value)
    UH.SetText(self.RoleAttri, string.format(Language.ShengQi.Text12, attri_info.attr_name, attri_info.attr_percent_value))
end



FuShiBookListItem = FuShiBookListItem or DeclareMono("FuShiBookListItem", UIWidgetBaseItem)
function FuShiBookListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local type_name = Language.ShengQi.FuShiTypeName[data.type]
    UH.SetText(self.Title, type_name)
    self.List:SetData(data.list)
    -- 计算高度
    local list_h = 0
    if not TableIsEmpty(data.list) then
        list_h = math.ceil(#data.list / 4) * (76 + 18) - 18
    end
    local h = 50 + list_h
    self.Trans:SetSizeWithCurrentAnchors(1,h)
end

FuShiBookCellItem = FuShiBookCellItem or DeclareMono("FuShiBookCellItem", UIWidgetBaseItem)
function FuShiBookCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    self.item = item
    self.Cell:SetData(item)
    if TableIsEmpty(FuShiData.Instance.sel_book_data) and data.fushi_type == 1 then
        FuShiData.Instance:SetBookSelData(data)
        self:SetSelected(true)
    end
end

function FuShiBookCellItem:OnSelClick()
    FuShiData.Instance:SetBookSelData(self.data)
end

FuShiBookAttriItem = FuShiBookAttriItem or DeclareMono("FuShiBookAttriItem", UIWidgetBaseItem)
function FuShiBookAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Language.ShengQi.AttriName[data.attri_type] .. ":")
    local min_v
    local max_v
    if ShengQiData.Instance:AttriTypeIsRate(data.attri_type) then
        min_v = data.min_value / 10 .. "%"
        max_v = data.max_value / 10 .. "%"
    else
        min_v = data.min_value
        max_v = data.max_value
    end
    local value_str = string.format("+(%s~%s)",min_v,max_v)
    UH.SetText(self.Value, value_str)
end