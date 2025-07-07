-- 神域探索上阵界面
ShenYuProbeUpFight = ShenYuProbeUpFight or DeclareView("ShenYuProbeUpFight", "shen_yu_probe/shen_yu_probe_up_fight")
function ShenYuProbeUpFight:OnCloseClick()
    ViewMgr:CloseView(ShenYuProbeUpFight)
end

function ShenYuProbeUpFight:OnRankClick()
    ViewMgr:OpenView(ShenYuProbeBoosRank)
end


ShenYuProbeUpFightPanel = ShenYuProbeUpFightPanel or DeclareMono("ShenYuProbeUpFightPanel", UIWFlushPanel)
function ShenYuProbeUpFightPanel:ShenYuProbeUpFightPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.role_data, func = self.FlushPanel, init_call = false},
        {data = self.data.boss_data, func = self.FlushPanel, init_call = false},
    }
end

function ShenYuProbeUpFightPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShenYuProbeUpFightPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelBossId(nil)
end

function ShenYuProbeUpFightPanel:FlushPanel()
    -- 打开界面直接需先设置bossid
    self.boss_id = self.data:GetSelBossId() or 0
    local boss_data = self.data:GetBossData(self.boss_id)
    local cfg = self.data.cfg.boss_cooperation[1]

    self.Score:SetScore(boss_data.total_cap or 0)

    local pet_num = boss_data.pet_num or 0
    local num_color = pet_num >= cfg.pet and COLORSTR.Green9 or COLORSTR.Red5
    local pet_num_str = ColorStr(string.format("(%s/%s)", pet_num, cfg.pet), num_color)
    UH.SetText(self.PetNums, string.format(Language.ShenYuProbe.UpFight, pet_num_str))

    local partner_num = boss_data.partner_num or 0
    num_color = partner_num >= cfg.partner and COLORSTR.Green9 or COLORSTR.Red5
    local partner_num_str = ColorStr(string.format("(%s/%s)", partner_num, cfg.partner), num_color)
    UH.SetText(self.PartnerNums, string.format(Language.ShenYuProbe.UpFight, partner_num_str))
    
    local is_has_buff = pet_num >= cfg.pet and partner_num >= cfg.partner
    local cur_buff_desc = is_has_buff and cfg.way_explain or Language.ShenYuProbe.Text3
    local cur_level = is_has_buff and 1 or 0
    local cur_pos = is_has_buff and Vector3.New(0, -103, 0) or Vector3.New(-157.1, -103, 0)
    UH.SetText(self.CurLevel, string.format(Language.ShenYuProbe.Text4, cur_level))
    UH.SetText(self.CurDesc, cur_buff_desc)
    self.CurLevel.gameObject:SetLocalPosition(cur_pos)

    self.NextLevel:SetObjActive(not is_has_buff)
    if not is_has_buff then
        UH.SetText(self.NextLevel, string.format(Language.ShenYuProbe.Text4, 1))
        UH.SetText(self.NextDesc, cfg.way_explain)
    end

    local cell_data_1 = self.data:GetBossCelHelpInfo(self.boss_id,1)
    local cell_data_2 = self.data:GetBossCelHelpInfo(self.boss_id,2)
    if TableIsEmpty(cell_data_1.item) then
        LogDG("神域探索boss信息异常", self.boss_id)
    end
    self.Cell1:SetData(cell_data_1)
    self.Cell2:SetData(cell_data_2)
end

function ShenYuProbeUpFightPanel:OnAutoUpClick()
    ShenYuProbeSelFight.AutoOnFight(self.boss_id)
end


