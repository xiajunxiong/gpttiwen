PetAttrPanel = PetAttrPanel or DeclareMono("PetAttrPanel", UIWFlushPanelGroup)
function PetAttrPanel:PetAttrPanel()
    self.flush_events = {
        EventProtocol.ToEventKey(SCSendPetAttrListInfo)
     --属性变化
    }
    self.data_cares = {
        {data = PetData.Instance.pet_select,func = self.onFlush, keys = {"index"},init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("PetZhiZhi", function ()
        return self.BtnGiftObj
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetGaiMingBtn", function ()
        return self.BtnGaiMing
    end)
end

function PetAttrPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    PetData.Instance:SetSelTab(Mod.Pet.PetAttr)

end

function PetAttrPanel:onFlush()
    local pet = PetData.Instance:GetCurSelect()
    if pet == nil then
        return
    end
    --[[ if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnGiftObj, Vector3.New(58, 17))
        self.red_point:SetNum(0)
    end ]]
    self.RedPoint:SetNum(PetData.Instance:CanOpenGift(pet))
    if self.red_point_upgrade == nil then
        self.red_point_upgrade = UiRedPoint.New(self.BtnUpgradeObj, Vector3.New(31, 17))
        self.red_point_upgrade:SetNum(0)
    end
    self.red_point_upgrade:SetNum(PetData.Instance:CanUpgradePoint(pet))
end

function PetAttrPanel:OnDisable()
    -- PetData.Instance.cur_select_skin.id = -1
    if ViewMgr:IsOpen(PetExp) then
        ViewMgr:CloseView(PetExp)
    end
end

function PetAttrPanel:OnClickFeiSheng()
    ViewMgr:OpenView(PetFeiSheng)
end

----------------------------------------------
PetAttrShowInfoPanel = PetAttrShowInfoPanel or DeclareMono("PetAttrShowInfoPanel", UIWFlushPanel)
--TypeBgSetter=======UIWTextureSetter
--Mutant=============PetMutantTag
--BtnFeiShengTxt=====Text
--BtnFeiSheng========GameObject
function PetAttrShowInfoPanel:PetAttrShowInfoPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    self.curResid = -1
    self.select_toggle_index = 1
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
    self.data_cares = {
        {data = self.pet_data.pet_select, func = self.FlushPetInfo, keys = {"index"}},
        {data = self.pet_data.pet_other_info, func = self.FlushPetInfo, init_call = false},
        {data = self.pet_data.pet_list, func = self.FlushPetInfo, init_call = false},
        {data = self.pet_data.cur_select_skin, func = self.FlushPetModel, keys = {"id","try_skin"}, false},
		{data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.FlushAttributeRemind},
		{data = AttributeDanData.Instance.attribute_red_piont_pet,func = self.FlushAttributeRemind},
    }
    GuideManager.Instance:RegisterGetGuideUi("PetFreeLifeBtn",function ()
        return self.PetFreeLifeBtn
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetSingleFreeBtn", function ()
        return self.PetSingleFreeBtn
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetFightToggleBtn", function ()
        return self.PetFightBtn
    end)
    -- self.pet_info_care = self:CareData(self.pet_info_care, self.pet_data.pet_list, self.FlushPetInfo, {data.index})
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems({26502,26503},BindTool.Bind(self.FlushPetInfo,self))
    end
end

function PetAttrShowInfoPanel:FlushAttributeRemind()
	local pet = self.pet_data:GetCurSelect()
	if pet then
		if self.attribute_redpoint == nil then
			self.attribute_redpoint = UiRedPoint.New(self.attributedanPos, Vector3.New(23,23,0))
		end
		self.attribute_redpoint:SetNum(self.pet_data:CanAttributePoint(pet))
	end
end

function PetAttrShowInfoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    self.curResid = -1
    GuideManager.Instance:UnRegisterGetGuideUi("PetFreeLifeBtn")
    GuideManager.Instance:UnRegisterGetGuideUi("PetSingleFreeBtn")
    GuideManager.Instance:UnRegisterGetGuideUi("PetFightToggleBtn")
    BagData.Instance:RemoveListenItems(self.item_listen)
end

function PetAttrShowInfoPanel:FlushPetModel()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    if self.BaseInfoPanel.show_type == 3 then
        local res_id = pet:ResId()
        if self.pet_data:GetPetSkinCfgList(pet:ID()) then --皮肤标签
            if self.pet_data.cur_select_skin.skin_data ~= nil and self.pet_data.cur_select_skin.try_skin then --该宠物皮肤为试穿或装备状态
                res_id = self.pet_data.cur_select_skin.skin_data.res_id
            end
        end
        self.showed_skin = true
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id), pet:FeiShengTimes())
        if self.pet_data.cur_select_skin.skin_data ~= nil then--and res_id == self.pet_data.cur_select_skin.skin_data.res_id then
            self.ui_obj:PlayAnim(ChAnimType.Show)
        end
        -- self.ui_obj:PlayAnim(ChAnimType.Show)
    elseif self.curResid == -1 or self.curResid ~= pet:ResId() or self.showed_skin then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()), pet:FeiShengTimes())
        if not self.showed_skin then
            self.ui_obj:PlayAnim(ChAnimType.Show)
        end
        self.curResid = pet:ResId()
        self.showed_skin = false
    end
    local can_show_feisheng = pet:FeiShengTimes() < pet:CanFeiShengMaxLv()
    self.BtnFeiSheng:SetActive(can_show_feisheng)
    if can_show_feisheng  then
        UH.SetText(self.BtnFeiShengTxt,PetFeiSheng.LvName(pet:FeiShengTimes() + 1))
    end
    --稀有标识与飞升按钮互斥
    if not StringIsEmpty(pet.config.limit_type) and can_show_feisheng == false then
        self.LimitSign:SetObjActive(true)
        self.LimitSign:SetType(pet:LimitType())
    else
        self.LimitSign:SetObjActive(false)
    end
end

