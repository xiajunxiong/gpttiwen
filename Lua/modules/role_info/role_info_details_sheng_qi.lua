RoleInfoDetailsShengQi = RoleInfoDetailsShengQi or DeclareMono("RoleInfoDetailsShengQi",UIWFlushPanel)
function RoleInfoDetailsShengQi:RoleInfoDetailsShengQi()
    self.sheng_qi_list = RoleInfoCtrl.Instance:Data():GetRoleEquips().sheng_qi_list
    self.info_index = 1

    self.type_img = {
        [SHENG_QI_TYPE.ATTACK] = "_LocShengQiLeiXing_Gong",
        [SHENG_QI_TYPE.HELP] = "_LocShengQiLeiXing_Fu",
        [SHENG_QI_TYPE.SHOOT] = "_LocShengQiLeiXing_Luo",
    }

    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
			}
		)
	end
end

function RoleInfoDetailsShengQi:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushInfo()
end

function RoleInfoDetailsShengQi:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function RoleInfoDetailsShengQi:FlushInfo()
    local sheng_qi = self.sheng_qi_list[self.info_index]
    if not sheng_qi then
        return
    end
    self.LBtn:SetActive(self.info_index > 1)
    self.RBtn:SetActive(self.info_index < #self.sheng_qi_list)

    local sheng_qi_type = sheng_qi:ShengQiType()
    UH.SpriteName(self.TypeImg, self.type_img[sheng_qi_type])
    local level_str = ColorStr("Lv." .. sheng_qi:Level(), COLORSTR.Green9)
    local name = sheng_qi:Name() .. "  " .. level_str
    UH.SetText(self.Name, name)
    local score = sheng_qi:Score()
    local score_level = sheng_qi:ScoreLevel()
    self.Score:SetScore(score,score_level)
    local state = sheng_qi:State()
    self.FightFlag:SetActive(state == ShengQiCfg.State.Fight)
    self.WaitFlag:SetActive(state == ShengQiCfg.State.Ready)

    local active_attri,attri_list_data = sheng_qi:AttriShowList()
    self.ActiveAttri:SetData(active_attri)
    self.AttriList:SetData(attri_list_data or {})

    local talen_list = sheng_qi:GetTalentSkillList()
    local new_talen_list = {}
    for i,v in pairs(talen_list) do
        if v.skill_id > 0 then
            table.insert(new_talen_list, v)
        end
    end
    self.TalentList:SetData(new_talen_list)
    local init_list = sheng_qi:GetInitativeSkillList()
    local show_init_list = {}
    for i,v in pairs(init_list) do
        if v.skill_id and v.skill_id > 0 then
            table.insert(show_init_list, v)
        end
    end
    self.InitList:SetData(show_init_list)
    self.NothingTip:SetActive(TableIsEmpty(show_init_list))

    self.ui_obj:SetData(DrawerHelper.GetWeaponPath(sheng_qi:ResId()))
end

function RoleInfoDetailsShengQi:OnLeftClick()
    local index = self.info_index - 1
    if index < 1 then
        return
    end
    self.info_index = index
    self:FlushInfo()
end

function RoleInfoDetailsShengQi:OnRightClick()
    local index = self.info_index + 1
    if index > #self.sheng_qi_list then
        return
    end
    self.info_index = index
    self:FlushInfo()
end