ShenYuProbeUpFightGrid = ShenYuProbeUpFightGrid or DeclareMono("ShenYuProbeUpFightGrid", UIWidgetBaseItem)
function ShenYuProbeUpFightGrid:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.PetCell:SetObjActive(data.show_type == 1)
    self.PartnerCell:SetObjActive(data.show_type == 2)
    local name = Language.ShenYuProbe.Text5
    local item = data.item
    local score = ""
    if data.show_type == 1 then
        name = item:Name()
        score = string.format(Language.ShenYuProbe.Text6, item:GetScore())
        self.PetCell:SetData(item)
    elseif data.show_type == 2 then
        name = item.info.name
        score = string.format(Language.ShenYuProbe.Text6, item.vo.cap)
        self.PartnerCell:SetData(item)
    end
    UH.SetText(self.Name, name)
    UH.SetText(self.Score, score)
    local state = data.show_type ~= nil and ColorStr(Language.ShenYuProbe.Text7, COLORSTR.Green9) or ColorStr("待上阵", COLORSTR.Red7)
    UH.SetText(self.State, state)
end

function ShenYuProbeUpFightGrid:OnClick()
    ViewMgr:OpenView(ShenYuProbeSelFight, {boss_id = self.data.boss_id, cell_index = self.data.cell_index})
end




-- ========== 选择出战 =========
ShenYuProbeSelFight = ShenYuProbeSelFight or DeclareView("ShenYuProbeSelFight", "shen_yu_probe/shen_yu_probe_sel_fight")
function ShenYuProbeSelFight:ShenYuProbeSelFight()
    self.Board:SetData(self:GetType(),Language.ShenYuProbe.Title1,Vector2.New(975, 628))
    self.data = ShenYuProbeData.Instance
    self.sel_type = 1
    self.sel_data = nil
end

-- 1是宠物，2是伙伴
function ShenYuProbeSelFight:LoadCallback(param_t)
    self.cell_index = param_t.cell_index or 1
    self.boss_id = param_t.boss_id or 0
    self.btn_func = param_t.btn_func
    self.sel_type = param_t.sel_type or 1
    if param_t.not_sel_type then
        self.Tabs[param_t.not_sel_type]:SetObjActive(false)
    end
    self.Tabs[self.sel_type].isOn = true
    self:FlushPanel()
end

function ShenYuProbeSelFight:FlushPanel()
    self.List:SetData({})
    local list_data = self.sel_type == 1 and self.data:GetCanFightPetList() or self.data:GetCanFightPartnerList()
    self.List:SetData(list_data)
    --self.List:SetectItem(1,true)

    self.NoneObj:SetActive(TableIsEmpty(list_data))
    if TableIsEmpty(list_data) then
        local type_name = self.sel_type == 1 and Language.ShenYuProbe.Pet or Language.ShenYuProbe.Partner
        UH.SetText(self.NoneTip, string.format(Language.ShenYuProbe.Text8, type_name))
    end
end

function ShenYuProbeSelFight:OnSelClick(data, value)
    --LOG("ShenYuProbeSelFight:OnSelClick",data.show_type)
    if value then
        self.sel_data = data
    else
        self.sel_data = nil
    end
end

function ShenYuProbeSelFight:OnUpFightClick()
    if not self.sel_data then
        PublicPopupCtrl.Instance:Center(Language.ShenYuProbe.Tip1)
        return
    end
    if self.btn_func then
        self.btn_func(self.sel_data)
        return
    end
    ViewMgr:CloseView(ShenYuProbeSelFight)
    local item = self.sel_data.item
    if self.sel_type == 1 then
        self.PetOnFight(self.boss_id,self.cell_index, item)
    elseif self.sel_type == 2 then
        self.PartnerOnFight(self.boss_id,self.cell_index, item)
    end
end

function ShenYuProbeSelFight:OnTabClick(type)
    self.sel_type = type
    self.List:SelectAll(false)
    self:FlushPanel()
end

