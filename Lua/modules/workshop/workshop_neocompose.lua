WorkshopNeoCompose = WorkshopNeoCompose or DeclareMono("WorkshopNeoCompose", UIWFlushPanel)
function WorkshopNeoCompose:WorkshopNeoCompose()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.skill_book_data = self.data:SkillBookData()
    self.lang = Language.Workshop
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false},
    }
    self.stuff_num_list = {}
    self.pos2 = Vector3.New(176, -16, 0)
    self.pos1 = Vector3.New(316, -58, 0)

    if GuideManager.Instance:IsGuide() then 
        self.data:QuickComposeItem(40003)
    end 
end 

function WorkshopNeoCompose:onFlush()

    local data = self.data:GetNeoComposeAccorCfg(true)
    self.NeoComposeAcc:SetData(data,true)

    self:FlushJump()
end 

function WorkshopNeoCompose:FlushJump()
    local data = self.data:GetNeoComposeAccorCfg(true)
    local mark_pos = self.data:GetLockNeoCompose()
    if mark_pos ~= nil then 
        if data[mark_pos.parent]~= nil and data[mark_pos.parent].children ~= nil
            and data[mark_pos.parent].children[mark_pos.child] ~= nil then 
            self.NeoComposeAcc:ClickChild(data[mark_pos.parent].children[mark_pos.child])
            if #data[mark_pos.parent].children > 5 then 
                self.NeoComposeAcc:JumpVerticalIndex(mark_pos.child,96,mark_pos.parent,56)
            end 
        else 
            LogError("check your lock please!",mark_pos.parent,mark_pos.child)
        end 
    end 
end 

function WorkshopNeoCompose:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.btn_timer ~= nil then
        TimeHelper:CancelTimer(self.btn_timer)
        self.btn_timer = nil
    end

    self.data:ClearQuickComposeType()
end 

function WorkshopNeoCompose:FlushRightItem()
    if self.target_info then
        self:OnClickItem(self.target_info)
    end
end

-- 左侧 区版
function WorkshopNeoCompose:OnClickItem(data)
    -- LogError("?OnClickItem?",data)
    self.target_info = data

    if data.group == ComposeGroup.EleCrystal or data.group == ComposeGroup.Key or data.group == ComposeGroup.DragonBall then 
        -- self:FlushPartCrystal(data)
        self.CrystalPanel:OnFlush()
    elseif data.group == ComposeGroup.Card or data.group == ComposeGroup.Skill or data.group == ComposeGroup.PetFlush or data.group == ComposeGroup.MarbasRefine then 
        self.ComposePanel:OnFlush()
        -- self:FlushCompose(data)
    elseif data.group == ComposeGroup.Life  then 
        self.LifePanel:OnFlush()
    elseif data.group == ComposeGroup.Map  then 
        self.TreasureMapPanel:OnFlush()
    end 
    
    self.PartCrystal:SetActive( data.group == ComposeGroup.EleCrystal or data.group == ComposeGroup.Key or data.group == ComposeGroup.DragonBall)
    self.PartCompose:SetActive( data.gdunroup == ComposeGroup.Card or data.group == ComposeGroup.Skill or data.group == ComposeGroup.PetFlush or data.group == ComposeGroup.MarbasRefine)
    self.PartLife:SetActive( data.group == ComposeGroup.Life )
    self.PartTreasureMap:SetActive( data.group == ComposeGroup.Map )

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function WorkshopNeoCompose:OnClickBtn(data)
    UH.SetText(self.LblTitle, data.type_name)
    self.data:NeoComposeData().select_type = data.index
    self.BtnParent:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function WorkshopNeoCompose:PlayBtnEffect()
    if self.btn_timer ~= nil then
        TimeHelper:CancelTimer(self.btn_timer)
        self.btn_timer = nil
    end
    self.btn_timer = TimeHelper:AddDelayTimer(function () 
        self.btn_timer = nil
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
    end, 0.2)
end

