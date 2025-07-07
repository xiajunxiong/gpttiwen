PetStrengthenPanel = PetStrengthenPanel or DeclareMono("PetStrengthenPanel", UIWFlushPanel)
--NeedFSMaxTip=====GameObject
--TypeBgSetter=====UIWTextureSetter
function PetStrengthenPanel:PetStrengthenPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
    end
    GuideManager.Instance:RegisterGetGuideUi("PetStrengthenBtn", function ()
        return self.StrengthenButtonObj, function ()
            self:OnClickStrengthen()
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("PetStrengthenSelectBtn", function ()
        local obj
        for i,mono in pairs(self.SelPetList.item_list or {}) do
            if mono:GetData().pet == nil then
                obj = mono
                break
            end
        end
        if obj == nil then
            return
        end
        return obj, function ()
            obj:OnAddClick()
        end
    end)

    self.data_cares = {
        {data = self.pet_data.pet_select, func = self.FlushShowInfo, keys = {"index"}},
        {data = self.pet_data.pet_list, func = self.FlushShowInfo, init_call = false},
        {data = self.pet_data.sel_pet_str_data, func = self.FlushStrSelInfo, init_call = false},
        {data = PetResonanceData.Instance.effect_play, func = self.PlayResonEffect, init_call = false},
        {data = self.pet_data.pet_select, func = self.FlushIndex, keys = {"index"}},
    }
    if self.TestBtn then
        self.TestBtn:SetActive(IS_EDITOR)
    end

    self.is_lian_yao = false
end

function PetStrengthenPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.pet_data:SetSelTab(Mod.Pet.PetStrChange)
end

function PetStrengthenPanel:FlushIndex()
    if self.pet_data.pet_select.index ~= self.pet_select_index then
        self.PetTypeSelBtn.isOn = false
        self.pet_select_index = self.pet_data.pet_select.index
    end
end

function PetStrengthenPanel:PlayResonEffect()
    self.EffectTool:Play("3165204")
end

function PetStrengthenPanel:FlushShowInfo()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local cur_pet = pet
    self.PetTypeSelBtn:SetObjActive(pet:IsLianYao())
    local is_show_yao = false
    if self.is_lian_yao and pet:IsLianYao() then
        pet = pet:YaoPet()
        is_show_yao = true
    else
        self.is_lian_yao = false
    end
    
    self.pet_data.strength_use_protect = 0
    local level_str = string.format(Language.Common.LvDot, pet:Level())
    level_str = ColorStr(level_str, COLORSTR.Yellow4)
    UH.SetText(self.PetLevelText, "")
    -- UH.SetText(self.LblName, pet.name)
    -- UH.SetText(self.LblLevel, string.format(self.lang.LevelStr, pet.level))
    local apti_list = cur_pet:ConfigGrowthList()
    local growth_list = cur_pet:TotalGrowthList()
    local v1 = cur_pet:TotalGrowth()
    local v2 = cur_pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr, COLORSTR.Yellow5, COLORSTR.Green11, v1, v2))
    PetData.SetStrLevel(nil, self.ImgNum, pet.str_level)
    self.EleAttr:SetData(pet:EleAttr())

    self.ui_obj:SetData(DrawerHelper.GetNpcPath(cur_pet:ResId()),cur_pet:FeiShengTimes())
    self.NormalBg:SetActive(pet.config.mutant_times == PetData.MutantLv.Zero and pet.config.quality < PetData.Quality.Shen)
    -- self.MutantBg:SetActive(pet.config.mutant_times >= 1)
    self.Mutant:SetActive(pet.config.mutant_times >= 1)
    -- self.ShenShouBg:SetActive(pet.config.mutant_times == 0 and pet.config.quality == 5)
    PetView.SetTypeBgImage(self.TypeBgSetter,pet.config.quality,pet.config.mutant_times,true,pet:IsLianYao())
    self:FlushStrengthenAddList()

    local str_tip = ""
    local pet_qua = pet:Quality()
    local pet_str_level = pet:StrLevel()
    local pet_fs_times = pet:FeiShengTimes()
    local str_cfg = Cfg.PetStrengthenConsume(pet_qua, pet_str_level)    -- 强化消耗配置
    if str_cfg == nil or str_cfg.material_num == 0 or Pet.StrengthMaxLv(pet_qua) <= pet_str_level  then
        self.StrRightInfo:SetActive(false)
        self.MaxTip:SetActive(true)
        self.NeedFSMaxTip:SetActive(false)
    else
        if PetFeiShengData.IsStrengthenNeedFS(str_cfg.strengthen_level,pet_fs_times) then
            self.StrRightInfo:SetActive(false)
            self.NeedFSMaxTip:SetActive(true)
            self.MaxTip:SetActive(false)
        else
            self.StrRightInfo:SetActive(true)
            self.NeedFSMaxTip:SetActive(false)
            self.MaxTip:SetActive(false)
            if is_show_yao then
                str_tip = string.format(Language.Pet.StrTip, pet:QuaName())
            end
        end
    end
    UH.SetText(self.StrTip, str_tip)
    
    self.Tip:SetActive(not is_show_yao)
    self.TipYao:SetActive(is_show_yao)
    if is_show_yao then
        UH.SetText(self.StrPetTipYao, string.format(Language.Pet.StrSelTip3, cur_pet:QuaName()))
        PetData.SetStrLevel(nil, self.SuccSetYao, pet.str_level + 1)
        PetData.SetStrLevel(nil, self.DefeatSetYao, pet.str_level or 1)
    else
        UH.SetText(self.StrPetTip, string.format(Language.Pet.StrSelTip2, cur_pet:QuaName()))
        PetData.SetStrLevel(nil, self.SuccSet, pet.str_level + 1)
        PetData.SetStrLevel(nil, self.DefeatSet, pet.str_level or 1)
    end

    UH.SetText(self.PetStrNewTip, Language.Pet.PetStrNewTip)
    UH.SetText(self.LblCost, str_cfg.bind_coin_consume)

    UH.SetText(self.PetName, string.format("%s  %s",level_str,cur_pet:Name()))
    if pet:Rank() >= 0 and pet:Rank() <= 9 then
        self.PetRankObj:SetActive(true)
        self.PetRankObj:SetRank(pet:Rank() + 1 )
    else
        self.PetRankObj:SetActive(false)
    end

    self.YaoMutantBtn:SetPet(pet)

    self:FlushStrSelInfo()
