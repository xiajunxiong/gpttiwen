--宠物装备-护符重置
AmuletResetView = AmuletResetView or DeclareView("AmuletResetView","pet_amulet/AmuletResetView")

function AmuletResetView:AmuletResetView()
    self.Board:SetData(self:GetType(),Language.Pet.PetEquip.Reset, Vector2.New(782, 580))
    self.handle = BagData.Instance:Care(BindTool.Bind(self.FlushResetView, self))
    self.pet_handle = PetData.Instance:GetPetList():Care(BindTool.Bind(self.FlushResetView, self))
    self.is_set_pos=true

    self.flush_item_handle = nil
end

function AmuletResetView:LoadCallback(param_t)
    self:FlushResetView(param_t)
end

function AmuletResetView:FlushResetView(param_t)
    local function GetIndex(list,param)
        if param == nil then return end
        for i,v in pairs(list) do
            local is_index = (v.index ~= nil and param.index == v.index) or (v.param.pet_index ~=nil and param.pet_index == v.param.pet_index)
            if param.item_id == v.item_id and is_index then
                return i
            end
        end
    end
    local data_list = AmuletData.Instance:GetBagAndPetAmuletList()
    local select_data = AmuletData.Instance:GetSelectedResetData()
    param_t = param_t or {}
    local param = {
        item_id = param_t.item_id or select_data.item_id,
        index = param_t.index or select_data.index or -1,
        pet_index = (param_t.param and param_t.param.pet_index) or (select_data.param and select_data.param.pet_index) or -1
    }
    local index = GetIndex(data_list,param or select_data) or 1
    local count = #data_list or 0
    if not self.jump_flag then
        AmuletData.Instance:SetSelectedResetData(param_t) 
        self.List:SetData(data_list)
        if self.flush_item_handle ~= nil then
            TimeHelper:CancelTimer(self.flush_item_handle)
            self.flush_item_handle = nil
        end

        self.flush_item_handle = TimeHelper:AddDelayTimer(function ()
            self.flush_item_handle = nil
            local jump_index
            if index <= 1 then
                jump_index = 1
            elseif index > count - 2 then
                jump_index = count - 2
            else
                jump_index = index - 1
            end
            self.List:JumpVerticalIndex(jump_index,113)
        end,0.02)
        self.jump_flag = true
    else
        self.List:SetData(data_list)
        local select_data = self.List:GetByIndex(index)
        AmuletData.Instance:SetSelectedResetData(select_data)
    end
end

function AmuletResetView:CloseCallback()
   -- AmuletData.Instance:SetResetCtrlType({})
    BagData.Instance:UnCare(self.handle)
    PetData.Instance:GetPetList():Uncare(self.pet_handle)

    if self.flush_item_handle ~= nil then
		TimeHelper:CancelTimer(self.flush_item_handle)
		self.flush_item_handle = nil
	end
end

function AmuletResetView:OnClickResetItem(data)
    AmuletData.Instance:SetSelectedResetData(data)
    self:FlushResetView()
end

--====================Panel=================
AmuletResetPanel = AmuletResetPanel or DeclareMono("AmuletResetPanel",UIWFlushPanel)
function AmuletResetPanel:AmuletResetPanel()
    self.data = AmuletData.Instance
    self.data_cares = {
      {data = self.data.SelectedResetData, func = self.FlushView,init_call = false},
    }
    self.is_open_anima=false
end
function AmuletResetPanel:OnDestroy()
    self.data:SetSelectedResetData({})
    UIWFlushPanel.OnDestroy(self)
end

