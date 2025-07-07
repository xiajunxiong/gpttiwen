FILE_REQ("modules/fabao_lottery/fabao_lottery_data")
VIEW_REQ("modules/fabao_lottery/fabao_lottery")
VIEW_REQ("modules/fabao_lottery/fabao_filler")
VIEW_REQ("modules/fabao_lottery/fabao_cost")
VIEW_REQ("modules/fabao_lottery/fabao_rate")
VIEW_REQ("modules/fabao_lottery/fabao_detail")

FabaoLotteryCtrl = FabaoLotteryCtrl or BaseClass(BaseCtrl)
function FabaoLotteryCtrl:__init()
    if FabaoLotteryCtrl.Instance ~= nil then
        Debuger.LogError("[FabaoLotteryCtrl] attempt to create singleton twice!")
        return
    end
    FabaoLotteryCtrl.Instance = self
    self.data = FabaoLotteryData.New()

    self:RegisterProtocol(SCFabaoDrawInfo, "SCFabaoDrawInfo")
    self:RegisterProtocol(SCFabaoDrawRewardInfo, "SCFabaoDrawRewardInfo")

    RemindCtrl.Instance:Remind():Register(
        Mod.Fabao.Main,
        self.data:DrawInfo(),
        BindTool.Bind(FabaoLotteryData.RemindDrawInfo, self.data)
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Fabao.Main,
        BagData.Instance.item_grid,
        BindTool.Bind(FabaoLotteryData.RemindBagData, self.data)
    )
end

function FabaoLotteryCtrl:__delete()
    -- body
end

function FabaoLotteryCtrl:OnUnloadGameLogic()
    self.data:Clear()
end

function FabaoLotteryCtrl:SCFabaoDrawInfo(protocol)
    -- LogDG("法宝抽奖信息", protocol)
    self.data:SetDrawInfo(protocol.item_info)
end
function FabaoLotteryCtrl:SCFabaoDrawRewardInfo(protocol)
    -- LogDG("法宝奖励信息", protocol)
    self.data:SetRewardInfo(protocol)
    -- faobao_id == -1 就是没有获得
    -- item_id = 0就是空
end
function FabaoLotteryCtrl:SendFabaoReq(param_t)
    local protocol = GetProtocol(CSFabaoReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    protocol.param3 = param_t.param3 or 0
    protocol.param4 = param_t.param4 or 0
    protocol.param5 = param_t.param5 or 0
    protocol.param6 = param_t.param6 or 0
    SendProtocol(protocol)
end
function FabaoLotteryCtrl.DrawMaterial(pos, p2, p3, p4, p5, p6)
    FabaoLotteryCtrl.Instance:SendFabaoReq(
        {
            req_type = FABAO_REQ_TYPE.DRAW_MATERIAL,
            param1 = pos,
            param2 = p2,
            param3 = p3,
            param4 = p4,
            param5 = p5,
            param6 = p6
        }
    )
end
function FabaoLotteryCtrl.DrawSpeedUp(pos, count)
    FabaoLotteryCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.DRAW_SPEED_UP, param1 = pos, param2 = count})
end
function FabaoLotteryCtrl.Draw(pos)
    FabaoLotteryCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.DRAW, param1 = pos})
end
