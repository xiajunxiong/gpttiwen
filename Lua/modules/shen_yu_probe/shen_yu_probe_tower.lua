-- 神域锁妖塔
ShenYuProbeTower = ShenYuProbeTower or DeclareView("ShenYuProbeTower", "shen_yu_probe/shen_yu_probe_tower")
function ShenYuProbeTower:OnCloseClick()
    ViewMgr:CloseView(ShenYuProbeTower)
end

function ShenYuProbeTower:LoadCallback(param)
    local well_type = param.well_type or 0
    if well_type then
        -- 打开界面则请求信息
        ShenYuProbeCtrl.Instance:SendTemplateReq({
            oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_EXPLORE_WELL_INFO,
            param_1 = well_type,
        })
    end
end

ShenYuProbeTowerPanel = ShenYuProbeTowerPanel or DeclareMono("ShenYuProbeTowerPanel", UIWFlushPanel)
function ShenYuProbeTowerPanel:ShenYuProbeTowerPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.well_view_data, func = self.FlushPanel, init_call = false}
    }
end

function ShenYuProbeTowerPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShenYuProbeTowerPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.well_view_data:Set({})
end

function ShenYuProbeTowerPanel:FlushPanel()
    local info = self.data.well_view_data:Val()
    if TableIsEmpty(info) then
        self.Info:SetActive(false)
        return
    end
    self.Info:SetActive(true)
    self.info = info
    local list_data = self.data:GetWellTaskGroup(self.info.well_type)
    table.sort(list_data, function (a,b)
        local a_info = ShenYuProbeData.Instance:GetWellTaskInfo(a.seq - 1, a.slot_idx)
        local b_info = ShenYuProbeData.Instance:GetWellTaskInfo(b.seq - 1, b.slot_idx)
        local a_num = a.slot_idx
        local b_num = b.slot_idx
        if a_info.uid then
            a_num = a_num + (a_info.uid == RoleData.Instance:GetRoleId() and 10000 or 1000)
        end
        if b_info.uid then
            b_num = b_num + (b_info.uid == RoleData.Instance:GetRoleId() and 10000 or 1000)
        end
        return a_num < b_num
    end)
    self.List.AutoBottom = self.data.well_view_data.is_up
    self.List:SetData(list_data)

    UH.SetText(self.ProgressText, string.format("%s/%s", info.seal_count, info.need_count))
    local progress_num = info.seal_count / info.need_count
    self.ProgressImg.fillAmount = progress_num
    self.BoxGetFlag:SetActive(info.seal_count == info.need_count)
end

function ShenYuProbeTowerPanel:OnBoxClick()
    local cfg_well_type = self.info.well_type + 1
    local cfg = ShenYuProbeData.Instance.cfg.suoyao_well[cfg_well_type]
    local huobi_list = {}
    if cfg.military_exploits_2 > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceExploits]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.military_exploits_2}))
    end
    if cfg.quintessence_2 > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceEssence]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.quintessence_2}))
    end

    local item_list = DataHelper.FormatItemList(cfg.seal_reward)
    item_list = AndList(huobi_list, item_list)

    local tab = {
        item_list = item_list,
        title = Language.ShenYuProbe.Text9
    }
    ViewMgr:OpenView(ShenYuProbeBox, tab)
end





ShenYuProbeTowerItem = ShenYuProbeTowerItem or DeclareMono("ShenYuProbeTowerItem", UIWidgetBaseItem)
function ShenYuProbeTowerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local task_info = ShenYuProbeData.Instance:GetWellTaskInfo(data.seq - 1, data.slot_idx)
    self.task_info = task_info
    local is_up = ShenYuProbeData.Instance.well_view_data.is_up 
    local show_type = task_info.show_type    -- 为空说明没有上阵
    local player_name = task_info.player_name or ColorStr(Language.ShenYuProbe.Text10, COLORSTR.Red9)
    local well_type = task_info.well_type or 0
    self.PetCell:SetObjActive(show_type == 1)
    self.PartnerCell:SetObjActive(show_type == 2)
    self.AddCell:SetActive(show_type == nil and not is_up)
    self.HeadNone:SetActive(show_type == nil)
    local item_name = ""
    self.Lock:SetActive(is_up and show_type == nil)
    if show_type == 1 then
        local pet = Pet.New({
            id = task_info.id,
            skin_id = task_info.skin_id,
            fly_flag = task_info.fly_flag,
            str_level = task_info.str_level,
        })
        self.PetCell:SetData(pet)
        item_name = pet:Name()
    elseif show_type == 2 then
        local partner = PartnerData.Instance:GetPartnerInfoById(task_info.id)
        local info = TableCopy(partner.info)
        local vo = TableCopy(partner.vo)
        vo.skin_id = task_info.skin_id
        vo.intensify_level = task_info.str_level
        vo.soar_count = task_info.fly_flag
        local new_partner = {id = task_info.id}
        new_partner.info = info
        new_partner.vo = vo
        self.PartnerCell:SetData(new_partner)
        item_name = new_partner.info.name
    end

    local head_cell_data = show_type == nil and {} or {
        avatar_type = task_info.avatar_type,
        avatar_id = task_info.head_id,
        avatar_quality = 0,
        top_level = 0,
        uid = task_info.uid, 
    }
    self.HeadCell:SetData(head_cell_data)

    UH.SetText(self.ItemName, item_name)
    UH.SetText(self.PlayerName, player_name)

    local cfg = ShenYuProbeData.Instance.cfg.suoyao_well[well_type + 1]
    local huobi_list = {}
    if cfg.military_exploits_1 > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceExploits]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.military_exploits_1}))
    end
    if cfg.quintessence_1 > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceEssence]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.quintessence_1}))
    end
    local item_list = DataHelper.FormatItemList(cfg.battle_reward)
    item_list = AndList(huobi_list, item_list)
    self.RewardList:SetData(item_list)

    if show_type == nil and not is_up then
        local type_name = data.seal_task == 1 and Language.ShenYuProbe.Pet or Language.ShenYuProbe.Partner
        UH.SetText(self.AddTip1, string.format(Language.ShenYuProbe.Text21, type_name))
        UH.SetText(self.AddTip2, string.format(Language.ShenYuProbe.Text22, type_name))
    end
end

function ShenYuProbeTowerItem:OnAddClick()
    local btn_func = function (data)
        local well_type = self.data.seq - 1
        local slot_idx = self.data.slot_idx
        local p3 = data.show_type == 2 and data.item.id or 0
        local p4 = data.show_type == 1 and data.item.index or -1
        
        ShenYuProbeCtrl.Instance:SendTemplateReq({
            oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_EXPLORE_WELL_SEAL,
            param_1 = well_type,
            param_2 = slot_idx,
            param_3 = p3,
            param_4 = p4,
        })
        ViewMgr:CloseView(ShenYuProbeSelFight)
    end
    ViewMgr:OpenView(ShenYuProbeSelFight, {
        btn_func = btn_func,
        sel_type = self.data.seal_task,
        not_sel_type = self.data.seal_task == 1 and 2 or 1, 
    })
end



-- 神域宝箱弹框
ShenYuProbeBox = ShenYuProbeBox or DeclareView("ShenYuProbeBox","shen_yu_probe/shen_yu_probe_box")
function ShenYuProbeBox:LoadCallback(param_t)
    local title = param_t.title or ""
    local item_list = param_t.item_list or {}
    self.Board:SetData(self:GetType(), title, Vector2.New(524, 366))
    self.ItemList:SetData(item_list)
end

function ShenYuProbeBox:OnBtnClick()
    ViewMgr:CloseView(ShenYuProbeBox)
end