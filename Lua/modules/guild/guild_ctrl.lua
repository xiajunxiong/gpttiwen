FILE_REQ("modules/guild/guild_data")
FILE_REQ("modules/guild/guild_cooperate_data_plus")
VIEW_REQ("modules/guild/guild_view")
VIEW_REQ("modules/guild/guild_list")
VIEW_REQ("modules/guild/guild_create")
VIEW_REQ("modules/guild/guild_notice")
VIEW_REQ("modules/guild/guild_apply")
VIEW_REQ("modules/guild/member_info")
VIEW_REQ("modules/guild/guild_pet_choose")
VIEW_REQ("modules/guild/train_finish")
VIEW_REQ("modules/guild/guild_task")
VIEW_REQ("modules/guild/guild_melee")
VIEW_REQ("modules/guild/guild_levelup")
VIEW_REQ("modules/guild/guild_detail")
VIEW_REQ("modules/guild/guild_melee_menu")
VIEW_REQ("modules/guild/guild_embattle")
VIEW_REQ("modules/guild/guild_embattle_show")
VIEW_REQ("modules/guild/guild_mission")
VIEW_REQ("modules/guild/guild_embattle_menu")
--VIEW_REQ("modules/guild/guild_trial")
VIEW_REQ("modules/guild/guild_trial_rank")
VIEW_REQ("modules/guild/guild_trial_select")
VIEW_REQ("modules/guild/guild_question")
VIEW_REQ("modules/guild/guild_reset_name_view")
VIEW_REQ("modules/guild/guild_invitation")
VIEW_REQ("modules/guild/guild_post")
VIEW_REQ("modules/guild/guild_arms")
VIEW_REQ("modules/guild/guild_jiyuan_jilu")
VIEW_REQ("modules/guild/guild_jiyuan_fabu")
VIEW_REQ("modules/guild/guild_honour_desc")
VIEW_REQ("modules/guild/rongyao_view")
VIEW_REQ("modules/guild/guild_active_rank_view")
VIEW_REQ("modules/guild/guild_active_box_view")
VIEW_REQ("modules/guild/guild_person_rank_view")
VIEW_REQ("modules/guild/guild_inborn")
VIEW_REQ("modules/guild/guild_inborn_reset_view")
VIEW_REQ("modules/guild/guild_inborn_up_view")
VIEW_REQ("modules/guild/guild_letter_view")
VIEW_REQ("modules/guild/guild_trial_new")
VIEW_REQ("modules/guild/guild_cooperate_task")

GuildCtrl = GuildCtrl or BaseClass(BaseCtrl)

GUILD_LIST_REQ_TYPE = {
    GET_GUILD_LIST = 1, --获取家族列表
    GET_GUILD_NOTICE = 2, --获取家族通知 param 家族id
    GET_GUILD_BASE_INFO = 3, --获取家族信息
    GET_GUILD_MEMBER_LIST = 4, --获取成员列表
    GET_GUILD_APPLY_LIST = 5, --获取申请列表
    GET_GUILD_BUILD_INFO = 6, --获取建造信息 param 家族id
    GET_GUILD_TASK_INFO = 7, --获取家族任务信息
    GET_GUILD_TASK_RANK_INFO = 8, -- 获取家族任务排行
	GET_GUILD_GATHER_DREAM_LIST = 9, --获取家族集愿信息列表
	GET_GUILD_PUBLISH_GATHER_DREAM = 10, --发布家族集愿信息 param item_id
	GET_GUILD_GIFT_GATHER_DREAM = 11, --赠与集愿碎片 param role_uid
	GET_GUILD_FETCH_GATHER_DREAM = 12, --领取集愿碎片
	GET_GUILD_GIFT_GATHER_DRAEM_LIST = 13, --获取玩家的赠予列表
	GET_GUILD_GET_GATHER_DRAEM_LIST = 14, --获取玩家的受赠列表
	GET_GUILD_ROLE_GIFT_GATHER_DRAEM_LIST = 15, --获取玩家今天赠送信息
	GET_GUILD_ROLE_CAN_PUBLISH_LIST = 16, --获取玩家可发布的集愿信息列表
	GET_GUILD_ROLE_SET_PUBLISH_SEND = 17, -- 设置玩家发布集愿是否发送消息至家族频道 param:0 发送 1:不发送		
    TAN_HE_ZU_ZHANG = 18,                   --弹劾族长
    REFUSE_GUILD_RECOMMEND = 19,            --拒绝公会推荐
    REQ_GUILD_RECOMMEND = 20,               --请求公会推荐
}
GUILD_EXPOP_TYPE = {
    OPEN = 1, --开始
    FETCH = 2, --领奖
    INFO = 3 --信息
}
GUILD_EVENTHANDLER_TYPE = {
    GUILD_EVENTHANDLER_ADD_MEMBER = 1,                  -- xx加入了公会
    GUILD_EVENTHANDLER_DEC_MEMBER = 2,                  -- xx离开了公会
    GUILD_EVENTHANDLER_APPOINTMENT = 3,                 -- xx被任命为 param - guild_post_type
    GUILD_EVENTHANDLER_TYPE_XUNBAO = 4,                 -- 寻宝获得宠物 param - pet_id
    GUILD_EVENTHANDLER_TYPE_ADD_PET = 5,                -- 获得宠物（除寻宝）param - pet_id
    GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH = 6,       -- 成就到达哪一阶段的1星 param - cur_prog
    GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND = 7,     -- 工坊打造 param - item_id
    GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY = 8,-- 打造灵饰 param - jewelry_id
    GUILD_EVENTHANDLER_TYPE_MITI = 9,                   -- 通过试炼之地 param - level
    GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE = 10,     -- 通过锢魔之塔第几层 10层增长 param - pass_layer
	GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP = 11,     	-- 家族升级 param - guild_level
}
GUILD_FIGHT_OPERATOR_TYPE = {
    ALL_RANK_INFO = 0, --请求所有排名和本家族信息
    CLOSE_RANK = 1,--关闭排行榜
}
GUILD_HONOR_BATTLE_STATE = {
    ENTER = 0, --入场时间
    SET = 1, --开始布阵
    BATTLE = 2, --战斗中
    END = 3 --战斗结束
}
GUILD_BOSS_REQ_TYPE = {
    INFO = 0,
    RANK = 1,
    SET_CHAPTER = 2,
    GET_REWARD = 3,
    GUILD_RANK = 4,
}
GUILD_EMBATTLE_STATE = {
    ENTER = 0,
     --入场
    SET = 1,
     --布阵
    BATTLE = 2,
     --战斗
    END = 3
 --结束
}
GUILD_EMBATTLE_REQ = {
    ASSIGN = 0,--分配区域 0 1 2 3 p2 role_id
    JOIN = 1,--到活动场景
    AUTO_ASSIGN = 2,--自动分配
    OB = 3,--观战 zoneid
}
--家族灵脉操作请求
GUILD_INBORN_REQ_TYPE = {
    TYPE_ALL_INFO = 0,      --获取所有信息
    TYPE_LEARN_SKILL = 1,   --学习技能 p1:inborn_type p2:type_seq
    TYPE_RESET_SKILL = 2,   --技能重置 p1:inborn_type p2:type_seq
}