function AmuletResetPanel:FlushView()
    local data = self.data:GetSelectedResetData()
    if data == nil or next(data) == nil then return end
    local skill_list = {}
    local new_skill_list = {}
    for i = 1,data.param.equipment_skill_num or MAX_PET_EQUIPMENT_SKILL_NUM do
        local skill_id = data.param.equipment_skill_list[i]
        if skill_id > 0 then
            local cfg = AmuletData.Instance:GetGiftCfg(skill_id)
            skill_list[i] = cfg
        end
        local new_skill_id = data.param.equipment_skill_refresh_list[i]
        if new_skill_id > 0 then
            cfg = AmuletData.Instance:GetGiftCfg(new_skill_id)
            cfg.seq=self.data:GetSkillSeqByItemId(data.item_id, i)
            new_skill_list[i] = cfg
        end
    end
    self.LeftSkillList:SetData(skill_list)
    if self.is_open_anima then
        self.AnimaList:SetData(new_skill_list or {})
    else
        self.RightSkillList:SetData(new_skill_list or {})
    end
    self.stuff_cfg = self.data:GetResetStuffCfg(data.item_id)
    self.NeedCell:SetDataByItemIdWithNeed(self.stuff_cfg.item_id,self.stuff_cfg.number)

    local is_has_new_skill = false
    for i = 1,data.param.equipment_skill_num or MAX_PET_EQUIPMENT_SKILL_NUM do
        if data.param.equipment_skill_refresh_list[i] ~= 0 then
            is_has_new_skill = true
            break
        end
    end
    self.NewSkillObj:SetActive(is_has_new_skill and not self.is_open_anima)
    self.AnimaSkillObj:SetActive(is_has_new_skill and self.is_open_anima)
    self.TipText.gameObject:SetActive(not is_has_new_skill)
    UH.SetText(self.TipText,Language.Pet.PetEquip.Tip)
    self.is_open_anima=false
end


function AmuletResetPanel:OnResetClick()
    local data = self.data:GetSelectedResetData()
    local ctrl_t = PetEquipResetType.Bag
    if (data.param.pet_index or -1) >= 0 then
        ctrl_t = PetEquipResetType.Pet
    end
    local index = data.index
    local seq=self.data:GetResetSeq(data.item_id)
    if ctrl_t == PetEquipResetType.Pet then
        index = data.param.pet_index
    end
    --LogError(PET_EQUIP_REQ_TYPE.REQ_TYPE_REFRESH,ctrl_t,seq,index)
    AmuletCtrl.Instance:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_REFRESH,ctrl_t,seq,index)
    if Item.GetNum(self.stuff_cfg.item_id) >= self.stuff_cfg.number then
        self.is_open_anima=true
    end
end

function AmuletResetPanel:OnSaveClick()
    local data = self.data:GetSelectedResetData()
    local ctrl_t = PetEquipResetType.Bag
    if (data.param.pet_index or -1) >= 0 then
        ctrl_t = PetEquipResetType.Pet
    end
    local index = data.index
    if ctrl_t == PetEquipResetType.Pet then
        index = data.param.pet_index
    end
    if self.data:IsNeedTip(data) then
        PublicPopupCtrl.Instance:AlertTip(Language.Pet.PetEquip.AlertTip, function()
            AmuletCtrl.Instance:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_SAVE_REFRESH,ctrl_t,index)
        end)
    else
        AmuletCtrl.Instance:SendOperateReq(PET_EQUIP_REQ_TYPE.REQ_TYPE_SAVE_REFRESH,ctrl_t,index)
    end
    if MainViewData.Instance:IsBattle() then
        PublicPopupCtrl.Instance:Center(ErrorText[110])
    end
end


AmuletResetSkillItem = AmuletResetSkillItem or DeclareMono("AmuletResetSkillItem",UIWidgetBaseItem)
function AmuletResetSkillItem:SetData(data)
    UH.SetIcon(self.Icon,data.icon_id,ICON_TYPE.SKILL)
    UH.SpriteName(self.Qua, PetData.GiftListItemFrameName[data.page])
    UH.SetText(self.Name,data.name)
    if data.not_name then
        self.Name.gameObject:SetActive(false)
    end
    UIWidgetBaseItem.SetData(self,data)
end
function AmuletResetSkillItem:OnSkillClick()
    PetData.Instance:SetClickSkill({skill_cfg = self.data})
    ViewMgr:OpenView(PetSkillTip)
end

function AmuletResetSkillItem:ShowName()
    self.Name.gameObject:SetActive(true)
end