NeoComposeCrystal = NeoComposeCrystal or DeclareMono("NeoComposeCrystal", UIWFlushPanel)
function NeoComposeCrystal:NeoComposeCrystal()
    self.stuff_num_list = {}
    self.pos2 = Vector3.New(176, -16, 0)
    self.pos1 = Vector3.New(316, -58, 0)
end 

function NeoComposeCrystal:OnFlush()
    -- body
    local data = self.MainPanel.target_info
    self.target_info = data
    self.make_num = 1
    local item_id = 0
    local item = nil
    self.stuff_num_list.item = {}
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}
    self.stuff_tips = nil 
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
        UH.SetText(self.NameStuffOne, item:Name())
        UH.SetText(self.NumStuffOne, string.format("%d/%d", item:Num(), data.stuff_num_1))
        local flag = item:Num() < data.stuff_num_1
        UH.Color(self.NumStuffOne, flag and COLORS.Red8 or COLORS.Green3)
        item.vo.jump_need_num = (math.floor(item:Num()%100) == 0 and item:Num() > 0) and 1 or (100 - math.floor(item:Num()%100))
        self.CellStuffOne:SetData(item)
    end
    for i = 1, 2 do
        item_id = data["stuff_id_" .. i]
        if item_id > 0 then
            item = Item.New()
            item.item_id = item_id
            item.vo.jump_need_num = data["stuff_num_" .. i] - item:Num()
            UH.SetText(self.NameStuffTwo[i], item:Name())
            UH.SetText(self.NumStuffTwo[i], string.format("%d/%d", item:Num(), data["stuff_num_" .. i]))
            self.CellStuffTwoList[i]:SetData(item)
            self.stuff_num_list.item[i] = item_id
            self.stuff_num_list.need[i] = data["stuff_num_" .. i]
            self.stuff_num_list.have[i] = item:Num()
            local flag = item:Num() < data["stuff_num_" .. i]
            UH.Color(self.NumStuffTwo[i], flag and COLORS.Red8 or COLORS.Green3)
            self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
        else
            UH.SetText(self.NameStuffTwo[i], "")
            UH.SetText(self.NumStuffTwo[i], "")
            self.CellStuffTwoList[i]:SetData(Item.New())
        end
    end

    local target = Item.New()
    target.item_id = data.target
    UH.SetText(self.LblName, target:Name())
    self.ItemCell:SetData(target)
    UH.SetText(self.LblCostCrystal, data.price)
    self.CompsoeBtnCost:SetActive(data.price > 0)
    self.CompsoeBtnFree:SetActive(data.price == 0)
    if target:IsCrystal()then 
        self.EleAttrObj:SetActive(true)
        self.EleAttr:SetData(EleCrystal.GetElement(target.item_id))
        self.EleNameObj:SetActive(false)
    else 
        self.EleAttrObj:SetActive(false)
        self.EleNameObj:SetActive(true)
        self.EleName.text = target:Name()
    end 
    self:FlushResult()
end

function NeoComposeCrystal:FlushResult()
    UH.SetText(self.LblNumCrystal, self.make_num)
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.make_num
    UH.SetText(self.LblCostCrystal, need)
    UH.Color(self.LblCostCrystal, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
end

-- 按钮组
function NeoComposeCrystal:OnClickCreateCrystal()
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        local plus_list = {}
        for i = 1,2 do 
            if self.stuff_num_list.need[i] and self.stuff_num_list.have[i] and self.stuff_num_list.need[i] > self.stuff_num_list.have[i] then 
                local need = self.stuff_num_list.need[i] - self.stuff_num_list.have[i] 
                
                -- ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.stuff_num_list.item[i], jump_need_num = need})})
                -- BagData.Instance:SetGetWaySweepMark({need = need})

                local item_p_list = {[1] = Item.Create({item_id = self.stuff_num_list.item[i], num = need})}
                -- 暂定龙珠不能购买
                if not LiveData.Instance.live_quick_buy_flag and self.target_info.group ~= 16 then
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], Item.GetName(self.stuff_num_list.item[i]), need))
                    ViewMgr:OpenView(LiveQuickBuy, {item_list = item_p_list})
                    break
                elseif self.target_info.group == 16 then
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], Item.GetName(self.stuff_num_list.item[i]), need))
                    break
                else 
                    table.insert(plus_list,item_p_list[1])
                end 
            end 
        end 

        if LiveData.Instance.live_quick_buy_flag then
            ViewMgr:OpenView(LiveQuickBuy, {item_list = plus_list,wait_quick = true})
        end 
        -- PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    if self.need_two then 
        self.EffectTool:Play(10010261)
    else 
        self.EffectTool:Play(10010260,self.CrystalEffPos)
    end 

    if GuideManager.Instance:IsGuide() then 
        GuideManager.Instance:ForceStop()
    end
    WorkshopCtrl.Instance:SendCompoundReq(self.seq, self.make_num, 0)

    self.MainPanel:PlayBtnEffect()
