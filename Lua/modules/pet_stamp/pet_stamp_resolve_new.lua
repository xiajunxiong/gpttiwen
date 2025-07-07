-- 新版神印分解
PetStampResolveNew = PetStampResolveNew or DeclareView("PetStampResolveNew", "pet/pet_stamp/pet_stamp_resolve_new")
function PetStampResolveNew:PetStampResolveNew()
    self.Board:SetData(self:GetType(), Language.PetStamp.Title5, Vector2.New(970, 592))
end


PetStampResolveNewPanel = PetStampResolveNewPanel or DeclareMono("PetStampResolveNewPanel",UIWFlushPanel)
function PetStampResolveNewPanel:PetStampResolveNewPanel()
    self.stamp_data = PetStampData.Instance
    self.data = PetStampResolveData.Instance
    self.data_cares = {
        {data = self.stamp_data.pet_stamp_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
    local lang = Language.PetStamp.ResQuaType
    self.type_list = {
        {text = lang[0], qua = ItemColor.None},
        {text = lang[1], qua = ItemColor.White},
        {text = lang[2], qua = ItemColor.Green},
        {text = lang[3], qua = ItemColor.Blue},
        {text = lang[4], qua = ItemColor.Purple},
        {text = lang[5], qua = ItemColor.Orange},
        {text = lang[6], qua = ItemColor.Red},
    }
end

function PetStampResolveNewPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.EmptyTip, Language.PetStamp.NoneResolve)
	self:FlushPanel()
end

function PetStampResolveNewPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

function PetStampResolveNewPanel:FlushPanel()
	self.data:ClearSelectItems()
    self.data:SetShowItem({})
    -- 全部道具材料
    local stuff_list = BagData.Instance:GetPetStampExpStuffList()
    self.stuff_count = table.nums(stuff_list)
    local stamp_list = self.stamp_data:GetBagGridList()
    self.item_list = AndList(stuff_list, stamp_list)
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
    self:SelectAllStuff()
	self:FlushInfo()
end

function PetStampResolveNewPanel:FlushInfo()
    local type_data = self.type_data
	local end_select_item = self.data:GetShowItem()
    self.InfoObj:SetActive(not TableIsEmpty(end_select_item))
    self.EmptyObj:SetActive(TableIsEmpty(end_select_item))
	if self.InfoObj.activeSelf then
		self.InfoMono:FlushPanel()
	end
    local exp_num,count,stuff_count = self.data:GetAllSelectItemResExp()
    local num_str = ColorStr(exp_num, COLORSTR.Yellow2)
    UH.SetText(self.ResoTip, string.format(Language.PetStamp.ResTip2,num_str))
    num_str = ColorStr(count, COLORSTR.Yellow2)
    local stuff_count_str = ColorStr(stuff_count, COLORSTR.Yellow2)
    UH.SetText(self.ResoTip2, string.format(Language.PetStamp.ResTip5,num_str, stuff_count_str))

    if type_data then
        local text = type_data.text
        if type_data.qua then
            if QualityColorStr[type_data.qua] then
                text = ColorStr(text, QualityColorStrShallow[type_data.qua])
            end
        end
		UH.SetText(self.SelectBtnText,text)
	else
		UH.SetText(self.SelectBtnText,Language.PetStamp.ResTip3)
	end
end

function PetStampResolveNewPanel:SelectAllStuff()
    for i = 1, self.stuff_count do
        local v = self.item_list[i]
        self.Items:SetectItem(i,true)
        self.data:SetSelectItems(v.index,v)
    end
end

function PetStampResolveNewPanel:OnClickLevelSelect()
	local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.type_list)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function PetStampResolveNewPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function PetStampResolveNewPanel:OnClickTypeBtn(data)
    self:OnClickCloseShaiXuan()
    self.Items:CancelSelect()
    self.data:ClearSelectItems()
    self:SelectAllStuff()
	for i,v in pairs(self.item_list) do
		if v.item_id > 0 and v:Type() == ItemType.PetStamp then
			if v:QuaLevel() == data.qua or data.qua == ItemColor.None then
				self.Items:SetectItem(i,true)
				self.data:SetSelectItems(v.index,v)
			end
		end
    end
    self.type_data = data
	self:FlushInfo()
end

function PetStampResolveNewPanel:OnClickItem(value,data)
    if data == nil or data.item_id == 0 then return end 
	if data.index == nil then
		return
    end
	self.data:SetSelectItems(data.index,data)
	--self.data:SetShowItem(data)
	self:FlushInfo()
end

function PetStampResolveNewPanel:OnBreakClick()
	local select_items = self.data:GetSelectItems()
	if TableIsEmpty(select_items) then
		PublicPopupCtrl.Instance:Center(Language.PetStamp.NoneResolve2)
		return
	end
	local break_func = function()
		for i,v in pairs(self.data:GetSelectItems()) do
            if v.index ~= nil then
                if v:Type() == ItemType.PetStamp then
                    PetCtrl.SendPetOp(PET_OP_TYPE.PetStampReslove, v.index)
                else
                    PetCtrl.SendPetOp(PET_OP_TYPE.PetStampExpStuffReslove, v.column_type, v.index, v.num)
                end
			end
		end	
    end
	local exp,stamp_num,stuff_num = self.data:GetAllSelectItemResExp()
	local param_t = {
        content = string.format(Language.PetStamp.ResTip1,stamp_num,stuff_num,exp),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                break_func()
            end 
        },
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end


PetStampResolveInfoPanel = PetStampResolveInfoPanel or DeclareMono("PetStampResolveInfoPanel",UIWFlushPanel)
function PetStampResolveInfoPanel:PetStampResolveInfoPanel()
	self.data = PetStampResolveData.Instance
end

function PetStampResolveInfoPanel:OnEnable()
	self:FlushPanel()
end

function PetStampResolveInfoPanel:FlushPanel()
	local data = self.data:GetShowItem()
	if TableIsEmpty(data) then
		return
	end
	UH.SetText(self.EffectDesc,data:SuitDesc())
	UH.SetText(self.Level,string.format(Language.PetStamp.ResTip4, data:Level()))
	UH.SetText(self.Name,Item.GetQuaName(data.item_id))
    self.Cell:SetData(data)
    local color = data:Color() or 1
    UH.SpriteName(self.Quality,ItemInfoConfig.QualitySp[color])
    local attri_list = data:AttriListInfo()
    self.AttriList:SetData(data:AttriListInfo())
    self.AttriRect:SetSizeWithCurrentAnchors(1, 40 * table.nums(attri_list) + 10)
end

PetStampResolveTypeItem = PetStampResolveTypeItem or DeclareMono("PetStampResolveTypeItem", UIWidgetBaseItem)
function PetStampResolveTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local text = data.text
    if data.qua then
        if QualityColorStr[data.qua] then
            text = ColorStr(text, QualityColorStr[data.qua])
        end
    end
    UH.SetText(self.Text, text)
end

PetStampResolveCell = PetStampResolveCell or DeclareMono("PetStampResolveCell", UIWidgetBaseItem)

function PetStampResolveCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Item:SetData(data)
end

function PetStampResolveCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function PetStampResolveCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end