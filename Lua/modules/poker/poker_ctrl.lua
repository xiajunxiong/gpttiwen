FILE_REQ("modules/poker/poker_info")
VIEW_REQ("modules/poker/poker_view")
VIEW_REQ("modules/poker/poker_false_view")

PokerConfig = {--请求类型
    DrawReqType = {
        TYPE_ROLE_DRAW = 0 --领取，p1:奖励组id p2:选择的位置
    }
}

PokerCtrl = PokerCtrl or BaseClass(BaseCtrl)
function PokerCtrl:__init()
    if PokerCtrl.Instance ~= nil then
        Debuger.LogError("[PokerCtrl] attempt to create singleton twice!")
        return
    end
    PokerCtrl.Instance = self
    self.data = PokerData.New()
    self:RegisterAllProtocols()
end

function PokerCtrl:__delete()
    PokerCtrl.Instance = nil
    self:UnRegisterAllProtocols()
end

function PokerCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRoleDrawInfo, "OnSCRoleDrawInfo", true)
    self:RegisterProtocol(SCDrawPokerInfo, "OnDrawPokerInfo")
    self:RegisterProtocol(CSDrawReq)
end

function PokerCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRoleDrawInfo)
    self:UnRegisterProtocol(CSDrawReq)
end

--建议使用 OnOpenPokerView(翻牌列表mypoker,type)
function PokerCtrl:OpenPokerView(type)
    if ViewMgr:IsOpen(PokerView) then
        return
    end
    self.data:SetPokerType(type)
    self.data:CreatePokerInfo()
    ViewMgr:OpenView(PokerView)
end

function PokerCtrl:OnOpenPokerView(list,type)
    if self.data:GetRoleInfo(list) then
        self:OpenPokerView(type)
    end
end

--建议使用 OpenPokerFalseView(翻牌列表mypoker,type)
function PokerCtrl:OpenPokerFalseView(type)
    if ViewMgr:IsOpen(PokerFalseView) then
        return
    end
    self.data:SetPokerType(type)
    self.data:CreatePokerInfo()
    ViewMgr:OpenView(PokerFalseView)
end

function PokerCtrl:OnOpenPokerFalseView(list,type)
    if self.data:GetRoleInfo(list) then
        self:OpenPokerFalseView(type)
    end
end


function PokerCtrl:OnSCRoleDrawInfo(protocol)
    ConspiracyCtrl.Instance:OnMiYiPokerChange(protocol)
    FightFieldBossCtrl.Instance:OnWildBossPokerChange(protocol)
    GhostNightCtrl.Instance:OnGhostNightPokerChange(protocol)
    self.data:SetSCRoleDrawInfo(protocol)
end

function PokerCtrl:OnDrawPokerInfo(protocol)
    local info = protocol.info
	self.data:FinishReward(info)
    self:OnOpenPokerView(info.mypoker,info.draw_group_type)
end

function PokerCtrl:SendDrawReq(type, param1, param2)
    local protocol = GetProtocol(CSDrawReq)
    protocol.type = type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

-- p1:奖励组id p2:选择的位置
function PokerCtrl:SendPokerReq(group_id, index)
    self:SendDrawReq(PokerConfig.DrawReqType.TYPE_ROLE_DRAW, group_id, index)
end

PokerData = PokerData or BaseClass()
function PokerData:__init()
    if PokerData.Instance ~= nil then
        Debuger.LogError("[PokerData] attempt to create singleton twice!")
        return
    end
    PokerData.Instance = self

    self.poker_type = 0
    self.poker_info = nil

    self.poker_info_data = SmartData.New()
end

function PokerData:__delete()
    PokerData.Instance = nil
    self:RemovePokerInfo()
end

function PokerData:PokerType()
    return self.poker_type
end

function PokerData:SetPokerType(poker_type)
    self.poker_type = poker_type
end

function PokerData:PokerInfo()
    return self.poker_info
end

function PokerData:RemovePokerInfo()
    if self.poker_info ~= nil then
        self.poker_info:CloseCall()
        self.poker_info:DeleteMe()
        self.poker_info = nil
    end
end

function PokerData:CreatePokerInfo()
    if self.poker_info ~= nil then
        self:RemovePokerInfo()
    end
    self.poker_info = PokerInfo.Create(self.poker_type)
end

function PokerData:FinishReward(params)
    self.poker_info_data:Set(params)
end

function PokerData:GetPokerInfo()
    return self.poker_info_data
end

--建议使用 GetMypokerByrIndex(info,protocol)
function PokerData:SetSCRoleDrawInfo(protocol)
    self:GetMypokerByrIndex(self:GetPokerInfo(),protocol)
end

function PokerData:GetRoleInfo(list)
    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(list or {}) do
        if v.role_uid == role_id and v.index == -1 then
            return v
        end
    end
end

--替换index？
function PokerData:GetMypokerByrIndex(info,protocol)
    if info.mypoker and info.draw_group_id == protocol.draw_group_id then
        for k,v in pairs(info.mypoker or {}) do
            if v.role_uid == protocol.role_uid and v.index == -1 then
                v.index = protocol.draw_index
                return protocol.draw_index
            end
        end
    end
end
function PokerData:GetNormalPoker(type, item_list)
    local info = {}
    info.draw_group_id = -1
    info.timeout = 10
    info.draw_group_type = type
    info.mypoker = {}
    for i = 1, 5 do
        info.mypoker[i] = {}
        info.mypoker[i].index = -1
        info.mypoker[i].role_uid = RoleData.Instance:GetRoleId()
        info.mypoker[i].role_name = ""
        info.mypoker[i].item_info = item_list[i] or item_list[1]
        -- info.mypoker[i].item_info.item_id = CommonItemId.QiangHuaShi
        -- info.mypoker[i].item_info.num = 5
        -- info.mypoker[i].item_info.is_bind = 1
    end
    return info
end