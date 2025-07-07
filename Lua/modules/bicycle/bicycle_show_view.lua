BicycleShowView = BicycleShowView or DeclareView("BicycleShowView","bicycle/bicycle_show_view")
function BicycleShowView:BicycleShowView()
    GuideManager.Instance:RegisterGetGuideUi("BicycleShowCloseBtn",function ()
        return self.GuideFrame1,function ()
            self:OnCloseClick()
        end
    end)
end

function BicycleShowView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.show_type == nil or  param_t.show_info == nil then
        return
    end
    if param_t.show_type ~= BicycleCfg.SHOW_TYPE.EQUIP then
        self["ShowMono" .. param_t.show_type]:SetObjActive(true)
        self["ShowMono" .. param_t.show_type]:SetData(param_t.show_info)
    end
    self.Animator:SetInteger("show_type",param_t.show_type)
    local show_type = param_t.show_type
    self.TitleBgObj:SetActive(param_t.show_type ~= BicycleCfg.SHOW_TYPE.EQUIP)
    if show_type == BicycleCfg.SHOW_TYPE.HALIDOM_ACTIVE or show_type == BicycleCfg.SHOW_TYPE.HALIDOM_UP then
        local haliom = param_t.show_info.bicycle:Halidom()
        UH.SetText(self.ModelName,haliom:Cfg().holy_name)
        param_t.show_shadow = false
    else
        UH.SetText(self.ModelName,param_t.show_info.bicycle:Name())
        param_t.show_shadow = true
    end
    TimeHelper:CancelTimer(self.time_handle)
    self:HandleModel(param_t)
    self.ShowEffectObj:SetActive(BicycleCfg.SHOW_TYPE_CARE[show_type] or show_type == BicycleCfg.SHOW_TYPE.BREAK_SUCC)
end

function BicycleShowView:HandleModel(param_t)
    local show_type = param_t.show_type
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow1:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = param_t.show_shadow,
			}
		)
    end
    if show_type == BicycleCfg.SHOW_TYPE.HALIDOM_ACTIVE or show_type == BicycleCfg.SHOW_TYPE.HALIDOM_UP then
        local haliom = param_t.show_info.bicycle:Halidom()
        local model_id = haliom:Cfg().holy_model
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(model_id))
    else
        local bicycle = param_t.show_info.bicycle
        local model_id = bicycle:ModelId()
        self.ui_obj:SetData(DrawerHelper.GetRidePath(model_id))
    end
    
    if param_t.show_type == BicycleCfg.SHOW_TYPE.EQUIP then
        if self.ui_obj_last == nil then
            self.ui_obj_last = UIObjDrawer.New()
            self.ModelShow2:SetShowData(
                {
                    ui_obj = self.ui_obj_last,
                    view = self
                }
            )
        end
        local last_bicycle = param_t.show_info.last_bicycle
        local last_model_id = last_bicycle:ModelId()
        self.ui_obj_last:SetData(DrawerHelper.GetRidePath(last_model_id))
    end
end

function BicycleShowView:CloseCallback()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
    if self.ui_obj_last ~= nil then
		self.ui_obj_last:DeleteMe()
	end
	self.ui_obj_last = nil
    BicycleData.Instance:BicycleShowType(BicycleCfg.SHOW_TYPE.Not)
end

function BicycleShowView:OnCloseClick()
    ViewMgr:CloseView(BicycleShowView)
end

BicycleShowActiveBicycle = BicycleShowActiveBicycle or DeclareMono("BicycleShowActiveBicycle",UIWidgetBaseItem)
-- @data:{bicycle}
function BicycleShowActiveBicycle:SetData(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PettEgg2)
    local bicycle = data.bicycle
    UH.SetText(self.LevelText,string.format(Language.Bicycle.Level,bicycle:Level()))
    local attri_data_list = BicycleData.Instance:ConvertAttriToData2(bicycle:Attri())
    table.insert(attri_data_list,1,{name = Language.Bicycle.NatureAttriName,value = ColorStr(bicycle:Nature(true),COLORSTR.Yellow1)})
    self.AttriList:SetData(attri_data_list)
    local skill_list = BicycleData.Instance:GetBicycleAllSkill(bicycle)
    self.SkillList:SetData(skill_list)