AmuletResetItem = AmuletResetItem or DeclareMono("AmuletResetItem",UIWidgetBaseItem)
function AmuletResetItem:SetData(data)
    self.Cell:SetData(data)
    UH.SetText(self.Name,Item.GetName(data.item_id))
    local skill_names = ""
    if data.param then
        for i = 1,data.param.equipment_skill_num or MAX_PET_EQUIPMENT_SKILL_NUM do
            local skill_id = data.param.equipment_skill_list[i]
            local level = AmuletData.Instance:GetIntensifySkillLevel(skill_id)
            if skill_id and skill_id > 0 then
                local cfg = AmuletData.Instance:GetGiftCfg(skill_id, level)
                local qua = cfg.page
                if level then
                    qua = 3
                end
                skill_names = skill_names..string.format(Language.PetEquipSelect.SkillNameColor2, Language.PetEquipSelect.SkillColorD[qua], cfg.name)
            end
        end
        if (data.param.pet_index or -1) >= 0 then
            self.UseObj:SetActive(true)
        else
            self.UseObj:SetActive(false)
        end
    end
    UH.SetText(self.SkillName,skill_names)
    local select_data = AmuletData.Instance:GetSelectedResetData()
    if (data.index ~= nil and data.index == select_data.index) or (data.param ~= nil and data.param.pet_index ~= nil and data.param.pet_index == select_data.param.pet_index) then
        UH.SpriteName(self.SelectImg,"XinXiDiBan4")
    else
        UH.SpriteName(self.SelectImg,"XinXiDiBan3")
    end
    UIWidgetBaseItem.SetData(self,data)
end

-- function AmuletResetItem:OnClickItem()
--     AmuletData.Instance:SetSelectedResetData(self.data)
-- end

function AmuletResetItem:OnDisable()
    self.Tran.localScale = Vector3.New(1,1,1)
    UIWidgetBaseItem.OnDisable(self)
end

AmuletResetAnimaItem = AmuletResetAnimaItem or DeclareMono("AmuletResetAnimaItem",UIWidgetBaseItem)
function AmuletResetAnimaItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.default_speed=0    --初始速度
    self.speed=0            --动画速度
    self.pos_y=0            --content的y位置
    self.stop=0             --转了第几圈开始启动停止
    self.pos_index=0        --目标位置下标
    self.con_length=0       --content的总长度
    self.stop_pos=0         --停止的位置
    self.target_item={}     --洗练到的技能
    if self.anim_handle == nil then
        self.anim_handle=Runner.Instance:RepeatCall(BindTool.Bind(self.OnAnim,self),0.02)
    end
    self:ResetAnim()
end

function AmuletResetAnimaItem:OnDestroy()
    Runner.Instance:CancelRepeat(self.anim_handle)
    UIWidgetBaseItem.OnDestroy(self)
end

function AmuletResetAnimaItem:ResetAnim()
    local gift_list,pos_index = AmuletData.Instance:GetGiftCfgList(self.data.seq,self.data.skill_id or self.data.param_1)
    self.pos_index=pos_index
    --self.List.StepCount = #gift_list
    self.con_length=(#gift_list-3) * 100  --100=(格子高度+spacing间隔)
    self.default_speed=self.con_length/50 --500=1/0.02
    self.speed=self.default_speed
    self.stop_pos=pos_index * 100 +35-180 --180=list的遮罩高度  35是偏移位置
    self.List:SetData(gift_list)
end
function AmuletResetAnimaItem:OnAnim()
    if self.speed >0 then
        self.pos_y=self.pos_y+self.speed
        if self.pos_y>=(self.con_length) then
            self.pos_y=0
            self.stop=self.stop+1
            self.target_item=self.List:GetItem(self.pos_index)
            self.target_item:ShowName()
        end
        if self.stop>0 then
            if self.pos_y>=self.stop_pos then
                self.pos_y=self.stop_pos
                self.speed=0

            end
            if self.speed > self.default_speed/5 then
                self.speed=self.speed-0.2
            end
        end
        self.Con:SetLocalPosition(Vector3.New(0,self.pos_y,0))
    end
end