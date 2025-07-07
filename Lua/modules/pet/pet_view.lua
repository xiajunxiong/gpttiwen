FILE_REQ("modules/pet/pet_attr")
FILE_REQ("modules/pet/pet_book")
FILE_REQ("modules/pet/pet_hitbook")
FILE_REQ("modules/pet/pet_xilian")
FILE_REQ("modules/pet/pet_exp")
FILE_REQ("modules/pet/pet_strengthen")
FILE_REQ("modules/pet/pet_strengthen_buff")
FILE_REQ("modules/pet/pet_gift_replace")
FILE_REQ("modules/pet/pet_relife")
FILE_REQ("modules/pet/pet_skill_choose")
FILE_REQ("modules/pet/pet_skill_tip")
FILE_REQ("modules/pet/pet_add_point")
FILE_REQ("modules/pet/pet_pieces")
FILE_REQ("modules/pet/pet_pieces_shenshou")
FILE_REQ("modules/pet/pet_gift_suggest")

PetView = PetView or DeclareView("PetView", "pet/pet", Mod.Pet.PetAttr)
VIEW_DECLARE_LEVEL(PetView,ViewLevel.L1)

----Board=======UIWBoard
----Tabbar======UIWidgetList

function PetView:PetView()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance

    GuideManager.Instance:RegisterGetGuideUi("PetTabXilian", function ()
        if self.Board then
    		return self.Board.Tabbar.item_list[2], function ()
                self.Board.Tabbar:ClickItem(2)
            end
        else
            GuideManager.Instance:UnRegisterGetGuideUi("PetTabXilian")
        end
	end)
    GuideManager.Instance:RegisterGetGuideUi("PetTabGetBook", function ()
        if self.Board then
            return self.Board.Tabbar.item_list[4], function ()
                self.Board.Tabbar:ClickItem(4)
            end
        else
            GuideManager.Instance:UnRegisterGetGuideUi("PetTabGetBook")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetTabStrengthen", function ()
        if self.Board then
            return self.Board.Tabbar.item_list[3], function ()
                self.Board.Tabbar:ClickItem(3)
            end
        else
            GuideManager.Instance:UnRegisterGetGuideUi("PetTabStrengthen")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetTabStamp", function ()
        if self.Board then
            return self.Board.Tabbar.item_list[5], function ()
                self.Board.Tabbar:ClickItem(5)
            end
        else
            GuideManager.Instance:UnRegisterGetGuideUi("PetTabStamp")
        end
    end)
end
function PetView:LoadCallback(param_t)
    PetCtrl.Instance:SendPatnerPetRankReqPet()
    self.Board:SetData(self:GetType(), self.lang.Pet)
    self.Board:SetTabbarVisible(true)
    self.Board:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.Pet)
    if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.Board:SetAuditBgShow()
	end
    self.Board.Tabbar:SetData(
        {
            {name = self.lang.Tab1, tab_objs = {self.CommonLeft, self.TabAttr}, mod = TabMod(Mod.Pet.PetAttr), open = Mod.Pet.PetAttr, open_tip = ErrorText[40]},
            --{name = self.lang.Tab2, tab_objs = {self.CommonLeft, self.TabSkill}, open = Mod.Pet.PetSkill, open_tip = ErrorText[40]},
            {name = self.lang.Tab5, tab_objs = {self.CommonLeft, self.TabXilian},open = Mod.Pet.PetXilian, open_tip = self.lang.PetTabbarNotOpen},
            {name = self.lang.Tab6, tab_objs = {self.CommonLeft, self.TabStrengthen}, mod = TabMod(Mod.Pet.PetStrChange),open = Mod.Pet.PetStrChange,open_tip = BindTool.Bind1(self.pet_data.GetPetStrTabbarTip, self.pet_data)},
            {name = self.lang.Tab4, tab_objs = {self.CommonLeft, self.TabGetBook}, mod = TabMod(Mod.Pet.PetGetBook),open = Mod.Pet.PetGetBook, open_tip = self.lang.PetTabbarNotOpen},
            {name = self.lang.Tab7, tab_objs = {self.CommonLeft,self.PetStamp}, open = Mod.Pet.PetStamp},
            {name = self.lang.Tab3, tab_objs = {self.TabBook},open = Mod.Pet.PetBook, mod = TabMod(Mod.Pet.PetBook)},
        }
    )
    if param_t then
        if param_t.open_param == 6 then
            self.is_open_exp = true
            param_t.open_param = 1
        elseif param_t.open_param == (Mod.Pet.PetGetBook % 1000) then
            self.pet_data:SetAutoClickGiftBook(param_t.item_cfg)
        end
        if next(PetData.Instance:GetPetList()) == nil then
            param_t.open_param = 3
            self.is_open_exp = false
        end
        self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Pet, self.Board.Tabbar, param_t.open_param))
    else
        self.Board.Tabbar:ClickItem(1)
    end
    self.Currency:DefaultSet()
    self.Board:SetCloseFunc(function()
        if ViewMgr:IsOpen(PetExp) then
            ViewMgr:CloseView(PetExp)
        end
    end)