function GuildCtrl:__init()
    if GuildCtrl.Instance ~= nil then
        Debuger.LogError("[GuildCtrl] attempt to create singleton twice!")
        return
    end
    GuildCtrl.Instance = self
    self.data = GuildData.New()

    --基础 成员 被退出 列表 公告 探险信息
    self:RegisterProtocol(SCGuildBaseInfo, "RecvGuildBaseInfo", true)
    self:RegisterProtocol(SCGuildMemberInfo, "RecvGuildMemberInfo", true)
    self:RegisterProtocol(SCGuildMemberInfoChange, "RecvGuildMemberInfoChange")
    self:RegisterProtocol(SCQuitGuild, "RecvQuitGuild", true)
    self:RegisterProtocol(SCSendGuildList, "RecvSendGuildList", true)
    self:RegisterProtocol(SCSendGuildNotic, "RecvSendGuildNotic", true)
    self:RegisterProtocol(SCSendGuildApplyList, "RecvSendGuildApplyList", true)
    self:RegisterProtocol(SCCreateRet, "RecvCreateRet", true)
    self:RegisterProtocol(SCSendGuildExploreInfo, "RecvSendGuildExploreInfo", true)
    self:RegisterProtocol(SCGuildShopInfo, "RecvGuildShopInfo", true)
    self:RegisterProtocol(SCSendGuildTrainingInfo, "RecvSendGuildTrainingInfo", true)
    self:RegisterProtocol(SCGuildPetTrainingExp, "RecvGuildPetTrainingExp", true)
    self:RegisterProtocol(SCGuildInvitationNotice, "RecvGuildInvitationNotice", true)

    self:RegisterProtocol(SCGuildQuitInfo, "RecvGuildQuitInfo", true)
    self:RegisterProtocol(SCGuildRecommend, "RecvSCGuildRecommend")

    --家族试炼
    self:RegisterProtocol(SCGuildBossInfo, "RecvGuildBossInfo")
    self:RegisterProtocol(SCGuildBossRankInfo, "RecvGuildBossRankInfo")
    self:RegisterProtocol(SCGuildBossChapterGuildRankInfo, "RecvSCGuildBossChapterGuildRankInfo")   --家族试炼家族排行榜
    self:RegisterProtocol(SCGuildBuildInfo, "RecvGuildBuildInfo", true)
    self:RegisterProtocol(SCGuildTaskAllInfo, "SCGuildTaskAllInfo")
    self:RegisterProtocol(SCGuildTaskSingleInfo, "SCGuildTaskSingleInfo")

    self:RegisterProtocol(SCActivityGuildFightParticipNum, "RecvActivityGuildFightParticipNum")

    self:RegisterProtocol(SCPlayerApplyList, "RecvSCPlayerApplyList") --申请列表
    self:RegisterProtocol(CSGuildAbdicationOp) --转让族长

    --自动批准
    self:RegisterProtocol(SCGuildNeedApproveInfo, "RecvSCGuildNeedApproveInfo") --自动批准下发
    self:RegisterProtocol(CSGuildNeedApproveReq) --自动批准请求
    
    self:RegisterProtocol(SCWorldGuildGuildTopInfo, "RecvSCWorldGuildGuildTopInfo") --荣耀之证排名
    -- 098 创建 退出 申请 审批 踢出 列表 修改 任命（转让）探险操作 购买 强化训练（技能升级） 宠物训练 请求信息 邀请
    self:RegisterProtocol(CSCreateGuild)
    self:RegisterProtocol(CSQuitGuild)
    self:RegisterProtocol(CSApplyJoinGuildReq)
    self:RegisterProtocol(CSApplyJoinGuildAck)
    self:RegisterProtocol(CSGuildExpelOutReq)
    self:RegisterProtocol(CSGetGuildList)
    self:RegisterProtocol(CSSetGuildNoticeReq)
    self:RegisterProtocol(CSGuildAppointmentOp)
    self:RegisterProtocol(CSGuildExploreOp)
    self:RegisterProtocol(CSGuildShopBuy)
    self:RegisterProtocol(CSGuildPetTrainingUp)
    self:RegisterProtocol(CSGuildPetTraining)
    self:RegisterProtocol(CSGetGuildTrainingInfo)
    self:RegisterProtocol(CSGuildInvitationReq)

    self:RegisterProtocol(CSGuildBossReq)

    -- self:RegisterProtocol(CSGuildBuildReq)
    self:RegisterProtocol(CSGuildRenameReq)
    self:RegisterProtocol(CSGuildBuildDonateReq)
    self:RegisterProtocol(CSGuildTaskReq)
    self:RegisterProtocol(CSGuildLevelUpReq) --升级
    self:RegisterProtocol(CSInvitationReply) --邀请回复

    -----------------家族大乱斗---------
    self:RegisterProtocol(SCActivityGuildFightRankAllInfo, "RecvActivityGuildFightRankAllInfo")
    self:RegisterProtocol(SCActivityGuildFightMyGuildInfo, "RecvActivityGuildFightMyGuildInfo")
    self:RegisterProtocol(SCActivityGuildFightSpecialInfo, "RecvActivityGuildFightSpecialInfo")
    self:RegisterProtocol(CSActivityGuildFightOperatorReq) --家族大乱斗请求
    -- EventSys:Bind(GlobalEventKeys.LoginSuccess, BindTool.Bind(self.OnLogin, self))
    -----------------家族荣誉战---------
    self:RegisterProtocol(CSGuildHonorReq)
    self:RegisterProtocol(SCGuildHonorInfo, "RecvSCGuildHonorInfo")
    self:RegisterProtocol(SCGuildHonorRoleInfo, "RecvSCGuildHonorRoleInfo")
    self:RegisterProtocol(SCGuildHonorRoleChange, "RecvSCGuildHonorRoleChange")
    self:RegisterProtocol(SCGuildHonorAddRoleInfo, "RecvSCGuildHonorAddRoleInfo")
    self:RegisterProtocol(SCGuildHonorZoneBattleResultInfo, "RecvSCGuildHonorZoneBattleResultInfo")
    self:RegisterProtocol(SCGuildHonorAllZoneBattleResultInfo, "RecvSCGuildHonorAllZoneBattleResultInfo")
    self:RegisterProtocol(SCGuildHonorZoneRoleNumInfo, "RecvSCGuildHonorZoneRoleNumInfo")

    -----------------家族周任务-----------
    self:RegisterProtocol(SCGuildWeekTaskAllInfo, "RecvSCGuildWeekTaskAllInfo")
    self:RegisterProtocol(SCGuildWeekRankList, "RecvSCGuildWeekRankList")

    -----------------家族答题------------
    self:RegisterProtocol(SCActivityGuildAnswerCurQuestion, "RecvSCActivityGuildAnswerCurQuestion")
    self:RegisterProtocol(SCActivityGuildAnswerResultInfo, "RecvSCActivityGuildAnswerResultInfo")
    self:RegisterProtocol(SCActivityGuildAnswerAwardSettle, "RecvSCActivityGuildAnswerAwardSettle")
    self:RegisterProtocol(CSActivityGuildAnswerAnswerQuestion)
    
    --self:RegisterProtocol(SCGuildEventInfo, "RecvSCGuildEventInfo")
    -----族徽修改
    self:RegisterProtocol(CSGuildSetBanner)
	
	-----------------家族集愿------------
	self:RegisterProtocol(SCGuildGatherDreamInfoList, "RecvSCGuildGatherDreamInfoList")
    self:RegisterProtocol(SCGuildGatherDreamSignleInfo, "RecvSCGuildGatherDreamSignleInfo")
    self:RegisterProtocol(SCRoleGiftGatherDreamList, "RecvSCRoleGiftGatherDreamList")
	self:RegisterProtocol(SCRoleCanPublishGatherDreamList, "RecvSCRoleCanPublishGatherDreamList")
	self:RegisterProtocol(SCRoleGatherDreamList, "RecvSCRoleGatherDreamList")

	-----------------家族活跃------------
	self:RegisterProtocol(CSGuildActiveGetGift)
	self:RegisterProtocol(CSGuildActivePersonRank)
	self:RegisterProtocol(CSCrossGuildActiveGuildRankInfo)
    self:RegisterProtocol(SCGuildActivePersonRank, "RecvSCGuildActivePersonRank")
	self:RegisterProtocol(SCCrossGuildRankListAck, "RecvSCCrossGuildRankListAck")
	self:RegisterProtocol(SCCrossGuildRankListAllServer, "SCCrossGuildRankListAllServer")
	self:RegisterProtocol(SCGuildActiveInfoMember, "RecvSCGuildActiveInfoMember")
	self:RegisterProtocol(SCGuildActiveInfo, "RecvSCGuildActiveInfo")
	-----------------家族灵脉------------
    self:RegisterProtocol(CSGuildInbornReq)
    self:RegisterProtocol(SCGuildInbornAllInfo, "RecvSCGuildInbornAllInfo")
    self:RegisterProtocol(SCGuildInbornSingleInfo, "RecvSCGuildInbornSingleInfo")
    self:RegisterProtocol(SCGuildInbornSingleTypeInfo, "RecvSCGuildInbornSingleTypeInfo")
    -- self:RegisterProtocol(SCGuildInbornOtherInfo, "RecvSCGuildInbornOtherInfo")

    -----------------家族每日任务-----------------------
    self:RegisterProtocol(CSRoleGuildTaskReq)
    self:RegisterProtocol(SCRoleGuildTaskInfo, "RecvRoleGuildTaskInfo")
    self:RegisterProtocol(SCRoleGuildTaskChange, "RecvRoleGuildTaskChange")

    RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildMember, self.data.apply_count, BindTool.Bind(self.FlushApplyRemind, self))
    RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildPet, self.data.guild_pet_point, BindTool.Bind(self.FlushPetRemind, self))
    RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildJiYuan, self.data.guild_jiyuan_get_point, BindTool.Bind(self.FlushJiYuanGetRemind, self))
    
    local guild_info_func = BindTool.Bind(self.FlushGuildInfoRemind, self)
	RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildInfo, self.data.guild_trial_point, guild_info_func)
	RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildInfo, ActivityData.Instance:GetActivityEvent(), guild_info_func)
	RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildInfo, GuildHofData.Instance.info_data, guild_info_func, "info")
    RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildInfo, self.data:GuildBossInfo(), guild_info_func)
    -- RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildInfo, self.data.guild_active_info_flush, guild_info_func)
  
	LoginData.Instance:BaseData():Care(BindTool.Bind(self.OnCrossServerStateChange, self),"is_on_crosssever")
    RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildShop, self.data.guild_shop_first_item, BindTool.Bind(self.FlushShopRemind, self))
	RemindCtrl.Instance:Remind():Register(Mod.Guild.GuildSkill, self.data.guild_inborn_point, BindTool.Bind(self.FlushSkillRemind, self))
	TimeCtrl.Instance.server_data:KeysCare({"server_open_day"}, BindTool.Bind(self.OnChangeDayNotice, self))
	
    local coo_remind_func = BindTool.Bind(self.FlushCooperateRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Guild.GuildCooperateTask,
        self.data.member_info,
        coo_remind_func
    ) 
    RemindCtrl.Instance:Remind():Register(
        Mod.Guild.GuildCooperateTask,
        self.data.cooperate_operate,
        coo_remind_func
    ) 

	ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GUILD_BOSS, 
       {MainType = GuildCtrl.Instance, challenge_func = "EnterTrialFight", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
       )

    ActivityData.Instance:CustomClickHandle(
        ActType.GuildMelee,
        function()
            if ActivityData.Instance:GetActivityStatus(ActType.GuildMelee) == ActStatusType.Standy then
                PublicPopupCtrl.Instance:Center(Language.Guild.ActivityTip)
                return true
            end
            local co = Cfg.TeamTypeTargetAct(ActType.GuildMelee)
            TeamCtrl.Instance:SendChangeTeamLimit(co.low_level, co.high_level, co.target_id)
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GUILD_FIGHT)
            self.data:SetDefeatedTimes(0)
        end
    )
    ActivityData.Instance:CustomClickHandle(
        ActType.GuildEmbattle,
        function()
            if GuildData.Instance:IsInGuildHonorFB(true) then
                PublicPopupCtrl.Instance:Center(ErrorText[81])
            else
                local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
                if status == ActStatusType.Close then
                    PublicPopupCtrl.Instance:Center(ErrorText[85])
                    return
                elseif status == ActStatusType.Standy then
                    PublicPopupCtrl.Instance:Center(Language.Guild.GuildEmbattleStandyTip)
                    return
                --[[ else
                    local info = GuildData.Instance:GuildHonorInfo()
                    if info.state == GUILD_HONOR_BATTLE_STATE.BATTLE then
                        PublicPopupCtrl.Instance:Center(ErrorText[112])
                        return
                    end ]]
                end
                --如果自己的家族没有参赛资格则显示赛况界面
                if GuildEmbattleData.Instance:EntryQualify() == false then
                    PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GotoTip[2])
                    ViewMgr:OpenView(GuildEmbattleMsg)
                else
                    if status == ActStatusType.Open and GuildEmbattleData.Instance:Base().round_state == 0 then
                        GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
                    else
                        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.GotoTip[1])
                    end
                    -- GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.JOIN})
                end
            end
        end
    )
    ActivityData.Instance:CustomClickHandle(
        ActType.GuildAnswer,
        function()
            if GuildData.Instance:IsInAnswerScene() then
                return
            end
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GUILD_ANSWER)
        end
    )
    --注册结算事件
    FinishData.Instance:PassRegister(
        BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT,
        {
            value_func = function()
                return self.data:GetMeleeSuccReward()
            end
        }
    )
    FinishData.Instance:FailRegister(
        BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT,
        {
            desc_func = function()
                return self.data:GetMeleeFailTip()
            end
        }
    )
    FinishData.Instance:SetCloseFunc(
        BATTLE_MODE.BATTLE_MODE_GUILD_BOSS,
        function()
            ViewMgr:OpenView(GuildTrialNew)
        end
    )
    local params = {}
    params.modKey = Mod.Guild.GuildJiYuan
    params.smData = self.data.guild_info
    FunOpen.Instance:RegisterCondition(params)
