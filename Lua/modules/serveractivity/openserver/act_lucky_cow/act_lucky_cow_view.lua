ActLuckyCowView = ActLuckyCowView or DeclareMono("ActLuckyCowView", UIWFlushPanel)

function ActLuckyCowView:ActLuckyCowView()
    self.Data = ActLuckyCowData.Instance
    self.data = self.Data.lucky_cow_data
    self.language = Language.LuckyCow
    self.config = Config.lucky_cow_auto.set_cow[1]
    self.data_cares = {
        {data = self.Data.lucky_cow_data, func = self.FlushShow, init_call = true},
    }
end

function ActLuckyCowView:Awake()
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

    local buy_end_time = TimeManager.FormatUnixTime2Date(self.Data:GetStateTime(ActLuckyCowState.BuyEnd))
    local show_time = TimeManager.FormatUnixTime2Date(self.Data:GetStateTime(ActLuckyCowState.Show))
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

    self.PetSign:SetType(pet_config.limit_type)
end

function ActLuckyCowView:FlushShow()
    self.Time:CloseCountDownTime()
    self.IHandlerBuyed.Interactable = true
    if self.data.state == ActLuckyCowState.CanBuy then
        self.ObjBuyed:SetActive(self.data.is_buy)
        if self.data.is_buy then
            UH.SetText(self.TxtBuyed, self.language.Btn[1])
        else
            self.Time:StampTime(self.Data:GetStateTime(ActLuckyCowState.BuyEnd), TimeType.Type_Special_4, self.language.Time)
        end
    else
        self.ObjBuyed:SetActive(true)
        if self.data.state == ActLuckyCowState.BuyEnd then
            UH.SetText(self.TxtBuyed, self.language.Btn[2])
            self.IHandlerBuyed.Interactable = false
        elseif self.data.state == ActLuckyCowState.Show then
            UH.SetText(self.TxtBuyed, self.language.Btn[3])
        else
            PublicPopupCtrl.Instance:Center(self.language.CloseTip)
            ActivityRandData.Instance:CheckRandOpenData()
        end
    end
end

function ActLuckyCowView:OnPetClick()
    local pet = Pet.New({pet_id = self.config.reward_item[0].item_id})
    pet.view_btn_hide = {}
    pet.view_btn_hide[1] = "BtnStamp"
    pet.view_btn_hide[2] = "PetWeapon"
    ViewMgr:OpenView(PetDetailView, pet)
end

function ActLuckyCowView:OnBuyClick()
    if not self.data.is_buy then
        if MallCtrl.IsNotCurrency(self.config.type, self.config.current_price) then
            ActLuckyCowCtrl.Instance:SendReq(ActLuckyCowReqType.Buy)
        end
    end
end

function ActLuckyCowView:OnBuyedClick()
    if self.data.state == ActLuckyCowState.Show and not TableIsEmpty(self.data.user_info) then
        ViewMgr:OpenView(ActLuckyCowNameView, {user_info = self.data.user_info})
    end
end

-------------------ActLuckyCowNameView-------------------
ActLuckyCowNameView = ActLuckyCowNameView or DeclareView("ActLuckyCowNameView", "serveractivity/openserver/act_lucky_cow_name_view")
VIEW_DECLARE_MASK(ActLuckyCowNameView, ViewMask.BgBlock)
function ActLuckyCowNameView:ActLuckyCowNameView()
    
end

function ActLuckyCowNameView:LoadCallback(param)
    self.List:SetData(param.user_info or {})
end

function ActLuckyCowNameView:OnCloseClick()
    ViewMgr:CloseView(ActLuckyCowNameView)
end

-------------------ActLuckyCowNameItem-------------------
ActLuckyCowNameItem = ActLuckyCowNameItem or DeclareMono("ActLuckyCowNameItem", UIWidgetBaseItem)
function ActLuckyCowNameItem:SetData(data)
    self.Avatar:SetAvatar(data.avatar_type, data.avatar_id,data.avatar_quality)
    UH.SetText(self.TxtName, data.role_name)
end