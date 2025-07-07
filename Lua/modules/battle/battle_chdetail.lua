BattleChdetail = BattleChdetail or DeclareView("BattleChdetail", "battle/battle_chdetail", 0, ViewLayer.Normal + 1)
VIEW_DECLARE_MASK(BattleChdetail,ViewMask.BlockClose)
function BattleChdetail:BattleChdetail()
    -- body
end
function BattleChdetail:LoadCallback(data)
    self.data = data
end
function BattleChdetail:OnFlush()
    self.EleAttr:SetData(self.data.info.element,false,self.data.info.is_chaos_element == 1)
    UH.SetText(self.Name,self.data.info.name)
    
    

    --[[ local list = {}
    for key, value in pairs(self.data.obj.buff_list) do
        table.insert( list, {value.buff_id,value.buff_level} )
    end
    self.BuffList:SetData(list) ]]
    if self.data.info.prof and self.data.info.prof > 0 then
        local prof = self.data.info.prof > 100 and RoleData.ProfIdToProfType(self.data.info.prof) or self.data.info.prof
        UH.SpriteName(self.ProfSp, ProfSettingB[prof])
        self.ProfSp.gameObject:SetActive(true)
    end
    if self.handle_care == nil then
        self.handle_care = self.data.obj.ch_data:Care(BindTool.Bind(self.FlushBuff,self),"flush_buff")
    end
    if self.handle_care2 == nil then
        self.handle_care2 = self.data.obj.ch_data:Care(BindTool.Bind(self.FlushBuff,self),"flush_special")
    end
    if self.handle_care3 == nil then
        self.handle_care3 = self.data.info:Care(BindTool.Bind(self.FlushHMP,self),"show_hp")
    end
    self:FlushHMP()
    self:FlushBuff()
    self.RaceObj:SetActive(false)
    local iType = ICON_TYPE.ITEM
    local icon_id = self.data.info.character_id--宠物icon_id=chid
    PartnerInfo.SetQuality(self.PartnerQua, 0)
    if self.data.info.character_type == BATTLE_CH_TYPE.PARTNER or self.data.info.character_type == BATTLE_CH_TYPE.PARTNER_ROBOT then
        local config = PartnerData.Instance:GetPartnerInfoById(tonumber(icon_id))
        if config then
                icon_id = config.info.icon_id
        end
        if self.data.info.skin_id ~= nil and self.data.info.skin_id ~= 0 then
            local co = Cfg.PartnerSkinById(self.data.info.skin_id)
            icon_id = co and co.head_portrait or icon_id
        end
        if config then
            local partner_qua = PartnerData.Instance:GetCurPartnerQualityInfo(config.id)
            PartnerInfo.SetQuality(self.PartnerQua, partner_qua.quality)
        end
    elseif self.data.info.character_type == BATTLE_CH_TYPE.ROLE or self.data.info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        icon_id = self.data.info.appearance.avatar_type
        UH.SetAvatar(self.Icon, self.data.info.appearance.avatar_type, self.data.info.appearance.avatar_id,self.data.info.appearance.avatar_quality)
        return
    elseif self.data.info.character_type == BATTLE_CH_TYPE.MONSTER then
        local config = Cfg.MonsterById(self.data.info.character_id)
        icon_id = config.icon_id
        if config then
            UH.SetText(self.RaceTex, Language.Pet.RaceClass[config.race])
            UH.SpriteName(self.RaceIcon, string.format("Race%s", config.race))
            self.RaceObj:SetActive(true)
        end
    elseif self.data.info.character_type == BATTLE_CH_TYPE.NPC_HELPER then
        local config = Config.npc_group_auto.npc_group[self.data.info.character_id]
        icon_id = config.icon_id
    elseif self.data.info.character_type == BATTLE_CH_TYPE.PET or self.data.info.character_type == BATTLE_CH_TYPE.PET_SHADOW then
        local pet_cfg = PetData.Instance:GetPetCfg(self.data.info.character_id)    
        if self.data.info.skin_id ~= nil and self.data.info.skin_id ~= 0 then
            local co = Cfg.PetSkinAttr(self.data.info.skin_id)
            if co and co.head_portrait then
                icon_id = co.head_portrait
            else
                icon_id = nil
            end
        elseif pet_cfg ~= nil then
            icon_id = pet_cfg.icon_id
        end
        if icon_id == nil then
            icon_id = pet_cfg and pet_cfg.icon_id or self.data.info.character_id--self.data.info.character_id
        end
        if pet_cfg then
            UH.SetText(self.RaceTex, Language.Pet.RaceClass[pet_cfg.race])
            UH.SpriteName(self.RaceIcon, string.format("Race%s", pet_cfg.race))
            self.RaceObj:SetActive(true)
        end
    end
    UH.SetIcon(self.Icon, icon_id, iType)
