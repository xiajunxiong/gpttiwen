
PartnerViewAttr = PartnerViewAttr or DeclareMono("PartnerViewAttr", UIWFlushPanel)

function PartnerViewAttr:PartnerViewAttr()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerBaseGradeInfo), EventProtocol.ToEventKey(SCPartnerFetter), EventProtocol.ToEventKey(SCPartnerSkillReq), 
        EventProtocol.ToEventKey(SCChangeFightStateReq), EventProtocol.ToEventKey(SCPartnerSkinSingleOper), EventProtocol.ToEventKey(SCPartnerSingleInfo),
        EventProtocol.ToEventKey(SCPartnerEqSingleSlots), EventProtocol.ToEventKey(SCPartnerAttrListInfo)}

    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Attr
    self.data.cur_sel_skin.skin_id = 0
    self.data.cur_sel_skin.skin_tab = false

    self.data_cares = {
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
        {data = self.data.cur_sel_skin, func = self.FlushSkinDetail, init_call = false},
        {data = BagData.Instance.item_grid,func = self.FlushRedPoint, init_call = false},
        {data = self.data.partner_attr_up_level_effect,func = self.FlushUpLevelEffect, init_call = false},
		{data = AttributeDanData.Instance.use_list_change,func = self.FlushAttributeDanData, init_call = false},
		{data = AttributeDanData.Instance.attribute_red_piont_partner,func = self.FlushAttributeRedPoint, init_call = false},
		{data = RoleData.Instance.base_data, func = self.FlushBtnLifeShow, keys = {"level"}, init_call = false},
    }
    self.partner_id_anim = 0
    self.detail_sp = {[0] = "ShuXingDiWen", "HuoBanShuXingFeiSheng1", "HuoBanShuXingFeiSheng2"}
    GuideManager.Instance:RegisterGetGuideUi("PartnerActiveUp10", function ()
        return self.UpLevelItem10.BtnUpObj, BindTool.Bind(self.OnClickUpLevel, self, 10)
    end)
end

function PartnerViewAttr:Awake()
    UIWFlushPanel.Awake(self)
    if IS_AUDIT_VERSION then
        for i = 1, self.AuditShow:Length() do
            self.AuditShow[i]:SetActive(not GLOBAL_CONFIG:PkgInfo().is_youyan)
        end
        for i = 1, self.AuditHide:Length() do
            self.AuditHide[i]:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
        end
    else
        for i = 1, self.AuditShow:Length() do
            self.AuditShow[i]:SetActive(false)
        end
        for i = 1, self.AuditHide:Length() do
            self.AuditHide[i]:SetActive(true)
        end
    end
end

function PartnerViewAttr:FlushAttributeDanData()
	 self:onFlush()
end

function PartnerViewAttr:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Attr
    self:onFlush()
end