end

function GuildCtrl:EnterTrialFight()
   self.data:EnterTrialFight()
end

function GuildCtrl:PetOnLimitNum()
    return 4
end

--布阵界面助战信息文字
function GuildCtrl:AssistTextFunc(assist_num)
    return string.format(Language.Arena.FormationAssistInfo, assist_num)
end

function GuildCtrl:FlushApplyRemind()
    local num = self.data:CheckApplyCount()
    return num
end

function GuildCtrl:FlushPetRemind()
    local num = self.data.guild_pet_point.count
    return num
end

function GuildCtrl:FlushJiYuanGetRemind()
    local num = self.data.guild_jiyuan_get_point.count
    return num
end

function GuildCtrl:FlushGuildInfoRemind()
    return self.data:GuildInfoRemind()
end

function GuildCtrl:OnCrossServerStateChange()
    self.data.guild_trial_point.flush = not self.data.guild_trial_point.flush
end

function GuildCtrl:FlushShopRemind()
    local num = self.data:GetShopRedPointNum()
    return num
end

function GuildCtrl:FlushSkillRemind()
    local num = self.data.guild_inborn_point.count
    return num
end

function GuildCtrl:FlushCooperateRemind()
    local num = self.data:GetCooperateRemindNum()
    return num
end

function GuildCtrl:__delete()
    self:UnRegisterProtocol(SCGuildBaseInfo)
    self:UnRegisterProtocol(SCGuildMemberInfo)
    self:UnRegisterProtocol(SCQuitGuild)
    self:UnRegisterProtocol(SCSendGuildList)
    self:UnRegisterProtocol(SCSendGuildNotic)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Guild.GuildMember)
