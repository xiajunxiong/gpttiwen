FavorNpcZoneRewardView = FavorNpcZoneRewardView or DeclareView("FavorNpcZoneRewardView", "favor/favor_npc_zone_reward")
VIEW_DECLARE_MASK(FavorNpcZoneRewardView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcZoneRewardView, ViewLevel.Lt)

function FavorNpcZoneRewardView:FavorNpcZoneRewardView()
    self.Board:SetData(self:GetType(),Language.Favor.ZoneReward,Vector2.New(531, 381))
end

function FavorNpcZoneRewardView:LoadCallback(paramt)
    self.zone_info=paramt.zone_info
    local zone_info = paramt.zone_info
    UH.SetText(self.Pro,Language.Favor.ZonePro..zone_info.zhiyou_num.."/"..#zone_info.npc_list)
    local list={}
    for _,v in pairs(zone_info.cfg_zone_reward.item) do
        table.insert(list,Item.Create(v))
    end
    self.List:SetData(list)
end

function FavorNpcZoneRewardView:FetchReward()
    if self.zone_info then
        if self.zone_info.zhiyou_num >= #self.zone_info.npc_list then
            FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.FETCH_ZONE_REWARD,self.zone_info.cfg_zone_reward.seq)
            ViewMgr:CloseView(FavorNpcZoneRewardView)
        end
    end
end

