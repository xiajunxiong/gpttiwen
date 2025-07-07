-- 宠物祝福
PetPrayInfo = PetPrayInfo or DeclareMono("PetPrayInfo", UIWFlushPanel) -- 600  1120
function PetPrayInfo:PetPrayInfo()
    self.data = PetData.Instance
    self.data_cares = {
        {data = self.data.pray_info_data, func = self.FlushPanel, init_call = false},
        {data = self.data.pet_select, func = self.FlushPanel, init_call = false},
    }
end

function PetPrayInfo:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.Tips, Language.PetPray.Tips)
    self:FlushPanel()
end

function PetPrayInfo:FlushPanel()
    self:FlushEffectList()
    self:FlushPetList()
    self:FlushAttriList()
    self:FlushSelList()
end

function PetPrayInfo:FlushPetList()
    local pet = self.data:GetCurSelect()
    if pet == nil then
        return
    end
    local list_data = {}
    local vice_pet_list = pet:PrayVicePetList() or {}
    for i = 1,3 do
        local pet_index = vice_pet_list[i]
        list_data[i] = {pet_index = pet_index, pos_index = i}
    end
    self.VicePetList:SetData(list_data)
end

function PetPrayInfo:FlushAttriList()
    local pet = self.data:GetCurSelect()
    if pet == nil then
        return
    end
    local list_data = pet:ViceAttriAddition() or {}
    local is_empty = TableIsEmpty(list_data)
    self.AttriEmptyObj:SetActive(is_empty)
    self.AttriList:SetObjActive(not is_empty)
    if not is_empty then
        self.AttriList:SetData(list_data)
    end
end

function PetPrayInfo:FlushEffectList()
    local pet = self.data:GetCurSelect()
    if pet == nil then
        return
    end
    local pet_cfg = pet:PetCfg()
    if pet_cfg.benediction_id and pet_cfg.benediction_id > 0 then
        self.Content:SetSizeWithCurrentAnchors(1, 1170)
        self.EffectList:SetObjActive(true)
        self.EmptyEffectObj:SetActive(false)
        local list_data = self.data:GetPrayCfgGroup(pet:PrayId())
        self.EffectList:SetData(list_data)
    else
        self.Content:SetSizeWithCurrentAnchors(1, 650)
        self.EffectList:SetObjActive(false)
        self.EmptyEffectObj:SetActive(true)
    end
end

function PetPrayInfo:FlushSelList()
    if not self.SelPanelObj.activeSelf then
        return
    end
    local list_data = self.data:GetPraySelListData()
    if TableIsEmpty(list_data) then
        self.SelPanelObj:SetActive(false)
        PublicPopupCtrl.Instance:Center(Language.PetPray.Error3)
        return
    end
    self.PetSelList:SetData(list_data)
end

function PetPrayInfo:OnOpenSelPanelClick()
    self.SelPanelObj:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self:FlushSelList()
end

function PetPrayInfo:OnCloseSelPanelClick()
    self.SelPanelObj:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end


-- =========== 副宠item ==========
PetPrayViceItem = PetPrayViceItem or DeclareMono("PetPrayViceItem", UIWidgetBaseItem)
-- data : {pet_index, pos_index}
function PetPrayViceItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pet_index = data.pet_index or -1
    self.AddImgObj:SetActive(pet_index < 0)
    self.PetItemObj:SetActive(pet_index >= 0)
    self.RemoveBtn:SetActive(pet_index >= 0)
    self.StrImg:SetObjActive(pet_index >= 0)
    if pet_index >= 0 then
        local pet = PetData.Instance:GetPetInfo(pet_index)
        self.PetItemCell:SetData(pet)
        UH.SetText(self.PetName, pet:Name())
        UH.SetText(self.PetScore, string.format(Language.PetPray.Tip1,pet:GetScore()))
        UH.SpriteName(self.StrImg, PetData.PetStrLevel[pet:StrLevel()])
    end
    UH.SetText(self.IndexText, string.format(Language.PetPray.Tip2, DataHelper.GetDaXie(data.pos_index)))
end

function PetPrayViceItem:OnRemoveClick()
    local pet_index = self.data.pet_index or -1
    if pet_index >= 0 then
        PetCtrl.SendPetOp(PET_OP_TYPE.PetPrayCancel, pet_index)
    end
end

function PetPrayViceItem:OnFosterClick()
    PetData.Instance.TargetPet.pet_index = self.data.pet_index
end

-- ========== 副宠属性加成item ==========
PetPrayAttriAdditionItem = PetPrayAttriAdditionItem or DeclareMono("PetPrayAttriAdditionItem", UIWidgetBaseItem)
-- data:{attri_type, attri_value}
function PetPrayAttriAdditionItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_data = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.AttriName, string.format("%s：", attri_data.attr_name))
    UH.SetText(self.AttriValue, attri_data.attr_percent_value)