end
function BattleChdetail:CloseCallback()
    self.data.obj.ch_data:Uncare(self.handle_care)
    self.data.obj.ch_data:Uncare(self.handle_care2)
    self.data.info:Uncare(self.handle_care3)
    self.handle_care = nil
    self.handle_care2 = nil
    self.handle_care3 = nil
end
function BattleChdetail:NeedShowHp()
    local group_id = BattleData.Instance:BaseData().monster_group_id
    local is_pvp = BattleData.Instance:IsPVP()
    if group_id == 1001 or group_id == 1000 or group_id == 1006 then
        return true
    elseif is_pvp then
        return false
    end
    local mode_id = BattleData.Instance:BattleMode()
    local info = Config.sundries_auto.hp_disappear[mode_id]
    if info and info.is_disappear == 1 then
        return true
    end
    return false
end
function BattleChdetail:FlushHMP()
    if FreedomCode.BattleUINew and BattleData.IsEnemyIdx(self.data.info.grid_idx) then
        local hide_flag = true
        --[[ if self:NeedShowHp() then
            hide_flag = false
        end ]]
        self.NewScheme[1]:SetActive(not hide_flag or self.data.info.show_hp == 1)
        self.NewScheme[2]:SetActive(not hide_flag or self.data.info.show_hp == 1)
        self.NewScheme[3]:SetActive(not hide_flag or self.data.info.show_hp == 1)
        if not hide_flag or self.data.info.show_hp == 1 then
            self.NewScheme[4].sizeDelta = Vector2.New(312.1, 255.6)
            UH.SetText(self.Level, self.data.info.level == -1 and "??" or self.data.info.level)
        else
            self.NewScheme[4].sizeDelta = Vector2.New(312.1, 313.4)
            UH.SetText(self.Level, "??")
        end
        if not hide_flag or self.data.info.show_hp == 1 then
            self.MP:SetProgress(self.data.info.curmp/self.data.info.maxmp)
            self.HP:SetProgress(self.data.info.curhp/self.data.info.maxhp)
            if self.data.info.maxhp > 10000000 then
                self.HP:SetText(self.data.info.level == -1 and "????" or string.format( "%.2f%%", self.data.info.curhp/self.data.info.maxhp * 100))
            else
                self.HP:SetText(self.data.info.level == -1 and "????" or self.data.info.curhp.."/"..self.data.info.maxhp)
            end
            self.MP:SetText(self.data.info.level == -1 and "????" or self.data.info.curmp.."/"..self.data.info.maxmp)
        end
    else
        self.NewScheme[1]:SetActive(true)
        self.NewScheme[2]:SetActive(true)
        self.NewScheme[3]:SetActive(true)
        if self.data.info.spirit_point > -1 then
            self.NewScheme[4].sizeDelta = Vector2.New(312.1, 225.3)
            self.Bg3Rect.sizeDelta = Vector2.New(315, 90)
            self.SP:SetText(self.data.info.level == -1 and "????" or self.data.info.spirit_point.."/"..FightData.FightOtherDate().max_sp)
            self.SP:SetProgress(self.data.info.spirit_point/FightData.FightOtherDate().max_sp)
            self.SP:SetObjActive(true)
        else
            self.NewScheme[4].sizeDelta = Vector2.New(312.1, 255.6)
            self.SP:SetObjActive(false)
            self.Bg3Rect.sizeDelta = Vector2.New(315, 61)
        end
        if self.data.info.top_level and self.data.info.top_level > 0 then
            UH.SetText(self.Level, self.data.info.top_level)
            UH.Color(self.Level, COLORS.Red8)
            self.PeakLevel:SetActive(true)
        else
            UH.SetText(self.Level, string.format(Language.Main.Battle.Ji, self.data.info.level == -1 and "??" or self.data.info.level))
            UH.Color(self.Level, COLORS.White)
            self.PeakLevel:SetActive(false)
        end
	    AvatarEffect.SetImageEnabled(self.PeakImg,self.data.info.appearance.avatar_quality)

        self.MP:SetProgress(self.data.info.curmp/self.data.info.maxmp)
        self.HP:SetProgress(self.data.info.curhp/self.data.info.maxhp)
        if BattleData.Instance:IsPVP() and BattleData.IsEnemyIdx(self.data.info.grid_idx) then
            self.HP:SetText("????")
            self.MP:SetText("????")
        else
            if self.data.info.maxhp > 10000000 then
                self.HP:SetText(self.data.info.level == -1 and "????" or string.format( "%.2f%%", self.data.info.curhp/self.data.info.maxhp * 100))
            else
                self.HP:SetText(self.data.info.level == -1 and "????" or self.data.info.curhp.."/"..self.data.info.maxhp)
            end
            self.MP:SetText(self.data.info.level == -1 and "????" or self.data.info.curmp.."/"..self.data.info.maxmp)
        end
    end
