PetBookPanel = PetBookPanel or DeclareMono("PetBookPanel", UIWFlushPanel)
--BtnList
--AllPetList
--LblName
--LblValue
--ImgType
--EleAttr
--LblType
--AttrList
--GiftList
--BtnParent
--LblRace
--LblAttr
--PetList
--ObjAllPet
--ObjPet
--ForceBtnList
--BtnForce
--ImgLevel
--LblWay
--StrBtnParent 
--MutantToggle=====Toggle
--TypeBgSetter=====UIWTextureStter
--Mutant===========PetMutantTag
function PetBookPanel:PetBookPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    self.last_cell = nil
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
        })
    end
    self.data_cares = {
        {data = self.pet_data:GetBookSelect(), func = self.FlushPetDetail, init_call = false},
        {data = self.pet_data.cur_book_skin, func = self.FlushPetModel, keys = {"id","try_skin","use_skin"}, init_call = false},
        {data = PetTaleData.Instance.tale_data, func = self.FlushTale, init_call = false},
        {data = PetTaleData.Instance.tale_data, func = self.onFlush, init_call = false},
    }
    self.curResid = -1
    self.gm_add_level = 1
    self.show_way_panel = true
    self.GmAddBtn:SetActive(DEBUG_MODE)
end

function PetBookPanel:onFlush()
    local flag = self.pet_data:GetIsAllAndDefault()
    self.ObjAllPet:SetActive(flag)
    self.ObjPet:SetActive(not flag)
    local all_data = nil
    if flag then
        all_data = self.pet_data:GetPetListQuality()
        self.AllPetList:SetData(all_data)
    else
        all_data = self.pet_data:GetPetListAttr()
        local list = {}
        local role_level = RoleData.Instance:GetRoleLevel()
        for i,v in pairs(all_data) do
            if not v.config.show_condition_level or role_level >= v.config.show_condition_level then
                table.insert(list,v)
            end
        end
        self.PetList:SetData(list)
    end
end

function PetBookPanel:OnClickBtnRace()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.RaceArrow:SetActive(not activeSelf)
    if activeSelf then
        self.BtnList:SetData(PetData.TypeBtnList.Race)
    else
        self.AttrArrow:SetActive(true)
    end
end

function PetBookPanel:OnClickBtnAttr()
    AudioMgr:PlayEffect(AudioType.UI, "click_tongyong1")
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.AttrArrow:SetActive(not activeSelf)
    if activeSelf then
        self.BtnList:SetData(PetData.TypeBtnList.Attr)
    else
        self.RaceArrow:SetActive(true)
    end
end

function PetBookPanel:OnClickBtnForce()
    if not self.StrBtnParent.activeSelf then
        local data = self.pet_data:GetBookSelect()
        local str_data = self.pet_data:GetStrengthenCfg(data.config.strengthen_id)
        local max_str = Pet.StrengthMaxLv(data:Quality())
        if max_str < #str_data then
            str_data = TableCopy(str_data)
            while max_str < #str_data do
                table.remove(str_data,#str_data)
            end
        end
        self.ForceBtnList:SetData(str_data)
    end
    self.StrBtnParent:SetActive(not self.StrBtnParent.activeSelf)
end
-- 宠物选中
function PetBookPanel:OnClickPet(cell)
    local data = cell:GetData()
    if self.cur_select_cell == cell then
        return
    end
    if self.cur_select_cell ~= nil and self.cur_select_cell ~= cell then
        self.cur_select_cell:SetMutant(false)
    end
    self.cur_select_cell = cell
    AudioMgr:PlayEffect(AudioType.UI,"click_tongyong3")
    if self.cur_mutant_toggle == 1
        or self.cur_mutant_toggle == 2 then
        self.NormalToggleBtn.isOn = true
        -- if data.config.quality ~= 5 then
        --     data = self.pet_data:GetMutantPetCfgByNormalId(data.config.prototype_id)
        -- end
    else
        self.pet_data:SetBookSelect(data)
    end
    -- self.pet_data:SetBookSelect(data)--cell:GetData())
