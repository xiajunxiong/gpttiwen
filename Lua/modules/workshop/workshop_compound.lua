WorkshopCompound = WorkshopCompound or DeclareMono("WorkshopCompound", UIWFlushPanel)

-- 监听合成是否换组换组刷新
function WorkshopCompound:WorkshopCompound()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.compose_data = self.data:GetComposeData()
    self.skill_book_data = self.data:SkillBookData()
    self.lang = Language.Workshop
    self.data_cares = {
        {data = self.compose_data, func = self.FlushLeftItem, keys = {"select_type"}, init_call = true},
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false},
        {data = self.skill_book_data, func = self.FlushStuff, init_call = false},
    }
    self.stuff_num_list = {}
end

function WorkshopCompound:onFlush()
    self.BtnSkill:Trigger()
    -- local show = self.data:GetTypeShow(ComposeGroup.Stone)
    -- UH.SetText(self.LblTitle, show[self.compose_data.select_type].type_name)
end

function WorkshopCompound:OnClickAdd(num)
    self.make_num = self.make_num + num
    if self.make_num <= 0 then
        self.make_num = 1
        PublicPopupCtrl.Instance:Center(ErrorText[19])
        return
    end
    local have = self.stuff_num_list.have[1]
    local need = self.stuff_num_list.need[1] * self.make_num
    local can_make = have > need
    if not can_make then
        self.make_num = self.make_num - 1
        PublicPopupCtrl.Instance:Center(ErrorText[20])
        return
    end
    self:FlushResult()
end
function WorkshopCompound:OnClickMax()
    local max_num = self.data:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
end

function WorkshopCompound:OnClickOneKey()
    self:OnClickMax()
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_PET_CARD_EXC, self.seq, self.make_num, 0)
end
--[[ function WorkshopCompound:OnClickStone()
    -- self.compose_data.select_type = type_id
    self.BtnParent:SetActive(not self.BtnParent.activeSelf)
    if self.BtnParent.activeSelf then
        self.BtnList:SetData(self.data:GetTypeShow(ComposeGroup.Stone))
    end
end ]]
function WorkshopCompound:OnClickSkill()
    self.compose_data.select_type = ComposeGroup.Skill
end
function WorkshopCompound:OnClickCard()
    self.compose_data.select_type = ComposeGroup.Card
end
function WorkshopCompound:OnCllickSelect()
    ViewMgr:OpenView(WorkshopSelectSkill)
end
function WorkshopCompound:OnClickBtn(data)
    UH.SetText(self.LblTitle, data.type_name)
    self.compose_data.select_type = data.index
    self.BtnParent:SetActive(false)
end
--[[ function WorkshopCompound:OnClickMine()
self.compose_data.select_type = ComposeGroup.Mine
-- 直接换成矿条列表
end ]]
-- 变的是
function WorkshopCompound:FlushLeftItem()
    local data = self.data:ComposeShowList()
    self.ItemList:SetData(data)
    self.ItemList:ClickItem(1)
end
function WorkshopCompound:OnClickCreate()
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    self.ctrl:SendCompoundReq(self.seq, self.make_num, 0)
end
function WorkshopCompound:OnClickCreate2()
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    self.EffectTool:Play(10010246)
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_PET_CARD_EXC, self.seq, self.make_num, 0)
end
function WorkshopCompound:OnClickCreateSKill()
    local total_num = self.skill_book_data.total_num
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if total_num < self.target_info.num then
        PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    self.EffectTool:Play(10010246)
    local list = self.data:SkillSelectList()
    self.ctrl:SendPetSkillSeq(self.target_info.seq, table.nums(list),list)
    self.data:ClearSkillSelect()
end
function WorkshopCompound:FlushLeftItem2()
    local data = self.data:ComposeShowList()
    self.ItemList:SetData(data)
end
function WorkshopCompound:FlushRightItem()
    self:FlushLeftItem2()
    if self.target_info then
        self:OnClickItem(self.target_info)
    end
end
function WorkshopCompound:OnClickItem(data)
    self.make_num = 1
    local item_id = 0
    self.target_info = data
    self.seq = data.seq
    self.is_not_enough_stuff = false
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}

    local item = Item.New()
    item_id = data.target
    item.item_id = item_id
    self.TargetCell:SetData(item)
    UH.SetText(self.TargetName, item:Name())
    UH.SetText(self.SkillTip, Language.Workshop.SkillBookTip[data.seq] or "")

    local new_item = Item.New()
    item_id = data["stuff_id"]
    new_item.item_id = item_id
    self.StuffCell:SetData(new_item)
    UH.SetText(self.StuffName, new_item:Name())
    UH.SetText(self.StuffNum, string.format("%d/%d", new_item:Num(), data["num"]))
    self.stuff_num_list.need[1] = data["num"]
    self.stuff_num_list.have[1] = new_item:Num()

    WorkshopData.Instance:SetSkillGroup(data.param_1, data.num)
    local flag = new_item:Num() < data["num"]
    UH.Color(self.StuffNum, flag and COLORS.Red8 or COLORS.Green3)
    self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
    self:FlushResult()
end

function WorkshopCompound:FlushResult()
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price or 0
    UH.SetText(self.LblCost, need)
    UH.SetText(self.LblCost2, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    UH.Color(self.LblCost2, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
    UH.SetText(self.LblRate, string.format(self.lang.Rate, COLORSTR.Green2, self.target_info.rate / 100))
    UH.SetText(self.LblNum, self.make_num)
end
function WorkshopCompound:FlushStuff()
    local total_num = self.skill_book_data.total_num
    UH.SetText(self.StuffNum, string.format("%d/%d", total_num, self.target_info["num"]))
    local flag = total_num < self.target_info["num"]
    UH.Color(self.StuffNum, flag and COLORS.Red8 or COLORS.Green3)
end