end

-- 刷新宠物强化选择信息
function PetStrengthenPanel:FlushStrSelInfo()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    local is_yao = false
    if self.is_lian_yao and pet:IsLianYao() then
        pet = pet:YaoPet()
        is_yao = true
    end
    if pet.index ~= self.pet_data.sel_pet_str_data.pet_index then
        self.pet_data:ClearSelPetStrData(pet.index)
    end
    self.cfg_list_data = self.pet_data:GetStrCfgListData(pet,is_yao)
    self.SelPetList:SetData(self.cfg_list_data)
end

function PetStrengthenPanel:FlushStrengthenAddList()
    local strengthen_add_list = {}
    local pet = self.pet_data:GetCurSelect()
    local is_yao = false
    if self.is_lian_yao and pet:IsLianYao() then
        pet = pet:YaoPet()
        is_yao = true
    end
    if pet then
        local start_level = pet:StrLevel() > 1 and pet:StrLevel() or 2
        for i= 2, Pet.StrengthMaxLv(pet:Quality()) do
            table.insert(strengthen_add_list,{str_level = i, pet = pet, is_yao = is_yao})
        end
    end

    self.StrengthenAddListYao:SetObjActive(is_yao)
    self.StrengthenAddList:SetObjActive(not is_yao)
    local list_mono = is_yao and self.StrengthenAddListYao or self.StrengthenAddList
    list_mono:SetData(strengthen_add_list)
    local attr_config = self.pet_data:GetStrengthenCfg(pet.config.strengthen_id, 2)
    local low_attr_config = self.pet_data:GetStrengthenCfg(pet.config.strengthen_id, 1)
    local row_count = PetData.GetRowCount(attr_config, low_attr_config, pet.config.main_attribute)
    local jump_index = pet:StrLevel() > 1 and pet:StrLevel() - 1 or 1
    local jump_height
    local item_h
    if is_yao then
        item_h = 1 * 30 + 15 + 108
        jump_height = (jump_index - 1) * (1 * 30 + 15 + 108)
    else
        item_h = row_count * 30 + 15 + 108
        jump_height = (jump_index - 1) * (row_count * 30 + 15 + 108)
    end
    list_mono:JumpVerticalIndex(jump_index, item_h)
    --self.StrengthenAddList:ForceJumpVertical(jump_height)
    -- TimeHelper:CancelTimer(self.time_ht)
    -- self.time_ht = Invoke(function ()
    --     self.StrengthenAddList:JumpVerticalIndex(jump_index, item_h, -50)
    -- end,0.1)
end

function PetStrengthenPanel:OnDestroy()
    GuideManager.Instance:UnRegisterGetGuideUi("PetStrengthenBtn")
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.pet_data.str_copet_level = 1
    self.ui_obj = nil
    if self.shop_pet_care then
        self.pet_data:ShopPetInfo():Uncare(self.shop_pet_care)
    end
    if self.send_delay_timer then
        TimeHelper:CancelTimer(self.send_delay_timer)
        self.send_delay_timer = nil
    end
    self.pet_data.strength_use_protect = 0
    self.pet_data:ClearShopPetInfo()
    self.pet_data:ClearSelPetStrData()
    TimeHelper:CancelTimer(self.time_ht)
end

function PetStrengthenPanel:OnStrTypeClick(value)
    self.is_lian_yao = value or false
    local pet = self.pet_data:GetCurSelect()
    self.pet_data:ClearSelPetStrData(pet.index)
    if pet and pet:IsLianYao() then
        self:FlushShowInfo()
    end
end

function PetStrengthenPanel:OnClickTipRich()
    MainOtherCtrl.Instance:GetWayViewById(self.pet_data:GetCurSelect().pet_id)
end

function PetStrengthenPanel:OnClickDrag()
   
end

function PetStrengthenPanel:OnClickChoose(order_type)
end

function PetStrengthenPanel:OnClickStrengthen()
    if #self.cfg_list_data > #self.pet_data.sel_pet_str_data.info then
        PublicPopupCtrl.Instance:Center(ErrorText[3])
        return
    end

    if not self.sended then
        local copet1 = self.pet_data.sel_pet_str_data.info[1]
        local copet2 = self.pet_data.sel_pet_str_data.info[2]
        local copet3 = self.pet_data.sel_pet_str_data.info[3]
        local is_yao = self.is_lian_yao and 1 or 0
        PetCtrl.SendPetOp(PET_OP_TYPE.Breakthroughs, self.pet_data:GetSelectPetIndex(), copet1 and copet1.index or -1, copet2 and copet2.index or -1, copet3 and copet3.index or -1, is_yao)
        self.pet_data:ClearSelPetStrData()
            self.send_delay_timer = TimeHelper:AddDelayTimer(function()
                self.sended = false
            end,1)
        self.sended = true
    end