function PartnerViewAttr:onFlush()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local info = data.info
    local activation_info = info.activation_info
    local activation_item = info.activation_item
    local vo = data.vo
    local fetter_cur = data.fetter_cur
    local consume_item = data.fetter_cur
    local fight_state = PartnerConfig.FightState
    self.data.cur_sel_skin.equip_skin_id = vo.skin_id
    self.data.cur_sel_skin.skin_id = 0
    
    local fly_flag = vo.soar_count
    UH.SpriteName(self.DetailSet, self.detail_sp[fly_flag])
    -- self.Name.text = info.name
    self.LevelSp.SpriteName = vo.up_level == 0 and "WeiJinJieDi" or "JinJieDi"
    self.Level.text = vo.up_level == 0 and self.language.level_z or string.format(self.language.level, DataHelper.GetDaXie(vo.up_level))

    self.QuaValShow.text = data:PartnerQuaValShowUp()
    -- local prof = Language.Common.Prof[info.job]
    -- local prof_sp = RoleData.GetProfSp(info.job)
    -- self.Prof.text = prof
    -- self.ProfI.SpriteName = prof_sp
    self.Cap.text = string.format(self.language.cap, vo.cap)
    self.FightToggle.isOn = vo.fight_state == fight_state.fight
    self.RestToggle.isOn = vo.fight_state == fight_state.rest

    -- local element = {}
    -- for i,v in ipairs(info.element) do
    --     local type = ElementSpriteType[v.type] or 0
    --     element[type] = v.num
    -- end
    -- self.Element:SetData(element)
    
    local attr_list = data:GetAttrList()
    self.Attr1:DataTransBattleAttrNormal(attr_list)
    self.OtherAttriList:SetData(self.data:GetOtherAttriListData())
    
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        self.FetterCells[i]:SetData(fetter_cur[i])
    end
    
    local lock_state = PartnerConfig.LockState
    -- self.BtnGo:SetActive(activation_info.activation_link ~= 0)
    self.DownTips:SetActive(vo.is_unlocked == lock_state.lock)
    self.UpLevelObj:SetActive(vo.is_unlocked == lock_state.unlock)
    -- self.SkillObjs:SetActive(vo.is_unlocked == lock_state.unlock)
    -- self.SkillsPreObj:SetActive(vo.is_unlocked == lock_state.lock)
    self.DownItemObj:SetActive(nil ~= activation_item)
    -- self.DownDeco:SetActive(vo.is_unlocked == lock_state.unlock)
    self.DownTipsLeft.text = nil == activation_item and string.format(self.language.get_way, activation_info.activation_desc) or ""
    if nil ~= activation_item then
        self.ConsumeItem:SetData(activation_item)
        self.DownTipsGetWay.text = data.get_way_tips or ""
    end
    
    local cur_up_info = data.cur_up_info
    local sel_skill_list = data.sel_skill_list
    local quality_skill_list = data.quality_skill_list
    -- local gift_info = PartnerCtrl.Instance:Data():GetPartnerGiftWithIdAndLevel(cur_up_info.skill_gift, cur_up_info.skill_gift_level)

    -- self.red_point_items = self.red_point_items or {}
    -- if PartnerConfig.LockState.unlock == vo.is_unlocked then
        -- local btn_replace = Language.Partner.BtnReplace
        for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
            if info then self.Skills[i]:SetData(quality_skill_list[i]) end
            -- local is_lock = vo.up_level + 1 < i 
            -- self.BtnReplaces[i].text = info.is_equip and btn_replace.forbid or btn_replace.enable
            -- self.BtnReplaceEnableObjs[i]:SetActive(not info.is_lock and not info.is_equip)
            -- self.BtnReplaceForbidObjs[i]:SetActive(not info.is_lock and info.is_equip)
            -- self.red_point_items[i] = self.red_point_items[i] or UiRedPoint.New(self.BtnReplaceObjs[i], Vector3.New(35,15,0))
            -- self.red_point_items[i]:SetNum((nil == info and not is_lock) and 1 or 0)
        end
    -- else
    --     for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
    --         local info = sel_skill_list[i]
    --         self.SkillsPre[i]:SetData(info)
    --     end
    -- end
    self.GiftCell:SetData(data.skill_f)
    -- self.GiftCell:SetData(gift_info)
    -- self.GiftName.text = string.format(self.language.gift, gift_info.name)
    -- self.GiftName.text = gift_info.name
    -- self.GiftContent.text = gift_info.desc

    UH.SetIcon(self.CrystalIcon, vo.crystal > 0 and Item.GetIconId(vo.crystal) or nil)
    self.CrystalEmpty:SetActive(0 == vo.crystal)
    self.CrystalNum.text = string.format(Language.EleCrystal.SimpleUseNum, vo.used_times)
    self.CrystalNumObj:SetActive(vo.crystal > 0)

    self.red_point_acti = self.red_point_acti or UiRedPoint.New(self.BtnGo, Vector3.New(77.7,24,0))
    self.red_point_acti:SetNum(data:ActiRedPoint() and 1 or 0)
    self.red_point_crystal = self.red_point_crystal or UiRedPoint.New(self.CrystalObj, Vector3.New(35,35,0))
    self.red_point_crystal:SetNum(data:CrystalRedPoint() and 1 or 0)

    self.red_point_biography = self.red_point_biography or UiRedPoint.New(self.BtnBiographyObj, Vector3.New(25,20,0))
    self.red_point_biography:SetNum(data:BiographyRedPoint() and 1 or 0)

    local btn_soar_show, soar_count = data:PartnerSoarBtnShow()
    self.BtnSoarObj:SetActive(btn_soar_show)
    self.BtnSoar.text = self.language.btn_soar_show[soar_count] or ""
    
    self.BtnSoarWeaponObj:SetActive(PartnerData.FuYuIsOpen())--RoleData.Instance:GetRoleLevel() >= 75)

    self.ImageSetterSoarWeapon:SetByPath(string.format(PartnerConfig.PngPathSoarWeaponS, info.arms_id))
    -- self.SoarWeaponName.text = vo.soar_count > 0 and info.arms_name or self.language.unactive
    self.SoarWeaponName.text = data:IsUnactiveWeapon() and self.language.unactive or info.arms_name

    self.RankItem:SetRank(vo.rank)
    self.RankItem:SetActive(PartnerData.PartnerRankShow(vo.rank))

    self.red_point_soar = self.red_point_soar or UiRedPoint.New(self.BtnSoarObj, Vector3.New(25,20,0))
    self.red_point_soar:SetNum(data:SoarRedPoint() and 1 or 0)

    self.red_point_soar_weapon = self.red_point_soar_weapon or UiRedPoint.New(self.BtnSoarWeaponObj, Vector3.New(35,35,0))
	self.red_point_soar_weapon:SetNum(data:SoarWeaponRedPoint() and 1 or 0)

    self:FlushSkin(vo.skin_id)
    self:FlushDetail(info, vo)
    self:FlushModelShow()
	self:FlushAttributeRedPoint()
    self:FlushBtnLifeShow()
