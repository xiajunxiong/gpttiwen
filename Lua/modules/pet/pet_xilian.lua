PetXilianPanel = PetXilianPanel or DeclareMono("PetXilianPanel", UIWFlushPanel)

function PetXilianPanel:PetXilianPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance

    self.data_cares = {
        {data = self.pet_data.xilian_data, func = self.ShowSuggestImage, keys = {"show_suggest"}},
        {data = self.pet_data.pet_select, func = self.FlushXilianPanel, keys = {"index"}},
        {data = BagData.Instance.item_grid, func = self.FlushStuffNum, init_call = false},
        {data = self.pet_data.pet_list, func = self.FlushXilianPanel, init_call = false},
    }
    self.giftOnly = false
    self.pet_data.xilian_data.gift_only = false
    self.autoBuy = false
    UH.SetText(self.RightNoticeText, self.lang.PetXilianRightNoticeStr)
    GuideManager.Instance:RegisterGetGuideUi("BtnXilianEvent", function ()
        if self.BtnXilianObj then
            return self.BtnXilianObj, function ()
                PetCtrl.SendPetOp(PET_OP_TYPE.GuideRefined, self.pet_data:GetCurSelect().index, 1)
            end
        end
    end)
    
end

function PetXilianPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.pet_data:SetSelTab(Mod.Pet.PetXilian)
end

function PetXilianPanel:FlushXilianPanel()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    UH.SetText(self.LeftNameText, pet:Name())

    local cfg2 = Cfg.PetInfo2ById(pet:ID())
    self.Mutant:SetActive(cfg2.mutant_times >= 1)

    self:SetSpandIcon(pet)

    self.StuffInfoObj:SetActive(pet.config.quality ~= 5)
    self.ShenShouNotice:SetActive(pet.config.quality == 5)
    self.BtnXilianInteraHandler.Interactable = pet.config.quality ~= 5

    if not pet:HasXilian() then
        self.RightInfoObj:SetActive(false)
        self.RightNotice:SetActive(true)
        self.BtnKeepInteraHandler.Interactable = false
        self.RightAttrList:SetPet()
        return
    end
    self.BtnKeepInteraHandler.Interactable = true
    self.RightNotice:SetActive(false)
    self.RightInfoObj:SetActive(true)
    self:FlushRightPanel(pet)
end

function PetXilianPanel:SetSpandIcon(pet)

    local info2 = Cfg.PetInfo2ById(pet:ID())
    if info2 == nil then
        return
    end
    local item = Item.Init(info2.refresh_id)
    self.cur_stuff_info = {refresh_id = info2.refresh_id, refresh_num = info2.refresh_num}
    -- self.Icon:SetData(item)

    local refresh_name = item:QuaName(true)
    UH.SetText(self.RefreshName, refresh_name)

    self.StuffItem:SetData(item)
    self:FlushStuffNum()
end

function PetXilianPanel:FlushStuffNum()
    if self.cur_stuff_info == nil then
        if DEBUG_MODE then
            LogError("FlushStuffNum self.cur_stuff_info is nil")
        end
        return
    end
    local hasNum = Item.GetNum(self.cur_stuff_info.refresh_id)
    local colorstr = hasNum >= self.cur_stuff_info.refresh_num and COLORSTR.Green9 or COLORSTR.Red10
    local str = string.format(Language.Pet.PetXilianCastNumFormat, colorstr, hasNum, self.cur_stuff_info.refresh_num)
    UH.SetText(self.RefreshNum, str)

    self:FlushLeftPanel()
end

--右侧洗练结果
function PetXilianPanel:FlushRightPanel(pet)
    UH.SetText(self.RighttNameText, pet:Name())
    local gift_list = pet:NewGiftList()

    if gift_list[1].skill_id == 0 then
        --copy一套现有技能
        gift_list = pet:GetSkillAndGiftListNoLingWu()
    else
        --洗炼技能显示
        --主动技能列表替换至列表首位
        local pet_skill_list = pet:GetMainSkillList({})--SkillList()
        for i=#pet_skill_list,1,-1 do
            table.insert(gift_list, 1, pet_skill_list[i])
            -- table.remove(gift_list)
        end
        local pet_equip_skill = pet:GetEquipSkillList({})
        for i=1, #pet_equip_skill do
            if pet_equip_skill[i].skill_id ~= 0 then
                table.insert(gift_list,pet_equip_skill[i])
            end
        end
        local pet_yao_skill = pet:LianYaoSkillList({})
        for i=1, #pet_yao_skill do
            if pet_yao_skill[i].skill_id ~= 0 then
                table.insert(gift_list,pet_yao_skill[i])
            end
        end
    end
    self.RightSkillList:SetData(gift_list)
    self.RightAttrList:SetPet(pet)