end

function PetStrengthenPanel:OnClickEcho()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    --ViewMgr:OpenView(PetStrBuff)
    ViewMgr:OpenView(PetResonanceView)
end

function PetStrengthenPanel:OnClickUseProtect()
   
end

function PetStrengthenPanel:OnClickAlertCancel()
   
end

function PetStrengthenPanel:OnClickAlertConfirm()
    
end

function PetStrengthenPanel:OnClickStrItem(data)
   
end
function PetStrengthenPanel:OnClickShopStrItem(data)
   
end

function PetStrengthenPanel:OnClickTip()
    ViewMgr:OpenView(PetStrengthenTipView,{index = 2})
    -- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[16].desc})
end
function PetStrengthenPanel:OnClickTip2()
    ViewMgr:OpenView(PetStrengthenTipView,{index = 1})
    -- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[19].desc})
end

function PetStrengthenPanel:OnClickCheckConfig()
    if self.pet_data.pet_skill_desc_id_list == nil then
        LogError("CheckCfg失败，联系韦方晨")
    end
    for k, v in pairs(self.pet_data.pet_skill_desc_id_list) do
        local loged = false
        for lv, cfg in ipairs(v) do
            if cfg.skill_level ~= 1 then
                local test_rates1 = string.split(tostring(cfg.skill_value), "|")
                local test_rates2 = string.split(tostring(v[lv - 1].skill_value), "|")
                if #test_rates1 ~= #test_rates2 then
                    LogError("skill_id",k,"skill_name",cfg.skill_name,"skill_level",cfg.skill_level,"前一级和本级描述数据不匹配 k",lv, "\n前一级参数",#test_rates1,"本级参数",#test_rates2)
                end
                local str_t = {}
                for i=1,#test_rates1 do
                    table.insert(str_t, test_rates1[i])
                    table.insert(str_t, "arrow")
                    table.insert(str_t, "color")
                    table.insert(str_t, test_rates2[i])
                end
                xpcall(function()
                    Format(cfg.description_2, unpack(str_t))
                end,
                function (err)
                        if not loged then
                            loged = true
                            LogError("skill_id",k,"skill_level", cfg.skill_level,"str_t",str_t)
                            LogError("hideAttrs ErrorLog:"..err)
                        end
                end)
            end
        end
    end
end


function PetStrengthenPanel:OnClickGotoFS()
    ViewMgr:OpenViewByKey(Mod.Pet.PetFeiSheng)
end
-------------------------------------
-- PetStrItem = PetStrItem or DeclareMono("PetStrItem", UIWidgetBaseItem)
-- function PetStrItem:PetStrItem()
--     self.pet_data = PetData.Instance
--     self.lang = Language.Pet
-- end

-- function PetStrItem:SetData(data)
--     local pet = data
--     if self.IsShop then
--         local info = data
--         pet = Pet.New(data.item_data.param)
--         UH.SetText(self.LblPrice, info.price)
--     elseif self.IsStrength then
--         if pet.level then
--             UH.SetText(self.LblLevel, string.format(self.lang.LevelStr, pet.level))
--         else
--             UH.SetText(self.LblLevel, "")
--         end
--     end
--     self.Cell:SetData(pet)
--     UIWidgetBaseItem.SetData(self, data)
--     local v1 = pet:TotalGrowth()
--     local v2 = pet:ConfigGrowth()
--     UH.SetText(self.LblName, pet.name)
--     UH.SetText(self.LblValue, string.format(self.lang.GrowthStr, COLORSTR.Yellow10, COLORSTR.Green11, v1, v2))
--     if self.PetState then
--         local state, is_follow = self.pet_data:GetPetState(pet.index)
--         UH.SpriteName(self.PetState, PetData.State[state])
--     end
--     if self.LockObj then
--         self.LockObj:SetActive(pet.info.lock == 1)
--     end
--     self.pet = pet
-- end
-- function PetStrItem:OnClickInfo()
--     ViewMgr:OpenView(PetDetailView, self.pet)
-- end
-- function PetStrItem:Click()
--     if self.ItemClick then
--         self.ItemClick:Trigger()
--     end
-- end
-----------------------------------------------------
StrengthenAddItem = StrengthenAddItem or DeclareMono("StrengthenAddItem", UIWidgetBaseItem)
function StrengthenAddItem:StrengthenAddItem()
    self.pet_data = PetData.Instance
    RichTextHelper.Bind(self.ClickRich, BindTool.Bind(self.OnClickDesc, self))
end

function StrengthenAddItem:SetData(data)
    local pet = data.pet or self.pet_data:GetCurSelect()

    local is_not = data.str_level > pet:StrLevel()
    local y = 18
    if self.is_set_pos == 1 then
        y = 68
    elseif self.is_set_pos == 0 then
        y = 137
    end
    self.is_set_pos = data.is_yao and 1 or 0
    if is_not then
        self.DescText.fontSize = 20
        self.ClickRich.fontSize = 20
        self.DescText.gameObject:SetLocalPosition(Vector3.New(24, y, 0))
        UH.LocalScale(self.StrNumRect, Vector3.New(0.75,0.75,1))
    else
        self.DescText.gameObject:SetLocalPosition(Vector3.New(-5, y, 0))
        self.DescText.fontSize = 22
        self.ClickRich.fontSize = 22
        UH.LocalScale(self.StrNumRect, Vector3.New(1,1,1))
    end
    local qua = pet:Quality()
    self.UpTip1:SetActive(not is_not and qua >= 5)
    self.UpTip2:SetActive(not is_not and qua >= 5)
    self.UpTip11:SetActive(not is_not and qua < 5)
    self.UpTip22:SetActive(not is_not and qua < 5)

    local attr_config = self.pet_data:GetStrengthenCfg(pet.config.strengthen_id, data.str_level)
    local low_attr_config = self.pet_data:GetStrengthenCfg(pet.config.strengthen_id, data.str_level - 1)
    local row_count, strengthen_add_str = PetData.GetRowCount(attr_config, low_attr_config, pet.config.main_attribute)

    if data.is_yao then
        strengthen_add_str = string.format(Language.Pet.PetStrengthSkillLevel, tostring(data.str_level))
        self.Rect.sizeDelta = Vector2.New(360, 1 * 30 + 10 + 108)
        self.DescRect.sizeDelta = Vector2.New(297, 30)
    else
        strengthen_add_str = strengthen_add_str..string.format(Language.Pet.PetStrengthSkillLevel, tostring(data.str_level))
        self.Rect.sizeDelta = Vector2.New(360, row_count * 30 + 10 + 108)
        self.DescRect.sizeDelta = Vector2.New(297, row_count * 30)
    end

    local copyStr = RichTextHelper.LinkStr(Language.Pet.PetStrengthDetailDesc, "_ul",COLORSTR.Purple1)
    self.ClickRich:SetObjActive(not data.is_yao)
    UH.SetText(self.ClickRich, copyStr)

    UH.SetText(self.DescText, strengthen_add_str)
    UH.SpriteName(self.StrLevelImg, PetData.PetStrLevel[data.str_level])
    self.Interactor.Interactable = not is_not
    self.data = data

    -- 刷新下面的技能
    local gift_list = TableCopy(pet:GetHitBookSkillAndGiftList())
    local main_list = {}
    for i,v in pairs(gift_list) do
        if v.main_skill and not v.lian_yao_skill and v.skill_id > 0 then
            table.insert(main_list, v)
        end
    end
    for i = 1 ,2 do
        self["MainSkill" .. i]:SetObjActive(main_list[i] ~= nil)
        if main_list[i] ~= nil then
            main_list[i].skill_level = data.str_level
            main_list[i].interactor = not is_not
            self["MainSkill" .. i]:SetData(main_list[i])
            self["MainSkill" .. i]:SetGray(is_not)
        end
    end
end

function StrengthenAddItem:OnClickDesc()
    self.pet_data.pet_strengthen_detail_str_level = self.data.str_level
    ViewMgr:OpenView(PetStrengthenDetailTip,{str_level = self.data.str_level})
end

function StrengthenAddItem:OnClickGift(cell)
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
-----------------------------------------------------
PetStrAlert = PetStrAlert or DeclareView("PetStrAlert", "pet/pet_strengthen_alert")
VIEW_DECLARE_MASK(PetStrAlert,ViewMask.BgBlock)
function PetStrAlert:PetStrAlert()
    self.pet_data = PetData.Instance
end
function PetStrAlert:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
end
function PetStrAlert:OnFlush()
    local pet = self.pet_data:GetCurSelect()
    local pet_sel = self.pet_data:GetPetInfo(self.pet_data:GetPetStrSelect())
    local show_rate = false

    if pet_sel and pet_sel.config.mutant_times >= 1 then
        UH.SetText(self.MutantNoticeText,string.format(Language.Pet.PetStrengthStuffMutant, pet_sel:Name(), pet_sel:Name()))
    elseif pet_sel and pet_sel:Level() > 10 then
        UH.SetText(self.MutantNoticeText,string.format(Language.Pet.PetStrengthStuffLevel, pet_sel:Name(), pet_sel:Name()))
    end

end
function PetStrAlert:OnClickCheck()
    self.ImgCheck:SetActive(not self.ImgCheck.activeSelf)
    self.pet_data:SetIsShowStrAlert(not self.ImgCheck.activeSelf)
end
function PetStrAlert:OnClickOk()
    -- local p2 = self.pet_data:GetPetStrSelect()
    -- PetCtrl.SendPetOp(PET_OP_TYPE.Breakthroughs, self.pet_data:GetSelectPetIndex(), p2, self.pet_data.strength_use_protect)
    local pet_sel = self.pet_data:GetPetInfo(self.pet_data:GetPetStrSelect())
    local pet = self.pet_data:GetCurSelect()
    self.pet_data:SetSelPetStrData(pet.index, pet_sel)
    ViewMgr:CloseView(PetStrAlert)
    ViewMgr:CloseView(PetStrSelectCopet)
end
function PetStrAlert:OnClickCancel()
    ViewMgr:CloseView(PetStrAlert)
end
function PetStrAlert:OnClickClose()
    ViewMgr:CloseView(PetStrAlert)
end
-------------------------------------------
PetStrSucc = PetStrSucc or DeclareView("PetStrSucc", "pet/pet_strengthen_succ")
VIEW_DECLARE_LEVEL(PetStrSucc, ViewLevel.L1)
-- VIEW_DECLARE_MASK(PetStrSucc, ViewMask.BgBlockClose)
function PetStrSucc:PetStrSucc()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function PetStrSucc:LoadCallback(param)
    if param.close_callback_func then
        self.close_callback_func = param.close_callback_func
    end
end

function PetStrSucc:CloseCallback()
    if self.close_callback_func ~= nil then
        self.close_callback_func()
        self.close_callback_func = nil
    end
    -- 判断下有无宠物福利奖励
    RewardCtrl.Instance:PlayPetIntensifyRewardDialog()
    -- 判断下有无触发强化共鸣
    PetResonanceData.Instance:CheckResonance(true)
end
--------------
PetStrSuccPanel = PetStrSuccPanel or DeclareMono("PetStrSuccPanel", UIWFlushPanel)
function PetStrSuccPanel:PetStrSuccPanel()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
    end
    self.is_play_anim = false
    self.ShowAnim:ListenEvent("AnimStart",BindTool.Bind(self.OnAnimStart,self))
    self.AttrAnim:ListenEvent("AnimState",BindTool.Bind(self.OnAnimState,self))
    GuideManager.Instance:RegisterGetGuideUi("PetStrSuccClose", function()
        return self.CloseGuide, function ()
            ViewMgr:CloseView(PetStrSucc)
            MainViewData.Instance.rb_toggle.isOn = false
        end
    end)
end

function PetStrSuccPanel:OnDestroy()
    TimeHelper:CancelTimer(self.time_delay_handle)
    TimeHelper:CancelTimer(self.time_delay_handle2)
    TimeHelper:CancelTimer(self.time_delay_handle3)
    if self.set_info_delay then
        TimeHelper:CancelTimer(self.set_info_delay)
        self.set_info_delay = nil
    end
    self.pet_data.strengthen_result = nil
    UIWFlushPanel.OnDestroy(self)
end

function PetStrSuccPanel:onFlush()
    self.EleAttrObj:SetActive(false)
    local pet_info = self.pet_data:GetStrengthenResult()
    if pet_info then
        self.PetCell:SetData(Pet.New({pet_id = pet_info.pet_id, str_level = self.pet_data.str_copet_level or 1}))
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet_info:ResId()),pet_info:FeiShengTimes())
        self:PlayEffect()
    end