end

function PartnerViewAttr:FlushSkin(skin_id)
    local partner_skins = self.data.partner_skins[self.data.cur_sel_partner]
    local is_empty = nil == partner_skins
    self.SkinEmptyObj:SetActive(is_empty)
    self.SkinInfoObj:SetActive(not is_empty)
    if is_empty then return end
    self.SkinList:SetData(partner_skins)
    self.data.cur_sel_skin.skin_id = skin_id > 0 and skin_id or partner_skins[1].partner_skin_id
    self.SkinContent.pivot = Vector2.New(1 == #partner_skins and 0.5 or 0, 1)
end

function PartnerViewAttr:FlushDetail(info, vo)
    local prof = Language.Common.Prof[info.job]
    local prof_sp = RoleData.GetProfSp(info.job)
    self.ProfI.SpriteName = prof_sp
    self.Name.text = info.name
    self.Quality.text = self.data:GetPartnerQualityUpDesc(self.data.cur_sel_partner)
    self.LevelDesc1.text = vo.level
    self.LevelDesc2.text = string.format(self.language.level_desc, RoleData.Instance:GetRoleLevel())
    if PartnerConfig.LockState.unlock == vo.is_unlocked then
        self.UpLevelItem1:SetData(info, 1, vo)
        self.UpLevelItem10:SetData(info, 10, vo)
    end
end

function PartnerViewAttr:FlushModelShow(skin_id, is_test)
    skin_id = skin_id or 0
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
        local audit_seq = PartnerData.partner_audit_show[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
        -- UH.SpriteName(self.PartnerImg, "AuditImg_"..audit_seq.."_"..self.data.cur_sel_partner)
        self.PartnerImg:SetByPath(string.format(PartnerConfig.AuditPath, audit_seq, self.data.cur_sel_partner))
        return
    end
    -- skin_id = (skin_id and self.data.cur_sel_skin.skin_tab) and skin_id or nil
    -- skin_id = skin_id or data.vo.skin_id
    local skin_id = (0 == data.vo.skin_id and self.data.cur_sel_skin.skin_tab) and data.skin_id_try or data.vo.skin_id
    if is_test then
        skin_id = data.skin_id_try
    end
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path, data.vo.soar_count)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                load_callback = function ()
                    self.ui_obj:PlayAnim(ChAnimType.Show)
                end
            }
        )
    else
        self.ui_obj:SetData(res_path, data.vo.soar_count)
    end
    if self.partner_id_anim ~= self.data.cur_sel_partner then
        self.partner_id_anim = self.data.cur_sel_partner
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
end

