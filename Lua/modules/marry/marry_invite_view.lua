
MarryInviteView = MarryInviteView or DeclareView("MarryInviteView", MarryConfig.ResPath .. "marry_invite")
VIEW_DECLARE_LEVEL(MarryInviteView, ViewLevel.L1)

function MarryInviteView:LoadCallback()
end

function MarryInviteView:OnClickClose()
    ViewMgr:CloseView(MarryInviteView)
end

MarryInviteViewF = MarryInviteViewF or DeclareMono("MarryInviteViewF", UIWFlushPanel)

function MarryInviteViewF:MarryInviteViewF()
    self.data = MarryCtrl.Instance:Data()
    self.friend_data = SocietyCtrl.Instance:Data()
	self.family_data = GuildCtrl.Instance:Data()
    self.language = Language.Marry.MarryInvite

    self.data_cares = {
		{data = self.data.marry_invite_sm, func = self.FlushRoleList, keys = {"type"}},
		{data = self.data.marry_invite_info, func = self.FlushInviteList, keys = {"invite_list"},},
		{data = self.data.marry_invite_info, func = self.FlushInfo, keys = {"info"}},
        {data = self.data.marry_invite_info, func = self.FlushRemind, keys = {"apply_list"}},
		{data = self.data.marry_invite_info, func = self.FlushRemind, keys = {"invite_list"}, init_call = false},
    }

    self.invite_init = true
end

function MarryInviteViewF:Awake()
    local invite_number = self.data:ConfigOtherInviteNumber()
    self.InviteFreeMax.text = string.format(self.language.InviteFreeMax, invite_number)
    self.InviteNumShow.text = string.format(self.language.InviteNumShow, 0, invite_number)
    
    self.TypeItems[self.data.marry_invite_sm.type.val].isOn = true
    
    UIWFlushPanel.Awake(self)
end

function MarryInviteViewF:FlushInfo()
    local info = self.data.marry_invite_info.info
    
    if info.init then return end
    local invite_number = self.data:ConfigOtherInviteNumber()
    local add_number = self.data:ConfigOtherAddNumber()
    local ft = os.date("*t", info.wedding_begin_time)

    self.InviteNumShow.text = string.format(self.language.InviteNumShow, info.count, invite_number + add_number * info.buy_invite_time)
    self.RegistTime.text = string.format(self.language.RegistTime, ft.month, ft.day, ft.hour, ft.min)
end

function MarryInviteViewF:FlushRoleList()
    local type = self.data.marry_invite_sm.type.val
    self.LeftTitle.text = self.language.LeftTitle[type]
    self.LeftEmpty.text = self.language.LeftEmpty[type]
    local role_list = {}
	if MarryConfig.InviteType.friend == type then
		role_list = self.friend_data:GetFriendListFriendWeddingInvite()
	elseif MarryConfig.InviteType.family == type then
		role_list = self.family_data:GetMemberListWeddingInvite()
    elseif MarryConfig.InviteType.apply == type then
		role_list = self.data:GetWeddingApplyList()
	end
    self.is_empty = 0 == #role_list
    self.LeftEmpty:SetObjActive(self.is_empty)
    self.RoleList:SetDataList(role_list)
end

function MarryInviteViewF:FlushInviteList()
    local invite_list = self.data.marry_invite_info.invite_list
    local info = self.data.marry_invite_info.info
    self.InviteList:SetData(invite_list)
    self.RightEmptyObj:SetActive(0 == info.count)

    if self.invite_init then
        self.invite_init = false
        return
    end
    self:FlushRoleList()
    PublicPopupCtrl.Instance:Center(self.language.InviteChangeTips)
end

function MarryInviteViewF:FlushRemind()
	self.red_point = self.red_point or UiRedPoint.New(self.BtnApplyObj, Vector3.New(30,30,0))
	self.red_point:SetNum(self.data:WeddingApplyRedPoint() and 1 or 0)
end

function MarryInviteViewF:OnSelectTypeItem(param)
    self.data.marry_invite_sm.type.val = param
end

function MarryInviteViewF:OnClickInvite()
    if self.is_empty then
        PublicPopupCtrl.Instance:Center(self.language.InviteEmptyTips)
        return
    end
    local type = self.data.marry_invite_sm.type.val
    local info = self.data.marry_invite_sm.info
    if MarryConfig.InviteType.friend == type then
        MarryCtrl.Instance:SendWeddingFBReqInvite(info.info.uid)
	elseif MarryConfig.InviteType.family == type then
        MarryCtrl.Instance:SendWeddingFBReqInvite(info.uid)
    elseif MarryConfig.InviteType.apply == type then
        MarryCtrl.Instance:SendWeddingFBReqMark(info.uid)
	end
end

function MarryInviteViewF:OnClickAdd()
    local info = self.data.marry_invite_info.info
    if info.init then return end
    if info.buy_invite_time < self.data:ConfigOtherAddTimes() then
        local currency_type = self.data:ConfigOtherMoneyType()
        local add_consume = self.data:ConfigOtherAddConsume()
        if MallCtrl.IsNotCurrency(currency_type, add_consume) then
            ViewMgr:OpenView(MarryPopupView, {
                title_name = self.language.TipsTitle,
                content = string.format(self.language.ExtentTips, add_consume .. FormatEnum.CurrencyType[currency_type], self.data:ConfigOtherAddNumber()),
                confirm_func = function ()
                    MarryCtrl.Instance:SendWeddingFBReqInviteSizeBuy()
                end
            })
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.AddTimesMax)
    end
end

MarryInviteViewRoleItem = MarryInviteViewRoleItem or DeclareMono("MarryInviteViewRoleItem", UIWidgetBaseItem)
function MarryInviteViewRoleItem:MarryInviteViewRoleItem()
    self.marry_data = MarryCtrl.Instance:Data()
end

function MarryInviteViewRoleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local type = self.marry_data.marry_invite_sm.type.val
    local role_name = ""
    if MarryConfig.InviteType.friend == type then
		role_name = data.info.name
	elseif MarryConfig.InviteType.family == type then
		role_name = data.name
    elseif MarryConfig.InviteType.apply == type then
		role_name = data.name
	end
    self.RoleName.text = role_name
end

function MarryInviteViewRoleItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarryInviteViewRoleItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.marry_invite_sm.info:Set(self.data)
    end
end


MarryInviteViewInviteItem = MarryInviteViewInviteItem or DeclareMono("MarryInviteViewInviteItem", UIWidgetBaseItem)
function MarryInviteViewInviteItem:MarryInviteViewInviteItem()
    self.marry_data = MarryCtrl.Instance:Data()
end

function MarryInviteViewInviteItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.InviteName.text = data.name
end