ShengQiUpPanel = ShengQiUpPanel or DeclareMono("ShengQiUpPanel", UIWFlushPanel)
function ShengQiUpPanel:ShengQiUpPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = true},
        {data = self.data.sel_stuff_list, func = self.FlushInfo, init_call = false},
    }
    self.type_list = {
        {text = Language.ShengQi.StuffType[0], item_id = 0, qua = 100},
        {text = Language.ShengQi.StuffType[1], item_id = Config.shengqi_auto.exp_mat[1], qua = 3},
        {text = Language.ShengQi.StuffType[2], item_id = Config.shengqi_auto.exp_mat[2], qua = 4},
        {text = Language.ShengQi.StuffType[3], item_id = Config.shengqi_auto.exp_mat[3], qua = 5},
        {text = Language.ShengQi.StuffType[4], item_id = Config.shengqi_auto.exp_mat[4], qua = 6},
    }

    GuideManager.Instance:RegisterGetGuideUi("ShengQiBlessPet",function ()
        return self.BlessPet,function ()
            self.BlessPet:OnSelClick()
            local pet_list = self.data:GetPetList()
            if TableIsEmpty(pet_list) then
                GuideManager.Instance:ForceStop()
                return
            end
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("ShengQiUpLevelBtn",function ()
        return self.UpBtn,function ()
            self:OnIntensifyClick()
        end
    end)
end

function ShengQiUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.cur_type_data = self.type_list[1]
    self:FlushTypeText()
    self.TypeList:SetData(self.type_list)
end

function ShengQiUpPanel:FlushPanel()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self:FlushShengQi()
    self:FlushInfo()
    -- 圣佑
    self.BlessPet:SetData(sheng_qi)
end

function ShengQiUpPanel:FlushTypeText()
    if self.cur_type_data then
        if QualityColorStrShallow[self.cur_type_data.qua] then
            local text = ColorStr(self.cur_type_data.text,QualityColorStrShallow[self.cur_type_data.qua])
            UH.SetText(self.SelTypeText, text)
        else
            UH.SetText(self.SelTypeText, self.cur_type_data.text)
        end
    end
end

function ShengQiUpPanel:FlushShengQi()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    self.ShengQiCell:SetData(sheng_qi)
    UH.SetText(self.Level, string.format(Language.ShengQi.Level2, sheng_qi:Level()))
    UH.SetText(self.Name, sheng_qi:Name())
end

function ShengQiUpPanel:FlushInfo()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    local is_max_level = sheng_qi:IsFullLevel()
    local stuff_exp = self.data:GetSelStuffExp()
    self.stuff_exp = stuff_exp
    local preview_level,need_coin = self.data:PreviewUpLevel(sheng_qi, stuff_exp)
    self.is_up_effect = preview_level > sheng_qi:Level()
    if preview_level == sheng_qi:Level() and not is_max_level then
        preview_level = preview_level + 1
    end

    self.BtnShow:SetActive(not is_max_level)
    self.BtnMax:SetActive(is_max_level)
    UH.SetInter(self.BtnInter, not is_max_level)

    --LogDG("StuffExp",stuff_exp,preview_level)
    local coin_str = need_coin 
    if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < need_coin then
        coin_str = ColorStr(coin_str, COLORSTR.Red8)
    end
    UH.SetText(self.CoinText, coin_str)
    self.need_coin = need_coin
    local attri_list = sheng_qi:ItemAttriList(preview_level)
    UH.SetText(self.CurLevel, string.format(Language.ShengQi.Level1,sheng_qi:Level()))
    UH.SetText(self.PreviewLevel, preview_level)

    if is_max_level then
        self.PreviewLevel:SetObjActive(false)
        self.CurLevel.gameObject:SetLocalPosition(Vector3.New(0,0,0))
        self.CurLevel.alignment = 4
    else
        self.PreviewLevel:SetObjActive(true)
        self.CurLevel.gameObject:SetLocalPosition(Vector3.New(-14.9,0,0))
        self.CurLevel.alignment = 3
    end

    for i = 1, 3 do
        local v = attri_list[i]
        self.ItemAttriList[i]:SetObjActive(not TableIsEmpty(v))
        if not TableIsEmpty(v) then
            self.ItemAttriList[i]:SetData(v)
        end
    end
    local layout_h = #attri_list >= 3 and 32 or 40
    self.ListLayout.spacing.y = layout_h

    -- ====  进度条 ====
    local start_exp, end_exp = sheng_qi:UpProgress()
    local all_exp = start_exp + stuff_exp

    local progress_num 
    if sheng_qi:IsFullLevel() then
        progress_num = 1
        self.Progress:SetText("")
    else
        self.Progress:SetText(string.format("%s/%s",all_exp,end_exp))
        progress_num = all_exp / end_exp
        if progress_num > 1 then
            progress_num = 1
        end
    end

    local level = sheng_qi:Level()
    if self.last_exp and self.last_exp ~= start_exp then
        if self.last_level and level > self.last_level then
            self.Progress:AddToProgress(1,function ()
                self.Progress:AddToProgress(progress_num)
            end)
        else
            self.Progress:AddToProgress(progress_num)
        end
    else
        self.Progress:SetProgress(progress_num)
    end
    
    self.last_exp = start_exp
    self.last_level = level
    local x = -211
    if end_exp > 0 then
        x = -211 - (end_exp - start_exp)/end_exp * 422 + 5
    end
    self.PregressEffectObj:SetLocalPosition(Vector3.New(x,1,0))

    self:FlushStuff()

    
end

function ShengQiUpPanel:FlushStuff()
    local stuff_list = self.data:GetAllSelStuff()
    local list_data = {}
    for i,v in pairs(stuff_list) do
        table.insert(list_data, {key = i,item_id = i.item_id,num = v, param = i.param})
    end
    for i = #list_data + 1, 5 do
        table.insert(list_data, {item_id = 0})
    end
    self.StuffList:SetData(list_data)
end

function ShengQiUpPanel:OnTypeClick(data)
    self:OnCloseSelClick()
    self.cur_type_data = data
    self:FlushTypeText()
end

function ShengQiUpPanel:OnCloseSelClick()
    self.SelToggle.isOn = false
end

function ShengQiUpPanel:OnAutoClick()
    local sheng_qi = self.data:GetSelShengQi()
    if sheng_qi:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.ShengQi.Tip13)
        return
    end

    local stuff_list = self.data:GetAllSelStuff()
    --if self.last_type_data and self.last_type_data ~= self.cur_type_data then
        for i,v in pairs(stuff_list) do
            if v then
                if i:Type() ~= ItemType.ShengQi then--and i:Color() >= self.cur_type_data.qua then
                    self.data:AddSelStuff(i,-9999)
                end
            end
        end
   -- end

    self.last_type_data = self.cur_type_data
    local exp_item_list = self.data:GetAllExpItem()

    if TableIsEmpty(exp_item_list) then
        PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip14)
    else
        table.sort(exp_item_list, function (a,b)
            return a:Color() > b:Color()
        end)
        for i,v in pairs(exp_item_list) do
            if v:Color() <= self.cur_type_data.qua then
                for j = 1, v:Num() do
                    local flag = self.data:AddSelStuff(v, 1)
                    if flag < 1 then
                        if flag == 0 then
                            PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip6)
                        end
                        return
                    end
                end
            end
        end
    end