function PetAttrShowInfoPanel:FlushPetInfo()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local fly_flag = pet:FeiShengTimes()
    self.NameFrame.enabled = fly_flag == 0
    UH.SpriteName(self.FlyBgSet, (fly_flag == 0 and "" or ("ChongWuMingChenFeiSheng"..fly_flag)))

    UH.SetText(self.PetName, pet.name)
    UH.Color(self.PetName, pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
    UH.SpriteName(self.LevelSprite, PetData.PetLevel[pet.config.quality])
    UH.SpriteName(self.PetLock, PetData.PetLock[pet.lock])

    local cfg2 = pet:PetCfg()
    if cfg2 then
        self.NormalBg:SetActive(cfg2.mutant_times == PetData.MutantLv.Zero and cfg2.quality < PetData.Quality.Shen)
        PetView.SetTypeBgImage(self.TypeBgSetter,cfg2.quality,cfg2.mutant_times,true,pet:IsLianYao())
        -- self.MutantBg:SetActive(cfg2.mutant_times >= 1)
        self.Mutant:SetMutant(cfg2.mutant_times)
        -- self.ShenShouBg:SetActive(cfg2.mutant_times == 0 and cfg2.quality == 5)
        self.BtnInborn:SetActive(cfg2.inborn_type > 0 and FunOpen.Instance:IsFunOpen(Mod.PetInborn.Main))
        local state = pet:State()
        if state == PetState.Fight or state == PetState.StandBy then
            self.BtnInbornRemind:SetNum(PetInbornData.Instance:GetPetInbornFlag(pet))
        else
            self.BtnInbornRemind:SetNum(0)
        end
    end
    UH.SetText(self.PetTypeText, self.lang.RaceClass[pet.config.race])
    UH.SpriteName(self.PetTypeImg, string.format("Race%s", pet:RaceId()))
    UH.SpriteName(self.PetTypeNameImg, string.format("_LocRaceName%s", pet:RaceId()))

    local score_num, score_level = pet:GetScore()
    UH.SetText(self.PetScore, score_num)
    UH.SpriteName(self.PetScoreLevel, "scoreLevel"..tostring(score_level))
    if pet:Rank() >= 0 and pet:Rank() <= 9 then
        self.PetRank:SetActive(true)
        self.PetRank:SetRank(pet:Rank() + 1)
    else
        self.PetRank:SetActive(false)
    end
    local has_buff,_, buff_level = PetData.Instance:CheckPetStrBuff(pet.index)
    --UH.SetText(self.BuffLevelText, has_buff and string.format(Language.Pet.BuffStrFormat, buff_level) or Language.Pet.notHasBuff)
    UH.SetText(self.BuffLevelText, Language.Pet.BuffNewTxt)

    self.BianYiBtn:SetActive(Pet.CanMutant(pet:Quality(),pet:MutTimes(),pet:FeiShengTimes()) and 
        Item.GetNum(cfg2.change_consume_item) > 0)
    local is_open_pray, open_pray_tip = FunOpen.Instance:IsFunOpen(Mod.Pet.PetPray)
    self.BtnPrayInteractor.Interactable = is_open_pray
    self.BtnPrayMask:SetActive(pet:PrayMainIndex() ~= nil or not is_open_pray)
    if self.select_toggle_index == 3 then
        if pet:PrayMainIndex() ~= nil then
            self.Toggles[3].isOn = false
            self.Toggles[1].isOn = true
            self.select_toggle_index = 1
            PublicPopupCtrl.Instance:Center(Language.PetPray.Error1)
        end
    end

    self:FlushPetState(pet.index)
    self:FlushPetModel()

    self:FlushPetWeapon()
    self:FlushAttributeRemind()
end

function PetAttrShowInfoPanel:FlushPetWeapon()
    local weapon_data = {}
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        self.PetWeapon:SetData(weapon_data)
        self.PetWeapon2:SetObjActive(false)
        return
    end
    local is_open,open_tip = FunOpen.Instance:IsFunOpen(Mod.PetWeapon.Main)
    if not is_open then
        self.PetWeapon:SetData(weapon_data)
        self.PetWeapon2:SetObjActive(false)
        return
    end
    local pet_weapon = pet:Weapon()
    weapon_data.is_show = true
    weapon_data.pet_weapon = pet_weapon
    self.PetWeapon:SetData(weapon_data)
    if pet:IsLianYao() then
        self.PetWeapon2:SetObjActive(true)
        local pet_weapon2 = pet:Weapon2()
        weapon_data.is_show = true
        weapon_data.pet_weapon = pet_weapon2
        self.PetWeapon2:SetData(weapon_data)
    else
        self.PetWeapon2:SetObjActive(false)
    end
end

function PetAttrShowInfoPanel:FlushPetState(pet_index)
    local fight_index = self.pet_data:GetFightIndex()
    local isStandBy = self.pet_data:GetIsStandBy(pet_index)
    self.PlayToggle.isOn = fight_index == pet_index
    self.WaitToggle.isOn = isStandBy
    self.RestToggle.isOn = fight_index ~= pet_index and not isStandBy

end

function PetAttrShowInfoPanel:OnClickBuffLevelIcon()
    -- local pet = self.pet_data:GetCurSelect()
    -- if pet == nil then
    --     return
    -- end
    -- ViewMgr:OpenView(PetStrBuff)
    ViewMgr:OpenView(PetResonanceView)
end

function PetAttrShowInfoPanel:OnInbornBtnClick()
    ViewMgr:OpenViewByKey(Mod.PetInborn.Main)
end

function PetAttrShowInfoPanel:OnClickState(state)
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local cur_state = self.pet_data:GetPetState(pet.index)
    if cur_state == state then
        return
    end
    if state ~= PetState.Relax and cur_state == PetState.Train then
        PublicPopupCtrl.Instance:Center(ErrorText[45])
        return
    end
    local p2 = state
    if state == PetState.StandBy then
        local flag = self.pet_data:GetStandbyCanUse()
        if not flag and cur_state ~= PET_STATE_OP.FIGHT then
            PublicPopupCtrl.Instance:Center(Format(Language.Pet.PetOnFightMax,PetData.Instance:GetMaxPetOn()))
            return
        end
        p2 = PET_STATE_OP.WAIT1
    end
    if state == PetState.Relax then
        p2 = PET_STATE_OP.RELAX
    end
	local role_id = RoleData.Instance:GetRoleId()
	if p2 == PET_STATE_OP.WAIT1 or p2 == PET_STATE_OP.FIGHT then
		if cur_state ~= PetState.Fight and cur_state ~= PetState.StandBy then
			UnityPlayerPrefs.SetInt(role_id.."pet_attribute_dan"..pet.index,2)
		end
	else
		UnityPlayerPrefs.SetInt(role_id.."pet_attribute_dan"..pet.index,1)
	end 
	AttributeDanData.Instance:RemindPetDataRole()

	PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet.index, p2)
    if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene then
        PublicPopupCtrl.Instance:Center(ErrorText[110])
    end
end

function PetAttrShowInfoPanel:OnClickLock()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.Lock, pet.index)
end

function PetAttrShowInfoPanel:OnClickChangeName()
    if RoleCtrl.NationalDayCheck() then return end
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    ViewMgr:OpenView(PetName)
end
function PetAttrShowInfoPanel:OnClickEcho()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    ViewMgr:OpenView(PetStrBuff)
end

function PetAttrShowInfoPanel:OnClickStorage()
    ViewMgr:OpenView(PetStorage)
end
function PetAttrShowInfoPanel:OnClickAttrExplain()
    ViewMgr:OpenView(AttrExplainView)
end

function PetAttrShowInfoPanel:OnClickToggle(param)
    self.select_toggle_index = param
    AudioMgr:PlayEffect(AudioType.UI, "click_tongyong3")
    --点击天赋时关闭经验药水界面
    if ViewMgr:IsOpen(PetExp) then 
        if param == 3 or param == 2 then
            ViewMgr:CloseView(PetExp)
        end
    end
    for i=1, self.ToggleBtnRect:Length() do
        self.ToggleBtnRect[i].sizeDelta = Vector2.New(i == param and 138 or 118, 58)
    end
    self:FlushPetModel()
end

function PetAttrShowInfoPanel:OnClickFreeLife(param)
    if param == 0 then
        self.FreeLifeSelectObj:SetActive(not self.FreeLifeSelectObj.activeSelf)
    elseif param == 1 then
        local pet = self.pet_data:GetCurSelect()
        if pet == nil then
            return
        end
        if pet then
            local state = PetData.Instance:GetPetState(pet.index)
            if state == PetState.Fight then
                PublicPopupCtrl.Instance:Center(ErrorText[25])
                return
            elseif state == PetState.StandBy then
                PublicPopupCtrl.Instance:Center(ErrorText[109])
                return
            elseif state == PetState.Train then
                PublicPopupCtrl.Instance:Center(ErrorText[168])
                return
            elseif BicycleData.Instance:JumpPet(pet) then
                return
            end
            if pet.lock == 1 then
                local info = {
                content = Language.Pet.FreeSelectedLockPet,
                confirm = {
                        func = function ()
                            PetCtrl.SendPetOp(PET_OP_TYPE.Lock, pet.index)
                            ViewMgr:CloseView(DialogTipsView)
                        end
                    },
                }
                PublicPopupCtrl.Instance:DialogTips(info)
                -- PetCtrl.SendPetOp(PET_OP_TYPE.Lock, pet.index)
                return
            end
        end
        ViewMgr:OpenView(PetFreelifeTip2)
    elseif param == 2 then
        ViewMgr:OpenView(PetFreeLifeSeveral)
    end
