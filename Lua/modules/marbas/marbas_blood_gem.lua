-- 造化血祭·元魂珠升级界面
MarbasBloodGemView = MarbasBloodGemView or DeclareView("MarbasBloodGemView", "marbas/marbas_blood_gem")
function MarbasBloodGemView:MarbasBloodSelectView()
    -- self.board
end
function MarbasBloodGemView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.BloodGemUp, Vector2.New(540, 523))
end

function MarbasBloodGemView:OnCloseClick()
    ViewMgr:CloseView(MarbasBloodGemView)
end


--------------------------MarbasBloodGemPanel--------------------------
MarbasBloodGemPanel = MarbasBloodGemPanel or DeclareMono("MarbasBloodGemPanel", UIWFlushPanel)
function MarbasBloodGemPanel:MarbasBloodGemPanel()
    self.data = MarbasData.Instance
    self.data_cares = {
        {data = self.data.item_grid, func = self.FlushPanel},
        {data = RoleData.Instance:GetCurrencyData(), func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
    self.plus_param = nil
end 

function MarbasBloodGemPanel:onFlush()
    self:FlushPanel()
end

function MarbasBloodGemPanel:FlushPanel()
    self.plus_param = self.data:GetMarbasGemNPlusInfo()

    self.call_cur:SetData(self.plus_param.item_cur)
    self.call_next:SetData(self.plus_param.item_next)
    -- self.cost_list:SetData(BagData.Instance:ItemListBuild(self.plus_param.item_cost))
    -- UH.SetText(self.call_name, self.plus_param.item_cost:QuaName())

    for i = 1,self.attr_list:Length() do 
        local flag = self.plus_param.list[i] ~= nil 
        self.attr_list[i].gameObject:SetActive(flag )
        if flag then 
            self.attr_list[i]:SetData(self.plus_param.list[i])
        end 
    end

    local num = RoleData.Instance:GetCurrency(CurrencyType.CanHun)
    self.flag_enough =  num >= self.plus_param.need_num
    local color = self.flag_enough and COLORSTR.Green7 or COLORSTR.Red8
    UH.SetText(self.cost_num,ColorStr(num,color).."/".. self.plus_param.need_num)
    UH.SetIcon(self.cost_sp_icon, Item.GetIconId(HuoBi[CurrencyType.CanHun]))

    UH.SetIcon(self.huobi_icon, Item.GetIconId(HuoBi[CurrencyType.CanHun]))
    UH.SetText(self.huobi_num, num)
    -- 

    UH.SetText(self.level_cur, ColorStr(self.plus_param.item_cur:GemLevel()..Language.Common.Level,self.plus_param.item_cur:ColorStr()))
    UH.SetText(self.level_next, ColorStr(self.plus_param.item_next:GemLevel()..Language.Common.Level,self.plus_param.item_next:ColorStr()))
end

function MarbasBloodGemPanel:OnClickPlus()
    if not self.flag_enough then
        PublicPopupCtrl.Instance:Center(string.format(Language.Marbas.BloodGemHuoBiLack,Item.GetName(HuoBi[CurrencyType.CanHun])))
        return
    end    

    local select_equip = self.data:TypeBloodGetBloodSelect()
    local mark = self.plus_param.item_cur
    local select_index = self.data.blood_view_data.select_index 
    
    local in_bag = mark.src_type ~= nil and mark.src_type == BloodSrcType.Bag
    MarbasCtrl.Instance:SendBloodGemPlusOper({
        item_src = mark.src_type ~= nil and mark.src_type or BloodSrcType.Bag,
        index = in_bag and mark:GridIndex() or select_equip.index ,
        column = in_bag and mark:GridColumn() or 0,
        slot_index = in_bag and 0 or select_index,
        -- buy_item_num = self.plus_param.param[0] ~= nil and self.plus_param.param[0][1] or 0,
        -- --这个num是数组长度
        -- num = self.plus_param.param[1] ~= nil and #self.plus_param.param[1] or 0,
        -- consume_list = self.plus_param.param[1],
    })

    ViewMgr:CloseView(MarbasBloodGemView)
end

function MarbasBloodGemPanel:OnClickAdd()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.CanHun})})
end