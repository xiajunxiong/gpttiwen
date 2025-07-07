ActNewServerPetView = ActNewServerPetView or DeclareMono("ActNewServerPetView", UIWFlushPanel)

function ActNewServerPetView:ActNewServerPetView()
    self.Data = ActNewServerPetData.Instance
    self.data = self.Data.info_data
    self.language = Language.NewServerPet
    self.config = Config.xinfulingchong_auto.reward_bug[1]
    self.item1 = self.config.reward_item[0]
    self.item2 = self.config.reward_item[1]
    self.data_cares = {
        {data = self.Data.info_data, func = self.FlushShow, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActNewServerPetView:Awake()
    UIWFlushPanel.Awake(self)
    self:FirstRemind()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_NEW_SERVER_PET, TimeType.Type_Special_4) 
    local pet_config = PetData.Instance:GetPetCfg(self.item1.item_id)
    local pet = Pet.New({pet_id = pet_config.id})
    self.AttrList:SetPet(pet, true)
    local skill_list = pet:GetSkillAndGiftGroupList()
    self.SkillList:SetData(skill_list)
    self:FlushShow()
    self.ModelUi1:SetData({item_id = self.item1})
    self.ModelUi2:SetData({item_id = self.item2})

    local item2_config = Item.GetConfig(self.item2.item_id)
    UH.SetText(self.TxtName1, pet_config.name)
    UH.SetText(self.TxtName2, item2_config.name)

    UH.SetText(self.TxtPrice, self.config.cfg_or == 1 and self.config.price / 10 or self.config.price)
    UH.SetActGold(self.Icon, self.config.price_type, self.config.cfg_or)
    UH.SetText(self.TxtDesc, self.language.Tip)
    UH.SetText(self.TxtDesc2, Format(self.language.BuyTip, pet_config.name))
    UH.SpriteName(self.ImgPet, PetData.PetLevel2[pet.config.quality])
end

-- 活动首次红点，换设备重新提示
function ActNewServerPetView:FirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_NEW_SERVER_PET)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_NEW_SERVER_PET)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_NEW_SERVER_PET), act_start)
        self.data:Notify()
    end
end

function ActNewServerPetView:FlushShow()
    self.ObjBuyed:SetActive(self.data.buy_count > 0)
end

function ActNewServerPetView:OnPet1Click()
    local pet = Pet.New({pet_id = self.item1.item_id})
    pet.view_btn_hide = {}
    pet.view_btn_hide[1] = "BtnStamp"
    pet.view_btn_hide[2] = "PetWeapon"
    ViewMgr:OpenView(PetDetailView, pet)
end

function ActNewServerPetView:OnPet2Click()
    local info = {
        item_list = {Item.Create({item_id = self.item2.item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function ActNewServerPetView:OnBuyClick()
    if self.data.buy_count > 0 then
        return
    end
    if self.config.cfg_or == 1 then
        local fee = self.config.price / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_NEW_SERVER_PET, fee, ACTIVITY_TYPE.RAND_NEW_SERVER_PET, ActNewServerPetReqType.Buy)
    else
        if MallCtrl.IsNotCurrency(self.config.price_type) then
            ActNewServerPetCtrl.Instance:SendReq(ActNewServerPetReqType.Buy)
        end
    end
end

function ActNewServerPetView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NEW_SERVER_PET, self.config.price)
end

function ActNewServerPetView:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    end
end