end

function PetStrSuccPanel:OnClickMask()
    if self.is_play_anim == false then
        return
    end
    self.EffectTool:Stop(self.effect_handle)
    TimeHelper:CancelTimer(self.time_delay_handle)
    TimeHelper:CancelTimer(self.time_delay_handle2)
    TimeHelper:CancelTimer(self.time_delay_handle3)
    PublicPopupCtrl.Instance:Center(Language.Pet.StrengthenEat)
    ViewMgr:CloseView(PetStrSucc)
    local pet_info = self.pet_data:GetStrengthenResult()
    if pet_info.str_level == PetData.MaxLevel then
        ViewMgr:CloseView(PetStrengthenPanel)
    else
        ViewMgr:FlushView(PetStrengthenPanel)
    end
end

function PetStrSuccPanel:OnClickDetail(param)
    if self.attr_anim_clickable then
        self.attr_anim_clickable = false
        if param == 0 then
            self.AttrAnim:SetTrigger(APH("Show"))
        elseif param == 1 then
            self.AttrAnim:SetTrigger(APH("Hide"))
        end
    end
end

function PetStrSuccPanel:PlayEffect()
    self.effect_handle = self.EffectTool:Play(10010212)
    local pet_info = self.pet_data:GetStrengthenResult()
    local old_level, is_succ = self.pet_data:OldStrLevel()
    self.time_delay_handle =
        TimeHelper:AddDelayTimer(
        function()
            if not self:Destroyed() then
                local audio_name = nil
                if is_succ then
                    audio_name = "qianghuachenggong"
                    if pet_info.is_yao_mutant then

                    else
                        self.effect_handle2 = self.EffectTool:Play(10010213)
                    end
                else
                    audio_name = "qianghuashibai"
                    self.effect_handle2 = self.EffectTool:Play(10010214)
                end
                AudioMgr:PlayEffect(AudioType.UI, audio_name)
            end
        end,
        2
    )
    self.time_delay_handle2 =
        TimeHelper:AddDelayTimer(
        function()
            if not self:Destroyed() then
                self:FlushView()
                if is_succ then
                    self.ShowAnim:SetTrigger("success")
                else
                    self.ShowAnim:SetTrigger("defeated")
                end
                self.time_delay_handle3 =
                    TimeHelper:AddDelayTimer(
                    function()
                        if not self:Destroyed() then
                            self.is_play_anim = true
                            self.TipStrObj:SetActive(true)
                        end
                    end,
                    1.5
                )
            end
        end,
        2.3
    )