end

function PetView:OnFlush()
    if self.is_open_exp then
        ViewMgr:OpenView(PetExp)
    end
    --检查并锁住
    local tabbar = self.Board.Tabbar
    if next(PetData.Instance:GetPetList()) == nil then
        tabbar.item_list[1]:TabbarOpenEvent(false)
        tabbar.item_list[2]:TabbarOpenEvent(false)
        tabbar.item_list[3]:TabbarOpenEvent(false)
        tabbar.item_list[4]:TabbarOpenEvent(false)
        tabbar.item_list[5]:TabbarOpenEvent(false)
    else
        tabbar.item_list[1]:TabbarOpenEvent(true)
        tabbar.item_list[2]:TabbarOpenEvent(true)
        tabbar.item_list[3]:TabbarOpenEvent(FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetStrChange))
        tabbar.item_list[4]:TabbarOpenEvent(true)
        tabbar.item_list[5]:TabbarOpenEvent(FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetStamp))

        -- self.Board.Tabbar:ClickItem(1)
    end
end
function PetView:CloseCallback()
    self.pet_data:SetSelect(-1)
    GuideManager.Instance:UnRegisterGetGuideUi("PetTabXilian")
    GuideManager.Instance:UnRegisterGetGuideUi("PetTabGetBook")
    GuideManager.Instance:UnRegisterGetGuideUi("PetTabStrengthen")
    GuideManager.Instance:UnRegisterGetGuideUi("PetTabStamp")
    local guide = GuideManager.Instance:Guide()
	local step = GuideManager.Instance:Step()
	if guide and GuideData.GuideList[guide.id] and step == 2 then
		GuideManager.Instance:ForceStop()
	end
    self.pet_data.pet_gift_replace_s.jump_anim = false
end

--宠物展示背景板设置工具函数
--uiw_setter:UIWTextureSetter
--quality:宠物品质
--mut_lv:变异等级
--is_dark:是否是暗面板
function PetView.SetTypeBgImage(uiw_setter,quality,mut_lv,is_dark, lian_yao)
    local tex_path = nil
    lian_yao = lian_yao or false
    if mut_lv > 0 then
        tex_path = string.format("pet/qb_mutant_%d_%s.png",mut_lv,is_dark and "d" or "l")
    elseif quality == PetData.Quality.Shen then
        tex_path = string.format("pet/qb_shenshou_%s.png",is_dark and "d" or "l")
    elseif is_dark ~= true then
        tex_path = "pet/qb_normal_l.png"
    end
    if lian_yao == true then
        tex_path = "pet/qb_lianyao_d.png"
    end
    -- LogError("SetTypeBgImage",quality,mut_lv,is_dark,tex_path)
    if StringIsEmpty(tex_path) then
        uiw_setter:SetTextureNil()
    else
        uiw_setter:SetByPath(tex_path)
    end
