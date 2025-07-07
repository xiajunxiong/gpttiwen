-- 洗髓灵性
BicycleRefineNatureView = BicycleRefineNatureView or DeclareView("BicycleRefineNatureView","bicycle/bicycle_refine_nature_view")
function BicycleRefineNatureView:BicycleRefineNatureView()
    self.data = BicycleData.Instance
    self.ht = self.data.protocol_info_2108:Care(BindTool.Bind(self.FlushPanel,self))
    self.item_ht = BagData.Instance:Care(BindTool.Bind(self.FlushPanel,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.Nature,Vector2.New(635, 434))
end

function BicycleRefineNatureView:LoadCallback(param_t)
    UH.SetText(self.Desc,Config.language_cfg_auto.textdesc[113].desc)
    self:FlushPanel()
end

function BicycleRefineNatureView:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    local cfg = self.bicycle:Cfg()
    local other_cfg = self.data.cfg.other[1]
    UH.SetText(self.NatureText,string.format(Language.Bicycle.Nature,ColorStr(self.bicycle:Nature(),COLORSTR.Yellow1),cfg.bicycle_grow_min,cfg.bicycle_grow_max))
    local stuff_id = other_cfg.grow_reset_use
    self.StuffCell:SetData(Item.Create({item_id = stuff_id,num = DataHelper.ConsumeNum(Item.GetNum(stuff_id), other_cfg.grow_reset_number, true)}))
end

function BicycleRefineNatureView:CloseCallback()
    self.data.protocol_info_2108:Uncare(self.ht)
    BagData.Instance:UnCare(self.item_ht)
end

function BicycleRefineNatureView:OnBtnClick()
    local cfg = self.bicycle:Cfg()
    if self.bicycle:Nature() >= cfg.bicycle_grow_max then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.NatureTip1)
        return
    end
    local other_cfg = self.data.cfg.other[1]
    local stuff_id = other_cfg.grow_reset_use
    if Item.GetNum(stuff_id) >= other_cfg.grow_reset_number then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ProgressAdd)
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.REFINE_NATURE,self.bicycle:Index())
    else
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.NatureTip2)
    end
end

-- 补充精力
BicycleEnergyView = BicycleEnergyView or DeclareView("BicycleEnergyView","bicycle/bicycle_energy_view")
function BicycleEnergyView:BicycleEnergyView()
    self.data = BicycleData.Instance
end

function BicycleEnergyView:LoadCallback(param_t)
    self.ht = self.data.protocol_info_2108:Care(BindTool.Bind(self.FlushPanel,self))
    self.item_ht = BagData.Instance:Care(BindTool.Bind(self.FlushStuff,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.Energy,Vector2.New(635, 434))
    
    UH.SetText(self.Desc,Config.language_cfg_auto.textdesc[114].desc)
    self:FlushStuff()
    self:FlushPanel()
end

function BicycleEnergyView:CloseCallback()
    self.data.protocol_info_2108:Uncare(self.ht)
    BagData.Instance:UnCare(self.item_ht)
end

function BicycleEnergyView:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    if self.bicycle:Energy() < 500 then
        self.data.bicycle_energy_remind_flag[self.bicycle.id] = 1
    end
    local cfg = self.bicycle:Cfg()
    local progress_num = self.bicycle:Energy()
    local progress_max_num = cfg.bicycle_energy
    self.Progress:SetProgress(progress_num / progress_max_num)
    self.Progress:SetText(string.format("%s/%s",progress_num,progress_max_num))
end

function BicycleEnergyView:FlushStuff()
    local other_cfg = self.data.cfg.other[1]
    UH.SetIcon(self.StuffIcon,Item.GetIconId(other_cfg.energy_up))
    local stuff_num = Item.GetNum(other_cfg.energy_up)
    self.BtnIcon:SetObjActive(stuff_num < 1)
    self.BtnIcon2:SetObjActive(stuff_num > 1)
    if stuff_num < 1 then
        UH.SetGold(self.BtnIcon,CurrencyType.CoinBind)
        UH.SetText(self.BtnText,other_cfg.energy_up_coin)
    else
        UH.SetIcon(self.BtnIcon2,Item.GetIconId(other_cfg.energy_up))
    end
    UH.SetText(self.StuffNum,stuff_num)
end

function BicycleEnergyView:OnStuffClick()
    local other_cfg = self.data.cfg.other[1]
    local item_id = other_cfg.energy_up
    MainOtherCtrl.Instance:GetWayViewById(item_id)
end

function BicycleEnergyView:OnBtnClick()
    local other_cfg = self.data.cfg.other[1]
    local cfg = self.bicycle:Cfg()
    local progress_num = self.bicycle:Energy()
    local progress_max_num = cfg.bicycle_energy
    if progress_num >= progress_max_num then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.EnergyTip1)
        return
    end
    if Item.GetNum(other_cfg.energy_up) < 1 then
        self:Dialog(other_cfg)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.ADD_ENERGY,self.bicycle:Index())
    end