end

function NeoComposeCrystal:OnClickMax()
    --计算最大个数
    local max_num = WorkshopData.Instance:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeCrystal:OnClickAdd(num)
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
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

NeoComposeCompose = NeoComposeCompose or DeclareMono("NeoComposeCompose", UIWFlushPanel)
function NeoComposeCompose:NeoComposeCompose()
    self.stuff_num_list = {}
    self.skill_book_data = WorkshopData.Instance:SkillBookData()
    self.data_cares = {
        {data = self.skill_book_data, func = self.FlushStuff, init_call = false},
    }
end 

function NeoComposeCompose:OnFlush()
    local data = self.MainPanel.target_info
    self.make_num = 1
    local item_id = 0
    self.target_info = data
    self.seq = data.seq
    self.is_not_enough_stuff = false
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}
    self.stuff_num_list.item = {}

    local item = Item.New()
    item_id = data.target
    item.item_id = item_id
    self.TargetCell:SetData(item)
    UH.SetText(self.TargetName, item:Name())
    UH.SetText(self.SkillTip, Language.Workshop.SkillBookTip[data.seq] or "")

    local new_item = Item.New()
    item_id = data["stuff_id"]
    new_item.item_id = item_id
    new_item.vo.jump_need_num = data["num"] - new_item:Num()
    self.StuffCell:SetData(new_item)
    UH.SetText(self.StuffName, new_item:Name())
    UH.SetText(self.StuffNum, string.format("%d/%d", new_item:Num(), data["num"]))
    self.stuff_num_list.need[1] = data["num"]
    self.stuff_num_list.have[1] = new_item:Num()
    self.stuff_num_list.item[1] = new_item

    WorkshopData.Instance:SetSkillGroup(data.param_1, data.num)
    local flag = new_item:Num() < data["num"]
    UH.Color(self.StuffNum, flag and COLORS.Red8 or COLORS.Green3)
    self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff

    local lock = data.level and data.level > RoleData.Instance:GetRoleLevel()

    self.CardMade.Interactable = not lock
    self.CardOneKeyMade.Interactable = not lock

    self.ItemParent:SetActive(data.group == ComposeGroup.Card or data.group == ComposeGroup.Key or data.group == ComposeGroup.PetFlush or data.group == ComposeGroup.MarbasRefine)
    self.SkillParent:SetActive(data.group == ComposeGroup.Skill )

    self:FlushResult() 
end

