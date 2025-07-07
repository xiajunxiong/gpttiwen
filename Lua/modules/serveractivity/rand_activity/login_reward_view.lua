ServerActivityData.LoginRewardIconName = {
    [0] = "day_1_icon",
    [1] = "day_2_icon",
    [2] = "day_3_icon",
    [3] = "day_4_icon",
    [4] = "day_5_icon",
    [5] = "day_6_icon",
    [6] = "day_7_icon",
    [7] = "day_8_icon",
    [8] = "day_9_icon",
    [9] = "day_10_icon",
    [10] = "day_11_icon",
    [11] = "day_12_icon",
    [12] = "day_13_icon",
    [13] = "day_14_icon",
    [14] = "day_15_icon",
    [15] = "day_16_icon",
    [16] = "day_17_icon",
    [17] = "day_18_icon",
    [18] = "day_19_icon",
    [19] = "day_20_icon",
    [20] = "day_21_icon",
    [21] = "day_22_icon",
    [22] = "day_23_icon",
    [23] = "day_24_icon",
    [24] = "day_25_icon",
    [25] = "day_26_icon",
    [26] = "day_27_icon",
    [27] = "day_28_icon",

    [28] = "yi_wan_tong_bi", -- 新的阶段
    [29] = "",--"day_23_icon",
    [30] = "day_12_icon",
    [31] = "day_23_icon",
    [32] = "",
    [33] = "26502",
    [34] = "zi_se_li_bao",
    [35] = "",
    [36] = "day_23_icon",
    [37] = "MoYuShi",
    [38] = "",
    [39] = "li_bao_hong",
    [40] = "cheng_se_li_bao",
    [41] = "",
    [42] = "MoYuShi",
    [43] = "WuLianJinSheng",
    [44] = "jing_yan_dan",
    [45] = "26502",
    [46] = "MoYuShi",
    [47] = "zi_se_li_bao",
    [48] = "",
    [49] = "chao_ji_xi_lian_lu",
    [50] = "MoYuShi",
    [51] = "jing_yan_dan",
    [52] = "cheng_se_li_bao",
    [53] = "26502",
    [54] = "MoYuShi",
    [55] = "",
}

LoginRewardPreviewImg = {
    XingTian = {img = "XingTian",
        title = Language.LoginRewardPreview.Titiles.XingTian,
        name = Language.LoginRewardPreview.Names.XingTian,
        effect = 5167000,
    }
}

LoginRewardView = LoginRewardView or DeclareView("LoginRewardView", "serveractivity/login_reward_view", Mod.RandActivity.LoginReward)
VIEW_DECLARE_LEVEL(LoginRewardView,ViewLevel.L1)
VIEW_DECLARE_MASK(LoginRewardView,ViewMask.None)
function LoginRewardView:LoginRewardView()
    local time_seq = ServerActivityData.Instance:GetLoginRewardTimeSeq()
    if time_seq == 1 then
        self.RewardPageObj1:SetActive(true)
        self.RewardPageObj2:SetActive(false)
    elseif time_seq == 2 then
        self.RewardPageObj1:SetActive(false)
        self.RewardPageObj2:SetActive(true)
    else
        LogError("28天奖励活动有问题请检查！！！time_seq = ",tostring(time_seq))
    end

    GuideManager.Instance:RegisterGetGuideUi("LoginRewardGetBtn",function ()
        return self.GetBtn,function()
            ServerActivityCtrl.Instance:SendLoginSystemReq(1)
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("XingTianCell",function ()
        return self.XingTianCell9,function (  )
            local cfg = Config.function_notice_auto.notice[24]
            FunOpen.Instance.cur_open_func_notice_data = cfg
            GivePetNoticeView.is_xing_tian_guide = true
            ViewMgr:OpenView(GivePetNoticeView)
        end
    end)
end

function LoginRewardView:LoadCallback()
    self.care_ht = ServerActivityData.Instance.login_reward_close:Care(BindTool.Bind(self.PlayCloseAnim,self))
end

function LoginRewardView:CloseCallback()
    ServerActivityData.Instance.login_reward_close:Uncare(self.care_ht)
    TimeHelper:CancelTimer(self.time_handle)
end

function LoginRewardView:OnClickPageClose()
    --直接关闭
    --ViewMgr:CloseView(LoginRewardView)
    ServerActivityData.Instance.login_reward_close:Notify()
end

function LoginRewardView:PlayCloseAnim()
    local icon_mono = ServerActivityData.Instance:GetLoginRewardActIconMono()
    if icon_mono then
        icon_mono:SetEffectState(false)
        self:PlayCloseAnimation(icon_mono.gameObject.transform)
        return
    end
    ViewMgr:CloseView(LoginRewardView)
end

function LoginRewardView:PlayCloseAnimation(icon_trans)
    self.Mask:SetActive(false)
	self.play_closeing = true
	self.gameObject.transform:SetParent(icon_trans)
	TimeHelper:CancelTimer(self.time_handle)
    self.panel_scale = 1
    self.close_anima_speed = 12
	self.panel_scale_lerp = self.panel_scale / self.close_anima_speed
	self.view_pos = self.gameObject:GetLocalPosition()
	self.view_pos_x_lerp = (self.view_pos.x + 30) / self.close_anima_speed
	self.view_pos_y_lerp = self.view_pos.y / self.close_anima_speed
	self.canvas_group_alpha = 1 / self.close_anima_speed
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.CloseAnimation,self),1,self.close_anima_speed)
end