end

function BicycleEnergyView:Dialog(other_cfg)
    local dialog_info = {
        content = Format(Language.Bicycle.AddEnergyTip, other_cfg.energy_up_coin),
        confirm = {
            func = function()
                if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= other_cfg.energy_up_coin then
                    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.ADD_ENERGY,self.bicycle:Index())
                else
                    PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.EnergyTip2)
                end
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        cancel = {
            func = function()
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        tnr = DTTodayNotRemind.BicycleEnergy,
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

function BicycleEnergyView:OnOpenAutoSetClick()
    ViewMgr:OpenView(BicycleEnergyAutoSet, {bicycle = self.bicycle})
end

-- 灵饰
BicycleOrnamentView = BicycleOrnamentView or DeclareView("BicycleOrnamentView","bicycle/bicycle_ornament_view")
function BicycleOrnamentView:BicycleOrnamentView()
    self.data = BicycleData.Instance
    self.ht = self.data.change_bicycle:Care(BindTool.Bind(self.FlushPanel,self))
    self.item_ht = BagData.Instance:Care(BindTool.Bind(self.FlushPanel,self))
    self.ht_2108 = self.data.protocol_info_2108:Care(BindTool.Bind(self.FlushPanel,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.Ornament,Vector2.New(721, 598))
end

function BicycleOrnamentView:LoadCallback(param_t)
    self:FlushModel()
    self:FlushPanel()
end

function BicycleOrnamentView:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    UH.SetText(self.Desc,string.format(Language.Bicycle.OrnamentDesc,self.bicycle:Cfg().give_energy))
    local ornament = self.bicycle:Ornament()
    if not ornament:IsShow() then
        local stuff_id = self.bicycle:Cfg().bicycle_jewelry
        self.StuffCell:SetData(Item.Create({item_id = stuff_id,num = DataHelper.ConsumeNum(Item.GetNum(stuff_id),1, true)}))
        UH.SetText(self.BtnText,Language.Bicycle.OrnamentBtn1)
        if Item.GetNum(stuff_id) > 0 then
            self.Interactor.Interactable = true
        else
            self.Interactor.Interactable = false
        end
        self.EquipName:SetObjActive(true)
        UH.SetText(self.EquipName,Item.GetName(stuff_id))
        self.InfoObj:SetActive(false)
        self.Help2Obj:SetActive(true)
    else
        self.InfoObj:SetActive(true)
        self.Help2Obj:SetActive(false)
        local next_cfg = ornament:NextCfg()
        self.EquipStuffObj:SetActive(next_cfg ~= nil)
        self.MaxFlagObj:SetActive(ornament:IsMaxLevel())
        local nature = ornament:Nature()
        local intansify_num = nature - 1
        if next_cfg ~= nil then
            local stuff_data = next_cfg.jewelry_consume_item[0]
            self.StuffCell:SetData(Item.Create({item_id = stuff_data.item_id,
            num = DataHelper.ConsumeNum(Item.GetNum(stuff_data.item_id), stuff_data.num, true),
            is_bind = stuff_data.is_bind}))
            UH.SetText(self.EquipName,Item.GetName(stuff_data.item_id))
            UH.SetText(self.BtnText,Language.Bicycle.OrnamentBtn2)
            self.Interactor.Interactable = Item.GetNum(stuff_data.item_id) >= stuff_data.num
            local next_nature = next_cfg.bicycle_grow / 100
            --intansify_num = next_nature - nature
        end
        UH.SetText(self.Intansify,(intansify_num * 100) .. "%")
        UH.SetText(self.Level,string.format(Language.Bicycle.Level2,ornament:Level()))
        local max_nature = self.bicycle:Cfg().bicycle_grow_max
        UH.SetText(self.Nature,string.format("%s/%s",ColorStr(nature,COLORSTR.Yellow1),max_nature))
    end
    self.RedPoint:SetNum(self.data:BicycleOrnamentOneRemindNum(self.bicycle) > 0 and 1 or 0)
end

function BicycleOrnamentView:FlushModel()
    local cfg = self.data:GetSelectedData()
    local model_id = cfg.model_id_2
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
				view = self
			}
		)
	end
    self.ui_obj:SetData(DrawerHelper.GetRidePath(model_id))
end

function BicycleOrnamentView:OnBtnClick()
    if self.effect_playing then
        return
    end
    self.bicycle = self.data:GetSelectedBicycle()
    local ornament = self.bicycle:Ornament()
    if not ornament:IsShow() then
        local stuff_id = self.bicycle:Cfg().bicycle_jewelry
        if Item.GetNum(stuff_id) > 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PettEgg2)
            BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.WEAR_ORNAMENTS,self.bicycle:Index())
            ViewMgr:CloseView(BicycleOrnamentView)
        else
            PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.OrnamentTip2)
            local item = Item.Create({item_id = stuff_id})
            MainOtherCtrl.Instance:GetWayView({item = item})
        end
    else
        local next_cfg = ornament:NextCfg()
        local stuff_data = next_cfg.jewelry_consume_item[0]
        if Item.GetNum(stuff_data.item_id) >= stuff_data.num then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
            self.effect_playing = true
            self.EquipEffect:PlayUp()
            self.EquipEffect:SetCallback(BindTool.Bind(function ()
                BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.JEWELRY_STREGTH,self.bicycle:Index())
                self.effect_playing = nil
            end,self))
        else
            PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.OrnamentTip3)
            local item = Item.Create({item_id = stuff_data.item_id})
            MainOtherCtrl.Instance:GetWayView({item = item})
        end
    end