end
-----------------------------------------
PetItem = PetItem or DeclareMono("PetItem", UIWidgetBaseItem)
--HeadCell===========UIWSubstitute[UIWPetHeadCell]
function PetItem:SetData(data)
    if self.data and self.data.bag_index == data.bag_index then
        if self.data.unlock_cell and data.emptyPet then
            self.EffectTool:Play("7165077")
            TimeHelper:CancelTimer(self.time_ht)
            self.time_ht = Invoke(function ()
                self:SetData(self.data)
            end)
            UIWidgetBaseItem.SetData(self, data)
            return
        end
    end

    UIWidgetBaseItem.SetData(self, data)
    if not self.IsAnother then
        self:SetSelected(data.index ~= nil and data.index == PetData.Instance:GetSelectPetIndex())
    end

    if self.red_point then
        self.red_point:SetNum(0)
    end
    if self.FlyBgSub then
        self.FlyBgSub:SetData({})
    end
    if data.emptyPet or data.unlock_cell then
        self:ShowEmptyPet(false)
        self.data = data
        return
    else
        self:ShowEmptyPet(true)
    end
    self.pet_data = self.IsAnother and AnotherData.Instance or PetData.Instance
    local state, is_follow = self.pet_data:GetPetState(data.index)
    UH.SpriteName(self.PetState, PetData.State[state])
    self.PetFollow:SetActive(is_follow)
    self.OnDefence:SetActive(self.pet_data:GetIsArena(data.index))
    -- UH.SetIcon(self.Icon, data:IconId())
    -- UH.SpriteName(self.QuaIcon, data:QuaIcon())
    if self.LockObj then
        self.LockObj:SetActive(data.info.lock == 1)
    end
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(122, 38))
        self.red_point:SetNum(0)
    end
    if not self.IsAnother then
        local breach_num = PetData.Instance:IsCanBreach(data)
        local point_num = PetData.Instance:CanAddPoint(data) + PetData.Instance:CanUpgradePoint(data) + (breach_num > 0 and 1 or 0)
		if point_num == 0 then
            point_num = PetData.Instance:CanAttributePoint(data) 
        end
       
        if point_num == 0 then
            if data.index == PetData.Instance:GetFightIndex() or PetData.Instance:GetIsStandBy(data.index) then
                if PetData.Instance:GetSelTab() == Mod.Pet.PetGetBook and next(BagData.Instance:GetPetEquipmentList()) and data.info.pet_equipment_id == 0 then
                    point_num = 1
                end
                -- 检查天赋的
                if point_num == 0 then
                    point_num = PetInbornData.Instance:GetPetInbornFlag(data)
                end
                
            end
        end
    
        -- 徽章的
        if point_num == 0 then
            if PetData.Instance:GetSelTab() == Mod.Pet.PetGetBook then
                point_num = PetData.Instance:PetBagdeRemind(data) or 0
            end
        end
       
        -- 在检查下魂器的
        if point_num == 0 then
            point_num = PetWeaponData.Instance:GetPetRemind(data,data:Weapon()) or 0
        end
     
        if point_num == 0 and data:IsLianYao() then
            point_num = PetWeaponData.Instance:GetPetRemind(data,data:Weapon2()) or 0
        end

        -- 检查下强化的
        if point_num == 0 and PetData.Instance:GetSelTab() == Mod.Pet.PetStrChange then
            point_num = PetData.Instance:GetStrengthenRemind(data)
        end
        if point_num == 0 and PetData.Instance:GaiMingRed(data) == 1 then
            point_num = PetData.Instance:GaiMingRed(data)
        end
        self.red_point:SetNum(point_num)
    end

    -- self.Mutant:SetActive(data.config.mutant_times >= 1)
    UH.SetText(self.PetName, data:Name())
    UH.Color(self.PetName, data:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)

    local score_num, score_level = data:GetScore()
    UH.SetText(self.PetScore, score_num)
    UH.SpriteName(self.PetScoreLevel, "scoreLevel"..tostring(score_level))
    -- PetData.SetStrLevel(nil, self.LevelNum, data.str_level)
    if self.PrayFlag then
        self.PrayFlag:SetActive(data:PrayMainIndex() ~= nil)
    end

    -- PetCtrl.FlushHeadByPet({
    --     icon = self.Icon,
    --     str_img = self.LevelNum,
    --     quality_img = self.QuaIcon,
    --     mutant_obj = self.Mutant,
    -- },data)
    self.HeadCell:SetDataByPet(data)
    local fly_flag = self.data:FeiShengTimes()
    if self.FlyBgSub then
        self.FlyBgSub:SetData({fly_flag = fly_flag})
    end
    local is_lianyao = self.data:IsLianYao()
    if self.YaoObj then
        self.YaoObj:SetActive(is_lianyao)
    end
    self.PetName.alignment = is_lianyao and 3 or 4