end

function ShengQiUpPanel:OnIntensifyClick()
    local sheng_qi = self.data:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return
    end
    if sheng_qi:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.ShengQi.Tip13)
        return
    end

    if not self.stuff_exp or self.stuff_exp <= 0 then
        PublicPopupCtrl.Instance:Error(Language.ShengQi.Tip15)
        self.EffectTool:Play("4165116")
        return
    end

    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self.need_coin) then
        local stuff_item_list = self.data:GetAllSelStuff()
        local is_dialog = false
        for i,v in pairs(stuff_item_list) do
            if i:Type() == ItemType.ShengQi and i:Color() >= ItemColor.Orange then
                is_dialog = true
                break
            end
        end

        local effect_func = function ()
            if not self.is_up_effect then
                return
            end
            local attri_list = sheng_qi:PetAttriList() or {}
            for i,v in pairs(attri_list) do
                if self.AttriEffectList[i] then
                    self.EffectTool:Play(3165170,self.AttriEffectList[i])
                end
            end
        end

        if is_dialog then
            local info = {
                content = Language.ShengQi.Tip16,
                confirm = {
                        func = function ()
                            ShengQiCtrl.Instance:SendCSHolyEquipLevelUpReq()
                            effect_func()
                            ViewMgr:CloseView(DialogTipsView)
                        end,
                        wait_time = 5
                   },
               }
            PublicPopupCtrl.Instance:DialogTips(info)
        else
            ShengQiCtrl.Instance:SendCSHolyEquipLevelUpReq()
            effect_func()
        end
    end