end
function PetStrSuccPanel:OnEffectEnd()
    if not self:Destroyed() then
        -- LogError("播放结束")
        self.EffectTool:Stop(self.effect_handle)
        ViewMgr:FlushView(PetStrengthenPanel)
    end
end

function PetStrSuccPanel:OnAnimStart(param)
    local function setSkillInfo()
        if self:Destroyed() then
            return
        end
        local pet_info = self.pet_data:GetStrengthenResult()
        local skill_list = pet_info:SkillList()
        for i = 1, self.SkillInfoArray:Length() do
            local skill_info = skill_list[i]
            if skill_info then
                -- self.SkillInfoArray[i]:SetData(skill_list[i])
                self.SkillInfoArray[i]:SetObjActive(true)
            else
                self.SkillInfoArray[i]:SetObjActive(false)
            end
        end
    end
    if param == "def" then
        if self.set_info_delay ~= nil then
            TimeHelper:CancelTimer(Self.set_info_delay)
        end
        self.set_info_delay = TimeHelper:AddDelayTimer(setSkillInfo,0.4)
    elseif param == "show" then
        if self.set_info_delay ~= nil then
            TimeHelper:CancelTimer(Self.set_info_delay)
        end
        self.set_info_delay = TimeHelper:AddDelayTimer(setSkillInfo,1.3)
    end