function PartnerViewAttr:FlushSkinDetail()
    local skin_id = self.data.cur_sel_skin.skin_id
    self.SkinDetailsObj:SetActive(skin_id > 0)
    local skin_info = self.data.partner_skin_ids[skin_id]
    self:FlushModelShow(skin_id)
    if 0 == skin_id or nil == skin_info then return end
    self.SkinName.text = skin_info.name
    self.SkinDeco.text = string.format(Language.Partner.Skin.get_way, skin_info.describe)
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local is_actived = self.data.partner_skin_actived[skin_id]
    self.BtnSkinObj:SetActive(is_actived)
    self.BtnTryObj:SetActive(not is_actived)
    if is_actived then
        if data.vo.skin_id == skin_id then
            self.BtnSkin.text = Language.Partner.Skin.btn_equip.remove
        else
            self.BtnSkin.text = Language.Partner.Skin.btn_equip.equip
        end
    else
        if data.skin_id_try == skin_id then
            self.BtnTry.text = Language.Partner.Skin.btn_equip.try_off
        else
            self.BtnTry.text = Language.Partner.Skin.btn_equip.try_on
        end
        -- self.BtnSkin.text = Config.getway_cfg_auto.get_way[skin_info.get_way].desc
    end
end

function PartnerViewAttr:FlushRedPoint()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    self.red_point_acti = self.red_point_acti or UiRedPoint.New(self.BtnGo, Vector3.New(77.7,24,0))
    self.red_point_acti:SetNum(data:ActiRedPoint() and 1 or 0)
    self.red_point_crystal = self.red_point_crystal or UiRedPoint.New(self.CrystalObj, Vector3.New(35,35,0))
    self.red_point_crystal:SetNum(data:CrystalRedPoint() and 1 or 0)
	self:FlushAttributeRedPoint()
    self:FlushLifeShowRedPoint()
end

function PartnerViewAttr:FlushAttributeRedPoint()
    if -1 == self.data.cur_sel_partner then return end
    self.red_point_attribute = self.red_point_attribute or UiRedPoint.New(self.AttributeObj, Vector3.New(23,23,0))
    self.red_point_attribute:SetNum(AttributeDanData.Instance:GetOnePartnerPiontNum(self.data.cur_sel_partner))
end

function PartnerViewAttr:FlushUpLevelEffect()
    if not self.data.partner_attr_up_level_effect.val then return end
    if self.up_level_effect1 then
        self.UIEffectTool:Stop(self.up_level_effect1)
        self.up_level_effect1 = nil
    end
    if self.up_level_effect2 then
        self.UIEffectTool:Stop(self.up_level_effect2)
        self.up_level_effect2 = nil
    end
    AudioMgr:PlayEffect(AudioType.UI, "progress_add")
    self.data.partner_attr_up_level_effect.val = false
    self.up_level_effect1 = self.UIEffectTool:Play(10010066)
    self.up_level_effect2 = self.UIEffectTool:Play(10010067)
    self.UpLevelAnim:SetTrigger("level_big")
end

