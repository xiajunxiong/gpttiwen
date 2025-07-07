PetSkillTip = PetSkillTip or DeclareView("PetSkillTip", "pet/pet_skill_tip")
VIEW_DECLARE_MASK(PetSkillTip, ViewMask.BlockClose)
-- BtnForget
-- FSSkillChangeObj=====GameObject
-- FSSkillStrBtnObj=====GameObject
-- FSAttriObj===========GameObject
-- FSAttri1=============Text
-- FSAttri2=============Text
function PetSkillTip:PetSkillTip()
    self.pet_data = PetData.Instance
end
function PetSkillTip:LoadCallback(param)
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    if param ~= nil and param.pos ~= nil  then 
        UH.AnchoredPosition(self.PosRect, param.pos)
    end 
end
function PetSkillTip:OnFlush()
    local data = self.pet_data:GetClickSkill()
    if data == nil or (data.gift_id == nil and data.skill_id == nil and data.skill_cfg == nil) then
        LogError("PetSkillTip GetClickSkill data is nil")
        return
    end
    local corner_icon = nil
    local skill_cfg = nil
    self.ConsumeMpText:SetObjActive(data.hide_mp == nil)
    if data.skill_cfg then --宠物唯一专用
        local skill_level_cfg = SkillData.GetConfig(data.skill_cfg.skill_id or data.skill_cfg.param_1, data.skill_cfg.skill_level)
        if skill_level_cfg and skill_level_cfg.pet_consume_mp then
            UH.SetText(self.ConsumeMpText, string.format(Language.Pet.PetSkillConsumeMp, tostring(skill_level_cfg.pet_consume_mp / 100)))
        end
        UH.SetText(self.LblName, data.skill_cfg.skill_name or data.skill_cfg.name)
        local skill_desc = data.skill_cfg.desc_1 and data.skill_cfg or Cfg.SkillDescPet(data.skill_cfg.skill_id, data.skill_cfg.skill_level)
        UH.SetText(self.LblDesc, string.format(Language.Pet.SkillShow, skill_desc.description or skill_desc.desc_1))
        UH.SetIcon(self.SkillIcon, data.skill_cfg.icon_id, ICON_TYPE.SKILL)
        self.BtnLearn:SetActive(false)
        self.MoneyObj:SetActive(false)
        corner_icon = data.skill_cfg.corner_icon
        skill_cfg = data.skill_cfg
    elseif data.skill_config then
        UH.SetText(self.LblName, data.skill_config.skill_name)
        UH.SetText(self.LblDesc, Format(Language.Pet.SkillShow, data.skill_config.description))
        UH.SetIcon(self.SkillIcon, data.skill_config.icon_id, ICON_TYPE.SKILL)
        self.BtnLearn:SetActive(false)
        self.MoneyObj:SetActive(false)
        skill_cfg = data.skill_config
    else
        if data.show_learn then --宠物学习唯一专用
            self:SetSkillBookSellPrice(data)
            local has_num = Item.GetNum(self.skill_item_id)
            UH.SetText(self.LearnBtnText, has_num <= 0 and Language.Pet.SkillBtn[3] or Language.Pet.SkillBtn[1])
            self.NotEnoughObj:SetActive(has_num <= 0)
            self.BtnLearn:SetActive(true)
            self.MoneyObj:SetActive(false)--初级才显示商会中的价格，高级书商会不卖
        else
            self.BtnLearn:SetActive(false)
            self.MoneyObj:SetActive(false)
            self.NotEnoughObj:SetActive(false)
        end
        UH.SetText(self.ConsumeMpText, " ")
        local level = AmuletData.Instance:GetIntensifySkillLevel(data.skill_id)
        if level and level > 0 then
            data.skill_level = level
        end
        skill_cfg = PetData.Instance:GetGiftDetail(data.gift_id or data.skill_id, data.skill_level)
        corner_icon = data.corner_icon
        if skill_cfg then
            UH.SetText(self.LblName, skill_cfg.name)
            UH.SetText(self.LblDesc, string.format(Language.Pet.SkillShow, skill_cfg.desc_1))
            UH.SetIcon(self.SkillIcon, skill_cfg.icon_id, ICON_TYPE.SKILL)
        else
            UH.SetText(self.LblName, "")
            UH.SetText(self.LblDesc, "")
            UH.SetIcon(self.SkillIcon)
        end
    end
    if skill_cfg == nil and data.skill_id ~= 0 then
        LogError("pet_skill_tip skill_cfg is nil,data is",data)
    end
    if skill_cfg and skill_cfg.page and skill_cfg.page <= 3 then--目前技能界面就3种技能页（初级和高级第三页是特殊的，不现实技能品质）
        local imgName = skill_cfg and tostring(skill_cfg.page)
        UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[imgName])
        self.SkillQuaObj:SetActive(true)
    else
        self.SkillQuaObj:SetActive(false)
    end
    self.SkillCornerObj:SetActive(corner_icon ~= nil)
    if corner_icon ~= nil and corner_icon ~= "" then
        UH.SetText(self.SkillCornerText, corner_icon)
    end
    self.SkillLock:SetActive(data.main_skill)
    self.MainSkillTipObj:SetActive(data.main_skill)
    if self.MainLevelObj then
        self.MainLevelObj:SetActive(data.main_skill or data.fs_skill)
        if data.main_skill or data.fs_skill then
            UH.SetText(self.MainLevelText, string.format(Language.Common.LvDot, data.skill_level))
            if data.skill_cfg then
                if data.main_skill then
                    local effect_id = SkillData.GetDecConfig(data.skill_cfg.skill_id, data.skill_cfg.skill_level).asset_id
                    if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(effect_id) then
                        self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(effect_id), self.EffectParent)
                    end
                end
            else
                LogError("main_skill,SkillTip skill_cfg is nil, data is ",data)
            end
        end
    end
    if data.fs_skill and data.owner_pet == nil then--非玩家自己的宠物展示的话owner_pet不是空，不显示更换飞升技能
        self.FSSkillChangeObj:SetActive(true)
        self.FSSkillStrBtnObj:SetActive(data.skill_level < PetFeiShengData.Instance:SkillMaxLv())
    else
        self.FSSkillChangeObj:SetActive(false)       
    end 
    if data.fs_skill then
        self.FSAttriObj:SetActive(true)
        local cfg = PetFeiShengData.SkillLvupCfg(data.skill_id,data.skill_level)
        local format_str = "%s <color=#"..COLORSTR.Green3..">+%s</color>"
        local attri1_txt = string.format(format_str,Language.Common.AttrList[cfg.att_type1],DataHelper.Percent(cfg.att_type1, cfg.att_num1))
        local attri2_txt = string.format(format_str,Language.Common.AttrList[cfg.att_type2],DataHelper.Percent(cfg.att_type2, cfg.att_num2))
        local pet = PetData.Instance:GetPetInfo(data.pet_idx)
        local fs_attri_show_max = (pet and pet:FeiShengTimes() <= PetFeiShengData.Lv.None)
        if fs_attri_show_max then
            local max_cfg = PetFeiShengData.SkillLvupCfg(data.skill_id,PetFeiShengData.Instance:SkillMaxLv())
            attri1_txt = string.format(Language.PetFeiSheng.SkillMaxAttriFormat,attri1_txt,DataHelper.Percent(max_cfg.att_type1, max_cfg.att_num1))
            attri2_txt = string.format(Language.PetFeiSheng.SkillMaxAttriFormat,attri2_txt,DataHelper.Percent(max_cfg.att_type2, max_cfg.att_num2))
        end
        UH.SetText(self.FSAttri1,attri1_txt)
        UH.SetText(self.FSAttri2,attri2_txt)
    else
        self.FSAttriObj:SetActive(false)
    end
