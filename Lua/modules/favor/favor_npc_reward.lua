FavorNpcRewardView = FavorNpcRewardView or DeclareView("FavorNpcRewardView", "favor/favor_npc_reward")
VIEW_DECLARE_MASK(FavorNpcRewardView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcRewardView, ViewLevel.Lt)

function FavorNpcRewardView:FavorNpcRewardView()
end

function FavorNpcRewardView:LoadCallback(paramt)
    local cfg_npc = paramt.cfg_npc
    self.Board:SetData(self:GetType(), Language.Favor.reward, Vector2.New(378, 579))
    local favor_info=FavorData.Instance:GetFavorInfo(cfg_npc)
    local list = {}
    for level = 2, 5 do
        local info = {}
        info.level = level
        local reward = cfg_npc["reward_" .. level .. "_list"]
        info.reward = {}
        for k=0,#reward do
            local item_data =Item.Create( reward[k])
            local is_fetch = favor_info.fetch_flag[level-1]==1
            table.insert(info.reward, {item_data = item_data,is_fetch=is_fetch})
        end
        table.insert(list, info)
    end
    self.List:SetData(list)
end


FavorNpcRewardItem = FavorNpcRewardItem or DeclareMono("FavorNpcRewardItem", UIWidgetBaseItem)

function FavorNpcRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local title = FavorData.Instance:GetNpcFavorTitle(data.level)
    UH.SetText(self.Title,title)
    UH.SetText(self.FavorLevel,data.level)
    -- local list={}
    -- for _,v in pairs(data.reward) do
    --     table.insert(list,Item.Create(v))
    -- end
    self.List:SetData(data.reward)
end

FavorNpcRewardCell = FavorNpcRewardCell or DeclareMono("FavorNpcRewardCell", UIWidgetBaseItem)

function FavorNpcRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data.item_data)
    self.ObjFetched:SetActive(data.is_fetch)
end