end

function BicycleOrnamentView:CloseCallback()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
    self.data.change_bicycle:Uncare(self.ht)
    BagData.Instance:UnCare(self.item_ht)
    self.data.protocol_info_2108:Uncare(self.ht_2108)
end

function BicycleOrnamentView:OnHelpClick()
    self.HelpContentObj:SetActive(true)
    local all_desc = self.data:GetOrnamentAllNatureDesc(self.bicycle:Ornament())
    UH.SetText(self.HelpContentText,all_desc)
end

function BicycleOrnamentView:OnCloseHelpClick()
    self.HelpContentObj:SetActive(false)
end


-- 洗练技能
BicycleRefineSkillView = BicycleRefineSkillView or DeclareView("BicycleRefineSkillView","bicycle/bicycle_refine_skill_view")
function BicycleRefineSkillView:BicycleRefineSkillView()
    self.data = BicycleData.Instance
    self.ht = self.data.protocol_info_2106:Care(BindTool.Bind(self.FlushPanel,self))
    self.item_ht = BagData.Instance:Care(BindTool.Bind(self.FlushPanel,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.RefineSkill,Vector2.New(874, 580))
    self.lock_skill_list = bit:d2b(0)
    self.cur_lock_num = 0
    self.max_lock_num = 1
end

function BicycleRefineSkillView:LoadCallback(param_t)
    UH.SetGold(self.LockStuffIcon,CurrencyType.Gold)
    self:FlushPanel()
end

function BicycleRefineSkillView:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    local new_skill_list = self.bicycle:NewBaseSkillList()
    local is_has_skill = self.data:IsHasSkill(new_skill_list)
    self.Desc:SetObjActive(not is_has_skill)
    self.SaveBtnObj:SetActive(is_has_skill)
    if is_has_skill then
        self.NewSkillList:SetData(self:ToListData(new_skill_list,false))
        UH.SetText(self.RightTitle,Language.Bicycle.RefineSkillTitle1)
    else
        self.NewSkillList:SetData({})
        UH.SetText(self.RightTitle,Language.Bicycle.RefineSkillTitle2)
        UH.SetText(self.Desc,Config.language_cfg_auto.textdesc[115].desc)
    end

    local unlock_skill_list = self:ToListData(self.bicycle:BaseSkillList(),true)
    self.UnlockSkillList:SetData(unlock_skill_list)
    self.max_lock_num = #unlock_skill_list - 1
    
    local stuff_id = self.data.cfg.other[1].skill_reset_use
    self.StuffCell:SetData(Item.Create({item_id = stuff_id,num = DataHelper.ConsumeNum(Item.GetNum(stuff_id), self.data.cfg.other[1].skill_reset_number, true)}))
end

function BicycleRefineSkillView:ToListData(skill_list,is_lock)
    local list = {}
    for i,v in pairs(skill_list) do
        if v:IsUnlock() then
            local data = {}
            data.skill = v
            data.is_has_lock = is_lock
            table.insert(list,data)
        end
    end
    if is_lock == true and #list == 1 then
        list[1].is_has_lock = false
    end
    return list
end

function BicycleRefineSkillView:CloseCallback()
    self.data.protocol_info_2106:Uncare(self.ht)
    BagData.Instance:UnCare(self.item_ht)
end

function BicycleRefineSkillView:OnRefineClick()
    local lock_price = self:GetLockPrice()
    local stuff_price = 0
    local stuff_id = self.data.cfg.other[1].skill_reset_use
    local need_stuff_num = self.data.cfg.other[1].skill_reset_number
    local stuff_num = Item.GetNum(stuff_id)
    local is_auto_buy = 0
    if self.AutoBuyToggle.isOn == true then
       is_auto_buy = 1
       stuff_price = ShopData.GetFastBuyData(stuff_id).price * (need_stuff_num - stuff_num)
    else
        if stuff_num < need_stuff_num then
            local item = Item.Create({item_id = stuff_id, jump_need_num = need_stuff_num - stuff_num})
            MainOtherCtrl.Instance:GetWayView({item = item})
            PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.RefineTip2)
            return
        end
    end
    local flag = bit:b2d(self.lock_skill_list)
    local price = lock_price + stuff_price
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, price) then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.REFINE_SKILL,self.bicycle:Index(),flag,is_auto_buy)
    end
end

