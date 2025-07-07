RoleInfoEquipView = RoleInfoEquipView or DeclareMono("RoleInfoEquipView", UIWFlushPanel)

function RoleInfoEquipView:RoleInfoEquipView()
    self.data = RoleInfoCtrl.Instance:Data()
    self.role_equips = self.data:GetRoleEquips()
    self.role_info = self.data:GetQueryRoleInfo(self.role_equips.role_id)
    self.RoleInfoToggle.isOn = true
end

function RoleInfoEquipView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:SetModelInfo()
    self:SetEquipInfo()
    self:SetMedalInfo()
    self:SetRoleInfo()
    self:SetRoleAttri()
end

function RoleInfoEquipView:SetModelInfo()
    local appearance = self.role_info.appearance
    if not self.ui_obj then
        self.ui_obj = UIChDrawer.CreateByAppearance(appearance)
        self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self})
        local re_data = Game.RendererEffectData()
        re_data.materialNames = {nil, "Ghost201"}
        re_data.animationClipName = "UI_jueseguang201"
        self.ui_obj:SetRenderEffect(re_data)
        self.Effect:SetActive(true)
    end
end

function RoleInfoEquipView:SetEquipInfo()
    local equipment_list = self.role_equips.equipment_list
    for i = 1, 5 do --八件套
        local equip = equipment_list[i - 1]
        if equip then
            self.EquipList[i]:SetData(equip)
            if equip.num ~= 0 then
                self.EquipTxtList[i]:SetActive(false)
            end
        else
            self.EquipList[i]:SetData(Item.New())
        end
    end
    local jewelry_list = self.role_equips.jewelry_list
    for i = 1, 3 do
        local jewelry = jewelry_list[i - 1]
        if jewelry then
            self.EquipList[i + 5]:SetData(jewelry)
            self.EquipTxtList[i + 5]:SetActive(false)
        else
            self.EquipList[i + 5]:SetData(Item.New())
        end
    end
end

function RoleInfoEquipView:SetMedalInfo()
    local medal_list = self.role_equips.medal_list
    local medal
    local index
    for i = 1, 3 do
        medal = medal_list[i]
        if medal then
            index = medal.index + 1
            local cell = self.EquipList[index + 8]
            if cell ~= nil then 
                cell.gameObject:SetActive(true)
                cell:SetData(medal)
            end 
            self.EquipTxtList[index + 8]:SetActive(false)
        end
    end
end

function RoleInfoEquipView:SetRoleInfo()
    local role_info = self.role_info
    local appearance = role_info.appearance
    appearance.avatar_level = role_info.level
    appearance.top_level = role_info.top_level

    UH.SetText(self.RoleName,role_info.name)
    if role_info.is_finish_top == nil or role_info.is_finish_top == 0 then
        self.GradingObj:SetActive(false)
        self.NoHjObj:SetActive(true)
    else
        local battlefield_info = ActivityData.GetConfig(ActType.Battlefield)
        self.GradingObj:SetActive(role_info.level >= battlefield_info.level_restrict)
        self.NoHjObj:SetActive(role_info.level < battlefield_info.level_restrict)
        if role_info.level >= battlefield_info.level_restrict then
            local GradingCfg = BattlefieldData.Instance:GetCurRankConfig(role_info.battlefield_score)
            UH.SetText(self.GradingText,GradingCfg.ranking_list)
            UH.SpriteName(self.GradingIcon,"Rank" .. GradingCfg.rank)
        end
    end
    local score_lv = RoleData.Instance:GetScoreLevel(role_info.capability,RoleData.ProfIdToProfType(role_info.prof),role_info.level)
    UH.SetText(self.ScoreText, role_info.capability)
    UH.SpriteName(self.ScoreLevelImg, Format("scoreLevel%d", score_lv))
    UH.SetText(self.ProfText,RolePromoteData.GetProfName(role_info.prof))

    local title_config = Config.face_cfg_auto.resource[self.role_equips.face_equip_list[3]]
    if title_config then
        self.TitleText:SetData(title_config)
    else
        title_config = RolePromoteData.GetProfConfig(role_info.prof)
        if title_config then
            self.TitleText:SetData({title_id = title_config.get_title})
        end
    end
    self.TitleText:SetObjActive(title_config ~= nil)

    UH.SetText(self.FamilyText,StringIsEmpty(role_info.guild_name) == true and Language.RoleInfo.None or role_info.guild_name)
    UH.SetText(self.CompRankText,self.role_equips.comp_rank < 0 and Language.RoleInfo.NoRank or Format(Language.RoleInfo.Rank,self.role_equips.comp_rank + 1))
    UH.SetText(self.ProfRankText,self.role_equips.prof_rank < 0 and Language.RoleInfo.NoRank or Format(Language.RoleInfo.Rank,self.role_equips.prof_rank + 1))
    self.Head:SetData(appearance)
    UH.SetText(self.KillText, Format(Language.RoleInfo.KillValue, role_info.kill_value))

    local marry_info = role_info.marry_info or {}
    if marry_info.role_id ~= 0 then
        UH.SetText(self.MarryText,marry_info.role_name)
    else
        UH.SetText(self.MarryText,Language.Common.Wu)
    end
    local swon_info = role_info.swon_info or {}
    if swon_info.member_num ~= 0 then
        UH.SetText(self.SwomText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(swon_info.sworn_name,COLORSTR.Red2), "_ul",COLORSTR.Red2))
    else
        UH.SetText(self.SwomText,Language.Common.Wu)
    end
    local achieve_info = role_info.achieve_info or {}
    UH.SetText(self.AchieveText,Format(Language.ItemInfo.RoleInfoDesc.AchieveText,achieve_info.level,achieve_info.num))
    UH.SetText(self.AppearanceText,Format(Language.Common.Levels,role_info.face_score_level))