end
function BattleChdetail:FlushBuff()
    local list = {}
    local temp
    for key, value in pairs(self.data.obj.buff_list) do
        temp = BattleData.BuffConfig(value.buff_id)
        if temp and temp.is_hide == 0 then
            table.insert( list, {value.buff_id,value.buff_level, value.sustain_num, value.buff_layer} )
        end
    end
    
    for key, value in pairs(self.data.obj.special_list) do
        temp = BagData.Instance:GetEquipEffectCfg(value)
        if temp and temp.is_hide == 0 then
            table.insert( list, {nil, key} )
        end
    end
    local spe_list = BattleData.Instance:GetSpecialEffectList(self.data.info.grid_idx)
    if spe_list ~= nil then
        for index, value in ipairs(spe_list) do
            if self:ForeachList(value, list) == true then
                temp = BagData.Instance:GetEquipEffectCfg(value)
                if temp and temp.is_hide == 0 then
                    table.insert( list, {nil, value} )
                end
            end
        end
    end
    self.BuffNone:SetActive(table.nums(list) == 0)
    self.BuffList:SetData(list)
end
function BattleChdetail:ForeachList(value, list)
    for k,v in pairs(list) do
        if v[1] == nil and value == v[2] then
            return false
        end
    end
    return true
end

BattleBuffItem = BattleBuffItem or DeclareMono("BattleBuffItem", UIWidgetBaseItem)
function BattleBuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.SustainNum,"")
    if data[1] ~= nil then
        local config = BattleData.BuffConfig(data[1])
        local config2 = BattleData.BuffLevelConfig(data[1],data[2])
        if config and config2 then
            UH.SetText(self.Name,config.buff_name)
            if config.max_merge_layer > 1 then
                UH.SetText(self.Desc,config2.desc .. string.format(Language.Main.Battle.Layer, data[4]))
            else
                UH.SetText(self.Desc,config2.desc)
            end
            UH.SpriteName(self.ArrowSp, config.is_positive_buff == 1 and "Zeng" or "Jian")
            UH.SetIcon(self.Icon,config.icon_id)
            UH.Color(self.Desc, config.is_positive_buff == 1 and COLORS.Green3 or COLORS.Red12)
            if config2.sustain_num and config2.sustain_num == 999 then
                UH.SetText(self.SustainNum, "")
            else
                UH.SetText(self.SustainNum, string.format(Language.Main.Battle.Sustain[config.sustain_type], data[3] or config2.sustain_num))
            end
        end
    end
    if data[2] ~= nil then
        local desc, icon_id, name = BagData.Instance:GetEquipEffectDesc(data[2], true)
        if desc ~= "" then
            UH.SetText(self.Name,name)
            UH.SetText(self.Desc,desc)
            UH.SpriteName(self.ArrowSp, "Zeng")
            UH.SetIcon(self.Icon, icon_id)
            UH.Color(self.Desc, COLORS.Green3)
        end
    end
end