function NeoComposeCompose:FlushResult()
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price or 0
    -- UH.SetText(self.LblCostCard, need )
    UH.Color(self.LblCostCard, coin < need and COLORS.Red8 or COLORS.Green1)
    UH.Color(self.LblCostSkill , coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
    UH.SetText(self.LblRate, string.format(Language.Workshop.Rate, COLORSTR.Green2, self.target_info.rate / 100))
    UH.SetText(self.LblNumCard, self.make_num)
    UH.SetText(self.LblCostCard, self.make_num > 0 and need * self.make_num or need )
    UH.SetText(self.LblCostSkill , need)
    self.RateObj:SetActive(self.target_info.rate < 10000)
    if self.target_info.rate < 10000 then
        self.FailCardTips:SetActive(self.target_info.group == ComposeGroup.Card)
        self.FailTips:SetActive(self.target_info.group ~= ComposeGroup.Card)

        if self.target_info.group == ComposeGroup.Card then 
            if self.target_info.target == CommonItemId.SilverCard then 
                UH.SetText(self.FailCardTipslbl,Language.Workshop.ComposeCardSilver)
            else 
                UH.SetText(self.FailCardTipslbl,Language.Workshop.ComposeCardGold)
            end 
        end 
    else 
        self.FailTips:SetActive(false)
        self.FailCardTips:SetActive(false)
    end 
end

function NeoComposeCompose:FlushStuff()
    local total_num = self.skill_book_data.total_num
    UH.SetText(self.StuffNum, string.format("%d/%d", total_num, self.target_info["num"]))
    local flag = total_num < self.target_info["num"]
    UH.Color(self.StuffNum, flag and COLORS.Red8 or COLORS.Green3)
end

-- 方法扩展 现在还支持钥匙和洗髓露
function NeoComposeCompose:OnClickCreateCard()
    local lock = self.target_info.level and self.target_info.level > RoleData.Instance:GetRoleLevel()
    if lock then 
        PublicPopupCtrl.Instance:Center(ErrorText[23])
        return
    end 

    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_stuff then
        if self.stuff_num_list.need[1] > self.stuff_num_list.have[1] then 
            local need = self.stuff_num_list.need[1] - self.stuff_num_list.have[1] 
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], self.stuff_num_list.item[1]:Name(), need))
            ViewMgr:OpenView(GetWayView, {item = self.stuff_num_list.item[1]})
            BagData.Instance:SetGetWaySweepMark({need = need})
        end 
        -- PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end
    self.EffectTool:Play(10010260,self.ComposeEffPos)
    WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_PET_CARD_EXC, self.seq, self.make_num, 0)

    self.MainPanel:PlayBtnEffect()
end

-- 方法扩展 现在还支持钥匙和洗髓露
function NeoComposeCompose:OnClickOneKeyCard()
    local lock = self.target_info.level and self.target_info.level > RoleData.Instance:GetRoleLevel()
    if lock then 
        PublicPopupCtrl.Instance:Center(ErrorText[23])
        return
    end 

    if self.is_not_enough_stuff then
        if self.stuff_num_list.need[1] > self.stuff_num_list.have[1] then 
            local need = self.stuff_num_list.need[1] - self.stuff_num_list.have[1] 
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], self.stuff_num_list.item[1]:Name(), need))
            ViewMgr:OpenView(GetWayView, {item = self.stuff_num_list.item[1]})
            BagData.Instance:SetGetWaySweepMark({need = need})
        end 
        -- PublicPopupCtrl.Instance:Center(ErrorText[22])
        return
    end

    self:OnClickMax()
    self.EffectTool:Play(10010260,self.ComposeEffPos)
    WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_PET_CARD_EXC,  self.seq, self.make_num, 0)

    self.MainPanel:PlayBtnEffect()
end

function NeoComposeCompose:OnClickCreateSKill()
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
    local list = WorkshopData.Instance:SkillSelectList()
    self.ctrl:SendPetSkillSeq(self.target_info.seq, table.nums(list),list)
    WorkshopData.Instance:ClearSkillSelect()
end

function NeoComposeCompose:OnClickMax()
    --计算最大个数
    local max_num = WorkshopData.Instance:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeCompose:OnClickAdd(num)
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
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeCompose:OnClickSelectSkill()
    ViewMgr:OpenView(WorkshopSelectSkill)
end

NeoComposeLife = NeoComposeLife or DeclareMono("NeoComposeLife", UIWFlushPanel)
function NeoComposeLife:NeoComposeLife()
    self.stuff_num_list = {}
end 

function NeoComposeLife:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
end 

