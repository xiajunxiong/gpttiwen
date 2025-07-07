FILE_REQ('modules/serveractivity/act_zysc/act_zysc_data')
VIEW_REQ('modules/serveractivity/act_zysc/act_zysc_view')
VIEW_REQ('modules/serveractivity/act_zysc/zysc_cjjl_view')
VIEW_REQ('modules/serveractivity/act_zysc/zysc_reward_view')

RA_ZhuiYueShangCheng = {
    ['ZYSC_MAX_BUY_ITEM'] = 20, ['ZYSC_SELECT_NUM'] = 6, ['ZYSC_DRAW_RECORD_LIST_NUM'] = 50, -- 追月商城抽奖记录长度
    ['ZYSC_DRAW_REWARD_LIST_NUM'] = 10, -- 追月商城抽奖奖励个数
}
-- RA_ZYSC_OP_TYPE = {
--     REQ = 0, -- 请求数据
--     BUY = 1, -- 直购奖励
--     BUYBYMONEY = 2, -- 花钱购买
--     SELECT = 3, -- 选择奖励
--     DRWARD = 4, -- 抽奖
-- }
ActZyscCtrl = ActZyscCtrl or BaseClass(BaseCtrl)
function ActZyscCtrl:__init()
    if ActZyscCtrl.Instance ~= nil then
        Debuger.LogError('[ActZyscCtrl] attempt to create singleton twice!')
        return
    end
    ActZyscCtrl.Instance = self
    self.data = ActZyscData.New()
    self:RegisterAllProtocols()
    self:RegisterRemind()

    self.ToggleSkip = false -- 是否跳过抽奖
    self.ID_QiongQi = 50237
end

function ActZyscCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

-- 红点
function ActZyscCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.Remind, self)
    RemindCtrl.Instance:Remind():Register(Mod.MidAutumnFestival.Zysc, self.data.SD_ZqzyInfo, remind_func)
end

function ActZyscCtrl:OnInit()
    self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SetRecommendSuit, self))
end

function ActZyscCtrl:SetRecommendSuit()
    -- local config = self.data:GetSC_ZqzyInfo_Data()
    self.bg_handle = BagData.Instance:ListenItems({[1] = CommonItemId.RabbitCoinId}, BindTool.Bind(self.FlushCurr, self))
end

function ActZyscCtrl:Remind()
    return self.data:Remind()
end

function ActZyscCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActZyscCtrl.Instance = nil
end

function ActZyscCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAZhuiYueShangDianInfo, 'onZyscInfo')
    self:RegisterProtocol(SCRAZhuiYueShangDianDrawRecord, 'onZyscCjjlInfo')
    self:RegisterProtocol(SCRAZhuiYueShangDianDrawRet, 'OnZyscDrawRet')
end

function ActZyscCtrl:onZyscInfo(protocol)
    self.data:On_SC_ZyscInfo(protocol)
end

function ActZyscCtrl:onZyscCjjlInfo(protocol)
    self.data:Sc_ZyscCjjlInfo(protocol)
end

function ActZyscCtrl:OnZyscDrawRet(protocol)
    self.data:OnSc_ZyscDrawRet(protocol)
end

function ActZyscCtrl:FlushCurr()
    self.data:onFlushCurr()
end
function ActZyscCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRAZhuiYueShangDianInfo, 'onZyscInfo')
    self:UnRegisterProtocol(SCRAZhuiYueShangDianDrawRecord, 'onZyscCjjlInfo')
    self:UnRegisterProtocol(SCRAZhuiYueShangDianDrawRet, 'OnZyscDrawRet')
end

-- 设置选中奖励seq 长度限制 RA_ZhuiYueShangCheng.ZYSC_SELECT_NUM = 6
-- @parma seq 表里奖励项 seq
-- @param pos 选中的位置
-- @return false:无法选择
function ActZyscCtrl:Set_RewardSeq(seq, isOn)
    local seq_save = self.data.data_Reward_Sl_Seq_Save
    if isOn then
        if self.data.SD_ZqzyInfo.data_Reward_Sl_length >= RA_ZhuiYueShangCheng.ZYSC_SELECT_NUM then
            PublicPopupCtrl.Instance:Center(Language.ActZysc.tip3)
            return false
        end
        table.insert(seq_save, seq)
        self.data.SD_ZqzyInfo.data_Reward_Sl_length = self.data.SD_ZqzyInfo.data_Reward_Sl_length + 1
        return true
    else
        for k, v in pairs(seq_save) do
            if seq == v then
                table.remove(seq_save, k)
                self.data.SD_ZqzyInfo.data_Reward_Sl_length = self.data.SD_ZqzyInfo.data_Reward_Sl_length - 1
            end
        end
        return false
    end
end

function ActZyscCtrl:Init_RewardSeq()
    self.data.data_Reward_Sl_Seq_Save = {}
end

function ActZyscCtrl:Clean_RewardSeq()
    self.data.data_Reward_Sl_Seq_Save = nil
    self.data.SD_ZqzyInfo.data_Reward_Sl_length = 0
end

---通过seq获取仲秋追月奖励配置
---@param begin 默认从0开始读 
function ActZyscCtrl:Get_ZqzyRewardBySeq(seq, begin)
    local data_reward = self.data:GetSC_ZqzyReward_Data()
    if begin == nil then
        return data_reward[seq]
    end
    return data_reward[seq + begin]
end

-- 请求保存奖励 必须选满
function ActZyscCtrl:REQ_SaveRewardSeq()
    local length = self.data.SD_ZqzyInfo.data_Reward_Sl_length
    if length < RA_ZhuiYueShangCheng.ZYSC_SELECT_NUM then
        PublicPopupCtrl.Instance:Center(Language.ActZysc.tip2)
        return false
    end
    local seq_save = self.data.data_Reward_Sl_Seq_Save
    -- self.data.SC_ZqzyInfo.data_Reward_Sl_Seq = TableCopy(seq_save, 1)
    for k, v in pairs(seq_save) do
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, ZyscReqType.ChooseReward, k - 1, v)
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, ZyscReqType.Info)
    return true
end

function ActZyscCtrl:REQ_DrawReward(twice)
    if twice == nil then
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, ZyscReqType.Draw, twice)
end
