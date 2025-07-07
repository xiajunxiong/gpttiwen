FILE_REQ("modules/marry/marry_config")
FILE_REQ("modules/marry/marry_data")

VIEW_REQ("modules/marry/marry_view")
VIEW_REQ("modules/marry/marry_popup_view")
VIEW_REQ("modules/marry/marry_gift_view")
VIEW_REQ("modules/marry/marry_title_view")
VIEW_REQ("modules/marry/marry_skill_view")
VIEW_REQ("modules/marry/marry_introduce_view")
VIEW_REQ("modules/marry/marry_certificate_view")
VIEW_REQ("modules/marry/marry_invite_view")
VIEW_REQ("modules/marry/marry_order_view")
VIEW_REQ("modules/marry/marry_car_view")
VIEW_REQ("modules/marry/marry_fancy_view")
VIEW_REQ("modules/marry/marry_letter_view")
VIEW_REQ("modules/marry/marry_propose_view")

FILE_REQ("modules/marry/wedding_fb/marry_data_wedding_fb")
VIEW_REQ("modules/marry/wedding_fb/wedding_fb_menu")
VIEW_REQ("modules/marry/wedding_fb/wedding_fb_game_view")
VIEW_REQ("modules/marry/wedding_fb/wedding_fb_bless_view")

MarryCtrl = MarryCtrl or BaseClass(BaseCtrl)

function MarryCtrl:__init()
	if MarryCtrl.Instance ~= nil then
		Debuger.LogError("[MarryCtrl] attempt to create singleton twice!")
		return
	end
	MarryCtrl.Instance = self
	self.data = MarryData.New()
    self:RegisterAllProtocols()
end

function MarryCtrl:__delete()
    self:UnRegisterAllProtocols()
    MarryCtrl.Instance = nil
end

function MarryCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function MarryCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSMarriageReq)
    self:RegisterProtocol(SCMarryInfo, "OnMarryInfo")
    self:RegisterProtocol(SCMarryReqRoute, "OnMarryReqRoute")
    self:RegisterProtocol(SCMarryQueryRet, "OnMarryQueryRet")
    self:RegisterProtocol(SCMarryDivorceRoute, "OnMarryDivorceRoute")
    self:RegisterProtocol(SCMarryCreateNotice, "OnMarryCreateNotice")
    self:RegisterProtocol(SCMarryInviteDisagreeRoute, "OnMarryInviteDisagreeRoute")
    self:RegisterProtocol(SCMarryLeaveNotice, "OnMarryLeaveNotice")
    self:RegisterProtocol(SCWeddingLookLoveInfo, "OnWeddingLookLoveInfo")
    self:RegisterProtocol(SCParadeFloatInfo, "OnParadeFloatInfo")

    self:RegisterProtocol(CSWeddingFBReq)
    self:RegisterProtocol(SCWeddingFBSceneInfo, "OnWeddingFBSceneInfo")
    self:RegisterProtocol(SCWeddingFBRoleInfo, "OnWeddingFBRoleInfo")
    self:RegisterProtocol(SCWeddingFBSync, "OnWeddingFBSync")
    self:RegisterProtocol(SCWeddingRegistList, "OnWeddingRegistList")
    self:RegisterProtocol(SCWeddingInviteList, "OnWeddingInviteList")
    self:RegisterProtocol(SCWeddingApplyList, "OnWeddingApplyList")
    self:RegisterProtocol(SCWeddingChange, "OnWeddingChange")
    self:RegisterProtocol(SCWeddingStatusSync, "OnWeddingStatusSync")
    self:RegisterProtocol(SCWeddingShowInfo, "OnWeddingShowInfo")
    self:RegisterProtocol(SCWeddingSubActBaoWeiXiuQiu, "OnWeddingSubActBaoWeiXiuQiu")
    self:RegisterProtocol(SCWeddingSubActZaoShengGuiZi, "OnWeddingSubActZaoShengGuiZi")
end

function MarryCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
    self.handle_fb_event = self.data:CareWeddingFBEventChange(BindTool.Bind(self.OnCareWeddingFBEventChange, self))
	RemindCtrl.Instance:Remind():Register(Mod.Society.Marry,self.data.info_data,BindTool.Bind(self.CheckRemind, self))
    self.handle_role_notice = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
end

function MarryCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSMarriageReq)
    self:UnRegisterProtocol(SCMarryInfo)
    self:UnRegisterProtocol(SCMarryReqRoute)
