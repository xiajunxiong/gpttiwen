-- @Author: Ghost
-- @Date: 2022-08-24 16:51:52
-- @Last Modified by:   Ghost
-- @Last Modified time: 2022-08-24 16:51:52
FILE_REQ('modules/serveractivity/act_yxlc/act_yxlc_data')
VIEW_REQ('modules/serveractivity/act_yxlc/act_yxlc_view')

ActYxlcCtrl = ActYxlcCtrl or BaseClass(BaseCtrl)
function ActYxlcCtrl:__init()
    if ActYxlcCtrl.Instance ~= nil then
        Debuger.LogError('[ActYxlcCtrl] attempt to create singleton twice!')
        return
    end
    ActYxlcCtrl.Instance = self
    self.data = ActYxlcData.New()
    self:RegisterRemind()
    self:RegisterAllProtocols()
end

function ActYxlcCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActYxlcCtrl.Instance = nil
end

function ActYxlcCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActYxlcCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYueXiLeiChongInfo, 'onYxlcInfo')
end

function ActYxlcCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRAYueXiLeiChongInfo, 'onYxlcInfo')
end

function ActYxlcCtrl:onYxlcInfo(protocol)
    self.data:SC_F_YXLCInfo(protocol)
end

function ActYxlcCtrl:CSReqReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG, 1, seq)
end

-- 是否可以领取奖励
function ActYxlcCtrl:CheckCanRec(money)
    return money <= self.data.SD_YXLCInfo.m_total_chongzhi
end

function ActYxlcCtrl:RegisterRemind()
    RemindCtrl.Instance:Remind():Register(Mod.MidAutumnFestival.Yxlc, self.data.SD_YXLCInfo, BindTool.Bind(self.RemindRec, self))
end

-- 红点
function ActYxlcCtrl:RemindRec()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG) then
        return 0
    end
    return self.data.SD_YXLCInfo.count_Remind or 0
end
