HeartSkillChangeView = HeartSkillChangeView or DeclareView("HeartSkillChangeView", "role/role_heart_skill_change",Mod.HeartSkillChang.Main)

function HeartSkillChangeView:HeartSkillChangeView()
    self.data = HeartSkillData.Instance
    self.Board:SetData(nil,Language.HeartSkill.SelectTitle,Vector2.New(686,534))
    self.Board:SetCloseFunc(function()self.Parent:SetActive(false)end)
    self.handle = BagData.Instance:Care(BindTool.Bind(self.OnFlushPanel,self))
end

function HeartSkillChangeView:OnClickClose()
    ViewMgr:CloseView(HeartSkillChangeView)
end

function HeartSkillChangeView:CloseCallback()
    BagData.Instance:UnCare(self.handle)
    TimeHelper:CancelTimer(self.ShowTimer0)
    TimeHelper:CancelTimer(self.ShowTimer1)
    TimeHelper:CancelTimer(self.ShowTimer2)
end

function HeartSkillChangeView:OnFlushPanel()
    self:CancelView()
end

function HeartSkillChangeView:LoadCallback(param_t)
    local item = param_t.item or {}
    self.ItemList[1]:SetData(item)
    self:FlushTargetView(item,param_t.config)
end

--设置目标
function HeartSkillChangeView:FlushTargetView(item,config)
    local color = Item.GetColor(item.item_id)
    local data = self.data:GetTargetData(color) or config
    if data ~= nil then
        self.target_item = Item.Init(data.displace_target)
        self.target_color = data.heart_skill_color
        UH.SetIcon(self.Icon,Item.GetIconId(data.displace_target),ICON_TYPE.ITEM)
        UH.SetText(self.Name,Item.GetQuaName(data.displace_target))
        self.Consume:SetData({item_id = data.consume_item})
        self.consume_item = data.consume_item
    end
    self.Icon:SetObjActive(data ~= nil)
    self.Consume:SetObjActive(data ~= nil)
end

--重置
function HeartSkillChangeView:CancelView()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData({})
        self.ItemList[i]:SetActive(false)
    end
    self.target_item = nil
    self.target_color = nil
    self.Icon:SetObjActive(false)
    self.IsPlayAnim = nil
    self.Consume:SetObjActive(false)
end

