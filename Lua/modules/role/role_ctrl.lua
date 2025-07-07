FILE_REQ("modules/role/role_config")
FILE_REQ("modules/role/role_data")
FILE_REQ("modules/role/role_other_data")
FILE_REQ("modules/role/title_data")
FILE_REQ("modules/role/skill_data")
FILE_REQ("modules/role/prestige_data")
FILE_REQ("modules/role/role_skill_data")
FILE_REQ("modules/role/role_promote_data")
FILE_REQ("modules/role/role_add_point_data")

VIEW_REQ("modules/role/role_view")
VIEW_REQ("modules/role/role_add_point_view")
VIEW_REQ("modules/role/role_promote_view")
VIEW_REQ("modules/role/role_profession")
VIEW_REQ("modules/role/role_select_profession")
VIEW_REQ("modules/role/role_profession_item")
VIEW_REQ("modules/role/role_change_profession_view")
VIEW_REQ("modules/role/role_promote_succe_view")
VIEW_REQ("modules/role/role_promote_help")
VIEW_REQ("modules/role/role_title_popup_view")
VIEW_REQ("modules/role/role_skill_sort")
VIEW_REQ("modules/role/role_skill_ding")
VIEW_REQ("modules/role/role_view_attribute")
VIEW_REQ("modules/role/role_skill_view")
VIEW_REQ("modules/role/role_prestige_view")
VIEW_REQ("modules/role/role_auto_add_point_view")

VIEW_REQ("modules/role/offline_reward")
VIEW_REQ("modules/role/reset_name_view")
VIEW_REQ("modules/role/potential_point_view")
VIEW_REQ("modules/role/new_skill_popup_view")
VIEW_REQ("modules/role/role_peifang_popup_view")
VIEW_REQ("modules/role/role_change_card_view")
VIEW_REQ("modules/role/role_skill_info_view")

RoleCtrl = RoleCtrl or BaseClass(BaseCtrl)

function RoleCtrl:__init()
    if RoleCtrl.Instance ~= nil then
        Debuger.LogError("[RoleCtrl] attempt to create singleton twice!")
        return
    end
    RoleCtrl.Instance = self

    self.data = RoleData.New()
    self.title_data = TitleData.New()
    self.skill_data = SkillData.New()
    self.promote_data = RolePromoteData.New()
    self.prestige_data = PrestigeData.New()
    self.role_skill_data = RoleSkillData.New()
    self.role_add_point_data = RoleAddPointData.New()

    -- 是否拦截头衔激活窗口，涉及转盘抽奖类的要延迟自己做展示
    self.is_shield_title_show = false
    -- 是否有拦截到头衔激活窗口的信息
    self.is_have_shield = false

    self:RegisterProtocol(SCRoleInfoAck, "RecvRoleFullInfo", true)
    self:RegisterProtocol(SCRoleAttrList, "RecvAttribute", true)
    self:RegisterProtocol(SCMoney, "RecvCurrency", true)
    self:RegisterProtocol(SCRoleExpChange, "RecvExp")
    self:RegisterProtocol(SCRoleLevelChange, "RecvLevel", true)

    self:RegisterProtocol(SCPrestigeAllVillageInfo, "SCPrestigeAllVillageInfo")
    self:RegisterProtocol(SCPrestigeAllShopInfo, "SCPrestigeAllShopInfo")
    self:RegisterProtocol(SCPrestigeSingleVillageInfo, "SCPrestigeSingleVillageInfo")
    self:RegisterProtocol(SCPrestigeSingleShopInfo, "SCPrestigeSingleShopInfo")
    self:RegisterProtocol(CSPrestigeShopingReq)
    self:RegisterProtocol(SCPrestigeDonateInfo, "SCPrestigeDonateInfo")
    self:RegisterProtocol(CSPrestigeDonateOp)

    self:RegisterProtocol(SCShopAllInfo, "SCShopAllInfo")
    self:RegisterProtocol(SCShopSingleInfo, "OnSCShopSingleInfo")
    self:RegisterProtocol(CSShopBuyReq)
    self:RegisterProtocol(CSFastPurchase)

    self:RegisterProtocol(SCTitleOtherInfo, "OnSCTitleOtherInfo")
    self:RegisterProtocol(SCTitleNotice, "OnTitleNotice")

    self:RegisterProtocol(SCTrainAllInfo, "OnSCTrainAllInfo")
    self:RegisterProtocol(SCTrainInfoPromoted, "OnSCTrainInfoPromoted")
    self:RegisterProtocol(CSTrainReq)
    self:RegisterProtocol(CSRolePetTrainSkillReq)
    self:RegisterProtocol(SCTrainRedDotFlagInfo, "OnSCTrainRedDotFlagInfo")

    self:RegisterProtocol(SCSkillList, "OnSCSkillList")
    self:RegisterProtocol(SCCommonUseSkillChangeNotice, "OnSCCommonUseSkillChangeNotice")
    self:RegisterProtocol(SCSkillChangeNotice, "OnSCSkillChangeNotice")
    self:RegisterProtocol(SCLifeSkillInfo, "OnSCLifeSkillInfo")
    self:RegisterProtocol(SCSkillExpDeltaChangeNotify, "OnSCSkillExpDeltaChangeNotify")
    self:RegisterProtocol(CSSkillOprateReq)
    self:RegisterProtocol(CSSkillPositionModify)

    self:RegisterProtocol(SCRoleAttributePlanInfo, "RoleAttributePlanInfo")
    self:RegisterProtocol(CSAttributePlanCreate)
    self:RegisterProtocol(CSAttributePlanChangeName)
    self:RegisterProtocol(CSAttributePlanSwitchPlan)
    self:RegisterProtocol(CSAttributePlanAddPoint)
    self:RegisterProtocol(CSAttributePlanClearPoint)

    self:RegisterProtocol(SCRoleProfessionChangeNotice, "OnRoleProfessionChange")
    self:RegisterProtocol(CSRoleCareerAdvanceReq)
    self:RegisterProtocol(CSRoleCareerChangeReq)

    self:RegisterProtocol(SCProfessionExtraInfo, "RecvProfessionExtraInfo")
    self:RegisterProtocol(CSProfessionExtraInfoReq)

    self:RegisterProtocol(CSSetBattlePosition)
    self:RegisterProtocol(SCSetBattlePosition, "OnBattlePosition")

    self:RegisterProtocol(CSRoleResetName)
    self:RegisterProtocol(SCRoleResetName, "OnRoleResetName")

    self:RegisterProtocol(SCBuffInfo, "OnSCBuffInfo")

    self:RegisterProtocol(CSFuncGuideOperator)
    self:RegisterProtocol(SCFuncGuideFlagInfo, "OnSCFuncGuideFlagInfo")

    self:RegisterProtocol(SCPrestigeChangeNotice, "OnPrestigeChangeNotice")

    self:RegisterProtocol(CSRealmBreakReq)
    self:RegisterProtocol(SCRealmBreakAllInfo, "OnRealmBreakAllInfo")
    self:RegisterProtocol(SCRealmBreakItemInfo, "OnRealmBreakItemInfo")
    self:RegisterProtocol(CSSetAutoAddPointFlag)

    self:RegisterProtocol(SCWorldLevelInfo,"OnSCWorldLevelInfo")

    self:RegisterProtocol(CSAdvanceCareerFightReq)
    self:RegisterProtocol(SCAdvanceCareerSubInfo,"OnAdvanceCareerSubInfo")

    self:RegisterProtocol(CSAuditBehaviourSubmitReq)

    self:RegisterProtocol(SCRoleAuthorityInfo,"OnSCRoleAuthorityInfo")

    self:RegisterProtocol(SCRoleHistoryMaxCapabilityChange, "OnSCRoleHistoryMaxCapabilityChange")
    --给服务器发送小游戏ID，收到fullInfo之后发送
    self:RegisterProtocol(CSMiniGameSetMiniGameId)

    self:RegisterProtocol(SCRoleInfoEtc, "OnSCRoleInfoEtc")
