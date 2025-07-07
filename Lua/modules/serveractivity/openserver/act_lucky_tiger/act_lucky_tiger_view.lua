ActLuckyTigerView = ActLuckyTigerView or DeclareMono("ActLuckyTigerView", UIWFlushPanel)

function ActLuckyTigerView:ActLuckyTigerView()
    self.Data = ActLuckyTigerData.Instance
    self.data = self.Data.lucky_tiger_data
    self.language = Language.LuckyCow
    self.config = Config.yinhunafu_auto.basic_configuration[1]
    self.data_cares = {
        {data = self.Data.lucky_tiger_data, func = self.FlushShow, init_call = true},
    }
end

function ActLuckyTigerView:Awake()
    UIWFlushPanel.Awake(self)
    local pet_config = PetData.Instance:GetPetCfg(self.config.reward_item[0].item_id)
    UH.SetIcon(self.ImgSetPetIcon, pet_config.icon_id)
    UH.SpriteName(self.ImgSetFrameQua, PetData.PinZhi[pet_config.quality])
    UH.SetText(self.TxtName, pet_config.name)
    UH.SpriteName(self.ImgSetQua, PetData.PetLevel2[pet_config.quality])
    UH.SpriteName(self.ImgSetPrice, HuoBi[self.config.type])
    UH.SetText(self.TxtLimitNum, Format(self.language.LimitNum, DataHelper.GetDaXie(self.config.buy_num)))
    UH.SetText(self.TxtOldPrice, self.config.original_price)
    UH.SetText(self.TxtNowPrice, self.config.current_price)
    UH.SetText(self.TxtDiscount, Format(self.language.Discount, self.config.discount * 10))

    local buy_end_time = TimeManager.FormatUnixTime2Date(self.Data:GetStateTime(ActLuckyTigerState.BuyEnd))
    local show_time = TimeManager.FormatUnixTime2Date(self.Data:GetStateTime(ActLuckyTigerState.Show))
    local get_num = function(num)
        return tonumber(num) < 10 and "0" .. num or num
    end
    UH.SetText(self.TipList[1], Format(
        self.language.Tips[1],
        buy_end_time.year,
        get_num(buy_end_time.month),
        get_num(buy_end_time.day),
        get_num(buy_end_time.hour),
        get_num(buy_end_time.minute),
        get_num(show_time.hour),
        get_num(show_time.minute)
    ))
    UH.SetText(self.TipList[2], Format(self.language.Tips[2], self.config.return_item))
    UH.SetText(self.TipList[3], Format(self.language.Tips[3], pet_config.name))

    self.ModelUiShow:SetData({item_id = Item.Create(self.config.reward_item[0])})

    self.PetSign:SetType(pet_config.limit_type)
end

function ActLuckyTigerView:FlushShow()
    self.Time:CloseCountDownTime()
    self.IHandlerBuyed.Interactable = true
    if self.data.state == ActLuckyTigerState.CanBuy then
        self.ObjBuyed:SetActive(self.data.is_buy)
        if self.data.is_buy then
            UH.SetText(self.TxtBuyed, self.language.Btn[1])
        else
            self.Time:StampTime(self.Data:GetStateTime(ActLuckyTigerState.BuyEnd), TimeType.Type_Special_9, self.language.Time)
        end
    else
        self.ObjBuyed:SetActive(true)
        if self.data.state == ActLuckyTigerState.BuyEnd then
            UH.SetText(self.TxtBuyed, self.language.Btn[2])
            self.IHandlerBuyed.Interactable = false
        elseif self.data.state == ActLuckyTigerState.Show then
            UH.SetText(self.TxtBuyed, self.language.Btn[3])
        else
            PublicPopupCtrl.Instance:Center(self.language.CloseTip)
            ActivityRandData.Instance:CheckRandOpenData()
        end
    end
end

function ActLuckyTigerView:OnPetClick()
    local pet = Pet.New({pet_id = self.config.reward_item[0].item_id})
    pet.view_btn_hide = {}
    pet.view_btn_hide[1] = "BtnStamp"
    pet.view_btn_hide[2] = "PetWeapon"
    ViewMgr:OpenView(PetDetailView, pet)
end

function ActLuckyTigerView:OnBuyClick()
    if not self.data.is_buy then
        if MallCtrl.IsNotCurrency(self.config.type, self.config.current_price) then
            ActLuckyTigerCtrl.Instance:SendReq(ActLuckyTigerReqType.Buy)
        end
    end
end

function ActLuckyTigerView:OnBuyedClick()
    if self.data.state == ActLuckyTigerState.Show and not TableIsEmpty(self.data.user_info) then
        ViewMgr:OpenView(ActLuckyCowNameView, {user_info = self.data.user_info})
    end
end