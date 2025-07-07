--命格分解界面
PartnerEquiqBreakView = PartnerEquiqBreakView or DeclareView("PartnerEquiqBreakView","mystery_store/PartnerEquiqBreakView",Mod.PartnerEquiqBreak.Main)

function PartnerEquiqBreakView:PartnerEquiqBreakView()
	self.Board:SetData(self:GetType(), Language.MysteryStore.BreakTitle, Vector2.New(970, 592))
end

function PartnerEquiqBreakView:LoadCallback()
	-- body
end

function PartnerEquiqBreakView:CloseCallback()
	-- body
end

PartnerEquiqBreakPanel = PartnerEquiqBreakPanel or DeclareMono("PartnerEquiqBreakPanel",UIWFlushPanel)
function PartnerEquiqBreakPanel:PartnerEquiqBreakPanel()
	self.data = PartnerEquiqBreakData.Instance
	self.bag_handle = BagData.Instance:Care(BindTool.Bind(self.FlushPanel, self))
end

function PartnerEquiqBreakPanel:Awake()
	UIWFlushPanel.Awake(self)
	UH.SetText(self.EmptyText,Language.MysteryStore.BreakTip)
	self:FlushPanel()
end

function PartnerEquiqBreakPanel:OnDestroy()
	BagData.Instance:UnCare(self.bag_handle)
end

function PartnerEquiqBreakPanel:FlushPanel()
	self.data:ClearSelectItems()
	self.data:SetShowItem({})
	self.item_list = BagData.Instance:GetPartnerEquipList()
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

function PartnerEquiqBreakPanel:FlushInfo(level_data)
	local end_select_item = self.data:GetShowItem()
	self.InfoObj:SetActive(end_select_item ~= nil and next(end_select_item) ~= nil)
	if self.InfoObj.activeSelf then
		self.InfoMono:FlushPanel()
	end
	local num,count = self.data:GetAllSelectItemBreakNum()
	UH.SetText(self.BreakNum,num)
	UH.SetText(self.HasNum,RoleData.Instance:GetCurrency(CurrencyType.LuckCoin))

	if level_data then
		UH.SetText(self.SelectBtnText,level_data.name)
	else
		UH.SetText(self.SelectBtnText,Language.MysteryStore.SelectBtnText)
	end
end

function PartnerEquiqBreakPanel:OnClickLevelSelect()
	local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data:GetEquiqBreakLevelList())
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function PartnerEquiqBreakPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function PartnerEquiqBreakPanel:OnClickLevelBtn(data)
	self:OnClickCloseShaiXuan()
	self.Items:CancelSelect()
	self.data:ClearSelectItems()
	for i,v in pairs(self.item_list) do
		if v.item_id > 0 then
			if v.limit_level >= data.start_level and v.limit_level <= data.end_level then
				self.Items:SetectItem(i,true)
				self.data:SetSelectItems(v.index,v)
			end
		end
	end
	self:FlushInfo(data)
end

function PartnerEquiqBreakPanel:OnClickItem(data)
	if data.index == nil then
		return
	end
	self.data:SetSelectItems(data.index,data)
	self.data:SetShowItem(data)
	self:FlushInfo()
end

function PartnerEquiqBreakPanel:OnBreakClick()
	local select_items = self.data:GetSelectItems()
	if select_items == nil or next(select_items) == nil then
		PublicPopupCtrl.Instance:Center(Language.MysteryStore.BreakAlert)
		return
	end
	local break_func = function()
		for i,v in pairs(self.data:GetSelectItems()) do
			if v.index ~= nil then
				PartnerCtrl.Instance:SendPartnerEqOperatorReq(PartnerConfig.PEOperType.equip_break,0,v.index)
			end
		end	
	end
	local num,count = self.data:GetAllSelectItemBreakNum()
	local param_t = {
        content = string.format(Language.MysteryStore.BreakAlert2,count,num),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                break_func()
            end 
        },
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

PartnerEquiqSelectInfoPanel = PartnerEquiqSelectInfoPanel or DeclareMono("PartnerEquiqSelectInfoPanel",UIWFlushPanel)
function PartnerEquiqSelectInfoPanel:PartnerEquiqSelectInfoPanel()
	self.data = PartnerEquiqBreakData.Instance
end

function PartnerEquiqSelectInfoPanel:OnEnable()
	self:FlushPanel()
end

function PartnerEquiqSelectInfoPanel:FlushPanel()
	local data = self.data:GetShowItem()
	if data == nil or next(data) == nil then
		return
	end
	for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
		local attr = data.attr_list[i]
		self.AttrList[i]:SetObjActive(attr.attr_value ~= 0)
		if attr.attr_value ~= 0 then
			local attr_str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, Language.Common.AttrList[attr.attr_type], DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
			UH.SetText(self.AttrList[i], attr_str)
		end
	end
	UH.SetText(self.Desc,data:Desc())
	UH.SetText(self.Level,data:LimitLv()..Language.Common.Level)
	UH.SetText(self.Name,Item.GetQuaName(data.item_id))
	local is_ac,suit_info = data:ActiveSuitInfo()
	UH.SetText(self.SpecialDesc,suit_info.content)
    self.Cell:SetData(data)
    local color = data:Color() or 1
    UH.SpriteName(self.Quality,ItemInfoConfig.QualitySp[color])
end

PartnerEquiqBreakLevelBtn = PartnerEquiqBreakLevelBtn or DeclareMono("PartnerEquiqBreakLevelBtn", UIWidgetBaseItem)

function PartnerEquiqBreakLevelBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.name
end

PartnerEquiqBreakCell = DeclareMono("PartnerEquiqBreakCell", UIWidgetBaseItem)

function PartnerEquiqBreakCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Item:SetData(data)
end

function PartnerEquiqBreakCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function PartnerEquiqBreakCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function PartnerEquiqBreakCell:OnClickItem()
	if self.data == nil or self.data.item_id == 0 then return end 
	if self.data:LimitLv() < PartnerEquiqBreakData.Instance:GetCanBreakLevel() then
		self.Selector.isOn = false
		PublicPopupCtrl.Instance:Center(Language.MysteryStore.BreakLimitTip)
		return
	end
    self.Mono:OnClickItem(self.data)
end