end

ShengQiUpAttriItem = ShengQiUpAttriItem or DeclareMono("ShengQiUpAttriItem")
function ShengQiUpAttriItem:SetData(data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local preview_attri_info = CommonStruct.AttrStruct(data.attri_type, data.preview_value)
    UH.SetText(self.Name, string.format("%s：%s",attri_info.attr_name, attri_info.attr_percent_value))
    UH.SetText(self.PreviewValue, preview_attri_info.attr_percent_value)

    if data.attri_value == data.preview_value then
        self.PreviewValue:SetObjActive(false)
        self.Name.gameObject:SetLocalPosition(Vector3.New(0,0,0))
        self.Name.alignment = 4
    else
        self.PreviewValue:SetObjActive(true)
        self.Name.gameObject:SetLocalPosition(Vector3.New(-104,0,0))
        self.Name.alignment = 3
    end
end

-- 材料
ShengQiUpStuffItem = ShengQiUpStuffItem or DeclareMono("ShengQiUpStuffItem", UIWidgetBaseItem)
function ShengQiUpStuffItem:SetData(data)
    local stuff_item = Item.Create({item_id = data.item_id, num = data.num, param = data.param})
    UIWidgetBaseItem.SetData(self, data)
    local item_id = stuff_item.item_id or 0
    self.RemoveBtnObj:SetActive(item_id ~= 0)
    self.AddImg:SetActive(item_id == 0)
    if item_id == 0 then
        self.ItemCell:SetData(Item.Create({item_id = 0}))
    else
        self.ItemCell:SetData(stuff_item)
    end
end

function ShengQiUpStuffItem:OnRemoveClick()
    if self.data.key then
        ShengQiData.Instance:AddSelStuff(self.data.key, -99999)
    end
end


-- >>>>> 圣器材料背包
ShengQiStuffBag = ShengQiStuffBag or DeclareMono("ShengQiStuffBag", UIWFlushPanel)
function ShengQiStuffBag:ShengQiStuffBag()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_stuff_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
    self.type_list = {
        {text = Language.ShengQi.QuaType[0], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.None},
        {text = Language.ShengQi.QuaType[3], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Blue},
        {text = Language.ShengQi.QuaType[4], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Purple},
        {text = Language.ShengQi.QuaType[5], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Orange},
        {text = Language.ShengQi.QuaType[6], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Red},
    }

    GuideManager.Instance:RegisterGetGuideUi("ShengQiLevelStuff",function ()
        local item
        for i,v in pairs(self.List.index_item) do
            local data = v:GetData()
            if data.item_id == 26763 then
                item = v
            end
        end
        if not item then
            GuideManager.Instance:ForceStop()
            return
        end
        return item, item:OnStuffClick()
    end)
end

function ShengQiStuffBag:Awake()
    UIWFlushPanel.Awake(self)
    self.cur_type_data = self.type_list[1]
    self.TypeList:SetData(self.type_list)

    self:FlushPanel()
end

function ShengQiStuffBag:FlushPanel()
    if TableIsEmpty(self.cur_type_data) then
        return
    end
    UH.SetText(self.SelTypeText, self.cur_type_data.text)
    local list_data = self.data:GetAllStuff(self.cur_type_data.qua)
    local min_count = self:GetMinCount(#list_data)
    if #list_data < min_count then
        local is_add = true
        for i = #list_data + 1, min_count do
            local item = Item.Create({item_id = 0})
            if is_add then
                item.show_add_item = true
                is_add = false
            end
            table.insert(list_data, item)
        end
    end
    self.List:SetData(list_data)
end

function ShengQiStuffBag:GetMinCount(num)
    if num < self.MinCount then
        return self.MinCount
    else
        local n = num % 4
        return num + 4 - n
    end
end

function ShengQiStuffBag:OnTypeClick(data)
    self:OnCloseSelClick()
    self.cur_type_data = data
    self:FlushPanel()
end

function ShengQiStuffBag:OnCloseSelClick()
    self.SelToggle.isOn = false
end




ShengQiStuffBagItem = ShengQiStuffBagItem or DeclareMono("ShengQiStuffBagItem", UIWidgetBaseItem)
function ShengQiStuffBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cur_num = ShengQiData.Instance:GetSelStuff(self.data)
    self.SelImg:SetActive(cur_num > 0)
    self.Cell:SetData(data)
    self.ShengQiEvent:SetActive(data.item_id > 0 and data:Type() == ItemType.ShengQi)
    self.StuffEvent:SetActive(data.item_id > 0 and data:Type() ~= ItemType.ShengQi)

    --local is_show_remove = data.item_id > 0 and data:Type() ~= ItemType.ShengQi and cur_num > 0
    local is_show_remove = data.item_id > 0 and cur_num > 0
    self.RemoveBtn:SetActive(is_show_remove)
    self.ShengQiRemoveBtn:SetActive(is_show_remove)
    local num = data:Num()
    if is_show_remove then
        CellFlushs.SetNum(self.Cell, string.format("%s/%s",cur_num,num))
    else
        CellFlushs.SetNum(self.Cell, num)
    end

    if self.AddItem then
        self.AddItem:SetActive(data.show_add_item)
    end
end

function ShengQiStuffBagItem:OnDestroy()
    TimeHelper:CancelTimer(self.touch_ht)
end

function ShengQiStuffBagItem:OnShengQiClick()
    local cur_num = ShengQiData.Instance:GetSelStuff(self.data)
    local num = cur_num > 0 and -1 or 1
    local func = function ()
        local result = ShengQiData.Instance:AddSelStuff(self.data, num)
        self:SelTip(result)
    end
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if self.data:IsHasFushi() and num > 0 and sheng_qi and not sheng_qi:IsFullLevel() then
        PublicPopupCtrl.Instance:AlertTip(Language.ShengQi.Tip17,function ()
            func()
        end)
    else
        func()
    end
end

function ShengQiStuffBagItem:OnStuffClick()
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if not sheng_qi then
        return
    end
    if sheng_qi:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.ShengQi.Tip13)
        return
    end
    local result = ShengQiData.Instance:AddSelStuff(self.data, 1)
    self:SelTip(result)
    if result < 1 then
        TimeHelper:CancelTimer(self.touch_ht)
    end
end

function ShengQiStuffBagItem:OnStuffDown()
    TimeHelper:CancelTimer(self.touch_ht)
	self.touch_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnStuffClick,self),0.2)
