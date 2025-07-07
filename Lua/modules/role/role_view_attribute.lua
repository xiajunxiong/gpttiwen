RoleViewAttribute = RoleViewAttribute or DeclareMono("RoleViewAttribute", UIWFlushPanelGroup)
------ExpProg====	UIWProgressBar
------ProfTxt====	Text
------RaceTxt====	Text
------LvTxt======	Text
------TitleTxt===	Text
------FamilyTxt==	Text
------NameTxt====	Text
------IdTxt======	Text
------IdTxt======	Text
------EleAttri===	UIWElementAttribute
------ModelShow==   ModelUIShow
------BattlePosition=====Text
------BattlePositionTog== Toggle
function RoleViewAttribute:RoleViewAttribute()
    self.player_data = RoleData.Instance
    self.player_base_data = self.player_data:GetBaseData()
    local battle_attr = self.player_data:AllBattleAttri()
    self.data_cares = {
        {data = self.player_base_data, func = self.Exp, keys = {"level", "cur_exp"}},
        {data = self.player_base_data, func = self.FlushBattlePosition, keys = {"battle_position"}},
        {
            data = battle_attr,
            func = self.EleAttriFl,
            keys = {
                BattleAttriType.ElementWind,
                BattleAttriType.ElementEarth,
                BattleAttriType.ElementFire,
                BattleAttriType.ElementWater
            }
        },
        {data = self.player_base_data, func = self.BaseInfo},
        {data = self.player_base_data, func = self.FlushRoleShow, keys = {"appearance"}},
        {data = RoleAddPointData.Instance.red_add_piont,func = self.FlushAddPointRemind},
		{data = AttributeDanData.Instance.attribute_red_piont,func = self.FlushAttributeRemind},
		{data = IllusionData.Instance.red_points,func = self.FlushIllsionRemind},
    }
    UH.SpriteName(self.prof_img, RoleData.GetProfSp(self.player_base_data.profession))
    self.model_obj = nil
    self.btn_add_point = UiRedPoint.New(self.addPointPos, Vector3.New(55,-15,0))
    self.ExpProg:SetProgWidth(282)
    RichTextHelper.Bind(self.IdCopy, BindTool.Bind(self.OnClickIdCopy, self))

    GuideManager.Instance:RegisterGetGuideUi("RoleChangeCardMainBtn",function ()
        return self.ChangeCardBtn, function ()
            ViewMgr:OpenView(RoleChangeCardView)
        end
    end)
end
-- function RoleViewAttribute:FlushRedPoint()
--     self.btn_add_point:SetNum(RoleAddPointData.Instance:GetRedAddPiontNum())
-- end

function RoleViewAttribute:FlushRoleShow()
    local aperance = RoleData.Instance:GetApearance()
    local card_model_res_id = LiveData.Instance:CardModel(aperance)
    self.ModelShow.gameObject.transform.parent:SetObjActive(card_model_res_id == nil)
    self.NPCModelShow.gameObject.transform.parent:SetObjActive(card_model_res_id ~= nil)
    if card_model_res_id then
		self:SetNpcModel(card_model_res_id)
	else
		if not self.ui_obj then
			self.ui_obj = UIChDrawer.CopyMainRoleData()
			self.ModelShow:SetShowData(
				{
					ui_obj = self.ui_obj,
					view = self
				}
			)
		else 
			self.ui_obj:SetData(RoleData.Instance:GetApearance())
		end
	end
end

function RoleViewAttribute:SetNpcModel(card_model_res_id)
    if not self.npc_ui_obj then
        self.npc_ui_obj = UIObjDrawer.New()
        self.NPCModelShow:SetShowData({ui_obj = self.npc_ui_obj,view = self})
    end
    self.npc_ui_obj:SetData(DrawerHelper.GetNpcPath(card_model_res_id))
end

function RoleViewAttribute:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    if self.npc_ui_obj then
        self.npc_ui_obj:DeleteMe()
        self.npc_ui_obj = nil
    end
    UIWFlushPanelGroup.OnDestroy(self)
end

function RoleViewAttribute:Exp()
    local data = RoleData.Instance
    local base_data = data:GetBaseData()
    local next_exp = RoleData.GetNextLevelExp(base_data.level)
    local cur_exp = base_data.cur_exp
    local rate = next_exp ~= 0 and cur_exp / next_exp or 1
    self.ExpProg:SetProgress(rate)
    self.ExpProg:SetText(string.format(Language.Common.Progress2TxtColorBFormat, cur_exp, next_exp))
    local is_open_change = FunOpen.Instance:IsFunOpen(Mod.ChangeCard.Main)
    self.ChangeCardBtn:SetActive(false)
