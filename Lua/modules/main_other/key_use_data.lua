KeyUseData = KeyUseData or BaseClass()
function KeyUseData:__init()
    if KeyUseData.Instance ~= nil then
        Debuger.LogError("[KeyUseData] attempt to create singleton twice!")
        return
    end
    KeyUseData.Instance = self

    self.item_list = {}
    self.key_data = SmartData.New({val = false})
    self.btn_name = nil         -- 使用按钮名字
    self.open_time = nil        -- 弹窗打开时间
end

function KeyUseData:AddItem(param_t)
    --宝图特殊处理
    if Item.GetBigType(param_t.item_id) == ITEM_BIG_TYPE.TREASURE_MAP then
        if self:IsNotSameByIndex(param_t.index) then
            table.insert(self.item_list,param_t)
        end
        return
    end
    if self:IsNotSame(param_t.item_id) then
        table.insert(self.item_list,param_t)
    end
end

function KeyUseData:RemoveItem()
    if #self.item_list ~= 0 then
        table.remove(self.item_list,1)
        self.key_data.val = not self.key_data.val
    else
        MainOtherCtrl.Instance:CloseKeyUse()
    end
end

function KeyUseData:GetItem()
    return self.item_list[1]
end

--是否插入同一个
function KeyUseData:IsNotSame(item_id)
    if #self.item_list == 0 then
        return true
    end
    for k,v in pairs(self.item_list or {}) do
        if v.item_id == item_id then
            return false
        end
    end
    return true
end

function KeyUseData:IsNotSameByIndex(index)
    if #self.item_list == 0 then
        return true
    end
    local item = self.item_list[#self.item_list]
    if item and item.index then
        return item.index ~= index
    end
    return false
end

function KeyUseData:ClearData()
    self.key_data:Clear()
    self.item_list = {}
end

function KeyUseData:SetBtnName(btn_name)
    self.btn_name = btn_name
end

function KeyUseData:SetOpenTime(open_time)
    self.open_time = open_time
end

ChooseGiftUseData = ChooseGiftUseData or BaseClass()
function ChooseGiftUseData:__init()
    if ChooseGiftUseData.Instance ~= nil then
        Debuger.LogError("[ChooseGiftUseData] attempt to create singleton twice!")
        return
    end
    ChooseGiftUseData.Instance = self

    self.preview_gift = SmartData.New({item = nil})
end

function ChooseGiftUseData:SetPreviewGift(item)
    self.preview_gift.item = item
end

function ChooseGiftUseData:GetPreviewGift()
    return self.preview_gift.item
end