function BicycleRefineSkillView:GetLockPrice()
    local lock_price = self.data.cfg.other[1]["lock_skill_" .. self.cur_lock_num]
    return lock_price or 0
end

function BicycleRefineSkillView:OnSaveClick()
    PublicPopupCtrl.Instance:Center(Language.Bicycle.Tips.RefineTip3)
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SAVE_REFINE_SKILL,self.bicycle:Index())
end

function BicycleRefineSkillView:OnLockClick(toggle_value,mono)
    mono.data.lock_value = toggle_value
    if toggle_value == false then
        self.cur_lock_num = self.cur_lock_num - 1
        self.lock_skill_list[32 - mono.data.skill:Index()] = 0
    else
        self.cur_lock_num = self.cur_lock_num + 1
        self.lock_skill_list[32 - mono.data.skill:Index()] = 1
    end
    if self.cur_lock_num > self.max_lock_num then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.RefineTip1)
        mono.Toggle.isOn = false
    end
    if self.cur_lock_num > 0 then
        UH.SetText(self.LockStuffNum,self:GetLockPrice())
        UH.SetText(self.LockTip,string.format(Language.Bicycle.RefineSkillLockTip,self.cur_lock_num))
        self.LockTip:SetObjActive(true)
    else
        self.LockTip:SetObjActive(false)
    end
end

BicycleRefineSkillItem = BicycleRefineSkillItem or DeclareMono("BicycleRefineSkillItem",UIWidgetBaseItem)
function BicycleRefineSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.SkillIcon:SetData(data.skill)
    UH.SetText(self.Desc,data.skill:Desc())
    self.Toggle:SetObjActive(data.is_has_lock == true)
end


-- 技能池
BicycleSkillPool = BicycleSkillPool or DeclareView("BicycleSkillPool","bicycle/bicycle_skill_pool_view")
function BicycleSkillPool:BicycleSkillPool()
    self.data = BicycleData.Instance
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.SkillPool,Vector2.New(828, 552))
    self.cur_skill_type = BicycleCfg.SKILL_TYPE.BASE
end

function BicycleSkillPool:LoadCallback(param_t)
    self.skill_list = {[0] = {}, [1] = {}, [2] = {}, [3] = {}}
    local base_skill_id_list = self.data:GetAllBaseSkill()
    for i,v in pairs(base_skill_id_list) do
        for i2,v2 in pairs(v) do -- 技能组
            for i3,v3 in pairs(v2) do
                if v3.max_level == nil then
                    v3.max_level = self.data:GetSkillMaxLevel(v3.skill_id)
                end
                table.insert(self.skill_list[i],BicycleSkill.New({skill_id = v3.skill_id,level = v3.max_level,
            skill_type = BicycleCfg.SKILL_TYPE.BASE}))
            end
        end
    end
    self.BaseSkillBtnToggle.isOn = true
end

function BicycleSkillPool:FlushSkillList()
    local list = self.skill_list[self.cur_skill_type]
    self.SkillList:SetData(list)
    self.SkillList:SetectItem(1,true)
    self:FlushSkillInfo(list[1])
end

function BicycleSkillPool:FlushSkillInfo(skill)
    self.SkillIcon:SetData(skill)
    UH.SetText(self.Desc1,Language.Bicycle.AllBicycle)
    UH.SetText(self.Desc2,Language.Bicycle.SkillType[skill:SkillType()])
    UH.SetText(self.Desc3,skill:Desc())
end

function BicycleSkillPool:CloseCallback()
    
end

function BicycleSkillPool:OnBtnToggleClick(type)
    self.cur_skill_type = type
    self:FlushSkillList()
end

function BicycleSkillPool:OnSkillItemClick(data)
    self:FlushSkillInfo(data)
end

BicycleSelectPetView = BicycleSelectPetView or DeclareView("BicycleSelectPetView","bicycle/bicycle_pet_select_view")
VIEW_DECLARE_MASK(BicycleSelectPetView,ViewMask.BlockClose)
function BicycleSelectPetView:BicycleSelectPetView()
    self.data = BicycleData.Instance
    self.ht = self.data.protocol_info_2104:Care(BindTool.Bind(self.FlushPanel,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.PetSelect,Vector2.New(384, 637))
    self.index = 0
    GuideManager.Instance:RegisterGetGuideUi("BicycleSelectPetBtn",function ()
        local pet_list = self.data:GetPetList()
        if TableIsEmpty(pet_list) then
            GuideManager.Instance:ForceStop()
            return
        end
        return self.PetList.item_list[1].Toggle,function ()
            self.PetList.item_list[1].Toggle.isOn = true
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleSelectPetOKBtn",function ()
        return self.OkBtn,function ()
            self:OnOkClick()
        end
    end)
end

function BicycleSelectPetView:LoadCallback(param_t)
    self.index = param_t.index
    self:FlushPanel()
end

function BicycleSelectPetView:FlushPanel()
    local pet_list = self.data:GetPetList()
    self.PetList:SetData(pet_list)
    self.bicycle = self.data:GetSelectedBicycle()
    self.cur_pet_count = self.data:GetPetCount(self.bicycle)
    UH.SetText(self.Tip,string.format(Language.Bicycle.SelectPetTip,self.cur_pet_count,BicycleCfg.MAX_PET_NUM))
end

function BicycleSelectPetView:OnToggleClick(toggle_value,mono)
    local pet = mono.data
    local bicycle_info = pet:Bicycle()
    if bicycle_info then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Tips.PetSelectTip3)
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CANCEL_DOMINANCE,bicycle_info.bicycle_index,bicycle_info.cell_index)
    else
        if self.cur_pet_count >= BicycleCfg.MAX_PET_NUM then
            PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.PetSelectTip1,BicycleCfg.MAX_PET_NUM))
            return
        else
            local bicycle_pet_list = self.bicycle:PetIndexList()
            local cell_index
            if bicycle_pet_list[self.index] < 0 then
                cell_index = self.index
            else
                cell_index = self.index == 0 and 1 or 0
            end
            self.select_flag = true -- 选择标记，表示勾选过宠物
            BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.DOMINANCE_PET,self.bicycle:Index(),pet.index,cell_index)
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
        end
    end