end

function MarryCtrl:RoleNoticeComplete()
    self.data.wedding_status_info:Set{init = true}
end

-- 副本事件注册示例(小游戏用)，先飘个字
function MarryCtrl:OnCareWeddingFBEventChange()
    local type = self.data.wedding_fb_event.type
    if MarryConfig.FBEventType.none ~= self.data.wedding_fb_event.type then
        -- PublicPopupCtrl.Instance:Center(Language.Common.Enter .. self.data.config_wedding_party[type].wedding_link)
        self:PlayPuzzle(type == MarryConfig.FBEventType.act_xxxy and 1 or 2)
        if type == MarryConfig.FBEventType.act_xxxy then
            --self:PlayPuzzle(1)
        elseif type == MarryConfig.FBEventType.act_zsgz then
            self:PlayAnswer(1)
            -- ViewMgr:OpenView(WeddingAnswerView)
        elseif type == MarryConfig.FBEventType.rite then
            self:PlayStory()
        end
    end
end

function MarryCtrl:SendMarryReq(req_type, param1,param2)
    local protocol = GetProtocol(CSMarriageReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

--结婚信息下发
function MarryCtrl:OnMarryInfo(protocol)
	self.data:SetMarryInfo(protocol)
end

function MarryCtrl:OnWeddingFBSceneInfo(protocol)
	self.data:SetWeddingFBSceneInfo(protocol)
    self:CreateWeddingBaoWeiXiuQiuMonster()
end

function MarryCtrl:OnWeddingFBRoleInfo(protocol)
	self.data:SetWeddingFBRoleInfo(protocol)
end

function MarryCtrl:OnWeddingFBSync(protocol)
	self.data:SetWeddingFBSync(protocol)
end

function MarryCtrl:OnWeddingRegistList(protocol)
	self.data:SetWeddingRegistList(protocol)
end

function MarryCtrl:OnWeddingInviteList(protocol)
	self.data:SetWeddingInviteList(protocol)
end

function MarryCtrl:OnWeddingApplyList(protocol)
	self.data:SetWeddingApplyList(protocol)
end

function MarryCtrl:OnWeddingChange(protocol)
	self.data:SetWeddingChange(protocol)
end

function MarryCtrl:OnWeddingStatusSync(protocol)
	self.data:SetWeddingStatusSync(protocol)
end

function MarryCtrl:OnWeddingShowInfo(protocol)
	self.data:SetWeddingShowInfo(protocol)
end

function MarryCtrl:OnWeddingSubActBaoWeiXiuQiu(protocol)
	self.data:SetWeddingSubActBaoWeiXiuQiu(protocol)
    self:CreateWeddingBaoWeiXiuQiuMonster()
end

function MarryCtrl:OnWeddingSubActZaoShengGuiZi(protocol)
    self.data:SetWeddingSubActZaoShengGuiZi(protocol)
end

--查询好友下发
function MarryCtrl:OnMarryQueryRet(protocol)
	self.data:SetMarryQueryRet(protocol)
end

function MarryCtrl:OnParadeFloatInfo(protocol)
    if protocol.is_stop == 1 then
        local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.obj_id)
        if obj ~= nil then
            obj:PlayShowEffect(protocol.is_stop)
        end
    end
end

function MarryCtrl:OnWeddingLookLoveInfo(protocol)
    self.data:SetWeddingLookLoveInfo(protocol)
    self:CreateWeddingLookLove(protocol)
end

function MarryCtrl:OnMarryCreateNotice(protocol)
    if SceneLogic.Instance:SceneID() ~= SceneLogic.Instance:MainCityID() then
        return
    end
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_SHIELDING_FLOWERS) then
		SocietyData.Instance:PlayGiftEffect(1, 1)
	end
end