function LoginRewardView:CloseAnimation()
	self.panel_scale = self.panel_scale - self.panel_scale_lerp
	self.panel_scale = self.panel_scale <= 0.1 and 0.1 or self.panel_scale
	UH.LocalScale(self.PanelTrans,Vector3.New(self.panel_scale,self.panel_scale,self.panel_scale))
	self.view_pos.x = self.view_pos.x - self.view_pos_x_lerp
	self.view_pos.y = self.view_pos.y - self.view_pos_y_lerp
	self.gameObject:SetLocalPosition(self.view_pos)
	self.CanvasGroup.alpha = self.CanvasGroup.alpha - self.canvas_group_alpha

	self.close_anima_speed = self.close_anima_speed - 1
	if self.close_anima_speed <= 0 then
		ViewMgr:CloseView(LoginRewardView)
		local icon_mono = ServerActivityData.Instance:GetLoginRewardActIconMono()
		if icon_mono then
			icon_mono:SetEffectState(true)
		end
	end
end

LoginRewardFlushPanel = LoginRewardFlushPanel or DeclareMono("LoginRewardFlushPanel", UIWFlushPanel)

function LoginRewardFlushPanel:LoginRewardFlushPanel()
    self.page_width = 1232
    self.page_spacing = 0
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.login_reward_data, func = self.FlushLoginData, keys = {"reward_flag"}},
    }
    self.ht =TimeHelper:AddRunTimer(BindTool.Bind(self.CheckClickObjActive,self),0.2)
end 

function LoginRewardFlushPanel:FlushLoginData() 
    local list_data = self.data:GetLoginRewardList()
    for i =1,#list_data do 
        self.cell_groups[i]:SetData(list_data[i])
    end 

    self:TryJumpPage()
    --self.next_day_special = self.data:GetLoginRewardSpecialDays(self.data:GetLoginRewardDays())

    local one_day_flag = self.data:GetLoginRewardFlag(list_data[1].login_days) > 0
    self.OneTipObj:SetActive(one_day_flag)
    if one_day_flag then
        local list_data = DataHelper.FormatItemList(list_data[1].reward_item)
        self.OneRewardList:SetData(list_data)
        UH.SetText(self.OneDayTip, Language.LoginReward.OnDayTip)
    end
   
    -- if self.next_day_special== nil then 
    --     ViewMgr:CloseView(LoginRewardView)
    --     return 
    -- end 
    
    if self.data.login_preview_data then
        self.Preview:SetObjActive(true)
        self.Preview:SetData(self.data.login_preview_data)
        self.data.login_preview_data = nil
    else
        self.Preview:SetObjActive(false)
    end