end

function RoleCtrl:OnInit()
    --[[ MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.RoleAttribute,AttributeDanData.Instance.attribute_red_piont,
        function()
            return AttributeDanData.Instance:GetRedAttibutePiontNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Role.RoleAttr)
        end
    )	
	
    MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.RoleAttrImprove,RoleAddPointData.Instance.red_add_piont,
        function()
            return RoleAddPointData.Instance:GetRedAddPiontNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Role.RoleAttr)
        end
    )
    MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.NewSkillUnlocked,self.skill_data.change_data,
        function()
            return self.skill_data:CacuRemindCount() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Role.RoleSkill)
        end
    )
    MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.Promote,self.data.promote_red_piont,
        function()
            return RolePromoteData.Instance:GetPromoteNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function()
            self.promote_data:SendTaskDemand()
            ViewMgr:OpenView(RolePromoteView)
        end
    ) ]]
    local role_pomote_remind_func = BindTool.Bind(self.RolePomoteRemind, self)
	RemindCtrl.Instance:Remind():Register(Mod.RolePromote.Main, self.data.promote_red_piont, role_pomote_remind_func)
	self.data.currencys:KeysCare({CurrencyType.CoinBind}, function()
        self.data.promote_red_piont:Notify()
    end)
    --[[ MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.RoleXiuLian, RoleSkillData.Instance.smart_data,
        function ()
            return RoleSkillData.Instance:GetXiulianSkillRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function ()
            ViewMgr:OpenViewByKey(Mod.Role.RoleSkill)
        end, "xiulian_remind")
    MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.RoleAssist, SkillData.Instance.change_data,
        function ()
            return RoleSkillData.Instance:GetAssistSkillRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function ()
            ViewMgr:OpenViewByKey(Mod.Role.RoleSkill)
        end, "change")
    MainViewData.Instance:RegisterUpgradeRemind(Language.Main.UpgradeRemind.SkillUpgrade, SkillData.Instance.change_data,
        function ()
            return SkillData.Instance:SkillCanUpgradeRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function ()
            ViewMgr:OpenViewByKey(Mod.Role.RoleSkill)
        end, "change") ]]
    self.data.currencys:KeysCare({CurrencyType.CoinBind},function ()
        self.data:ChangeSikllRedPiont()
    end)
    self.data.base_data:KeysCare({"level"},function ()
        self.data:ChangeSikllRedPiont()
    end)
