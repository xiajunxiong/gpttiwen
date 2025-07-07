FILE_REQ("modules/marry_party/marry_party_config")
FILE_REQ("modules/marry_party/marry_party_data")

VIEW_REQ("modules/marry_party/marry_party_notice_view")

VIEW_REQ("modules/marry_party/marry_party_menu")
VIEW_REQ("modules/marry_party/marry_party_game_view")
VIEW_REQ("modules/marry_party/marry_party_answer_view")
VIEW_REQ("modules/marry_party/marry_party_fight_view")
VIEW_REQ("modules/marry_party/marry_party_bless_view")

MarryPartyCtrl = MarryPartyCtrl or BaseClass(BaseCtrl)

function MarryPartyCtrl:__init()
	if MarryPartyCtrl.Instance ~= nil then
		Debuger.LogError("[MarryPartyCtrl] attempt to create singleton twice!")
		return
	end
	MarryPartyCtrl.Instance = self
	self.data = MarryPartyData.New()
    self:RegisterAllProtocols()
end

function MarryPartyCtrl:__delete()
    self:UnRegisterAllProtocols()
    MarryPartyCtrl.Instance = nil
end

function MarryPartyCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSQingYuanShengHuiReq)

    self:RegisterProtocol(SCQingYuanShengHuiSceneInfo, "OnQingYuanShengHuiSceneInfo")
    self:RegisterProtocol(SCQingYuanShengHuiRoleInfo, "OnQingYuanShengHuiRoleInfo")
    self:RegisterProtocol(SCQingYuanShengHuiQuestionResult, "OnQingYuanShengHuiQuestionResult")
    self:RegisterProtocol(SCQingYuanShengHuiQuestionRoute, "OnQingYuanShengHuiQuestionRoute")
    self:RegisterProtocol(SCQingYuanShengHuiQuestionInfo, "OnQingYuanShengHuiQuestionInfo")
    self:RegisterProtocol(SCQingYuanShengHuiOpponentInfo, "OnQingYuanShengHuiOpponentInfo")
    self:RegisterProtocol(SCQingYuanShengHuiMatchOpRoute, "OnQingYuanShengHuiMatchOpRoute")
    self:RegisterProtocol(SCQingYuanShengHuiInfo, "OnQingYuanShengHuiInfo")

    ActivityData.Instance:CustomClickHandle(ActType.MarryParty, function()
		SceneCtrl.Instance:RequestEnterFb(FB_TYPE.MARRY_PARTY)
	end)
end

function MarryPartyCtrl:OnInit()
end

function MarryPartyCtrl:UnRegisterAllProtocols()
end

function MarryPartyCtrl:OnQingYuanShengHuiSceneInfo(protocol)
    self.data:SetQingYuanShengHuiSceneInfo(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiRoleInfo(protocol)
    self.data:SetQingYuanShengHuiRoleInfo(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiQuestionResult(protocol)
    self.data:SetQingYuanShengHuiQuestionResult(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiQuestionRoute(protocol)
    self.data:SetQingYuanShengHuiQuestionRoute(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiQuestionInfo(protocol)
    self.data:SetQingYuanShengHuiQuestionInfo(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiOpponentInfo(protocol)
    self.data:SetQingYuanShengHuiOpponentInfo(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiMatchOpRoute(protocol)
    self.data:SetQingYuanShengHuiMatchOpRoute(protocol)
end

function MarryPartyCtrl:OnQingYuanShengHuiInfo(protocol)
    self.data:SetQingYuanShengHuiInfo(protocol)
end

function MarryPartyCtrl:SendQingYuanShengHuiReq(param_t)
    local protocol = GetProtocol(CSQingYuanShengHuiReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    SendProtocol(protocol)
end

-- 预约
function MarryPartyCtrl:SendQingYuanShengHuiReqRegist()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.regist,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 进入场景
function MarryPartyCtrl:SendQingYuanShengHuiReqEnter()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.enter,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 采集
function MarryPartyCtrl:SendQingYuanShengHuiGather()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.gather,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 打开答题界面
function MarryPartyCtrl:SendQingYuanShengHuiQuestionOpen()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.question_open,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 答题 p1:选项
function MarryPartyCtrl:SendQingYuanShengHuiQuestionAnswer(index)
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.question_answer,
        param1 = index
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 关闭答题界面
function MarryPartyCtrl:SendQingYuanShengHuiQuestionClose()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.question_close,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 打开匹配界面
function MarryPartyCtrl:SendQingYuanShengHuiMatchOpen()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.match_open,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 关闭匹配界面
function MarryPartyCtrl:SendQingYuanShengHuiMatchClose()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.match_close,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 开始匹配
function MarryPartyCtrl:SendQingYuanShengHuiMatch(is_match)
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.match,
        param1 = is_match and 1 or 0
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 祈愿
function MarryPartyCtrl:SendQingYuanShengHuiBless()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.bless,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 领取战斗奖励
function MarryPartyCtrl:SendQingYuanShengHuiBattleReward(times)
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.battle_reward,
        param1 = times,
    }
    self:SendQingYuanShengHuiReq(param_t)
end

-- 预告信息
function MarryPartyCtrl:SendQingYuanShengHuiOrderInfo()
    local param_t = {
        req_type = MarryPartyConfig.FBReqType.order_info,
    }
    self:SendQingYuanShengHuiReq(param_t)
end