end

function PetStrSuccPanel:OnAnimState(param)
    if param == "Shown" or param == "Hided" then
        self.attr_anim_clickable = true
    end
end

function PetStrSuccPanel:FlushView()
    local qua_green = QualityColorStr[2]
    local qua_red = QualityColorStr[6]

    local old_level, is_succ = self.pet_data:OldStrLevel()
    self.cur_own_index = self.pet_data:GetSelectPetIndex()
    local pet_info = self.pet_data:GetStrengthenResult()
    local pet_cfg = self.pet_data:GetPetCfg(pet_info.pet_id)
    PetData.SetStrLevel(nil, self.ImgNum, old_level)
    local cur_str_level = pet_info.is_show_yao and pet_info:YaoStrLevel() or pet_info.str_level
    PetData.SetStrLevel(nil, self.ImgNum1, cur_str_level)

    local is_show_yao = pet_info.is_show_yao == true
    self.Attr:SetActive(not is_show_yao)
    self.AttrYao:SetObjActive(is_show_yao == true)
    self.Grouth:SetActive(not is_show_yao)
    if is_show_yao then
        self.AttrYao:SetPet(pet_info)
    end

    local is_yao_mutant = pet_info.is_yao_mutant == true

    -- if pet_info.str_level == PetData.MaxLevel then
    --     ViewMgr:CloseView(PetStrengthenPanel)
    -- end

    -- 暂时隐藏元素属性
    -- self.EleAttrObj:SetActive(true)
    self.NameObj:SetActive(true)
    self.EleAttr:SetData(pet_info:EleAttr())
    UH.SetText(self.LblName, pet_info.name)
    -- local old_level = pet_info:StrLevel() - 1
    local v1 = pet_info:TotalGrowth(old_level)
    local v2 = pet_info:ConfigGrowth(old_level)
    local v3 = pet_info:TotalGrowth()
    local v4 = pet_info:ConfigGrowth()
    local add = v3 - v1
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr, COLORSTR.Yellow13, COLORSTR.Green4, v1, v2))
    UH.SetText(
        self.LblValue2,
        string.format(
            self.lang.GrowthStr3,
            v3,
            add > 0 and qua_green or qua_red,
            add > 0 and string.format("+%s", v3 - v1) or v3 - v1,
            v4,
            add > 0 and qua_green or qua_red,
            add > 0 and string.format("+%s", v4 - v2) or v4 - v2
        )
    )
    --设置宠物属性
    self.AttrList:SetPet(pet_info)
    self:SetAttrInfo(old_level, is_succ, pet_info)
    
    local skill_list = pet_info:SkillList()
    for i = 1, self.SkillInfoArray:Length() do
        local skill_info = skill_list[i]
        if skill_info then
            self.SkillInfoArray[i]:SetData(skill_list[i])
        else
            self.SkillInfoArray[i]:SetObjActive(false)
        end
    end
    self.ShowObj:SetActive(true)
end

function PetStrSuccPanel:SetAttrInfo(old_level, is_succ, pet_info)
    local attr_config = self.pet_data:GetStrengthenCfg(pet_info.config.strengthen_id, pet_info.str_level)
    local old_attr_config = self.pet_data:GetStrengthenCfg(pet_info.config.strengthen_id, old_level)
    local pet_attr_info = self.pet_data:GetPetAttrInfo(pet_info.index)
    local row_count = 1
    local change_value_list = {}
    for k,v in pairs(SimpleAttrNameForType) do
        if attr_config[v] ~= nil and attr_config[v] - old_attr_config[v] ~= 0 then
            change_value_list[row_count] = {}
            change_value_list[row_count].attr_name = string.format(Language.Common.Xstr, COLORSTR.Yellow2, Language.AttrItems.AttrNameSample[k])
            change_value_list[row_count].old_value = pet_attr_info[k] - (attr_config[v] - old_attr_config[v])
            change_value_list[row_count].changed_value = (is_succ and "(+" or "(-")..math.abs(attr_config[v] - old_attr_config[v])..")"
            change_value_list[row_count].new_value = pet_attr_info[k]
            row_count = row_count + 1
            if row_count > 2 then
                break
            end
        end
    end
    for i=1, 2 do
        if change_value_list[i] ~= nil then
            UH.SetText(self.AttrBefore[i], change_value_list[i].attr_name..change_value_list[i].old_value or "")
            local add_num = string.format(Language.Common.Xstr, is_succ and COLORSTR.Green3 or COLORSTR.Red8, change_value_list[i].changed_value or "")
            UH.SetText(self.AttrChange[i], change_value_list[i].new_value..add_num or "")
            self.AttrArrow[i]:SetActive(true)
        else
            UH.SetText(self.AttrBefore[i],"")
            UH.SetText(self.AttrChange[i],"")
            self.AttrArrow[i]:SetActive(false)
        end
    end
end

function PetStrSuccPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end