end
function RoleCtrl:__delete()
    self:UnRegisterProtocol(SCRoleInfoAck)
    self:UnRegisterProtocol(SCRoleInfoAck)
    self:UnRegisterProtocol(SCRoleAttrList)
    self:UnRegisterProtocol(SCMoney)
    self:UnRegisterProtocol(SCRoleExpChange)
    self:UnRegisterProtocol(SCRoleLevelChange)
    self:UnRegisterProtocol(SCPrestigeAllVillageInfo)
    self:UnRegisterProtocol(SCPrestigeAllShopInfo)
    self:UnRegisterProtocol(SCPrestigeSingleVillageInfo)
    self:UnRegisterProtocol(SCPrestigeSingleShopInfo)
    self:UnRegisterProtocol(CSPrestigeShopingReq)
    self:UnRegisterProtocol(SCPrestigeDonateInfo)
    self:UnRegisterProtocol(CSPrestigeDonateOp)
    self:UnRegisterProtocol(SCShopAllInfo)
    self:UnRegisterProtocol(SCShopSingleInfo)
    self:UnRegisterProtocol(CSShopBuyReq)
    self:UnRegisterProtocol(CSFastPurchase)
    self:UnRegisterProtocol(SCTitleOtherInfo)
    self:UnRegisterProtocol(SCTitleNotice)
    self:UnRegisterProtocol(SCTrainAllInfo)
    self:UnRegisterProtocol(CSTrainReq)
    self:UnRegisterProtocol(CSRolePetTrainSkillReq)
    self:UnRegisterProtocol(SCTrainRedDotFlagInfo)
    self:UnRegisterProtocol(SCSkillList)
    self:UnRegisterProtocol(SCCommonUseSkillChangeNotice)
    self:UnRegisterProtocol(SCSkillChangeNotice)
    self:UnRegisterProtocol(SCLifeSkillInfo)
    self:UnRegisterProtocol(SCSkillExpDeltaChangeNotify)
    self:UnRegisterProtocol(CSSkillOprateReq)
    self:UnRegisterProtocol(CSSkillPositionModify)
    self:UnRegisterProtocol(SCRoleAttributePlanInfo)
    self:UnRegisterProtocol(CSAttributePlanCreate)
    self:UnRegisterProtocol(CSAttributePlanChangeName)
    self:UnRegisterProtocol(CSAttributePlanSwitchPlan)
    self:UnRegisterProtocol(CSAttributePlanAddPoint)
    self:UnRegisterProtocol(CSAttributePlanClearPoint)
    self:UnRegisterProtocol(SCRoleProfessionChangeNotice)
    self:UnRegisterProtocol(CSRoleCareerAdvanceReq)
    self:UnRegisterProtocol(CSRoleCareerChangeReq)
    self:UnRegisterProtocol(SCProfessionExtraInfo)
    self:UnRegisterProtocol(CSProfessionExtraInfoReq)
    self:UnRegisterProtocol(CSSetBattlePosition)
    self:UnRegisterProtocol(SCSetBattlePosition)
    self:UnRegisterProtocol(CSRoleResetName)
    self:UnRegisterProtocol(SCRoleResetName)
    self:UnRegisterProtocol(SCBuffInfo)
    self:UnRegisterProtocol(CSFuncGuideOperator)
    self:UnRegisterProtocol(SCFuncGuideFlagInfo)
    self:UnRegisterProtocol(SCPrestigeChangeNotice)
    self:UnRegisterProtocol(CSRealmBreakReq)
    self:UnRegisterProtocol(SCRealmBreakAllInfo)
    self:UnRegisterProtocol(CSSetAutoAddPointFlag)
    self:UnRegisterProtocol(CSAdvanceCareerFightReq)
    self:UnRegisterProtocol(SCRoleAuthorityInfo)
    RoleCtrl.Instance = nil
end

function RoleCtrl:OnUnloadGameLogic()
    self.data:ClearData()
    self.IsRoleNoticeComplete = nil
    self.prestige_data:ClearData()
    self.level_up_wait_cap = false
end

function RoleCtrl:SendAttributePlanCreate(plan_name)
    local protocol = GetProtocol(CSAttributePlanCreate)
    protocol.plan_name = plan_name
    SendProtocol(protocol)
end

function RoleCtrl:SendAttributePlanChangeName(plan_index, plan_name)
    local protocol = GetProtocol(CSAttributePlanChangeName)
    protocol.plan_index = plan_index
    protocol.plan_name = plan_name
    SendProtocol(protocol)
end

function RoleCtrl:SendAttributePlanSwitchPlan(switch_to_index)
    local protocol = GetProtocol(CSAttributePlanSwitchPlan)
    protocol.switch_to_index = switch_to_index
    SendProtocol(protocol)
end

function RoleCtrl:SendAttributePlanAddPoint(plan_index, add_point_list)
    local protocol = GetProtocol(CSAttributePlanAddPoint)
    protocol.plan_index = plan_index
    protocol.add_point_list = add_point_list
    SendProtocol(protocol)
end

function RoleCtrl:SendAttributePlanClearPoint(plan_index)
    local protocol = GetProtocol(CSAttributePlanClearPoint)
    protocol.plan_index = plan_index
    SendProtocol(protocol)
end

-- 职业晋升协议 101 ~ 102 color===[0,4)
function RoleCtrl:SendRoleCareerAdvance(profession_id, color)
    local protocol = GetProtocol(CSRoleCareerAdvanceReq)
    protocol.profession_id = profession_id
    protocol.to_color = color or 0
    SendProtocol(protocol)
end

-- 职业转职 101 ~ 201
function RoleCtrl:SendRoleCareerChangeReq(param)
    local protocol = GetProtocol(CSRoleCareerChangeReq)
    protocol.to_profession = param.to_profession
    protocol.avartar_type = param.avartar_type
    protocol.color = param.color
    SendProtocol(protocol)
end

--接收服务端转换职业的次数
function RoleCtrl:RecvProfessionExtraInfo(protocol)
    self.data:SetProfessionExtraInfo(protocol)
end

--发送请求
function RoleCtrl:SendRequestChangeProfTime()
    local protocol = GetProtocol(CSProfessionExtraInfoReq)
    SendProtocol(protocol)
end

function RoleCtrl:SendRoleBattlePosition(position)
    local protocol = GetProtocol(CSSetBattlePosition)
    protocol.position = position
    SendProtocol(protocol)
end

function RoleCtrl:SendRoleResetName(new_name)
    local protocol = GetProtocol(CSRoleResetName)
    protocol.new_name = new_name
    SendProtocol(protocol)
end

function RoleCtrl:SendSkillReq(operate_type, param1, param2, param3)
    local protocol = GetProtocol(CSSkillOprateReq)
    protocol.operate_type = operate_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.param3 = param3 or 0
    SendProtocol(protocol)
end