end

function PetAttrShowInfoPanel:OnGotoRefineClick()
    
end

function PetAttrShowInfoPanel:OnClickAttributeDan()
   AttributeDanData.Instance:SetAttributeDanType(2)
   ViewMgr:OpenView(AttributeDanView)
end


function PetAttrShowInfoPanel:OnOpenPetWeaponClick()
    LogError("废弃OnOpenPetWeaponClick")
end

function PetAttrShowInfoPanel:OnBianYiClick()
    local pet = self.pet_data:GetCurSelect()
    ViewMgr:OpenView(PetVariationDialog, {pet = pet})
end

function PetAttrShowInfoPanel:OnPrayMaskClick()
    local is_open_pray, open_pray_tip = FunOpen.Instance:IsFunOpen(Mod.Pet.PetPray)
    if not is_open_pray then
        PublicPopupCtrl.Instance:Center(open_pray_tip)
        return
    end
    PublicPopupCtrl.Instance:Center(Language.PetPray.Error1)
end
----------------------------------------------
PetAttrBase = PetAttrBase or DeclareMono("PetAttrBase", UIWFlushPanel)
function PetAttrBase:PetAttrBase()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
    self.data_cares = {
        {data = self.pet_data:GetSelectSkill(), func = self.FlushBaseAttr, init_call = true},
        {data = self.pet_data.pet_other_info, func = self.FlushFollowState, init_call = false},
        {data = self.pet_data.pet_list, func = self.FlushBaseAttr, init_call = false},
        {data = self.pet_data.pet_select, func = self.FlushBaseAttr, keys = {"index"}, init_call = false}
    }
    self.old_pet = nil
    GuideManager.Instance:RegisterGetGuideUi("PetStrengthenOpen", function ()
        return self.BtnQianghuaObj, function ()
            self:OnClickQiangHua()
        end
    end)

    self.show_type = 1      -- 1显示基础属性，2显示详细属性, 3显示皮肤信息
end

function PetAttrBase:OnClickHelp()
    ViewMgr:OpenView(PetRestraintView)
end

function PetAttrBase:FlushFollowState()
    local is_cur_index = self.pet_data:GetFollowIndex() == self.pet_data:GetSelectPetIndex()
    self.PetFollowGouToggle.isOn = is_cur_index
    UH.SetText(self.PetFollowText, is_cur_index and self.lang.FollowState[2] or self.lang.FollowState[1])
end


function PetAttrBase:OnClickFollow()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local _, flag = self.pet_data:GetPetState(pet.index)
    PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet.index, flag and PET_STATE_OP.NOFOLLOW or PET_STATE_OP.FOLLOW)
end

function PetAttrBase:OnDestroy()
    GuideManager.Instance:UnRegisterGetGuideUi("PetStrengthenOpen")
    self.old_pet = nil
    UIWFlushPanel.OnDestroy(self)
end
-- function PetAttrBase:onFlush()
--     local pet = self.pet_data:GetCurSelect()
--     if pet == nil then
--         return
--     end
--     self.pet_info_care = self:CareData(self.pet_info_care, self.pet_data.pet_list, self.FlushBaseAttr, {pet.index})
-- end
function PetAttrBase:FlushBaseAttr()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    self:FlushShowType()
    -- UH.SetText(self.LblName, pet:CfgName())
    UH.SetText(self.LblLevel, string.format(self.lang.LevelStr, pet:Level()))
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr4, COLORSTR.Yellow1, COLORSTR.Green5, v1, v2))
    self.ElemAttr:SetData(pet:EleAttr())
    PetData.SetStrLevel(self.ImgJie, self.ImgJieNum, pet:StrLevel())
    local need_exp = self.pet_data:GetLevelExp(pet:Level())
    if self.old_pet == nil or self.old_pet ~= pet then
        self.ImgExpEx:SetProgress(pet:Exp() / need_exp)
        self.old_pet = pet
    else
        self.ImgExpEx:AddToProgress(pet:Exp() / need_exp)
    end
    self.ImgExpEx:SetText(string.format(Language.Common.Progress2TxtColorBFormat, pet:Exp(), need_exp))
    -- self.AttrList:SetPet(pet)
    local total_attr = self.pet_data:RealAttr()
    --pet:TotalAttr()
    if total_attr == nil then
        total_attr = pet:TotalAttr()
    end
    for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(pet:HP() / v)
            self.ProgMaxHp:SetText(string.format(Language.Common.Progress2TxtColorBFormat, pet:HP(), v))--string.format("%d/%d", pet:HP(), v))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(pet:MP() / v)
            self.ProgMaxMp:SetText(string.format(Language.Common.Progress2TxtColorBFormat, pet:MP(), v))--string.format("%d/%d", pet:MP(), v))
        elseif self["Lbl"..k] ~= nil then
            UH.SetText(self["Lbl" .. k], v)
        end
    end
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnAddPointObj, Vector3.New(77, 20))
        self.red_point:SetNum(0)
    end
    self.red_point:SetNum(PetData.Instance:CanAddPoint(pet))--pet:PointNum())
   -- LogError("PetStrChange ",FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetStrChange))
    self.BtnQianghuaInteractor.Interactable = FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetStrChange)
    local addPointIsOpen = FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetAddPoint)
    self.BtnAddPointObj:SetActive(addPointIsOpen)
    self.BtnGuiyuanInteracor.Interactable = (pet:Level() > 1)

    UH.SpriteName(self.StrLevelImg, PetData.PetStrLevel[pet:StrLevel()])
    self:FlushFollowState()
    self:FlushBreach()
    self:FlushOtherAttri()
end

function PetAttrBase:FlushBreach()
    local pet = self.pet_data:GetCurSelect()
    local flags = PetData.Instance:IsCanBreach2(pet)
    self.BreachBtnInter.Interactable = flags[4] == 1
    self.BreachTipObj:SetActive(flags[4] == 1)
    self.BreachBtnInter.gameObject:SetActive(flags[5] == 0 and flags[4] == 1)
    self.BtnAddObj:SetActive(flags[5] ~= 0 or flags[4] ~= 1)
    self.BreachRedPoint:SetNum(flags[1])
    self.BreachEffectObj:SetActive(flags[3] == 1)
end

function PetAttrBase:OnClickReLife()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    ViewMgr:OpenView(PetReLife)
end

function PetAttrBase:OnClickGuiYuan()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    if pet.level > 1 then
        ViewMgr:OpenView(PetGuiYuan,{data = pet, fromAttr = true})
    else
        PublicPopupCtrl.Instance:Center(self.lang.CannotGuiyuan)
    end
end

function PetAttrBase:OnClickAddPoints()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetAddPoint) then
        return
    end
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    ViewMgr:OpenView(PetAddPoint)
end
function PetAttrBase:OnClickAddExp()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    ViewMgr:OpenView(PetExp)
end