end

function PetItem:ShowEmptyPet(show)
    if self.HideObjArray then
        for i=1, self.HideObjArray:Length() do
            self.HideObjArray[i]:SetActive(show)
        end
        self.PlusObj:SetActive(self.data.emptyPet == true)
        self.UnLockObj:SetActive(self.data.unlock_cell == true)
        if not show then
            -- UH.SetIcon(self.Icon, nil)
            UH.SetText(self.PetName, "")
            UH.SetText(self.PetScore, "")
            UH.SpriteName(self.PetState, "")
            if self.YaoObj then
                self.YaoObj:SetActive(false)
            end
            self.PetFollow:SetActive(false)
            self.LockObj:SetActive(false)
            self.OnDefence:SetActive(false)
            -- self.Mutant:SetActive(false)
            self.PrayFlag:SetActive(false)
            self.HeadCell:Clean()
        end
    end
end

function PetItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function PetItem:OnUnlockClick()
    -- if self.playing then
    --     return
    -- end
    -- PetCtrl.Instance:UnlockCell(function ()
    --     self.EffectTool:Play("7165077")
    --     self.playing = true
    --     TimeHelper:CancelTimer(self.time_ht)
    --     ViewMgr:CloseView(DialogTipsView)
    --     self.time_ht = Invoke(function ()
    --         PetCtrl.SendPetOp(PET_OP_TYPE.ExpandBag)
    --         self.playing = false
    --     end)
    -- end)
    PetCtrl.Instance:UnlockCell(function ()
        ViewMgr:CloseView(DialogTipsView)
        PetCtrl.SendPetOp(PET_OP_TYPE.ExpandBag)
    end)
end

function PetItem:PlayUnlockEffect()

end

function PetItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
end

function PetItem:OnPlusFrameClick()
    ViewMgr:OpenView(PetGuideTipView)
end

----------------------------------------------------------------------
PetListPanel = PetListPanel or DeclareMono("PetListPanel", UIWFlushPanel)
function PetListPanel:PetListPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    self.pet_index = -1
    self.pet_info_care = nil
    
    self.data_cares = {
        -- {data = self.pet_data.pet_select, func = self.FlushPetInfo, keys = {"index"}},
        {data = self.pet_data.pet_list, func = self.FlushPetList, init_call = false},
        {data = self.pet_data.pet_other_info, func = self.FlushPetList, init_call = false},
        {data = self.pet_data.pet_other_info, func = self.PlayExtendGridEffect, keys = {"extend_grid_num"}, init_call = false},
		{data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.FlushPetList},
        {data = AttributeDanData.Instance.attribute_red_piont_pet,func = self.FlushPetList},
        {data = self.pet_data.TargetPet, func = self.FlushSel, init_call = false},
        {data = self.pet_data.select_tab_data, func = self.FlushPetList, init_call = false},
    }
    local guide_func = function (target_pet_id)
        if target_pet_id == nil then
            return self.PetList:GetItemMonoByIndex(1)
        end
        local pet_list = self:GetSortedList()
        for k,v in ipairs(pet_list) do
            if target_pet_id and v.pet_id == target_pet_id then
                self.PetList:JumpVerticalIndex(k, 106)
                return self.PetList:GetItemMonoByIndex(k)
            end
        end
        -- 没有强行停止
        GuideManager.Instance:ForceStop()
        --return self.PetList:GetItem(1)
    end
    GuideManager.Instance:RegisterGetGuideUi("PetFreeLifeChoose",function ()
        if Config.function_guide_auto.pet_skill and Config.function_guide_auto.pet_skill[1] then
            return guide_func(Config.function_guide_auto.pet_skill[1].pet_id)
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetReinforceItem",function ()
        if Config.function_guide_auto.pet_skill and Config.function_guide_auto.pet_skill[1] then
            return guide_func(Config.function_guide_auto.pet_skill[1].pet_id_qianghua)
        end
    end)