end

function BicycleSelectPetView:OnOkClick()
    ViewMgr:CloseView(BicycleSelectPetView)
end

function BicycleSelectPetView:CloseCallback()
    if self.cur_pet_count > 0 and self.select_flag then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Tips.PetSelectTip2)
    end
    self.data.protocol_info_2104:Uncare(self.ht)
end

BicycleSelectPetItem = BicycleSelectPetItem or DeclareMono("BicycleSelectPetItem",UIWidgetBaseItem)
-- data == pet
function BicycleSelectPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.PetItem:SetData(data)
    UH.SetText(self.Level,string.format(Language.Bicycle.Level,data:Level()))
    UH.SetText(self.Name,data:Name())
    if data:Bicycle() then
        self.ToggleFlag:SetActive(true)
    else
        self.ToggleFlag:SetActive(false)
    end
end

BicycleSkillTip = BicycleSkillTip or DeclareView("BicycleSkillTip","bicycle/bicycle_skill_tip")
function BicycleSkillTip:BicycleSkillTip()

end

-- @skill : BicycleSkill
function BicycleSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.skill == nil then
        return
    end
    if param_t.pos then
        UH.LocalPosG(self.Pos,param_t.pos)
    end
    local skill = param_t.skill
    UH.SetText(self.Name, skill:Name())
    UH.SetText(self.SkillTypeText,Language.Bicycle.SkillType[skill:SkillType()])
    UH.SetText(self.Desc,skill:Desc())
    self.SkillIcon:SetData(skill)
end

function BicycleSkillTip:OnClickMask()
    ViewMgr:CloseView(BicycleSkillTip)
end

BicycleSkillInfoView = BicycleSkillInfoView or DeclareView("BicycleSkillInfoView", "bicycle/bicycle_skill_info")
VIEW_DECLARE_MASK(BicycleSkillInfoView,ViewMask.BlockClose)

function BicycleSkillInfoView:BicycleSkillInfoView()
	self.data = BicycleData.Instance
end

function BicycleSkillInfoView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.skill == nil then
        return
    end
    if param_t.pos then
        UH.LocalPosG(self.Pos,param_t.pos)
    end
    local skill = param_t.skill
	if nil == skill then return end
    UH.SetText(self.SkillName,skill:Name())
    self.SkillCell:SetData(skill)
    local next_skill_effect = self.data:GetSkillEffect(skill.skill_id,skill:Level() + 1)
    self.NextInfoObj:SetActive(nil ~= next_skill_effect)
    local level_str = next_skill_effect ~= nil and string.format(Language.Bicycle.Level,skill:Level()) or Language.Bicycle.MaxLevel
    UH.SetText(self.SkillLevel,level_str)
    UH.SetText(self.CurDesc,skill:Desc())
    UH.SetText(self.SkillTpye,Language.Bicycle.SkillType[skill:SkillType()])
    if next_skill_effect then
        UH.SetText(self.NextDesc,next_skill_effect.description)
    end
    if skill:IsUnlock() then
        self.TipsObj:SetActive(false)
    else
        self.TipsObj:SetActive(true)
        UH.SetText(self.TipText,string.format(Language.Bicycle.SkillInfoUnlockTip,skill:UnlockLevel()))
    end
    if param_t.is_special then
        self.NextInfoObj:SetActive(false)
    end
end

-- 技能重置弹框
BicycleSkillResetDialog = BicycleSkillResetDialog or DeclareView("BicycleSkillResetDialog","bicycle/bicycle_skill_reset_view")

function BicycleSkillResetDialog:LoadCallback(param_t)
    param_t = param_t or {}
    local content = param_t.content or ""
    UH.SetText(self.Content,content)
    self.ok_event = param_t.ok_func
end

