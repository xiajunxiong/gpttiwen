LingBaoBagView = LingBaoBagView or DeclareView("LingBaoBagView", "ling_bao/ling_bao_bag")
function LingBaoBagView:LingBaoBagView()
    
end

function LingBaoBagView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.LingBao.BagTitle, Vector2.New(574, 600))
end

function LingBaoBagView:OnCloseClick()
    ViewMgr:CloseView(LingBaoBagView)
end

--------------------------LingBaoBagPanel--------------------------
LingBaoBagPanel = LingBaoBagPanel or DeclareMono("LingBaoBagPanel", UIWFlushPanel)
function LingBaoBagPanel:LingBaoBagPanel()
    self.Data = LingBaoData.Instance
    self.data_cares = {
        {data = self.Data:ItemList(), func = self.FlushBag, init_call = false},
        {data = self.Data:EquipList(), func = self.FlushBag, init_call = false},
    }
    self.select_state = 0
end

function LingBaoBagPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.GetWayList:SetData(self.Data:GetWayConfig())
    self:FlushBag()
end

function LingBaoBagPanel:FlushBag()
    self.bag_data = self.Data:GetBagData(true, 20, 5, nil, self.select_state)
    self.BagList:SetData(self.bag_data)
end

function LingBaoBagPanel:OnLevelSelectClick()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.select_data = self.Data:GetStateDataByList(self.select_state)
        self.BtnList:SetData(self.select_data)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function LingBaoBagPanel:OnCloseShaiXuanClick()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end 

function LingBaoBagPanel:OnSelectClick(data)
    UH.SetText(self.TxtSelected, data.name)
    self.select_state = data.state
    self:FlushBag()
    self:OnLevelSelectClick()
end 

function LingBaoBagPanel:OnResolveClick()
    ViewMgr:OpenView(LingBaoResolveView)
end

function LingBaoBagPanel:OnGetWayClick()
    self:SetGetWayValue(true)
end

function LingBaoBagPanel:SetGetWayValue(value)
    self.ObjGetWay:SetActive(value)
end

--------------------------LingBaoSelectBtnItem--------------------------
LingBaoSelectBtnItem = LingBaoSelectBtnItem or DeclareMono("LingBaoSelectBtnItem", UIWidgetBaseItem)
function LingBaoSelectBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjLine:SetActive(data.state ~= 0)
    UH.SetText(self.TxtName, data.is_select and ColorStr(data.name, COLORSTR.Yellow1) or data.name)
end