VIEW_REQ("modules/serveractivity/yxdm/yxdm_view")
VIEW_REQ("modules/serveractivity/yxdm/yxdm_cm_view")
VIEW_REQ("modules/serveractivity/yxdm/yxdm_reward_view")

YxdmCtrl = YxdmCtrl or BaseClass(BaseCtrl)

function YxdmCtrl:__init()
    if YxdmCtrl.Instance ~= nil then
        Debuger.LogError("[YxdmCtrl] attempt to create singleton twice!")
        return
    end
    YxdmCtrl.Instance = self
    self.data = YxdmData.New() -- 月夕灯谜 活动数据
    self.answerResdata = YxdmAnSwerResData.New() -- 月夕灯谜 回答结果
    self:RegisterRemind()
    self:RegisterAllProtocols()
end

function YxdmCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.Remind, self)
    RemindCtrl.Instance:Remind():Register(Mod.MidAutumnFestival.Yxdm, self.data.yxdm_data, remind_func)
end

function YxdmCtrl:__delete()
    self:UnRegisterAllProtocols()
    YxdmCtrl.Instance = nil
end

function YxdmCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSRAYueXiDengMReq)
    self:RegisterProtocol(SCRAYueXiDengMiInfo, "OnSCRAYueXiDengMiInfo")
    self:RegisterProtocol(SCRAYueXiDengMiAnswerResult, "OnSCRAYueXiDengMiAnswerResult")
end

function YxdmCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSRAYueXiDengMReq)
    self:UnRegisterProtocol(SCRAYueXiDengMiInfo)
    self:UnRegisterProtocol(SCRAYueXiDengMiAnswerResult)

end

function YxdmCtrl:Remind()
    local num = self.data:Remind()
    return num
end

function YxdmCtrl:OnSCRAYueXiDengMiInfo(protocol)
    self.data:SetInfo(protocol)
end

function YxdmCtrl:OnSCRAYueXiDengMiAnswerResult(protocol)
    self.answerResdata:SetInfo(protocol)
end

function YxdmCtrl:SendReq(type, user_answer)
    local protocol = ProtocolPool.Instance:GetProtocol(CSRAYueXiDengMReq)
    protocol.op_type = type or YxdmReqType.Info
    protocol.user_answer = user_answer or ""
    SendProtocol(protocol)
end

-- ===========================Data===========================
YxdmData = YxdmData or BaseClass()

-- 月夕谜灯操作请求类型
YxdmReqType = {
    Info = 0, -- 请求信息
    Start = 1, -- 请求开始答题
    Answer = 2 -- 答题 user_answer:玩家答案
}

function YxdmData:__init()
    if YxdmData.Instance ~= nil then
        Debuger.LogError("[YxdmData] attempt to create singleton twice!")
        return
    end
    YxdmData.Instance = self
    self.cfg = Config.yuexidengmi_auto
    self.cfg_other = self.cfg.other[1]
    self.today_max_answer = self.cfg_other.day_answer_up
    self.yxdm_data = SmartData.New()
end

function YxdmData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI) or self.yxdm_data.info==nil then
        return 0
    end
    return self.yxdm_data.info.today_answer_count < self.today_max_answer and 1 or 0
end

function YxdmData:SetInfo(protocol)
    self.yxdm_data.info = protocol.info
end

function YxdmData:GetShowReward()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI, self.cfg.show_reward) or
                    {}
    return DataHelper.FormatItemList(cfg[1].show_reward_item)
end
-- ===========================Data===========================
YxdmAnSwerResData = YxdmAnSwerResData or BaseClass()
function YxdmAnSwerResData:__init()
    if YxdmAnSwerResData.Instance ~= nil then
        Debuger.LogError("[YxdmAnSwerResData] attempt to create singleton twice!")
        return
    end
    YxdmAnSwerResData.Instance = self
    self.answerRes_data = SmartData.New()
end

function YxdmAnSwerResData:SetInfo(protocol)
    self.answerRes_data.info = protocol.info
end