function PetAttrBase:OnClickBreach()
    ViewMgr:OpenView(PetBreachView)
end

function PetAttrBase:OnClickQiangHua()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    --+10无法强化
    if pet:StrLevel() == PetData.MaxLevel then
        PublicPopupCtrl.Instance:Center(ErrorText[60])
        return
    end
    if not FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetStrChange) then
        local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Pet.PetStrChange)
        PublicPopupCtrl.Instance:Center(string.format(self.lang.StrengthenOpenLevel, open_level))
        return
    end
    -- ViewMgr:OpenView(PetStrengthen)
end

--传参(第几个, 属性类型)
function PetAttrBase:OnClickBaseAttr(obj, attr_type)
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = 195, y = -107 + pos_y}
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_PET,
        param1 = pet.index,
        attr_type = attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end

function PetAttrBase:FlushShowType()
    local btn_text_1
    local btn_text_2
    if self.show_type == 1 then
        btn_text_1 = self.lang.SkinAttri
        btn_text_2 = self.lang.OtherAttri
    elseif self.show_type == 2 then
        btn_text_1 = self.lang.SkinAttri
        btn_text_2 = self.lang.CommonAttri
    elseif self.show_type == 3 then
        btn_text_1 = self.lang.CommonAttri
        btn_text_2 = self.lang.OtherAttri
    end
    UH.SetText(self.ShowTypeBtnText1, btn_text_1)
    UH.SetText(self.ShowTypeBtnText2, btn_text_2)
    self.OtherInfoObj:SetActive(self.show_type == 2)
    self.CommonInfoObj:SetActive(self.show_type == 1)
    self.SkinInfoObj:SetActive(self.show_type == 3)
end

function PetAttrBase:FlushOtherAttri()
    local list_data = self.pet_data:GetOtherAttriListData()
    self.OtherAttriList:SetData(list_data)
end

function PetAttrBase:OnShowTypeClick(value)
    if value == 1 then
        if self.show_type == 1 or self.show_type == 2 then
            self.show_type = 3
        else
            self.show_type = 1
        end
    elseif value == 2 then
        if self.show_type == 3 or self.show_type == 1 then
            self.show_type = 2
        else
            self.show_type = 1
        end
    end
    self:FlushShowType()
end

----------------------------------------------------------------
PetAttrGift = PetAttrGift or DeclareMono("PetAttrGift", UIWFlushPanel)
function PetAttrGift:PetAttrGift()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_list, func = self.FlushGiftInfo},
        {data = self.pet_data.pet_select, func = self.FlushGiftInfo, keys = {"index"}, init_call = false}
    }
    self.cur_index = nil
    self.runner_handle = {}
end

function PetAttrGift:FlushGiftInfo()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local opened, _ = FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetGaiMing)
    self.BtnInter.Interactable = opened
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(Language.Pet.GrowthStr4, COLORSTR.Yellow1, COLORSTR.Green5, v1, v2))
    local gift_list = pet:GetSkillAndGiftListNoEmpty()
    self.AttrList:SetPet(pet)
    self.GiftList1:SetData(gift_list)
    self.cur_index = pet.index
    self.RedPoint:SetNum(self.pet_data:GaiMingRed(pet))
end

function PetAttrGift:OnClickGift(cell)
    -- local data = cell:GetData()
    -- if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
    --     self.pet_data:SetClickSkill(data)
    --     ViewMgr:OpenView(PetSkillTip)
    -- elseif data.show_lingwu then
    --     PetLingwuSkillTip.SetSlotIndex(data.index)
    --     ViewMgr:OpenView(PetLingwuSkillTip)
    -- end
    PetGiftItem.CommonClickFunc(cell)
end
function PetAttrGift:OnClickTip()
    ViewMgr:OpenView(PetGiftTip)
end

function PetAttrGift:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_delay_handle)
    for key, value in pairs(self.runner_handle) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.runner_handle = nil
    self.gift_index = nil
    self.cur_index = nil
end
function PetAttrGift:OnClickGaiMing()
    local opened, tip = FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetGaiMing)
    if opened == false then
        PublicPopupCtrl.Instance:Center(tip)
        return
    end
    ViewMgr:OpenViewByKey(Mod.Pet.PetGaiMing)
end

-----------------------------------------------------------
PetGiftItem = DeclareMono("PetGiftItem", UIWidgetBaseItem)
--BianYiQua===========GameObject
--BianYi2Qua==========GameObject
function PetGiftItem:Awake()
    self.awaked = true
    if self.need_set_up_pos and self.setPosRunner == nil then
        self:SetCanUpPos()
    end