end
function PetBookPanel:OnClickGift(data)
    self.pet_data:SetClickSkill(data)
    ViewMgr:OpenView(PetSkillTip)
end

function PetBookPanel:OnClickHelp()
    ViewMgr:OpenView(PetRestraintView)
end

function PetBookPanel:FlushPetDetail(keys)
    local data = self.pet_data:GetBookSelect()
    if data.EleAttr == nil then
        return
    end
    local ele_data = data:EleAttr() --[[ PetData.GetElement(data) ]]
    local gift_list = data:GetSkillAndGiftGroupList()
    -- 重置+1强化
    local str_level = 1
    self.pet_data:SetPetStrengthenLevel(str_level)
    PetData.SetStrLevel(nil, self.ImgLevel, str_level)

    self.EleAttr:SetData(ele_data)

    gift_list[1].main_skill = true
    self.GiftListScrollRect.enabled = false--#gift_list > 4
    if self.delay_fresh_size ~= nil then
        TimeHelper:CancelTimer(self.delay_fresh_size)
        self.delay_fresh_size = nil
    end
    self.delay_fresh_size = TimeHelper:AddDelayFrameTimer(function ()
        self.GiftContentRect.sizeDelta = Vector2.New(10 + #gift_list * 95, 135.85)
        UH.AnchoredPosition(self.GiftContentRect, Vector2.New((10 + #gift_list * 95) / 2, 0))--重置技能列表的位置
        self.GiftListScrollRect.enabled = #gift_list >= 4
    end,1)
    

    self.GiftList:SetData(gift_list)
    UH.SetText(self.PetMaxSkillNumText, string.format(self.lang.PetGetSkillMaxNumFormat, data.config.gift_max))
    self.AttrList:SetData(self.lang.AttrClass)

    local way_cfg = self.pet_data:GetCatchWay(data:CatchWay())
    self.show_get_way_btn = false
    if way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpNone then
        UH.SetText(self.GetWayText, self.lang.PetGetwayNotyet)
        UH.SetText(self.GetWayUnlockText,"")
    elseif way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpScene then
        local map_cfg = MapData.Instance:GetMapShowData(way_cfg[1].catch_link)
        local limit_level = MapData.Instance:GetMapMinLevel(map_cfg.restrict_in)
        self.show_get_way_btn = limit_level <= RoleData.Instance:GetRoleLevel()
        UH.SetText(self.GetWayText, string.format(Language.Common.Xstr, COLORSTR.Red6, map_cfg.name).." "..Language.Pet.GetCatch)
        if not self.show_get_way_btn then
            UH.SetText(self.GetWayUnlockText, string.format(Language.Common.Xstr, COLORSTR.Red6, limit_level)..Language.Pet.CatchMapUnlockLevel)
        else
            self.GoToGetBtn:SetData(way_cfg[1])
            UH.SetText(self.GetWayUnlockText, "")
        end
    elseif way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpView then
        UH.SetText(self.GetWayText, string.format(Language.Common.Xstr, COLORSTR.Red6, way_cfg[1].catch_way)..Language.Pet.GetCatch)
        local level
        self.show_get_way_btn, _, level = FunOpen.Instance:GetFunIsOpened(way_cfg[1].catch_link)
        if self.show_get_way_btn then
            self.GoToGetBtn:SetData(way_cfg[1])
            UH.SetText(self.GetWayUnlockText, "")
        else
            UH.SetText(self.GetWayUnlockText, string.format(Language.Pet.OpenAtLevel, level))
        end
    elseif way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpNpc then
        self.show_get_way_btn = true
        UH.SetText(self.GetWayText, string.format(Language.Common.Xstr, COLORSTR.Red6, way_cfg[1].catch_way)..Language.Pet.GetCatch)
        self.GoToGetBtn:SetData(way_cfg[1])
    else
        UH.SetText(self.GetWayText, string.format(Language.Common.Xstr, COLORSTR.Red6, way_cfg[1].catch_way)..Language.Pet.GetCatch)
        UH.SetText(self.GetWayUnlockText, "")
    end
    self.GoToGetBtn:SetObjActive(self.show_get_way_btn)

    UH.SetText(self.LblLevel, "+1")
    UH.SetText(self.LblName, data.name)
    UH.SetText(self.LblType, self.lang.RaceClass[data.config.race])
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr6, data:MaxBp()))
    UH.SpriteName(self.ImgType, PetData.PetLevel2[data:Quality()])

    -- self.XiyouBg:SetActive(data.config.quality == 4)

    -- self.ToggleBtnPar:SetActive(data.config.quality ~= 5) --神兽不显示变异\普通的标签按钮
    --self.ToggleBtnMutant:SetActive(data.config.quality ~= 5)

    -- self.ShenShouBg:SetActive(data.config.mutant_times == 0 and data.config.quality == 5)
    -- self.NormalBg:SetActive(data.config.mutant_times == 0 and data.config.quality ~= 5)
    -- self.MutantBg:SetActive(data.config.mutant_times >= 1)

    PetView.SetTypeBgImage(self.TypeBgSetter,data.config.quality,data.config.mutant_times,false,data:IsLianYao())

    if Pet.CanMutantMaxLv(data.config.quality) >= PetData.MutantLv.Two then --显示二变标签
        self.Mutant:SetMutant(PetData.MutantLv.Zero)
        if data.config.mutant_times >= 1 then
            self.MutantToggle:SetObjActive(true)
            self.MutantToggle.isOn = data.config.mutant_times == PetData.MutantLv.One
        else
            self.MutantToggle:SetObjActive(false)
        end
    else
        self.MutantToggle:SetObjActive(false)
        self.Mutant:SetMutant(data.config.mutant_times)
    end

    -- if data.config.quality ~= 5 then
    --     for i=1, self.ToggleBtnRect:Length() do
    --         self.ToggleBtnRect[i].sizeDelta = Vector2.New(i == 1 and 138 or 113, 58)
    --     end
    -- else
    --     for i=1, self.ToggleBtnRect:Length() do
    --         self.ToggleBtnRect[i].sizeDelta = Vector2.New(182, 58)
    --     end
    -- end
    for i=1, self.ToggleBtnRect:Length() do
        self.ToggleBtnRect[i].sizeDelta = Vector2.New(i == 1 and 138 or 113, 58)
    end
    
    UH.SpriteName(self.ImgRace, string.format("Race%s", data:RaceId()))
    UH.SpriteName(self.ImgRaceName, string.format("_LocRaceName%s", data:RaceId()))

    local cfg = data:PetCfg()
    self.PetTradeBtn:SetActive(cfg.bugb_button == 1)

    self:FlushPetModel()
end

function PetBookPanel:FlushPetModel()
    local pet = self.pet_data:GetBookSelect()
    if pet == nil then
        return
    end
    if self.cur_mutant_toggle == 2 then
        if self.pet_data:GetSkinHasIsActive(self.pet_data.cur_book_skin.id) then
            if self.pet_data.cur_book_skin.use_skin and self.pet_data.cur_book_skin.skin_data ~= nil then
                self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet_data.cur_book_skin.skin_data.res_id))
                self.ui_obj:PlayAnim(ChAnimType.Show)
            end
        else
            local res_id = pet:ResId()
            if self.pet_data:GetPetSkinCfgList(pet:ID()) then --皮肤标签
                if self.pet_data.cur_book_skin.skin_data ~= nil and self.pet_data.cur_book_skin.try_skin then --该宠物皮肤为试穿或装备状态
                    res_id = self.pet_data.cur_book_skin.skin_data.res_id
                end
            end
            self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
            if self.pet_data.cur_book_skin.try_skin or self.showed_skin then
                self.ui_obj:PlayAnim(ChAnimType.Show)
            end
            self.showed_skin = true
        end
    elseif self.curResid == -1 or self.curResid ~= pet:ResId() or self.showed_skin then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
        if not self.showed_skin then
            self.ui_obj:PlayAnim(ChAnimType.Show)
        end
        self.curResid = pet:ResId()
        self.showed_skin = false
    end
    if not StringIsEmpty(pet.config.limit_type) then
        self.LimitSign:SetObjActive(true)
        self.LimitSign:SetType(pet:LimitType())
    else
        self.LimitSign:SetObjActive(false)
    end
    -- 专属魂器信息
    local exc_weapon = pet:ExclusiveWeapon()
    self.PetWeapon:SetObjActive(exc_weapon ~= nil)
    if exc_weapon then
        self.PetWeapon:SetData({is_show = true, pet_weapon = exc_weapon})
    end
    self:FlushTale()
