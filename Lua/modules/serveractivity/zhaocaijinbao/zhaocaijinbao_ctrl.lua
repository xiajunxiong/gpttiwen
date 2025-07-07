FILE_REQ("modules/serveractivity/zhaocaijinbao/zhaocaijinbao_data")

VIEW_REQ("modules/serveractivity/zhaocaijinbao/zhaocaijinbao_view")
ActZhaoCaiJinBaoCtrl = ActZhaoCaiJinBaoCtrl or BaseClass(BaseCtrl)

ActZhaoCaiJinBaoCtrl.REQ_TYPE = {
    REQ_INFO = 0,                   -- 0 请求数据信息
    REQ_DRAW = 1,                   -- 1 抽奖
    REQ_REFRESH = 2,                -- 2 元宝刷新奖池
    REQ_BUY_ITEM_DIRECT = 3,        -- 3 道具购买 非直购 param_1: 商城seq
    REQ_BUY_ITEM_SHOP = 4,          -- 4 道具购买 直购 param_1: 商城seq
    REQ_GET_CHONGZHI_REWARD = 5,    -- 5 领取充值奖励 param_1: 奖励seq
    REQ_GET_RECORD = 6,             -- 6 申请抽奖记录
}

function ActZhaoCaiJinBaoCtrl:__init()
    if ActZhaoCaiJinBaoCtrl.Instance ~= nil then
        Debuger.LogError("[ActZhaoCaiJinBaoCtrl] attempt to create singleton twice!")
        return
    end
    ActZhaoCaiJinBaoCtrl.Instance = self
    self.data = ActZhaoCaiJinBaoData.New()

    self:RegisterProtocol(SCRAZhaoCaiJinBaoRoleInfo, "OnSCRAZhaoCaiJinBaoRoleInfo")
    self:RegisterProtocol(SCRAZhaoCaiJinBaoDrawResult, "OnSCRAZhaoCaiJinBaoDrawResult")
    self:RegisterProtocol(SCRAZhaoCaiJinBaoRecordListInfo, "OnSCRAZhaoCaiJinBaoRecordListInfo")
    
end

function ActZhaoCaiJinBaoCtrl:OnInit()
    self.data:initConfig()
    Remind.Instance:Register(Mod.RandActivity.ZhaoCaiJinBao, self.data.zhaocai_smart, function()
		return self.data:GetRewardFlag()
	end,"reward_flag")
end

-- 0 请求数据信息
-- 1 抽奖           param_1: 抽奖次数
-- 2 元宝刷新奖池
-- 3 道具购买 非直购 param_1: 商城seq
-- 4 道具购买 直购 param_1: 商城seq
-- 5 领取充值奖励 param_1: 奖励seq
-- 6 请求抽奖记录
function ActZhaoCaiJinBaoCtrl:SendZhaoCaiJinBaoReq(req_type,param)
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO, opera_type = req_type, param_1 = param or 0})
end

function ActZhaoCaiJinBaoCtrl:OnSCRAZhaoCaiJinBaoRoleInfo(protocol)
    -- LogError("OnSCRAZhaoCaiJinBaoRoleInfo",protocol)
    self.data:SetZhaoCaiInfo(protocol)
end

function ActZhaoCaiJinBaoCtrl:OnSCRAZhaoCaiJinBaoDrawResult(protocol)
    self.data:SetDrawResult(protocol)
    if #protocol.seq_list > 1 then
        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,self.data:GetDrawResultItemList())
    end
end

function ActZhaoCaiJinBaoCtrl:OnSCRAZhaoCaiJinBaoRecordListInfo(protocol)
    self.data:SetRecordList(protocol)
    if self.open_record_view then
        self.open_record_view = false
        ViewMgr:OpenView(ActZhaoCaiJinBaoRecordView)
    end
end