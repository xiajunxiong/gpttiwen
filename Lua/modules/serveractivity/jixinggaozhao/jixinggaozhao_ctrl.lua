FILE_REQ("modules/serveractivity/jixinggaozhao/jixinggaozhao_data")

VIEW_REQ("modules/serveractivity/jixinggaozhao/jixinggaozhao_view")
ActJiXingGaoZhaoCtrl = ActJiXingGaoZhaoCtrl or BaseClass(BaseCtrl)

ActJiXingGaoZhaoCtrl.REQ_TYPE = {
    REQ_INFO = 0,                   -- 0 请求数据信息
    REQ_DRAW = 1,                   -- 1 抽奖
    REQ_BUY_ITEM_DIRECT = 2,        -- 2 道具购买 非直购 param_1: 商城seq
    REQ_BUY_ITEM_SHOP = 3,          -- 3 道具购买 直购 param_1: 商城seq
    REQ_GET_DRAW_REWARD = 4,        -- 4 领取抽奖次数奖励
}

function ActJiXingGaoZhaoCtrl:__init()
    if ActJiXingGaoZhaoCtrl.Instance ~= nil then
        Debuger.LogError("[ActJiXingGaoZhaoCtrl] attempt to create singleton twice!")
        return
    end
    ActJiXingGaoZhaoCtrl.Instance = self
    self.data = ActJiXingGaoZhaoData.New()

    self:RegisterProtocol(SCRAJiXingGaoZhaoRoleInfo, "OnSCRAJiXingGaoZhaoRoleInfo")
    self:RegisterProtocol(SCRAJiXingGaoZhaoPoolInfo, "OnSCRAJiXingGaoZhaoPoolInfo")
    self:RegisterProtocol(SCRAJiXingGaoZhaoDrawResult, "OnSCRAJiXingGaoZhaoDrawResult")
    self:RegisterProtocol(SCRAJiXingGaoZhaoRecordListInfo, "OnSCRAJiXingGaoZhaoRecordListInfo")
    self:RegisterProtocol(SCRAJiXingGaoZhaoRecordInfo, "OnSCRAJiXingGaoZhaoRecordInfo")
end

function ActJiXingGaoZhaoCtrl:OnInit()
    self.data:initConfig()
end

-- 0 请求数据信息
-- 1 抽奖
-- 2 道具购买 非直购 param_1: 商城seq
-- 3 道具购买 直购 param_1: 商城seq
-- 4 领取抽奖奖励 param_1:
function ActJiXingGaoZhaoCtrl:SendJiXingGaoZhaoReq(req_type,param)
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, opera_type = req_type, param_1 = param or 0})
end

function ActJiXingGaoZhaoCtrl:OnSCRAJiXingGaoZhaoRoleInfo(protocol)
    self.data:SetJiXingInfo(protocol)
end

function ActJiXingGaoZhaoCtrl:OnSCRAJiXingGaoZhaoPoolInfo(protocol)
    self.data:SetPoolInfo(protocol)
end

function ActJiXingGaoZhaoCtrl:OnSCRAJiXingGaoZhaoDrawResult(protocol)
    self.data:SetDrawResult(protocol)
end

function ActJiXingGaoZhaoCtrl:OnSCRAJiXingGaoZhaoRecordListInfo(protocol)
    self.data:SetRecordList(protocol)
end

function ActJiXingGaoZhaoCtrl:OnSCRAJiXingGaoZhaoRecordInfo(protocol)
    self.data:AddRecordInfo(protocol)
end