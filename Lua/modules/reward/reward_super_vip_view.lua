RewardSuperVipView = RewardSuperVipView or DeclareMono("RewardSuperVipView", UIWFlushPanel)

function RewardSuperVipView:RewardSuperVipView()
    self.data = SuperVipCtrl.Instance.data
    self.data_cares = {
        {data = self.data.load_data, func = self.OnFlush,init_call = true},
        {data = self.data.info_data, func = self.FlushRequestView,init_call = false},
    }
    self.data:SetRemindNum()
end

function RewardSuperVipView:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRequestView()
end

function RewardSuperVipView:FlushRequestView()
    local info = self.data:GetRequestInfo()
    if not IsEmpty(info) then
        local data = info.data or {}
        local recharge_num = math.floor(RechargeData.Instance:GetRechargeNum()/10)
        local today_recharge_num = self:GetTodayRechargeNum(data)
        today_recharge_num = today_recharge_num <= 0 and 0 or today_recharge_num
        local item_list,reward_list = self.data:GetParseList(data.content)
        local activation1,activation2 = tonumber(data.gold_kefu_activation2),tonumber(data.gold_kefu_activation1)
        local get_flag = today_recharge_num >= activation1 or recharge_num >= activation2

        UH.SetText(self.Desc,Format(data.content.activation_mode,activation1,
        today_recharge_num,activation1,activation2,recharge_num,activation2))

        local desc_list = Split(Language.SuperVip.VipTip[get_flag and 2 or 1],"|")
        UH.SetText(self.Text[1],desc_list[1])
        UH.SetText(self.Text[2],desc_list[2])

        for i=1,self.ItemList:Length() do
            self.ItemList[i]:SetData(item_list[i])
        end
        self.Grid:SetData(reward_list)
        self.BtnSave:SetActive(get_flag)
        self.GetPanel:SetActive(not get_flag)
        self.ImgCode:SetObjActive(get_flag)
        self.ImgChat:SetObjActive(not get_flag)
        --下载头像图片
        if self.data:WeChatImg() == nil then
            SuperVipCtrl.Instance:SendWeChatRequestReq(data.head_img)
        end
        --下载二维维码
        if self.data:CodeImg() == nil then
            SuperVipCtrl.Instance:SendCodeRequestReq(data.code_img)
        end
        self.Image:SetActive(not get_flag)
    end
end
        
--加载图片
function RewardSuperVipView:OnFlush()
    local we_chat_img = self.data:WeChatImg()
    if we_chat_img and self.ImgChat then
        self.ImgChat.texture = we_chat_img
    end
    local code_img = self.data:CodeImg()
    if code_img and self.ImgCode then
        self.ImgCode.texture = code_img
    end
end

function RewardSuperVipView:GetTodayRechargeNum(data)
    local prefs_num = self.data:GetTodayRechargeNum()
    local prefs_server_num = self.data:GetServerDayNum()
    local server_num = TimeCtrl.Instance:GetCurOpenServerDay()
    local recharge_num = RechargeData.Instance:GetTodayRechargeNum()
    if prefs_server_num == server_num then
        return math.floor((recharge_num - prefs_num)/10)
    end
    return math.floor(recharge_num/10)
end

function RewardSuperVipView:OnClickGet()
    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
    ViewMgr:CloseView(RewardView)
end

function RewardSuperVipView:OnClickSave()
    UH.SavePhotoToPicture(self.ImgCode.texture,"share_code")
    PublicPopupCtrl.Instance:Center(Language.SuperVip.SaveTip)
end

----------------------------RewardSuperVipItem----------------------------
RewardSuperVipItem = RewardSuperVipItem or DeclareMono("RewardSuperVipItem", UIWidgetBaseItem)

function RewardSuperVipItem:SetData(data)
    UH.SetText(self.Num,data.index)
    UH.SetText(self.Desc,data.desc)
    UH.SetText(self.Title,data.title)
end