end

function GuildCtrl:OnInit()
    self.handle_login = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))
    self.handle_act = ActivityData.Instance.activity_status_event:KeysCare({"act_type", "status"}, BindTool.Bind(self.OnActEvent, self))
    ActivityData.Instance:CustomStatusButtonName(ActType.GuildMelee, ActStatusType.Standy, Language.Activity.ActItem.Status[2])
end

function GuildCtrl:OnUnloadGameLogic()
    --[[ LoginData.Instance:BaseData():Uncare(self.handle_login)
    ActivityData.Instance.activity_status_event:Uncare(self.handle_act) ]]
    self.chat_guild_task = nil
    self.data.guild_letter_flag = false
end
function GuildCtrl:OnActEvent()
    --[[ local status = ActivityData.Instance:GetActivityStatus(ActType.GuildMelee)
    if status == ActStatusType.Close then
        GuildCtrl.SendActivityGuildFightOperatorReq(GUILD_FIGHT_OPERATOR_TYPE.ALL_RANK_INFO)
        local join_num = self.data.melee_join_member_num or 0
        if join_num > 0 then
            if MainViewData.Instance:IsBattle() then
                if nil ~= self.handle_gm then
                    MainViewData.Instance:BaseData():Uncare(self.handle_gm)
                    self.handle_gm = nil
                end
                self.handle_gm = MainViewData.Instance:BaseData():Care(function ()
                    MainViewData.Instance:BaseData():Uncare(self.handle_gm)
                    self.handle_gm = nil
                    ViewMgr:OpenView(GuildMelee)
                end, "is_battle")
            else
                ViewMgr:OpenView(GuildMelee)
            end
            self.data.melee_join_member_num = 0
        end
    end ]]
    local status = ActivityData.Instance:GetActivityStatus(ActType.RushDungeon)
    --[[ if status == ActStatusType.Close then
        RushDungeonData.Instance:ClearData()
    end ]]
    -- 在答题场景并且获得开始
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildAnswer)
    if status == ActStatusType.Open then
        if self.data:IsInAnswerScene() then
            ViewMgr:OpenView(GuildQuestion)
        -- SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()
        end
    elseif status == ActStatusType.Close then
        --if self.data:IsInAnswerScene() then
            -- SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()
            -- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GUILD_HOME)
        --[[ local x,y = SceneCtrl.Instance:CurScene():PosToServer(MainPlayer:GetPos())
            local param_t = {}
            param_t.scene_id = 160
            param_t.pos_x = x
            param_t.pos_y = y
            SceneCtrl.Instance:SendFlyByShoe(param_t) ]]
        --end
    end
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
    if status == ActStatusType.Open then
        GuildData.Instance:SetShowZoneId(nil)
    end
end

function GuildCtrl:OnLoginEvent()
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        -- LookingForRabbitsCtrl.Instance:OnActOpen()--暂时放这里
        -- TeamCtrl.Instance:SendTeamMatch(nil, nil, 0)
        self.data:ClearGuildInfo()
        RushDungeonCtrl.Instance:SendBraveGroundReq({req_type = BRAVE_GROUND_REQ_TYPE.INFO})
        RushDungeonCtrl.Instance:SendBraveGroundReq({req_type = BRAVE_GROUND_REQ_TYPE.RANK_INFO})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
        GuildCtrl.SendGetGuildTrainingInfo()
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_APPLY_LIST})
        -- GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BUILD_INFO,param = self.data:GetGuildID()})
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GATHER_DREAM_LIST})

        self.data:CheckTrainInfo()

        GuildCtrl.SendGuildBossReq()

        if RoleCtrl.Instance.data:GetBaseData().profession ~= 0 then
            GuildData.Instance:UpdateFakeGuildTask()
        end
        RoleCtrl.Instance.data:GetBaseData():KeysCare(
            {"profession"},
            function()
                if RoleCtrl.Instance.data:GetBaseData().profession ~= 0 then
                    GuildData.Instance:UpdateFakeGuildTask()
                end
            end
        )
    end
end