function NeoComposeLife:OnFlush()
    local data = self.MainPanel.target_info
    self.make_num = 1 -- self.make_num or 1
    self.stuff_num_list.need = {}
    self.stuff_num_list.have = {}
    self.target_info = data
    local item = nil
    local item_id = 0
    local flag_complete = true

    for i = 1,4 do 
        local vo = {}
        local _item = Item.New({item_id = self.target_info["stuff_id_"..i] or 0}) 
        vo.item_info = _item
        vo.active_seq = self.target_info.active_seq
        self.MaterialList[i]:SetData(vo)

        if data["stuff_id_" .. i] > 0 then 
            self.stuff_num_list.need[i] = data["stuff_num_" .. i]
            self.stuff_num_list.have[i] = _item:Num()
        end 

    end 
    
    local is_learned = data.active_seq == 0 or( data.active_seq > 0 and WorkshopData.Instance:GetAcitveInfo(data.active_seq))
    self.Active:SetActive(is_learned)
    self.NoActive:SetActive(not is_learned)
    if is_learned and flag_complete then 
        self.CanDoAnim:Play()
    end 
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function NeoComposeLife:OnClickGet()
    MainOtherCtrl.Instance:GetWayView({item = Item.Init(self.target_info.activation)})
    -- ItemInfoCtrl.Instance:ItemInfoView({show_ways = true,item_list = {Item.Init(self.target_info.activation)}})
end

function NeoComposeLife:OnClickAdd(num)
    self.make_num = self.make_num + num
    if self.make_num <= 0 then
        self.make_num = 1
        PublicPopupCtrl.Instance:Center(Language.Workshop.ForgeMulitLimitedZero)
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
        PublicPopupCtrl.Instance:Center(Language.Workshop.ForgeMulitLimited)
        return
    end
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeLife:OnClickAddDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickAdd(num)
        end
	end,0.1)
end

function NeoComposeLife:OnClickAddUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function NeoComposeLife:OnClickMax()
    --计算最大个数
    local max_num = WorkshopData.Instance:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeLife:FlushResult()
    UH.SetText(self.LblNum, self.make_num)
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local need = self.target_info.price * self.make_num
    UH.SetText(self.LblCost, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need

    self.BtnMoney:SetActive(need > 0)
    self.BtnFree:SetActive(not (need > 0) )

    for i =1 ,4 do 
        local need_num = self.target_info["stuff_num_"..i]
        self.MaterialList[i]:FlushStuffNum(need_num,self.make_num,true)
    end 
end

function NeoComposeLife:OnClickToMake()
    -- 钱不够 材料不足
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if WorkshopData.Instance:CalcMaxNum(self.stuff_num_list.have, self.stuff_num_list.need) == 0 then
        -- PublicPopupCtrl.Instance:Center(ErrorText[22])
        local operate = nil
        local lack_num = 0
        for i = 1 ,5 do 
            if self.target_info["stuff_id_"..i] > 0 and Item.GetNum(self.target_info["stuff_id_"..i]) < self.target_info["stuff_num_"..i]  then 
                operate = Item.Create({item_id = self.target_info["stuff_id_"..i]})
                lack_num = self.make_num * self.target_info["stuff_num_"..i] - Item.GetNum(self.target_info["stuff_id_"..i])

                PublicPopupCtrl.Instance:Center(string.format(ErrorText[5],Item.GetName(self.target_info["stuff_id_"..i]),
                lack_num))
                break
            end 
        end  

        if operate ~=nil then 
            operate.vo.jump_need_num = lack_num
            MainOtherCtrl.Instance:GetWayView({item = operate}) 
        end 
        return
    end

    self:ShowCreateEffectEffect()
end

function NeoComposeLife:OnClickStuff(index)
    if self.target_info == nil then
        return
    end

    local data = Item.New()
    data.item_id = self.target_info["stuff_id_" .. index]
    if data.item_id == 0 then
        return
    end
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function NeoComposeLife:OnClickTarget()
    local data = Item.New()
    data.item_id = self.target_info.target
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function NeoComposeLife:ShowCreateEffectEffect()
    self.WaitNabe:SetActive(false)
    self.NabingProg:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
    
    self.EffectTool:Play("10010285")
    self.prog_step = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdateCreateProgEffect, self), 0.1)
    self.ProtectArea:SetActive(true)
end

function NeoComposeLife:UpdateCreateProgEffect()
    if self.ProgSlider == nil then return end 
    local value_prog = self.ProgSlider.value
    local cur_prog = math.floor(value_prog * 100)
    if self.ProText~= nil then 
        UH.SetText(self.ProText,cur_prog.."%")
    end 