function RoleCtrl:SendSkillPositionModify()
    local protocol = GetProtocol(CSSkillPositionModify)
    protocol.skill_num = self.skill_data.skill_num
    protocol.skill_idx_list = self.skill_data.skill_idx_list
    SendProtocol(protocol)
end

--req_type : 0、修炼技能 |1、琉璃鼎     |2、修炼提升信息下发  |3、设置红点提示信息  |4、宠物修炼技能|5、伙伴修炼技能
--param1   ：train_type |合成物品的seq|                   |train_type         |train_type   |train_type
--param2   ：修炼的次数  |                                                    |修炼次数      |修炼次数
function RoleCtrl:SendCSTrainReq(req_type, param1, param2)
    local protocol = GetProtocol(CSTrainReq)
    protocol.req_type = req_type or 1
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function RoleCtrl.sendTrainPromoted()
    RoleCtrl.Instance:SendCSTrainReq(2)
end

function RoleCtrl:sendCSRolePetTrainSkillReq(train_num)
    local protocol = GetProtocol(CSRolePetTrainSkillReq)
    protocol.train_num = train_num
    SendProtocol(protocol)
end

function RoleCtrl:OnSCTrainRedDotFlagInfo(protocol)
    self.role_skill_data:SetTrainSkillReddotFlag(protocol)
    self.skill_data.change_data:SetDirty("red_dot_flag_change")
end

-- 境界请求操作，0是请求突破，1是请求吐纳，2是请求领取经验，3是请求挑战心灵之境 param=seq
function RoleCtrl:SendOfflineBreak(notice_type, param)
    local protocol = GetProtocol(CSRealmBreakReq)
    protocol.notice_type = notice_type or 0
    protocol.param = param or 0
    SendProtocol(protocol)
end

function RoleCtrl:SendOfflineBreakFight(seq)
    self:SendOfflineBreak(3, seq)
end

-- 境界信息下发
function RoleCtrl:OnRealmBreakAllInfo(protocol)
    self.data:SetBreakInfo(protocol)
    FightData.Instance:SetFightSoulInfo(protocol)
end

function RoleCtrl:OnRealmBreakItemInfo(protocol)
    self.data:SetBreakCumulationItem(protocol)
end

function RoleCtrl:CSPrestigeShopingReq(index, num)
    local protocol = GetProtocol(CSPrestigeShopingReq)
    protocol.index = index
    protocol.num = num
    SendProtocol(protocol)
end

function RoleCtrl:CSPrestigeDonateOp(village_id, column, index)
    local protocol = GetProtocol(CSPrestigeDonateOp)
    protocol.village_id = village_id or 0
    protocol.column = column or 0
    protocol.index = index or 0
    SendProtocol(protocol)
end

function RoleCtrl:CSShopBuyReq(index, num,is_rand_list)
    local protocol = GetProtocol(CSShopBuyReq)
    protocol.index = index
    protocol.num = num
    protocol.is_rand_list = is_rand_list or 0
    SendProtocol(protocol)
end

function RoleCtrl.FastPurchase(item_id, num)
    local protocol = GetProtocol(CSFastPurchase)
    protocol.item_id = item_id
    protocol.buy_num = num or 1
    SendProtocol(protocol)
end

function RoleCtrl:SendFuncGuideOperator(op_type, param)
    local protocol = GetProtocol(CSFuncGuideOperator)
    protocol.op_type = op_type
    protocol.param = param or -1
    SendProtocol(protocol)
end

function RoleCtrl:SendAdvanceCareerFightReq()
    local protocol = GetProtocol(CSAdvanceCareerFightReq)
    SendProtocol(protocol)
end


function RoleCtrl:SendMiniGameId()
    local protocol = GetProtocol(CSMiniGameSetMiniGameId)
    protocol.mini_game_id = LoginData.Instance:GetMiniGame()
    SendProtocol(protocol)
end

function RoleCtrl:RecvRoleFullInfo(protocol)
    self.data:Set(RecvRoleFullKey,protocol)
    LoginData.Instance:SetServerData(protocol)
    if MainPlayer then
        MainPlayer:SetMoveSpeed(protocol.move_speed)
    end
    MainProber:SetRolelevel(protocol.level)
    MainProber.role_authority = protocol.m_authority_type
    if self.IsRoleNoticeComplete == nil then
        EventSys:Fire(GlobalEventKeys.RoleNoticeComplete)
        self.IsRoleNoticeComplete = true
    end
    EventSys:Fire(GlobalEventKeys.RoleNoticeChange)
    --开始游戏上报（角色登录时会下发本协议，转换职业和形象时也会下发）
    local dev_mod = string.gsub(tostring(UnityEngine.SystemInfo.deviceModel), " ", "_")
    local uuid = GLOBAL_CONFIG:PkgInfo().device_id
	if VersionTag.HasTag(VersionTag.FileInfoJson) then
		local temp = MainProber.GetUUID()
		if not IS_IOS() then
			uuid = temp
		else
			if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
                uuid = "uu_" .. uuid
			else
				uuid = temp
			end
		end
	end
    MainProber:Step(MainProber.STEP_GAME_START, MainProber.user_id,MainProber.server_id, 
        MainProber.role_name, MainProber.role_id, protocol.level, PLATFORM, dev_mod, uuid)
    MainProber:SendRoleReport()
    QQBigPlayerCtrl.Instance:NoticeQQPlayerInfoChange()
    if self.role_login_trigger then
        self:TriggerRoleLogin()
    end
    self:SendMiniGameId()
    
    QQBigPlayerCtrl.Instance:SendGetWeekInfo()

end

