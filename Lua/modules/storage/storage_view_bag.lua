
StorageViewBag = StorageViewBag or DeclareMono("StorageViewBag", UIWFlushPanel)

function StorageViewBag:StorageViewBag()
    self.data = BagCtrl.Instance:Data()
    self.data.storage_cur_sel_type = 1
    self.data.cur_storage_select_type.val = 0

    local list = self.data:GetStorageBagTypeList()
    self:OnClickItem(list[1])

    self.data_cares = {
        {data = self.data.cur_storage_select_type,func = self.onFlushSelectTool},
        {data = self.data.item_grid,func = self.onFlush,init_call = false},
        {data = self.data.storage_type,func = self.FlushGridSelect,keys = {"is_storage"}},
	}
end

function StorageViewBag:onFlush()
    local list = self.data:GetStorageBagTypeList()
    self.TypeList:SetData(list)

    local grids = self.data:GetStorageBGridsWithType(self.data.storage_cur_sel_type)
    self.GridBag:SetData(grids)
end

function StorageViewBag:onFlushSelectTool()
    self.GridBag:CancelSelect()
    self.SelectTool.SelType = self.data.cur_storage_select_type.val == 0 and 2 or 1
end 

function StorageViewBag:OnClickItem(data)
    self.data.storage_cur_sel_type = data.type
    self.Name.text = data.name
    if self.Toggle then
        if self.Toggle.isOn then 
            AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")
        end 

        self.Toggle.isOn = false
    end
    ViewMgr:FlushView(StorageView, StorageViewBag:GetTypeName())
end

function StorageViewBag:Useless()
end

function StorageViewBag:FlushBottom(data)
    self.data.storage_type.is_storage = false

    local _data = self.GridBag:GetSelectGroupData()

    if BagData.Instance.cur_storage_select_type.val == 0 
        and not data and self.data.cur_storage_mark ~=nil then 
        BagCtrl.Instance:SendKnapsackOperaReq({                     
            oper_type = ItemInfoConfig.BagOperType.store, 
            param_1 = self.data.cur_storage_mark.column_type, 
            param_2 = self.data.cur_storage_mark.index,
            param_3 = BagData.Instance.storage_cur_sel_index - 1
        })
    end 
end

function StorageViewBag:FlushGridSelect()
    if self.data.storage_type.is_storage then 
        self.GridBag:CancelSelect()
    end 
end

function StorageViewBag:OnClickStoreBatch()
    local _data = self.GridBag:GetSelectGroupData()
    if #_data == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Bag.BagStorageStoreEmpty)
        return 
    end 

    local page = self.data.storage_cur_sel_index or 1

    for k,v in pairs(_data) do 
        if v.index ~= nil then 
            BagCtrl.Instance:SendKnapsackOperaReq{
                oper_type = ItemInfoConfig.BagOperType.store, 
                param_1 = v.column_type, 
                param_2 = v.index,
                param_3 = page - 1
            }
        end
    end 
end 

function StorageViewBag:OnClickChangeSelect()
    self.Gou:SetActive(not self.Gou.activeSelf)
    
    self.data.cur_storage_select_type.val = self.Gou.activeSelf and 1 or 0
end


StorageViewBagTypeItem = DeclareMono("StorageViewBagTypeItem", UIWidgetBaseItem)

function StorageViewBagTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.name
end

function StorageViewBagTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end