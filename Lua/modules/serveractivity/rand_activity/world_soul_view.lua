WorldSoulView = WorldSoulView or DeclareView("WorldSoulView","serveractivity/world_soul_view",Mod.RandActivity.WorldSoul)

VIEW_DECLARE_LEVEL(WorldSoulView, ViewLevel.L1)

function WorldSoulView:WorldSoulView()
    self.data = ServerActivityData.Instance
    self.care_handle = self.data.world_soul_data:Care(BindTool.Bind(self.FlusnItemView,self))
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_WORLD_SOUL,TimeType.Type_Special_4,Language.WorldSoul.Time)
    self.ModelGame = UIObjDrawer.New()
    self.ModelShow:SetShowData({ui_obj = self.ModelGame,view = self,show_shadow = false})
    self.page = 1
end

function WorldSoulView:LoadCallback()
    self.toggle_list = self.data:GetWorldSoulList()
    local index = self:GetIndex() or 1
    UH.SetToggle(self.Toggle[index],true)
    self:OnClickItem(self.toggle_list[index])
    self:FlushItemList()
end

function WorldSoulView:GetIndex()
    for i,v in ipairs(self.toggle_list) do
        if self.data:GetWorldSoulFlag(v.seq) == 0 then
            return i
        end
    end
end

function WorldSoulView:CloseCallback()
	self.data.world_soul_data:Uncare(self.care_handle)
    Delete(self,[[ModelGame]])
	self.care_handle = nil
end

function WorldSoulView:OnClickClose()
    ViewMgr:CloseView(WorldSoulView)
end

function WorldSoulView:FlusnItemView()
    self:OnClickItem(self.select_data)
    self:FlushItemList()
end

function WorldSoulView:FlushItemList()
    for i=1,self.ToggleList:Length() do
        if self.toggle_list[i] then
            self.ToggleList[i]:SetData(self.toggle_list[i])
        end
        self.ToggleList[i]:SetObjActive(self.toggle_list[i] ~= nil)
    end
end

function WorldSoulView:FlushGiftView(gift_id)
    local config = BagData.Instance:GetGiftItemList(gift_id)
    self.gift_list = config.item_list or {}
    self:OnClickSelect(-self.page + 1)
end

function WorldSoulView:FlushModelShow(item_id)
    local config = Cfg.PetWeaponBaseCfg(item_id)
    local data = DrawerHelper.GetWeaponPath(config.pet_weapon_res)
    UH.SetText(self.Title, config.name)
    UH.LocalPos(self.ModelRect,Vector3.New(18,(item_id == 41036 or item_id == 41035) and -20 or 80,0))
    self.ModelGame:SetData(data)
end

function WorldSoulView:OnClickSelect(num)
    self.page = self.page + num
    self.BtnLeft:SetActive(self.page > 1)
    self.BtnRight:SetActive(self.page < #self.gift_list)
    self:FlushModelShow(self.gift_list[self.page].item_id)
end

function WorldSoulView:OnClickItem(data)
    local target_num = data.acc_price
    local BtnName = Language.HeavenSent.BtnName
    local flag = self.data:GetWorldSoulFlag(data.seq)
    local recharge_num = self.data:GetWorldSoulRechargeNum()

    self.Grid:SetData(DataHelper.FormatItemList(data.reward_item,1))
    UH.SetText(self.BtnName,BtnName[flag == 0 and (recharge_num >= target_num and 2 or 1) or 3])
    UH.SetText(self.Num,recharge_num .. Language.Common.HuoBi[CurrencyType.Gold])
    self.RedPiont:SetNum((flag == 0 and recharge_num >= target_num) and 1 or 0)
    self:FlushGiftView(data.gift_item.item_id)
    self.select_data = data
end

function WorldSoulView:OnClickRecharge()
    if self.RedPiont:GetNum() ~= 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_WORLD_SOUL,1,self.select_data.seq)
    else
        if self.data:GetWorldSoulFlag(self.select_data.seq) == 0 then
            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
            ViewMgr:CloseView(WorldSoulView)
        end
    end
end

function WorldSoulView:OnClickBlock()
    CellClicks.BagGridNoButton({GetData = function()
        return Item.Init(self.gift_list[self.page].item_id)
    end})
end
----------------------------WorldSoulItem----------------------------
WorldSoulItem = WorldSoulItem or DeclareMono("WorldSoulItem", UIWidgetBaseItem)

function WorldSoulItem:SetData(data)
    local target_num = data.acc_price
    local flag = ServerActivityData.Instance:GetWorldSoulFlag(data.seq)
    local recharge_num = ServerActivityData.Instance:GetWorldSoulRechargeNum()
    UH.SetText(self.Num,data.acc_price .. Language.Common.HuoBi[CurrencyType.Gold])
    self.RedPiont:SetNum((flag == 0 and recharge_num >= target_num) and 1 or 0)
    UIWidgetBaseItem.SetData(self, data)
end