end

function PetBookPanel:FlushTale()
    local pet = self.pet_data:GetBookSelect()
    if pet == nil then
        return
    end
    -- 传记按钮
    self.TaleBtnObj:SetActive(PetTaleData.Instance:IsOpenTale(pet))
    local mutant_id = pet:PetCfg().mutant_id
    local red_num = PetTaleData.Instance:TaleOneRemind(mutant_id)
    self.TaleBtnRed:SetNum(red_num)
end

function PetBookPanel:OnDisable()
    self.pet_data:SetBookSkinSelect(-1)
end

function PetBookPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
    if self.delay_fresh_size ~= nil then
        TimeHelper:CancelTimer(self.delay_fresh_size)
        self.delay_fresh_size = nil
    end
    self.cur_mutant_toggle = 0
    self.curResid = -1
    self.ui_obj = nil
    PetData.Instance:SetRaceIndex(-1)
    PetData.Instance:SetAttrIndex(-1)
    PetData.Instance:SetBookSelect({})
end

-- 宠物列表 分类点击
function PetBookPanel:OnClickItem(data)
    local race_index = self.pet_data:GetRaceIndex()
    local attr_index = self.pet_data:GetAttrIndex()
    if data[2] == PetData.Race then
        if race_index == data[1] then
            self.BtnParent:SetActive(false)
            return
        end
        UH.SetText(self.LblRace, data.name)
        self.pet_data:SetRaceIndex(data[1])
    else
        if attr_index == data[1] then
            self.BtnParent:SetActive(false)
            return
        end
        UH.SetText(self.LblAttr, data.name)
        self.pet_data:SetAttrIndex(data[1])
    end
    self.BtnParent:SetActive(false)
    self:Flush()