end

function NeoComposeLife:CanelCreateEffect()
    if self.prog_step then 
        TimeHelper:CancelTimer(self.prog_step)
    end 
    self.WaitNabe:SetActive(true)
    self.NabingProg:SetActive(false)

    WorkshopCtrl.Instance:SendCompoundReq(self.target_info.seq, self.make_num, 0)
    
    self.ProtectArea:SetActive(false)
end


NeoComposeTreasureMap = NeoComposeTreasureMap or DeclareMono("NeoComposeTreasureMap", UIWFlushPanel)
function NeoComposeTreasureMap:NeoComposeTreasureMap()
    self.stuff_num_list = {}
end 

function NeoComposeTreasureMap:OnFlush()
   -- body
   local data = self.MainPanel.target_info
   self.target_info = data
   self.make_num = 1
   self.is_not_enough_stuff = false

   local item = Item.Init(data.target)
   self.ItemCell:SetData(item)
   UH.SetText(self.LblName, item:Name())
   UH.SetText(self.LblNameShow,item:QuaName())

   for  i = 1,self.stuffs:Length() do 
    local flag = data["stuff_id_"..i] > 0 
    self.stuffs[i].gameObject:SetActive(flag)
    if flag then 
        local stuff_info = Item.Create({item_id = data["stuff_id_"..i]})
        stuff_info.vo.jump_need_num = data["stuff_num_"..i] - stuff_info:Num()
        stuff_info.vo.need_num = data["stuff_num_"..i]
        self.stuffs[i]:SetData(stuff_info)

        local num_flag = stuff_info:Num() < data["stuff_num_" .. i]
        self.is_not_enough_stuff = num_flag and num_flag or self.is_not_enough_stuff
    end 
   end 

   self.MainPanel.block:SetActive(false)
   self:FlushResult()


    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    end
    local num = self.is_not_enough_stuff and 0 or 1
    self.red_point:SetNum(num)
   
end

function NeoComposeTreasureMap:FlushResult()
    UH.SetText(self.LblNumCrystal, self.make_num)
    -- local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    -- local need = self.target_info.price * self.make_num
    -- UH.SetText(self.LblCostCrystal, need)
    -- UH.Color(self.LblCostCrystal, coin < need and COLORS.Red8 or COLORS.Green1)
    -- self.is_not_enough_coin = coin < need
end

function NeoComposeTreasureMap:OnClickMax()
    --计算最大个数
    local haves_num = {}
    local needs_num = {}
    for i = 1,self.stuffs:Length() do 
        haves_num[i] = self.stuffs[i]:GetData():Num()
        needs_num[i] = self.stuffs[i]:GetData().vo.need_num
    end 

    local max_num = WorkshopData.Instance:CalcMaxNum(haves_num, needs_num)
    self.make_num = max_num == 0 and 1 or max_num
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeTreasureMap:OnClickAdd(num)
    self.make_num = self.make_num + num
    if self.make_num <= 0 then
        self.make_num = 1
        PublicPopupCtrl.Instance:Center(ErrorText[19])
        return
    end
    local can_make = true
    for i = 1,self.stuffs:Length() do 
        if not self.stuffs[i]:IsEnough(self.make_num) then 
            can_make = false
        end 
    end

    if not can_make then
        self.make_num = self.make_num - 1
        PublicPopupCtrl.Instance:Center(ErrorText[20])
        return
    end
    self:FlushResult()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function NeoComposeTreasureMap:OnClickCreateMap()
    if not BagData.Instance:TryFindEmptyInBag(self.target_info.target) then 
        PublicPopupCtrl.Instance:Center(ErrorText[132])
        return 
    end    

    if not BagData.Instance:TryFindEmptyInBagByTypeNum(nil,self.make_num) then 
        PublicPopupCtrl.Instance:Center(ErrorText[132])
        return 
    end 

    if self.is_not_enough_stuff then
        local plus_list = {}
        for i = 1,self.stuffs:Length() do 
            if not self.stuffs[i]:IsEnough() then 
                local need = self.stuffs[i]:GetData().vo.need_num - self.stuffs[i]:GetData():Num() 
                local item_p_list = {[1] = Item.Create({item_id = self.stuffs[i]:GetData().item_id, num = need})}
                if not LiveData.Instance.live_quick_buy_flag then
                    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], Item.GetName( self.stuffs[i]:GetData().item_id), need))
                    ViewMgr:OpenView(LiveQuickBuy, {item_list = item_p_list})
                    break
                else
                    table.insert(plus_list,item_p_list[1])
                end 
            end 
        end 

        if LiveData.Instance.live_quick_buy_flag then
            ViewMgr:OpenView(LiveQuickBuy, {item_list = plus_list,wait_quick = true})
        end 
        return
    end

    self.EffectTool:Play(3167020,self.eff_pos)

    if GuideManager.Instance:IsGuide() then 
        GuideManager.Instance:ForceStop()
    end
    
    self.MainPanel.block:SetActive(true)
    -- self.MainPanel:PlayBtnEffect()
