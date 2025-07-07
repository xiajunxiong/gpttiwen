MarbasLeftEquip = MarbasLeftEquip or DeclareMono("MarbasLeftEquip", UIWFlushPanel)
function MarbasLeftEquip:MarbasLeftEquip()
    self.Data = MarbasData.Instance
    self.data = self.Data.left_equip_info
    self.data_cares = {
        {data = self.data, func = self.FlushAll, keys = {"mod_id"}, init_call = false},
        {data = self.data, func = self.FlushEquipOnly, keys = {"flush_flag"}, init_call = false},
        {data = self.Data.equip_list, func = self.FlushEquip, init_call = false},
        {data = self.Data.refine_remind, func = self.FlushRemind, init_call = false},
        {data = self.Data.bathe_remind, func = self.FlushRemind, init_call = false},
        {data = self.Data.refine_info, func = self.FlushRemind, keys = {"info"}, init_call = false},
        {data = self.Data.bathe_info, func = self.FlushRemind, keys = {"info"}, init_call = false},
    }
end

function MarbasLeftEquip:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function MarbasLeftEquip:FlushAll()
    self:FlushEquip()
    self:FlushRemind()
end

function MarbasLeftEquip:FlushEquip()
    for i = 1, self.EquipList:Length() do
        self.EquipList[i]:SetData(self.Data.equip_list[i - 1] or {})
    end
    self:FlushSelect()
end

function MarbasLeftEquip:FlushEquipOnly()
    for i = 1, self.EquipList:Length() do
        self.EquipList[i]:SetData(self.Data.equip_list[i - 1] or {})
    end
end

-- 优先选中不为空
function MarbasLeftEquip:FlushSelect()
    -- 不延时一帧会报错
    TimeHelper:CancelTimer(self.ht_delay)
    self.ht_delay = TimeHelper:AddDelayFrameTimer(function()
        if TableIsEmpty(self.Data.equip_list[self.data.part]) then
            for i = 1, self.EquipList:Length() do
                if not TableIsEmpty(self.Data.equip_list[i - 1]) then
                    self.data.part = i - 1
                    break
                end
                if i == self.EquipList:Length() then
                    self.data.part = 0
                end
            end
        end
        self.EquipList[self.data.part + 1]:MySelect()
    end, 1)
end

function MarbasLeftEquip:FlushRemind()
    for i = 1, self.EquipList:Length() do
        self.EquipList[i]:FlushRemind()
    end
end

function MarbasLeftEquip:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht_delay)
end

--------------MarbasLeftEquipItem--------------
MarbasLeftEquipItem = MarbasLeftEquipItem or DeclareMono("MarbasLeftEquipItem", UIWidgetBaseItem)
function MarbasLeftEquipItem:MarbasLeftEquipItem()
    UH.SpriteName(self.Icon, MarbasConfig.equip_type_sp[self.Part])
    UH.SetText(self.TxtPart, Language.Common.Marbas[self.Part])
    self.Block.enabled = true
end

function MarbasLeftEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetObjActive(not TableIsEmpty(data))
    self.Cell:SetData(data or {})
    self.ObjLevelShow:SetActive(not TableIsEmpty(data))
    if not TableIsEmpty(data) then 
        UH.SetText(self.TxtLevelShow, Language.Marbas.ItemInfo.LevelShow..data:LevelShow())
    end 
    self:FlushRemind()
end

function MarbasLeftEquipItem:FlushRemind()
    self.Remind:SetNum(MarbasData.Instance:LeftEquipItemRemind(self.Part))
end

function MarbasLeftEquipItem:OnSelect(value)
    if value then
        MarbasData.Instance.left_equip_info.part = self.Part
        MarbasData.Instance.left_equip_info:SetDirty("part")
    end
    self.Block.enabled = not value
end

function MarbasLeftEquipItem:MySelect()
    self.Selector:Trigger()
end