end

function PetBookPanel:OnTaleClick()
    ViewMgr:OpenView(PetTaleView)
end

function PetBookPanel:OnClickToggle(param)
    if self.cur_mutant_toggle == param then
        return
    end
    self.cur_mutant_toggle = param
    local pet = self.cur_select_cell and self.cur_select_cell:GetData() or self.pet_data:GetBookSelect()
    local new_pet = nil
    if param == 0 then
        self.pet_data.cur_book_skin.try_skin = false
        self.pet_data.cur_book_skin.use_skin = false
        self.pet_data:SetBookSkinSelect(-1)
        new_pet = self.pet_data:GetPetCfgByNormalId(pet.config.prototype_id)
    elseif param == 1 then
        self.pet_data.cur_book_skin.try_skin = false
        self.pet_data.cur_book_skin.use_skin = false
        self.pet_data:SetBookSkinSelect(-1)
        new_pet = self.pet_data:GetMutantPetCfgByNormalId(pet.config.prototype_id,PetData.MutantLv.One)
    else
        new_pet = self.pet_data:GetBookSelect():Val()
    end
    self.AttrObj:SetActive(param ~= 2)
    self.SkinObj:SetActive(param == 2)

    if self.cur_select_cell ~= nil then
        self.cur_select_cell:SetMutant(param == 1)
    end
    -- if pet.config.quality ~= 5 then
    --     for i=1, self.ToggleBtnRect:Length() do
    --         self.ToggleBtnRect[i].sizeDelta = Vector2.New(i == param + 1 and 138 or 113, 58)
    --     end
    -- else
    --     for i=1, self.ToggleBtnRect:Length() do
    --         self.ToggleBtnRect[i].sizeDelta = Vector2.New(182, 58)
    --     end
    -- end

    for i=1, self.ToggleBtnRect:Length() do
        self.ToggleBtnRect[i].sizeDelta = Vector2.New(i == param + 1 and 138 or 113, 58)
    end

    self.pet_data:SetBookSelect(new_pet)
