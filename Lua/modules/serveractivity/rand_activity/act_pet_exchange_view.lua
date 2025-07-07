ActPetExchangeView = ActPetExchangeView or DeclareView("ActPetExchangeView","serveractivity/act_pet_exchange",Mod.RandActivity.PetExchange)
ActPetExchangeView.ReqType = {
    Info = 0,   -- 获取信息
    Exchange = 1, -- 兑换
    BuyItem = 2,    -- 商店购物消费
    ItemCheck = 3,  -- 商店购买直购物品检测
}
VIEW_DECLARE_LEVEL(ActPetExchangeView, ViewLevel.L1)
function ActPetExchangeView:ActPetExchangeView()
    self.Board:SetData(self:GetType(),ActivityRandData.GetName(ACTIVITY_TYPE.RAND_PET_EXCHANGE))
end 

function ActPetExchangeView:LoadCallback()
    -- self.Time:CreateActTime(ACTIVITY_TYPE.RAND_PET_EXCHANGE,TimeType.Type_Special_4)
    local list_data = self:GetListData()
    self.PetList:SetData(list_data)
    if not TableIsEmpty(list_data) then
        self.PetList:ClickItem(1)
    end

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.PetModel:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet
        })
    end
   
    self.bag_care = BagData.Instance:Care(BindTool.Bind(self.OnFlush,self))
end 

function ActPetExchangeView:GetListData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfgSixStart(ACTIVITY_TYPE.RAND_PET_EXCHANGE,Config.pet_exchange_activity_auto.pet_exchange)
    local list_data = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for i,v in pairs(cfg) do
        if role_level >= v.grade_limit then
            table.insert(list_data, v)
        end
    end
    return list_data
end

function ActPetExchangeView:OnFlush()
    if self._data then 
        local pet = Pet.New({id = self._data.item.item_id})
        if pet and pet:ResId() then
            self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
            UH.SetText(self.PetName,pet:Name())
            UH.SpriteName(self.PetLevel, PetData.PetLevel[pet.config.quality])
            self:FlushPetAttrInfo(pet)
        end
        self.cur_pet = self._data
        self._data = nil 
    end 
    self:FlushStuffInfo()
end

function ActPetExchangeView:FlushPetAttrInfo(pet)
    self.AttrList:SetPet(pet, true)
    self.SkillList:SetData(pet:GetSkillAndGiftGroupList())
end

function ActPetExchangeView:FlushStuffInfo()
    if self.cur_pet == nil then return end
    self.StuffCellList:SetData(self.cur_pet.consume_item)
end

function ActPetExchangeView:ClickShowPet(data_)
    self._data = data_
    self:OnFlush()
end

function ActPetExchangeView:OnClickSkillShow(data_)
    PetData.Instance:SetClickSkill(data_)
    ViewMgr:OpenView(PetSkillTip,{pos = Vector2.New(-128,-50)})
end

function ActPetExchangeView:OnClickPetExchange()
    if self.cur_pet == nil then return end 

    local is_can = true
    for i,v in pairs(self.cur_pet.consume_item) do
        local need_num = v.num
        local has_num = Item.GetNum(v.item_id)
        if has_num < need_num then
            is_can = false
            break
        end
    end

    if not is_can then
        ViewMgr:OpenView(ActPetExchangeStore)
    end

    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_EXCHANGE,ActPetExchangeView.ReqType.Exchange,self.cur_pet.seq1)
end

function ActPetExchangeView:CloseCallback()
    BagData.Instance:UnCare(self.bag_care)
    if self.handle_care then
        BagData.Instance:UnCare(self.handle_care)
        self.handle_care = nil
    end
end

function ActPetExchangeView:OnStoreClick()
    ViewMgr:OpenView(ActPetExchangeStore)
end

----------------------------------pet exchange cell----------------------------------------
PetExchangeCell = PetExchangeCell or DeclareMono("PetExchangeCell",UIWidgetBaseItem)

function PetExchangeCell:SetData(data)
    local pet = Pet.New({id = data.item.item_id})
    UH.SetText(self.Name,pet:Name())
    UH.SetText(self.Level,Language.Pet.ColorLevelT[pet.config.quality])
    UIWidgetBaseItem.SetData(self, data)
end

function PetExchangeCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end


PetExchangeStuffItem = PetExchangeStuffItem or DeclareMono("PetExchangeStuffItem", UIWidgetBaseItem)
function PetExchangeStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id, is_bind = 1})
    self.ItemCell:SetData(item)
    local has_num = Item.GetNum(data.item_id)
    local need_num = data.num
    local color = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
    local num_str = ColorStr(has_num,color)
    UH.SetText(self.ItemNum, string.format("%s/%s",num_str, need_num))
end


ActPetExchangeStore = ActPetExchangeStore or DeclareView("ActPetExchangeStore","serveractivity/act_pet_exchange_store")
function ActPetExchangeStore:OnCloseClick()
    ViewMgr:CloseView(ActPetExchangeStore)
end

ActPetExchangeStorePanel = ActPetExchangeStorePanel or DeclareMono("ActPetExchangeStorePanel", UIWFlushPanel)
function ActPetExchangeStorePanel:ActPetExchangeStorePanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.pet_exchange_data, func = self.FlushPanel, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActPetExchangeStorePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ActPetExchangeStorePanel:FlushPanel()
    local list_data = self:GetListData()
    self.List:SetData(list_data)
end

function ActPetExchangeStorePanel:SendBuy()
    if self.cfg == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_PET_EXCHANGE, self.cfg.buy_money)
end

function ActPetExchangeStorePanel:GetListData()
    local cfg = self.data:GetActOpenDayCfgSixStart(ACTIVITY_TYPE.RAND_PET_EXCHANGE,Config.pet_exchange_activity_auto.item_buy)
    return cfg
end

function ActPetExchangeStorePanel:OnBuyClick(data)
    self.cfg = data
    local remain_num = self.data:GetPetExchangeShopRemain(data)
    if remain_num < 1 then
        PublicPopupCtrl.Instance:Error(Language.Activity.ButLimitTip)
        return
    end
    if data.cfg_or == 1 then
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_PET_EXCHANGE,data.buy_money / 10, ACTIVITY_TYPE.RAND_PET_EXCHANGE,ActPetExchangeView.ReqType.ItemCheck,data.seq)
    else
        if MallCtrl.IsNotCurrency(data.price_type, data.buy_money) then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_EXCHANGE,ActPetExchangeView.ReqType.BuyItem,data.seq)
        end
    end
end


ActPetExchangeStoreItem = ActPetExchangeStoreItem or DeclareMono("ActPetExchangeStoreItem", UIWidgetBaseItem)
function ActPetExchangeStoreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.item)
    UH.SetText(self.ItemName, item:Name())
    self.ItemCell:SetData(item)
    local remain_num = ServerActivityData.Instance:GetPetExchangeShopRemain(data)
    UH.SetInter(self.BtnInter, remain_num > 0)
    local remain_str
    if remain_num > 0 then
        remain_str = ColorStr(remain_num, COLORSTR.Green9)
    else
        remain_str = ColorStr(remain_num, COLORSTR.Red20)
    end
    UH.SetText(self.LimitText, string.format("%s：%s/%s",Language.Activity.LimitType[data.limit_type],remain_str,data.buy_times))
    if data.cfg_or == 1 then
        UH.SetText(self.Price, data.buy_money / 10)
    else
        UH.SetText(self.Price, data.buy_money)
    end
    UH.SetActGold(self.Icon, data.price_type, data.cfg_or)
end