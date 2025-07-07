
local CurAppear = {
    [1] = "1",
    [2] = "2",
    [3] = "3",
    [4] = "4",
    [5] = "5"
}

local ChangeAppear = {
    [1] = {"1","2","3","4"},
    [2] = {"1","2","3","4"},
    [3] = {"1","2","3","4"},
    [4] = {"1","2","3","4"},
    [5] = {"1","2","3","4"},
}

RoleAppearChangeView = RoleAppearChangeView or DeclareView("RoleAppearChangeView","role_appear_change/role_appear_change",Mod.RoleAppear.Main)
--AppearList====UIWidgetList
--PlayerName====Text
--CellItem====UIWSubstitute
--GreenBtn====Toggle

VIEW_DECLARE_LEVEL(RoleAppearChangeView,ViewLevel.L1)

VIEW_DECLARE_MASK(RoleAppearChangeView,ViewMask.BgBlockNoAnim)

function RoleAppearChangeView:LoadCallback()
    LuaMonoBehaviour:Awake()
    -- self.Currency:DefaultSet()
    self.data = RoleAppearChangeData.Instance
    self.select_data = self.data:GetSeletData()
    self.AppearList:SetData(self.select_data)
    self.AppearList:ClickItem(1)
    self.CellItem:SetDataByItemId(CommonItemId.ChangeAppear)    --角色形象更换券
    local cur_appear_id = RoleData.Instance:GetBaseData().appearance_id --当前形象id
 
    self.GreenBtn.isOn = true
    self:ColorClick(1)
end

function RoleAppearChangeView:RoleAppearChangeView()
    self.PlayerName.text =  RoleData.Instance:GetBaseData().name
end

function RoleAppearChangeView:ChangeClick()


    if BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) > 0 then
        PublicPopupCtrl.Instance:AlertTip(Language.RoleAppearChange.CheckTips,function()
            self:NeoToChange()
        end)
    end 

    -- if BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) > 0 then
    --     RoleAppearChangeCtrl.Instance:SendAppearanceID()
    -- end
end

function RoleAppearChangeView:NeoToChange()
    RoleAppearChangeCtrl.Instance:SendAppearanceID()
end

function RoleAppearChangeView:Close()
    ViewMgr:CloseView(RoleAppearChangeView)
end

function RoleAppearChangeView:ColorClick(index)
    RoleAppearChangeData.Instance:SetCurColor(index)
    -- RoleData.Instance:GetBaseData().send_appear_id = self.data:GetAppearSendID(index) 
end


FlushCouponNum = FlushCouponNum or DeclareMono("FlushCouponNum",UIWFlushPanel)
--CouponNum====Text
--CurAppear====ModelUIShow
--ChangedAppear====ModelUIShow
--BtnChangeInetrActor=====InteractorHandler
--BtnChangeText======Text
--color_buttons======Array[UIImageSpriteSet]

function FlushCouponNum:FlushCouponNum()
    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.New()
        self.CurAppear:SetShowData({
            ui_obj = self.cur_ui_obj,
            view = self
        })
    end 

    if self.change_ui_obj == nil then
        self.change_ui_obj = UIChDrawer.New()
        self.ChangedAppear:SetShowData({
            ui_obj = self.change_ui_obj,
            view = self
        })
    end 

    self.data = RoleAppearChangeData.Instance

    self.data_cares = {
        {data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.Flush},
        {data = RoleData.Instance:GetBaseData(),func = self.FlushModel,keys = {"appearance","send_appear_id"}},
        {data = self.data.appear_data,func = self.FlushModel,keys = {"cur_left_select","cur_color"}},
        {data = self.data.appear_data,func = self.Flush,keys = {"cur_left_select","cur_color"}},
        -- {data = RoleAppearChangeData.Instance.base_data,func = self.FlushRightModle,keys = {"send_appear_id"}}
    }
end

function FlushCouponNum:Flush()
    self.CouponNum.text = tostring( BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) .. "/1")
    local role_appearance = RoleData.Instance:GetApearance()
    local flag_1 = BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) > 0 
    local flag_2 = role_appearance.avatar_type ~= self.data.appear_data.cur_left_select or role_appearance.color ~= self.data.appear_data.cur_color
    self.BtnChangeInetrActor.Interactable = flag_1 and flag_2
    local flag_ = flag_1 and flag_2
    self.BtnChangeText.color = flag_ and COLORS.Red2 or COLORS.Gray1 

    local cur_role_app = self.data:GetSelectRoleAppCfg()
    if cur_role_app == nil then return end 
    local cur_color_list = Split(cur_role_app.color_list,",")
    for i = 1,4 do 
        UH.SpriteName(self.color_buttons[i], "PeiSe"..cur_color_list[i])
    end 
end

function FlushCouponNum:FlushModel()
    self:CreateModel()
end

function FlushCouponNum:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.cur_ui_obj ~= nil then
		self.cur_ui_obj:DeleteMe()
	end
    self.cur_ui_obj = nil

    if self.change_ui_obj ~= nil then
		self.change_ui_obj:DeleteMe()
	end
	self.change_ui_obj = nil
end

function FlushCouponNum:CreateModel()
    local send_appear_id = RoleData.Instance:GetBaseData().send_appear_id

    local role_appearance = RoleData.Instance:GetApearance()
    -- role_appearance.weapon_id = 0
    self.cur_ui_obj:SetData(role_appearance)

    --根据形象id改变形象
    local chance_appearance = RoleAppearChangeData.Instance:GetAppearSendAppearance(send_appear_id)
    chance_appearance.weapon_id = role_appearance.weapon_id
    self.change_ui_obj:SetData(chance_appearance)


end

AppearItem = AppearItem or DeclareMono("AppearItem",UIWidgetBaseItem)
--PicName====Text
--LeftAppear====UIImageSpriteSet
--Select====Toggle
--HeadIcon====AvatarCell
--CurImage====ModelUIShow
function AppearItem:SetData(data)
    self.PicName.text = data.image_name
    self.data = data
    if data.index == 1 then self.Select.isOn = true end

    self.HeadIcon:SetAvatar(data.avartar_type,data.avatar_id,data.avatar_quality)
end

function AppearItem:Click()
    RoleAppearChangeData.Instance.appear_data.cur_left_select = self.data.avartar_type
    -- RoleData.Instance:GetBaseData().send_appear_id = RoleAppearChangeData.Instance:GetAppearSendID() 
end