end
--BtnOpen
function PetGiftItem:SetData(data)
    local skill_cfg = nil
    local new_data = TableCopy(data)
    local desc_cfg = nil
    -- 异兽临世不需要额外加颜色
    if self.NotAddColor ~= true then
        if self.LightBg ~= nil then
            UH.Color(self.Name, self.LightBg and COLORS.Yellow15 or COLORS.Yellow5)
        end
    end
    if self.UselessObj then
        self.UselessObj:SetActive(data.useLess)
    end
    if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if self.NewTipObj then
        self.NewTipObj:SetActive(data.new_flag)
    end
    if data.skill_id ~= 0 then
        data.skill_level = data.skill_level == 0 and 1 or data.skill_level
        if data.is_fu_skill then
            local level = AmuletData.Instance:GetIntensifySkillLevel(data.skill_id)
            if level and level > 0 then
                data.skill_level = level
                data.page = 3
            end
        end
        skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id, data.skill_level)
        -- if skill_cfg and skill_cfg.page ~= nil and skill_cfg.page ~= 3 then
            desc_cfg = SkillData.GetDecConfig(data.skill_id, data.skill_level)
        -- else
        --     desc_cfg = SkillData.GetDecConfig(data.skill_id, 1)
        -- end
        if skill_cfg == nil then
            if IS_EDITOR then
                LogError("pet skill_id:",data.skill_id," 等级",data.skill_level,"没有技能配置")
            end
            return
        end
        if data.main_skill or data.fs_skill then
            if desc_cfg == nil then
                if IS_EDITOR then
                    LogError("pet skill_id:",data.skill_id," 等级",data.skill_level,"没有技能描述")
                end
                return
            end
            skill_cfg = SkillData.GetConfig(data.skill_id, data.skill_level)
            UH.SetText(self.Name, skill_cfg.skill_name or desc_cfg.skill_name)
            new_data.skill_cfg = new_data.skill_cfg or {}
            new_data.skill_cfg.skill_id = skill_cfg.skill_id or 0
            new_data.skill_cfg.skill_level = data.skill_level
            new_data.skill_cfg.description = desc_cfg.description
            new_data.skill_cfg.icon_id = desc_cfg.icon_id
            new_data.skill_cfg.skill_name = skill_cfg.skill_name or desc_cfg.skill_name
            new_data.skill_cfg.corner_icon = desc_cfg.corner_icon
            UH.SetIcon(self.Icon, desc_cfg.icon_id, ICON_TYPE.SKILL)
        else
            -- if skill_cfg.page then
                -- if skill_cfg.page ~= 3 then
                --     new_data.skill_id = skill_cfg.gift_id or 0
                -- else
                    -- new_data.skill_id = skill_cfg.param_1 or 0
                -- end
            -- end
            new_data.desc = skill_cfg.desc_2 or skill_cfg.description_2
            new_data.name = skill_cfg.name or skill_cfg.skill_name
            new_data.corner_icon = desc_cfg and desc_cfg.corner_icon or nil
            UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
            UH.SetText(self.Name, new_data.name)

        end
        self.IconImage.enabled = true
        self.LingwuObj:SetActive(false)
    else
        self:showEmpty(data.show_lingwu, data.lingwu_red)
    end
    if self.Normal then
        self.Normal:SetActive(data.skill_id == 0)
    end
    if self.SkillQuaObj then
        if not data.main_skill then
            --第一页的是初级，第二页的是高级
            local imgName = skill_cfg and skill_cfg.page
            if data.page then
                imgName = data.page
            end
            if imgName ~= nil and tonumber(imgName) then
                UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[imgName])
                self.SkillQuaObj:SetActive(true)
            else
                self.SkillQuaObj:SetActive(false)
            end
        else
            self.SkillQuaObj:SetActive(false)
        end
    end
    if desc_cfg then
        local conner_show = desc_cfg.corner_icon ~= nil and desc_cfg.corner_icon ~= ""
        self.SkillCornerObj:SetObjActive(conner_show)
        self.SkillCornerNormalObj:SetObjActive(conner_show)
        if conner_show then
            local cor_sprite = data.fs_skill and "fs_skill_corner" or "main_skill_corner"
            local cor_txt_color =  data.fs_skill and COLORS.Yellow7 or COLORS.White
            self.SkillCornerObj.SpriteName = cor_sprite
            self.SkillCornerNormalObj.SpriteName =  cor_sprite
            UH.Color(self.SkillCornerText,cor_txt_color)
            UH.Color(self.SkillCornerNormalText,cor_txt_color)
            if data.corner_icon then
                UH.SetText(self.SkillCornerText, data.corner_icon)
                UH.SetText(self.SkillCornerNormalText, data.corner_icon)
            else
                UH.SetText(self.SkillCornerText, desc_cfg.corner_icon)
                UH.SetText(self.SkillCornerNormalText, desc_cfg.corner_icon)
            end

        end
    else
        self.SkillCornerObj:SetObjActive(false)
        self.SkillCornerNormalObj:SetObjActive(false)
    end
    self.SkillLock:SetActive(data.main_skill or data.fs_skill)
    if self.BianYiQua and self.BianYi2Qua then
        if data.main_skill and data.pet_qua ~= nil and data.pet_qua >= PetData.Quality.Shen and 
            data.mutant_lv > PetData.MutantLv.Zero then
            self.BianYiQua:SetActive(data.mutant_lv == PetData.MutantLv.One)
            self.BianYi2Qua:SetActive(data.mutant_lv == PetData.MutantLv.Two)
            self.SkillLock:SetActive(false)
        else
            self.BianYiQua:SetActive(false)
            self.BianYi2Qua:SetActive(false)
        end
    end
    if data.main_skill then
        local effect_id = desc_cfg.asset_id or PetData.Instance:GetSkillEffect(data.skill_id)
        if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(effect_id) then
            -- if data.interactor ~= false then
                self.main_skill_eff_delay = TimeHelper:AddDelayFrameTimer(function ()
                    if self.effect_handle ~= nil then
                        self.EffectTool:Stop(self.effect_handle)
                    end
                    self.effect_handle = UH.PlayEffect(self.EffectTool, tonumber(effect_id), self.EffectRoot)
                end)
            -- end
            if self.CanPar == nil then
                local comp = self.unityMono.gameObject:GetComponentsInParent(typeof(UnityEngine.UI.ScrollRect),true)
                if comp.Length > 0 then
                    self.CanPar = comp[0].gameObject:GetComponent(typeof(UnityEngine.RectTransform))
                end
            end
            if self.CanPar then
                self.CanUp.sizeDelta = self.CanPar.sizeDelta
                if self.setPosRunner == nil then
                    self:SetCanUpPos()
                end
            end
            self.CanUp.gameObject:SetActive(true)
            self.CanNormal:SetActive(false)
        else
            self.CanNormal:SetActive(true)
            self.CanUp.gameObject:SetActive(false)
        end
    else
        self.CanNormal:SetActive(true)
        self.CanUp.gameObject:SetActive(false)
        if self.setPosRunner ~= nil then
            TimeHelper:CancelTimer(self.setPosRunner)
            self.setPosRunner = nil
        end
    end
    if self.MainLevelObj then
        local owner_pet = data.owner_pet
        if owner_pet == nil then
            owner_pet = data.pet_idx ~= nil and PetData.Instance:GetPetInfo(data.pet_idx) or nil
        end
        local show_lv = data.main_skill or data.fs_skill or
            (owner_pet ~= nil and Pet.CanLvupGift(owner_pet:FeiShengTimes()) and  --没达到升级条件不显示等级
                PetData.Instance:HasGiftLvupCfg(data.skill_id))    --超书也显示等级

        if not show_lv and data.is_fu_skill and data.skill_level and data.skill_level > 1 then
            show_lv = true
        end

        self.MainLevelObj:SetActive(show_lv)
        self.MainLevelNormalObj:SetActive(show_lv and not data.useLess)
        if show_lv then
            local show_lv = data.skill_level
            UH.SetText(self.MainLevelText, string.format(Language.Common.LvDot, show_lv))
            UH.SetText(self.MainLevelNormalText, string.format(Language.Common.LvDot, show_lv))
        end
    end
    if self.SkillBgItem then
        local bgName = (data.main_skill or data.fs_skill) and "zhu2" or "bei2"
        bgName = data.is_fu_skill and "fu1" or bgName
        bgName = data.is_yao_skill and "yao" or bgName
        UH.SpriteName(self.XiLianBg, bgName)
    elseif self.XiLianBg ~= nil then
        UH.SpriteName(self.XiLianBg, "")
    end
    if self.FuSkillSign then
        self.FuSkillSign:SetActive(data.is_fu_skill)
    end
    if self.YaoSkillSign then
        self.YaoSkillSign:SetActive(data.is_yao_skill)
    end
    
    -- self:SetCanUpPos()
    UIWidgetBaseItem.SetData(self, new_data)
end

function PetGiftItem:OnDestroy()
    self:SetGray(false)
    if self.setPosRunner ~= nil then
        TimeHelper:CancelTimer(self.setPosRunner)
        self.setPosRunner = nil
    end
    if self.main_skill_eff_delay ~= nil then
        TimeHelper:CancelTimer(self.main_skill_eff_delay)
        self.main_skill_eff_delay = nil
    end
end

function PetGiftItem:SetCanUpPos()
    if not self.awaked then
        self.need_set_up_pos = true
        return
    end
    if self.unityMono ~= nil then
        if self.CanUp and self.CanPar then
            local pos = self.CanPar.position
            self.CanUp.position = pos
            UH.AnchoredPosition(self.CanPos, Vector2.New(-self.CanUp.anchoredPosition.x, -self.CanUp.anchoredPosition.y))
            if self.setPosRunner == nil then
                self.setPosRunner = TimeHelper:AddRunFrameTimer(function ()
                    self.setPosRunner = nil
                    self:SetCanUpPos()
                end, 1,1,false)
            end
        end
    end
end

function PetGiftItem:Click()
    self.IconClick:Trigger()
end

function PetGiftItem:showEmpty(show_lingwu, lingwu_red)
    self.IconImage.enabled = false
    self.LingwuObj:SetActive(show_lingwu)
    if self.lingwu_red_point then
        self.lingwu_red_point:SetNum(lingwu_red and lingwu_red or 0)
    elseif lingwu_red ~= nil and lingwu_red > 0 then
        self.lingwu_red_point = UiRedPoint.New(self.LingwuObj, Vector2.New(31, 33))
        self.lingwu_red_point:SetNum(lingwu_red)
    end
    UH.SetText(self.Name, "")