end

function RoleInfoEquipView:SetRoleAttri()
    local attribute_list = self.role_info.attribute_list
    for i=1,2 do
        self.Attris[i]:SetProgWidth(250)
        self.Attris[i]:SetProgress(1)
        self.Attris[i]:SetText(Format(Language.Common.Progress2TxtColorBFormat,attribute_list[i-1],attribute_list[i-1]))
    end
    for i=3,self.Attris:Length() do
        self.Attris[i]:SetData({name = Language.RoleInfo.AttriNames[i - 1],value = attribute_list[i - 1]})
    end
    self.CompareBtn:SetActive(self.role_info.role_id ~= RoleData.Instance:GetRoleId())
end

function RoleInfoEquipView:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    UIWFlushPanel.OnDestroy(self)
end

function RoleInfoEquipView:OnClickKillValue()
    DuelCtrl.Instance:OpenTips()
end

function RoleInfoEquipView:OpenInfoAccrssory()
    ViewMgr:OpenView(RoleInfoAccrssoryView)
end

function RoleInfoEquipView:OnClickSwon()
    local swon_info = self.role_info.swon_info or {}
    if not IsEmpty(swon_info.member_num) then
        ViewMgr:OpenView(RoleInfoSwornDescView,{
            pos = Vector3.New(272.5,7.5),
            desc = SwornData.GetSwornRoleNameDesc(swon_info.role_info),
            title_name = swon_info.sworn_name
        })
    end
end

function RoleInfoEquipView:OnAttriCompareClick()
    ViewMgr:OpenView(RoleInfoAttriCompareView, {look_attri_list = self.role_info.attribute_list, name = self.role_info.name})
end


-- 玩家基础属性对比
RoleInfoAttriCompareView = RoleInfoAttriCompareView or DeclareView("RoleInfoAttriCompareView", "role_info/role_info_attri_compare_view")
function RoleInfoAttriCompareView:RoleInfoAttriCompareView()
    self.Board:SetData(self:GetType(),Language.RoleInfo.AttriCompareTitle, Vector2.New(872, 616))
end

function RoleInfoAttriCompareView:LoadCallback(param_t)
    local look_attri_list = param_t.look_attri_list
    if TableIsEmpty(look_attri_list) then
        return
    end
    UH.SetText(self.RName, param_t.name or "")
    local self_attri_list = {}
    self_attri_list[0] = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxHp)
    self_attri_list[1] = RoleData.Instance:GetBattleAttri(BattleAttriType.MaxMp)
    self_attri_list[2] = RoleData.Instance:GetBattleAttri(BattleAttriType.Attack)
    self_attri_list[3] = RoleData.Instance:GetBattleAttri(BattleAttriType.Defense)
    self_attri_list[4] = RoleData.Instance:GetBattleAttri(BattleAttriType.Agility)
    self_attri_list[5] = RoleData.Instance:GetBattleAttri(BattleAttriType.Mental)
    self_attri_list[6] = RoleData.Instance:GetBattleAttri(BattleAttriType.Recovery)

    self.LAttriInfo:SetAttri(self_attri_list,look_attri_list)
    self.RAttriInfo:SetAttri(look_attri_list)
end

RoleInfoAttriCompareInfo = RoleInfoAttriCompareInfo or DeclareMono("RoleInfoAttriCompareInfo")
function RoleInfoAttriCompareInfo:SetAttri(attri_list, t_attri_list)
    for i=1,2 do
        self.Attris[i]:SetProgWidth(250)
        self.Attris[i]:SetProgress(1)
        self.Attris[i]:SetText(Format(Language.Common.ProgressAttriCompare,attri_list[i-1],attri_list[i-1]))
    end
    for i=3,self.Attris:Length() do
        local data = {name = Language.RoleInfo.AttriNames[i - 1],value = attri_list[i - 1]}
        if t_attri_list then
            data.is_down = data.value < t_attri_list[i - 1]
        end
        self.Attris[i]:SetData(data)
    end
end


RoleInfoAttriCompareAttriItem = RoleInfoAttriCompareAttriItem or DeclareMono("RoleInfoAttriCompareAttriItem", UIWAttrItem)

function RoleInfoAttriCompareAttriItem:SetData(data)
	UIWAttrItem.SetData(self, data)
	self.Name.text = data.name
    self.Value.text = data.value
    if self.DownFlag then
        self.DownFlag:SetActive(data.is_down)
    end

	self:HideZero(data.value == 0)
end