function PartnerViewAttr:FlushBtnLifeShow()
    local is_open = FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerLife)
    self.BtnLifeObj:SetActive(is_open)
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local vo = data.vo
    local pe_list = data.pe_list
    for i = 1, PartnerConfig.PARTNER_PE_MAX do   
        local pe = pe_list[i]
        self.PEItems[i]:SetActive(pe.item_id > 0)
    end
    self:FlushLifeShowRedPoint()
end

function PartnerViewAttr:FlushLifeShowRedPoint()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    self.red_point_life = self.red_point_life or UiRedPoint.New(self.BtnLifeObj, Vector3.New(20,20,0))
    self.red_point_life:SetNum(data:LifeActiveRedPoint() or data:LifeEquipRedPoint() and 1 or 0)
end

function PartnerViewAttr:OnClickSkin()
    local skin_id = self.data.cur_sel_skin.skin_id
    local skin_info = self.data.partner_skin_ids[skin_id]
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    if self.data.partner_skin_actived[skin_id] then
        if data.vo.skin_id == skin_id then
            PartnerCtrl.Instance:SendPartnerSkinReqRemove(self.data.cur_sel_partner, skin_id) 
            data.skin_id_try = 0
        else
            PartnerCtrl.Instance:SendPartnerSkinReqEquip(self.data.cur_sel_partner, skin_id)
            -- data.skin_id_try = skin_id
        end
    else
        local is_test = false
        if data.skin_id_try == skin_id then
            data.skin_id_try = 0
            self.BtnTry.text = Language.Partner.Skin.btn_equip.try_on
        else
            is_test = true
            data.skin_id_try = skin_id
            self.BtnTry.text = Language.Partner.Skin.btn_equip.try_off
        end
        self:FlushModelShow(nil, is_test)
        -- GetWayData.OpenPanel(Config.getway_cfg_auto.get_way[skin_info.get_way])
    end
end

function PartnerViewAttr:OnClickGo()
    -- local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    -- local activation_info = data.info.activation_info
    -- Debuger.LogError("Go   ".. activation_info.activation_link)
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local info = data.info
    local activation_item = data.info.activation_item
    if activation_item:Num() < activation_item.cost_num then
        MainOtherCtrl.Instance:GetWayView({item = activation_item})
    else
        PartnerCtrl.Instance:SendPartnerActiPartner(self.data.cur_sel_partner)
        -- 对此处进行伙伴解锁的方式进行标记，接受了标记的伙伴将在激活后勾选天赋 -- 11242021R
        -- 拆除本逻辑，未激活的伙伴自然是未出战，勾选天赋已改为出战才能勾选 12212021R
        -- self.data:MarkGiftRedRemind(self.data.cur_sel_partner)
    end
end

function PartnerViewAttr:OnClickWash()
    if PartnerCtrl.Instance:UnLockTips() then return end
	ViewMgr:OpenView(PartnerViewAttrWash)
end

function PartnerViewAttr:OnClickReplace(index)
    if PartnerCtrl.Instance:UnLockTips() then return end
    -- if not ViewMgr:IsOpen(PartnerViewSkillReplace) then
    --     local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    --     local sel_skill_list = data.sel_skill_list
    --     self.data.cur_sel_skill = index
    --     self.data.cur_skill_info = sel_skill_list[index]
    --     ViewMgr:OpenView(PartnerViewSkillReplace)
    -- end
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local sel_skill_list = data.sel_skill_list
    PartnerCtrl.Instance:SendPartnerSkillActivate{index = index, is_equip = sel_skill_list[index].is_equip}
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function PartnerViewAttr:OnClickHelp()
	ViewMgr:OpenView(PetRestraintView)
end

