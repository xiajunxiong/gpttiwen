-- 幻兽灵魄分解
HuanShouSoulResolve = HuanShouSoulResolve or DeclareView("HuanShouSoulResolve", "huanshou/huan_shou_soul_resolve", Mod.HuanShou.SoulResolve)
function HuanShouSoulResolve:HuanShouSoulResolve()
    self.Board:SetData(self:GetType(), Language.HuanShou.Title3, Vector2.New(877, 592))
end


HuanShouSoulResolvePanel = HuanShouSoulResolvePanel or DeclareMono("HuanShouSoulResolvePanel",UIWFlushPanel)
function HuanShouSoulResolvePanel:HuanShouSoulResolvePanel()
    self.data = HuanShouData.Instance
    self.data_cares = {
        --{data = self.data.bag_item_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false}

    }
    local lang = Language.HuanShou.ResQuaType
    self.type_list = {
        {text = lang[0], qua = ItemColor.None},
        {text = lang[1], qua = ItemColor.White},
        {text = lang[2], qua = ItemColor.Green},
        {text = lang[3], qua = ItemColor.Blue},
        {text = lang[4], qua = ItemColor.Purple},
        {text = lang[5], qua = ItemColor.Orange},
        {text = lang[6], qua = ItemColor.Red},
        {text = lang[9], qua = ItemColor.Gold},
    }
    self.jinghua_item_id = Config.huanshou_auto.other[1].jinghua_item
end

function HuanShouSoulResolvePanel:Awake()
    UIWFlushPanel.Awake(self)
	self:FlushPanel()
end

function HuanShouSoulResolvePanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

function HuanShouSoulResolvePanel:FlushPanel()
	self.data:ClearResolveSelectItems()
    self.item_list = self.data:GetAllSoulItem()
	local item_list_plus = self.item_list
	if #self.item_list < 30 then
		item_list_plus = self.item_list
		local plus = 30 - #self.item_list
        for i = plus ,1, -1 do 
            local item = {item_id = 0}
            table.insert(item_list_plus,item)
        end 
	end
    self.Items:SetData(item_list_plus)
	self:FlushInfo()
end

function HuanShouSoulResolvePanel:FlushInfo()
    local type_data = self.type_data
    local ex_num = self.data:SetResolveSelectItemsExportNum()
    local ex_item_id = self.jinghua_item_id
    local ex_item = Item.Create({item_id = ex_item_id})
    self.ExItem:SetData(ex_item)
    UH.SetText(self.ExNum, string.format("%s  x%s", ex_item:QuaName(), ex_num))

    if type_data then
        local text = type_data.text
        if type_data.qua then
            if QualityColorStr[type_data.qua] then
                text = ColorStr(text, QualityColorStrShallow[type_data.qua])
            end
        end
		UH.SetText(self.SelectBtnText,text)
	else
		UH.SetText(self.SelectBtnText, Language.HuanShou.Text10)
	end
end

function HuanShouSoulResolvePanel:OnClickLevelSelect()
	local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.type_list)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function HuanShouSoulResolvePanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function HuanShouSoulResolvePanel:OnClickTypeBtn(data)
    self:OnClickCloseShaiXuan()
    self.Items:CancelSelect()
    self.data:ClearResolveSelectItems()
	for i,v in pairs(self.item_list) do
		if v.item_id > 0 and v:Color() == data.qua or data.qua == ItemColor.None then
			self.Items:SetectItem(i,true)
			self.data:SetResolveSelectItems(v.index,v)
		end
    end
    self.type_data = data
	self:FlushInfo()
end

function HuanShouSoulResolvePanel:OnClickItem(value,data)
    if data == nil or data.item_id == 0 then return end 
	if data.index == nil then
		return
    end
	self.data:SetResolveSelectItems(data.index,data)
	self:FlushInfo()
end

function HuanShouSoulResolvePanel:OnBreakClick()
	local select_items = self.data:GetResolveSelectItems()
	if TableIsEmpty(select_items) then
		PublicPopupCtrl.Instance:Center(Language.HuanShou.Tip6)
		return
    end
    if self.playing then
        return
    end
   
    local break_func = function()
        self.Lock:SetActive(true)
        self.playing = true
        self.EffectTool:Play("30000825")
        for k, v in pairs(self.Items.index_item)do
            if v:IsSelected() then
                v:Play()
            end
        end
    end
    local sel_num = table.nums(select_items)
    local ex_num = self.data:SetResolveSelectItemsExportNum()
    local ex_item_id = self.jinghua_item_id
	local param_t = {
        content = string.format(Language.HuanShou.ResTip1,sel_num,ex_num, Item.GetQuaName(ex_item_id)),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                break_func()
            end 
        },
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function HuanShouSoulResolvePanel:EffectEndFunc()
    self.playing = false
    self.Lock:SetActive(false)
    local select_items = self.data:GetResolveSelectItems()
    if TableIsEmpty(select_items) then
		return
    end
    local index_list = {}
    for i,v in pairs(select_items) do
        if v.index ~= nil then
            table.insert(index_list, v.index)
        end
    end
    HuanShouCtrl.Instance:SendResolve(index_list)
end

HuanShouResolveTypeItem = HuanShouResolveTypeItem or DeclareMono("HuanShouResolveTypeItem", UIWidgetBaseItem)
function HuanShouResolveTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local text = data.text
    if data.qua then
        if QualityColorStr[data.qua] then
            text = ColorStr(text, QualityColorStr[data.qua])
        end
    end
    UH.SetText(self.Text, text)
end

HuanShouResolveCell = HuanShouResolveCell or DeclareMono("HuanShouResolveCell", UIWidgetBaseItem)

function HuanShouResolveCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Item:SetData(data)
    UH.LocalScale(self.AnimRect, Vector3.one)
end

function HuanShouResolveCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function HuanShouResolveCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function HuanShouResolveCell:Play()
    self.Animation:Play("resolve_scale")
    self.EffectTool:Play("30000813")
end