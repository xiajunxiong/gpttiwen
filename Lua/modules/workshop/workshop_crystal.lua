WorkshopCrystal = WorkshopCrystal or DeclareMono("WorkshopCrystal", UIWFlushPanel)
function WorkshopCrystal:WorkshopCrystal()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        {data = self.data:CrystalData(), func = self.FlushLeftItem, keys = {"select_crystal"}, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false}
    }
    self.stuff_num_list = {}
    self.pos2 = Vector3.New(176, -16, 0)
    self.pos1 = Vector3.New(19.8, -54.5, 0)
    --self.cpos2 = Vector3.New(123, -200, 0)
    --self.cpos1 = Vector3.New(123, -220, 0)
end

function WorkshopCrystal:onFlush()
    self:FlushLeftItem()
    local crystal_data = self.data:CrystalData()
    local show = self.data:GetTypeShow(ComposeGroup.EleCrystal)
    --Debuger.LogError(Serialize(show))
    UH.SetText(self.LblTitle, show[crystal_data.select_crystal].type_name)
end
function WorkshopCrystal:FlushRightItem()
    if self.target_info then
        self:OnClickItem(self.target_info)
    end
end
function WorkshopCrystal:FlushLeftItem()
    local data = self.data:CrystalShowList()
    self.ItemList:SetData(data)
    self.ItemList:ClickItem(1)
end
function WorkshopCrystal:OnClickBtn(data)
    local crystal_data = self.data:CrystalData()
    UH.SetText(self.LblTitle, data.type_name)
    crystal_data.select_crystal = data.index
    self.BtnParent:SetActive(false)
end
function WorkshopCrystal:OnClickCrystal()
    self.BtnParent:SetActive(not self.BtnParent.activeSelf)
    if self.BtnParent.activeSelf then
        local data = self.data:GetTypeShow(ComposeGroup.EleCrystal)
        self.BtnList:SetData(data)
    end
end
function WorkshopCrystal:OnClickCreate()
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    self.EffectTool:Play(self.need_two and 10010261 or 10010260)
    self.ctrl:SendCompoundReq(self.seq, self.make_num, 0)
end
function WorkshopCrystal:OnClickItem(data)
    self.make_num = 1
    local item_id = 0
    local item = nil
    self.target_info = data
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}
    self.is_not_enough_stuff = false
    self.need_two = false
    self.seq = data.seq
    -- 判断是否需要两个物品
    if data.stuff_id_1 > 0 and data.stuff_id_2 > 0 then
        self.OneObj:SetActive(false)
        self.TwoObj:SetActive(true)
        UH.LocalPos(self.ResPos, self.pos2)
        self.need_two = true
        -- UH.LocalPos(self.ChoosePos, self.cpos2)
    else
        self.need_two = false
        self.OneObj:SetActive(true)
        self.TwoObj:SetActive(false)
        UH.LocalPos(self.ResPos, self.pos1)
        -- UH.LocalPos(self.ChoosePos, self.cpos1)
        item = Item.New()
        item.item_id = data.stuff_id_1
        UH.SetText(self.Name1, item:Name())
        UH.SetText(self.Num1, string.format("%d/%d", item:Num(), data.stuff_num_1))
        local flag = item:Num() < data.stuff_num_1
        UH.Color(self.Num1, flag and COLORS.Red8 or COLORS.Green3)
        self.Cell1:SetData(item)
    end
    for i = 1, 2 do
        item_id = data["stuff_id_" .. i]
        if item_id > 0 then
            item = Item.New()
            item.item_id = item_id
            UH.SetText(self.NameList[i], item:Name())
            UH.SetText(self.NumList[i], string.format("%d/%d", item:Num(), data["stuff_num_" .. i]))
            self.CellList[i]:SetData(item)
            self.stuff_num_list.need[i] = data["stuff_num_" .. i]
            self.stuff_num_list.have[i] = item:Num()
            local flag = item:Num() < data["stuff_num_" .. i]
            UH.Color(self.NumList[i], flag and COLORS.Red8 or COLORS.Green3)
            self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
        else
            UH.SetText(self.NameList[i], "")
            UH.SetText(self.NumList[i], "")
            self.CellList[i]:SetData(Item.New())
        end
    end
    local target = EleCrystal.New()
    target.item_id = data.target
    UH.SetText(self.LblName, target:Name())
    self.ItemCell:SetData(target)
    UH.SetText(self.LblCost, data.price)
    self.EleAttr:SetData(target:Element())
    self:FlushResult()
end

function WorkshopCrystal:OnClickAdd(num)
    self.make_num = self.make_num + num
    if self.make_num <= 0 then
        self.make_num = 1
        PublicPopupCtrl.Instance:Center(ErrorText[19])
        return
    end
    local can_make = true
    for k, v in pairs(self.stuff_num_list.need) do
        local have = self.stuff_num_list.have[k]
        local need = v * self.make_num
        if have < need then
            can_make = false
        end
    end
    if not can_make then
        self.make_num = self.make_num - 1
        PublicPopupCtrl.Instance:Center(ErrorText[20])
        return
    end
    self:FlushResult()
end

function WorkshopCrystal:OnClickMax()
    --计算最大个数
    local max_num = self.data:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
end

function WorkshopCrystal:FlushResult()
    UH.SetText(self.LblNum, self.make_num)
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.make_num
    UH.SetText(self.LblCost, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
end