function PartnerViewAttr:OnClickFight(is_fight)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
	local role_id = RoleData.Instance:GetRoleId()
	if data.vo.fight_state == 1 then
		UnityPlayerPrefs.SetInt(role_id.."partner_attribute_dan"..self.data.cur_sel_partner,1)
	else
		UnityPlayerPrefs.SetInt(role_id.."partner_attribute_dan"..self.data.cur_sel_partner,2)
	end
	AttributeDanData.Instance:RemindPartnerDataRole()
    local fight_order_list = self.data:GetPartnerFightOrderList():Val()
    if not is_fight or #fight_order_list < PartnerConfig.PARTNER_FIGHT_NUM_MAX then
        PartnerCtrl.Instance:SendPartnerFight(self.data.cur_sel_partner)
    else
        FormationData.Instance.partner_click_id = self.data.cur_sel_partner
        ViewMgr:OpenView(FormationView)
    end
end

function PartnerViewAttr:OnToggleUp(param)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    self.data.cur_sel_skin.skin_tab = 3 == param
end

function PartnerViewAttr:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function PartnerViewAttr:OnClickAttrExplain()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    ViewMgr:OpenView(AttrExplainView)
end

function PartnerViewAttr:OnClickCrystal()
    if -1 == self.data.cur_sel_partner then return end
    self.data.cur_sel_crystal.partner = self.data.cur_sel_partner
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    data.crystal_red_point_show = false
    data.crystal_red_point.val = false
    self:FlushRedPoint()
    self.data:PartnerCrystalRedPoint()
    if PartnerConfig.LockState.unlock == data.vo.is_unlocked then
        self.data.cur_sel_crystal.role = 0
        ViewMgr:OpenView(PartnerCrystalView)
    else
        PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
    end
end

function PartnerViewAttr:OnClickAttributeDan()
	if -1 == self.data.cur_sel_partner then return end
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	if PartnerConfig.LockState.unlock == data.vo.is_unlocked then
		AttributeDanData.Instance:SetAttributeDanType(3)
		AttributeDanData.Instance:SetCurSelPartner(self.data.cur_sel_partner)
		ViewMgr:OpenView(AttributeDanView)
    else
        PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
    end
end

function PartnerViewAttr:OnClickSkinTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[69].desc)
end

--传参(第几个, 属性类型)
function PartnerViewAttr:OnClickContainAttr(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = 152, y = 60 + pos_y}
    self:OpenAttrReasonView(pos_t, attr_type)
end

function PartnerViewAttr:OnClickOtherAttr(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = 152, y = 158 + pos_y}
    self:OpenAttrReasonView(pos_t, attr_type)
end

function PartnerViewAttr:OpenAttrReasonView(pos_t, attr_type)
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local info = data.info
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_PARTNER,
        param1 = info.partner_id,
        attr_type = attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end

function PartnerViewAttr:OnClickResetLevel()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if PartnerConfig.LockState.lock == data.vo.is_unlocked then
        PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
        return
    end
    if 1 == data.vo.level then
        PublicPopupCtrl.Instance:Center(self.language.reset_level1)
        return
    end
    ViewMgr:OpenView(PartnerLevelResetView)
end


function PartnerViewAttr:OnPressDownUpLevel(up_value)
    local value = up_value and up_value.value or 1
    TimeHelper:CancelTimer(self.time_rt_level)
    self.time_rt_level = TimeHelper:AddRunTimer(function()
        self:OnClickUpLevel(value)
    end, 0.5)
end

function PartnerViewAttr:OnPressUpUpLevel()
    TimeHelper:CancelTimer(self.time_rt_level)
end

function PartnerViewAttr:OnClickUpLevel(up_level)
    if -1 == self.data.cur_sel_partner then
        TimeHelper:CancelTimer(self.time_rt_level)
        return
    end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if not data:UpLevelIsEnough(up_level) then
        TimeHelper:CancelTimer(self.time_rt_level)
    end 
end

function PartnerViewAttr:OnClickBiography()
    if -1 == self.data.cur_sel_partner then return end
    self.data.cur_sel_crystal.partner = self.data.cur_sel_partner
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    if PartnerConfig.LockState.unlock == data.vo.is_unlocked then
        self.data:PartnerBiographyInfo(0, 0)
        -- ViewMgr:CloseView(PartnerView)
        ViewMgr:OpenView(PartnerBiographyView)
    else
        PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
    end
