FavorNpcUpView = FavorNpcUpView or DeclareView("FavorNpcUpView", "favor/favor_npc_up_view")
VIEW_DECLARE_MASK(FavorNpcUpView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcUpView, ViewLevel.Lt)

function FavorNpcUpView:FavorNpcUpView()
end

function FavorNpcUpView:LoadCallback(paramt)
    self.Panel:InitData(paramt.old_val)
end


FavorNpcUpPanel = FavorNpcUpPanel or DeclareMono("FavorNpcUpPanel", UIWFlushPanel)

function FavorNpcUpPanel:FavorNpcUpPanel()
    self.data = FavorData.Instance 
end

function FavorNpcUpPanel:InitData(old_val)
    self.old_val = old_val
    self.data_cares = {{data = self.data.favor_data_change, func = self.FlushData, init_call = false,keys={"val"}}}
    self:FlushData()
end

function FavorNpcUpPanel:FlushData()
    local progress_data = self.data:GetNpcProGress()
    local val = progress_data.val
    local target = progress_data.target
    self.ProGress:SetProgress(val / target)
    self.ProGress:SetText(string.format("%s/%s", val, target))
    UH.SetText(self.UpNum, "+" .. (val - self.old_val))
    local level = self.data:GetNpcFavorLevel()
    UH.SetText(self.FavorLevel, level)
    UH.SetText(self.FavorTitle, self.data:GetNpcFavorTitle(level))
end