function MarryCtrl:SendWeddingFBReq(param_t)
    local protocol = GetProtocol(CSWeddingFBReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    -- ylog("SendWeddingFBReq", protocol)
    SendProtocol(protocol)
end

-- 赠送祝福道具
function MarryCtrl:SendWeddingFBReqBless(shop_item_index)
    local param_t = {
        req_type = MarryConfig.FBReqType.common,
        param1 = MarryConfig.FBCommonOperType.bless,
        param2 = shop_item_index,
    }
    self:SendWeddingFBReq(param_t)
end

-- 标记剧情已播放
function MarryCtrl:SendWeddingFBReqRite()
    local param_t = {
        req_type = MarryConfig.FBReqType.common,
        param1 = MarryConfig.FBCommonOperType.rite,
        param2 = 1,
    }
    self:SendWeddingFBReq(param_t)
end

-- 用餐
function MarryCtrl:SendWeddingFBReqMeal()
    local param_t = {
        req_type = MarryConfig.FBReqType.common,
        param1 = MarryConfig.FBCommonOperType.meal,
    }
    self:SendWeddingFBReq(param_t)
end

-- 获取热度奖励
function MarryCtrl:SendWeddingFBReqHeatReward(heat_type)
    local param_t = {
        req_type = MarryConfig.FBReqType.common,
        param1 = MarryConfig.FBCommonOperType.heat_reward,
        param2 = heat_type,
    }
    self:SendWeddingFBReq(param_t)
end

-- 获取喜糖奖励
function MarryCtrl:SendWeddingFBReqCandies()
    local param_t = {
        req_type = MarryConfig.FBReqType.common,
        param1 = MarryConfig.FBCommonOperType.candies,
    }
    self:SendWeddingFBReq(param_t)
end

-- 婚宴预约
function MarryCtrl:SendWeddingFBReqRegist(regist_time)
    local param_t = {
        req_type = MarryConfig.FBReqType.regist,
        param1 = regist_time,
    }
    self:SendWeddingFBReq(param_t)
end

-- 取消婚宴预约
function MarryCtrl:SendWeddingFBReqRegistOff()
    local param_t = {
        req_type = MarryConfig.FBReqType.regist_off,
    }
    self:SendWeddingFBReq(param_t)
end

-- 请求婚宴预约信息
function MarryCtrl:SendWeddingFBReqRegistList()
    local param_t = {
        req_type = MarryConfig.FBReqType.regist_list,
    }
    self:SendWeddingFBReq(param_t)
end

-- 取消订阅婚宴预约信息
function MarryCtrl:SendWeddingFBReqSubRegistList()
    local param_t = {
        req_type = MarryConfig.FBReqType.sub_regist_list,
        param1 = 0,
    }
    self:SendWeddingFBReq(param_t)
end

-- 婚宴邀请
function MarryCtrl:SendWeddingFBReqInvite(uid)
    local param_t = {
        req_type = MarryConfig.FBReqType.invite,
        param1 = uid,
    }
    self:SendWeddingFBReq(param_t)
end

-- 婚宴申请
function MarryCtrl:SendWeddingFBReqApply(wedding_id)
    local param_t = {
        req_type = MarryConfig.FBReqType.apply,
        param1 = wedding_id,
    }
    self:SendWeddingFBReq(param_t)
end

-- 婚宴申请处理
function MarryCtrl:SendWeddingFBReqMark(uid)
    local param_t = {
        req_type = MarryConfig.FBReqType.mark,
        param1 = uid,
        param2 = 1,
    }
    self:SendWeddingFBReq(param_t)
end

-- 扩展邀请上限
function MarryCtrl:SendWeddingFBReqInviteSizeBuy()
    local param_t = {
        req_type = MarryConfig.FBReqType.invite_size_buy,
    }
    self:SendWeddingFBReq(param_t)
end

-- 预定巡游
function MarryCtrl:SendWeddingFBReqXunYou(float_type)
    local param_t = {
        req_type = MarryConfig.FBReqType.xun_you,
        param1 = float_type,
    }
    self:SendWeddingFBReq(param_t)
end

-- 展示信息
function MarryCtrl:SendWeddingFBReqWeddingShowInfo(wedding_id)
    local param_t = {
        req_type = MarryConfig.FBReqType.wedding_show_info,
        param1 = wedding_id,
    }
    self:SendWeddingFBReq(param_t)
end

--求婚信息转发 飘字或者弹窗 
function MarryCtrl:OnMarryReqRoute(protocol)
    if protocol.role_id == RoleId() then
        ViewMgr:OpenView(MarryProposeView,{propose_type = MarryConfig.PROPOSE_TYPE.INVITE,
        role_id = protocol.inviter_role_id,marry_type = protocol.marry_type})
    else
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[24])
    end
end

--收到回应
function MarryCtrl:OnMarryInviteDisagreeRoute(protocol)
    if protocol.is_agree == 0 then
        self:OnResultRefuse(protocol)
    else
        ViewMgr:OpenView(MarryProposeView, {propose_type = MarryConfig.PROPOSE_TYPE.REPLY, role_id = protocol.target_id})
    end