end

function RoleViewAttribute:EleAttriFl()
    local ele_data = {
        [BattleAttriType.ElementWind] = self.player_data:GetBattleAttri(BattleAttriType.ElementWind),
        [BattleAttriType.ElementEarth] = self.player_data:GetBattleAttri(BattleAttriType.ElementEarth),
        [BattleAttriType.ElementFire] = self.player_data:GetBattleAttri(BattleAttriType.ElementFire),
        [BattleAttriType.ElementWater] = self.player_data:GetBattleAttri(BattleAttriType.ElementWater)
    }
    self.EleAttri:SetData(ele_data)
end

function RoleViewAttribute:BaseInfo()
    local lang = Language.Role.Attri

    local name_show = Split(self.player_base_data.name,"_")
    self.NameTxt.text = name_show[1] -- 查看角色不显示服务器后缀
    self.IdTxt.text = string.format("ID:%d", self.player_base_data.role_id)
    local copyStr = RichTextHelper.LinkStr(Language.Role.CopyId, "_ul",COLORSTR.Green5)
    UH.SetText(self.IdCopy, copyStr)
    self.ProfTxt.text = string.format(lang.Prof, RolePromoteData.GetProfName(self.player_base_data.profession_id))
    self.RaceTxt.text = string.format(lang.Race, lang.RaceRenXing)
    if self.player_base_data.top_level > 0 then
        self.LvTxt.text = string.format(lang.Lv, string.format(Language.PeakRoad.Level2, self.player_base_data.top_level))
    else
        self.LvTxt.text = string.format(lang.Lv, self.player_base_data.level)
    end
    self.TitleTxt.text = string.format(lang.Title, self.player_base_data.title_id ~= 0 and TitleData.GetTitleName(self.player_base_data.title_id) or Language.Common.Wu)
    self.FamilyTxt.text = string.format(lang.Guild, StringIsEmpty(self.player_base_data.guild_name) and Language.Common.Wu or self.player_base_data.guild_name)
    UH.SetText(self.KillText, string.format(lang.KillValue, self.player_base_data.kill_value))

    local duel_cfg = DuelData.Instance:GetKillValueConfig(self.player_base_data.kill_value)
    if duel_cfg ~= nil and duel_cfg.attr_down > 0 then
        self.DuelBuff:SetActive(true)
        UH.SetText(self.DuelBuffValue, string.format(Language.Role.DuelBuffDesc, duel_cfg.attr_down))
    else
        self.DuelBuff:SetActive(false)
    end

    local is_level_limit = FlyUpData.Instance:IsLevelLimit()
    level_limit_str = is_level_limit and Language.Role.LevelLimit or ""
    UH.SetText(self.LevelLimitTip, level_limit_str)
end

function RoleViewAttribute:FlushBattlePosition()
    local is_front = self.player_base_data.battle_position ~= ROLE_BATTLE_POSITTION.BEHIND
    local battle_position_str = is_front and Language.Role.BattleFront or Language.Role.BattleBehind
    self.BattlePosition.text = battle_position_str
    self.BattlePositionTog.isOn = is_front
end

function RoleViewAttribute:OnClickFightPos()
    local position = self.player_base_data.battle_position == ROLE_BATTLE_POSITTION.BEHIND and ROLE_BATTLE_POSITTION.FRONT or ROLE_BATTLE_POSITTION.BEHIND
    RoleCtrl.Instance:SendRoleBattlePosition(position)
end

function RoleViewAttribute:OnClickChange()
    ViewMgr:OpenView(RoleChangeCardView)
end

function RoleViewAttribute:OnClickExchange()
    ViewMgr:OpenViewByKey(Mod.IllusionExchange.Main)
end

function RoleViewAttribute:OnClickAppearance()
    ViewMgr:OpenViewByKey(Mod.Appearance.Main)
end

function RoleViewAttribute:OnClickIllusion()
    ViewMgr:OpenViewByKey(Mod.Illusion.HuanHua)
end

function RoleViewAttribute:FlushIllsionRemind()
    self.IllusionRemind:SetNum(IllusionData.Instance:GetRemind())
end

function RoleViewAttribute:OnClickAttributeDan()
	AttributeDanData.Instance:SetAttributeDanType(1)
    ViewMgr:OpenView(AttributeDanView)
end

function RoleViewAttribute:OnClickIdCopy()
    UnityEngine.GUIUtility.systemCopyBuffer = self.player_base_data.role_id
    PublicPopupCtrl.Instance:Center(Language.Role.CopyIdSuccess)
