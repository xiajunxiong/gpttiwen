AnotherEquipView = AnotherEquipView or DeclareMono("AnotherEquipView", UIWFlushPanel)

function AnotherEquipView:AnotherEquipView()
    self.data = AnotherData.Instance
    self.data_cares = {
        {data = self.data:GetEquipList(), func = self.FlushEquipList},
        {data = self.data:GetJewelryList(), func = self.FlushJewelryList},
        {data = self.data:GetElementCrystal(), func = self.FlushCrystalView, keys = {"element_crystal_id"}},
        {data = self.data.equip_data, func = self.FlushMedalView, keys = {"medal_info"}},
        {data = self.data.role_data, func = self.FlushModelShow, keys = {"appearance"}},
        {data = self.data:GetEquipList(), func = self.FlushModelShow},
    }
end

function AnotherEquipView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,"ui_obj")
end

function AnotherEquipView:Awake()
    UIWFlushPanel.Awake(self)
    if AnotherData.IsGuide() then
        UH.LocalPos(self.BtnArrange,Vector3.New(0,-324.5,0))
    end
end

function AnotherEquipView:FlushModelShow()
    self.cur_appearance = self.data:GetRoleAppearance()
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.CreateByAppearance(self.cur_appearance)
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    else
        self.ui_obj:SetData(self.cur_appearance)
    end
end

function AnotherEquipView:FlushGridList()
    self.GridList:SetData(self.data:GridList())
end

function AnotherEquipView:FlushEquipList()
    local list = self.data:GetEquipList()
    for i = 1, self.EquipList:Length() do
        self.EquipList[i]:SetData(list[i - 1] and list[i - 1] or Item.New())
	end
end

function AnotherEquipView:FlushJewelryList()
    local list = self.data:GetJewelryList()
    for i = 1, self.JewelryList:Length() do
        self.JewelryList[i]:SetData(list[i - 1] and list[i - 1] or Item.New())
	end
end

function AnotherEquipView:FlushMedalView()
    local medal_list = self.data:GetMedalInfo()
    for i = 1, 3 do 
        self.BtnMedalsList[i]:SetData(medal_list[i])
    end 
end

function AnotherEquipView:OnClickMedal(data)
    local medal_list = self.data:GetMedalInfo()
	if data.item_id == 0 then return end
    data.param.sp_fail = BagData.Instance:GetEquipedMedalIsSpFail(data.grid_index + 1,medal_list)
    local medal = Item.Create(data)
    local info = {item_list = {medal},
		func = function () return {ItemInfoFunc.AnotherUnFix} end}
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function AnotherEquipView:FlushCrystalView()
    local element_crystal = self.data:GetElementCrystal()
    local item_id = element_crystal.element_crystal_id or 0
    local crystal = EleCrystal.New()
    crystal.item_id = item_id
    UH.SetText(self.CrystalNum,Format(Language.EleCrystal.SimpleUseNum, element_crystal.used_times or 0))
    -- self.CrystalCell:SetData(item_id ~= 0 and EleCrystal.New({item_id = item_id}) or Item.New())
    UH.SetIcon(self.Crystal,crystal:IconId())
    self.CrystalNumObj:SetActive(item_id ~= 0)
    self.CrystalObj:SetActive(item_id ~= 0)
end

function AnotherEquipView:OnClickCrystal()
    ViewMgr:OpenView(CrystalView,{crystal_func = function()
        return self.data:GetCrystalList()
    end})
end

function AnotherEquipView:OnClickBtnArrange()
    AnotherCtrl.Instance:SendEquipReq({req_type = ANOTHER_EQUIP_TYPE.INORDER})
end

AnotherBagViewGrids = AnotherBagViewGrids or DeclareMono("AnotherBagViewGrids", UIWFlushPanel)

function AnotherBagViewGrids:AnotherBagViewGrids()
    self.data = AnotherData.Instance
    self.data_cares = {
        {data = self.data:GetColumnGrids(),func = self.FlushGrid},
    }
    self:FlushGrid()
end 

function AnotherBagViewGrids:FlushGrid()
    -- LogError("?FlushGrid?")
    -- local grids =  -- self.data:CheckBetterEqiup( or {})
	self.GridList:SetData(self.data:GetColumnGrids())
end

AnotherMedalEquipCell = AnotherMedalEquipCell or DeclareMono("AnotherMedalEquipCell",UIWidgetBaseItem)
function AnotherMedalEquipCell:AnotherMedalEquipCell()
end

function AnotherMedalEquipCell:SetData(data)
    if data == nil then return end 

    if data.item_id ~= 0 then 
        local medal = Item.Create(data)
        self.Call:SetData(medal)
        -- local medal = Item.Create({item_id = data.item_id,param = {sp_id = data.param.sp_id }})
        -- UH.SetIcon(self.Icon, medal:IconId())        
        -- if medal.param.sp_id > 0 then 
		-- local cfg = Cfg.MedalEffectById(medal.param.sp_id)
        --     self.MedalSpEff:GetComponent(typeof(UnityUI.Text)).text = cfg.name
        --     self.MedalSpEff:GetComponent(typeof(UnityUI.Outline)).effectColor = Medal_Color_Outline[cfg.sp_priority]
        --     self.MedalSpEff:GetComponent(typeof(UnityUI.Shadow)).effectColor = Medal_Color_Shadow[cfg.sp_priority]
		-- else 
        --     self.MedalSpEff:GetComponent(typeof(UnityUI.Text)).text = ""
        -- end 
	end 
        
    self.CallObj:SetActive(data.item_id ~= 0 )
    -- self.IconObj:SetActive( data.item_id ~= 0)
    -- self.MedalSpEff:SetActive( data.item_id ~= 0)
    UIWidgetBaseItem.SetData(self,data)
end