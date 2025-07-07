
StorageViewStorage = StorageViewStorage or DeclareMono("StorageViewStorage", UIWFlushPanel)

function StorageViewStorage:StorageViewStorage()
    self.flush_events = {EventProtocol.ToEventKey(SCKnapsackInfoAck), EventProtocol.ToEventKey(SCKnapsackExtend)}

    self.data = BagCtrl.Instance:Data()
    self.data.storage_cur_sel_index = 1

    self.data_cares = {
        {data = self.data.cur_storage_select_type,func = self.onFlushSelectTool},
        {data = self.data:GetColumnGrids(ItemColumnType.Storage),func = self.FlushGridStorage,init_call = false},
        {data = self.data.storage_type,func = self.FlushGridSelect,keys = {"is_storage"}},
	}
end

function StorageViewStorage:onFlush()
    local pages = self.data:GetStoragePageList()

    self.TabList:SetData(pages)
    self.TabList:ClickItem(self.data.storage_cur_sel_index)
end

function StorageViewStorage:onFlushSelectTool()
    self.GridStorage:CancelSelect()
    self.SelectTool.SelType = self.data.cur_storage_select_type.val == 0 and 2 or 1
end 

function StorageViewStorage:FlushGridStorage()
    local grids = self.data:GetStorageSGridsWithPage(self.data.storage_cur_sel_index)
    self.GridStorage:SetData(grids)
end

function StorageViewStorage:OnClickPage(data)
    self.data.storage_cur_sel_index = data.index

    self:FlushGridStorage()
    AudioMgr:PlayEffect(AudioType.UI, "bag_col_change")
end

function StorageViewStorage:FlushBottom(data)
    self.data.storage_type.is_storage = true

    local _data = self.GridStorage:GetSelectGroupData()

    if BagData.Instance.cur_storage_select_type.val == 0 
        and not data and self.data.cur_storage_mark ~=nil then 
        BagCtrl.Instance:SendKnapsackOperaReq({
            oper_type = ItemInfoConfig.BagOperType.fetch , 
            param_1 = self.data.cur_storage_mark.column_type, 
            param_2 = self.data.cur_storage_mark.index, 
        })
    end 
end

function StorageViewStorage:FlushGridSelect()
    if not self.data.storage_type.is_storage then 
        self.GridStorage:CancelSelect()
    end 
end

function StorageViewStorage:OnClickFetchBatch()
    local _data = self.GridStorage:GetSelectGroupData()
    if #_data == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Bag.BagStorageFetchEmpty)
        return 
    end 
    
    if not BagData.Instance:TryFindEmptyInBagByType() then 
        PublicPopupCtrl.Instance:Center(Language.Bag.BagFetchWithoutEmpty)
        return 
    end 

    for k,v in pairs(_data) do 
	    BagCtrl.Instance:SendKnapsackOperaReq{
            oper_type = ItemInfoConfig.BagOperType.fetch, 
            param_1 = v.column_type, 
            param_2 = v.index, 
        }
    end 
end

function StorageViewStorage:OnClickTidy()

	local param_t = {
		oper_type = BAG_OPERA_TYPE.OPERA_TYPE_INORDER,
		param_1 = ItemColumnType.Storage
	}
    BagCtrl.Instance:SendKnapsackOperaReq(param_t)
    
    AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")

    if self.tidy_countdown_handle ~= nil then 
		TimeHelper:CancelTimer(self.tidy_countdown_handle)
		self.tidy_countdown_handle = nil
	end 
	local end_time = 5
	self.BtnTidyInteractor.Interactable = false
	self.tidy_countdown_handle = TimeHelper:AddCountDownTT(function()
        UH.SetText(self.BtnTidyLabel,Format(Language.Bag.BagTiming,end_time))
        end_time = end_time - 1
	end,
    function()
        UH.SetText(self.BtnTidyLabel,Language.Bag.BagTimed)
        self.BtnTidyInteractor.Interactable = true
	end,5,1,true)
end

function StorageViewStorage:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.tidy_countdown_handle)
    BagData.Instance.cur_storage_mark = nil
end

StorageViewStoragePageItem = DeclareMono("StorageViewStoragePageItem", UIWidgetBaseItem)

function StorageViewStoragePageItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = BagCtrl.Instance:Data():GetColumnInfo(ItemColumnType.Storage)
    local cur_unlock_page = (info.extend_times or 0) + 2

    self.Mask:SetActive(cur_unlock_page <= data.index)
    
    local icon_s = StorageConfig.StorageTabIconS
    if cur_unlock_page < data.index then
        self.Icon.SpriteName = icon_s.lock
    elseif cur_unlock_page == data.index then
        self.Icon.SpriteName = icon_s.add
    else
        self.Icon.SpriteName = icon_s.normal
    end
end

function StorageViewStoragePageItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function StorageViewStoragePageItem:OnClickMask()
    local info = BagCtrl.Instance:Data():GetColumnInfo(ItemColumnType.Storage)
    local cur_unlock_page = (info.extend_times or 0) + 2
    if cur_unlock_page == self.data.index then
        AudioMgr:PlayEffect(AudioType.UI, "bag_unlock_pop")
        local lock_tips = Language.Storage.LockTips
        PublicPopupCtrl.Instance:DialogTips{content = string.format(lock_tips.tips, info.expand_price or 0),
            cancel = {
                func = BindTool.Bind(PublicPopupCtrl.Instance.CloseDialogTips, PublicPopupCtrl.Instance)
            },
            confirm = {
                name = lock_tips.confirm_name,
                func = function()
                    BagCtrl.Instance:SendKnapsackOperaReq{oper_type = StorageConfig.OperType.extend, param_1 = ItemColumnType.Storage, param_2 = StorageConfig.STORAGE_PAGE_PER}
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    AudioMgr:PlayEffect(AudioType.UI, "bag_unlock_eff")
                end
            }
        }
    end
end


---------------批量 仓库单元----------------
StorageBatchItemCell = DeclareMono("StorageBatchItemCell", UIWidgetBaseItem)

function StorageBatchItemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- local _item = Item.New(data)
    self.Item:SetData(data)
end 

function StorageBatchItemCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function StorageBatchItemCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)

    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
        BagData.Instance.cur_storage_mark = nil
    end

    if self.data.item_id > 0 then 
        if selected then 
            self.Selected:SetActive(selected and BagData.Instance.cur_storage_select_type.val == 1)
            if BagData.Instance.cur_storage_select_type.val == 0 then
                self.Double:SetActive(selected)
                BagData.Instance.cur_storage_mark = self.data
            end 
        elseif self.Double.activeSelf then 
            self.Double:SetActive(false)
        elseif self.Selected.activeSelf then 
            self.Selected:SetActive(false)
        end 
    else 
        self.Double:SetActive(false)
        self.Selected:SetActive(false)
    end 
end