--添加按钮
function HeartSkillChangeView:OnClickItem(index)
    self.Parent:SetActive(true)
    self.List:CancelSelect()
    local list = self.data:GetBagSkillList(self:GetSelectData())
    self.List:SetData(list)
    self.select_index = index
    self.NotItemTip:SetActive(#list == 0)
end

function HeartSkillChangeView:OnClickSelect(cell,data)
    local item_data = self.ItemList[self.select_index]:GetData()
    if item_data and item_data.item_id then
        UH.SetText(self.BtnName,Language.HeartSkill.ChangeBtnName[1])
    else
        UH.SetText(self.BtnName,Language.HeartSkill.ChangeBtnName[2])
    end
    self.select_data = data
end

--确定选中
function HeartSkillChangeView:OnClickSelectSure()
    if self.select_data and self.select_index then
        self.ItemList[self.select_index]:SetData(self.select_data)
        if self.select_index == 1 then
            self:FlushTargetView(self.select_data)
        end
    end
    self.Parent:SetActive(false)
end

function HeartSkillChangeView:GetSelectData()
    local list,num = {},0
    for i=1,self.ItemList:Length() do
        local data = self.ItemList[i]:GetData()
        if data and data.item_id then
            num = num + 1
        end
        list[i] = data or {}
    end
    return list,num
end

--快捷填充
function HeartSkillChangeView:OnClickFull()
    local list,num = self:GetSelectData()
    if self.target_color then
        local index = 1
        local info_data = self.data:GetBagSkillFullList(list,#list - num,self.target_color)
        if #info_data == 0 then
            PublicPopupCtrl.Instance:Center(Language.HeartSkill.NotHasFullTip)
            return
        end
        for i=1,self.ItemList:Length() do
            if list[i].item_id == nil and info_data[index]then
                self.ItemList[i]:SetData(info_data[index])
                index = index + 1
            end
            if i == 1 and info_data[i] then
                self:FlushTargetView(info_data[i])
            end
        end
    else
        local info_data = self.data:GetBagSkillFullList(list,#list - num)
        if #info_data == 0 then
            PublicPopupCtrl.Instance:Center(Language.HeartSkill.NotHasFullTip)
            return
        end
        for i=1,self.ItemList:Length() do
            if info_data[i] then
                self.ItemList[i]:SetData(info_data[i])
            end
            if i == 1 and info_data[i] then
                self:FlushTargetView(info_data[i])
            end
        end
    end
end

--点击合成
function HeartSkillChangeView:OnClickEnter()
    if self.IsPlayAnim then
        return
    end
    local list,num = self:GetSelectData()
    if num < self.ItemList:Length() then
        PublicPopupCtrl.Instance:Center(Language.HeartSkill.PopupTip)
        return
    end
    for k,v in pairs(list) do
        if self.target_color and v.item_id and self.target_color ~= Item.GetColor(v.item_id) then
            PublicPopupCtrl.Instance:Center(Language.HeartSkill.PopupTip)
            return
        end
    end
    if Item.GetNum(self.consume_item) <= 0 then
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = self.consume_item})})
        PublicPopupCtrl.Instance:Center(ErrorText[24])
        return
    end
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetEffect()
    end
    self.ShowTimer0 = Invoke(function()
        for i=1,self.ItemList:Length() do
            self.ItemList[i]:SetChange()
        end
    end,1)
    self.ShowTimer1 = Invoke(function()
        for i=1,self.ItemList:Length() do
            self.ItemList[i]:CancelEffect()
        end
        self.EffectTool:Play(4161039)
    end,2)
    self.ShowTimer2 = Invoke(function()
        HeartSkillCtrl.Instance:SendDisplaceReq(list)
    end,3.5)
    self.IsPlayAnim = true
end

function HeartSkillChangeView:OnClickCell()
    CellClicks.BagGridNoButton({GetData = function()
        return self.target_item
    end})
end

----------------------------HeartSkillChangeCall----------------------------
HeartSkillChangeCall = HeartSkillChangeCall or DeclareMono("HeartSkillChangeCall", UIWidgetBaseItem)

function HeartSkillChangeCall:SetData(data)
    if data.item_id then
        self.Call:SetData(Item.Init(data.item_id))
    end
    UIWidgetBaseItem.SetData(self, data)
    self:SetActive(data.item_id ~= nil)
end

function HeartSkillChangeCall:SetActive(active)
    self.Call:SetObjActive(active)
    self.Image:SetActive(not active)
    self.Default:SetActive(active)
end

function HeartSkillChangeCall:SetEffect()
    self.Effect2:SetActive(true)
end

function HeartSkillChangeCall:SetChange(active)
    self.Default:SetActive(false)
    self.Effect1:SetActive(true)
end

function HeartSkillChangeCall:CancelEffect()
    self.Default:SetActive(false)
    self.Effect1:SetActive(false)
    self.Effect2:SetActive(false)
end

----------------------------HeartSkillChangeItem----------------------------
HeartSkillChangeItem = HeartSkillChangeItem or DeclareMono("HeartSkillChangeItem", UIWidgetBaseItem)

function HeartSkillChangeItem:SetData(data)
    local item = Item.New(data)
    self.Call:SetData(item)
    UH.SetText(self.Name,item:QuaName())
    UIWidgetBaseItem.SetData(self, data)
end

function HeartSkillChangeItem:Click()
	self.ToggleClick:Trigger()
end

----------------------------HeartSkillChangeConsume----------------------------
HeartSkillChangeConsume = HeartSkillChangeConsume or DeclareMono("HeartSkillChangeConsume", UIWidgetBaseItem)

function HeartSkillChangeConsume:SetData(data)
    data.item = Item.New(data)
    UH.SetIcon(self.Icon,data.item:IconId(), ICON_TYPE.ITEM)
    UH.SetText(self.Num,DataHelper.ConsumeNum(data.item:Num(),1,true))
    UIWidgetBaseItem.SetData(self, data)
end

function HeartSkillChangeConsume:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end