end
function PetListPanel:onFlush()
    self:FlushPetList()
    -- self.PetList:ClickItem(1)
end

function PetListPanel:FlushSel()
    local pet_index = self.pet_data.TargetPet.pet_index
    if pet_index == nil or pet_index < 0 then
        return
    end
    local pet_list = self:GetSortedList()
    for k,v in ipairs(pet_list) do
        if pet_index == v.index then
            self.PetList:ClickItem(k)
            self.PetList:JumpVerticalIndex(k,106)
            self.pet_data.TargetPet.pet_index = -1
        end
    end
end

function PetListPanel:GetSortedList()
    local list = self.pet_data:GetPetList()
    list = table.rearrange(list)
    return self.pet_data:PetSort(list)
end

function PetListPanel:FlushPetList()
    -- 刷新宠物列表 宠物列表变化要刷新 宠物状态变化要刷新
    local list = self:GetSortedList()
    local nums = table.nums(list)
    self.pet_list = list

    --修改为初始+开启
    local other_info = self.pet_data:GetOtherInfo()
    local total = self.pet_data:GetPetStartBag() + (other_info.extend_grid_num or 0)
    UH.SetText(self.PetNum, string.format(Language.Pet.HavePet, nums, total))
    self.AddImg:SetActive(total < self.pet_data:GetPetMaxBag())

    if #list > 0 and #list < total then
        local end_index = total - #list
        for i = 1, end_index do
            local bag_index = #list
            table.insert(list, {bag_index = bag_index, emptyPet = true})
        end
    end

    local max_bag = self.pet_data:GetPetMaxBag()
    if total < max_bag then
        local cfg = self.pet_data:GetPetOtherCfg()
        local lock_count = max_bag - total >= cfg.add_num and cfg.add_num or max_bag - total
        for i = 1, lock_count do
            local bag_index = #list
            table.insert(list, {bag_index = bag_index, unlock_cell = true})
        end
    end

    self.PetList:SetData(list)
    if GuideManager.Instance:IsGuide() and GuideManager.Instance:Guide().id == 25 then
        local _,index = GuideData.Instance:CheckXiLianGuide()
        self.pet_data:SetSelect(index)
        self.PetList:ClickItemData(self.pet_data:GetCurSelect())
        --local jump_index = self.PetList.data_t[self.pet_data:GetCurSelect()] or 1
        --self.PetList:JumpVerticalIndex(jump_index, 106)
        self.PetList:SetData(list)
    else
        if self.pet_data:GetCurSelect() then
            --self.PetList:ClickItemData(self.pet_data:GetCurSelect())
        else
            self.PetList:ClickItem(1)
        end
    end
    if self.pet_data.jump_top then
        self.pet_data.jump_top = false
        self:JumpToTop()
    end
end

function PetListPanel:JumpToTop()
    if self.pet_data:GetSelectPetIndex() ~= -1 and (self.pet_data:GetCurSelect():State() == PetState.Fight or self.pet_data:GetCurSelect():State() == PetState.StandBy) then
        self.PetList:JumpVerticalIndex(1, 106)
    end
end

function PetListPanel:OnClickPet(mono)
    local data = mono:GetData()
    if data.emptyPet then
        ViewMgr:OpenView(PetGuideTipView)
    else
        self.pet_data:SetSelect(data.index)
        mono:SetSelected(true)
    end
    -- 请求属性
    -- PetCtrl.SendPetOp(PET_OP_TYPE.PetAttrReq, data.index)
end

function PetListPanel:PlayExtendGridEffect()
    self.EffectTool:Play(7165078)
end

function PetListPanel:OnClickAdd()--拓展背包
    local mono_index = #self.pet_list
    local mono = self.PetList:GetItemMonoByIndex(mono_index)
    if mono then
        mono:OnUnlockClick()
    else
        PetCtrl.Instance:UnlockCell()
    end