end

-- ========== 祝福效果item ==========
PetPrayEffectItem = PetPrayEffectItem or DeclareMono("PetPrayEffectItem", UIWidgetBaseItem)
-- data:pray_cfg
function PetPrayEffectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cur_pet = PetData.Instance:GetCurSelect()
    local is_active, active_info = PetData.Instance:CheckPrayCfgPet(data, cur_pet:PrayVicePetList())
    local cur_effect_cfg = cur_pet:PrayActiveCfg()
    local img_name
    local cur_txt = ""
    if cur_effect_cfg.benediction_level == data.benediction_level then
        img_name = "XiaoGuoTiao2"
        cur_txt = Language.PetPray.CurTxt
    else
        img_name = "XiaoGuoTiao"
    end
    UH.SpriteName(self.Img, img_name)
    self.Interactor.Interactable = is_active
    UH.SetText(self.LevelText, string.format("%s.", data.benediction_level))
    local content_txt = string.format(data.show_txt, active_info[1].name,active_info[2].name,active_info[3].name)
    if not is_active then
        content_txt = ColorStr(content_txt, COLORSTR.Gray3)
    end
    UH.SetText(self.ContentText, content_txt)
    UH.SetText(self.CurTxt,cur_txt)
end

-- ========== 祝福副宠选择item ==========
PetPraySelItem = PetPraySelItem or DeclareMono("PetPraySelItem", UIWidgetBaseItem)
-- data:{pet}
function PetPraySelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pet = data.pet
    local main_pet_index = pet:PrayMainIndex()
    self.FlagObj:SetActive(main_pet_index ~= nil)
    self.MutantFlag:SetActive(pet:IsMutant())
    --self.SelBtnObj:SetActive(main_pet_index == nil)
    UH.SetText(self.Name, pet:Name())
    UH.SetText(self.Score, pet:GetScore())
    UH.SetText(self.Level, pet:Level())
    UH.SetIcon(self.Icon, pet:IconId())
    UH.SpriteName(self.QuaIcon, pet:QuaIcon())
    UH.SpriteName(self.StrImg, PetData.PetStrLevel[pet:StrLevel()])
end

function PetPraySelItem:OnRemoveClick()
    PetCtrl.SendPetOp(PET_OP_TYPE.PetPrayCancel, self.data.pet.index)
end

function PetPraySelItem:OnSelClick()
    if self.data.pet:PrayMainIndex() ~= nil then
        return
    end
    local cur_pet = PetData.Instance:GetCurSelect()
    local vice_pet_list = cur_pet:PrayVicePetList() or {}
    if #vice_pet_list >= 3 then
        PublicPopupCtrl.Instance:Center(Language.PetPray.Error2)
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetPray, cur_pet.index, self.data.pet.index)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
end

PetPrayActiveTip = PetPrayActiveTip or DeclareView("PetPrayActiveTip", "pet/pet_pray_active_tip")
VIEW_DECLARE_MASK(PetPrayActiveTip,ViewMask.BgBlockClose)
--param_t:{cfg_list[]:pray_cfg}
function PetPrayActiveTip:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.cfg_list == nil or #param_t.cfg_list < 1 then
        return
    end
    self:FlushPanel(param_t.cfg_list)
end

function PetPrayActiveTip:FlushPanel(cfg_list)
    local cfg_len = #cfg_list
    self.ShowType1:SetActive(cfg_len == 2)
    self.ShowType2:SetActive(cfg_len == 1)
    if cfg_len == 2 then
        self.PanelTans:SetSizeWithCurrentAnchors(1, 400)
        self:HandleCfg(cfg_list[1], self.UpLevel1, self.UpTxt1)
        self:HandleCfg(cfg_list[2], self.UpLevel2, self.UpTxt2)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PetStrength)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
        self.PanelTans:SetSizeWithCurrentAnchors(1, 266)
        self:HandleCfg(cfg_list[1], self.ActiveLevel, self.ActiveTxt)
    end
end

function PetPrayActiveTip:HandleCfg(cfg, level_text, content_text)
    local name1 = ColorStr(PetData.Instance:GetPetCfg(cfg.activate_1).name, COLORSTR.Blue9)
    local name2 = ColorStr(PetData.Instance:GetPetCfg(cfg.activate_2).name, COLORSTR.Blue9)
    local name3 = ColorStr(PetData.Instance:GetPetCfg(cfg.activate_3).name, COLORSTR.Blue9)
    UH.SetText(level_text, string.format("%s.",cfg.benediction_level))
    UH.SetText(content_text, string.format(cfg.show_txt, name1, name2, name3))
end