end

--更换职业
function RoleViewAttribute:OnClickChangeProf()
    if MainViewData.Instance:IsBattle() then
        PublicPopupCtrl.Instance:Center(Language.Role.ChangeProfInBattle)
        return 
	end
    if ActivityData.Instance:GetActivityStatus(ActType.ChiefElection) ~= ActStatusType.Close then
        if SceneData.Instance:GetMode() == SceneModeType.ChiefPrimary or SceneData.Instance:GetMode() == SceneModeType.ChiefElelction then
            PublicPopupCtrl.Instance:Center(Language.ChiefElection.ErrorLog[7])
            return
        end
    end
    local npc_config = VoMgr:NpcVoByNpcId(40015)
    SceneLogic.Instance:AutoToPos(npc_config[1].scene_id,nil,function()SceneLogic.Instance:AutoToNpc(40015)end,nil,true)
    ViewMgr:CloseView(RoleView)
end

function RoleViewAttribute:AddPoint()
    ViewMgr:OpenView(RoleAddPointView)
end

function RoleViewAttribute:ResetRoleName()
    if RoleCtrl.NationalDayCheck() then return end
    -- if LoginData.Instance:IsOnCrossSever() then
    --     PublicPopupCtrl.Instance:Center(Language.Role.CannotResetNameInCross)
    --     return
    -- end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    if Item.GetNum(26056) > 0 then
        local reset_func = function(name)
            PublicPopupCtrl.Instance:AlertTip(
                Format(Language.Role.ResetNameConfirm, name),
                function()
                    RoleCtrl.Instance:SendRoleResetName(name)
                end
            )
        end
        PublicPopupCtrl.Instance:ResetName({reset_func = reset_func, tips = Language.ResetName.Tips2})
    else
        PublicPopupCtrl.Instance:AlertTip(Language.Role.PopupTips4,function()
            ViewMgr:CloseView(RoleView)
            MallCtrl.Instance:Jump(26056)
        end)
    end
end

function RoleViewAttribute:FlushAddPointRemind()
    self.btn_add_point:SetNum(RoleAddPointData.Instance:GetRedAddPiontNum())
end

function RoleViewAttribute:FlushAttributeRemind()
    if self.attribute_redpoint == nil then
        self.attribute_redpoint = UiRedPoint.New(self.attributedanPos, Vector3.New(23,23,0))
    end
    self.attribute_redpoint:SetNum(AttributeDanData.Instance:GetRedAttibutePiontNum())
end

function RoleViewAttribute:OnClickAttrExplain()
    ViewMgr:OpenView(AttrExplainView)
end

--世界等级说明
function RoleViewAttribute:OnClickWorldLevelDesc()
    ---检测是否跨服
    if LoginCtrl.Instance.data:BaseData().is_connected_cross then 
        self.world_level:SetActive(true)
    else
        PublicPopupCtrl.Instance:Center(Language.Role.WorldLevelTip)
    end   
end

function RoleViewAttribute:OnClickWorldLevelDescClose()
    self.world_level:SetActive(false)
end

function RoleViewAttribute:OnClickKillValue()
    DuelCtrl.Instance:OpenTips()
end

-----------------------------------------

RoleViewAttriDetail = RoleViewAttriDetail or DeclareMono("RoleViewAttriDetail", UIWFlushPanel)
---Hp=========UIWProgressBar
---Mp=========UIWProgressBar
---Atk========Text
---Def========Text
---Agility====Text
---Mental=====Text
---Recovery===Text
---Critcal====Text
---Hit========Text
---Dodge======Text
---CounterAtk=Text
---MagicAtk===Text
---MagicDef===Text
---CriticalDec===Text
---CriticalInc===Text
function RoleViewAttriDetail:RoleViewAttriDetail()
    local battle_attr = RoleData.Instance:AllBattleAttri()
    self.data_cares = {
        {data = battle_attr, func = self.BaseAttr},
        {data = RoleData.Instance.base_data, func = self.FlushScore, keys = {"capability"}, init_call = false}
    }
    self.Hp:SetProgWidth(202)
    self.Mp:SetProgWidth(202)
end

