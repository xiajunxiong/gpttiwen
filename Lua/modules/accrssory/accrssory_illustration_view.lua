AccrssoryIllustrationView = AccrssoryIllustrationView or DeclareMono("AccrssoryIllustrationView", UIWFlushPanel)
function AccrssoryIllustrationView:AccrssoryIllustrationView()
    self.Data = AccrssoryData.Instance
    self.data = self.Data.illustration_data
    self.language = Language.Accrssory
    self.illustration_config = self.Data:GetIllustrationConfig()
    self.data:Set({suit_id = 1, part = 1, is_show_fly = 0})
    self.data_cares = {
        {data = self.data, func = self.FlushSuitItem, keys = {"suit_id"}, init_call = false},
        {data = self.data, func = self.FlushRight, keys = {"part"}, init_call = false},
    }
    -- ObjBlock
end

function AccrssoryIllustrationView:Awake()
    UIWFlushPanel.Awake(self)
    self.ObjTitle:SetActive(not self.Data:IsShowFly())
    local is_can_click_fly = self.Data:IsCanClickFly()
    self.IHandlerBtn2.Interactable = is_can_click_fly
    self.ObjGrey:SetActive(not is_can_click_fly)
    self:FlushTopItem(false)
end

function AccrssoryIllustrationView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- 展示的套装配饰（是否飞升）
function AccrssoryIllustrationView:FlushTopItem(value)
    self.SuitNameList:SetData({})
    self.SuitNameList:SetData(self.Data:GetSuitIdNameData(value))
    self.SuitNameList:SetectItem(1, true)
    self:FlushSuitItem()
end

function AccrssoryIllustrationView:OnGreyClick()
    PublicPopupCtrl.Instance:Center(self.language.NeedFlyTip2)
end

function AccrssoryIllustrationView:FlushSuitItem()
    self.SuitList:SetData({})
    self.SuitList:SetData(self.illustration_config[self.data.suit_id])
    self.SuitList:SetectItem(self.data.part, true)
    self:FlushRight()
end

function AccrssoryIllustrationView:FlushRight()
    local now_select_data = self.illustration_config[self.data.suit_id][self.data.part]
    local item = Item.Init(now_select_data.peishi_id, 1, 1)
    self.Cell:SetData(item)
    UH.SetText(self.TxtName, item:QuaName(true))
    UH.SetText(self.TxtRecommondProf, Format(self.language.RecommondProf, Language.Common.Prof[now_select_data.recommend_profession]))
    self.BaseAttrList:SetData(self:GetAttrData(self.Data:GetAccrssoryBaseAttr(now_select_data.peishi_id)))
    self.ExtraAttrList:SetData(self:GetAttrData(self.Data:GetAccrssoryRandMaxAttr(now_select_data.peishi_id)))
    for i = 1, self.AccrssorySkillItem:Length() do
        self.AccrssorySkillItem[i]:SetData({skill_id = now_select_data["skill_"..i], skill_level = 3, skill_num_suit = i == 1 and 2 or 5})
    end
end

function AccrssoryIllustrationView:GetAttrData(data)
    local t = {}
    local index = 1
    local len_data = #data
    for i = 1, len_data do
        local index = math.floor((i - 1) / 2) + 1
        if not t[index] then
            t[index] = {}
            t[index].index = index
            t[index].attr_list = {}
        end
        local t1 = TableCopy(data[i])
        t1.attr_name = t1.attr_name
        t1.attr_value = t1.attr_percent_value
        table.insert(t[index].attr_list, t1)
    end
    return t
end

--------------AccrssoryNameItem--------------
AccrssoryNameItem = AccrssoryNameItem or DeclareMono("AccrssoryNameItem", UIWidgetBaseItem)
function AccrssoryNameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Root.name = "AccrssoryNameItem"..data.suit_id
    UH.SetText(self.TxtName, data.suit_name)
end

function AccrssoryNameItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AccrssoryData.Instance.illustration_data.suit_id = self.data.suit_id
    end
end

--------------AccrssorySuitItem--------------
AccrssorySuitItem = AccrssorySuitItem or DeclareMono("AccrssorySuitItem", UIWidgetBaseItem)
function AccrssorySuitItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Root.name = "AccrssorySuitItem"..data.peishi_part
    local item = Item.Init(data.peishi_id, 1, 1)
    UH.SetText(self.TxtName, item:QuaName(true))
    UH.SetText(self.TxtPart, Format(Language.Accrssory.PartName, Language.Accrssory.EquipType[data.peishi_part]))
    self.Cell:SetData(item)
end

function AccrssorySuitItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AccrssoryData.Instance.illustration_data.part = self.data.peishi_part
    end
end

--------------------------AccrssoryIllustrationAttrItem--------------------------
AccrssoryIllustrationAttrItem = AccrssoryIllustrationAttrItem or DeclareMono("AccrssoryIllustrationAttrItem", UIWidgetBaseItem)
function AccrssoryIllustrationAttrItem:SetData(data)
    self.ObjShuXing1:SetActive(data.index % 2 == 1)
    for i = 1, 2 do
        self.AccrssoryAttrItem[i]:SetData(data.attr_list[i] or {})
    end
end

--------------------------AccrssoryAttrItem--------------------------
AccrssoryAttrItem = AccrssoryAttrItem or DeclareMono("AccrssoryAttrItem", UIWidgetBaseItem)
function AccrssoryAttrItem:SetData(data)
    if data.attr_name then
        -- 是否需要冒号
        if data.not_need then
            UH.SetText(self.TxtAttrName, data.attr_name)
        else
            UH.SetText(self.TxtAttrName, data.attr_name .. ":")
        end
    else
        UH.SetText(self.TxtAttrName, "")
    end
    local value = data.show_percent and data.attr_percent_value or data.attr_value
    if data.attr_value then
        if data.not_add then
            UH.SetText(self.TxtAttrValue, value)
        else
            UH.SetText(self.TxtAttrValue, "+" .. value)
        end
    else
        UH.SetText(self.TxtAttrValue, "")
    end
    if data.show_arrow then
        self.ImgSetArrow:SetObjActive(true)
        UH.SpriteName(self.ImgSetArrow, data.is_up and "up_s" or "down_s")		
    else
        self.ImgSetArrow:SetObjActive(false)
    end
end

--------------------------AccrssoryAttrItem2--------------------------
AccrssoryAttrItem2 = AccrssoryAttrItem2 or DeclareMono("AccrssoryAttrItem2", UIWidgetBaseItem)
function AccrssoryAttrItem2:SetData(data)
    if data.attr_name then
        UH.SetText(self.TxtAttrName, data.attr_name)
    else
        UH.SetText(self.TxtAttrName, "")
    end
    if data.attr_value then
        UH.SetText(self.TxtAttrValue, "+" .. data.attr_value)
    else
        UH.SetText(self.TxtAttrValue, "")
    end
    self.ObjLine:SetActive(not data.is_last)
end

--------------------------AccrssoryTxtItem--------------------------
AccrssoryTxtItem = AccrssoryTxtItem or DeclareMono("AccrssoryTxtItem", UIWidgetBaseItem)
function AccrssoryTxtItem:SetData(data)
    UH.SetText(self.Txt, data.txt)
end