end

function PetGiftItem:SetGray(value)
    if self.Interactor then
        self.Interactor.Interactable = not value
    end
    if self.EffectRoot then
        self.EffectRoot:SetActive(not value)
    end
end

function PetGiftItem.CommonClickFunc(cell)
    local data = cell:GetData()
    if data == nil then
        return
    end
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        local gift_det = PetData.Instance:GetGiftDetail(data.skill_id, data.skill_level)
        local owner_pet = data.owner_pet
        if owner_pet == nil then
            owner_pet = data.pet_idx ~= nil and PetData.Instance:GetPetInfo(data.pet_idx) or nil
        end
        if owner_pet ~= nil and  Pet.CanLvupGift(owner_pet:FeiShengTimes()) and
            PetData.Instance:HasGiftLvupCfg(data.skill_id) and
            (data.is_fu_skill == nil or data.is_fu_skill == false) then --宠物护符的超级技能显示普通技能的框
            ViewMgr:OpenView(PetSkillLvupTip)
        else
            ViewMgr:OpenView(PetSkillTip)
        end
    elseif data.show_lingwu then
        --这里炼妖换另一个面板
        --假设炼妖数量不够直接跳转元宝商店
        if data and data.is_yao_skill == true then        
            local stuff_cfg = PetRefiningData.Instance.other_info
            local stuff_id = stuff_cfg.addskill_unlock
            local item = Item.Create({item_id = stuff_id})
            if item:Num() <= 0 then
                StoreData.Instance:SetJumpItemId(stuff_id)
                ViewMgr:OpenViewByKey(Mod.Store.Gold)
                return
            end
        end
        PetLingwuSkillTip.SetSlotIndex(data.index)
        ViewMgr:OpenView(PetLingwuSkillTip,{data = data})
    end
end

PetGiftItemSmall = DeclareMono("PetGiftItemSmall", UIWidgetBaseItem)
--BtnOpen
function PetGiftItemSmall:SetData(data)
    local skill_cfg = nil
    local new_data = TableCopy(data)
    local desc_cfg = nil
    if self.LightBg ~= nil then
        UH.Color(self.Name, self.LightBg and COLORS.Yellow15 or COLORS.Yellow5)
    end
    if self.UselessObj then
        self.UselessObj:SetActive(data.useLess)
    end
    if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if data.skill_id ~= 0 then
        data.skill_level = data.skill_level == 0 and 1 or data.skill_level        
        skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id, data.skill_level)
        -- if skill_cfg and skill_cfg.page ~= nil and skill_cfg.page ~= 3 then
            desc_cfg = SkillData.GetDecConfig(data.skill_id, data.skill_level)
        -- else
        --     desc_cfg = SkillData.GetDecConfig(data.skill_id, 1)
        -- end
        if skill_cfg == nil then
            if IS_EDITOR then
                LogError("pet skill_id:",data.skill_id," 等级",data.skill_level,"没有技能配置")
            end
            return
        end
        if skill_cfg.name == nil then
            if desc_cfg == nil then
                if IS_EDITOR then
                    LogError("pet skill_id:",data.skill_id," 等级",data.skill_level,"没有技能描述")
                end
                return
            end
            skill_cfg = SkillData.GetConfig(data.skill_id, data.skill_level)
            UH.SetText(self.Name, skill_cfg.skill_name or desc_cfg.skill_name)
            new_data.skill_cfg = new_data.skill_cfg or {}
            new_data.skill_cfg.skill_id = skill_cfg.skill_id or 0
            new_data.skill_cfg.skill_level = data.skill_level
            new_data.skill_cfg.description = desc_cfg.description
            new_data.skill_cfg.icon_id = desc_cfg.icon_id
            new_data.skill_cfg.skill_name = skill_cfg.skill_name or desc_cfg.skill_name
            new_data.skill_cfg.corner_icon = desc_cfg.corner_icon
            UH.SetIcon(self.Icon, desc_cfg.icon_id, ICON_TYPE.SKILL)
        else
            UH.SetText(self.Name, skill_cfg.name)
            new_data.desc = skill_cfg.desc_2
            new_data.name = skill_cfg.name
            new_data.corner_icon = desc_cfg and desc_cfg.corner_icon or nil
            UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
        end
        self.IconImage.enabled = true
        self.LingwuObj:SetActive(false)
    else
        self:showEmpty(data.show_lingwu)
    end
    if self.Normal then
        self.Normal:SetActive(data.skill_id == 0)
    end
    if self.SkillQuaObj then
        if not data.main_skill then
            --第一页的是初级，第二页的是高级
            local imgName = skill_cfg and tostring(skill_cfg.page)
            if imgName ~= nil and tonumber(imgName) then
                UH.SpriteName(self.SkillQuaImg, "PetSkillQua"..imgName)
                self.SkillQuaObj:SetActive(true)
            else
                self.SkillQuaObj:SetActive(false)
            end
        else
            self.SkillQuaObj:SetActive(false)
        end
    end
    if desc_cfg then
        if desc_cfg.corner_icon ~= nil and desc_cfg.corner_icon ~= "" then
            UH.SetText(self.SkillCornerText, desc_cfg.corner_icon)
        end
        self.SkillCornerObj:SetActive(desc_cfg.corner_icon ~= nil and desc_cfg.corner_icon ~= "")
    else
        self.SkillCornerObj:SetActive(false)
    end
    self.SkillLock:SetActive(data.main_skill)
    if data.main_skill then
        local effect_id = desc_cfg.asset_id or PetData.Instance:GetSkillEffect(data.skill_id)
        if not StringIsEmpty(effect_id) and VersionTag.HasTag(VersionTag.ClipAnima2D) then
            if data.interactor ~= false then
                TimeHelper:AddDelayFrameTimer(function ()
                    self.effect_handle = UH.PlayEffect(self.EffectTool, tonumber(effect_id), self.EffectParent)
                end)
            end
        end
    end
    if self.MainLevelObj then
        self.MainLevelObj:SetActive(data.main_skill)
        if data.main_skill then
            UH.SetText(self.MainLevelText, string.format(Language.Common.LvDot, data.skill_level))
        end
    end
    if self.SkillBgItem then
        local bgName = data.main_skill and "zhu2" or "bei2"
        bgName = data.is_fu_skill and "fu1" or bgName
        UH.SpriteName(self.XiLianBg, bgName)
    elseif self.XiLianBg ~= nil then
        UH.SpriteName(self.XiLianBg, "")
    end
    if self.FuSkillSign then
        self.FuSkillSign:SetActive(data.is_fu_skill)
    end
    UIWidgetBaseItem.SetData(self, new_data)
end
function PetGiftItemSmall:Click()
    self.IconClick:Trigger()
end

function PetGiftItemSmall:showEmpty(show_lingwu)
    self.IconImage.enabled = false
    self.LingwuObj:SetActive(show_lingwu)
    UH.SetText(self.Name, "")
end
---------------------------------------------------------------------
PetGiftTip = PetGiftTip or DeclareView("PetGiftTip", "pet/pet_gift_tip")
function PetGiftTip:PetGiftTip()
    self.lang = Language.Pet
