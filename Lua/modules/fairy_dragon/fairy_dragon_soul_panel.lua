-- 神龙龙魂
FairyDragonSoulPanel = FairyDragonSoulPanel or DeclareMono("FairyDragonSoulPanel", UIWFlushPanel)
function FairyDragonSoulPanel:FairyDragonSoulPanel()
    self.data = FairyDragonData.Instance
    self.dragon = FairyDragon.Instance
    self.soul_data = FairyDragonSoulData.Instance
    self.data_cares = {
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false},
        {data = self.soul_data.soul_item_list, func = self.FlushPanel, init_call = false},
    }
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = FairyDragonSkinPanel,
                show_shadow = true,
			}
		)
    end
end

function FairyDragonSoulPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function FairyDragonSoulPanel:OnEnable()

    UIWFlushPanel.OnEnable(self)
    local res_id = self.dragon:ResId()
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.dragon:ResId()))
    self:FlushPanel()
end

function FairyDragonSoulPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function FairyDragonSoulPanel:FlushPanel()
    local list_cfg = self.soul_data:GetHoleCfg()
    for i = 1, self.ItemList:Length() do
        if list_cfg[i] then
            self.ItemList[i]:SetData(list_cfg[i])
        end
    end
end

function FairyDragonSoulPanel:OnExchangeClick()
    --ShoppingCtrl.Instance:OpenStoreView(0)
    ViewMgr:OpenView(LBShopView, {is_out = true})
end

function FairyDragonSoulPanel:OnBagClick()
    ViewMgr:OpenView(FairyDragonSoulBagView)
end

function FairyDragonSoulPanel:OnGoToPrayClick()
    ViewMgr:OpenViewByKey(Mod.FairyDragon.Pray)
end



FairyDragonSoulPanelItem = FairyDragonSoulPanelItem or DeclareMono("FairyDragonSoulPanelItem", UIWidgetBaseItem)
-- data == Config.god_dragon_auto.dragon_soul_unlock
function FairyDragonSoulPanelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local soul_type = data.dragon_soul_hole
    local info = FairyDragon.Instance:GetSoulInfo(soul_type) or {}
    local is_unlock = info.is_unlock == 1
    local soul_item
    UH.SetText(self.TriggerRate, "")
    UH.SetText(self.TriggerText, "")
    UH.SetText(self.LockDesc, "")
    if not is_unlock then
        soul_item = Item.Create({item_id = 0, is_grey = true})
        UH.SetText(self.LockDesc, FairyDragonSoulData.Instance:GetUnlockTip(data))
    else
        local bag_index = info.index
        soul_item = FairyDragonSoulData.Instance:GetItem(bag_index) or Item.Create({item_id = 0})
        UH.SetText(self.TriggerText, Language.FairyDragon.SoulTypeTrigger[soul_type])
        if soul_item.item_id ~= 0 then
            local rate = soul_item:SpTriggerRate()
            UH.SetText(self.TriggerRate, string.format(Language.FairyDragon.Text15,rate))
            UH.SetText(self.Level, "Lv" .. soul_item:Level())
        end
    end
    UH.SetInter(self.Inter, is_unlock)
    self.ItemCell:SetData(soul_item)
    self.AddObj:SetActive(is_unlock == true and soul_item.item_id == 0)
    self.Frame:SetActive(soul_item.item_id ~= 0 and data.client_order ~= 1 and data.client_order ~= 5)
    self.Frame2:SetActive(data.client_order == 1 or data.client_order == 5)
    self.LevelInfo:SetActive(soul_item.item_id ~= 0)
    self.Lock:SetActive(not is_unlock)
    self.item = soul_item

    self.RedPoint:SetNum(FairyDragonData.Instance:RemindBySoulType(soul_type))
end

function FairyDragonSoulPanelItem:OnAddClick()
    FairyDragonSoulData.Instance:SetBagSelType(self.data.dragon_soul_hole)
    ViewMgr:OpenView(FairyDragonSoulBagView)
end

function FairyDragonSoulPanelItem:OnItemClick()
    local data_list = {self.item}
    local func_list = {}
    table.insert(func_list, ItemInfoFunc.DragonSoulRep2)
    table.insert(func_list, ItemInfoFunc.DragonSoulUp)
    table.insert(func_list, ItemInfoFunc.DragonSoulUnfix)
    local info = {
        func = function ()
			return func_list
        end,
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function FairyDragonSoulPanelItem:OnLockClick()
    PublicPopupCtrl.Instance:Error(FairyDragonSoulData.Instance:GetUnlockTip(self.data, true) or "")
end