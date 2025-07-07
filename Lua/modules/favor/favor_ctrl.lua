FILE_REQ('modules/favor/favor_data')
VIEW_REQ("modules/favor/favor_npc_view")
VIEW_REQ("modules/favor/favor_npc_up_view")
VIEW_REQ("modules/favor/favor_npc_levelup_view")
VIEW_REQ("modules/favor/favor_npc_book")
VIEW_REQ("modules/favor/favor_npc_zone_reward")
VIEW_REQ("modules/favor/favor_npc_feast")
VIEW_REQ("modules/favor/favor_npc_reward")

FAVOR_NPC_OP_TYPE = {
    GIFT = 0,                 --送礼           p1:favor_index  p2:item_id p3:num
    BANQUET = 1,              --宴请           p1:favor_index  p2:宴请seq
    FETCH_FAVORE_REWARD = 2,  --领取奖励        p1:favor_index  p2:reward_seq [0,)
    FETCH_ZONE_REWARD = 3,    --领取区域奖励     p2:reward_seq [0,)
}


FavorCtrl = FavorCtrl or BaseClass(BaseCtrl)
function FavorCtrl:__init()
	if FavorCtrl.Instance ~= nil then
		Debuger.LogError("[FavorCtrl] attempt to create singleton twice!")
		return
	end
	FavorCtrl.Instance = self
	self.data = FavorData.New()
    self:RegisterAllProtocols()
    self:RegisterRemind()
end

function FavorCtrl:__delete()
    self:UnRegisterAllProtocols()
    FavorCtrl.Instance = nil
end

function FavorCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSNpcFavorReq)
    self:RegisterProtocol(SCNpcFavorInfo, "OnNpcFavorInfo")
    self:RegisterProtocol(SCNpcFavorItemInfo, "OnNpcFavorItemInfo")
    self:RegisterProtocol(SCNpcFavorCommonInfo, "OnNpcFavorCommonInfo")
end

function FavorCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCNpcFavorInfo)
    self:UnRegisterProtocol(SCNpcFavorItemInfo)
    self:UnRegisterProtocol(SCNpcFavorCommonInfo)
end

function FavorCtrl:RegisterRemind()
    RemindCtrl.Instance:Remind():Register(Mod.FavorNpc.Main, self.data.favor_data_change, BindTool.Bind(self.RemindZone, self),"val")
end

function FavorCtrl:SendFavorReq(req_type,param1,param2,gift_item_list)
	local protocol = GetProtocol(CSNpcFavorReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.gift_item_list = gift_item_list or {}
	SendProtocol(protocol)
end

function  FavorCtrl:OnNpcFavorInfo(protocol)
    self.data:SetNpcFavorInfo(protocol)
end

function  FavorCtrl:OnNpcFavorItemInfo(protocol)
    self.data:SetNpcFavorItemInfo(protocol)
end

function  FavorCtrl:OnNpcFavorCommonInfo(protocol)
        self.data:SetNpcFavorCommonInfo(protocol)
end

function FavorCtrl:RemindZone()
   return self.data:RemindZone()
end