function BicycleSkillResetDialog:OnCloseClick()
    ViewMgr:CloseView(BicycleSkillResetDialog)
end

function BicycleSkillResetDialog:OnOkClick()
    if self.ok_event then
        self.ok_event()
    end
    ViewMgr:CloseView(BicycleSkillResetDialog)
end

-- 灵骑装备
BicycleEquipView = BicycleEquipView or DeclareView("BicycleEquipView","bicycle/bicycle_equip_view")
function BicycleEquipView:BicycleEquipView()
    self.data = BicycleData.Instance
    self.item_ht = BagData.Instance:Care(BindTool.Bind(self.FlushStuff,self))
    self.ht = self.data.change_bicycle:Care(BindTool.Bind(self.FlushPanel,self))
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.Equip,Vector2.New(720, 596))
end

function BicycleEquipView:LoadCallback(param_t)
    param_t = param_t or {}
    self.equip_index = param_t.index or 0
    self:FlushPanel()
end

function BicycleEquipView:FlushPanel()
    self.bicycle = self.data:GetSelectedBicycle()
    self.equip = self.bicycle:Equip()[self.equip_index]
    self.equip_is_show = self.equip:IsShow()
    local btn_str = self.equip_is_show ~= true and Language.Bicycle.EquipActive or Language.Bicycle.EquipUp
    UH.SetText(self.BtnText,btn_str)
    self.equip_cfg = self.equip:Cfg()
    UH.SetText(self.EquipName,Item.GetName(self.equip_cfg.equip_icon))
    self.EquipCell:SetData({equip = self.equip})
    local cur_effect_desc = self.equip:CurEffectDesc(true)
    local next_effect_desc = self.equip:NextEffectDesc(true)
    UH.SetText(self.CurEffectDesc,cur_effect_desc)
    self.NextDescObj:SetActive(next_effect_desc ~= nil)
    self.MaxShowObj:SetActive(self.equip:IsMaxLevel())
    if next_effect_desc then
        UH.SetText(self.NextEffectDesc,next_effect_desc)
    end
    self:FlushStuff()
end

function BicycleEquipView:FlushStuff()
    local cfg
    if not self.equip_is_show then
        cfg = self.equip_cfg
    else
        cfg = self.equip:NextCfg()
    end
    self.UseStuffObj:SetActive(cfg ~= nil)
    if not cfg then
        return
    end
    local item_data = cfg.equip_consume_item[0]
    local item_num = Item.GetNum(item_data.item_id)
    local num = DataHelper.ConsumeNum(item_num, item_data.num, true)
    local role_level = RoleData.Instance:GetRoleLevel()
    self.StuffCell:SetData(Item.Create({item_id = item_data.item_id, num = num}))
    local bicycle_is_active = self.data:BicycleIsActiveById(self.bicycle.id)
    self.Interactor.Interactable = item_num >= item_data.num and role_level >= cfg.role_level_limit and bicycle_is_active
    UH.SetText(self.StuffName,Item.GetName(item_data.item_id))
    self.RedPoint:SetNum(self.Interactor.Interactable == true and 1 or 0)
end

function BicycleEquipView:CloseCallback()
    BagData.Instance:UnCare(self.item_ht)
    self.data.change_bicycle:Uncare(self.ht)
end

function BicycleEquipView:OpenAllDesc()
    self.AllDescMaskObj:SetActive(true)
    local all_desc = self.data:GetEquipAllEffectDesc(self.equip)
    UH.SetText(self.AllDesc,all_desc)
end

function BicycleEquipView:CloseAllDesc()
    self.AllDescMaskObj:SetActive(false)
end

function BicycleEquipView:OnHelpClick()
    self:OpenAllDesc()
end

function BicycleEquipView:OnCloseMaskClick()
    self:CloseAllDesc()
end

function BicycleEquipView:OnBtnClick()
    if self.effect_playing then
        return
    end
    local stuff_data
    local oper_type
    local bicycle_is_active = self.data:BicycleIsActiveById(self.bicycle.id)
    if not bicycle_is_active then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
        return
    end
    if self.equip_is_show == true then
        local next_cfg = self.equip:NextCfg()
        local role_level = RoleData.Instance:GetRoleLevel()
        if role_level < next_cfg.role_level_limit then
            PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.EquipTip2,next_cfg.role_level_limit))
            return
        end
        stuff_data = next_cfg.equip_consume_item[0]
        oper_type = BicycleCfg.OP_TYPE.UP_EQUIP
    else
        local cfg = self.equip:Cfg()
        local role_level = RoleData.Instance:GetRoleLevel()
        if role_level < cfg.role_level_limit then
            PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.EquipTip3,cfg.role_level_limit))
            return
        end
        stuff_data = cfg.equip_consume_item[0]
        oper_type = BicycleCfg.OP_TYPE.ACTIVE_EQUIP
    end
    if Item.GetNum(stuff_data.item_id) >= stuff_data.num then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
        self.effect_playing = true
        self.EquipEffect:PlayActive()
        self.EquipEffect:SetCallback(BindTool.Bind(function ()
            BicycleCtrl.Instance:Req(oper_type,self.bicycle:Index(),self.equip:Index())
            self.effect_playing = nil
        end,self))
    else
        local item = Item.Create({item_id = stuff_data.item_id})
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.EquipTip1)
        MainOtherCtrl.Instance:GetWayView({item = item})
    end
