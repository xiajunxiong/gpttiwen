IllustrateMainView = IllustrateMainView or DeclareMono("IllustrateMainView", UIWFlushPanel)

function IllustrateMainView:IllustrateMainView()
    self.data = IllustrateData.Instance
    self.data_cares = {
        {data = self.data.select_data,func = self.FlushListView,init_call = true,keys = {"type"}},
        {data = self.data.active_data,func = self.FlushPageListView,init_call = false,keys = {"list"}},
        {data = self.data.page_red_piont,func = self.FlushTabList,init_call = false,keys = {"list"}},
        {data = BagData.Instance.item_grid,func = self.FlushPageListView,init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BtnIllustrated", function()
        if self.CardPanelList.index_item then
            for k,item in pairs(self.CardPanelList.index_item) do
                local obj,func = item:GetItemGameObject(GuideData.Instance:IllustrateItem())
                if obj ~= nil then
                    return obj,func
                end
            end
        end
    end)
    self.data:SetSelectFunc(function(type)
        if type == self.data:GetSelectType() then
            self:FlushListView()
        else
            local index = DataHelper.SimpleIndex(self.data:GetBtnList(),"page",type)
            self.TabList:SetDataList(self.data:GetBtnList(),index)
        end
    end)
end

function IllustrateMainView:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTabList()
    self:FlushCardProgress()
    self:FlushAttrCardView()
    self.CardAttrBlock:SetActive(false)
    self.CardPanelList:SetData(self.data:GetIllustrateCardData())
end

function IllustrateMainView:FlushTabList()
    local index = DataHelper.SimpleIndex(self.data:GetBtnList(),"page",self.data:GetSelectType())
    self.TabList:SetDataList(self.data:GetBtnList(),index)
end

function IllustrateMainView:FlushPageListView()
    if self.data:GetSelectType() ~= 7 then
        self.PanelList:SetData(self.data:GetPageList())
    end
    self:FlushCardProgress()
    self:FlushAttrCardView()
end

function IllustrateMainView:FlushListView()
    local page_list = self.data:GetPageList()
    if #page_list ~= 0 and self.data:GetSelectType() ~= 7 then
        self.PanelList:SetData(page_list)
        local index = self:GetIndex(page_list,self.data:GetSelectSeq()) or 0
        if index > 2 then
            self.PanelList:JumpVerticalIndex(index,164)
            self.data:SetSelectSeq()
        else
	        TimeHelper:CancelTimer(self.show_time)
            self.show_time = TimeHelper:AddDelayTimer(function()
                self.PanelList:JumpToTop()
            end, 0.2)
        end
    else
        self:FlushAttrTypeView()
    end
    self.AttrPanel:SetActive(#page_list == 0)
    self.PanelList:SetObjActive(self.data:GetSelectType() ~= 7)
    self.ObjIllustrateCardPanel:SetActive(self.data:GetSelectType() == 7)
end

function IllustrateMainView:GetIndex(list,seq)
    for k,v in pairs(list or {}) do
        if v.seq == seq then
            return k
        end
    end
end

function IllustrateMainView:FlushAttrTypeView()
    local attr_data = self.data:GetActiveAttrData()
    self.BaseList:SetData(self.data:GetAttrTypeData(attr_data,IllustrateConfig.BaseType))
    self.ModifyList:SetData(self.data:GetAttrTypeData(attr_data,IllustrateConfig.ModifyType))
    self.DefenceList:SetData(self.data:GetAttrTypeData(attr_data,IllustrateConfig.DefenceType))
end

-- 刷新图鉴卡册的总属性
function IllustrateMainView:FlushAttrCardView()
    local attr_card_total = self.data:GetActiveAttrDataByList(self.data:GetPageList(7))
    self.CardAttrList:SetData(self.data:GetAttrTypeData(attr_card_total, IllustrateConfig.BaseType))
end

-- 刷新图鉴卡册的进度条
function IllustrateMainView:FlushCardProgress()
    local page_list = self.data:GetPageList(7)
    local num = self.data:GetListActiveNum(page_list)
    self.Progress:SetText(Format("%s/%s", num, #page_list))
    self.Progress:SetProgress(num / #page_list)
end

function IllustrateMainView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.show_time)
end

function IllustrateMainView:OnCardAttrBlockClick()
    self.CardAttrBlock:SetActive(false)
end

function IllustrateMainView:OnAttrShowClick()
    self.CardAttrBlock:SetActive(true)
end

----------------------------IllustrateBtnItem----------------------------
IllustrateBtnItem = IllustrateBtnItem or DeclareMono("IllustrateBtnItem", UIWidgetBaseItem)

function IllustrateBtnItem:SetData(data)
    UH.SetText(self.Name,data.page_name)
    local is_open = RoleData.Instance:GetRoleLevel() >= (data.level or 0)
    if is_open then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.Root, Vector3.New(68,24,0))
            self.red_point:SetNum(0)
        end
        self.red_point:SetNum(IllustrateData.Instance:GetPageRedNum(data.page))
    end
    self.Lock:SetActive(not is_open)
    self.BtnInter.Interactable = is_open
    UIWidgetBaseItem.SetData(self, data)
end

function IllustrateBtnItem:OnClickItem()
	IllustrateData.Instance:SetSelectType(self.data.page)
end

function IllustrateBtnItem:OnClickLock()
    PublicPopupCtrl.Instance:Center(Format(ErrorText[90],self.data.level))
end

function IllustrateBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

----------------------------IllustrateItem----------------------------
IllustrateItem = IllustrateItem or DeclareMono("IllustrateItem", UIWidgetBaseItem)

function IllustrateItem:SetData(data)
    UH.SetText(self.Name,data.collection_name)
    UH.SpriteName(self.Color,"illustrate_"..data.color)
    UH.SpriteName(self.Bottom,"illustrate"..data.color)
    self.Name.color = IllustrateConfig.COLOR[data.color]
    UH.SetText(self.Desc,Language.Illustrate.BackCard[data.back_card])
    self.Desc:SetObjActive(data.limit_prof ~= 0)
    self.AtrtrList:SetData(data.attr_list)

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnActive, Vector3.New(79,25,0))
        self.red_point:SetNum(0)
    end
    local is_active = IllustrateData.Instance:GetActiveSeq(data.seq)
    if not is_active then
        local can_active = self:GetCanActive(data.item_list)
        self.red_point:SetNum(can_active and 1 or 0)
        self.BtnInter.Interactable = can_active
        UH.SetText(self.BtnName,Language.Illustrate.BtnName[can_active and 1 or 2])
    else
        self:FlushItemList(data.item_list)
    end
    self.Complete:SetActive(is_active)
    self.Effect:SetActive(false)
    UIWidgetBaseItem.SetData(self, data)
end

function IllustrateItem:FlushItemList(item_list)
    for k,v in pairs(item_list) do
        v.is_active = true
    end
    self.ItemList:SetData(item_list)
end

function IllustrateItem:OnClickActive()
    self.Effect:SetActive(true)
	self.effect_time = TimeHelper:AddDelayTimer(function()
        IllustrateCtrl.Instance:SendCollectSeq(IllustrateConfig.Req.Active, self.data.seq)
        self.Complete:SetActive(true)
        self.Effect:SetActive(false)
        self.red_point:SetNum(0)
        self:FlushItemList(self.data.item_list)
	end, 0.5)
end

function IllustrateItem:OnRelease()
	UIWidgetBaseItem.OnRelease(self)
	TimeHelper:CancelTimer(self.effect_time)
end

function IllustrateItem:OnDestroy()
	TimeHelper:CancelTimer(self.effect_time)
end

function IllustrateItem:GetCanActive(item_list,is_active)
    for k,v in pairs(item_list) do
        v.have = Item.GetNum(v.item_id)
        v.is_active = false
    end
    self.ItemList:SetData(item_list)
    for k,v in pairs(item_list) do
        if v.have < v.num then
            return false
        end
    end
    return true
end

----------------------------IllustrateCell----------------------------
IllustrateCell = IllustrateCell or DeclareMono("IllustrateCell",UIWidgetBaseItem)
function IllustrateCell:SetData(data)
    local item = Item.New({item_id = data.item_id})
    self.Cell:SetData(item)
    UH.SetText(self.Name,string.sub(item:Name(),1,string.len(item:Name())-6))
    if not data.is_active then
        local has_color = data.have >= data.num and COLORSTR.Green4 or COLORSTR.Yellow1
        has_color = data.have == 0 and COLORSTR.White or has_color
        local color = data.have >= data.num and COLORSTR.Green4 or COLORSTR.White
        UH.SetText(self.Num,Format("<color=#%s>%d</color><color=#%s>/%d</color>",
        has_color,data.have,color,data.num))
    else
        UH.SetText(self.Num,"")
    end
    self.Mark:SetActive(data.is_active)
    UIWidgetBaseItem.SetData(self, data)
end

----------------------------IllustrateAttrItem----------------------------
IllustrateAttrItem = IllustrateAttrItem or DeclareMono("IllustrateAttrItem",UIWidgetBaseItem)
function IllustrateAttrItem:SetData(data)
    UH.SetText(self.AttrName,data.name)
    UH.SpriteName(self.Image,"attr_item"..data.color)
end

----------------------------IllustrateAttrCell----------------------------
IllustrateAttrCell = IllustrateAttrCell or DeclareMono("IllustrateAttrCell",UIWidgetBaseItem)
function IllustrateAttrCell:SetData(data)
    UH.SetText(self.Name,data.attr_name)
    local attr_value = data.attr_value
    if DataHelper.IsPercent(data.attr_type) then
        attr_value = Percent(attr_value)
    end
    UH.SetText(self.Value,"+" .. attr_value)
end

----------------------------IllustrateCardItem----------------------------
IllustrateCardItem = IllustrateCardItem or DeclareMono("IllustrateCardItem",UIWidgetBaseItem)
function IllustrateCardItem:SetData(data)
    self.height = #data.item_list
    self.List:SetData(data.item_list)
    UH.SetText(self.PageName, Language.Illustrate.CollectCardName[data.color - 1])
    self:FlushTitleView(IllustrateConfig.CollectCardColor[data.color])
    UIWidgetBaseItem.SetData(self, data)
    if not self.is_care then
        self.is_care = true
        self:careData(IllustrateData.Instance.active_change, BindTool.Bind(self.FlushRedPageView, self), "flag")
    end
    self:FlushRedPageView()
end

function IllustrateCardItem:FlushTitleView(color)
    self.Gradient.topColor = color[1]
    self.Gradient.buttomColor = color[2]
    self.Outline.effectColor = color[3]
    self.Shadow.effectColor = color[3]
end

function IllustrateCardItem:FlushRedPageView()
    local active_num = IllustrateData.Instance:GetListActiveNum(self.data.item_list)
    UH.SetText(self.PageNum,Format("（%s/%s）",active_num, #self.data.item_list))
end

function IllustrateCardItem:ExpendSize(data, axis)
    return math.ceil(#data.item_list / 6) * 236 + 30
end

function IllustrateCardItem:GetItemGameObject(seq)
    for k,item in pairs(self.List.index_item) do
        local obj,func = item:GetItemGameObject(seq)
        if obj ~= nil then
            return obj,func
        end
    end
end

function IllustrateCardItem:OnDestroy()
    if self.is_care then
        self:uncareData()
    end
end

----------------------------CardItem----------------------------
CardItem = CardItem or DeclareMono("CardItem",UIWidgetBaseItem)

function CardItem:SetData(data)
    UH.SpriteName(self.Image, IllustrateConfig.CollectCardName .. data.color)
    UH.SetText(self.Name, Item.GetName(data.item_list[1].item_id))
    local attr = data.attr_data
    UH.SetText(self.TxtAttr, Format(Language.Illustrate.CollectCardAttr, data.attr_data[1].attr_name, data.attr_data[1].attr_value))
    if not self.data or self.data.seq ~= data.seq then
        self.EffectShowActive:SetActive(false)
    end
    UIWidgetBaseItem.SetData(self, data)
    
    if not self.is_care then
        self.is_care = true
        self:careData(BagData.Instance.item_grid, BindTool.Bind(self.FlushRedNumView, self))
        self:careData(IllustrateData.Instance.active_change, BindTool.Bind(self.FlushRedNumView, self), "flag")
    end
    self:FlushRedNumView()
end

function CardItem:OnClickItem()
    if self.RedPos.activeSelf then
        self.red_point:SetNum(0)
        IllustrateCtrl.Instance:SendCollectSeq(IllustrateConfig.Req.Active, self.data.seq)
        
        if self.effect_handle1 then
            self.EffectTool:Stop(self.effect_handle1)
            self.effect_handle1 = nil
        end
        if self.effect_handle1 == nil then
            self.effect_handle1 = self.EffectTool:Play(100108225)
        end
        self.EffectShowActive:SetActive(true)

        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
        TimeHelper:CancelTimer(self.effect_time)
        self.effect_time = TimeHelper:AddDelayTimer(function()
            TimeHelper:CancelTimer(self.effect_time)
            self.EffectShowActive:SetActive(false)
        end, 2)
    end
end

function CardItem:GetCanActive(item_list)
    for k,v in pairs(item_list) do
        v.have = Item.GetNum(v.item_id)
    end
    if IllustrateData.Instance:GetActiveSeq(self.data.seq) then
        return false
    end
    for k,v in pairs(item_list) do
        if v.have < v.num then
            return false
        end
    end
    return true
end

function CardItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.effect_time)
    if self.is_care then
        self:uncareData()
    end
end

function CardItem:FlushRedNumView()
    local has_active = IllustrateData.Instance:GetActiveSeq(self.data.seq)
    local can_active = self:GetCanActive(self.data.item_list)
    
    self.InterHandler.Interactable = has_active
    self.ObjOnClick:SetActive(can_active)
    self.Image:SetObjActive(has_active)

    self.EffectCanActive:SetActive(can_active)
    if self.EffectCanActive.activeSelf then
        if self.effect_handle2 == nil then
            self.effect_handle2 = self.EffectTool:Play(100108224)
        end
    end
    
    UH.SetText(self.TxtCollect, Format("%s/%s", self.data.item_list[1].have, self.data.item_list[1].num))
    self.TxtCollect:SetObjActive(not has_active)
    if can_active then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.RedPos, Vector3.zero)
        end
        self.red_point:SetNum(1)
    end
    local item = Item.New({item_id = self.data.item_list[1].item_id})
    item.is_grey = not self.InterHandler.Interactable
    self.Cell:SetData(item)
    self.RedPos:SetActive(can_active)
end

function CardItem:GetItemGameObject(seq)
    if seq == self.data.seq then
        return self.gameObject,function()
            self:OnClickItem()
        end
    end
end

----------------------------CardAttrItem----------------------------
CardAttrItem = CardAttrItem or DeclareMono("CardAttrItem",UIWidgetBaseItem)
function CardAttrItem:SetData(data)
    local attr_value = data.attr_value
    if DataHelper.IsPercent(data.attr_type) then
        attr_value = Percent(attr_value)
    end
    UH.SetText(self.TxtCardAttrItem, Format(Language.Illustrate.CollectCardAttr, data.attr_name, attr_value))
end