end

function NeoComposeTreasureMap:OnEffectComplete()
    WorkshopCtrl.Instance:SendCompoundReq(self.target_info.seq, self.make_num, 0)    
end

NeoComposeStuffItem = NeoComposeStuffItem or DeclareMono("NeoComposeStuffItem",UIWidgetBaseItem)
function NeoComposeStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name,self.data:QuaName())
    UH.SetText(self.num,string.format("%d/%d", self.data:Num(), self.data.vo.need_num))
    local flag = self.data:Num() < self.data.vo.need_num
    UH.Color(self.num, flag and COLORS.Red8 or COLORS.Green3)

    self.call:SetData(self.data)
end

function NeoComposeStuffItem:IsEnough(make_num)
    local mult = make_num == nil and 1 or make_num
    local need = self.data.vo.need_num * mult
    local have = self.data:Num()

    return have >= need
end

--------------------------------------------
NeoComposeParentItem = NeoComposeParentItem or DeclareMono("NeoComposeParentItem",UIWidgetBaseItem)
function NeoComposeParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)
    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
end

function NeoComposeParentItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function NeoComposeParentItem:CheckRedPoint()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(158,20,0))
    end

    local num = 0
    for k,v in pairs(self.data.children) do 
        num = num + ((WorkshopData.Instance:CheckCanNeoCompose(v)) and 1 or 0)
    end 

    self.red_point:SetNum(num > 0 and 1 or 0)
end

function NeoComposeParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

-----------
NeoComposeItem = NeoComposeItem or DeclareMono("NeoComposeItem",UIWidgetBaseItem)
function NeoComposeItem:NeoComposeItem()
    self.workshop_data = WorkshopData.Instance
end

function NeoComposeItem:SetData(data)
    local level = RoleData.Instance:GetRoleLevel()
    local limited_level = data.level ~= nil and data.level or data.limit_level
    local item = Item.Create({item_id = data.target})
    -- 添加锁定状态
    data.is_lock = level < limited_level
    UH.SetText(self.LblName, item:Name())
    UH.SetIcon(self.Icon, item:IconId())
    self.Interactable.Interactable = not data.is_lock
    UH.SetText(self.LblType, item:TypeDesc())
    if data.group == ComposeGroup.Life then
        data.is_lock = not self.workshop_data:GetAcitveInfo(data.active_seq)
        if data.activation == 0 then data.is_lock = false end
        self.ZheZhao:SetActive(data.is_lock)
    else 
        self.ZheZhao:SetActive(data.is_lock)
    end 
    UH.Color(self.LblName, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    UH.Color(self.LblLevel, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    UH.Color(self.LblType, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    self.Interactable.Interactable = not data.is_lock
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
end
function NeoComposeItem:CheckRedPoint()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(158,38,0))
    end
    local num = self.workshop_data:CheckCanNeoCompose(self.data) and 1 or 0
    self.red_point:SetNum(num)
end
function NeoComposeItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end
function NeoComposeItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle --[[ and self.Toggle.isActiveAndEnabled ]] then
        self.Toggle:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end



