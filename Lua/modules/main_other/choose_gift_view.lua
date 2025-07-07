ChooseGiftView = ChooseGiftView or DeclareView("ChooseGiftView", "main_other/choose_gift")

VIEW_DECLARE_MASK(ChooseGiftView, ViewMask.BgBlock)

function ChooseGiftView:OnClickClose()
    ViewMgr:CloseView(ChooseGiftView)
end

--自选礼包统一放在了礼包表 自选礼包不会再使用主动消耗表
function ChooseGiftView:LoadCallback(param_t)
    self.param_t = param_t or {}
    local TittleTip = Language.MainOther.TittleTip
    self.item_list,self.config = self:GetItemList(self.param_t.item_id)
    UH.SetText(self.Title,Format(TittleTip,self.config.select_num))
    self.CallList:SetData(self.item_list)
    if self.config.select_num == 1 then
        self.CallList:SetSelType(0)
    end
end

--点击确认
function ChooseGiftView:OnClickConfirm()
    local select_info,select_num = self:GetSelectInfo()
    if select_num ~= self.config.select_num then
        PublicPopupCtrl.Instance:Center(Format(Language.MainOther.GiftUseTip,self.config.select_num))
        return
    end
    if self.param_t.num > 1 then
        self:SendUseAll(select_info)
        return
    end
    self:SendUseItemGift(select_info)   
    ViewMgr:CloseView(ChooseGiftView)
end

--礼包批量使用
function ChooseGiftView:SendUseAll(select_info)
    PublicPopupCtrl.Instance:AlertTip(Language.MainOther.GiftUseAllTip,function()
        self:SendUseItemGift(select_info)
        ViewMgr:CloseView(ChooseGiftView)
    end)
end

--道具使用
function ChooseGiftView:SendUseItemGift(select_info)
    if self.config.is_expense == true then 
        self.param_t.param1 = select_info[1].index
        BagCtrl.Instance:SendUseItem(self.param_t)
    else 
        BagCtrl.Instance:SendUseOptionalGift(self.param_t.column_type,
        self.param_t.index,select_info,self.param_t.num)
    end 
end

--获取选中的道具
function ChooseGiftView:GetSelectInfo()
    local select_info = {}
    local select_list = self.CallList:GetSelectGroupData() or {}
    for k,v in pairs(select_list) do
        table.insert(select_info,{index = v.vo.mark_index or v.index,num = v.num,item_id = v.item_id})
    end
    return select_info,#select_list
end

function ChooseGiftView:GetItemList(item_id)
    local item_list,config = BagData.Instance:GiftItemList(item_id)
    for i,v in ipairs(item_list) do
        v.index = i - 1
        v.select_num = config.select_num
        v.func = function()return #self.CallList:GetSelectGroupData()end
    end
    return item_list,config
end

----------------------------ChooseGiftItem----------------------------
ChooseGiftItem = ChooseGiftItem or DeclareMono("ChooseGiftItem", UIWidgetBaseItem)

function ChooseGiftItem:SetData(data)
    self.Call:SetData(data)
    UH.SetText(self.Name,data:Name())
    UH.RenameGame(self,[[Item]].. data.index)
    self.UseFlag:SetActive(FabaoData.GetUseFlag(data))
    UIWidgetBaseItem.SetData(self, data)

    if self.recommd ~= nil then 
    --     if data:Type() == ItemType.Gem then 
    --         self.recommd:SetActive(WorkshopData.Instance:GetProfRecommendAllPart(data.item_id))
    --     else 
            self.recommd:SetActive(false)
    --     end 
    end 
end

function ChooseGiftItem:OnClickItem()
    if GetActive(self.Select) and self.data.func() > self.data.select_num then
        PublicPopupCtrl.Instance:Center(Format(Language.MainOther.SelelctTip, self.data.select_num))
        self:SetSelected(false)
    end
end