PetStrSuccYao = PetStrSuccYao or DeclareMono("PetStrSuccYao")
function PetStrSuccYao:SetPet(pet)
    local pet_info = PetData.Instance:GetStrengthenResult()
    self.Top:SetActive(pet_info.is_yao_mutant == true)
    self.AttrList:SetPet(pet)
    local v3 = pet:TotalGrowth()
    local v4 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(Language.Pet.GrowthStr9, v3, v4))
    local yao_pet = pet:YaoPet()
    local skill_list = yao_pet:SkillList()
    for i = 1, self.SkillInfoArray:Length() do
        local skill_info = skill_list[i]
        if skill_info then
            self.SkillInfoArray[i]:SetData(skill_list[i])
            self.SkillInfoArray[i]:SetObjActive(true)
        else
            self.SkillInfoArray[i]:SetObjActive(false)
        end
    end

    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        for i = 1, self.SkillInfoArray:Length() do
            local skill_info = skill_list[i]
            if skill_info then
                self.SkillInfoArray[i]:SetObjActive(true)
            end
        end
    end,1)
end

function PetStrSuccYao:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
end

-------------------------------------------------------
PetStrengthenTipView = PetStrengthenTipView or DeclareView("PetStrengthenTipView","pet/pet_strengthen_tip")
VIEW_DECLARE_LEVEL(PetStrengthenTipView, ViewLevel.L1)
function PetStrengthenTipView:PetStrengthenTipView()
    self.lang = Language.Pet
    self.lang_cfg_index = {
        [1] = {
            [1] = 19,
            [2] = 70,
            [3] = 71,
            [4] = 72,
            [5] = 73,
        },
        --角色进阶满2段时使用2
        [2] = {
            [1] = 16,
            [2] = 74,
            [3] = 75,
            [4] = 76,
            [5] = 77,
        },
        --角色进阶未满2段时使用3
        [3] = {
            [1] = 16,
            [2] = 74,
            [3] = 252,
            [4] = 253,
            [5] = 254,
        },
        --角色未满2段时消耗概述使用4
        [4] = {
            [1] = 19,
            [2] = 70,
            [3] = 307,
            [4] = 308,
            [5] = 309,
        },
    }
    self.cur_index = 1
end

function PetStrengthenTipView:LoadCallback(data)
    self.type_index = data.index
    self.Board:SetData(self:GetType(), self.lang.StrengthenTip[data.index], Vector2.New(1228, 598))
    self:SetStringList()
end

function PetStrengthenTipView:OnClickBtn(param)
    self.cur_index = param
    self:SetStringList()
end

function PetStrengthenTipView:SetStringList()
    if self.type_index == 2 and FlyUpData.Instance:GetStage() < FlyUpStageType.High then
        self.type_index = 3
    end
    if self.type_index == 1 and FlyUpData.Instance:GetStage() < FlyUpStageType.High then
        self.type_index = 4
    end
    
    local str_index = self.lang_cfg_index[self.type_index][self.cur_index]
    local str = Config.language_cfg_auto.textdesc[str_index]
    local try_split = Split(str.desc, "|")
    self.SplitList:SetData(try_split)
end
-------------------
PetStrengthenTipSplitItem = PetStrengthenTipSplitItem or DeclareMono("PetStrengthenTipSplitItem", UIWidgetBaseItem)
--CellTips=====Text
function PetStrengthenTipSplitItem:SetData(data)
    self.data = data

    local data_string = Split(self.data, "@")

    self.TextParent.text = data_string[1]

    if data_string[2] then
        self.TextChild.text = data_string[2]
    end 

    if data_string[3] then
        self.TextEnd.text = data_string[3]
    end
    if self.delay_set ~= nil then
        TimeHelper:CancelTimer(self.delay_set)
    end
    self.delay_set = TimeHelper:AddDelayTimer(function ()
        self.HorLayoutGroup.childControlWidth = false
    end, 0.2)
end

function PetStrengthenTipSplitItem:OnDestroy()
    if self.delay_set ~= nil then
        TimeHelper:CancelTimer(self.delay_set)
        self.delay_set = nil
    end
end

------------------------------------------
PetStrengthenSkillInfoItem = PetStrengthenSkillInfoItem or DeclareMono("PetStrengthenSkillInfoItem", UIWidgetBaseItem)
function PetStrengthenSkillInfoItem:PetStrengthenSkillInfoItem()
    self.pet_data = PetData.Instance
end

function PetStrengthenSkillInfoItem:SetData(data)
    local old_level, is_succ = self.pet_data:OldStrLevel()
    local skill_desc_cfg = self.pet_data:GetSkillConfig(data.skill_id, data.skill_level)
    local old_skill_desc_cfg = self.pet_data:GetSkillConfig(data.old_skill_id or data.skill_id, old_level)
    if skill_desc_cfg == nil or old_skill_desc_cfg == nil then
        if old_skill_desc_cfg == nil then
            skill_desc_cfg = self.pet_data:GetSkillConfig(data.skill_id,1)
        else
            skill_desc_cfg = old_skill_desc_cfg
        end
        self.LblDesc.text = skill_desc_cfg.description
        UH.SetIcon(self.SkillIcon, skill_desc_cfg.icon_id, ICON_TYPE.SKILL)
        return
    end
    local new_rates = string.split(tostring(skill_desc_cfg.skill_value), "|")
    local old_rates = string.split(tostring(old_skill_desc_cfg.skill_value),"|")
    local arrow_sprite = RichTextHelper.SpriteStrAtlas(is_succ and "TiShengJianTou_Lv" or "TiShengJianTou_Hong", 29, 18)
    local color_str = is_succ and COLORSTR.Green3 or COLORSTR.Red5
    local str_t = {}
    for i=1,#old_rates do
        table.insert(str_t, old_rates[i])
        table.insert(str_t, arrow_sprite)
        table.insert(str_t, color_str)
        table.insert(str_t, new_rates[i])
    end
    local str = string.format(skill_desc_cfg.description_2, unpack(str_t))
    
    self.LblDesc.text = str
    UH.SetIcon(self.SkillIcon, skill_desc_cfg.icon_id, ICON_TYPE.SKILL)

    local effect_id = skill_desc_cfg.asset_id
    if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(effect_id) then
        self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(effect_id), self.EffectParent)
    end
