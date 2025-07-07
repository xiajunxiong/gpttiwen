FILE_REQ("modules/fabao/fabao_data")
VIEW_REQ("modules/fabao/fabao_view")
-- VIEW_REQ("modules/fabao/fabao_neo_view")
VIEW_REQ("modules/fabao/fabao_wash")
VIEW_REQ("modules/fabao/fabao_shop")
VIEW_REQ("modules/fabao/fabao_resolve")
VIEW_REQ("modules/fabao/fabao_view_up")
VIEW_REQ("modules/fabao/fabao_view_pre")
VIEW_REQ("modules/fabao/fabao_jieup_view")
VIEW_REQ("modules/fabao/fabao_fetters")
VIEW_REQ("modules/fabao/fabao_qiling_view") 

FabaoCtrl = FabaoCtrl or BaseClass(BaseCtrl)
FABAO_REQ_TYPE = {
    INFO = 0, --信息
    ACTIVE = 1, --激活法宝 p1 法宝背包对应位置[0-17]
    UPLEVEL = 2, --升级  p1 法宝背包对应位置[0-17] p2 [0,1]0修炼一次，1修炼升级--是否使用物品 [0-2] 0 1次 1 物品 2 10次
    ADVANCED = 3, --进阶  p1 法宝背包对应位置[0-17]
    TAKEUP = 4, --穿戴  p1 法宝背包对应位置[0-3] p2 法宝背包对应位置[0-17]
    TAKEOFF = 5, --脱下  p1 法宝背包对应位置[0-3]
    DECONMPOSE = 6, --分解  p1 法宝背包对应位置[0-17]
    RAND = 7, --洗练  p1 法宝背包对应位置[0-17] p2 对应法宝阶级[0-3]
    EXCHANGE = 8, --兑换  p1 奖励对应seq
    GUIDE_MONSTER = 9,--法宝指引怪
    DRAW_MATERIAL = 10,--设置材料 p1 法宝鼎对应位置 p2~p6 物品id
    DRAW_SPEED_UP = 11,--加速 p1 法宝鼎对应位置 p2 次数
    DRAW = 12,--获取抽奖奖励 p1 法宝鼎对应位置
    QI_LING_UP_LEVEL=13,  -- 器灵升级 p1-器灵列表index （器灵id）
    QI_LING_PUT_ON=14,    -- 器灵穿戴 p1-器灵列表index
    QI_LING_TAKE_OFF=15,  -- 器灵脱下 p1-器灵列表index
}
FABAO_REASON = {
    ACTIVE = 0,
    UPLEVEL = 1,
    ADVANCE = 2,
    DECOMPOSE = 3,
    RANDATTR = 4,
    TAKE_UPOROFF = 5,
}

FA_BAO_QI_LING_PUT_ON_MAX_NUM=3   --法宝器灵御灵上限
FA_BAO_QI_LING_MAX_NUM=14         -- 法宝器灵数量上限

function FabaoCtrl:__init()
    if FabaoCtrl.Instance ~= nil then
        Debuger.LogError("[FabaoCtrl] attempt to create singleton twice!")
        return
    end
    FabaoCtrl.Instance = self
    self.data = FabaoData.New()
    self:RegisterProtocol(SCFabaoInfo, "RecvFabaoInfo")
    self:RegisterProtocol(SCFabaoSignInfo, "RecvFabaoSignInfo")
    self:RegisterProtocol(SCFabaoEquipBarInfo, "RecvFabaoEquipBarInfo")
    self:RegisterProtocol(SCFaBaoQiLingInfo, "RecvFaBaoQiLingInfo")
    self:RegisterProtocol(CSFabaoReq)
    RemindCtrl.Instance:Remind():Register(
        Mod.Fabao.Main,
        BagData.Instance.item_grid,
        BindTool.Bind(FabaoData.RemindBagData, self.data)
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Fabao.Main,
        self.data:GetFabaoList(),
        BindTool.Bind(FabaoData.RemindBagData, self.data)
    )
    RemindCtrl.Instance:Remind():Register(Mod.Fabao.Qiling, self.data.qiling_info, BindTool.Bind(self.RemindQiling, self))
    BagData.Instance:ListenItems(self.data:GetQiLingRemingItemList(), function()
        self.data.qiling_info:Notify()
    end)
end

function FabaoCtrl:__delete()

end
function FabaoCtrl:OnUnloadGameLogic()
    self.data:Clear()
end
function FabaoCtrl:RecvFabaoInfo(protocol)
    -- LogDG("法宝总信息", protocol)
    -- Debuger.LogErrorSD("【法宝】服务端下发总信息" .. Serialize(protocol))
    self.data:SetFabaoBar(protocol.fabao_equip_bar)
    self.data:SetFabaoList(protocol.fabao_list)
    self.data:SetUsedCount(protocol.week_use_coin_times)
end
function FabaoCtrl:RecvFabaoSignInfo(protocol)
    -- LogDG("法宝单条信息变化", protocol)
    if protocol.reason == FABAO_REASON.TAKE_UPOROFF then
        self.data:SetFabaoBar(protocol.fabao_equip_bar)
    else
        self.data:ChangeFabaoList(protocol.fabao_info.fabao_index, protocol.fabao_info)
        self.data:SetUsedCount(protocol.week_use_coin_times)
        if protocol.reason == FABAO_REASON.ACTIVE and not ViewMgr:IsOpen(FabaoLottery) then     --如果不是在法宝抽奖界面激活的法定，则直接显示获得法宝显示 
            ViewMgr:OpenView(FabaoActive, {id = protocol.fabao_info.id})
        end
    end
    -- Debuger.LogErrorSD("【法宝】服务端下发单个信息" .. Serialize(protocol))
end
function FabaoCtrl:RecvFabaoEquipBarInfo(protocol)
    -- LogDG("法宝栏信息变化", protocol)
    self.data:SetFabaoBar(protocol.fabao_bar)
    -- Debuger.LogErrorSD("【法宝】服务端下发法宝栏信息" .. Serialize(protocol))
end

function FabaoCtrl:RecvFaBaoQiLingInfo(protocol)
    self.data:SetQiLingInfo(protocol)
end

function FabaoCtrl:SendFabaoReq(param_t)
    -- LogDG("发送协议",param_t)
    -- Debuger.LogErrorSD(Serialize(param_t))
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
function FabaoCtrl.SendActive(id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.ACTIVE, param1 = id})
end
function FabaoCtrl.SendUpLevel(id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.UPLEVEL, param1 = id})
end
function FabaoCtrl.SendAdvanced(id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.ADVANCED, param1 = id})
end
function FabaoCtrl.SendTakeUp(index, id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.TAKEUP, param1 = index, param2 = id})
end
function FabaoCtrl.SendTakeOff(index)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.TAKEOFF, param1 = index})
end
function FabaoCtrl.SendFenJie(id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.DECONMPOSE, param1 = id})
end
function FabaoCtrl.SendWash(id)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.RAND, param1 = id})
end
function FabaoCtrl.SendExchange(seq)
    FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.EXCHANGE, param1 = seq})
end

function FabaoCtrl:RemindQiling()
    local num =self.data:RemindQiling() 
    return num
end