end

BicycleEquipEffectMgr = BicycleEquipEffectMgr or DeclareMono("BicycleEquipEffectMgr",UIWFlushPanel)
function BicycleEquipEffectMgr:BicycleEquipEffectMgr()
    self.effect_type = 1   -- 1 : 播放激活特效，2播放升级特效
end

function BicycleEquipEffectMgr:Awake()
    UIWFlushPanel.Awake(self)
    self.effect_2161011_start_pos = self.Effect2161011:GetLocalPosition()
    self.effect_2161011_end_active_pos = self.Effect2161038:GetLocalPosition()
    self.effect_2161011_end_up_pos = self.Effect2161039:GetLocalPosition()
    self:Reset()
end

function BicycleEquipEffectMgr:GetEndPos()
    if self.effect_type == 1 then
        return self.effect_2161011_end_active_pos
    else
        return self.effect_2161011_end_up_pos
    end
end

function BicycleEquipEffectMgr:OnDestroy()
    TimeHelper:CancelTimer(self.move_handle)
    TimeHelper:CancelTimer(self.ht1)
    TimeHelper:CancelTimer(self.ht)
    TimeHelper:CancelTimer(self.end_ht)
    UIWFlushPanel.OnDestroy(self)
end

function BicycleEquipEffectMgr:PlayActive()
    self.effect_type = 1
    self:Play()
end

function BicycleEquipEffectMgr:PlayUp()
    self.effect_type = 2
    self:Play()
end

function BicycleEquipEffectMgr:PlayCommon()
    self.Effect2161037:SetActive(true)
    self.ht1 = Invoke(function ()
        self:Move2161011()
    end,1)
end

function BicycleEquipEffectMgr:Play()
    self:Reset()
    TimeHelper:CancelTimer(self.ht)
    self.ht = Invoke(function ()
        self:PlayCommon()
    end,0.1)
end

function BicycleEquipEffectMgr:Reset()
    if self.end_func then
        self.end_func()
        self.end_func = nil
    end
    TimeHelper:CancelTimer(self.move_handle)
    TimeHelper:CancelTimer(self.ht1)
    self.Effect2161037:SetActive(false)     -- 聚光特效
    self.Effect2161011:SetActive(false)     -- 飞行子弹特效
    self.Effect2161038:SetActive(false)     -- 爆发特效
    self.Effect2161039:SetActive(false)     -- 升级特效
end

function BicycleEquipEffectMgr:SetMovePos(pos1, pos2)
    self.move_pos1 = pos1
    self.move_pos2 = pos2
end

function BicycleEquipEffectMgr:Move2161011()
    self.cur_step = 1
    self.default_step = 20
    TimeHelper:CancelTimer(self.move_handle)
    UH.LocalPosG(self.Effect2161011,self.effect_2161011_start_pos)
    self.Effect2161011:SetActive(true)
    local pos1 = self.move_pos1 or Vector3.New(-2, 1, 0)
    local pos2 = self.move_pos2 or Vector3.New(2, 1, 0)
    self.curve = Game.CurveParabola(self.effect_2161011_start_pos, pos1, self:GetEndPos(), pos2)
    self.move_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.move2161011,self),1,self.default_step)
end