end

--结婚NPC按钮
function MarryCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self.data:GetNpcId() then
        local NpcButtonName = Language.Marry.NpcButtonName
        if self.data:GetHusbandOrWifeUid() == 0 then--发起求婚按钮只有在已婚后才消失
            table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[1],BindTool.Bind(self.OnInvationPropose, self)))--发起求婚
        end
        table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[2],BindTool.Bind(self.OnMarriageProcess, self)))--结婚流程
        -- 婚宴预约按钮在未预约和已预约状态显示
        if self.data:HasWeddingType(MarryConfig.HasWeddingType.no) or self.data:HasWeddingType(MarryConfig.HasWeddingType.now) then
            table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[5],BindTool.Bind(self.OnMarriageWedding, self)))--婚宴预约
        end
        -- 婚宴邀请按钮在预约完后显示
        if self.data:HasWeddingType(MarryConfig.HasWeddingType.now) then
            table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[4],BindTool.Bind(self.OnMarriageInvation,self)))--婚宴邀请
        end
        -- 进入婚宴按钮在预约完后显示
        if self.data:HasWeddingType(MarryConfig.HasWeddingType.now) then
            table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[6],BindTool.Bind(self.OnMarriageEnterFB, self)))--进入婚宴
        end
        if self.data:GetHusbandOrWifeUid() ~= 0 then--申请离婚按钮只有在已婚后才出现
            table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(NpcButtonName[3],BindTool.Bind(self.OnMarriageDivorce, self)))
        end
	end
end

--收到组队邀请
function MarryCtrl:OnResultTeam(info)
    ViewMgr:OpenView(MarryPopupView,{
        cancel_time = Config.get_married_auto.other[1].propose_time,
        cancel_name = Language.Marry.BtnName[1],
        confirm_name = Language.Marry.BtnName[2],
        content = Format(Language.Marry.ErrorText[0],info.leader.name),
        cancel_func = function()TeamCtrl.Instance:SendTeamInvationRet(info.leader.uid, false, info.is_cross,info.team_index)end,
        confirm_func = function()TeamCtrl.Instance:SendTeamInvationRet(info.leader.uid, true, info.is_cross,info.team_index)end
    })
end

--收到求婚拒绝
function MarryCtrl:OnResultRefuse(protocol)
    ViewMgr:OpenView(MarryPopupView,{
        cancel_name = Language.Marry.BtnName[3],
        confirm_name = Language.Marry.BtnName[4],
        content = Language.Marry.ErrorText[1],
        cancel_func = function()ViewMgr:OpenView(MarryLetterView,{info = SocietyData.Instance:GetFriendInfoByUid(protocol.target_id)})end
    })
end