end

function PetBookPanel:OnMutantClick(is_on)
    local pet = self.cur_select_cell and self.cur_select_cell:GetData() or self.pet_data:GetBookSelect()
    -- self.pet_data.cur_book_skin.try_skin = false
    -- self.pet_data.cur_book_skin.use_skin = false
    -- self.pet_data:SetBookSkinSelect(-1)
    local new_pet = self.pet_data:GetMutantPetCfgByNormalId(pet.config.prototype_id,is_on and 1 or 2)
    self.pet_data:SetBookSelect(new_pet)
end

-- 强化等级
function PetBookPanel:OnClickForceItem(data)
    self.StrBtnParent:SetActive(false)
    local cell_data = self.pet_data:GetBookSelect()
    if data.add_bp > 0 then
        UH.SetText(self.LblValue, string.format(self.lang.GrowthStr5, cell_data:MaxBp(), COLORSTR.Green11, tostring(data.add_bp / 100)))
    else
        UH.SetText(self.LblValue, string.format(self.lang.GrowthStr6, cell_data:MaxBp()))
    end
    PetData.SetStrLevel(nil, self.ImgLevel, data.level)
    UH.SetText(self.LblLevel, data.name)
    self.pet_data:SetPetStrengthenLevel(data.level)
    self.AttrList:SetData(self.lang.AttrClass)
    self.gm_add_level = data.level
end

function PetBookPanel:OnClickBtnPanelParent()
    self.StrBtnParent:SetActive(false)
end

function PetBookPanel:OnClickPetTrade()
    local pet = self.pet_data:GetBookSelect()
    TradeData.Instance:SetSearchItemId(pet.pet_id)
    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
end

function PetBookPanel:OnClickWayBtn(data)
    -- local way_config = BagData.Instance:GetWayCfg(data.catch_link)
    if data.jump_type == PetData.CatchWayJumpType.JumpScene then
        if data.catch_link ~= "" then
            -- 跳转场景后，打开自动抓宠设置界面
            --组队不生效
            if TeamData.Instance:InTeam() then
                PublicPopupCtrl.Instance:Center(Language.Fight.Seal.TeamTip)
                return
            end
            SceneLogic.Instance:AutoToPos(data.catch_link, nil, nil, nil, true)
            ViewMgr:OpenView(FightAutoSealSetView,{["scene_id"] = data.catch_link})
            ViewMgr:CloseView(PetView)
        end
    elseif data.jump_type == PetData.CatchWayJumpType.JumpShop then
        MallCtrl.Instance:Jump(data.catch_link)
        ViewMgr:CloseView(PetView)
    elseif data.jump_type == PetData.CatchWayJumpType.JumpView then
        -- LogError(data.catch_link,"::B")
        ViewMgr:OpenViewByKey(data.catch_link)
        ViewMgr:CloseView(PetView)
    -- elseif data.jump_type == PetData.CatchWayJumpType.JumpHunt then
    elseif data.jump_type == PetData.CatchWayJumpType.JumpNpc then
        -- LogError(data.catch_link,"::C")
        SceneLogic.Instance:AutoToNpc(data.catch_link,nil,false,true)
        ViewMgr:CloseView(PetView)
    else--if data.jump_type == PetData.CatchWayJumpType.JumpNone then
        LogError("宠物获取途径跳转类型 ",data.jump_type," 不存在")
    end