function GuildCtrl:RecvGuildBaseInfo(protocol)
    if self.data.is_invitation_flag and self.data:GetGuildID() == 0 then
        ViewMgr:OpenView(GuildView)
        self.data:SetInvitationFlag()
    end
    if ViewMgr:IsOpen(GuildList) and self.data:GetGuildID() == 0 then
        ViewMgr:CloseView(GuildList)
        ViewMgr:OpenView(GuildView)
        GuildCtrl.SendGetGuildTrainingInfo()
    end
    if MainPlayer then
        MainPlayer:Vo().guild_name = protocol.guild_name
        MainPlayer:Vo().arms = protocol.arms
    end
    RoleData.Instance:GetBaseData().guild_name = protocol.guild_name
    RoleData.Instance:GetBaseData().arms = protocol.arms
    if self.data.guild_info.can_tan_he_patriarch ~= nil then
        if self.data.guild_info.can_tan_he_patriarch == 1 and protocol.can_tan_he_patriarch == 1 
            and self.data.guild_info.is_join_tan_he_patriarch ~= protocol.is_join_tan_he_patriarch 
            and protocol.is_join_tan_he_patriarch == 1 then
            ViewMgr:OpenView(GuildAccuse)
        end
    end
    self.data:SetGuildInfo(protocol)
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_TASK_INFO})
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_TASK_RANK_INFO})
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GATHER_DREAM_LIST})
	GuildCtrl.SendGuildBossReq()
    GuildRedBagCtrl.SendGuildHongBao()
    GuildRedBagCtrl.GetGuildHongBao()
    self.data:CheckInbornRemind()
end

function GuildCtrl:RecvCreateRet(protocol)
    if protocol.param == 1 then
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_TASK_INFO})
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_TASK_RANK_INFO})
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_GATHER_DREAM_LIST})
        GuildCtrl.SendGetGuildTrainingInfo()
        ViewMgr:CloseView(GuildList)
        ViewMgr:CloseView(GuildCreate)
        ViewMgr:OpenView(GuildView)
    end
end

function GuildCtrl:RecvGuildMemberInfo(protocol)
    for k, v in pairs(protocol.member_list) do
        if v.uid == RoleData.Instance:GetRoleId() then
            MainPlayer:Vo().guild_post = v.post
        end
    end
    self.data:SetMemberInfo(protocol)
end

function GuildCtrl:RecvGuildMemberInfoChange(protocol)
    self.data:SetMemberInfoChange(protocol)
end

function GuildCtrl:RecvQuitGuild(protocol)
    if protocol.reason == 1 then --主动
    else
        PublicPopupCtrl.Instance:Center(ErrorText[27])
    end
    ViewMgr:CloseView(GuildView)
    self.data:ClearGuildInfo()
    ChatData.Instance:ClearGuildRecord()
    self.chat_guild_task = nil
end

function GuildCtrl:RecvSendGuildList(protocol)
    self.data:SetGuildList(protocol.guild_list)
end

function GuildCtrl:RecvSendGuildNotic(protocol)
    self.data:SetGuildNotice(protocol)
end

function GuildCtrl:RecvSendGuildApplyList(protocol)
    self.data:SetApplyList(protocol)
end

function GuildCtrl:RecvSendGuildTrainingInfo(protocol)
    self.data:SetGuildTrainInfo(protocol.guild_training_level)
    self.data:SetGuildPetInfo(protocol.traning_pet_index)
    PetData.Instance:SetTrainInfo(protocol.traning_pet_index)
    self.data.guild_pet_point.count = self.data:CheckPetTrain()
end

function GuildCtrl:RecvGuildPetTrainingExp(protocol)
    self.data:SetTrainResult(protocol)
    local result =  self.data:GetTrainResult()
    -- local pet = PetData.Instance:GetPetInfo(result.pet_index)
    local exp = Item.Init(CommonItemId.PetExp, result.exp)
    local exp = Item.Init(CommonItemId.Exp,result.exp)
    exp.name = Language.Guild.PetExp
    local exp = Item.Init(CommonItemId.Exp,result.exp)
    exp.name = Language.Guild.PetExp
    local item_list = {
        [1] = exp,
        --[[ [2] = {
            virtual_type = REWARD_VIRTUAL_TYPE.TYPE_PET,
            icon_id = pet:IconId(),
            qua_frame = pet:QuaIcon(),
            name = pet:CfgName(),
        } ]]
    }
    local t_time = TimeManager.FormatDHMS(result.time)
    local str_ = string.format(Language.Guild.TrainResult[1], t_time.hour, t_time.min)

    PublicPopupCtrl.Instance:Reward2(item_list,str_)
end

function GuildCtrl:RecvGuildInvitationNotice(protocol)
    self.data:SetInvitationInfo(protocol.invitation_info)
    if MainViewData.Instance:IsBattle() then
        return
    end
    if not FunOpen.Instance:GetFunIsOpened(Mod.Guild) then
        return
    end
    ViewMgr:OpenView(GuildInvitation)
end

-------------家族试炼----------
function GuildCtrl:RecvGuildBossInfo(protocol)
    self.data:SetGuildBossInfo(protocol)
    self.data.boss.flushselect = not self.data.boss.flushselect
	self.data:ShowTrialPoint()
end

function GuildCtrl:RecvGuildBossRankInfo(protocol)
    self.data:SetGuildBossRank(protocol)
end

function GuildCtrl:RecvSCGuildBossChapterGuildRankInfo(protocol)
    self.data:SetGuildTrialTotalRank(protocol.rank_list)
end

function GuildCtrl:RecvGuildBuildInfo(protocol)
    self.data:SetBuildFlag(bit:d2b(protocol.build_flag))
    self.data:SetBuildValue(protocol.build_value)
    self.data:SetBuildDonate(protocol.build_donate_count)
end

-- 下发探险信息
function GuildCtrl:RecvSendGuildExploreInfo(protocol)

end

-- 下发购入次数
function GuildCtrl:RecvGuildShopInfo(protocol)
    self.data:SetShopBuyList(protocol.shop_buy_list)
end

function GuildCtrl.SendCreateGuild(param_t)
    local protocol = GetProtocol(CSCreateGuild)
    protocol.name = param_t.name or ""
    protocol.notice = param_t.notice or ""
    protocol.arms = param_t.arms
    SendProtocol(protocol)
end

function GuildCtrl.SendQuitGuild(guild_id)
    local protocol = GetProtocol(CSQuitGuild)
    protocol.guild_id = guild_id
    SendProtocol(protocol)
end

function GuildCtrl.SendApplyJoinGuildReq(guild_id, one_key)
    local protocol = GetProtocol(CSApplyJoinGuildReq)
    protocol.guild_id = guild_id
    protocol.one_key = one_key or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendApplyJoinGuildAck(param_t)
    local protocol = GetProtocol(CSApplyJoinGuildAck)
    protocol.guild_id = param_t.guild_id
    protocol.uid = param_t.uid
    protocol.result = param_t.result
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildExpelOutReq(param_t)
    local protocol = GetProtocol(CSGuildExpelOutReq)
    protocol.guild_id = param_t.guild_id
    protocol.expel_uid = param_t.expel_uid
    SendProtocol(protocol)
end

function GuildCtrl.SendGetGuildList(param_t)
    local protocol = GetProtocol(CSGetGuildList)
    protocol.req_type = param_t.req_type
    protocol.param = param_t.param or 0
	protocol.param1 = param_t.param1 or 0
    SendProtocol(protocol)
end