function RoleCtrl:TriggerRoleLogin()
    if self.role_login_trigger and self.role_currency_get then
        RoleCtrl.Instance:PlatReport()
        ChannelAgent.BehaviourTrigger(BehaviourType.RoleLogin)
        self.role_login_trigger = false
        self.role_currency_get = false
		--新增一个角色上报
		    --看后续SDK调用支付如果需要就打开
			  		--SDK角色上报  本身没有这个上报 新加一个参数 防止后续使用   暂时关闭 
			 --需要参数 区服ID  角色ID  角色名字 角色等级  
			 
		local dada = tostring(LoginData.Instance:GetLastLoginServer() .. "_"..RoleData.Instance:GetRoleId().."_"..RoleData.Instance:GetRoleName().."_".. RoleData.Instance:GetRoleLevel())	
        PlatChannelAgent.FacebookActive(dada)
		
		
    end
end

function RoleCtrl:OnRoleProfessionChange(protocol)
    if self.data:GetProfession() == protocol.position then
        RoleSettingData.Instance:SetHeadTipsFlag(ChangeHeadType.Advancement,self.data:ProfStage(),true)
    end
    self.data:Set({[[profession]],[[profession_id]]},protocol)
    EventSys:Fire(GlobalEventKeys.RoleProfessionChange)
end

function RoleCtrl:RecvAttribute(protocol)
    local old_list = self.data:AllBattleAttri()
    if MedalData.Instance:IsMarkAttrView() then
        local change_list = DataHelper.GetChangeList(old_list,protocol.attr_list)
        local title_info = {fromType = AttrChangeData.ChangeFromType.Role, showName = self.data:GetRoleName()}
        MedalData.Instance:CatchAttrInfo(change_list, title_info)
    elseif next(old_list) ~= nil then
        if 0 == protocol.notify_reason or 2 == protocol.notify_reason then  --reason为2是法宝抽奖提升战力，需要延迟显示
            local change_list = DataHelper.GetChangeList(old_list,protocol.attr_list)
            local title_info = {fromType = AttrChangeData.ChangeFromType.Role,showName = RoleData.Instance:GetRoleName()}
            if 0 == protocol.notify_reason then
                MainOtherCtrl.Instance:OpenAttrChange(change_list,title_info)
            else
                FabaoLotteryData.Instance:SetDelayAttriChange(change_list,title_info)
            end
        end
    end
    -- reason为2是法宝抽奖提升战力，需要延迟显示
    if MedalData.Instance:IsMarkAttrView() or 2 == protocol.notify_reason then
        self.data:Set({[[capability]]},protocol)
    else
        self.data:Set({[[capability]]},protocol)
        self.data:ChangeRoleTotalScore()
    end
    self.data:SetRoleAttrList(protocol.attr_list)
    TeamData.Instance:NeedFlushMenu()
    self.data:ChangePromoteRedPiont()
    self:ReportCapbilityChange()
end

function RoleCtrl:RecvCurrency(protocol)
    -- LogError("RecvCurrency",protocol)
    for k,v in pairs(protocol.money_list) do
        self.data:SetCurrency(v.msg_money_type,v.money_num, protocol.is_no_show == 1)
    end
    self.data:ChangeSikllRedPiont()
    self.role_currency_get = true
    self:TriggerRoleLogin()
end

function RoleCtrl:RecvExp(protocol)
    local addition = self.data:GetWorldLevelAddExp() or 100
    addition = addition / 100
    if protocol.change_exp > 0 then
        if protocol.change_reason == ADD_EXP_REASON.TYPE_LEVEL_COMPLEMENT or protocol.change_reason == ADD_EXP_REASON.NEW_LEVEL_COMPLEMENT then
            self:ExpChangeTip(Language.PublicPopup.LevelComplementExpTip, protocol.change_exp, CommonItemId.LevelComplementExp, false, protocol.change_reason, true)
        elseif protocol.change_reason == ADD_EXP_REASON.OFFLINE_RECEIVE then
            self:ExpChangeTip(Language.PublicPopup.BreakContent, protocol.change_exp)
        elseif protocol.change_reason == ADD_EXP_REASON.OFFLINE_BREAK or protocol.change_reason == ADD_EXP_REASON.ACHIEVEMENT_NUM then
            self:SaveNeedDelayExp(protocol.change_exp)
        else
            self:ExpChangeTip(Language.PublicPopup.ExpTip, protocol.change_exp, nil, nil, protocol.change_reason)
        end
    end
    self.data:Set({[[cur_exp]]},protocol)
end

-- is_num_base 是否为基数，为true则会乘上加成
-- not_addition 是否不包含世界等级加成 true不包含
function RoleCtrl:ExpChangeTip(str, num, exp_item_id, is_num_base, change_reason, not_addition)
    str = str or Language.PublicPopup.ExpTip
    num = num or 1
    exp_item_id = exp_item_id or CommonItemId.Exp
    is_num_base = is_num_base or false
    local addition = self.data:GetWorldLevelAddExp() or 100
    local icon_id = CommonItemId.Exp
    addition = addition / 100
    if is_num_base then
        num = math.floor(num * addition)
    end
    local num_str = ColorStr(tostring(num), QualityColorStr[Item.GetColor(exp_item_id)])
    --有些不要飘字只要聊天栏显示
    if change_reason and ADD_EXP_REASON.REALM_BREAK_FIGHT == change_reason then
    else
        if addition > 1 and not_addition ~= true then
            local add_str = ColorStr(tostring(addition), COLORSTR.Yellow1)
            local desc = string.format(Language.RoleExpTip.AddTip2,str,num_str,add_str)
            PublicPopupCtrl.CenterRich(desc,{sprite = icon_id, w = 40, h = 40}, {sprite = "LvJianTou", w = 36, h = 42})
        else
            local desc = string.format("%s%s |",str,num_str)
            PublicPopupCtrl.CenterRich(desc,{sprite = icon_id, w = 40, h = 40})
        end
    end
    local item = Item.Init(exp_item_id, num)
    local add_desc = ""
    if change_reason == ADD_EXP_REASON.KILL_NORMAL_MONSTER then
        if not TeamData.Instance:InTeam() then
            add_desc = Language.RoleExpTip.TeamTips[1]
        else
            add_desc = Language.RoleExpTip.TeamTips[TeamData.Instance:MemberNumRole()]
        end
        add_desc = ColorStr(add_desc, COLORSTR.Yellow16)
    end
    if addition > 1 and not_addition ~= true then
        local num = ColorStr(tostring(addition), COLORSTR.Green8) 
        local world_level_add = string.format(Language.RoleExpTip.AddTip,num)
        add_desc = add_desc .. ColorStr(world_level_add, COLORSTR.Yellow16)
    end
    if change_reason and ADD_EXP_REASON.NEW_LEVEL_COMPLEMENT == change_reason then
        item.qua_name = RichTextHelper.ColorStr(item:Name(), COLORSTR.Red12)
    end
    ChatCtrl.Instance:ChannelChatCustom{role_exp = {item = item, num = num}, addition = add_desc}