end

function PetBookPanel:OnClickAddBtn()
    local data = self.pet_data:GetBookSelect()
    GMCmdCtrl.Instance:SendGMCommand("addpet", data.pet_id.." "..self.gm_add_level)
end

-----------------------------------------------
PetBookSkin = PetBookSkin or DeclareMono("PetBookSkin", UIWFlushPanel)
function PetBookSkin:PetBookSkin()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_book_cell, func = self.FlushSkinInfo},
        {data = self.pet_data.cur_book_skin, func = self.FlushSkinInfo, keys = {"id"}},
    }
end

function PetBookSkin:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.pet_data:SetBookSkinSelect(-1)
    self.pet_data.cur_book_skin.try_skin = false
end

function PetBookSkin:FlushSkinInfo()
    local cur_pet = self.pet_data:GetBookSelect()
    if cur_pet == nil then
        self:showEmpty()
        return
    end
    local skin_list = self.pet_data:GetPetSkinCfgList(cur_pet:ID())
    if skin_list ~= nil and next(skin_list) then
        self.NoSkinObj:SetActive(false)
        self.SkinPanelObj:SetActive(true)
        self.SkinList:SetData(skin_list)
        if self.pet_data.cur_book_skin.id ~= -1 then
            self:FlushSkinDesc()
        else
            self.SkinList:ClickItem(1)
        end
    else
        self:showEmpty()
        return
    end
end

function PetBookSkin:FlushSkinDesc()
    local cur_data = self.pet_data.cur_book_skin.skin_data
    if cur_data == nil then
        return
    end
    local has_active = self.pet_data:GetSkinHasIsActive(self.pet_data.cur_book_skin.id)
    UH.SetText(self.SkinNameText, cur_data.name)
    
    local btn_str = self.pet_data.cur_book_skin.try_skin and Language.Pet.PetSkinTry[2] or Language.Pet.PetSkinTry[1]
    self.BtnTryObj:SetActive(not has_active)
    self.BtnUseObj:SetActive(has_active)
    if has_active then
        local cur_pet = self.pet_data:GetBookSelect()
        if cur_pet ~= nil then
            btn_str = cur_pet.info.skin_id == self.pet_data.cur_book_skin.id and Language.Pet.PetSkinPutOff or Language.Pet.PetSkinUse
        end
        UH.SetText(self.SkinDescText, cur_data.describe)
        UH.SetText(self.BtnText, btn_str)
    else
        UH.SetText(self.SkinDescText, Language.Pet.PetGetway..":<color=#"..COLORSTR.Red5..">"..Language.Pet.PetSkinGet.."</color>\n"..cur_data.describe)
        UH.SetText(self.BtnTryText, btn_str)
    end
end

function PetBookSkin:showEmpty()
    self.NoSkinObj:SetActive(true)
    self.SkinPanelObj:SetActive(false)
end

function PetBookSkin:OnClickSkin(data)
    self.pet_data:SetBookSkinSelect(data.pet_skin_id, data)
end

