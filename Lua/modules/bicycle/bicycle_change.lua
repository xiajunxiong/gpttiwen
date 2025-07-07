BicycleChangeView = BicycleChangeView or DeclareView("BicycleChangeView", "bicycle/bicycle_change")
function BicycleChangeView:BicycleChangeView()

end

function BicycleChangeView:LoadCallback(param_t)
    
end

function BicycleChangeView:OnCloseClick()
    ViewMgr:CloseView(BicycleChangeView)
end

BicycleChangePanel = BicycleChangePanel or DeclareMono("BicycleChangePanel", UIWFlushPanel)
function BicycleChangePanel:BicycleChangePanel()
    self.Data = BicycleData.Instance
    self.language = Language.Bicycle.Change
    self.data = self.Data.change_data
    self.change_success = false
    self.data_cares = {
        {data = self.data, func = self.FlushShow, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushShow, init_call = false},
    }
    self.item_id = self.Data:ChangeItemList(self.data.change_type)
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:ChangeItemList(), BindTool.Bind(self.FlushItem, self))
    end
end 

function BicycleChangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.wait_handle)
    self.Data:ClearChangeItem()
    BagData.Instance:RemoveListenItems(self.item_listen)
end

function BicycleChangePanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SpriteName(self.ImgTitle, "_LocZhuanHuanZi"..self.data.change_type)
    UH.SetIcon(self.Icon, Item.GetIconId(self.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.TxtSelect, self.language.Name[1][self.data.change_type])
    UH.SetText(self.TxtTarget, self.language.Name[2][self.data.change_type])
    UH.SetText(self.TxtTip, self.language.Tip[7][self.data.change_type])
    self:FlushShow()
    self:FlushItem()
end

function BicycleChangePanel:FlushShow()
    if self.change_success then
        MainOtherCtrl.Instance:OpenGiftView({[1] = self.data.change}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
        PublicPopupCtrl.Instance:CenterI({item_id = self.data.change.item_id, num = 1,
            color_str = self.data.change:ColorStr(),item = self.data.change,icon_id = self.data.change:IconId()})
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = self.data.change, num = 1}}
        self.Data:ClearChangeItem()
        self.change_success = false
        self.ObjBlock:SetActive(false)
    end
    self.CellSelect:SetData(self.data.now)
    self.CellTarget:SetData(self.data.change)
    local is_has_now = not TableIsEmpty(self.data.now)
    local is_has_change = not TableIsEmpty(self.data.change)
    self.ObjSelect:SetActive(is_has_now)
    self.ObjTarget:SetActive(is_has_change)
    -- self.TxtSelect:SetObjActive(not is_has_now)
    -- self.TxtTarget:SetObjActive(not is_has_change)
    UH.SetText(self.TxtNameSelect, is_has_now and self.data.now:QuaName() or "")
    UH.SetText(self.TxtNameTarget, is_has_change and self.data.change:QuaName() or "")
end

function BicycleChangePanel:FlushItem()
    local has_num = Item.GetNum(self.item_id)
    UH.SetText(self.TxtNum, Format(Language.Common.Slash, has_num, 1))
end

function BicycleChangePanel:OnChangeItemClick()
    ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Create({item_id = self.item_id})}}
end

-- 消耗格子：1，目标格子：2
function BicycleChangePanel:OnItemClick(type)
    if type == 2 and TableIsEmpty(self.data.now) then
        PublicPopupCtrl.Instance:Center(self.language.Tip[6][self.data.change_type])
        return
    end
    ViewMgr:OpenView(BicycleChangeListView, {type = type})
end

function BicycleChangePanel:OnConfirmClick()
    if TableIsEmpty(self.data.now) then 
        PublicPopupCtrl.Instance:Center(self.language.Tip[1][self.data.change_type])
        return 
    end 

    if TableIsEmpty(self.data.change) then
        PublicPopupCtrl.Instance:Center(self.language.Tip[2][self.data.change_type])
        return 
    end 

    if Item.GetNum(self.item_id) == 0 then 
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = self.item_id})})
        PublicPopupCtrl.Instance:Center(Format(self.language.Tip[3], Item.GetName(self.item_id)))
        return 
    end 

	local info = {
		content = string.format(self.language.Tip[5], 
        self.data.now:QuaName(), self.data.change:QuaName()),
		confirm = {
			func = function()
                self.EffectTool:Play(3165000)
                self.ObjBlock:SetActive(true)
                self.Arrow:SetInteger(APH("state"), 1)
                self.change_success = true
                ViewMgr:CloseView(DialogTipsView)
            end
		},
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end

function BicycleChangePanel:EffectComplete()
    self.Arrow:SetInteger(APH("state"), 0)
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CHANGE_ITEM, self.data.now.item_id, self.data.change.item_id)
end

----------------------------BicycleChangeListView----------------------------
BicycleChangeListView = BicycleChangeListView or DeclareView("BicycleChangeListView", "bicycle/bicycle_change_list")
VIEW_DECLARE_MASK(BicycleChangeListView, ViewMask.BgBlock)
function BicycleChangeListView:BicycleChangeListView()
    self.Data = BicycleData.Instance
    self.language = Language.Bicycle.Change
end

function BicycleChangeListView:LoadCallback(param_t)
    self.m_type = param_t.type
    self.change_type = BicycleData.Instance:ChangeItemType()
    local title = self.language.Name2[self.m_type][self.change_type]
    self.Board:SetData(self:GetType(), title, Vector2.New(656, 493))
    UH.SetText(self.TxtEmpty, self.language.Tip[4][self.change_type])
    self.list_data = self.Data:GetChangeItemList(self.m_type)
    self.ObjEmpty:SetActive(TableIsEmpty(self.list_data))
    self.List:SetData(self.list_data)
end

function BicycleChangeListView:OnSelect(data)
    self.select_data = data
end

function BicycleChangeListView:OnConfirmClick()
    if not TableIsEmpty(self.select_data) then
        if self.m_type == 1 then
            self.Data:ChangeItemNow(self.select_data)
            self.Data:ChangeItemChange({})
        else
            self.Data:ChangeItemChange(self.select_data)
        end
    end
    ViewMgr:CloseView(BicycleChangeListView)
end

----------------------------BicycleChangeSelectItem----------------------------
BicycleChangeSelectItem = BicycleChangeSelectItem or DeclareMono("BicycleChangeSelectItem", UIWidgetBaseItem)
function BicycleChangeSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.TxtName, data:QuaName())
end