PetReLife = PetReLife or DeclareView("PetReLife", "pet/pet_relife")
VIEW_DECLARE_MASK(PetReLife, ViewMask.Block)
function PetReLife:PetReLife()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
end

function PetReLife:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.ReLiftTitle[1], Vector2.New(750, 375))
    self.cur_select_index = 1
end
function PetReLife:OnFlush()
    self.other_info = self.pet_data:GetPetOtherCfg()
    self.pet = self.pet_data:GetCurSelect()
    self.pet_index = self.pet.index
    self.CostItem:SetDataByItemIdWithNeed(self.other_info.relife_need, 1)
    self.CovertItem:SetDataByItemId(self.other_info.item_return_exp)
    self.level = self.other_info.min_return_exp
    self.BtnGet.Interactable = self.pet:Level() > self.other_info.min_return_exp
    UH.SetText(self.LblTip, string.format(self.lang.ExpLost, 100 - self.other_info.rate_return_exp))
    self:FlushView()
end
function PetReLife:OnClickTab(index)
    self.Board:ChangeTitle(self.lang.ReLiftTitle[index])
    self.cur_select_index = index
    self:FlushView()
end
function PetReLife:FlushView()
    if self.cur_select_index == 1 then
        self:FlushCanGetLevelOne()
    else
        self:FlushCanGet()
    end
end
-- 点击返生
function PetReLife:OnClickRelife()
    if self.pet:CheckState() then
        PetCtrl.SendPetOp(PET_OP_TYPE.Rebirth, self.pet_index)
    end
end
-- 点击转化
function PetReLife:OnClickConvert()
    if self.pet:CheckState() then
        PetCtrl.SendPetOp(PET_OP_TYPE.ConversionExp, self.pet_index, self.level)
    end
end

function PetReLife:OnClickAdd(num)
    local pet_level = self.pet:Level()
    self.level = self.level + num
    if self.level < self.other_info.min_return_exp then
        self.level = self.other_info.min_return_exp
        PublicPopupCtrl.Instance:Center(ErrorText[9])
    end
    -- 不能超过自身等级 也不能低于20级
    if self.level > pet_level - 1 then
        self.level = pet_level - 1
    end
    if self.level < self.other_info.min_return_exp then
        self.level = self.other_info.min_return_exp
    end
    self:FlushCanGet()
end

function PetReLife:FlushCanGet()
    UH.SetText(self.LblLevel, self.level)
    self.covert_cfg = self.pet_data:GetExpItemCfg(self.other_info.item_return_exp)
    if self.covert_cfg == nil then
        return
    end
    local exp = self.pet_data:GetExpTotal(self.level, self.pet:Level() - 1)
    self.count = (exp + self.pet:Exp()) * 0.8 / self.covert_cfg.add_exp
    CellFlushs.SetNum(self.CovertItem, math.floor(self.count))
end

function PetReLife:FlushCanGetLevelOne()
    self.covert_cfg = self.pet_data:GetExpItemCfg(self.other_info.item_return_exp)
    if self.covert_cfg == nil then
        return
    end
    local exp = self.pet_data:GetExpTotal(1, self.pet:Level() - 1)
    self.count = (exp + self.pet:Exp()) * 0.8 / self.covert_cfg.add_exp
    CellFlushs.SetNum(self.CovertItem, math.floor(self.count))
end

------------------------------------
PetGuiYuan = PetGuiYuan or DeclareView("PetGuiYuan", "pet/pet_guiyuan")
function PetGuiYuan:PetGuiYuan()
    self.pet_data = PetData.Instance
    self.other_info = self.pet_data:GetPetOtherCfg()
end

function PetGuiYuan:LoadCallback(data)
    self.Board:SetData(self:GetType(), Language.Pet.GuiYuan, Vector2.New(682, 421))
    local exp_list = self.pet_data:GetPetExpItemList()
    self.pet = data.data
    self.fromAttr = data.fromAttr
    self.make_price = data.price
    if self.pet == nil then
        return
    end
    self.close_callback = data.close_callback
    -- LogError("PetGuiYuan data =",data == nil)
    -- if data ~= nil then

    -- else
        UH.SetText(self.CoinNum, self.other_info.reset_price)
        UH.SetText(self.Desc, string.format(Language.Pet.GuiYuanTip, self.pet.level, self.pet:Name(), self.other_info.reset_price, self.other_info.rate_return_exp))
    -- end
    -- 获取可用于计算的经验
    local total_exp = 0
    local temp_list = TableCopy(exp_list)
    local result_data = {}
    if self.pet:Level() > 1 then
        local exp = self.pet_data:GetExpTotal(1, self.pet:Level() - 1)
        total_exp = (exp + self.pet:Exp()) * self.other_info.rate_return_exp / 100 --上一级+当前 * 返回比
        --对全部经验进行一个计算
        table.sort(temp_list, DataSorter.KeyUpperSorter("add_exp"))
        for index, value in ipairs(temp_list) do
            if index ~= #temp_list then
                value.num = math.floor(total_exp / value.add_exp)
                total_exp = total_exp - value.num * value.add_exp
            else
                value.num = math.ceil(total_exp / value.add_exp)
            end
        end
        local temp_list2 = {}
        for key, value in pairs(temp_list) do
            -- if value.item_id == self.other_info.item_return_exp then
            --     value.num = value.num + 1
            -- end
            if value.num > 0 then
                table.insert(temp_list2, value)
            end
        end
        -- 突破材料
        local breach_stuff = self.pet_data:GetBreachAllStuff(self.pet)
        if breach_stuff ~= nil then
            table.insert(temp_list2,breach_stuff)
        end
        result_data = temp_list2
    end
    self.PetPillList:SetData(result_data)
    self.Tips:SetActive(data.fromAttr == false)
end

function PetGuiYuan:OnClickCancel()
    ViewMgr:CloseView(PetGuiYuan)
end

function PetGuiYuan:OnClickOk()
    if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < self.other_info.reset_price then
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
                MallCtrl.Instance:Jump(24326)
            end)
        return
    end
    if self.fromAttr then
        PetCtrl.SendPetOp(PET_OP_TYPE.Rebirth, self.pet.index)
    else
        local param_t = {
            is_from_pet = 1,
            index = self.pet.index or 0,
            price = self.make_price,
        }
        TradeCtrl.Instance:SendAddItemReq(param_t)
    end
    ViewMgr:CloseView(PetGuiYuan)
end

function PetGuiYuan:CloseCallback()
    if self.close_callback then
        Call(self.close_callback)
    end
end