function RoleViewAttriDetail:BaseAttr()
    local data = RoleData.Instance

    local cur_hp = data:GetBattleAttri(BattleAttriType.CurHp)
    local max_hp = data:GetBattleAttri(BattleAttriType.MaxHp)
    local cur_mp = data:GetBattleAttri(BattleAttriType.CurMp)
    local max_mp = data:GetBattleAttri(BattleAttriType.MaxMp)

    local hp_rate = max_hp ~= 0 and cur_hp / max_hp or 1
    self.Hp:SetProgress(hp_rate)
    self.Hp:SetText(string.format(Language.Common.Progress2TxtColorBFormat, cur_hp, max_hp))

    local mp_rate = max_mp ~= 0 and cur_mp / max_mp or 1
    self.Mp:SetProgress(mp_rate)
    self.Mp:SetText(string.format(Language.Common.Progress2TxtColorBFormat, cur_mp, max_mp))

    self.Atk.text = tostring(data:GetBattleAttri(BattleAttriType.Attack))
    self.Def.text = tostring(data:GetBattleAttri(BattleAttriType.Defense))
    self.Agility.text = tostring(data:GetBattleAttri(BattleAttriType.Agility))
    self.Mental.text = tostring(data:GetBattleAttri(BattleAttriType.Mental))
    self.Recovery.text = tostring(data:GetBattleAttri(BattleAttriType.Recovery))

    self.Critcal.text = Percent(data:GetBattleAttri(BattleAttriType.Critical))
    self.Hit.text = Percent(data:GetBattleAttri(BattleAttriType.Hit))
    self.Dodge.text = Percent(data:GetBattleAttri(BattleAttriType.Dodge))
    self.CounterAtk.text = Percent(data:GetBattleAttri(BattleAttriType.CounterAttack))

    self.MagicAtk.text = tostring(data:GetBattleAttri(BattleAttriType.MagicAttack))
    self.MagicDef.text = tostring(data:GetBattleAttri(BattleAttriType.MagicDefense))

    self.CriticalDec.text = Percent(data:GetBattleAttri(BattleAttriType.CriticalDecValue))
    self.CriticalInc.text = Percent(data:GetBattleAttri(BattleAttriType.CriticalIncValue))

    self:FlushScore()
end
function RoleViewAttriDetail:FlushScore()
    local role_score,score_level = RoleData.Instance:GetRoleScore()
    self.Score:SetScore(role_score, score_level)
    -- UH.SetText(self.ScoreText, role_score)
    -- UH.SpriteName(self.ScoreLevelImg, "scoreLevel"..score_level)    
end
--传参(第几个, 属性类型)
function RoleViewAttriDetail:OnClickBaseAttr(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = -75, y = 84 + pos_y}
    self:OpenAttrReasonView(pos_t, attr_type)
end

function RoleViewAttriDetail:OnClickAmendAttr(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = 255, y = 84 + pos_y}
    self:OpenAttrReasonView(pos_t, attr_type)
end

function RoleViewAttriDetail:OpenAttrReasonView(pos_t, attr_type)
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_ROLE,
        param1 = 0,
        attr_type = attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end

-----------------------------------------

RoleViewAttriDetailSp = RoleViewAttriDetailSp or DeclareMono("RoleViewAttriDetailSp", UIWFlushPanel)
---Position====Text
---Petrify=====Text
---Sleep=======Text
---Chaos=======Text
---Drunk=======Text
---Chivalrous==Text
---GuildContri=Text
---Silence=Text
---SevereInjuer=Text

function RoleViewAttriDetailSp:RoleViewAttriDetailSp()
    local battle_attr = RoleData.Instance:AllBattleAttri()
    self.data_cares = {
        {data = battle_attr, func = self.AttriSp}
    }
end

function RoleViewAttriDetailSp:AttriSp()
    local data = RoleData.Instance
    self.Position.text = Percent(data:GetBattleAttri(BattleAttriType.AntiPosition))
    self.Petrify.text = Percent(data:GetBattleAttri(BattleAttriType.AntiPetrify))
    self.Sleep.text = Percent(data:GetBattleAttri(BattleAttriType.AntiSleep))
    self.Chaos.text = Percent(data:GetBattleAttri(BattleAttriType.AntiChaos))
    self.Drunk.text = Percent(data:GetBattleAttri(BattleAttriType.AntiDrunk))
    self.Silence.text = Percent(data:GetBattleAttri(BattleAttriType.AntiSilence))
    self.SevereInjuer.text = Percent(data:GetBattleAttri(BattleAttriType.AntiSevereInjuer))
    -- self.Chivalrous.text = data:GetCurrency(CurrencyType.Chivalrous)
    -- self.GuildContri.text = data:GetCurrency(CurrencyType.GuildCotribution)

    UH.SetText(self.PhysicsIncrease, Percent(data:GetBattleAttri(BattleAttriType.PhysicsIncrease)))
    UH.SetText(self.MagicIncrease, Percent(data:GetBattleAttri(BattleAttriType.MagicIncrease)))
    UH.SetText(self.PhysicsReduce, Percent(data:GetBattleAttri(BattleAttriType.PhysicsReduce)))
    UH.SetText(self.MagicReduce, Percent(data:GetBattleAttri(BattleAttriType.MagicReduce)))
    UH.SetText(self.XiaoGuoMingZhong, Percent(data:GetBattleAttri(BattleAttriType.XiaoGuoMingZhong)))
    UH.SetText(self.ZhenShiShangHai, data:GetBattleAttri(BattleAttriType.ZhenShiShangHai))
    UH.SetText(self.AntiEffect, Percent(data:GetBattleAttri(BattleAttriType.AntiEffect)))
    UH.SetText(self.FaBao, Percent(data:GetBattleAttri(BattleAttriType.FaBao)))
    UH.SetText(self.FaBaoShangHai, Percent(data:GetBattleAttri(BattleAttriType.FaBaoSahnghai)))
    UH.SetText(self.ZhiLiaoJiaCheng, Percent(data:GetBattleAttri(BattleAttriType.HealIncrease)))
    UH.SetText(self.GeDang, data:GetBattleAttri(BattleAttriType.GeDang))