function PetBookSkin:OnClickUse()
    if self.pet_data:GetSkinHasIsActive(self.pet_data.cur_book_skin.id) then
        local pet_list = self.pet_data:GetPetList()
        -- LogError("skin_data",self.pet_data.cur_book_skin.skin_data)
        local skin_pet_id = string.split(self.pet_data.cur_book_skin.skin_data.pet_id,"|")
        for k,v in pairs(skin_pet_id) do
            skin_pet_id[k] = tonumber(v)
        end
        -- LogError("skin_pet_id",skin_pet_id,type(skin_pet_id[1]), self.pet_data.cur_book_skin.id)
        local has_pet = false
        for k,v in pairs(pet_list) do
            for i = 1, #skin_pet_id do 
                if v.pet_id == skin_pet_id[i] then
                    if self.pet_data.cur_book_skin.id ~= v.info.skin_id then
                        PetCtrl.SendPetOp(PET_OP_TYPE.UsePetSkin, v.info.index, self.pet_data.cur_book_skin.id) --穿戴皮肤
                    end
                    has_pet = true
                end
            end
        end
        self.pet_data.cur_book_skin.use_skin = true
        PublicPopupCtrl.Instance:Center(has_pet and Language.Pet.PetSkinEquipSucc or Language.Pet.PetSkinEquipNotHasInList)
    end
end

function PetBookSkin:OnClickTry()
    self.pet_data.cur_book_skin.try_skin = not self.pet_data.cur_book_skin.try_skin
    UH.SetText(self.BtnTryText, self.pet_data.cur_book_skin.try_skin and Language.Pet.PetSkinTry[2] or Language.Pet.PetSkinTry[1])
end

function PetBookSkin:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[68].desc})
end

------------------------------------------------------------
AllPetItem = DeclareMono("AllPetItem", UIWidgetBaseItem)
function AllPetItem:AllPetItem()
    self.pet_data = PetData.Instance
end
function AllPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if next(data) == nil then
        return
    end
    local temp = data[1]
    UH.SpriteName(self.ImgType, string.format("_LocPet%d", temp:Quality()))
    self.PetList:SetData(data)
    if temp:Quality() == 1 then
        self.PetList:ClickItem(1)
        if not self.PetList:ClickItem(1) then
            self.pet_data:SetBookSelect(data[1])
        end
    end
end