end

-- 保存需要延时播放的经验
function RoleCtrl:SaveNeedDelayExp(change_exp)
    if not self.need_delay_exp then
        self.need_delay_exp = {}
    end
    table.insert(self.need_delay_exp, {change_exp = change_exp})
end

-- 释放需要延时播放的经验
function RoleCtrl:ReleaseDelayExp()
    if not self.need_delay_exp then
        return
    end
    for k, v in pairs(self.need_delay_exp) do
        self:ExpChangeTip(Language.PublicPopup.ExpTip, v.change_exp)
    end
    self.need_delay_exp = {}
end

function RoleCtrl:RecvLevel(protocol)
    if protocol.change_type == ROLE_LEVEL_CHANGE_TYPE.NORMAL then
        local old_level = self.data:GetRoleLevel()
        if old_level < protocol.level then
            MainOtherCtrl.Instance:OnRoleLevelChange()
            self.data:ChangePromoteRedPiont()
        end
        self.data:Set({[[level]],[[cur_exp]]},protocol)
        MainProber:SetRolelevel(protocol.level)
        RoleCtrl.Instance:PlatReport()
        if GLOBAL_CONFIG:Flags().capability_change_report or not IS_IOS() then
            self.level_up_wait_cap = true
        else
            ChannelAgent.BehaviourTrigger(BehaviourType.LevelUp)
        end
        EventSys:Fire(GlobalEventKeys.RoleLevelChange)
        local uuid = GLOBAL_CONFIG:PkgInfo().device_id
        if VersionTag.HasTag(VersionTag.FileInfoJson) then
            local temp = MainProber.GetUUID()
            if not IS_IOS() then
                uuid = temp
            else
                if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
                else
                    uuid = temp
                end
            end
        end
        MainProber:Step(MainProber.STEP_HEARTBEAT, MainProber.user_id,
        MainProber.server_id, MainProber.role_name, MainProber.role_id, protocol.level, uuid)
    elseif protocol.change_type == ROLE_LEVEL_CHANGE_TYPE.TOP then
        local old_level = self.data:GetTopLevel()
        if old_level and old_level < protocol.level then
            ViewMgr:OpenView(PeakLevelShow)
        end
        self.data:Set({[[cur_exp]]},protocol)
        self.data:Set({[[top_level]]},{top_level = protocol.level})
        PeakRoadData.Instance:PeakData().level = protocol.level
    end
end

function RoleCtrl:OnBattlePosition(protocol)
    self.data:Set({[[battle_position]]},protocol)
end

function RoleCtrl:OnSCTitleOtherInfo(protocol)
    -- LogDG("称号变化测试", protocol)
    self.data:Set({[[title_id]]},protocol)
end

function RoleCtrl:OnRoleResetName(protocol)
    if protocol.obj_id == SceneData.Instance:GetRoleObjId() then
        self.data:Set({[[name]],[[role_name]]},protocol)
    end
    MainProber:SetRoleName(protocol.new_name)
    ChannelAgent.BehaviourTrigger(BehaviourType.ResetRoleName)
end

function RoleCtrl:OnTitleNotice(protocol)
    LogDG("称号信息改变", protocol)
    if DuelData.Instance:IsDuelTitle(protocol.title_id) then
        return
    end
    local data = TitleData.FormatPopup(protocol.notice, protocol.title_id)
    TitleData.Instance:AddPopupList(data)
    
    if not ViewMgr:IsOpen(RoleTitlePopupView) then
        if not self.is_shield_title_show then
            ViewMgr:OpenView(RoleTitlePopupView)
        else
            self.is_have_shield = true
        end
    end
end

-- 将拦截的激活头衔窗口的消息放出来
function RoleCtrl:ReleaseShieldTitle()
    if self.is_have_shield then
        ViewMgr:OpenView(RoleTitlePopupView)
    end
    self.is_shield_title_show = false
    self.is_have_shield = false
end

function RoleCtrl:OnSCBuffInfo(protocol)
    MainViewData.Instance:SetBuffInfo(protocol)
end

function RoleCtrl:SCPrestigeAllVillageInfo(protocol)
    self.prestige_data:SetAllVillagePrestige(protocol.ret_data)
end

function RoleCtrl:SCPrestigeAllShopInfo(protocol)
    self.prestige_data:SetAllShopInfo(protocol)
end

function RoleCtrl:SCPrestigeSingleVillageInfo(protocol)
    local old_data = self.prestige_data:GetVillagePrestigeData(protocol.index) or {}
    self.prestige_data:SetSingleVillagePrestige(protocol.index, protocol.data)
    if PrestigeData.IsPopupParam(old_data,protocol.data) then
        local param = {village_id = protocol.index,old_data = old_data,new_data = protocol.data}
        self.prestige_data:SetPopupParam(param)
        if ViewMgr:IsOpen(PrestigePopupView) then
            ViewMgr:FlushView(PrestigePopupView)
        else
            ViewMgr:OpenView(PrestigePopupView)
        end
    end