end
function PetGiftTip:LoadCallback()
end
function PetGiftTip:onFlush()
    self.LblTip.text = self.lang.GiftTip
end
function PetGiftTip:OnClickMask()
    ViewMgr:CloseView(PetGiftTip)
end
-------------------------------
-- 宠物天赋开启特效
PetGiftOpenEffect = PetGiftOpenEffect or DeclareMono("PetGiftOpenEffect", UIWFlushPanel)
function PetGiftOpenEffect:PetGiftOpenEffect()
    -- 速度
    -- 圈数
    -- 由慢到快再到慢
    -- init里也要改
    self.height = 86
    self.speed = 35 --起始速度  86/24=每次变化的间距
    self.index = 1
    self.total_index = 5
    self.circle = 1
    self.total_circle = 5 --播放圈数
    self.effect_circle = 3 --第几圈开始播特效
    self.other_config = PetData.Instance:GetPetOtherCfg()
    self.time = 0
    self.play_effect = false
    self.end_func = nil
    self.runner = nil
end
function PetGiftOpenEffect:SetData(index, result, end_func)
    self.sp_config = string.split(self.other_config["talent_" .. index], "|")
    table.insert(self.sp_config, result)
    self.total_index = table.nums(self.sp_config)
    self.end_func = end_func
end
function PetGiftOpenEffect:Init()
    self.Frame:SetActive(true)
    self.speed = 35 --这里重新初始化
    self.index = 1
    self.circle = 1
    self.time = 0
    self.play_effect = false
end
function PetGiftOpenEffect:Reset()
    self.Frame:SetActive(false)
end
--可以在disable关闭特效 但是好像是说要接着播
--[[ function PetGiftOpenEffect:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:Reset()
    if self.runner then
        self.runner:DeleteMe()
    end
    if self.end_func then
        self.end_func()
    end
end ]]
--速度减到1就可以停
function PetGiftOpenEffect:Play(runner)
    self.runner = runner
    if self.Vg.padding.top <= 0 - self.height then
        -- 换成当前图片
        UH.SetIcon(self.sp_top, self.sp_config[self.index], ICON_TYPE.SKILL)
        self.Vg.padding.top = 0
        self.index = self.index + 1
        if self.index > self.total_index then
            self.index = 1
            self.circle = self.circle + 1
            if self.circle == self.total_circle then
                self.Vg.padding.top = 0
                self.Frame:SetActive(false)
                runner:DeleteMe()
                if self.end_func then
                    self.end_func()
                end
                return
            end
        end
    end
    UH.SetIcon(self.sp_down, self.sp_config[self.index], ICON_TYPE.SKILL)
    if self.circle == self.effect_circle then
        self.speed = self.speed - Time.deltaTime
        if self.play_effect == false then
            self.EffectTool:Play("10010223", self.EffectNode)
            self.play_effect = true
        end
    else
        self.speed = self.speed + Time.deltaTime
    end
    self.Vg.padding.top = self.Vg.padding.top - self.speed
end
function PetGiftOpenEffect:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.runner then
        self.runner:DeleteMe()
    end
end

-----------------------------------------------
PetAttrSkin = PetAttrSkin or DeclareMono("PetAttrSkin", UIWFlushPanel)
function PetAttrSkin:PetAttrSkin()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_list, func = self.FlushSkinInfo},
        {data = self.pet_data.pet_list, func = self.FlushSkinDesc, init_call = false},
        {data = self.pet_data.pet_select, func = self.ChangePetFlush, keys = {"index"},init_call = false},
        {data = self.pet_data.cur_select_skin, func = self.FlushSkinDesc, keys = {"id"}},
        {data = self.pet_data.pet_has_skin_list, func = self.FlushSkinDesc, init_call = false},
    }
end

function PetAttrSkin:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.pet_data:SetCurSkinSelect(-1)
    self.pet_data.cur_select_skin.try_skin = false
end

function PetAttrSkin:ChangePetFlush()
    if self.old_index == nil or self.old_index ~= self.pet_data.pet_select.index then
        self.pet_data:SetCurSkinSelect(-1)
        self.pet_data.cur_select_skin.try_skin = false
        self:FlushSkinInfo()
    end
    self.old_index = self.pet_data.pet_select.index
end

function PetAttrSkin:FlushSkinInfo()
    local cur_pet = self.pet_data:GetCurSelect()
    if cur_pet == nil then
        self:showEmpty()
        return
    end
    local skin_list = self.pet_data:GetPetSkinCfgList(cur_pet:ID())
    if skin_list ~= nil and next(skin_list) then
        self.NoSkinObj:SetActive(false)
        self.SkinPanelObj:SetActive(true)
        self.SkinList:SetData(skin_list)
        if self.pet_data.cur_select_skin.id ~= -1 and self.old_index ~= nil and self.old_index == self.pet_data.pet_select.index then
            self:FlushSkinDesc()
        else
            local index = 1
            local skin_data = skin_list[index]
            for i,v in pairs(skin_list) do
                if cur_pet:SkinId() == v.pet_skin_id then
                    index = i
                    skin_data = v
                    break
                end
            end
            if index then
                self.SkinList:SetectItem(index, true)
                self:OnClickSkin(skin_data,true)
            end
        end
    else
        self:showEmpty()
        return
    end
end

function PetAttrSkin:FlushSkinDesc()
    local cur_data = self.pet_data.cur_select_skin.skin_data
    if cur_data == nil then
        return
    end
    local has_active = self.pet_data:GetSkinHasIsActive(self.pet_data.cur_select_skin.id)
    UH.SetText(self.SkinNameText, cur_data.name)
    
    local btn_str = self.pet_data.cur_select_skin.try_skin and Language.Pet.PetSkinTry[2] or Language.Pet.PetSkinTry[1]
    -- self.BtnTryObj:SetActive(not has_active)
    -- self.BtnUseObj:SetActive(has_active)
    if has_active then
        local cur_pet = self.pet_data:GetCurSelect()
        if cur_pet ~= nil then
            btn_str = cur_pet.info.skin_id == self.pet_data.cur_select_skin.id and Language.Pet.PetSkinPutOff or Language.Pet.PetSkinUse
        end
        UH.SetText(self.SkinDescText, cur_data.describe)
        UH.SetText(self.BtnText, btn_str)
    else
        UH.SetText(self.SkinDescText, Language.Pet.PetGetway..":<color=#"..COLORSTR.Red5..">"..Language.Pet.PetSkinGet.."</color>\n"..cur_data.describe)
        UH.SetText(self.BtnTryText, btn_str)
    end
end

function PetAttrSkin:showEmpty()
    self.NoSkinObj:SetActive(true)
    self.SkinPanelObj:SetActive(false)
end

function PetAttrSkin:OnClickSkin(data,value)
    self.pet_data:SetCurSkinSelect(data.pet_skin_id, data)
    local has_active = self.pet_data:GetSkinHasIsActive(self.pet_data.cur_select_skin.id)
    if has_active then
        if value then
            PetCtrl.SendPetOp(PET_OP_TYPE.UsePetSkin, self.pet_data:GetSelectPetIndex(), self.pet_data.cur_select_skin.id) --穿戴皮肤
        else
            PetCtrl.SendPetOp(PET_OP_TYPE.UsePetSkin, self.pet_data:GetSelectPetIndex(), 0) --卸下皮肤
        end
    else
        self.pet_data.cur_select_skin.try_skin = value
    end
end

