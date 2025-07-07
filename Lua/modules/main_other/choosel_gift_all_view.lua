ChooseGiftAllView = ChooseGiftAllView or DeclareView("ChooseGiftAllView", "main_other/choosel_gift_all")

VIEW_DECLARE_MASK(ChooseGiftAllView, ViewMask.BgBlock)

function ChooseGiftAllView:ChooseGiftAllView()
    self.care_handle = MainOtherData.Instance.choosel_all_data:Care(BindTool.Bind(self.FlusnItemView,self))
end

function ChooseGiftAllView:CloseCallback()
	MainOtherData.Instance.choosel_all_data:Uncare(self.care_handle)
	self.care_handle = nil
end

function ChooseGiftAllView:OnClickClose()
    ViewMgr:CloseView(ChooseGiftAllView)
end

function ChooseGiftAllView:LoadCallback(param_t)
    self.param_t = param_t or {}
    local TittleTip = Language.MainOther.TittleTip
    self.item_list,self.config = self:GetItemList(self.param_t.item_id,self.param_t.num)
    self.CallList:SetData(self.item_list)
    UH.SetText(self.LimitNum,Format(Language.MainOther.ChooseGiftLimit,self.param_t.num * self.config.select_num))
    self:FlusnItemView()
end

function ChooseGiftAllView:FlusnItemView()
    self.make_num = self:GetSelectedNum()
    UH.SetText(self.SelectNum,Format(Language.MainOther.ChooseGiftSelect,self.make_num))
end

function ChooseGiftAllView:GetSelectedNum()
    local num = 0
    for k,v in pairs(self.CallList:GetSelectGroupData() or {}) do
        if v.make_num ~= nil then
            num = num + v.make_num
        end
    end
    return num
end

--点击确认
function ChooseGiftAllView:OnClickConfirm()
    local select_info,select_num = self:GetSelectInfo()
    if select_num == 0 then
        PublicPopupCtrl.Instance:Center(Language.MainOther.ChooseGiftTip1)
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Language.MainOther.ChooseGiftTip2,function()
        for k,v in pairs(select_info) do
            self:SendUseItemGift({v},v.make_num)
        end
        ViewMgr:CloseView(ChooseGiftAllView)
    end)
end

--道具使用
function ChooseGiftAllView:SendUseItemGift(select_info,select_num)
    if self.config.is_expense == true then
        local param_t = TableCopy(self.param_t)
        param_t.param1 = select_info[1].index
        param_t.num = select_num or 1
        BagCtrl.Instance:SendUseItem(param_t)
    else 
        BagCtrl.Instance:SendUseOptionalGift(self.param_t.column_type,
        self.param_t.index,select_info,select_num)
    end
end

--获取选中的道具
function ChooseGiftAllView:GetSelectInfo()
    local select_info = {}
    local select_list = self.CallList:GetSelectGroupData() or {}
    for k,v in pairs(select_list) do
        table.insert(select_info,{
            index = v.vo.mark_index or v.index,num = v.num or 1,
            item_id = v.item_id,
            make_num = v.make_num,
        })
    end
    return select_info,#select_list
end

function ChooseGiftAllView:GetItemList(item_id,num)
    local item_list,config = BagData.Instance:GiftItemList(item_id)
    for i,v in ipairs(item_list) do
        v.index = i - 1
        v.limit_num = num
        v.func = function()
            return self:GetSelectedNum()
        end
    end
    return item_list,config
end
----------------------------ChooseGiftAllItem----------------------------
ChooseGiftAllItem = ChooseGiftAllItem or DeclareMono("ChooseGiftAllItem", UIWidgetBaseItem)

function ChooseGiftAllItem:SetData(data)
    self.Call:SetData(data)
    self.is_set = true
    UH.SetText(self.Name,data:Name())
    UH.RenameGame(self,[[Item]].. data.index)
    self.UseFlag:SetActive(FabaoData.GetUseFlag(data))
    UIWidgetBaseItem.SetData(self, data)
    if data.make_num == nil then
        self.data.make_num = 0
        self.is_flush = true
    else
        self.is_flush = false
    end
    self:FlushNumView()
    if not self.is_input_listen then
        self.InputText.onValueChanged:AddListener(BindTool.Bind(self.OnInputClick, self))
        self.is_input_listen = true
    end
    self.is_set = false

    if self.recommd ~= nil then 
    --     if data:Type() == ItemType.Gem then 
        --     self.recommd:SetActive(WorkshopData.Instance:GetProfRecommendAllPart(data.item_id))
        -- else 
            self.recommd:SetActive(false)
    --     end 
    end 
end

function ChooseGiftAllItem:OnInputClick()
    if not self.is_flush then
        self.is_flush = true
        return
    end
    local num = tonumber(self.InputText.text) or 0
    if num <= 0 then
        MainOtherData.Instance:CheckChooselAllData()
        if not self.is_set then
            self:SetSelected(false)
        end
        self.data.make_num = 0
        self:FlushNumView()
        return
    end
    self:SetSelected(true)
    self.data.make_num = num
    local sel_num = self.data.func()
    if sel_num > self.data.limit_num then
        num = self.data.make_num - (sel_num - self.data.limit_num)
        if num <= 0 then
            self:SetSelected(false)
        else
            PublicPopupCtrl.Instance:Center(Language.MainOther.ChooseGiftTip3)
        end
    end
    self.data.make_num = num
    self:FlushNumView()
    MainOtherData.Instance:CheckChooselAllData()
end

function ChooseGiftAllItem:FlushNumView()
    UH.SetText(self.InputText,self.data.make_num)
end

function ChooseGiftAllItem:OnClickAddNum(num)
    if self.data.make_num + num <= 0 then
        MainOtherData.Instance:CheckChooselAllData()
        self:SetSelected(false)
        return
    end
    if self.data.func() + num > self.data.limit_num then
        PublicPopupCtrl.Instance:Center(Language.MainOther.ChooseGiftTip3)
        return
    end
    self.data.make_num = self.data.make_num + num
    self:FlushNumView()
    if self.data.make_num <= 1 then
        self:SetSelected(true)
    end
    MainOtherData.Instance:CheckChooselAllData()
end

function ChooseGiftAllItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        if self.data.make_num == 0 and self.data.func() + 1 > self.data.limit_num then
            PublicPopupCtrl.Instance:Center(Language.MainOther.ChooseGiftTip3)
            self:SetSelected(false)
            return
        end
        if self.data.make_num == 0 then
            self.data.make_num = 1
        end
    else
        self.data.make_num = 0
    end
    self:FlushNumView()
end

function ChooseGiftAllItem:OnClickItem()
    MainOtherData.Instance:CheckChooselAllData()
end