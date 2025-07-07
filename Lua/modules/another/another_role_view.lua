AnotherRoleView = AnotherRoleView or DeclareMono("AnotherRoleView", UIWFlushPanel)

function AnotherRoleView:AnotherRoleView()
    self.data = AnotherData.Instance
    self.data_cares = {
        {data = self.data.role_data, func = self.FlushAttrList, keys = {"attr_list"}},
        {data = self.data.role_data, func = self.FlushModelShow, keys = {"appearance"}},
        {data = self.data.equip_data, func = self.FlushMedalView, keys = {"medal_info"}},
        {data = self.data.equip_data, func = self.FlushCrystalView, keys = {"element_crystal"}},
        {data = self.data.role_point_data, func = self.FlushPointView, init_call = false},
        {data = self.data.equipment_list, func = self.FlushEquipList},
        {data = self.data.jewelry_list, func = self.FlushJewelryList},
        {data = self.data.equipment_list, func = self.FlushModelShow},
    }
end

function AnotherRoleView:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTopView()
    self:FlushPosView()
    if AnotherData.IsGuide() then
        self.data:SetRolePointViewShow(true)
        UH.LocalPos(self.Right,Vector3.New(416.5,-20,0))
    end
end

function AnotherRoleView:OnDestroy()
    self.data:InitializationRoleData()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.effect_time1)
	TimeHelper:CancelTimer(self.effect_time2)
    Delete(self,"ui_obj")
end

function AnotherRoleView:FlushModelShow()
    self.cur_appearance = self.data:GetRoleAppearance()
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.CreateByAppearance(self.cur_appearance)
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    else
        self.ui_obj:SetData(self.cur_appearance)
    end
end

function AnotherRoleView:FlushPosView()
    local position = self.data:GetRolePosition()
    self.BtnFight.isOn = position == 1
    UH.SetText(self.Position,position == 1 and Language.Role.BattleFront or Language.Role.BattleBehind)
end

function AnotherRoleView:FlushTopView()
    local ranking = BattlefieldData.Instance:GetRankNum()
    local config = BattlefieldData.Instance:GetRankConfig(ranking)
    local score = BattlefieldData.Instance:GetRankScore()
    for i=1,self.StarList:Length() do
        self.StarList[i]:SetActive(i <= config.level)
    end
    UH.SetText(self.RankName,BattlefieldData.QuaName(config))
    local level = BattlefieldData.Instance:GetOtherConfig().level_limit
    UH.SetText(self.RoleLevel,Format(Language.Battlefield.RoleLevel,level))
    if BattlefieldData.Instance:GetTopRace() == 0 then
        UH.SetText(self.RankName,Language.Battlefield.NotRankTip)
        self.Lock:SetActive(true)
        self.Gray.enabled = true
    else
        UH.SpriteName(self.Rank,[[Rank]].. config.rank)
        UH.SetText(self.RankName,BattlefieldData.QuaName(config))
        UH.SetText(self.Score,Language.Battlefield.Integral[1] .. score)
        self.Effect:SetActive(true)
        self.Lock:SetActive(false)
        local role_id = RoleData.Instance:GetRoleId()
        if UnityPlayerPrefs.GetInt([[BattlefieldLockEffect]] .. role_id)==0 then
            self.effect_time1 = Invoke(function()
                self.EffectTool:Play(10010032)
            end,1)
            self.effect_time2 = Invoke(function()
                self.Effect:SetActive(false)
                self.Gray.enabled = false
            end,2)
            UnityPlayerPrefs.SetInt([[BattlefieldLockEffect]] .. role_id,1)
        else
            self.Gray.enabled = false
        end
    end
    self.Score:SetObjActive(BattlefieldData.Instance:GetTopRace() ~= 0)
    UH.SetText(self.Name,RoleData.Instance:GetRoleName())
end

