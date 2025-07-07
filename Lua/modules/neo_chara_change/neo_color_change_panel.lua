NeoColorChangePanel = NeoColorChangePanel or DeclareMono("NeoColorChangePanel", UIWFlushPanel)
--ColorBtnHandles=====LuaUIEventClick[]
--ColorBtns===========Toggle[]
--CurAppear===========ModelUIShow
--NextAppear==========
--CurProfLbl==========
--ItemCell============UIWSubstitute[UIWItemCell]
--ItemNameCountTxt====Text
--ExeInteractor=======InteractorHandler
--CurrencyImg=========UIImageSpriteSet
--CurrencyNum=========Text


local cost_currency_type = CurrencyType.CoinBind
function NeoColorChangePanel:NeoColorChangePanel()
    self.cur_ui_obj = nil
    self.next_ui_obj  = nil
    NeoCharaChangeData.Instance:SetColorChangeColor(1)

    self.data_cares = {
        {data = NeoCharaChangeData.Instance:ColorChangeData(),func = self.FlushColorSet,keys = {"color_index"}},
        {data = NeoCharaChangeData.Instance:ColorChangeData(),func = self.FlushCost,keys = {"color_index"}},
        {data = RoleData.Instance:GetCurrencyData(), func = self.FlushCost,keys={cost_currency_type}, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushCost, init_call = false},
    }
    self:FlushCost()
end

function NeoColorChangePanel:Awake()
    UIWFlushPanel.Awake(self)
    local role_apear = NeoColorChangePanel.baseApearance()
    -- LogError("role_apear===",role_apear)
    local color_cfg = RoleAppearChangeData:GetRoleAppCfgByAppear(role_apear)
    if color_cfg and color_cfg.color_list ~= nil then 
        local cur_color_list = Split(color_cfg.color_list,",")
        for i = 1,4 do 
            UH.SpriteName(self.ColorBtns[i], "PeiSe"..cur_color_list[i])
        end 
    end 
    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.New()
    end 

    self.cur_ui_obj:SetData(role_apear)
    self.CurAppear:SetShowData({
        ui_obj = self.cur_ui_obj,
        view = self,
        seq = 1,
    })
    local prof_txt = string.format(Language.RoleAppearChange.CurProfLbl,RolePromoteData.GetProfName(RoleData.Instance:GetRoleProfId()))
    UH.SetText(self.CurProfLbl,prof_txt)
end

function NeoColorChangePanel.baseApearance()
    local role_apear = TableCopy(RoleData.Instance:GetApearance())
    role_apear.fashion_body_id = 0  --  主角的基础上去掉时装
    role_apear.fashion_weapon_id = 0
    role_apear.weapon_id = RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(RoleData.Instance:GetBaseData().profession_id)] or 0
    -- LogError("PPP==",role_apear)
    return role_apear
end

function NeoColorChangePanel:FlushColorSet()
    local role_apear = NeoColorChangePanel.baseApearance()
    local cur_sel_co = NeoCharaChangeData.Instance:ColorChangeData().color_index
    role_apear.color = cur_sel_co
    if self.next_ui_obj == nil then
        self.next_ui_obj = UIChDrawer.New()
        self.NextAppear:SetShowData({
            ui_obj = self.next_ui_obj,
            view = self,
            seq = 2,
        })    
    end
    self.next_ui_obj:SetData(role_apear)
    self.ColorBtnHandles[cur_sel_co + 1].isOn = true
end

function NeoColorChangePanel:OnColorClick(index)
    NeoCharaChangeData.Instance:SetColorChangeColor(index)
end 

function NeoColorChangePanel:OnDestroy()
    -- NeoCharaChangeData.Instance:SetColorChangeColor(1)
    if self.next_ui_obj ~= nil then
        self.next_ui_obj:DeleteMe()
    end
    if self.cur_ui_obj ~= nil then
        self.cur_ui_obj:DeleteMe()
    end

    UIWFlushPanel.OnDestroy(self)
end

function NeoColorChangePanel:FlushCost()
    local cfg = Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId(),0);
    local need_item_id = cfg.change_item
    local need_item_count = cfg.change_item_num
    local need_num = need_item_count - Item.GetNum(need_item_id)
    need_num = need_num < 1 and 1 or need_num
    local need_item = Item.Create({item_id = need_item_id, jump_need_num = need_num})
    self.ItemCell:SetData(need_item)
    local item_name = need_item:Name()
    local bag_item_count = BagData.Instance:GetNumByItemId(need_item_id)
    local num_color = bag_item_count >= need_item_count and COLORSTR.Green4 or COLORSTR.Red8
    UH.SetText(self.ItemNameCountTxt,string.format(Language.RoleAppearChange.Color.ItemTips,item_name,num_color,bag_item_count,need_item_count))
    if cfg.change_color_coin > 0 then
        self.CurrencyNum:SetObjActive(true)
        self.CurrencyImg:SetObjActive(true)
        UH.SetText(self.CurrencyNum,cfg.change_color_coin)
    else
        self.CurrencyNum:SetObjActive(false)
        self.CurrencyImg:SetObjActive(false)
    end

    local enough_cost = RoleData.Instance:GetCurrency(cost_currency_type) >= cfg.change_color_coin and 
            bag_item_count >= need_item_count

    local role_appearance = RoleData.Instance:GetApearance()
    local not_same_color = role_appearance.color ~= NeoCharaChangeData.Instance:ColorChangeData().color_index
    self.ExeInteractor.Interactable = enough_cost and not_same_color
end

function NeoColorChangePanel:ChangeClick()
    local cfg = Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId(),0);
    if NeoCharaChangeData.Instance:ColorChangeData().color_index == RoleData.Instance:GetApearance().color then
        PublicPopupCtrl.Instance:Center(Language.RoleAppearChange.Color.SameColor)
    elseif cfg.change_item_num > BagData.Instance:GetNumByItemId(cfg.change_item) then
        ItemInfoCtrl.Instance:ItemInfoView({item_list = {Item.Init(cfg.change_item)},show_ways = true})
        BagData.Instance:SetGetWaySweepMark({need = cfg.change_item_num - BagData.Instance:GetNumByItemId(cfg.change_item)})
    elseif cfg.change_color_coin > RoleData.Instance:GetCurrency(cost_currency_type) then
        MallCtrl.IsNotCurrency(cost_currency_type,cfg.change_color_coin)
    else
        NeoCharaChangeCtrl.Instance:RequestChangeColor(NeoCharaChangeData.Instance:ColorChangeData().color_index)
        ViewMgr:OpenView(NeoColorChangeSuccView)
        ViewMgr:CloseView(NeoProfChangeView)
    end
end