--发起求婚
function MarryCtrl:OnInvationPropose()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.Marry.WeddingCrossTips,function()
            LoginCtrl.Instance:SendCrossStartReq()
        end)
        return
    end
    if not self:IsHasTeam() then
        return
    end
    --判断是否已经结过婚
    if self.data:GetHusbandOrWifeUid() ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.Love.LoveHadMarryTip)
        return
    end
    local team_list = TeamCtrl.Instance:Data():GetMemberRoleList()
    local info = team_list[#team_list].info or {}
    -- 判断等级
    if RoleLevel() < self.data:GetMarryLevel() or info.level < self.data:GetMarryLevel() then
        PublicPopupCtrl.Instance:Center(Format(Language.Love.LoveMarryLevelTip,self.data:GetMarryLevel()))
        return
    end	
    ViewMgr:OpenView(MarryLetterView,{info = team_list[#team_list].info or {}})
end

function MarryCtrl:IsHasTeam()
	local team_list = TeamCtrl.Instance:Data():GetMemberRoleList()
	--判断是否组队
	if not TeamCtrl.Instance:Data():InTeam() then
		PublicPopupCtrl.Instance:Center(ErrorText[56])
		return false
	end
	--判断是否是队长
	if not TeamCtrl.Instance:Data():IsLeader() then
		PublicPopupCtrl.Instance:Center(ErrorText[57])
		return false
	end
	--判断人数
	if #team_list ~= 2 then
		PublicPopupCtrl.Instance:Center(Language.Love.LoveNotCanMarryTip)
		return false
	else
		-- local info = team_list[#team_list].info or {}
		-- if info.is_partner == 1 then
		-- 	PublicPopupCtrl.Instance:Center(Language.Love.LoveNotCanPartner)
		-- 	return false
		-- end
	end
	return true
end

--结婚流程
function MarryCtrl:OnMarriageProcess()
    ViewMgr:OpenView(MarryIntroduceView)
end

--申请离婚
function MarryCtrl:OnMarriageDivorce()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.Marry.WeddingCrossTips,function()
            LoginCtrl.Instance:SendCrossStartReq()
        end)
        return
    end
    local cooling_off_time = self.data:GetCoolingOffTime()
    if cooling_off_time == 0 or (cooling_off_time - TimeHelper:GetServerTime() <= 0) then
        ViewMgr:OpenView(MarryRelieveSelectView)
    else
        ViewMgr:OpenView(MarryQuitView,{time_stamp = cooling_off_time})
    end
end

--婚宴邀请
function MarryCtrl:OnMarriageInvation()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Marry.WeddingCrossTips,
            confirm = {func = function()
                ViewMgr:CloseView(DialogTipsView)
                LoginCtrl.Instance:SendCrossStartReq()
            end}
        }
    else
        ViewMgr:OpenView(MarryInviteView)
    end
end

--婚宴预约
function MarryCtrl:OnMarriageWedding()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Marry.WeddingCrossTips,
            confirm = {func = function()
                ViewMgr:CloseView(DialogTipsView)
                LoginCtrl.Instance:SendCrossStartReq()
            end}
        }
    else
        ViewMgr:OpenView(MarryOrderView)
    end
end

--进入副本
function MarryCtrl:OnMarriageEnterFB()
    local info = self.data.marry_invite_info.info
    if info.init then return end
    --婚宴未开启时飘字提示
    if TimeHelper.GetServerTime() < info.wedding_begin_time then
        PublicPopupCtrl.Instance:Center(Language.Marry.WeddingTimeTips)
        return
    end
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Marry.WeddingCrossTips,
            confirm = {func = function()
                ViewMgr:CloseView(DialogTipsView)
                LoginCtrl.Instance:SendCrossStartReq()
            end}
        }
    else
        if TeamData.Instance:InTeam() then
            PublicPopupCtrl.Instance:DialogTips{
                content = Language.Marry.WeddingTeamTips,
                confirm = {func = function()
                    TeamCtrl.Instance:SendExitTeam()
                    ViewMgr:CloseView(DialogTipsView)
                    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.WEDDING_FB, info.wedding_id)
                end}
            }
        else
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.WEDDING_FB, info.wedding_id)
        end
    end
end

----------------------------离婚----------------------------
function MarryCtrl:OnMarryDivorceRoute(protocol)
    ViewMgr:OpenView(MarryPopupView,{
        title_name = Language.Marry.TitleNameList[7],
        content = Format(Language.Marry.ErrorText[7],self.data:GetHusbandOrWifeName()),
        cancel_func = function()
            self:SendMarryReq(MarryConfig.REQUEST_TYPE.DIVORCE_RESPOND,0)
        end,
        confirm_func = function()
            ViewMgr:OpenView(MarryRelieveComfirmView,{confirm_func = function()
                self:SendMarryReq(MarryConfig.REQUEST_TYPE.DIVORCE_RESPOND,1)
                PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[17])
            end})
        end
    })
end

--离婚类型
function MarryCtrl:OnMarriageRelieve(relieve_type)
    if relieve_type == 0 then
        if not self:IsHasTeam() then
            return
        end
        ViewMgr:OpenView(MarryRelieveComfirmView,{confirm_func = function()
            self:SendMarryReq(MarryConfig.REQUEST_TYPE.DIVORCE)
            PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[18])
        end})
    elseif relieve_type == 1 then
        ViewMgr:OpenView(MarryRelieveComfirmView,{confirm_func = function()
            local force_divorce_time = Config.get_married_auto.other[1].force_divorce_time
            local info = SocietyData.Instance:GetFriendInfoByUid(self.data:GetHusbandOrWifeUid())
            if info and TimeCtrl.Instance:GetDataDayNum(TimeHelper:GetServerTime(),info.last_online_timestamp) >= force_divorce_time then
                self:SendMarryReq(MarryConfig.REQUEST_TYPE.SINGLE_DIVORCE)
                PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[17])
            else
                PublicPopupCtrl.Instance:Center(Format(Language.Marry.ErrorText[21],force_divorce_time))
            end
        end})
    else
        local divorce_use_num = Config.get_married_auto.other[1].divorce_use
        ViewMgr:OpenView(MarryPopupView,{
            title_name = Language.Marry.TitleNameList[8],
            content = Format(Language.Marry.ErrorText[8],divorce_use_num),
            confirm_func = function()
                ViewMgr:OpenView(MarryRelieveComfirmView,{confirm_func = function()
                    if MallCtrl.IsNotCurrency(CurrencyType.Gold,divorce_use_num) then
                        self:SendMarryReq(MarryConfig.REQUEST_TYPE.FORCE_DIVORCE)
                        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[17])
                    end
                end})
            end
        })
    end