end

--左侧当前信息
function PetXilianPanel:FlushLeftPanel()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    self.LeftAttrList:SetPet(pet)
    local gift_list = pet:GetSkillAndGiftListNoEmptyWithLingWu(true)
    self.LeftSkillList:SetData(gift_list)
end

function PetXilianPanel:ShowSuggestImage()
    if self.pet_data.xilian_data.show_suggest then
        AudioMgr:PlayEffect(AudioType.UI, "tongyongjinjie01")
    end
    
    self.SuggestImg:SetActive(self.pet_data.xilian_data.show_suggest)
    self.DeleteResultBtnObj:SetActive(not self.pet_data.xilian_data.show_suggest)
end

function PetXilianPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.GiftOnlyImgObj:SetActive(false)
    self.AutoBuyImgObj:SetActive(false)
    self.giftOnly = false
    self.pet_data.xilian_data.gift_only = false
    PetData.Instance.xilian_new_flag = {}
    PetData.Instance.xi_lian_tip_index = nil
    -- if self.delay_click ~= nil then
    --     TimeHelper:CancelTimer(self.delay_click)
    -- end
    -- self.clicked_xilian = false
end

function PetXilianPanel:OnClickButtonGiftOnly()
    self.giftOnly = not self.giftOnly
    self.pet_data.xilian_data.gift_only = self.giftOnly
    self.GiftOnlyImgObj:SetActive(self.giftOnly)
end

function PetXilianPanel:OnClickButtonAutoBuy()
    self.autoBuy = not self.autoBuy
    self.AutoBuyImgObj:SetActive(self.autoBuy)
end

function PetXilianPanel:OnClickButtonXilian()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local info2 = Cfg.PetInfo2ById(pet:ID())
    PetData.Instance.xilian_data.click_save = false
    if info2 == nil or pet.config.quality == 5 then
        return
    end
    if not self.autoBuy then
        local hasNum = Item.GetNum(info2.refresh_id)
        if hasNum < info2.refresh_num then
            if info2.refresh_id ~= 27398 then
                local num = Item.GetNum(27398)
                if num >= (info2.refresh_num - hasNum) * 3 then
                    local p1 = ColorStr(num .. "", COLORSTR.Yellow6)
                    local p2 = ColorStr(math.floor(num / 3) .. "", COLORSTR.Yellow6)
                    WorkshopData.Instance:QuickComposeItem(info2.refresh_id)
                    PublicPopupCtrl.Instance:AlertTip(string.format(Language.Pet.XiLianComTip, p1, p2),
                    function ()
                        ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
                    end)
                    return
                end
            end
            MallData.Instance:SetJumpItemNum(info2.refresh_num-hasNum)
            PublicPopupCtrl.Instance:Center(Language.Pet.PetXilianNotEnoughStuff)
            MarketData.Instance:SetJumpIndex(info2.refresh_id)
            ViewMgr:OpenViewByKey(Mod.Mall.Market)
            return
        end
    end
    -- self.EffectTool:Play(10010701)
    -- if pet:HasXilian() then
        -- self.EffectTool:Play(10010702)
    -- end
    -- self.clicked_xilian = true

    local hasNum = Item.GetNum(info2.refresh_id)
    if self.autoBuy and hasNum < info2.refresh_num then
        local need_num = info2.refresh_num - hasNum
        local price = MarketData.Instance:GetItemPrice(info2.refresh_id) * need_num
        if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin, price) then
            PublicPopupCtrl.Instance:Center(Language.PetStuffBuy.Tip1)
            return
        end

        if PetData.Instance.xi_lian_tip_index ~= pet.index then
            local num_str = DataHelper.ConsumeNum(hasNum, info2.refresh_num, true)
            local item = Item.Create({item_id = info2.refresh_id, num = num_str})
            local price_str = ColorStr(string.format(Language.PetStuffBuy.Text1,price), COLORSTR.Yellow6)
            local context = string.format(Language.PetStuffBuy.Text2,price_str,item:QuaName())
            local info = {
                context = context,
                item = item,
                r_func = function ()
                    PetCtrl.SendPetOp(PET_OP_TYPE.Refined, pet.index, self.giftOnly and 0 or 1, self.autoBuy and 1 or 0)
                end,
                toggle_func = function (toggle_value)
                    if toggle_value then
                        PetData.Instance.xi_lian_tip_index = pet.index
                    end
                end
            }
            ViewMgr:OpenView(PetStuffBuyDialog, info)
        else
            self.EffectTool:Play(10010702)
            PetCtrl.SendPetOp(PET_OP_TYPE.Refined, pet.index, self.giftOnly and 0 or 1, self.autoBuy and 1 or 0)
        end
    else
        PetCtrl.SendPetOp(PET_OP_TYPE.Refined, pet.index, self.giftOnly and 0 or 1, self.autoBuy and 1 or 0)
    end

    self:CheckItemDialog()
