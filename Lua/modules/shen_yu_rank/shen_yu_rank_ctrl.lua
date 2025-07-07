FILE_REQ("modules/shen_yu_rank/shen_yu_rank_data")
VIEW_REQ("modules/shen_yu_rank/shen_yu_rank_view")
VIEW_REQ("modules/shen_yu_rank/shen_yu_rank_reward_view")

ShenYuRankCtrl = ShenYuRankCtrl or BaseClass(BaseCtrl)
function ShenYuRankCtrl:__init()
    if ShenYuRankCtrl.Instance ~= nil then
        Debuger.LogError("[ShenYuRankCtrl] attempt to create singleton twice!")
        return
    end
    ShenYuRankCtrl.Instance = self
    self.data = ShenYuRankData.New()
    self:RegisterAllProtocol()
end

function ShenYuRankCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCGodSpaceFactionPersonalRankList, "OnSCGodSpaceFactionPersonalRankList")
    self:RegisterProtocol(SCGodSpaceAllFactionRankList, "OnSCGodSpaceAllFactionRankList")

end

function ShenYuRankCtrl:OnSCGodSpaceFactionPersonalRankList(protocol)
    self.data:OnRankDataChange(protocol, protocol.rank_target)
end

function ShenYuRankCtrl:OnSCGodSpaceAllFactionRankList(protocol)
    self.data:OnRankDataChange(protocol, GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION)
end

function ShenYuRankCtrl:ReqRank()
    local param_t = {}
    param_t.oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_QUERY_RANK
    param_t.param_1 = self.data.rank_sel_data.cur_select_target_type
    param_t.param_2 = self.data.rank_sel_data.cur_select_rank_type
    LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end


function ShenYuRankCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCGodSpaceFactionPersonalRankList, "OnSCGodSpaceFactionPersonalRankList")
    self:UnRegisterProtocol(SCGodSpaceAllFactionRankList, "OnSCGodSpaceAllFactionRankList")
end


function ShenYuRankCtrl:__delete()
    self:UnRegisterAllProtocols()
    ShenYuRankCtrl.Instance = nil
end

function ShenYuRankCtrl:OnUnloadGameLogic()
self.data:ClearData() 
end