end 

function LoginRewardFlushPanel:OnClickEnd()
    ViewMgr:CloseView(LoginRewardView)
end

function LoginRewardFlushPanel:OnClickIntro()
    self.part_page:SetActive(true)
    if self.operate_days ~= nil then 
        self.operate_days:SetActive(false)
    end 
end

function LoginRewardFlushPanel:ClickPage(page_)
    local pos = self.part_page_content:GetLocalPosition()
    local cur_page_offset = Vector3.zero
    local width = self.page_width + self.page_spacing
    -- page1 
    if pos.x > -width then 
        if page_ == 0 then 
            cur_page_offset = Vector3.zero
        else 
            cur_page_offset = Vector3.New(-width, 0, 0)
        end 
    -- page2
    elseif pos.x <= -width and pos.x > -(2*width - self.page_spacing) then 
        if page_ == 0 then 
            cur_page_offset = Vector3.zero
        else 
            cur_page_offset = Vector3.New(-(2*width - self.page_spacing), 0, 0)
        end 
    --page3
    elseif pos.x <= -(2*width - self.page_spacing) and pos.x > -(3*width - self.page_spacing) then 
        if page_ == 0 then 
            cur_page_offset = Vector3.New(-width, 0, 0)
        else 
            cur_page_offset = Vector3.New(-(3*width - self.page_spacing), 0, 0)
        end 
    --page4
    elseif pos.x <= -(3*width - self.page_spacing) then 
        if page_ == 0 then 
            cur_page_offset = Vector3.New(-(2*width - self.page_spacing), 0, 0)
        else 
            cur_page_offset = Vector3.New(-(3*width - self.page_spacing), 0, 0)
        end 
    end 
    self.part_page_content:SetLocalPosition(cur_page_offset)
end

function LoginRewardFlushPanel:TryJumpPage()
    -- 位置是固定的
    local cur_pos = self.data:GetLoginRewardDays()
    local remind, can_get_day = self.data:GetLoginRewardNum()
    if remind > 0 then
        cur_pos = can_get_day
    end
    local width = self.page_width + self.page_spacing
    if self.data:GetLoginRewardFlag(cur_pos) < 0 then cur_pos = cur_pos + 1 end 
    local cur_page_offset = Vector3.zero
    self.ClickLeftObj:SetActive(not (cur_pos >= 1 and cur_pos <= 8))
    self.ClickRightObj:SetActive(not (cur_pos >= 26 and cur_pos <= 28))
    if cur_pos >= 1 and cur_pos <= 8 then 
        cur_page_offset = Vector3.zero
    elseif cur_pos >= 9 and cur_pos <= 16 then 
        cur_page_offset = Vector3.New(-width, 0, 0)
    elseif cur_pos >= 17 and cur_pos <= 25 then 
        cur_page_offset = Vector3.New(-(2*width - self.page_spacing), 0, 0)
    elseif cur_pos >= 26 and cur_pos <= 28 then
        cur_page_offset = Vector3.New(-(3*width - self.page_spacing), 0, 0)
    end 

    self.part_page_content:SetLocalPosition(cur_page_offset)
end

function LoginRewardFlushPanel:OnDestroy()
    TimeHelper:CancelTimer(self.ht)
    UIWFlushPanel.OnDestroy(self)
end

function LoginRewardFlushPanel:CheckClickObjActive()
    local cur_page_offset = self.part_page_content:GetLocalPosition()
    self.ClickLeftObj:SetActive(not (cur_page_offset.x > -100))
    self.ClickRightObj:SetActive(not (cur_page_offset.x < -3000))
end

function LoginRewardFlushPanel:CloseTipClick()
    ViewMgr:CloseView(LoginRewardView)
end


LoginRewardCellItem = LoginRewardCellItem or DeclareMono("LoginRewardCellItem",UIWidgetBaseItem)

