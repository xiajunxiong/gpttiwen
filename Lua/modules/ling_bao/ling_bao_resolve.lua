LingBaoResolveView = LingBaoResolveView or DeclareView("LingBaoResolveView", "ling_bao/ling_bao_resolve",Mod.Marbas.Resolve)
function LingBaoResolveView:LingBaoResolveView()
end

function LingBaoResolveView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.LingBao.ResolveTitle, Vector2.New(574, 620))
end

function LingBaoResolveView:OnCloseClick()
    ViewMgr:CloseView(LingBaoResolveView)
end

--------------------------LingBaoResolvePanel--------------------------
LingBaoResolvePanel = LingBaoResolvePanel or DeclareMono("LingBaoResolvePanel", UIWFlushPanel)
function LingBaoResolvePanel:LingBaoResolvePanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data_cares = {
        {data = self.Data:ItemList(), func = self.FlushResolve, init_call = false},
    }
    self.select_state = 0
end

function LingBaoResolvePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushResolve()
end

function LingBaoResolvePanel:FlushResolve()
    self.select_data = nil
    self.bag_data = self.Data:GetBagData(false, 20, 5, nil, self.select_state, true)
    self.BagList:SetData(self.bag_data)
    self.ObjEmpty:SetActive(TableIsEmpty(self.bag_data))
    self:OnSelect()
end

function LingBaoResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.time_func then 
        TimeHelper:CancelTimer(self.time_func)
    end 
end

function LingBaoResolvePanel:OnEffectComplete()
    self.ObjBlock:SetActive(false)

    local select_items = self.BagList:GetSelectedItem()
    local params = {}

    for k,v in pairs(select_items) do 
        table.insert(params, v:GetData():GridIndex())
    end 
    LingBaoCtrl.Instance:SendResolve(params)
end

function LingBaoResolvePanel:OnSelect()
    local data = self.BagList:GetSelectedData()
    --获取分解内容
    local resolve_list = self.Data:GetResolveList(data)
    self.List:SetData(resolve_list)
    self.select_data = data
end

function LingBaoResolvePanel:OnLevelSelectClick()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.select_data = self.Data:GetStateDataByList(self.bag_data, self.select_state)
        self.BtnList:SetData(self.select_data)
        self:OnSelect()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function LingBaoResolvePanel:OnCloseShaiXuanClick()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end 

function LingBaoResolvePanel:OnSelectClick(data)
    UH.SetText(self.TxtSelected, data.name)
    self.select_state = data.state
    self:FlushResolve()
    self:OnLevelSelectClick()
end 

function LingBaoResolvePanel:OnClickResolve()
    if self.select_data == nil then
        return
    end
    if self.is_playing == true then
        return
    end
    local select_items = self.BagList:GetSelectedItem()
    if TableIsEmpty(select_items) then 
        PublicPopupCtrl.Instance:Center(self.language.ResolveEmptyTips)
        return 
    end 

    self.is_playing = true
    
    --做个动画做完再发协议
    for i, v in ipairs(select_items) do
        v:PlayAnim()
    end
    self.handle_delay =
    TimeHelper:AddDelayTimer(
    function()
        -- self.EffectTool:Play(30000825)
    end,
    0.4)

    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(3165232)
            self.EffectTool:Play(3165233)
            self.is_playing = false
        end,
        0.8
    )
    self.ObjBlock:SetActive(true)
end

--------------------------LingBaoResolveItem--------------------------
LingBaoResolveItem = LingBaoResolveItem or DeclareMono("LingBaoResolveItem", UIWidgetBaseItem)
function LingBaoResolveItem:LingBaoResolveItem()
    self.is_long_click = false
end

function LingBaoResolveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)

    self.Cell.MengBan.transform:SetAsLastSibling()
    self:OnSelect(false)
end 

function LingBaoResolveItem:OnSelect(selected)
    if not self.is_long_click then
        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
    end 
end 

function LingBaoResolveItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end

function LingBaoResolveItem:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.Cell)
            end
        end,
        0.2
    )
end

function LingBaoResolveItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if ViewMgr:IsOpen(LingBaoResolveView) then 
                self.EffectTool:Play(30000813)
            end 
        end,
        0.3336
    )
end


--------------------------LingBaoResolvePreShowCell--------------------------
LingBaoResolvePreShowCell = LingBaoResolvePreShowCell or DeclareMono("LingBaoResolvePreShowCell", UIWidgetBaseItem)
function LingBaoResolvePreShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.Icon, data:IconId())
    UH.SetText(self.Content, data:QuaName().."x"..data:Num())
end