end

function ShengQiStuffBagItem:OnStuffUp()
    TimeHelper:CancelTimer(self.touch_ht)
end

function ShengQiStuffBagItem:OnStuffRemove()
    ShengQiData.Instance:AddSelStuff(self.data, -1)
end

function ShengQiStuffBagItem:SelTip(result)
    if result == 0 then
        PublicPopupCtrl.Instance:Center(Language.ShengQi.Tip6)
    elseif result == -1 then
        PublicPopupCtrl.Instance:Center(string.format(Language.ShengQi.Tip7, ShengQiCfg.MAX_SHENG_QI_STUFF_COUNT))
    elseif result == -2 then
        PublicPopupCtrl.Instance:Center("圣器数值异常，麻烦联系客服处理！")
    elseif result == -3 then
        LogDG("圣器选择材料异常 result == ", result)
    end
end

function ShengQiStuffBagItem:OnAddItemClick()
    ShengQiCtrl.GetShengQiJump()
end


ShengQiPetItem = ShengQiPetItem or DeclareMono("ShengQiPetItem", UIWidgetBaseItem)
-- data = sheng_qi
function ShengQiPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pet = data:BlessPet()
    self.AddImg:SetActive(pet == nil)
    self.LianYaoFlag:SetActive(pet ~= nil and pet:IsLianYao())
    if pet then
        self.ItemCell:SetData(pet)
        UH.SetText(self.Name, pet:Name())
        UH.Color(self.Name, pet:IsLianYao() and COLORS.Pink1 or COLORS.Yellow7)
    else
        self.ItemCell:SetData(Item.Create({item_id = 0}))
        UH.SetText(self.Name, Language.ShengQi.Text14)
        UH.Color(self.Name, COLORS.Yellow7)
    end
    local pet_list = ShengQiData.Instance:GetPetList()
    self.TipImg:SetActive(not TableIsEmpty(pet_list) and pet == nil)