end

function MarryCtrl:CheckRemind()
    return self.data:GetRemindNum()
end

function MarryCtrl:PlayStory()
    if not self.data:IsPlayStory() then
        self:SendWeddingFBReqRite()
        if not self.data.IsShowStory then
            self.data.IsShowStory = true
            StoryData.Instance:StartStoryById(109)
        end
    end
end

function MarryCtrl:PlayPuzzle(state)
    local npc_list = {}
    local cfg = Config.get_married_auto.heart_heart[1]
    for i = 1, 2 do
        local npc_id = cfg["npc_id" .. i]
        if npc_id then
            table.insert(npc_list, npc_id)
        end
    end
    if state == 1 then
        -- 开始
        for i,v in pairs(npc_list) do
            local v_m = SceneCtrl.Instance:VoMgr()
            local count = not v_m.npc_vo_list[v] and 0 or v_m.npc_vo_list[v].count or 0
            if count == 0 then
                SceneCtrl.Instance:AddNpc(v)
            end
        end
    elseif state == 2 then
        -- 结束
        for i,v in pairs(npc_list) do
            local v_m = SceneCtrl.Instance:VoMgr()
            local count = not v_m.npc_vo_list[v] and 0 or v_m.npc_vo_list[v].count or 0
            for i = 1,count do
                SceneCtrl.Instance:DeleteNpc(v)
            end
        end
    end
end

function MarryCtrl:PlayAnswer(state)
    local npc_list = {}
    local cfg = Config.get_married_auto.answer_questions
    for k,v in pairs(cfg) do 
        table.insert(npc_list, v.npc_id)
    end 
    if state == 1 then
        -- 开始
        for i,v in pairs(npc_list) do
            SceneCtrl.Instance:AddNpc(v)
        end
    elseif state == 2 then
        -- 结束
        for i,v in pairs(npc_list) do
            SceneCtrl.Instance:DeleteNpc(v)
        end
    end
end

function MarryCtrl:CreateWeddingBaoWeiXiuQiuMonster()
    if MarryData.Instance:ConfigOtherSceneId() == SceneData.Instance:SceneId() then
        local patrol_monster_list = MarryData.Instance:GetBaoWeiXiuQiuMonsterList()
        for k, v in pairs(patrol_monster_list) do
            local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.WeddingBaoWeiXiuQiuMonster, k)
            local vo = CommonStruct.NpcVo(v)
            local is_show = self.data:GetBaoWeiXiuQiuMonsterShow(vo.id)
            if is_show then
                vo.obj_type = SceneObjType.WeddingBaoWeiXiuQiuMonster
                vo.obj_id = obj_id
                vo.res_id = v.res_id
                vo.move_speed = v.move_speed
                vo.stop_time = v.stop_time
                vo.alway_show = true
                VoMgr:AddGCObj(vo)
            else
                VoMgr:DelGCObj(obj_id)
            end
        end
    end
end
function MarryCtrl:CreateWeddingLookLove(protocol)
    if SceneData.Instance:SceneId() == Config.get_married_auto.other[1].scene_id then
        local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.WeddingLookLove, 1)
        if protocol and protocol.flag == 0 then
            local vo = CommonStruct.NpcVo(protocol)
            vo.obj_id = obj_id
            vo.obj_type = SceneObjType.WeddingLookLove
            vo.is_server = true
            vo.alway_show = true
            vo.scene_id = Config.get_married_auto.other[1].scene_id
            VoMgr:AddGCObj(vo)
        else
            VoMgr:DelGCObj(obj_id, true)
        end
    end
end

function MarryCtrl:OnMarryLeaveNotice(protocol)
    if self.data.marry_id == protocol.marry_id then
        self.data:ClearData()
        ViewMgr:CloseView(MarryQuitView)
    end
end