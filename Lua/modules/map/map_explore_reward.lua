MapExploreRewardView = MapExploreRewardView or DeclareView("MapExploreRewardView", "map/map_explore_reward")
VIEW_DECLARE_MASK(MapExploreRewardView,ViewMask.None)
VIEW_DECLARE_LEVEL(MapExploreRewardView, ViewLevel.Lt)
function MapExploreRewardView:MapExploreRewardView()
    self.AudioOpen = CommonAudio.SkillLevelUp
    self.AudioClose = CommonAudio.ViewCloseL1
end
function MapExploreRewardView:LoadCallback(param_t)
    self.param_t = param_t
    self.total_ten_num.gameObject:SetActive(math.floor(self.param_t.total/10) > 0)
    self.cur_ten_num.gameObject:SetActive(math.floor(self.param_t.cur_num/10) > 0)

    UH.SpriteName(self.total_ten_num,"yellow_"..math.floor(self.param_t.total/10))
    UH.SpriteName(self.cur_ten_num,
        self.param_t.is_complete and "green_"..math.floor(self.param_t.cur_num/10) or "orange_"..math.floor(self.param_t.cur_num/10))

    UH.SpriteName(self.total_num,"yellow_"..math.floor(self.param_t.total%10))
    UH.SpriteName(self.cur_num,
        self.param_t.is_complete and "green_"..math.floor(self.param_t.cur_num%10) or "orange_"..math.floor(self.param_t.cur_num%10))
    
    self.list:SetData(self.param_t.reward_list)

    self.EffectTool:Play("4161002")
    self.wait_timer = TimeHelper:AddDelayTimer(BindTool.Bind(self.ShowTop, self), 0.5)
end 

function MapExploreRewardView:ShowTop()
    if self.Top ~= nil then self.Top:SetActive(true) end 
    self.EffectTool:Play("4161001")
    self.wait_timer = nil 
end

function MapExploreRewardView:CloseCallback()
    TimeHelper:CancelTimer(self.wait_timer)
    if self.wait_timer ~= nil then self.wait_timer = nil end 
    if self.param_t.close_func then
        self.param_t.close_func()
    end
end

function MapExploreRewardView:OnClickBlock()
    if self.wait_timer ~= nil then return end 
    ViewMgr:CloseView(MapExploreRewardView)
end