end

function ShengQiPetItem:OnSelClick()
    ViewMgr:OpenView(ShengQiSelectPetView)
end



--- ============= 圣佑宠物选择 ==============
ShengQiSelectPetView = ShengQiSelectPetView or DeclareView("ShengQiSelectPetView","sheng_qi/sheng_qi_pet_select_view")
VIEW_DECLARE_MASK(ShengQiSelectPetView,ViewMask.BlockClose)
function ShengQiSelectPetView:ShengQiSelectPetView()
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.PetSelect,Vector2.New(384, 637))
end

ShengQiSelectPetPanel = ShengQiSelectPetPanel or DeclareMono("ShengQiSelectPetPanel", UIWFlushPanel)
function ShengQiSelectPetPanel:ShengQiSelectPetPanel()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_sheng_qi, func = self.FlushPanel, init_call = false},
    }

    GuideManager.Instance:RegisterGetGuideUi("ShengQiSelectPetToggle",function ()
        local pet_list = self.data:GetPetList()
        if TableIsEmpty(pet_list) then
            GuideManager.Instance:ForceStop()
            return
        end
        return self.PetList.item_list[1].Toggle,function ()
            self.PetList.item_list[1]:OnToggleClick()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("ShengQiSelectPetOKBtn",function ()
        return self.OkBtn,function ()
            self:OnOkClick()
        end
    end)

end

function ShengQiSelectPetPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShengQiSelectPetPanel:FlushPanel()
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if not sheng_qi then
        return
    end
    local pet_list = self.data:GetPetList()
    self.EmptyState:SetActive(TableIsEmpty(pet_list))
    self.PetList:SetData(pet_list)
    local pet = sheng_qi:BlessPet()
    local count = pet == nil and 0 or 1
    UH.SetText(self.Tip,string.format(Language.ShengQi.Text15,count,1))
end

function ShengQiSelectPetPanel:OnOkClick()
    ViewMgr:CloseView(ShengQiSelectPetView)
end


ShengQiSelectPetItem = ShengQiSelectPetItem or DeclareMono("ShengQiSelectPetItem",UIWidgetBaseItem)
-- data == pet
function ShengQiSelectPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.PetItem:SetData(data)
    UH.SetText(self.Level,string.format(Language.Bicycle.Level,data:Level()))
    UH.SetText(self.Name,data:Name())
    if data:ShengQi() then
        self.is_on = true
        self.ToggleFlag:SetActive(true)
    else
        self.is_on = false
        self.ToggleFlag:SetActive(false)
    end
end

function ShengQiSelectPetItem:OnToggleClick()
    local sheng_qi = ShengQiData.Instance:GetSelShengQi()
    if not sheng_qi then
        return
    end
    local p2 = self.is_on and -1 or self.data.index
    ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_BLESS_PET,sheng_qi.index,p2)
end