function BicycleEquipEffectMgr:move2161011()
    local pos = self.curve:GetPos(self.cur_step / self.default_step)
    UH.LocalPosG(self.Effect2161011,pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        self.Effect2161011:SetActive(false)
        self:ExEndFunc()
	end
end

-- 子弹特效移动结束后的回调
function BicycleEquipEffectMgr:SetCallback(func)
    self.end_func = func
end

-- 升级或爆发特效播完的回调
function BicycleEquipEffectMgr:SetCallback2(func)
    self.end_func_2 = func
end

function BicycleEquipEffectMgr:ExEndFunc()
    if self.end_func then
        self.end_func()
        self.end_func = nil
    end
    if self.effect_type == 1 then
        self.Effect2161038:SetActive(true)
    else
        self.Effect2161039:SetActive(true)
    end
    if self.end_func_2 then
        TimeHelper:CancelTimer(self.end_ht)
        self.end_ht = Invoke(function ()
            self:OnEndEvent()
        end)
    end
end

function BicycleEquipEffectMgr:OnEndEvent()
    if self.end_func_2 then
        self.end_func_2()
        self.end_func_2 = nil
    end
end

BicycleEquipItemLevelInfo = BicycleEquipItemLevelInfo or DeclareMono("BicycleEquipItemLevelInfo", ItemInfoViewItem)

function BicycleEquipItemLevelInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    local show_type = self.item1:ShowType()
    self.Rect:SetSizeWithCurrentAnchors(1,280)
    if show_type == ShowType.BicycleEquip then
        local equip_id = BicycleData.Instance:GetEquipIdByItem(self.item1)
        if equip_id then
            self.List:SetData(BicycleData.Instance:GetEquipAllEffectList(equip_id))
        else
            self.List:SetData({})
        end
    elseif show_type == ShowType.HeartSkill then
        self.List:SetData(HeartSkillData.Instance:GetConsumeItemData(self.item1.item_id) or {})
    elseif show_type == ShowType.Halidom then
        local id = BicycleData.Instance:GetHalidomIdByItem(self.item1)
        if id then
            self.List:SetData(BicycleData.Instance:GetHalidomAllEffectList(id))
        else
            self.List:SetData({})
        end
    elseif show_type == ShowType.BicycleOrnament then
        local id = BicycleData.Instance:GetOrnamentIdByItem(self.item1)
        if id then
            self.List:SetData(BicycleData.Instance:GetOrnamentAllEffectList(id))
        else
            self.List:SetData({})
        end
    elseif show_type == ShowType.IllustrateRare then
        local cfg = IllustrateData.Instance:GetAllLvRareAttr(self.item1.item_id)
        self.List:SetData(cfg)
    elseif show_type == ShowType.PetWeapon then
        local list_data = self.item1:GetAllEffectList()
        self.List:SetData(list_data)
        if TableIsEmpty(list_data) then
            self.Rect:SetSizeWithCurrentAnchors(1,2)
        end
    end
end

--@data {equip_level,equip_skill_id} skill_cfg
BicycleEquipItemLevelItem = BicycleEquipItemLevelItem or DeclareMono("BicycleEquipItemLevelItem", UIWidgetBaseItem)
function BicycleEquipItemLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local level_text = data.level_text or Format(Language.Bicycle.Level,data.equip_level)
    UH.SetText(self.Level,level_text)
    if data.desc then
        UH.SetText(self.Desc, data.desc)
    else
        local config = data.skill_cfg or Cfg.MedalEffectById(data.equip_skill_id)
        local desc = data.attr_desc or ""
        desc = desc .. config.description
        UH.SetText(self.Desc, desc)
    end
end

-- 灵骑点化加成总览
BicycleDianHuaDialog = BicycleDianHuaDialog or DeclareView("BicycleDianHuaDialog", "bicycle/bicycle_dian_hua_dialog")
VIEW_DECLARE_MASK(BicycleDianHuaDialog,ViewMask.BgBlockClose)
function BicycleDianHuaDialog:BicycleDianHuaDialog()
    self.data = BicycleData.Instance
end

function BicycleDianHuaDialog:LoadCallback(param_t)
    local bicycle = param_t.bicycle or self.data:GetSelectedBicycle()
    if TableIsEmpty(bicycle) or bicycle:DianHuaLevel() == 0 then
        self.Empty:SetActive(true)
        self.Show:SetActive(false)
        return
    end
    self.Empty:SetActive(false)
    self.Show:SetActive(true)

    local attri_list = bicycle:GetDianHuaAttriList()
    self.AttriList:SetData(attri_list)
    local score = bicycle:DianHuaScore()
    UH.SetText(self.Score, "+" .. score)
end


-- 灵骑自动补充体力设置
BicycleEnergyAutoSet = BicycleEnergyAutoSet or DeclareView("BicycleEnergyAutoSet", "bicycle/bicycle_energy_auto_set")
function BicycleEnergyAutoSet:LoadCallback(param)
    self.bicycle = param.bicycle
    self.ht = BicycleData.Instance.protocol_info_2108:Care(BindTool.Bind(self.FlushPanel, self))
    self:FlushPanel()
    UH.SetText(self.Tip, Language.Bicycle.Text11)
end

function BicycleEnergyAutoSet:CloseCallback()
    BicycleData.Instance.protocol_info_2108:Uncare(self.ht)
end

function BicycleEnergyAutoSet:FlushPanel()
    local cfg = self.bicycle:Cfg()
    local progress_num = self.bicycle:Energy()
    local progress_max_num = cfg.bicycle_energy
    self.Progress:SetProgress(progress_num / progress_max_num)
    self.Progress:SetText(string.format("%s/%s",progress_num,progress_max_num))

    local is_auto = self.bicycle:IsAutoRecover()
    UH.SetText(self.BtnText, is_auto and Language.Bicycle.BtnCloseAuto or Language.Bicycle.BtnOpenAuto)
end

function BicycleEnergyAutoSet:OnCloseClick()
    ViewMgr:CloseView(BicycleEnergyAutoSet)
end

function BicycleEnergyAutoSet:OnBtnClick()
    local is_auto = self.bicycle:IsAutoRecover()
    if is_auto then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CALCEL_AUTO_RECOVER, self.bicycle.index)
    else
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.OPEN_AUTO_RECOVER, self.bicycle.index)
    end
end