function LoginRewardCellItem:SetData(data)
    self.data = data

    local item_ = Item.New(data.reward_item[0])
    self.DayNum.text = string.format( Language.LoginReward.DayNum,data.login_days)
    self.ItemName.text = data.show_name -- item_:Name()

    self.ItemNumObj:SetActive(data.show_num  > 1)
    UH.SpriteName(self.ItemNum,"x" .. data.show_num)
    UH.SpriteName(self.ItemIcon,ServerActivityData.LoginRewardIconName[data.seq])

    local flag = ServerActivityData.Instance:GetLoginRewardFlag(data.login_days)
    self.Interactor.Interactable = flag >= 0
    self.EffParent:SetActive(flag >= 0)
    self.got:SetActive(flag < 0)

    if self.red_point == nil then 
        self.red_point = UiRedPoint.New(self.red_pos, Vector3.New(0, 0), false)
    end 
    
    local num = flag > 0 and 1 or 0
    self.red_point:SetNum(num)
    
end 

function LoginRewardCellItem:OnClick()
    if self.Interactor.Interactable then
        if ServerActivityData.Instance:GetLoginRewardDays() >= self.data.login_days then 
            ServerActivityCtrl.Instance:SendLoginSystemReq(self.data.login_days)
        else 
            PublicPopupCtrl.Instance:Center(string.format(Language.LoginReward.EndShow,self.data.login_days,self.data.show_name))
        end 
    else 
        PublicPopupCtrl.Instance:Center(Language.LoginReward.RewardGot)
    end 
end

LoginRewardSub = LoginRewardSub or DeclareMono("LoginRewardSub",UIWFlushPanel)
function LoginRewardSub:LoginRewardSub()
    self.data_cares = {
		{data = ServerActivityData.Instance.login_reward_data, func = self.FlushPanel, keys = {"reward_flag"}},
    }
    GuideManager.Instance:RegisterGetGuideUi("LoginRewardActBtn",function ()
        return self,function ()
            ViewMgr:OpenView(LoginRewardView)
        end
    end)
end

function LoginRewardSub:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function LoginRewardSub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    ServerActivityData.Instance:SetLoginRewardActIconMono(self)
end

function LoginRewardSub:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.effect:SetActive(false)
    ServerActivityData.Instance:SetLoginRewardActIconMono(self)
end

function LoginRewardSub:SetEffectState(state)
	self.effect:SetActive(state or false)
end

function LoginRewardSub:FlushPanel()
    local remind, can_get_day = ServerActivityData.Instance:GetLoginRewardNum()
    local day = ServerActivityData.Instance:GetLoginRewardDays()
    local reward_data
    local name_text

    if remind > 0 and can_get_day then
        reward_data = ServerActivityData.Instance:GetLoginRewardData(can_get_day)
        name_text = Language.LoginReward.Name
    else
        day = day >= 28 and 28 or day + 1
        reward_data = ServerActivityData.Instance:GetLoginRewardData(day)
        name_text = Language.LoginReward.NextName
    end

    if not reward_data then
        return
    end
    UH.SetText(self.Name,name_text)
    if StringIsEmpty(reward_data.icon_name) then
        UH.SetIcon(self.Icon,Item.GetIconId(reward_data.reward_item[0].item_id))
    else
        UH.SpriteName(self.IconSprite,reward_data.icon_name)
    end
end

function LoginRewardSub:OnClick()
    ViewMgr:OpenView(LoginRewardView)
end

LoginRewardPreviewItem = LoginRewardPreviewItem or DeclareMono("LoginRewardPreviewItem", UIWidgetBaseItem)
function LoginRewardPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LoginTip, data.login_days)
    local reward_item_id = data.reward_item[0].item_id
    --UH.SetText(self.Name, Item.GetName(reward_item_id))
    if data.preview_pop and data.preview_img ~= "" then
        local img_cfg = LoginRewardPreviewImg[data.preview_img]
        if img_cfg then
            UH.SetText(self.Name, img_cfg.name)
            UH.SetText(self.RewardTip, img_cfg.title)
            UH.SpriteName(self.ModelImg, img_cfg.img)
            self.UIEffectTool:Play(img_cfg.effect)
        end
    end
end

function LoginRewardPreviewItem:OnCloseClick()
    self.gameObject:SetActive(false)
end