function PetAttrSkin:OnClickUse()
    if self.pet_data:GetSkinHasIsActive(self.pet_data.cur_select_skin.id) then
        if self.pet_data.cur_select_skin.id == self.pet_data:GetCurSelect().info.skin_id then
            PetCtrl.SendPetOp(PET_OP_TYPE.UsePetSkin, self.pet_data:GetSelectPetIndex(), 0) --卸下皮肤
        else
            PetCtrl.SendPetOp(PET_OP_TYPE.UsePetSkin, self.pet_data:GetSelectPetIndex(), self.pet_data.cur_select_skin.id) --穿戴皮肤
        end
        self.pet_data.cur_select_skin.try_skin = false
    end
end

function PetAttrSkin:OnClickTry()
    self.pet_data.cur_select_skin.try_skin = not self.pet_data.cur_select_skin.try_skin
    UH.SetText(self.BtnTryText, self.pet_data.cur_select_skin.try_skin and Language.Pet.PetSkinTry[2] or Language.Pet.PetSkinTry[1])
end

function PetAttrSkin:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[68].desc})
end

----------------------------------------
PetSkinItem = PetSkinItem or DeclareMono("PetSkinItem", UIWidgetBaseItem)
function PetSkinItem:PetSkinItem()
    self.pet_data = PetData.Instance
end

function PetSkinItem:SetData(data)
    self.data = data
    local cur_pet = self.pet_data:GetCurSelect()
    if cur_pet ~= nil then
        self.OnUseObj:SetActive(cur_pet:SkinId() == data.pet_skin_id)
        self.SkinImg:SetByPath(string.format("pet_skin/%s.png",self.data.res_id))
    end
    self.SkinSign:SetObjActive(not StringIsEmpty(self.data.limit_sign))
    if not StringIsEmpty(self.data.limit_sign) then
        self.SkinSign:SetData(self.data)
    end
end

function PetSkinItem:Click()
    -- if self.Toggle.isOn then
    --     self.pet_data:SetCurSkinSelect(self.data.pet_skin_id, self.data)
    -- else
    --     self.Toggle.isOn = true
    -- end
end

PetAttrItem = PetAttrItem or DeclareMono("PetAttrItem", UIWidgetBaseItem)

function PetAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = Language.PetOtherAttriNames[data.attr_type]
    local value = PetData.Instance:GetPetAttriValue(data.attr_type)
    if data.is_percent then  -- 是千分比
        value = value / 10
        value = string.format("%s%%",value)
    end
    UH.SetText(self.Name, name)
    UH.SetText(self.Value, value)
end

function PetAttrItem:OnClickBaseAttr()
    local pet = PetData.Instance:GetCurSelect()
    if pet == nil then
        return
    end
    local obj = self.gameObject
    --local pos_y = obj.transform.localPosition.y
    local pos_y = UH.GetRootPosition(self.gameObject).y
    local pos_t = {x = 195, y = pos_y}
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_PET,
        param1 = pet.index,
        attr_type = self.data.attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end


PetAttrWeaponItem = PetAttrWeaponItem or DeclareMono("PetAttrWeaponItem", UIWidgetBaseItem)
-- data:{is_show:bool, pet_weapon:PetWeapon}
function PetAttrWeaponItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Obj:SetActive(data.is_show == true)
    if not data.is_show then
        return
    end
    local pet_weapon = data.pet_weapon
    self.pet_weapon = pet_weapon
    if pet_weapon then
        UH.SetText(self.Name,pet_weapon:QuaName())
        UH.SetIcon(self.Icon,pet_weapon:IconId())
        self.TipObj:SetActive(false)
        self.Qua:SetObjActive(true)
        local qua = pet_weapon:QuaType()
        UH.SpriteName(self.Qua, string.format("Qua%s",qua))
    else
        UH.SetText(self.Name,"")
        UH.SetIcon(self.Icon,nil)
        self.TipObj:SetActive(true)
        self.Qua:SetObjActive(false)
    end
    self:FlushRemind()
end

function PetAttrWeaponItem:FlushRemind()
    if not self.IsSelf then
        self.RedPoint:SetNum(0)
        return
    end
    local pet_weapon = self.data.pet_weapon
    if not pet_weapon then
        if not TableIsEmpty(BagData.Instance.item_list[ItemColumnType.PetWeapon]) then
            self.RedPoint:SetNum(1)
        else
            self.RedPoint:SetNum(0)
		end
        return
    end
    local pet_index = pet_weapon:PetIndex()
    if pet_index and pet_index >= 0 then
        local pet = PetData.Instance:GetPetInfo(pet_index)
        local num = PetWeaponData.Instance:GetPetRemind(pet, pet_weapon) or 0
        self.RedPoint:SetNum(num)
    else
        self.RedPoint:SetNum(0)
    end
end

function PetAttrWeaponItem:OnOpenPetWeaponClick()
    if self.IsSelf then
        self:HandleSelfWeapon()
    else
        self:HandleOtherWeapon()
    end
end

function PetAttrWeaponItem:HandleSelfWeapon()
    local weapon = self.pet_weapon
    if weapon then
        local info = {
            item_list = {weapon},func = function ()
                local func = {ItemInfoFunc.PetWeaponReplace, ItemInfoFunc.PetWeaponIntensify, ItemInfoFunc.PetWeaponRefine,}
                if weapon:QuaType() > 4 then
                    table.insert(func, ItemInfoFunc.PetWeaponChange)
                end
                table.insert(func, ItemInfoFunc.PetWeaponUnfix)
                return func
            end
        }
        PetWeaponData.Instance:SetWeaponIdx(self.WeaponIdx)
        ItemInfoCtrl.Instance:ItemInfoView(info)
    else
        local list = PetWeaponData.Instance:GetAllWeaponListData(true)
        if TableIsEmpty(list) then
            PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip1)
        else
            PetWeaponData.Instance:SetWeaponIdx(self.WeaponIdx)
            ViewMgr:OpenViewByKey(Mod.PetWeapon.Main)
        end
    end
end

function PetAttrWeaponItem:HandleOtherWeapon()
    local weapon = self.pet_weapon
    if weapon then
        local info = {
            item_list = {weapon}
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    end
end

PetVariationDialog = PetVariationDialog or DeclareView("PetVariationDialog", "pet/pet_variation_dialog")
VIEW_DECLARE_MASK(PetVariationDialog,ViewMask.BgBlockClose)
-- param_t:{pet}
function PetVariationDialog:LoadCallback(param_t)
    param_t = param_t or {}
    local pet = param_t.pet
    if not pet then
        return
    end
    self.pet = pet
    local cfg = pet:PetCfg()
    self.need_num = cfg.change_consume_num
    self.has_num = Item.GetNum(cfg.change_consume_item)
    self.item = Item.Create({item_id = cfg.change_consume_item, num = DataHelper.ConsumeNum(self.has_num, self.need_num, true)})
    local item_name = self.item:QuaName()
    local pet_name = pet:QuaName()
    local pet_type = pet:Quality() >= 5 and Language.Pet.PetType1 or Language.Pet.PetType2
    UH.SetText(self.Desc, string.format(Language.Pet.VariationDesc,item_name,pet_type,pet_name,pet_type))
    self.ItemCell:SetData(self.item)
end

function PetVariationDialog:OnVariationClick()
    if self.has_num < self.need_num then
        PublicPopupCtrl.Instance:Error(Language.Pet.NotItem)
        MainOtherCtrl.Instance:GetWayView({item = self.item})
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetMutant, self.pet.index)
    ViewMgr:CloseView(PetVariationDialog)
end