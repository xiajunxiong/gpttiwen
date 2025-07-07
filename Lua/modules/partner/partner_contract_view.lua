
PartnerContractView = PartnerContractView or DeclareView("PartnerContractView", PartnerConfig.ResPath .. "partner_contract_show")

function PartnerContractView:PartnerContractView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerContractView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.ContractItems, Vector2.New(776, 536))
end

PartnerContractViewF = PartnerContractViewF or DeclareMono("PartnerContractViewF", UIWFlushPanel)

function PartnerContractViewF:PartnerContractViewF() 
    self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Contract
	self.data.partner_pact_show_info.sel_item:Set{}

    self.data_cares = {
        {data = self.data.partner_pact_items, func = self.FlushItems},
        {data = self.data.partner_pact_show_info, func = self.FlushBtnShow, keys = {"sel_item"}, init_call = false},
        {data = self.data.partner_pact_show_info, func = self.FlushItems, keys = {"sort_type"}, init_call = false},
        {data = self.data.item_flush, func = self.FlushItems, init_call = false},
    }
end

function PartnerContractViewF:Awake()
	UIWFlushPanel.Awake(self)

	for i = 1, self.SortTypes:Length() do
		self.SortTypes[i].text = self.language.SortTypes[i]
	end
end

function PartnerContractViewF:FlushItems()
	local list = self.data:GetPartnerPactItems()
	local ppsi = self.data.partner_pact_show_info
	local is_empty = 0 == #list
	self.ShowList:SetDataList(list)
	self.EmptyObj:SetActive(is_empty)
	self.BtnEquipInter.Interactable = not is_empty
	self.SortName.text = self.language.SortTypes[ppsi.sort_type]
	self.BtnEquip.text = self.language.Equip
end

function PartnerContractViewF:FlushBtnShow()
	local sel_item = self.data.partner_pact_show_info.sel_item
	local value1 = sel_item.partner and sel_item.partner == self.data.cur_sel_partner
	-- local value2 = sel_item.partner and sel_item.partner ~= self.data.cur_sel_partner
	self.BtnEquipInter:SetObjActive(not value1)
	self.BtnOffObj:SetActive(value1)
	-- self.BtnEquip.text = value2 and self.language.Equiped or self.language.Equip
	local num = self.data:GetPartnerPactItemNumById(sel_item.item:PartnerId())
	self.NumTips.text = string.format(self.language.NumTips, sel_item.item:PartnerName(), num)
end

function PartnerContractViewF:OnClickEquip()
	local ppsi = self.data.partner_pact_show_info
	local sel_item = ppsi.sel_item
	local num = self.data:GetPartnerPactItemNumById(sel_item.item:PartnerId())
	if self.data.cur_sel_partner == sel_item.item:PartnerId() then
		PublicPopupCtrl.Instance:Center(self.language.NotPactSelfTips)
		return
	end
	if sel_item.item:CheckLock() then return end
	if sel_item.partner then
		PublicPopupCtrl.Instance:DialogTips{
			content = self.language.ReplaceTips,
            confirm = {
				func = function ()
                    ViewMgr:CloseView(DialogTipsView)
                    PartnerCtrl.Instance:SendPartnerContractReplace(self.data.cur_sel_partner, ppsi.sel_pos, sel_item.partner, sel_item.pos)
                end
            }
        }
	else
		if -1 == self.data.cur_sel_partner then return end
		local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
		if nil == data then return end
		local pact_item = data.pact_items[ppsi.sel_pos + 1] 
		if pact_item.item_id > 0 and pact_item.item:PartnerId() == sel_item.item:PartnerId() then
			PartnerCtrl.Instance:SendPartnerContractPutOn(self.data.cur_sel_partner, ppsi.sel_pos, sel_item.save_index)	
		else
			if num >= 2 then
				PublicPopupCtrl.Instance:Center(self.language.NumMaxTips)
				return
			end
			PartnerCtrl.Instance:SendPartnerContractPutOn(self.data.cur_sel_partner, ppsi.sel_pos, sel_item.save_index)	
		end
		ViewMgr:CloseView(PartnerContractView)
	end
end

function PartnerContractViewF:OnClickPutOff()
	local ppsi = self.data.partner_pact_show_info
	PartnerCtrl.Instance:SendPartnerContractPutOff(self.data.cur_sel_partner, ppsi.sel_pos)
end

function PartnerContractViewF:OnOpenSort()
	self.SortObj:SetActive(true)
end

function PartnerContractViewF:OnCloseSort()
	self.SortObj:SetActive(false)
end

function PartnerContractViewF:OnClickSort(sort_type)
	self.SortObj:SetActive(false)
	self.data.partner_pact_show_info.sort_type = sort_type
end

function PartnerContractViewF:OnClickStore()
    ViewMgr:OpenView(TreasureHuntView,{auto_select = 5})
end

PartnerContractViewItem = DeclareMono("PartnerContractViewItem", UIWidgetBaseItem)

function PartnerContractViewItem:PartnerContractViewItem()
	self.partner_data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Contract
end

function PartnerContractViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	local item = data.item
	local partner_id = item:PartnerId()
	self.Cell:SetData(item)
	self.Name.text = item:Name()
	self.DescShow.text = string.format(self.language.DescShow, item:PartnerName(), item:AttrUp())
	self.NextShow.text = item:MaxAttrUp() and self.language.MaxShow or string.format(self.language.NextShow, item:AttrUpNext())
	self.CapShow.text = string.format(self.language.CapShow, item:ScoreAdd(self.partner_data.cur_sel_partner))

	self.Contract1:SetActive(data.partner and data.partner == self.partner_data.cur_sel_partner)
	self.Contract2:SetActive(data.partner and data.partner ~= self.partner_data.cur_sel_partner)
	if data.partner and data.partner ~= self.partner_data.cur_sel_partner then
		self.Contract2Name.text = string.format(self.language.Contract2, self.partner_data.partner_list[data.partner].info.name)
	end
end

function PartnerContractViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function PartnerContractViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
		self.partner_data.partner_pact_show_info.sel_item:Set(self.data)
    end
end

PartnerContractViewSortItem = DeclareMono("PartnerContractViewSortItem", UIWidgetBaseItem)

function PartnerContractViewSortItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
end