function AnotherRoleView:FlushAttrList()
    local attr_data = self.data:GetRoleAttrList()
    local attr_list = self.data:GetAttrTypeData(attr_data,
    RoleAttributeTypeConfig,RoleData.Instance:GetRoleProf())
    for i=1,self.AttrList:Length() do
        if attr_list[i] then
            self.AttrList[i]:SetData(attr_list[i])
        end
        self.AttrList[i]:SetObjActive(attr_list[i] ~= nil)
    end
end

function AnotherRoleView:FlushPointView()
    self.PointSub:SetActive(self.data:IsRolePointViewShow())
end

function AnotherRoleView:FlushEquipList()
    local list = self.data:GetEquipList()
    for i = 1, self.EquipList:Length() do
        self.EquipList[i]:SetData(list[i - 1] and list[i - 1] or Item.New())
	end
end

function AnotherRoleView:FlushJewelryList()
    local list = self.data:GetJewelryList()
    for i = 1, self.JewelryList:Length() do
        self.JewelryList[i]:SetData(list[i - 1] and list[i - 1] or Item.New())
	end
end

function AnotherRoleView:FlushMedalView()
    local medal_list = self.data:GetMedalInfo()
    for i = 1, 3 do 
        self.BtnMedalsList[i]:SetData(medal_list[i])
    end 
end

function AnotherRoleView:OnClickMedal(data)
    local medal_list = self.data:GetMedalInfo()
    if data.item_id == 0 then return end
    data.param.sp_fail = BagData.Instance:GetEquipedMedalIsSpFail(data.grid_index + 1,medal_list)
    local medal = Item.Create(data)
    local info = {item_list = {medal},func = function () return {} end}
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function AnotherRoleView:FlushCrystalView()
    local element_crystal = self.data:GetElementCrystal()
    local item_id = element_crystal.element_crystal_id or 0
    local crystal = EleCrystal.New()
    crystal.item_id = item_id
    UH.SetText(self.CrystalNum,Format(Language.EleCrystal.SimpleUseNum, element_crystal.used_times or 0))
    UH.SetIcon(self.Crystal,crystal:IconId())
    self.CrystalNumObj:SetActive(item_id ~= 0)
    self.CrystalObj:SetActive(item_id ~= 0)
end

function AnotherRoleView:OnClickFightPos()
    AnotherCtrl.Instance:SendPositionReq(not self.BtnFight.isOn and 0 or 1)
    UH.SetText(self.Position,self.BtnFight.isOn and Language.Role.BattleFront or Language.Role.BattleBehind)
end

function AnotherRoleView:OnClickAddPoint()
    self.data:SetRolePointViewShow(true)
end

function AnotherRoleView:OnClickCrystal()
    ViewMgr:OpenView(CrystalView,{crystal_func = function()
        return self.data:GetCrystalList()
    end})
end

function AnotherRoleView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(155)
end

----------------------------AnotherAttrItem----------------------------
AnotherAttrItem = AnotherAttrItem or DeclareMono("AnotherAttrItem", UIWidgetBaseItem)

function AnotherAttrItem:SetData(data)
    local target_num = data.attr_limit
    local attr_value = Valve(data.attr_value,target_num)
    self.Progress:SetProgress(attr_value / target_num)
    local is_limit = attr_value == target_num

    if DataHelper.IsPercent(data.attr_type) then
		attr_value = Percent(attr_value)
        target_num = Percent(target_num)
    end
    self.Progress:SetText(Format([[%s/%s]],is_limit and 
    attr_value or ColorStr(attr_value,COLORSTR.Yellow7),target_num))

    if self.ImgSet ~= nil then
        self:FlushImgSetView(is_limit)
    end
    UH.SetText(self.Name,data.attr_name)
end

function AnotherAttrItem:FlushImgSetView(is_limit)
    local img_name = is_limit and [[JinDuTiaoChengHong]] or [[JinDuTiaoCheng]]
    UH.SpriteName(self.ImgSet,img_name)
end