end

function PetSkillTip:SetSkillBookSellPrice(data)
    local skill_cfg = PetData.Instance:GetGiftDetail(data.gift_id or data.skill_id)
    if skill_cfg == nil then
        UH.SetText(self.CostNum,"")
        return
    end
    self.skill_item_id = skill_cfg.item_id
	local _,config = MallData.Instance:GetShopItemConfig(skill_cfg.item_id)
    UH.SetText(self.CostNum, config.price)
end

function PetSkillTip:OnClickMask()
    ViewMgr:CloseView(PetSkillTip)
end
function PetSkillTip:OnClickForget()
    local cur_data = self.pet_data:GetClickSkill()
    local skill_name = cur_data.skill_cfg ~=nil and cur_data.skill_cfg.skill_name or PetData.Instance:GetGiftDetail(cur_data.gift_id).name

    local param_t = {
        content = string.format(
            Language.Pet.PetForgetWarn,
            skill_name
        ),
        confirm = {
            func =  function()
                local pet = self.pet_data:GetCurSelect()
                if pet == nil then return end
                local data = self.pet_data:GetClickSkill()
                PetCtrl.SendPetOp(PET_OP_TYPE.RemoveSkill, pet.index, data.index)
                ViewMgr:CloseView(PetSkillTip)

                ViewMgr:CloseView(DialogTipsView)
            end 
        },
        cancel = {
            func =  function()
                ViewMgr:CloseView(PetSkillTip)
                ViewMgr:CloseView(DialogTipsView)
            end 
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PetSkillTip:OnClickLearn()
    local has_num = Item.GetNum(self.skill_item_id)
    local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, self.skill_item_id)
    --有书时弹出打书界面，没有书时跳转购买
    if has_num > 0 then
        local open = false
        local cur_pet = self.pet_data:GetCurSelect()
        if cur_pet then
            local pet_gift_list = cur_pet.info.gift_list
            for k,v in pairs(pet_gift_list) do
                if v.gift_id ~= 0 then
                    open = true
                    break
                end
            end
        end
        if open then
            self.pet_data:SetLearnPreSkill(self.pet_data:GetClickSkill())
            ViewMgr:OpenView(PetGiftReplace)
        else
            PublicPopupCtrl.Instance:Center(Language.Pet.NotFrameLearnSkill)
        end
    else
        if PetData.Instance.hit_book_type.index <= 1 then--初级
            MarketData.Instance:SetJumpIndex(self.skill_item_id)
            ViewMgr:OpenViewByKey(Mod.Mall.Market)
        elseif PetData.Instance.hit_book_type.index > 1 then --高级/超级
            TradeData.Instance:SetSearchItemId(self.skill_item_id)
            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
        end
    end
    ViewMgr:CloseView(PetSkillTip)
end

function PetSkillTip:OnClickMainSkillInfo()
    local data = self.pet_data:GetClickSkill()
    self.MainSkillTipObj:SetActive(false)
    self.MainSkillInfoObj:SetActive(true)
    local skill_info_list_data = self.pet_data:GetSkillLevelCfgList(data.skill_id)
    local max_str = Pet.StrengthMaxLv(data.pet_qua or PetData.Quality.Jin)
    if max_str < #skill_info_list_data then
        skill_info_list_data = TableCopy(skill_info_list_data)
        while max_str < #skill_info_list_data do
            table.remove(skill_info_list_data,#skill_info_list_data)
        end
    end
    self.MainSkillInfoList:SetData(skill_info_list_data)
    self.MainSkillInfoList:JumpVerticalIndex(data.skill_level, 114)
end

function PetSkillTip:OnChangeFSSkillClick()
    ViewMgr:CloseView(PetSkillTip)
    PetFeiShengData.Instance:MarkIsChangeSkill(true)
    ViewMgr:OpenView(PetFeiShengSkillSel)
end

function PetSkillTip:OnStrFSSkillClick()
    ViewMgr:CloseView(PetSkillTip)
    ViewMgr:OpenView(PetFSSkillLvupTipView)
end

------------------------------
PetSkillTipMainSkillInfoItem = PetSkillTipMainSkillInfoItem or DeclareMono("PetSkillTipMainSkillInfoItem", UIWidgetBaseItem)
function PetSkillTipMainSkillInfoItem:PetSkillTipMainSkillInfoItem()
    
end

function PetSkillTipMainSkillInfoItem:SetData(data)
    UH.SetText(self.LblDesc, string.format(Language.Pet.SkillShow, data.description))
    UH.SetText(self.LevelText,data.skill_level..Language.Common.Level)
end
------------------------------
PetSkillTipPro = PetSkillTipPro or DeclareView("PetSkillTipPro","pet/pet_skill_tip_pro")
VIEW_DECLARE_MASK(PetSkillTipPro, ViewMask.BlockClose)
function PetSkillTipPro:PetSkillTipPro()
end

function PetSkillTipPro:LoadCallback()

end
----------------------------------
PetSkillTipProFlush = PetSkillTipProFlush or DeclareMono("PetSkillTipProFlush",UIWFlushPanel)
function PetSkillTipProFlush:PetSkillTipProFlush()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.hit_book_type, func = self.FlushSkillInfo, keys ={"tip_pro_index"}}
    }
    local passive_cfg = Cfg.SkillPassive(self.pet_data.hit_book_type.suggest_skill_id)
    if passive_cfg then
        self.skill_info_list = {}
        for i=0,2 do
            local skill_info = TableCopy(Cfg.SkillPassiveByType(passive_cfg.skill_type,i))
            skill_info.skill_cfg = self.pet_data:GetGiftDetail(skill_info.skill_id)
            table.insert(self.skill_info_list, skill_info)
        end
        self.passive_cfg = passive_cfg
        self.pet_data.hit_book_type.tip_pro_index = self.passive_cfg.is_same_type + 1
    else
        self.pet_data.hit_book_type.tip_pro_index = 3
    end
    
end

function PetSkillTipProFlush:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.pet_data.hit_book_type.tip_pro_index = 3
end

function PetSkillTipProFlush:FlushSkillName()
    for i=1,self.ToggleTextArray:Length() do
        UH.SetText(self.ToggleTextArray[i], self.skill_info_list[i].skill_cfg.name)
        self.SuggestSignArray[i]:SetActive(self.skill_info_list[i].skill_id == self.pet_data.hit_book_type.suggest_skill_id)
    end
    self.seted = true
end

function PetSkillTipProFlush:FlushSkillInfo()
    self.ToggleArray[self.pet_data.hit_book_type.tip_pro_index].isOn = true
    local skill_info = self.skill_info_list[self.pet_data.hit_book_type.tip_pro_index]
    local skill_cfg = skill_info.skill_cfg

    UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[skill_cfg.page])
    UH.SetIcon(self.SkillIcon, skill_cfg.icon_id)
    UH.SetText(self.LblDesc, skill_cfg.desc_1)
    if not self.seted then
        self:FlushSkillName()
    end
end

function PetSkillTipProFlush:OnClickToggle(param)
    self.pet_data.hit_book_type.tip_pro_index = param
end
