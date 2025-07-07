
PartnerViewShowList = PartnerViewShowList or DeclareMono("PartnerViewShowList", UIWFlushPanel)

function PartnerViewShowList:PartnerViewShowList()
    self.flush_events = {EventProtocol.ToEventKey(SCChangeFightStateReq), EventProtocol.ToEventKey(SCPartnerBaseGradeInfo), EventProtocol.ToEventKey(SCRoleLevelChange), EventProtocol.ToEventKey(SCPartnerUseElementCrystal), EventProtocol.ToEventKey(SCPartnerSoarNotice), EventProtocol.ToEventKey(SCPartnerIntensifyInfo)}

    self.data = PartnerCtrl.Instance:Data()
    self.data.guide_partner = self.data.guide_partner or 0
    self.data.cur_sel_partner = self.data.guide_partner > 0 and self.data.guide_partner or -1
    self.data.guide_partner = 0

    GuideManager.Instance:RegisterGetGuideUi("PartnerViewItemFirst", function ()
        return self.ShowList.index_item[1], function ()
            self.ShowList.index_item[1]:Click()
		end
    end)

    local jump_lx = false
    local jump_h = false
    local jump_ml = false
    GuideManager.Instance:RegisterGetGuideUi("PartnerViewItemLY", function ()
        local partner = self.data:GetPartnerInfoById(4)
        local index = self.ShowList.data_t[partner]
        if self.ShowList.index_item[index] then
            return self.ShowList.index_item[index], self.ShowList.index_item[index]:Click()
        elseif not jump_lx then
            jump_lx = true
            local data, first_partner = self.data:GetPartnerAllInfoLockSort()
            self.ShowList:JumpVerticalIndex(self.data:GetPartnerInfoLockSortPos(data,4),106)
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("PartnerViewItemH", function ()
        local partner = self.data:GetPartnerInfoById(6)
        local index = self.ShowList.data_t[partner]
        if self.ShowList.index_item[index] then
            return self.ShowList.index_item[index], self.ShowList.index_item[index]:Click()
        elseif not jump_h then
            jump_h = true
            local data, first_partner = self.data:GetPartnerAllInfoLockSort()
            self.ShowList:JumpVerticalIndex(self.data:GetPartnerInfoLockSortPos(data,6),106)
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("PartnerViewItemML", function ()
        local partner = self.data:GetPartnerInfoById(1)
        local index = self.ShowList.data_t[partner]
        if self.ShowList.index_item[index] then
            return self.ShowList.index_item[index], self.ShowList.index_item[index]:Click()
        elseif not jump_ml then
            jump_ml = true
            local data, first_partner = self.data:GetPartnerAllInfoLockSort()
            self.ShowList:JumpVerticalIndex(self.data:GetPartnerInfoLockSortPos(data,1),106)
        end
    end)

    self.partner_show_jump = nil

end

function PartnerViewShowList:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.partner_show_jump ~= nil then
        TimeHelper:CancelTimer(self.partner_show_jump)
        self.partner_show_jump = nil
    end
end 

function PartnerViewShowList:onFlush()
    local data, first_partner = self.data:GetPartnerAllInfoLockSort()
    local jump_pos = self.data.cur_sel_partner < 0 and first_partner or self.data.cur_sel_partner
    self.ShowList:SetData(data)
    self.ShowList:ClickItemData(self.data:GetPartnerInfoById(jump_pos))

    if self.partner_show_jump ~= nil then
        TimeHelper:CancelTimer(self.partner_show_jump)
        self.partner_show_jump = nil
    end

    if self.data:PartnerShowListJump() then 
        self.partner_show_jump =  TimeHelper:AddDelayTimer(function ()
            self.partner_show_jump = nil
            if self.ShowList then 
                self.ShowList:JumpVerticalIndex(self.data:GetPartnerInfoLockSortPos(data,jump_pos),106)
            end 
        end,0.2)
        self.data:PartnerShowListJump(false) 
    end 

end
-- function PartnerViewShowList:OnClickItem(data)
--     self.data.cur_sel_partner = data.id
--     self.data.item_flush.val = not self.data.item_flush.val
--     ViewMgr:FlushView(PartnerView, PartnerViewLife:GetTypeName())
--     ViewMgr:FlushView(PartnerView, PartnerViewModel:GetTypeName())
-- end


PartnerViewShowItem = DeclareMono("PartnerViewShowItem", UIWidgetBaseItem)

function PartnerViewShowItem:PartnerViewShowItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerViewShowItem:OnDestroy()
    self:uncareData()
end

function PartnerViewShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}
    self:careData(self.partner_data.cur_sel_tabbar, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(self.partner_data.quality_flush, BindTool.Bind(self.PartnerQuality, self))
    self:careData(data.up_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.acti_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.skill_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.life_active_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.life_equip_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.crystal_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.attr_level_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.biography_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.flush_keys, BindTool.Bind(self.FlushCap, self), "flush_cap")
    self:careData(data.flush_keys, BindTool.Bind(self.FlushIcon, self), "flush_skin")
    self:careData(data.quality_red_point, BindTool.Bind(self.PartnerRedPoint, self))
    self:careData(data.gift_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(data.pact_equip_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(data.pact_up_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(data.soar_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(data.soar_weapon_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(data.strengthen_red_point, BindTool.Bind(self.PartnerRedPoint, self))
	self:careData(AttributeDanData.Instance.attribute_red_piont_partner, BindTool.Bind(self.PartnerRedPoint, self))

    local info = data.info
    local vo = data.vo
    
    local fly_flag = vo.soar_count
    self.FlyBgSub:SetData({fly_flag = fly_flag})


    local prof_sp = RoleData.GetProfSp(info.job)
    local lock_state = PartnerConfig.LockState

    self.Name.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name)
    self.Prof.SpriteName = prof_sp

    local level = RoleData.Instance:GetBaseData().level
    self.Level.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow16 or COLORSTR.Gray4, "Lv.".. level)

    self.Lock:SetActive(vo.is_unlocked == lock_state.lock)
    self.Normal:SetActive(vo.is_unlocked == lock_state.unlock)
    -- self.Cap1.text = vo.cap
    -- self.Cap2.SpriteName = data.cap_sp

    local fight_state = PartnerConfig.FightState
    -- self.BtnFight:SetActive(vo.fight_state == fight_state.rest)
    -- self.BtnRest:SetActive(vo.fight_state == fight_state.fight)
    -- self.FightSp:SetObjActive(vo.is_unlocked == lock_state.unlock and (vo.fight_state == fight_state.fight or 1 == vo.is_arena_match))
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan

    self.FightSp:SetObjActive(vo.is_unlocked == lock_state.unlock and vo.fight_state == fight_state.fight)
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan
    self.LeiObj:SetActive(vo.is_unlocked == lock_state.unlock and 1 == vo.is_arena_match)

    self.ItemInter.Interactable = (vo.is_unlocked == lock_state.unlock)

    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
    self.StrLevel.SpriteName = PartnerInfo.StrLevelSp(vo.intensify_level)
    local crystal = vo.crystal or 0
    if crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(crystal))
    end
    self.EleAttr:SetObjActive(crystal > 0)
    UIWPetHeadCell.EffAnimName(vo.soar_count, self.FSQuaAnim)
end

function PartnerViewShowItem:FlushIcon()
    UH.SetIcon(self.IconSp, PartnerData.IconId(self.data), ICON_TYPE.ITEM)
end

function PartnerViewShowItem:FlushCap()
    self.Cap1.text = self.data.vo.cap
    self.Cap2.SpriteName = self.data.cap_sp
end

function PartnerViewShowItem:PartnerQuality()
    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(self.data.id)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
end

function PartnerViewShowItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function PartnerViewShowItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.partner_data.cur_sel_partner = self.data.id
        self.partner_data.item_flush.val = not self.partner_data.item_flush.val
        -- ViewMgr:FlushView(PartnerView, PartnerViewLife:GetTypeName())
        self.partner_data:LifeActiveRedPointShow()
        ViewMgr:FlushView(PartnerView, PartnerViewModel:GetTypeName())
        if self.partner_data:IsInfoFromClient(self.data.id) then
            PartnerCtrl.Instance:SendPartnerSingleInfo(self.data.id)
        end
    end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end

function PartnerViewShowItem:OnClickFight()
    PartnerCtrl.Instance:SendPartnerFight(self.data.id)
end

function PartnerViewShowItem:PartnerRedPoint()
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(122,40,0))
    local up = PartnerConfig.PartnerViewTabs.Up == self.partner_data.cur_sel_tabbar.val and self.data:UpRedPoint()
    local acti = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:ActiRedPoint()
    local skill = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:SkillRedPoint()
    local life_active = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:LifeActiveRedPoint()
    local life_equip = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:LifeEquipRedPoint()
    local crystal = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:CrystalRedPoint()
    local attr_level = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:AttrLevelRedPoint()
    local biography = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:BiographyRedPoint()
    local qua = PartnerConfig.PartnerViewTabs.Quality == self.partner_data.cur_sel_tabbar.val and self.data:QualityRedPoint()
    local gift = PartnerConfig.PartnerViewTabs.Gift == self.partner_data.cur_sel_tabbar.val and self.data:GiftRedPoint()
	local attribute = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and AttributeDanData.Instance:GetOnePartnerPiont(self.data)
	local pact_equip = PartnerConfig.PartnerViewTabs.Contract == self.partner_data.cur_sel_tabbar.val and self.data:PactEquipRedPoint()
	local pact_up = PartnerConfig.PartnerViewTabs.Contract == self.partner_data.cur_sel_tabbar.val and self.data:PactUpRedPoint()
	local soar = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:SoarRedPoint()
	local soar_weapon = PartnerConfig.PartnerViewTabs.Attr == self.partner_data.cur_sel_tabbar.val and self.data:SoarWeaponRedPoint()
	local strengthen = PartnerConfig.PartnerViewTabs.Strengthen == self.partner_data.cur_sel_tabbar.val and self.data:StrengthenRedPoint()
	self.red_point_item:SetNum((up or acti or skill or life_active or life_equip or crystal or attr_level or biography or qua or gift or attribute or pact_equip or pact_up or soar or soar_weapon or strengthen) and 1 or 0)
end