end

BicycleShowActiveSkill = BicycleShowActiveSkill or DeclareMono("BicycleShowActiveSkill",UIWidgetBaseItem)
function BicycleShowActiveSkill:SetData(data)
   
end


BicycleShowUpSkill = BicycleShowUpSkill or DeclareMono("BicycleShowUpSkill",UIWidgetBaseItem)
function BicycleShowUpSkill:SetData(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
    local skill = data.other_info
    self.SkillIcon:SetData(skill)
    UH.SetText(self.SkillLevel,string.format(Language.Bicycle.Level,skill:Level()))
    UH.SetText(self.CurSkillDesc,skill:Desc())
    local last_skill_effect_cfg = BicycleData.Instance:GetSkillEffect(skill.skill_id,skill:Level() - 1)
    UH.SetText(self.LastSkillDesc,last_skill_effect_cfg.description)
end

BicycleShowUpLevel = BicycleShowUpLevel or DeclareMono("BicycleShowUpLevel",UIWidgetBaseItem)
-- data = {last_bicycle,cur_bicycle}
function BicycleShowUpLevel:SetData(data)
    --AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    local last_bicycle = data.last_bicycle
    local cur_bicycle = data.bicycle
    UH.SetText(self.LastLevel,last_bicycle.level)
    UH.SetText(self.CurLevel,cur_bicycle:Level())
    local attri_list_3 = BicycleData.Instance:ConvertAttriToData3(last_bicycle.attri_t,cur_bicycle:Attri())
    self.AttriList:SetData(attri_list_3)
    -- local add_skill_point_value = cur_bicycle:SkillPoint() - last_bicycle.remain_skill_point
    -- UH.SetText(self.SkillPointDesc,string.format(Language.Bicycle.RemainSkillPoint,add_skill_point_value,cur_bicycle:SkillPoint()))

    -- local last_skill_list = BicycleData.Instance:GetBicycleAllSkill(last_bicycle)
    -- local cur_skill_list = BicycleData.Instance:GetBicycleAllSkill(cur_bicycle)
    -- local new_skill_list = self:GetNewSkillList(last_skill_list,cur_skill_list)
    -- self.SkillObj:SetActive(next(new_skill_list) ~= nil)
    -- if next(new_skill_list) ~= nil then
    --     self.SkillList:SetData(new_skill_list)
    -- end
end

function BicycleShowUpLevel:GetNewSkillList(last_skill_list,cur_skill_list)
    local list = {}
    for i,v in pairs(cur_skill_list) do
        local num = 0
        for j,v2 in pairs(last_skill_list) do
            if v.skill_id == v2.skill_id then
                num = num + 1
                break
            end
        end
        if num == 0 then
            table.insert(list,v)
        end
    end
    return list
end

BicycleShowBreak = BicycleShowBreak or DeclareMono("BicycleShowBreak",UIWidgetBaseItem)
function BicycleShowBreak:SetData(data)
    local last_bicycle = data.last_bicycle
    local cur_bicycle = data.bicycle
    local attri_list_3 = BicycleData.Instance:ConvertAttriToData3(last_bicycle.attri_t,cur_bicycle:Attri())
    self.AttriList:SetData(attri_list_3)
    --local add_skill_point_value = cur_bicycle:SkillPoint() - last_bicycle.remain_skill_point
end

BicycleShowSkillItem = BicycleShowSkillItem or DeclareMono("BicycleShowSkillItem",UIWidgetBaseItem)
function BicycleShowSkillItem:SetData(skill)
    UIWidgetBaseItem.SetData(self,skill)
    self.SkillIcon:SetData(skill)
    UH.SetText(self.SkillDesc,skill:Desc())
end


-- ============ 圣物提升 ==============
BicycleShowHalidomUp = BicycleShowHalidomUp or DeclareMono("BicycleShowHalidomUp",UIWidgetBaseItem)
function BicycleShowHalidomUp:SetData(data)
    local last_halidom = data.last_bicycle:Halidom()
    local cur_halidom = data.bicycle:Halidom()
    UH.SetText(self.LastLevel, last_halidom:Level())
    UH.SetText(self.CurLevel, cur_halidom:Level())
    local cfg1 = last_halidom:Cfg()
    local cfg2 = cur_halidom:Cfg()
    local attri_info_1 = CommonStruct.AttrStruct(cfg1.att_type, cfg1.att_value)
    local attri_info_2 = CommonStruct.AttrStruct(cfg2.att_type, cfg2.att_value)
    UH.SetText(self.AttriName, attri_info_1.attr_name .. ":")
    UH.SetText(self.AttriValue, attri_info_1.attr_percent_value)
    UH.SetText(self.AttriAddValue, attri_info_2.attr_percent_value)
end

-- ============ 圣物激活 ==============
BicycleShowHalidomActive = BicycleShowHalidomActive or DeclareMono("BicycleShowHalidomActive", UIWidgetBaseItem)
function BicycleShowHalidomActive:SetData(data)
    local last_halidom = data.last_bicycle:Halidom()
    local cur_halidom = data.bicycle:Halidom()
    local level = cur_halidom:Level()
    local cfg = cur_halidom:Cfg()
    if cur_halidom:IsShow() then
        UH.SetText(self.Level, level)
    else
        UH.SetText(self.Level, Language.Bicycle.Halidom.Tip1)
    end
    local attri_info = CommonStruct.AttrStruct(cfg.att_type, cfg.att_value)
    UH.SetText(self.AttriName, attri_info.attr_name .. ":")
    UH.SetText(self.AttriValue, attri_info.attr_percent_value)

    if data.last_bicycle.show_is_hlidom_canvert then
        UH.SpriteName(self.TitleImg, "_LocWenZiShengWuHuHuanChengGong")
    end
end

-- ============== 点化成功 ============
BicycleDianHuaSucc = BicycleDianHuaSucc or DeclareMono("BicycleDianHuaSucc", UIWidgetBaseItem)
function BicycleDianHuaSucc:SetData(data)
    local old_bicycle = data.last_bicycle
    local attri_list = old_bicycle:GetNextDianHuaAttriList()
    for i,v in pairs(attri_list) do
        --v.obj_name = "AttriItem" .. i
        if self.List[i] then
            self.List[i]:SetData(v)
            self.List[i]:SetObjActive(true)
        end
    end
    --self.AttriList:SetData(attri_list) -- 不知为啥，这种播不了动画
end


-- ============== 灵技置换 ==========
BicycleShowSkillConvert = BicycleShowSkillConvert or DeclareMono("BicycleShowSkillConvert", UIWidgetBaseItem)
function BicycleShowSkillConvert:SetData(data)
    local cur_bicycle = data.bicycle
    local old_bicycle = data.last_bicycle
    local protocol_info = BicycleData.Instance.protocol_info_2105
    if protocol_info.notice_type == BicycleCfg.NOTICE_TYPE.REPLACE_SKILL then
        local skill_index = protocol_info.param_1
        local skill_a = old_bicycle:GetBaseSkillByIndex(skill_index)
        local skill_b = cur_bicycle:GetBaseSkillByIndex(skill_index)
        if skill_a then
            self.SkillItemA:SetData(skill_a)
            UH.SetText(self.DescA, skill_a:Desc())
        end
        if skill_b then
            self.SkillItemB:SetData(skill_b)
            UH.SetText(self.DescB, skill_b:Desc())
        end
    end
end


-- ============== 灵技学习 ==========
BicycleShowSkillStudy = BicycleShowSkillStudy or DeclareMono("BicycleShowSkillStudy", UIWidgetBaseItem)
function BicycleShowSkillStudy:SetData(data)
    local cur_bicycle = data.bicycle
    local protocol_info = BicycleData.Instance.protocol_info_2105
    if protocol_info.notice_type == BicycleCfg.NOTICE_TYPE.LEARA_SKILL then
        local skill = cur_bicycle:GetBaseSkillByIndex(protocol_info.param_1)
        if skill then
            self.SkillItem:SetData(skill)
            UH.SetText(self.Desc, skill:Desc())
        end
    end
end