-- 一键上阵
function ShenYuProbeSelFight.AutoOnFight(boss_id)
    local pet_list = ShenYuProbeData.Instance:GetCanFightPetList()
    local partner_list = ShenYuProbeData.Instance:GetCanFightPartnerList()
    local pet_is_empty = TableIsEmpty(pet_list)
    local partner_is_empty = TableIsEmpty(partner_list)
    local t = {boss_id = boss_id}
    if pet_is_empty and not partner_is_empty then
        t.partner_id_1 = partner_list[1].item.id
        if partner_list[2] then
            t.partner_id_2 = partner_list[2].item.id
        end
    elseif partner_is_empty and not pet_is_empty then
        t.pet_index_1 = pet_list[1].item.index
        if pet_list[2] then
            t.pet_index_2 = pet_list[2].item.index
        end
    elseif not pet_is_empty and not partner_is_empty then
        t.pet_index_1 = pet_list[1].item.index
        t.partner_id_2 = partner_list[1].item.id
    else
        PublicPopupCtrl.Instance:Error(Language.ShenYuProbe.Tip2)
    end

    -- 保留原来的
    for i = 1, 2 do
        local info = ShenYuProbeData.Instance:GetBossCelHelpInfo(boss_id,i)
        if info.item ~= nil then
            if info.show_type == 1 then
                t["pet_index_" .. i] = info.item.index
                t["partner_id_" .. i] = nil
            else
                t["partner_id_" .. i] = info.item.id
                t["pet_index_" .. i] = nil
            end
        end
    end

    if not TableIsEmpty(t) then
        ShenYuProbeCtrl.Instance:SendHelpInfo(t)
    end
end

-- 上阵宠物
function ShenYuProbeSelFight.PetOnFight(boss_id,cell_index, pet)
    local t = ShenYuProbeData.Instance:GetBossHelperInfo(boss_id)
    t["pet_index_" .. cell_index] = pet.index
    ShenYuProbeCtrl.Instance:SendHelpInfo(t)
end

-- 上阵伙伴
function ShenYuProbeSelFight.PartnerOnFight(boss_id, cell_index, partner)
    local t = ShenYuProbeData.Instance:GetBossHelperInfo(boss_id)
    t["partner_id_" .. cell_index] = partner.id
    ShenYuProbeCtrl.Instance:SendHelpInfo(t)
end





ShenYuProbeSelFightItem = ShenYuProbeSelFightItem or DeclareMono("ShenYuProbeSelFightItem", UIWidgetBaseItem)
function ShenYuProbeSelFightItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data.item
    local show_type = data.show_type
    self.PetCell:SetObjActive(show_type == 1)
    self.PartnerCell:SetObjActive(show_type == 2)
    local element
    local name
    if show_type == 1 then
        self.PetCell:SetData(item)
        element = item:EleAttr()
        name = item:Name()
        local score_num, score_level = item:GetScore()
        UH.SetText(self.ScoreNum, score_num)
        UH.SpriteName(self.ScoreLevel, "scoreLevel"..tostring(score_level))
    elseif show_type == 2 then
        self.PartnerCell:SetData(item)
        element = item.vo.crystal or 0
        element = EleCrystal.GetElement(element)
        name = item.info.name
        UH.SetText(self.ScoreNum, item.vo.cap)
        UH.SpriteName(self.ScoreLevel, item.cap_sp)
    end

    UH.SetText(self.Name, name)
    if element then
        self.Element:SetData(element)
    end
end



ShenYuProbePartnerCell = ShenYuProbePartnerCell or DeclareMono("ShenYuProbePartnerCell", UIWidgetBaseItem)
-- data == partner
function ShenYuProbePartnerCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local vo = data.vo
    local prof_sp = RoleData.GetProfSp(info.job or 0)
    UH.SpriteName(self.Prof, prof_sp)
    UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
    UIWPetHeadCell.EffAnimName(vo.soar_count, self.FSQuaAnim)
    local str_level = vo.intensify_level == 0 and 1 or vo.intensify_level
    UH.SpriteName(self.StrLevel, PartnerInfo.StrLevelSp(str_level))
end