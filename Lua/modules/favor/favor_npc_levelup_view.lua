FavorNpcLevelUpView = FavorNpcLevelUpView or DeclareView("FavorNpcLevelUpView", "favor/favor_npc_levelup_view")
VIEW_DECLARE_MASK(FavorNpcLevelUpView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcLevelUpView, ViewLevel.Lt)

function FavorNpcLevelUpView:FavorNpcLevelUpView()
end

function FavorNpcLevelUpView:LoadCallback(paramt)
    self.Panel:InitData(paramt.old_val)
end

FavorNpcLevelUpPanel = FavorNpcLevelUpPanel or DeclareMono("FavorNpcLevelUpPanel", UIWFlushPanel)

function FavorNpcLevelUpPanel:FavorNpcLevelUpPanel()
    self.data = FavorData.Instance
end

function FavorNpcLevelUpPanel:InitData(old_val)
    self.old_val = old_val
    self.data_cares = {{data = self.data.favor_data_change, func = self.FlushData, init_call = false,keys={"val"}}}
    self:FlushData()
end

function FavorNpcLevelUpPanel:FlushData()
    local old_level = self.data:GetNpcFavorLevel(self.old_val)
    UH.SetText(self.PreFavorTitle, self.data:GetNpcFavorTitle(old_level))
    UH.SetText(self.PreFavorLevel, old_level)

    local cur_level = self.data:GetNpcFavorLevel()
    local cur_title = self.data:GetNpcFavorTitle(cur_level)
    UH.SetText(self.CurFavorLevel, cur_level)
    UH.SetText(self.CurFavorTitle, cur_title)

    local progress_data = self.data:GetNpcProGress()
    local val = progress_data.val
    local target = progress_data.target
    self.ProGress:SetProgress(val / target)
    self.ProGress:SetText(string.format("%s/%s", val, target))
    UH.SetText(self.FavorLevel, cur_level)
    UH.SetText(self.FavorTitle, cur_title)

    local cfg = self.data:GetCurFavorNpcConfig()
    local favor_info = self.data:GetFavorInfo()
    local list = {}
    if favor_info then
        for i = 1, cur_level - 1 do
            if favor_info.fetch_flag[i] == 0 then
                FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.FETCH_FAVORE_REWARD, cfg.seq, i - 1)
                local rewards = cfg["reward_" .. (i+1) .. "_list"]
                for k=0,#rewards do
                    table.insert(list, Item.Create(rewards[k]))
                end
            end
        end
    end
    self.ListReward:SetData(list)
end