function AllPetItem:ExpendSize(data, axis)
    local re = self.ScrollGrid:CacuSize(#data, axis)
    return re + 60
end
------------------------------------------------------------
--宠物图标
PetBookItem = DeclareMono("PetBookItem", UIWidgetBaseItem)
-- Icon = UIImageSpriteSet
-- CatchIcon = UIImageSpriteSet
-- ItemClick = LuaUIEventClick
-- Mutant = GameObject
-- Interactor = InteractorHandler
-- UnLockText = Text
function PetBookItem:SetData(data)
    self.data = data
    CellFlushs.SetIcon(self, data:IconId())
    UH.SpriteName(self.CatchIcon, data:QuaIcon())
    self.Mutant:SetActive(data.config.mutant_times >= 1)

    self.has_get = PetData.Instance:PetHasGeted(data:ID())
    self.Interactor.Interactable = self.has_get
    self.ArrowObj:SetActive(ServerActivityData.Instance:IsCanPromote(data:ID()))
    local way_cfg = PetData.Instance:GetCatchWay(data:CatchWay())
    if way_cfg[1].jump_type == PetData.CatchWayJumpType.JumpScene and not self.has_get then
        local map_cfg = MapData.Instance:GetMapShowData(way_cfg[1].catch_link)
        local limit_level = MapData.Instance:GetMapMinLevel(map_cfg.restrict_in)
        if limit_level > RoleData.Instance:GetRoleLevel() then
            UH.SetText(self.UnLockText, string.format(Language.Pet.CanCatchLevel, limit_level))
        else
            UH.SetText(self.UnLockText, "")
        end
    else
        UH.SetText(self.UnLockText, "")
    end
    
    if self.LimitFlag then
        local limit_type = data:LimitType()
        self.LimitFlag:SetActive(limit_type > 0)
    end

    local mutant_id = data:PetCfg().mutant_id
    local red_num = PetTaleData.Instance:TaleOneRemind(mutant_id)
    self.RedPoint:SetNum(red_num)
end

function PetBookItem:SetMutant(is_mutant)
    if self.has_get then
        self.Mutant:SetActive(is_mutant)
    end
end

function PetBookItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
--------------------------------------------------------
--宠物属性格子
PetBookAttrItem = DeclareMono("PetBookAttrItem", UIWidgetBaseItem)
--data来自 Language
function PetBookAttrItem:PetBookAttrItem()
    self.pet_data = PetData.Instance
end
function PetBookAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if next(data) == nil then
        return
    end
    self.LblType.text = data[3]
    local cell_data = self.pet_data:GetBookSelect()
    local zz = cell_data:PetCfg()[data[2]]
    for i = 1, 5 do
        if i <= zz then
            self.StarList[i].fillAmount = 1
        else
            self.StarList[i].fillAmount = 0
        end
        if i - zz == 0.5 then
            self.StarList[i].fillAmount = 0.5
        end
    end
    local str_level = self.pet_data:GetPetStrengthenLevel()
    local str_cfg = self.pet_data:GetStrengthenCfg(cell_data.config.strengthen_id, str_level)
    local str = cell_data:PetCfg()[data[1]]
    local add = str_cfg[data[5]]
    if add > 0 then
        str = string.format(Language.Pet.AttrStr, str, COLORSTR.Green11, tostring(add / 100))
    end
    UH.SetText(self.LblValue, str)
end
--------------------------------------------
PetTypeItem = DeclareMono("PetTypeItem", UIWidgetBaseItem)
function PetTypeItem:PetTypeItem()
    self.lang = Language.Pet
end
function PetTypeItem:SetData(data)
    local str = ""
    if data[2] == PetData.Race then
        str = self.lang.RaceClass[data[1]]
    else
        str = data[1] == -1 and self.lang.Default or self.lang.AttrClass[data[1]][4]
    end
    UH.SetText(self.Name, str)
    data.name = str
    UIWidgetBaseItem.SetData(self, data)
end
----------------------------------------------
--[[ function PetTypeItem:Click()
self.GiftCell:Click()
end ]]
-- 强化按钮
PetForceItem = DeclareMono("PetForceItem", UIWidgetBaseItem)
function PetForceItem:PetForceItem()
    self.lang = Language.Pet
end
function PetForceItem:SetData(data)
    local showname = string.format("+%d", data.level)
    UH.SetText(self.Name, showname)
    data.name = showname
    UIWidgetBaseItem.SetData(self, data)
end
----------------获取途径按钮-----------------------------------
PetGetWayBtnItem = PetGetWayBtnItem or DeclareMono("PetGetWayBtnItem",UIWidgetBaseItem)
function PetGetWayBtnItem:PetGetWayBtnItem()
end
function PetGetWayBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.WayText, data.catch_way)
    --LogError("data",data)
end

-----------百科界面皮肤item-----------------------------
PetBookSkinItem = PetBookSkinItem or DeclareMono("PetBookSkinItem", UIWidgetBaseItem)
function PetBookSkinItem:PetBookSkinItem()
    self.pet_data = PetData.Instance
end

function PetBookSkinItem:SetData(data)
    self.data = data
    local cur_pet = self.pet_data:GetBookSelect()
    if cur_pet ~= nil then
        self.OnUseObj:SetActive(cur_pet:SkinId() == data.pet_skin_id)
        self.SkinImg:SetByPath(string.format("pet_skin/%s.png",self.data.res_id))
    end
    self.SkinSign:SetObjActive(not StringIsEmpty(self.data.limit_sign))
    if not StringIsEmpty(self.data.limit_sign) then
        self.SkinSign:SetData(self.data)
    end
end

function PetBookSkinItem:Click()
    if self.Toggle.isOn then
        self.pet_data:SetBookSkinSelect(self.data.pet_skin_id, self.data)
    else
        self.Toggle.isOn = true
    end
end