end

function RoleCtrl:SCPrestigeSingleShopInfo(protocol)
    self.prestige_data:SetShopInfo(protocol)
end

function RoleCtrl:SCShopAllInfo(protocol)
    self.prestige_data:SetAllCommonShopInfo(protocol)
end

function RoleCtrl:OnSCShopSingleInfo(protocol)
    self.prestige_data:SetCommonShopInfo(protocol)
end

function RoleCtrl:SCPrestigeDonateInfo(protocol)
    self.prestige_data:SetDonateInfo(protocol)
end

function RoleCtrl:RoleAttributePlanInfo(protocol)
    RoleAddPointData.Instance:SetRolePlanInfo(protocol)
end

function RoleCtrl:OnSCSkillList(protocol)
    self.skill_data:OnSCSkillList(protocol)
    self.data:ChangeSikllRedPiont()
end

function RoleCtrl:OnSCSkillChangeNotice(protocol)
    self.skill_data:OnSCSkillChangeNotice(protocol)
    self.data:ChangeSikllRedPiont()
end

function RoleCtrl:OnSCLifeSkillInfo(protocol)
    self.skill_data:SetLifeSkillList(protocol.life_skill_list)
    self.data:ChangeSikllRedPiont()
end

function RoleCtrl:OnSCSkillExpDeltaChangeNotify(protocol)
    PublicPopupCtrl.Instance:Center(Format(Language.Role.GetExp,
    self.skill_data:GetSkillDescName(protocol.skill_id),protocol.exp_change))
    self.data:ChangeSikllRedPiont()
end

function RoleCtrl:OnSCCommonUseSkillChangeNotice(protocol)
    self.skill_data:OnSCCommonUseSkillChangeNotice(protocol)
    self.data:ChangeSikllRedPiont()
end

function RoleCtrl:OnSCTrainAllInfo(protocol)
    self.role_skill_data:SetAllInfo(protocol)
end

function RoleCtrl:OnSCTrainInfoPromoted(protocol)
    self.role_skill_data:SetTrainPromotedInfo(protocol)
end

function RoleCtrl:OnChangeProfession(appearance)
    local role_appearance = self.data:GetApearance()
    local cache_appearance = NeoCharaChangeData.Instance:GetCacheAppearance()
    if appearance.prof_base == role_appearance.prof_base then
        if appearance.advance_time > role_appearance.advance_time then  --进阶提示
            ViewMgr:OpenView(RolePromoteSucceView,
            {last_appearance = TableCopy(role_appearance),cur_appearance = TableCopy(appearance)})
        elseif cache_appearance ~= nil then       
            if cache_appearance.avartar_type ~= role_appearance.avartar_type or 
                cache_appearance.color ~= role_appearance.color then --在转职界面只换色提示
                NeoCharaChangeData.Instance:ColorChangeData().color_index = role_appearance.color
                ViewMgr:OpenView(NeoColorChangeSuccView)
            end
        end
    else
        ViewMgr:OpenView(RoleChangeProfView)
    end

    -- 检查缓存信息，若比对后发现颜色不一致，则展示换色界面
    -- 备注：角色的appearance信息本身会被协议1400自动更新，不能作为换色依据
    -- if not ViewMgr:IsOpen(RolePromoteSucceView) and not ViewMgr:IsOpen(RoleChangeProfView) 
    --     and cache_appearance ~= nil and cache_appearance.color ~=role_appearance.color then 
    --     NeoCharaChangeData.Instance:ColorChangeData().color_index = role_appearance.color
    --     ViewMgr:OpenView(NeoColorChangeSuccView)
    -- end 
    if cache_appearance ~= nil then NeoCharaChangeData.Instance:CacheAppearance() end 
end

--宠物选择界面打开（未进行新手宠物选择）
function RoleCtrl:OnSCFuncGuideFlagInfo(protocol)
    GuideData.Instance.guide_flag = protocol.flag
    -- 新手寻宝获得第一个宠物的指引屏蔽 22.12.10
    -- if protocol.flag[FuncGuideFlagType.FUNC_GUIDE_NEWBIE_PET_CHOICE] == 0 then
    --     if TreasureHuntData.Instance.open_info == nil or next(TreasureHuntData.Instance.open_info) == nil then
    --         TreasureHuntData.Instance.force_open_guide = true
    --     elseif FunOpen.Instance:GetFunIsOpened(31001) then
    --         local item_id = GuideData.Instance:GetTreasureHuntPetHuntItem()
    --         if BagData.Instance:GetNumByItemId(item_id) ~= 0 then
    --             TreasureHuntData.Instance:GetOpenedTreasureID()
    --             GuideManager.Instance:Start(56) --id56是寻宝获得新手宠物的指引id
    --         end
    --     end
    -- end
    -- 自动抓宠指引判断 -- 老板说先屏蔽掉，掉线不重新指引
    -- if flag_t[#flag_t - 2] == 0 then
    --     local guide_id = 70
    --     local guide_cfg = Cfg.GuideCfgById(guide_id)
    --     local task_id = tonumber(guide_cfg.trigger_param_1) or 0-- 后置任务
    --     local task_list = TaskData.Instance:GetTaskListWithListType(TaskConfig.ListType.accepted)
    --     for i,v in pairs(task_list) do
    --         if task_id == v.task_id then
    --             GuideManager.Instance:Start(guide_id)
    --         end
    --     end
    -- end
end

function RoleCtrl:NewSkillPopup(skill_id, is_life_skill, skill_level)
    self.skill_data:PushNewSkillPopup({skill_id = skill_id, 
        is_life_skill = is_life_skill, skill_level = skill_level})
    if ViewMgr:IsClosed(NewSkillPopupView) then
        ViewMgr:OpenView(NewSkillPopupView)
    end
end

function RoleCtrl:OnPrestigeChangeNotice(protocol)
    local item = Item.Init(65561)
    local config = self.prestige_data:GetVillageList(protocol.village_id)
    item.qua_name = config and RichTextHelper.ColorStr(config.name .. item:Name(), QualityColorStr[item:Color()]) or item:QuaName()
    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = protocol.param}}
    PublicPopupCtrl.Instance:CenterV({item_id = item.item_id,icon_id = config and config.pic or "v1000",num = protocol.param,
    content = Format(Language.PublicPopup.PrestigeTip,item.qua_name, QualityColorStr[item:Color()], protocol.param)})
    self.prestige_data:SetPopupParamValue(protocol.param)