end

function PetListPanel:OnUnlockClick()
    PetCtrl.Instance:UnlockCell(function ()
        ViewMgr:CloseView(DialogTipsView)
        PetCtrl.SendPetOp(PET_OP_TYPE.ExpandBag)
    end)
end
-----------------------------------------
PetGuideTipView = PetGuideTipView or DeclareView("PetGuideTipView", "pet/pet_guide_tip")
VIEW_DECLARE_MASK(PetGuideTipView, ViewMask.BlockClose)
function PetGuideTipView:PetGuideTipView()
end

function PetGuideTipView:LoadCallback(param_t)
    local list = param_t.list or PetData.Instance:PetGetWayList()
    self.GetWayList:SetData(list)
    local tip = param_t.tip or Language.Pet.GetWayTip
    UH.SetText(self.Tip, tip)
end

function PetGuideTipView:OnClickClose()
    ViewMgr:CloseView(PetGuideTipView)
end
----------------------------
PetGetWayItem = PetGetWayItem or DeclareMono("PetGetWayItem", UIWidgetBaseItem)
function PetGetWayItem:PetGetWayItem()
end

function PetGetWayItem:SetData(data)
    UH.SetText(self.WayText, data.desc)
    self.data = data
end

function PetGetWayItem:OnClickGetWay()
    ViewMgr:CloseView(PetGuideTipView)
    if self.data.id == 211 then
        TradeData.Instance:SetSelectBigType(3)
        ViewMgr:OpenViewByKey(Mod.Mall.Trade)
    else
        if self.data.id == 213 then
            ViewMgr:CloseView(PetView)
        end
        GetWayData.OpenPanel(self.data)
    end
end

-- 宠物限定标识
PetLimitSignItem = PetLimitSignItem or DeclareMono("PetLimitSignItem",UIWidgetBaseItem)

function PetLimitSignItem:Awake()
    self.awake = true
    self:ShowEffect()
end

function PetLimitSignItem:SetType(_type)
    self.limit_type = _type
    self:Show()
end

function PetLimitSignItem:Show()
    local _type = self.limit_type
    if _type == nil or _type == PetData.LimitType.Null then
        self.gameObject:SetActive(false)
        return
    end
    self.gameObject:SetActive(true)
    local data = PetData.LimitRes[_type]
    UH.SpriteName(self.SignImg,data.img or "")

    self:ShowEffect()
end

function PetLimitSignItem:ShowEffect()
    if not self.awake or not self.limit_type or self.limit_type == PetData.LimitType.Null then
        return
    end
    local data = PetData.LimitRes[self.limit_type]
    if self.effect_ht and data.effect_id ~= self.last_effect_id then
        self.EffectTool:Stop(self.effect_ht)
        self.effect_ht = nil
    end
    if not self.effect_ht then
        self.effect_ht = self.EffectTool:Play(data.effect_id)
        self.last_effect_id = data.effect_id
    end
end

function PetLimitSignItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_handle)
end

function PetLimitSignItem:SetSignName(sign_name)
    if sign_name == nil or sign_name == "" then
        self.gameObject:SetActive(false)
        return
    else
        self.gameObject:SetActive(true)
    end
    UH.SpriteName(self.SignImg,sign_name or "")
end


PetMutantTag = PetMutantTag or DeclareMono("PetMutantTag")
--RootObj=====GameObject
--TxtSetter====UIImageSpriteSet
--BgSetter====UIImageSpriteSet
--TopSetter===UIImageSpriteSet


function PetMutantTag:SetMutant(mutant)
    if mutant == PetData.MutantLv.Zero then
        self.RootObj:SetActive(false)
    else
        self.RootObj:SetActive(true)
        local is_two = mutant == PetData.MutantLv.Two
        self.TxtSetter.SpriteName = is_two and "_LocBianyi2" or "_LocBianyi"
        self.BgSetter.SpriteName = is_two and "mutantBg2" or "mutantBg"
        self.TopSetter.SpriteName = is_two and "mutantSmallBg2" or "mutantSmallBg"
    end
end