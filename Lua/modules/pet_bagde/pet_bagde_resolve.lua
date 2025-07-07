PetBagdeResolve = PetBagdeResolve or DeclareView("PetBagdeResolve", "pet/pet_bagde/pet_bagde_resolve", Mod.PetBagde.Resolve)
function PetBagdeResolve:PetBagdeResolve()
    self.Board:SetData(self:GetType(), Language.PetBagde.ResolveTitle)
end

function PetBagdeResolve:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[271].desc)
end

PetBagdeResolvePanel = PetBagdeResolvePanel or DeclareMono("PetBagdeResolvePanel", UIWFlushPanel)
function PetBagdeResolvePanel:PetBagdeResolvePanel()
    self.data = PetBagdeData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushPanel, init_call = false},
        {data = self.data.resolve_data, func = self.FlushStuff, init_call = false},
    }
end

function PetBagdeResolvePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function PetBagdeResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
    self.data:ClearReslove()
    PetBagdeResolvePanel.is_playing = false
end

function PetBagdeResolvePanel:FlushPanel()
    local bagde_list = self.data:GetAllBagdeOfBag() or {}
    self.EmptyState:SetActive(#bagde_list == 0)
    self.BagdeList:SetData(bagde_list)
    self:FlushStuff()
end

function PetBagdeResolvePanel:FlushStuff()
    local output_stuff_list = self.data:ResloveOutputStuffList()
    self.ItemList:SetData(output_stuff_list)
end

function PetBagdeResolvePanel:OnBtnClick()
    if PetBagdeResolvePanel.is_playing then
        return
    end
    if TableIsEmpty(self.data.resolve_data) then
        return
    end
    for i,v in pairs(self.BagdeList.item_list) do
        if v:IsSelected() then
            v:PlayAnim()
        end
    end
    PetBagdeResolvePanel.is_playing = true
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.EffectTool:Play(30000825)
    end, 0.4)
    self.time_ht_2 = Invoke(function ()
        PetBagdeResolvePanel.is_playing = false
        self:ResolveEvent()
        --self:FlushPanel()
    end, 2.2)
end

function PetBagdeResolvePanel:ResolveEvent()
    local item_info_list = {}
    for i,v in pairs(self.data.resolve_data) do
        if v then
            local item = i
            local info = {}
            info.col = item.column_type
            info.index = item.index
            table.insert(item_info_list, info)
        end
    end
    PetBagdeCtrl.Instance:SenResolve(item_info_list)
    self.data:ClearReslove()
end


PetBagdeResolveItem = PetBagdeResolveItem or DeclareMono("PetBagdeResolveItem", UIWidgetBaseItem)
function PetBagdeResolveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    local is_sel = PetBagdeData.Instance.resolve_data[data]
    self:SetSelected(is_sel)
end

function PetBagdeResolveItem:PlayAnim()
    self.Anim:Play()
end

function PetBagdeResolveItem:OnSelect(value)
    if PetBagdeResolvePanel.is_playing then
        self:SetSelected(false)
        return
    end
    PetBagdeData.Instance:AddReslove(self.data, value)
end

function PetBagdeResolveItem:OnPressDown()
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

function PetBagdeResolveItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
end

function PetBagdeResolveItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
end