end

function PetXilianPanel:CheckItemDialog()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    --if not self.is_xilian_tip_end and pet:Quality() >= 3 and pet:Quality() <= 4 and not pet:IsMutant() then
    if not self.is_xilian_tip_end then
        if not self.xilian_timestamp then
            self.xilian_timestamp = {}
        end
        table.insert(self.xilian_timestamp, os.time())
        local len = #self.xilian_timestamp
        local need_times = Config.pet_cfg_auto.other[1].continuou_times
        if len >= need_times then
            local time = self.xilian_timestamp[len] - self.xilian_timestamp[len + 1 - need_times]
            if time <= Config.pet_cfg_auto.other[1].xilian_time and not DialogTipsData:TodayIsTrue(DTNewTodayNotRemind.PetXiLianTip) then
                --self.is_xilian_tip_end = true
                ViewMgr:OpenView(PetXiLianItemTip)
            end
        end
    end
end

function PetXilianPanel:OnClickButtonKeep()
    local pet = self.pet_data:GetCurSelect()
    if pet and pet:HasXilian() then
        self.pet_data.xilian_data.click_save = true
        PetCtrl.SendPetOp(PET_OP_TYPE.RefinedSave, pet.index)
        self.EffectTool:Play(10010322,self.LeftAttrList.SliderList[1].gameObject)
        self.EffectTool:Play(10010322,self.LeftAttrList.SliderList[2].gameObject)
        self.EffectTool:Play(10010322,self.LeftAttrList.SliderList[3].gameObject)
        self.EffectTool:Play(10010322,self.LeftAttrList.SliderList[4].gameObject)
        self.EffectTool:Play(10010322,self.LeftAttrList.SliderList[5].gameObject)
    end
end

function PetXilianPanel:OnClickGift(cell)
    -- local data = cell:GetData()
    -- if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
    --     self.pet_data:SetClickSkill(data)
    --     ViewMgr:OpenView(PetSkillTip)
    -- elseif data.show_lingwu then
    --     PetLingwuSkillTip.SetSlotIndex(data.index)
    --     ViewMgr:OpenView(PetLingwuSkillTip)
    -- end
    PetGiftItem.CommonClickFunc(cell)
end

function PetXilianPanel:OnClickDeleteResult()
    local pet = self.pet_data:GetCurSelect()
    if pet then
        PetCtrl.SendPetOp(PET_OP_TYPE.RefinedGiveUp, pet.index)
    end
end