end

function RoleViewAttriDetailSp:OnClickAttrBlock()
    self.AttrBlock:SetActive(false)
    ViewMgr:CloseView(AttrReasonView)
end

--传参(第几个, 属性类型)
function RoleViewAttriDetailSp:OnClickResistAttr(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = -75, y = 84 + pos_y}
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_ROLE,
        param1 = 0,
        attr_type = attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end

--传参(第几个, 属性类型)
function RoleViewAttriDetailSp:OnClickResistAttr2(obj, attr_type)
    local pos_y = obj.transform.localPosition.y
    local pos_t = {x = 260, y = 84 + pos_y}
    local param_t = {
        reason_type = AttrReasonConfig.ATTRLIST_REASON_TYPE.TYPE_ROLE,
        param1 = 0,
        attr_type = attr_type,
        position = pos_t,
    }
    PublicPopupCtrl.Instance:OpenAttrReason(param_t)
end

RoleViewAttriWorldLevelTip = RoleViewAttriWorldLevelTip or DeclareMono("RoleViewAttriWorldLevelTip", UIWFlushPanel)

function RoleViewAttriWorldLevelTip:RoleViewAttriWorldLevelTip()
    self.player_data = RoleData.Instance
end

function RoleViewAttriWorldLevelTip:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.Title, Language.Role.WorldLevelTab)
    self:FlushPanel()
end

function RoleViewAttriWorldLevelTip:FlushPanel()
    local next_index --下一等级index
    local day = self.player_data.world_level_data.open_day
    local content = ""
    local cur_world_Lv = self.player_data:GetWorldLevel()
    local next_day
    local next_level
    for i,v in pairs(Config.roleexp_auto.world_level) do
        if v.world_level > cur_world_Lv then
            next_day = v.day
            next_level = v.world_level
            break
        end
    end
    
    local cfg = Config.roleexp_auto.other[1]
    local day_after = cfg.world_level_open

    content = Format(Language.Role.WorldLevelAttrList[1],cur_world_Lv)

    local add_exp = self.player_data:GetWorldLevelAddExp()
    if add_exp > 100 and self.player_data:GetRoleLevel() >= cfg.world_level_effect and not FlyUpData.Instance:IsLevelLimit() then 
        content = content .. "\n" .. Format(Language.Role.WorldLevelAttrList[2], add_exp)
    end

    local ta_day = self:GetNextServerDate(day,next_day,self.player_data.world_level_data.today_zero_timestamp)
    if ta_day then
        content = content .. "\n" .. Format(Language.Role.WorldLevelAttrList[3],ta_day.year,ta_day.month,ta_day.day,next_level)
    end

    if FlyUpData.Instance:IsLevelLimit() then
        content = content .. "\n" .. Language.Role.WorldLevelAttrList[4]
    end

    UH.SetText(self.Content, content)

    UH.SetText(self.Desc, Format(Language.Role.WorldLevelDesc, DataHelper.GetDaXie(day_after), cfg.world_level_effect, cfg.world_level_overflow))
end

---计算时间
function  RoleViewAttriWorldLevelTip:GetNextServerDate(day,nextDay,curtime) --计算下一次开服时间
    if nextDay==nil then return end
    local nextTime=curtime+(nextDay-day)*24*3600
    local tb = {}
    tb.year = tonumber(os.date("%Y",nextTime))
    tb.month =tonumber(os.date("%m",nextTime))
    tb.day = tonumber(os.date("%d",nextTime))
    return tb
end