end

function PartnerViewAttr:OnClickSoar()
    if PartnerCtrl.Instance:UnLockTips() then return end
    ViewMgr:OpenView(PartnerSoarView)
end

function PartnerViewAttr:OnClickSoarWeapon()
    if PartnerCtrl.Instance:UnLockTips() then return end
    ViewMgr:OpenView(PartnerSoarWeaponView)
end    
    
function PartnerViewAttr:OnClickQuaVal()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[251].desc})
end

function PartnerViewAttr:OnClickLife()
    ViewMgr:OpenView(PartnerLifeView)
end


PartnerViewAttrSkinItem = DeclareMono("PartnerViewAttrSkinItem", UIWidgetBaseItem)

function PartnerViewAttrSkinItem:PartnerViewAttrSkinItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerViewAttrSkinItem:OnDestroy()
    self:uncareData()
end

function PartnerViewAttrSkinItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
	self.care_handles = {}

    self:careData(self.partner_data.cur_sel_skin, BindTool.Bind(self.FlushSelect, self), "skin_id")

    self.UsingObj:SetActive(self.partner_data.cur_sel_skin.equip_skin_id == self.data.partner_skin_id)
    self.ShowImageSetter:SetByPath(string.format(PartnerConfig.PngPath, data.partner_skin_id))
    self.SkinSign:SetData(data)
end

function PartnerViewAttrSkinItem:FlushSelect()
	if self.data.partner_skin_id == self.partner_data.cur_sel_skin.skin_id then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		UIWidgetBaseItem.SetSelected(self, false)
	end
end

function PartnerViewAttrSkinItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function PartnerViewAttrSkinItem:OnSelect(selected)
    local value = self.partner_data:CurSelSkin(self.data.partner_skin_id, selected)
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
    end
end

PartnerViewAttrUpLevelItem = DeclareMono("PartnerViewAttrUpLevelItem", UIWidgetBaseItem)

function PartnerViewAttrUpLevelItem:PartnerViewAttrUpLevelItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.up_value = { value = 1 }
end

function PartnerViewAttrUpLevelItem:SetData(data, up_value, vo)
    UIWidgetBaseItem.SetData(self, data)

    local have_num = RoleData.Instance:GetCurrency(CurrencyType.PartnerLevelItem)
    local cost_num = data["cost_up_level" .. up_value] or 0
    
    self.up_value.value = up_value
    self.CostNum.text = RichTextHelper.ColorStr(cost_num, have_num < cost_num and COLORSTR.Red7 or COLORSTR.Red15)

    self.red_point_up = self.red_point_up or UiRedPoint.New(self.BtnUpObj, Vector3.New(77.7,24,0))
	self.red_point_up:SetNum((PartnerConfig.FightState.fight == vo.fight_state and RoleData.Instance:GetRoleLevel() > vo.level and cost_num > 0 and have_num >= cost_num) and 1 or 0)
end

function PartnerViewAttrUpLevelItem:UpValue()
    return self.up_value
end

PartnerViewAttrAttrItem = PartnerViewAttrAttrItem or DeclareMono("PartnerViewAttrAttrItem", UIWidgetBaseItem)
function PartnerViewAttrAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = Language.PetOtherAttriNames[data.attr_type]
    local value = PartnerData.Instance:GetPartnerAttriValue(data.attr_type)
    if data.is_percent then  -- 是千分比
        value = value / 10
        value = string.format("%s%%",value)
    end
    UH.SetText(self.Name, name)
    UH.SetText(self.Value, value)
end

function PartnerViewAttrAttrItem:OnClickBaseAttr()
    local partner = PartnerData.Instance:GetCurSelect()
    if partner == nil then
        return
    end
    local pos_y = UH.GetRootPosition(self.gameObject).y
    local pos_t = {x = 170, y = pos_y}
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_PARTNER,
        param1 = partner.id,
        attr_type = self.data.attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end