-------------------------------------------------------------------
PetXilianAttrList = PetXilianAttrList or DeclareMono("PetXilianAttrList", UIWFlushPanel)
--MaxLength 进度条长度
--ColorIndex 使用颜色组 1是浅色 JinDuTiaoDi JinDuTiao_JianGe2  2是深色 JinDuTiao_1 JinDuTiao_JianGe
--MaxHeight 进度条高度
--BgList
--SideList
local color_list = {
    [1] = {COLORSTR.Red12, COLORSTR.Yellow13, COLORS.Red12, COLORS.Yellow13},
    [2] = {COLORSTR.Yellow22, COLORSTR.Yellow5, COLORS.Yellow22, COLORS.Yellow5}
}
function PetXilianAttrList:SetPet(pet)
    if pet == nil then
        self.pet = nil
        return
    end
    self.MaxLength = self.MaxLength == nil and 185 or self.MaxLength
    self.MaxHeight = self.MaxHeight == nil and 22 or self.MaxHeight
    self.ColorIndex = self.ColorIndex == 2 and 2 or 1
    local total_growth_list = pet:TotalGrowthList()
    local config_growth_list = pet:ConfigGrowthList()
    local total_xilian_growth_list = pet:TotalXilianGrowthList()
    local suggest_count = 0
    local suggest = true
    local full_growth = true
    if self.pet == nil or self.pet.index ~= PetData.Instance:GetSelectPetIndex() then
        full_growth = false
    end
    for k, v in pairs(config_growth_list) do
        local index = PetData.GetZZIndex(k)
        self.ZZList[index].sizeDelta = Vector2.New(self.MaxLength, self.MaxHeight)
        self.SliderList[index].value = (6 - v + total_xilian_growth_list[k]) / 6
        UH.SetText(
            self.ValueList[index],
            string.format(
                Language.Pet.PetZZ2,
                index == pet:MainAttr() and color_list[self.ColorIndex][1] or color_list[self.ColorIndex][2],
                tostring(total_xilian_growth_list[k]),
                string.format( "%.1f", tonumber(v))
            )
        )
        
        UH.SetText(
            self.ValueList[index],
            string.format(
                Language.Pet.PetZZ2,
                index == pet:MainAttr() and color_list[self.ColorIndex][1] or
                    color_list[self.ColorIndex][2],
                tostring(total_xilian_growth_list[k]),
                tostring(v)
            )
        )
        UH.SpriteName(self.BgList[index], (self.ColorIndex == 1) and "JinDuTiaoDi" or "JinDuTiao_1")
        local sidelist = self["SideList"..index]
        for i = 1, sidelist:Length() do
            UH.SpriteName(sidelist[i], (self.ColorIndex == 1) and "JinDuTiao_JianGe2" or "JinDuTiao_JianGe")
        end
        local spName = nil
        if total_xilian_growth_list[k] - total_growth_list[k] > 0 then
            spName = "greenArrow"
        else
            if total_xilian_growth_list[k] - total_growth_list[k] < 0 then
                spName = "redArrow"
                if index == pet:MainAttr() then
                    suggest = false
                end
            end
        end
        if full_growth then
            full_growth = total_xilian_growth_list[k] == v
        end
        -- if index ~= pet:MainAttr() then
            suggest_count = suggest_count + total_xilian_growth_list[k] - total_growth_list[k]
        -- end
        UH.SpriteName(self.ArrowList[index], spName)
        PetData.Instance:SetXilianDataSuggest(suggest_count > 0 and suggest)        
        UH.Color(
            self.TitleList[index],
            index == pet:MainAttr() and color_list[self.ColorIndex or 1][3] or color_list[self.ColorIndex or 1][4]
        )
    end
    if full_growth then
        AudioMgr:PlayEffect(AudioType.UI,"chongwuqianghua")
    end
    self.pet = pet
end



PetXiLianItemTip = PetXiLianItemTip or DeclareView("PetXiLianItemTip", "pet/pet_xilian_item_tip")
function PetXiLianItemTip:PetXiLianItemTip()
    self.Board:SetData(self:GetType(),Language.Pet.XiLianItemTitle, Vector2.New(628, 324))
    self.item_id = 26502
end

function PetXiLianItemTip:LoadCallback()
    local item = Item.Create({item_id = self.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
    UH.SetText(self.Desc, string.format(Language.Pet.XiLianItemTip,item:Name()))
    self.Toggle.isOn = DialogTipsData:TodayIsTrue(DTNewTodayNotRemind.PetXiLianTip)
end

function PetXiLianItemTip:CloseCallback()
    if self.Toggle.isOn then
        DialogTipsData:SetTodayRemind(DTNewTodayNotRemind.PetXiLianTip)
    end
end

function PetXiLianItemTip:OnJumpClick()
    ViewMgr:OpenViewByKey(Mod.Reward.GodToken)
    ViewMgr:CloseView(PetXiLianItemTip)
end