end

------------------------
PetStrengthenDetailTip = PetStrengthenDetailTip or DeclareView("PetStrengthenDetailTip","pet/pet_strengthen_detail_tip")
VIEW_DECLARE_MASK(PetStrengthenDetailTip, ViewMask.BlockClose)
function PetStrengthenDetailTip:PetStrengthenDetailTip()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function PetStrengthenDetailTip:LoadCallback(param)
    local pet = self.pet_data:GetCurSelect()
    local str_data = self.pet_data:GetStrengthenCfg(pet.config.strengthen_id, param.str_level)
    if str_data ~= nil and str_data.add_bp > 0 then
        UH.SetText(self.GrowthValue, string.format(self.lang.GrowthStr7, pet:MaxBp(), COLORSTR.Green10, tostring(str_data.add_bp / 100)))
    else
        UH.SetText(self.GrowthValue, pet:MaxBp())
    end
    UH.SpriteName(self.StrLevelImg, "PetNum"..param.str_level)
    self.AttrList:SetData(Language.Pet.AttrClass)
end
--------------------------------------------------------
PetStrengthenDetailAttrItem = DeclareMono("PetStrengthenDetailAttrItem", UIWidgetBaseItem)
--data来自 Language
function PetStrengthenDetailAttrItem:PetStrengthenDetailAttrItem()
    self.pet_data = PetData.Instance
end
function PetStrengthenDetailAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if next(data) == nil then
        return
    end
    self.LblType.text = data[3]
    local cell_data = self.pet_data:GetCurSelect()
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
    local str_level = self.pet_data.pet_strengthen_detail_str_level
    local str_cfg = self.pet_data:GetStrengthenCfg(cell_data.config.strengthen_id, str_level)
    local str = cell_data:PetCfg()[data[1]]
    local add = str_cfg[data[5]]
    if add > 0 then
        str = string.format(Language.Pet.AttrStr, str, COLORSTR.Green10, tostring(add / 100))
    end
    UH.SetText(self.LblValue, str)
end

PetStrengthenSelItem = PetStrengthenSelItem or DeclareMono("PetStrengthenSelItem", UIWidgetBaseItem)
-- data:{qua, str_num, pet, pos_index, is_oneself, cfg}
function PetStrengthenSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TipObj:SetActive(data.pet == nil)
    self.PetObj:SetActive(data.pet ~= nil)
    self.RemoveBtnObj:SetActive(data.pet ~= nil)
    if data.pet then
        self.PetCell:SetData(data.pet)
        UH.SetText(self.PetName, string.format("+%s %s",data.pet:StrLevel(),data.pet:Name()))
    else
        UH.SpriteName(self.Qua, PetData.PinZhi[data.qua])
        UH.SpriteName(self.StrNum, PetData.PetStrLevel[data.str_num])
        local pet = data.str_pet or PetData.Instance:GetCurSelect()
        local tip = data.is_oneself == 0 and Language.Pet.StrSelTip1 .. Language.Pet.ColorLevelT[data.qua] or pet:InitialName()
        local tip_str = string.format("+%s %s", data.str_num, tip)
        self.tip_str = tip_str
        UH.SetText(self.TipName, tip_str)
    end
    self.RedPoint:SetNum(0)
    local sel_pet = PetData.Instance:GetCurSelect()
    if not data.pet and sel_pet then
        local remind_num = PetData.Instance:GetStrengthenRemind(sel_pet)
        self.RedPoint:SetNum(remind_num)
    end
end

function PetStrengthenSelItem:OnAddClick()
    PetData.Instance.sel_pet_str_data.click_index = self.data.pos_index
    PetData.Instance.sel_pet_str_data.is_yao = self.data.is_yao
    ViewMgr:OpenView(PetStrSelectCopet, {pet_title = self.tip_str})
end

function PetStrengthenSelItem:OnRemoveClick()
    PetData.Instance:SetSelPetStrData(nil, nil, self.data.pos_index, false)
    PetData.Instance.sel_pet_str_data:Notify()
end


PetLianYaoMutanBtn = PetLianYaoMutanBtn or DeclareMono("PetLianYaoMutanBtn")
function PetLianYaoMutanBtn:SetPet(pet)
    self.pet = pet
    self.show_not_active_btn = false
    self.show_active_btn = false
    if pet:IsLianYao() then
        local yao_pet = pet:YaoPet()
        local quality = yao_pet:Quality()
        local mu_lv = yao_pet:MutTimes()
        local is_can_mutant = Pet.CanMutant(quality,mu_lv,pet:FeiShengTimes())
        if is_can_mutant then
            -- self.show_not_active_btn = mu_lv == 0
            -- self.show_active_btn = mu_lv ~= 0
            self.show_active_btn = true
        end
    end
    self.NotActiveBtn:SetActive(self.show_not_active_btn)
    self.ActiveBtn:SetActive(self.show_active_btn)
end

function PetLianYaoMutanBtn:OnClick()
    ViewMgr:OpenView(PetLianYaoMutantView, {pet = self.pet})
end