function GuildCtrl.SetGuildNoticeReq(param_t)
    local protocol = GetProtocol(CSSetGuildNoticeReq)
    protocol.guild_id = param_t.guild_id
    protocol.notice = param_t.notice or ""
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildAppointmentOp(param_t)
    local protocol = GetProtocol(CSGuildAppointmentOp)
    protocol.guild_id = param_t.guild_id
    protocol.uid = param_t.uid
    protocol.post = param_t.post
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildAbdicationOp(uid)
    local protocol = GetProtocol(CSGuildAbdicationOp)
    protocol.guild_id = GuildData.Instance:GetGuildID()
    protocol.uid = uid
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildNeedApproveOp(is_need_approval)
    local protocol = GetProtocol(CSGuildNeedApproveReq)
    protocol.is_need_approval = is_need_approval or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildExploreOp(param_t)
    local protocol = GetProtocol(CSGuildExploreOp)
    protocol.op_type = param_t.op_type
    protocol.param = param_t.param or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildShopBuy(param_t)
    local protocol = GetProtocol(CSGuildShopBuy)
    protocol.guild_id = param_t.guild_id
    protocol.shop_seq = param_t.shop_seq or 0
    protocol.count = param_t.count or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildPetTrainingUp(training_type)
    local protocol = GetProtocol(CSGuildPetTrainingUp)
    protocol.training_type = training_type
    SendProtocol(protocol)
end
function GuildCtrl.SendGetGuildTrainingInfo()
    local protocol = GetProtocol(CSGetGuildTrainingInfo)
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildPetTraining(param_t)
    local protocol = GetProtocol(CSGuildPetTraining)
    protocol.pet_index = param_t.pet_index
    protocol.places_index = param_t.places_index or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildInvitationReq(param_t)
    local protocol = GetProtocol(CSGuildInvitationReq)
    protocol.uid = param_t.uid
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildBossReq(req_type, p1)
    local protocol = GetProtocol(CSGuildBossReq)
    protocol.req_type = req_type or GUILD_BOSS_REQ_TYPE.INFO
    protocol.p1 = p1 or 0
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildReNameReq(param_t)
    local protocol = GetProtocol(CSGuildRenameReq)
    protocol.new_name = param_t.new_name
    SendProtocol(protocol)
end

function GuildCtrl.SendGuildBuildDonateReq(param_t)
    local protocol = GetProtocol(CSGuildBuildDonateReq)
    protocol.guild_id = param_t.guild_id
    protocol.item_id = param_t.item_id
    protocol.num = param_t.num
    protocol.build_seq = param_t.build_seq
    SendProtocol(protocol)
end

function GuildCtrl:SCGuildTaskAllInfo(protocol)
    GuildData.Instance:SetGuildTaskAllInfo(protocol)
end

function GuildCtrl:SCGuildTaskSingleInfo(protocol)
    GuildData.Instance:SetGuildTaskSingleInfo(protocol)
end

function GuildCtrl:CSGuildTaskReq(req_type, index)
    local protocol = GetProtocol(CSGuildTaskReq)
    protocol.req_type = req_type
    protocol.index = index
    SendProtocol(protocol)
end
-- 升级
function GuildCtrl.SendGuildLevelUpReq(guild_id)
    local protocol = GetProtocol(CSGuildLevelUpReq)
    protocol.guild_id = guild_id
    SendProtocol(protocol)
end

function GuildCtrl:RecvActivityGuildFightRankAllInfo(protocol)
    self.data.melee_nums_info.resi_role = protocol.rank_all_info.resi_role
    self.data.melee_nums_info.resi_guild = protocol.rank_all_info.resi_guild
    self.data:SetMeleeRankAllInfo(protocol)
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetMeleeTotalPreReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end
function GuildCtrl:RecvActivityGuildFightSpecialInfo(protocol)
    self.data.melee_nums_info.resi_role = protocol.resi_role
    self.data.melee_nums_info.resi_guild = protocol.resi_guild
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetMeleeTotalPreReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end
function GuildCtrl:RecvActivityGuildFightMyGuildInfo(protocol)
    self.data:SetMeleeMyRankInfo(protocol)
    if protocol.type == 1 then
        ViewMgr:OpenView(GuildMelee)
    end
end

function GuildCtrl:RecvActivityGuildFightParticipNum(protocol)
    self.data:SetMeleeMyGuildNum(protocol.guild_join_num)
end

function GuildCtrl:RecvSCPlayerApplyList(protocol)
    self.data:SetPlayerApplyList(protocol.guild_id_list)
end

function GuildCtrl:RecvSCGuildNeedApproveInfo(protocol)
    self.data:SetNeedApproval(protocol.is_need_approval)
end

function GuildCtrl.SendActivityGuildFightOperatorReq(op_type)
    local protocol = GetProtocol(CSActivityGuildFightOperatorReq)
    protocol.op_type = op_type
    SendProtocol(protocol)
end

function GuildCtrl.InvitationReply(result)
    local protocol = GetProtocol(CSInvitationReply)
    protocol.result = result
    SendProtocol(protocol)
end

function GuildCtrl.GuildHonorReq(param_t)
    local protocol = GetProtocol(CSGuildHonorReq)
    protocol.req_type = param_t.req_type or 0
    protocol.p1 = param_t.p1 or 0
    protocol.p2 = param_t.p2 or 0
    SendProtocol(protocol)
end

function GuildCtrl:RecvSCGuildHonorInfo(protocol)
    self.data:SetGuildHonorInfo(protocol)
    self.data.embattle.flushstate = not self.data.embattle.flushstate
    if protocol.state == GUILD_HONOR_BATTLE_STATE.ENTER or protocol.state == GUILD_HONOR_BATTLE_STATE.SET then
        local act_enter_info = GetProtocol(SCActivityEnterInfo)
        act_enter_info.enter_info = {}
        act_enter_info.enter_info.act_type = ActType.GuildEmbattle
        act_enter_info.enter_info.is_enter_flag = 0
        act_enter_info.enter_info.enter_end_timestamp = protocol.next_match_state
        ActivityCtrl.Instance:OnActivityEnterInfo(act_enter_info)
    end
end

function GuildCtrl:RecvSCGuildHonorRoleInfo(protocol)
    self.data:SetHonorRoleInfo(protocol)
    self.data.embattle.flushview = not self.data.embattle.flushview
end

function GuildCtrl:RecvSCGuildHonorRoleChange(protocol)
    if protocol.change_reson == 1 then --删除是彻底删除不可参加
        self.data:DeleteHonorRoleInfo(protocol.role_id, protocol.zone_id)
    else
        self.data:MoveHonorRoleInfo(protocol.role_id, protocol.zone_id)
    end
    self.data.embattle.flushview = not self.data.embattle.flushview
end

function GuildCtrl:RecvSCGuildHonorAddRoleInfo(protocol)
    self.data:AddHonorRoleInfo(protocol.role_info)
    self.data.embattle.flushview = not self.data.embattle.flushview
