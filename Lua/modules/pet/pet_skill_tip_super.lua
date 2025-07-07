PetSkillTipSuper = PetSkillTipSuper or DeclareView("PetSkillTipSuper", "pet/pet_skill_tip_super")
VIEW_DECLARE_MASK(PetSkillTipSuper, ViewMask.BlockClose)
-- BtnForget
function PetSkillTipSuper:PetSkillTipSuper()
    self.pet_data = PetData.Instance
end
function PetSkillTipSuper:LoadCallback(param)
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    if param ~= nil and param.pos ~= nil  then
        UH.AnchoredPosition(self.PosRect, param.pos)
    end 
end
function PetSkillTipSuper:OnFlush()
    local data = self.pet_data:GetClickSkill()
    if data == nil then
        LogError("PetSkillTipSuper GetClickSkill data is nil")
        return
    end
    local corner_icon = nil
    local skill_cfg = nil
    if data.skill_cfg then --宠物唯一专用
        local skill_level_cfg = SkillData.GetConfig(data.skill_cfg.skill_id or data.skill_cfg.param_1, data.skill_cfg.skill_level)

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
        self.MainLevelObj:SetActive(data.main_skill)
        if data.main_skill then
            UH.SetText(self.MainLevelText, string.format(Language.Common.LvDot, data.skill_level))
            local effect_id = SkillData.GetDecConfig(data.skill_cfg.skill_id, data.skill_cfg.skill_level).asset_id
            if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(effect_id) then
                self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(effect_id), self.EffectParent)
            end
        end
    end
end

function PetSkillTipSuper:SetSkillBookSellPrice(data)
    local skill_cfg = PetData.Instance:GetGiftDetail(data.gift_id or data.skill_id)
    if skill_cfg == nil then
        UH.SetText(self.CostNum,"")
        return
    end
    self.skill_item_id = skill_cfg.item_id
	local _,config = MallData.Instance:GetShopItemConfig(skill_cfg.item_id)
    UH.SetText(self.CostNum, config.price)
end

function PetSkillTipSuper:OnClickMask()
    ViewMgr:CloseView(PetSkillTipSuper)
end
function PetSkillTipSuper:OnClickForget()
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
                ViewMgr:CloseView(PetSkillTipSuper)

                ViewMgr:CloseView(DialogTipsView)
            end 
        },
        cancel = {
            func =  function()
                ViewMgr:CloseView(PetSkillTipSuper)
                ViewMgr:CloseView(DialogTipsView)
            end 
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PetSkillTipSuper:OnClickLearn()
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
    ViewMgr:CloseView(PetSkillTipSuper)
end

function PetSkillTipSuper:OnClickMainSkillInfo()
    local data = self.pet_data:GetClickSkill()
    self.MainSkillTipObj:SetActive(false)
    self.MainSkillInfoObj:SetActive(true)
    self.MainSkillInfoList:SetData(self.pet_data:GetSkillLevelCfgList(data.skill_id))
    self.MainSkillInfoList:JumpVerticalIndex(data.skill_level, 114)
end
