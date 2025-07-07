GodClothesSelectView = GodClothesSelectView or DeclareView("GodClothesSelectView", "god_clothes/god_clothes_select")
VIEW_DECLARE_MASK(GodClothesSelectView, ViewMask.BgBlock)
function GodClothesSelectView:GodClothesSelectView()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesSelectView:LoadCallback(param_t)
    
end

function GodClothesSelectView:OnCloseClick()
    ViewMgr:CloseView(GodClothesSelectView)
end


----------------------------GodClothesSelectPanel----------------------------
GodClothesSelectPanel = GodClothesSelectPanel or DeclareMono("GodClothesSelectPanel", UIWFlushPanel)
function GodClothesSelectPanel:GodClothesSelectPanel()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
end

function GodClothesSelectPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_select = 0
    self.max_select = 3
    self.now_select_data = {}
    self.ObjBlock:SetActive(false)
    self.List:SetData(self.Data:SelectShow())
    self:FlushAll()
end

function GodClothesSelectPanel:FlushAll()
    UH.SetText(self.TxtDesc, Format(self.language.SelectStr, self.now_select, self.max_select))
    if self.Data:IsSelect() then
        ViewMgr:CloseView(GodClothesSelectView)
    end
end

function GodClothesSelectPanel:OnSelect(mono, selected)
    local mono_data = mono:GetData()
    self.now_select = #self.List:GetSelectedData()
    if self.now_select > self.max_select then
        mono:SetSelected(false)
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[4])
    end
    UH.SetText(self.TxtDesc, Format(self.language.SelectStr, self.now_select, self.max_select))
end

function GodClothesSelectPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function GodClothesSelectPanel:OnConfirmClick()
    if self.now_select == self.max_select then
        local select_data = self.List:GetSelectedData()
        GodClothesCtrl.Instance:SendReq(GodClothesReqType.Select, select_data[1].seq, select_data[2].seq, select_data[3].seq)
        self.ObjBlock:SetActive(true)
    else
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[5])
    end
end

--------------GodClothesSelectItem--------------
GodClothesSelectItem = GodClothesSelectItem or DeclareMono("GodClothesSelectItem", UIWidgetBaseItem)
function GodClothesSelectItem:GodClothesSelectItem()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.New(data.reward_item)
    UH.SetText(self.TxtName, item:QuaName(true))
    self.Cell:SetData(item)
end