end

function GuildCtrl:RecvSCGuildHonorZoneBattleResultInfo(protocol)
    self.data:SetZoneBattleResult(protocol.zone_id,protocol.win_guild_id)
    self.data:SetZoneInfo(self.data.zone_battle_result)
    local show = true
    for i = 1,3 do
        if self.data:GetZoneBattleResult(i) == 0 then
            show = false
        end
    end
    if show == true then
        ViewMgr:OpenView(FinishPassViewGH)
    end
end

function GuildCtrl:RecvSCGuildHonorAllZoneBattleResultInfo(protocol)
    self.data:SetHonorBattleInfo(protocol.battle_info)
    self.data:SetZoneInfo(protocol.zone_battle_result)
    for i = 1,3 do
        self.data:SetZoneBattleResult(i,protocol.zone_battle_result[i])
    end
end

function GuildCtrl:RecvSCGuildHonorZoneRoleNumInfo(protocol)
    self.data:SetZoneNum(protocol.zone_role_num)
end

function GuildCtrl:OnReplyRoleIsOnline(protocol)
    if protocol.online == 1 and ViewMgr:IsOpen(GuildList) then
        SocietyCtrl.Instance:SendAddRecentChat(protocol.role_id)
        SocietyData.Instance:SetRecentlyChatRole(protocol.role_id)
    else
        PublicPopupCtrl.Instance:Error(ErrorText[64])
    end
end

function GuildCtrl:RecvSCGuildWeekTaskAllInfo(protocol)
    self.data:SetGWTAllInfo(protocol)
    -- 屏蔽家族任务
    -- local all, cur = self.data:GWTTaskInfo()
    -- if self.chat_guild_task == nil and all and cur and all.task_prog < cur.param_2 then
    --     local info = {hide_main = true, block_role = true, is_guild_task = true}
    --     ChatCtrl.Instance:ChannelChatCustom(info,ChatConfig.ChannelId.guild)
    --     ChatData.Instance:SetChannelUnread(ChatConfig.ChannelId.guild)
    --     self.chat_guild_task = true
    -- end
	self.data:ShowTrialPoint()
end

function GuildCtrl:RecvSCGuildWeekRankList(protocol)
    self.data:SetGWTRankList(protocol)
end

function GuildCtrl:RecvSCActivityGuildAnswerCurQuestion(protocol)
    self.data:SetQuestionInfo(protocol)
end

function GuildCtrl:RecvSCActivityGuildAnswerResultInfo(protocol)
    PublicPopupCtrl.Instance:Center(Language.Guild.AnswerTip2[protocol.answer_result])
end

function GuildCtrl:RecvSCActivityGuildAnswerAwardSettle(protocol)
    local param_t = {}
    param_t.hint = Language.Guild.AnswerTip
    param_t.btn1 = {text = Language.Guild.IKnow}
    param_t.items = protocol.item_list
    param_t.time_param = {time = 5, desc = Language.Guild.IKnow2}
    AnswerCtrl.Instance:OpenReward(param_t)
end

-- 0-4
function GuildCtrl.GuildAnswerAnswerQuestion(answer)
    local protocol = GetProtocol(CSActivityGuildAnswerAnswerQuestion)
    protocol.answer = answer or 0
    SendProtocol(protocol)
end

--[[function GuildCtrl:RecvSCGuildEventInfo(protocol)
   self.data:SetGuildEvent(protocol)
end--]]

function GuildCtrl.IsHasGuild(view)
    if view == "GuildList" then
        if GuildData.Instance:GetGuildID() > 0 then -- 判断是否有家族
            GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
            ViewMgr:OpenView(GuildView)
        else
            GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})-- 请求家族列表
            ViewMgr:OpenView(GuildList)
        end
        return true
    end
end
function GuildCtrl.GuildSetBanner(arms)
    local protocol = GetProtocol(CSGuildSetBanner)
    protocol.arms = arms
    SendProtocol(protocol)
end

function GuildCtrl:RecvSCGuildGatherDreamInfoList(protocol)
	self.data:SetGatherDreamInfoList(protocol.dream_list)
end

function GuildCtrl:RecvSCGuildGatherDreamSignleInfo(protocol)
	self.data:SetGatherDreamSignleInfo(protocol.dreamsignleinfo)
end

function GuildCtrl:RecvSCRoleGiftGatherDreamList(protocol)
	if self.data.today_is_publish_gather_dream_flag == -1 then
		if protocol.today_is_publish_gather_dream_flag == 0 then
			self.data.today_is_publish_gather_dream_red = true
		else
			self.data.today_is_publish_gather_dream_red = false
		end
	end
	self.data.today_is_publish_gather_dream_flag = protocol.today_is_publish_gather_dream_flag
	self.data:SetRoleGiftGatherDreamList(protocol.gift_uid_list)
end

function GuildCtrl:RecvSCRoleCanPublishGatherDreamList(protocol)
	local temp_list = {}
	for k, v in pairs(protocol.pet_id_list) do
		local patch_id = PetData.Instance:GetPatchIdByPetId(v.pet_id) --pet_cfg = Config.pet_cfg_auto.pet_info_2[v.pet_id]	
        if patch_id > 0 then
            table.insert(temp_list,patch_id)
        end
    end
	local bag_have_list = BagData.Instance:GetItemsByShowType(ShowType.PetPieces)
	for k, v in pairs(bag_have_list) do
		local is_add = true
		for k1, v1 in pairs(temp_list) do
			if v1 == v.item_id then
				is_add = false
				break
			end
		end
		if is_add then
			table.insert(temp_list,v.item_id)
		end
	end
	local c_family = {}
	for k, v in pairs(Config.family_auto.family_pray) do
		if v.type == 0 then
			table.insert(c_family,v)
		end
	end
	local show_list = {}
	for k, v in pairs(temp_list) do
		local quality = 1
		local pet_cfg = Config.pet_cfg_auto.pet_info_2
		for k1, v1 in pairs(pet_cfg) do
			if v1.patch_id == v then
				quality = v1.quality
				break
			end
		end
		local is_can_pray = 0
		for k1, v1 in pairs(c_family) do
			if quality == v1.quality then
				is_can_pray = v1.is_can_pray
				break
			end
		end
		if is_can_pray == 1 then
			table.insert(show_list,v)
		end
	end
	
	local patch_id_list = {}
    for i=1,#show_list do
        patch_id_list[i] = {}
        patch_id_list[i].patch_id = show_list[i]
		patch_id_list[i].patch_type= 0
    end
	table.sort(patch_id_list,function(a,b)
		local color_a =  Item.GetConfig(a.patch_id).color
		local color_b = Item.GetConfig(b.patch_id).color
		return color_a > color_b end)
	self.data.publish_is_send_flag = protocol.publish_is_send_flag
	self.data:SetRoleCanPublishGatherDreamList(patch_id_list)
end