end

function RoleCtrl:SendSetAutoAddPoint(req_type, param_t)
    local protocol = GetProtocol(CSSetAutoAddPointFlag)
    protocol.req_type = req_type
    protocol.index = param_t.index or 0
    protocol.param0 = param_t.param0 or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    protocol.param3 = param_t.param3 or 0
    protocol.param4 = param_t.param4 or 0
    SendProtocol(protocol)
end

function RoleCtrl:OnSCWorldLevelInfo(protocol)
    self.data.world_level_data.open_day = protocol.open_day
    self.data.world_level_data.today_zero_timestamp = protocol.today_zero_timestamp
end

function RoleCtrl:OpenAutoLearnDialogTip(info)
    if nil ~= info.tnr and DialogTipsData.Instance.today_not_reminds[info.tnr..RoleData.Instance:GetRoleId()] then
        if info and info.confirm then
            local confirm_func = info.confirm.func
            if confirm_func and type(confirm_func) == "function" then
                confirm_func()
                return
            end
        end
    end
    ViewMgr:OpenView(RoleTrainSkillTipsView, info)
end

function RoleCtrl.NationalDayCheck()
    -- local start_time = os.time({year = 2021, month = 10, day = 1, hour = 0, minute = 0, sceond = 0})
    -- local end_time = os.time({year = 2021, month = 10, day = 4, hour = 0, minute = 0, sceond = 0})
    local start_time = os.time({year = 2022, month = 6, day = 4, hour = 0, minute = 0, sceond = 0})
    local end_time = os.time({year = 2022, month = 6, day = 5, hour = 0, minute = 0, sceond = 0})
    -- return TimeHelper.IsInTimeInterval(start_time, end_time, Language.Role.NationalDayTips)
    return TimeHelper.IsInTimeInterval(start_time, end_time, Language.Role.DuanWuJieTips)
end

function RoleCtrl:OnAdvanceCareerSubInfo(protocol)
    self.promote_data:SetAdvanceCareerSubInfo(protocol)
    RoleData.Instance:ChangePromoteRedPiont()
end

function RoleCtrl:SendAuditBehaviourSubmitReq(beh)
    local pro = GetProtocol(CSAuditBehaviourSubmitReq)
    pro.behaviour_type = beh
    SendProtocol(pro)
end

function RoleCtrl:PlatReport()
    local url = GLOBAL_CONFIG:Urls().plat_report_url
    local game_id = GLOBAL_CONFIG:Urls().game_id
    local app_key = GLOBAL_CONFIG:Urls().app_key
    if StringIsEmpty(url) or StringIsEmpty(game_id) or StringIsEmpty(app_key) then
        return
    end

    if RechargeCtrl.Instance.wait_info then
        return
    elseif RoleLevel() < 30 and RechargeData.Instance:GetRechargeNum() <= 0 then
        return
    end
    local form = {}
    form.game_id = game_id
    form.role_id = self.data.base_data.role_id
    form.server_no = LoginData.Instance:GetLastLoginServer()
    form.level = self.data.base_data.level
    form.time = os.time()
    form.open_time = TimeCtrl.Instance:ServerStartTS()
    form.pay_total = RechargeData.Instance:GetRechargeNum() * 10
    -- LogError("signstr","game_id="..game_id.."&level="..form.level.."&open_time="..form.open_time.."&pay_total="..form.pay_total.."&role_id="..form.role_id.."&server_no="..form.server_no.."&time="..form.time.."|"..app_key)
    form.sign = string.lower(GameUtil.StrMd5("game_id="..game_id.."&level="..form.level.."&open_time="..form.open_time.."&pay_total="..form.pay_total.."&role_id="..form.role_id.."&server_no="..form.server_no.."&time="..form.time.."|"..app_key))
    -- LogError("form",form)
    WebRequester:Post(url,function (data,error)
        if StringIsEmpty(error) then
            if GLOBAL_CONFIG:Flags().log_print then
                print("PlatReport callback data",data)
            end
        end
    end,form)
end

function RoleCtrl:OnSCRoleAuthorityInfo(protocol)
    MainProber.role_authority = protocol.m_authority_type
end

function RoleCtrl:OnSCRoleHistoryMaxCapabilityChange(protocol)
    if GLOBAL_CONFIG:Flags().max_capability_report or not IS_IOS() then
        ChannelAgent.ToPlatWithParam("Capability_"..protocol.max_role_capability)
    end
end

function RoleCtrl:ReportCapbilityChange()
    if GLOBAL_CONFIG:Flags().capability_change_report or not IS_IOS() then
        ChannelAgent.ToPlatWithParam("CapabilityChange_"..self.data:GetRoleScore())
        if self.level_up_wait_cap then
            ChannelAgent.BehaviourTrigger(BehaviourType.LevelUp)
            self.level_up_wait_cap = false
        end
    end
end

function RoleCtrl:RolePomoteRemind()
    return RolePromoteData.Instance:GetPromoteNum()
end

function RoleCtrl:OnSCRoleInfoEtc(protocol)
    self.data:Set({"da_wan_ka_level"}, protocol)
end