function GuildCtrl:RecvSCRoleGatherDreamList(protocol)
	if protocol.info_type == 0 then
		self.data:SetRoleGatherDreamGetList(protocol.get_list)
	else
		self.data:SetRoleGatherDreamSendList(protocol.send_list)
	end
end

function GuildCtrl:IsCreateTeam(uid)
    if SceneData.Instance:GetMode() == SCENE_TYPE.GUILD_FIGHT and ActivityData.IsOpen(ActType.GuildMelee) then
        if not TeamData.Instance:InTeam() then
            TeamData.Instance.cur_sel_target = Cfg.TeamTypeTargetAct(ActType.GuildMelee)
            TeamCtrl.Instance:SendCreateTeam()
        end
        Invoke(function()
            TeamCtrl.Instance:SendTeamInvitationReq(uid)
        end,1)
        return true
    end
end

function GuildCtrl:ShowJiYuanRewardList(protocol)
	local item_list = {}
	if protocol.param1 > 0 then
		local gongxian_data = Item.New() --家族资金
		gongxian_data.item_id = 65605
		gongxian_data.num = protocol.param1
		table.insert(item_list,gongxian_data)
	end
	if protocol.param2 > 0 then
		local coin_data = Item.New()	--铜币
		coin_data.item_id = 65532
		coin_data.num = protocol.param2
		table.insert(item_list,coin_data)
	end
	if protocol.param3 > 0 then
		local exp_data = Item.New()	--家族贡献
		exp_data.item_id = 65536
		exp_data.num = protocol.param3
		table.insert(item_list,exp_data)
	end
	if protocol.param4 > 0 and protocol.param5 > 0 then
		local item_data = Item.New() --道具
		item_data.item_id = protocol.param4
		item_data.num = protocol.param5
		table.insert(item_list,item_data)	 
	end
	MainOtherCtrl.Instance:OpenGiftView(item_list)
end

function GuildCtrl:SendGetMeleeReward(param)
    local param1 = {}
    param1.activity_type = ActType.GuildMelee
    param1.op_type = param.op_type
    param1.param1 = param.param1
    ActivityCtrl.Instance:SendActivityAdvanceFetchReq(param1)
end
function GuildCtrl:RecvGetMeleeReward(protocol)
    self.data:SetRewardFlags(protocol)
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetMeleeTotalPreReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end
function GuildCtrl:RecvGetAnswerReward(protocol)
    self.data:SetRewardFlags(protocol)
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetAnswerTotalPreReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end

function GuildCtrl:RecvSCWorldGuildGuildTopInfo(protocol)
    self.data:SetTopInfo(protocol.top_info)
end

function GuildCtrl:RecvGuildQuitInfo(protocol)
   self.data:SetGuildQuitInfo(protocol)
end

function GuildCtrl:SendGuildActivePersonRank()
    local protocol = GetProtocol(CSGuildActivePersonRank)
    SendProtocol(protocol)
end


function GuildCtrl:RecvSCGuildActivePersonRank(protocol)
	self.data.guild_active_person_rank_my_index = protocol.index
	self.data:SetGuildActivePersonRank(protocol.rank_list)
end

function GuildCtrl.SendGuildActiveGetGift(seq)
    local protocol = GetProtocol(CSGuildActiveGetGift)
    protocol.seq = seq
    SendProtocol(protocol)
end

function GuildCtrl:RecvSCCrossGuildRankListAck(protocol)
	self.data.guild_active_total_rank_my_index = protocol.index
	self.data:SetGuildActiveTotalRank(protocol.rank_list)
end

function GuildCtrl:SCCrossGuildRankListAllServer(protocol)
	self.data.guild_active_total_rank_my_index = protocol.index
	self.data:SetGuildActiveTotalRank(protocol.rank_list)
end


function GuildCtrl:SendCrossGuildActiveGuildRankInfo()
    local protocol = GetProtocol(CSCrossGuildActiveGuildRankInfo)
    protocol.is_all = 1
	protocol.plat_type = 0
	protocol.server_id = 0
	protocol.rank_type = 1
	protocol.last_snapshot_time = 0
	protocol.guild_id = self.data:GetGuildID()
	protocol.index_start = 0
	protocol.index_end = 49
    SendProtocol(protocol)
end

function GuildCtrl:RecvSCGuildActiveInfoMember(protocol)
    -- LogDG(protocol)
	self.data:SetGuildActiveBoxFlag(protocol.flag)
	if protocol.seq ~= -1 then
		local reward_list = DataHelper.FormatItemList(self.data:GetGuildActiveRewardBySeq(protocol.seq))
		PublicPopupCtrl.Instance:Reward2(reward_list,Language.Guild.GuildActiveBoxGetReward)
	end
end

function GuildCtrl:RecvSCGuildActiveInfo(protocol)
	self.data:SetGuildActiveInfo(protocol)
end

---------------家族灵脉------------
--req_type -> GUILD_INBORN_REQ_TYPE
function GuildCtrl:SendGuildInbornReq(req_type, param1, param2)
    local protocol = GetProtocol(CSGuildInbornReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function GuildCtrl:RecvSCGuildInbornAllInfo(protocol)
    self.data:SetGuildInbornAllInfo(protocol)
end

function GuildCtrl:RecvSCGuildInbornSingleInfo(protocol)
    self.data:SetGuildInbornSingleInfo(protocol)
end

function GuildCtrl:RecvSCGuildInbornSingleTypeInfo(protocol)
    self.data:SetGuildInbornSingleTypeInfo(protocol)
end

function GuildCtrl:RecvSCGuildInbornOtherInfo(protocol)
    self.data:SetGuildInbornOtherInfo(protocol)
end

function GuildCtrl:RecvSCGuildRecommend(protocol)
    self.data:SetRecommendInfo(protocol)
    --活动中不推荐
    local act_list = {ActType.GuildEmbattle, ActType.GuildMelee, ActType.GuildAnswer}
    for i, v in pairs(act_list) do
        if ActivityData.Instance:GetActivityStatus(act_type) ~= ActStatusType.Close then
            return
        end
    end
    ViewMgr:OpenView(GuildLetterView)
end

function GuildCtrl:OnChangeDayNotice()
    self.data.guild_letter_flag = false
end

function GuildCtrl:RecvRoleGuildTaskInfo(protocol)
    LogDG("?平哥哥，这是家族任务信息下发哦",protocol)
    self.data:SetRoleGuildTaskInfo(protocol)
end

function GuildCtrl:RecvRoleGuildTaskChange(protocol)
    LogDG("?平哥哥，这是家族任务变化信息下发哦",protocol)
    self.data:SetRoleGuildTaskChange(protocol)
end

function GuildCtrl.SendGuildCooperateReq(param_t)
    LogDG("?平哥哥，这是"..Language.GuildCooperateTask.ReqCommon[param_t.req_type].."的请求哦！",param_t)
    local protocol = GetProtocol(CSRoleGuildTaskReq)
    protocol.req_type = param_t.req_type
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    SendProtocol(protocol)
end

