MapExploreTotalRewardView = MapExploreTotalRewardView or DeclareView("MapExploreTotalRewardView", "map/map_explore_total_reward")
-- VIEW_DECLARE_MASK(MapExploreTotalRewardView,ViewMask.None)
function MapExploreTotalRewardView:MapExploreTotalRewardView()
    self.Board:SetData(self:GetType(),Language.Map.MapExploreRewardTitle,Vector2.New(835, 620))
end

function MapExploreTotalRewardView:LoadCallback(param_t)
    if param_t ~= nil then 
        self.Panel:SetSceneID(param_t.scene_id)
    end 
    -- self.Panel:FlushPanel()
end 

MapExploreTotalRewardPanel = MapExploreTotalRewardPanel or DeclareMono("MapExploreTotalRewardPanel", UIWFlushPanel)
function MapExploreTotalRewardPanel:MapExploreTotalRewardPanel()
    self.data = MapExploreData.Instance
    self.data_cares = {
        {data = self.data.explore_reward_data, func = self.FlushPanel},
    }
end 

function MapExploreTotalRewardPanel:SetSceneID(id)
    self.scene_id = id
    self:FlushPanel()
end

function MapExploreTotalRewardPanel:FlushPanel()
    local param = self.data:GetExploreSceneReward(self.scene_id == nil and SceneData.Instance:SceneId() or self.scene_id)
    self.List:SetData(param.list)
end

function MapExploreTotalRewardPanel:OnClickReward(data)
    if data.flag == 1 then 
        PublicPopupCtrl.Instance:Center(Language.ArenaAchieve.GetTip1)
        return 
    end 

    MapCtrl.Instance:SendMapExplorationReq({
        req_type = 3,
        param = data.scene_id,
        param_1 = data.index,
    })
end

MapExploreTotalRewardCell = MapExploreTotalRewardCell or DeclareMono("MapExploreTotalRewardCell", UIWidgetBaseItem)
function MapExploreTotalRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.RewardList:SetData(data.reward_list)
    self.ListRect.enabled = #data.reward_list > 2
    self.Progress:SetProgress(data.pro_rate)
    self.Progress:SetText(data.pro_str)
    UH.SetText(self.Title,data.title)
    self.BtnReward:SetActive(data.flag == 0)
    self.BtnInter.Interactable = data.pro_complete
    UH.SetText(self.BtnText,data.pro_complete and Language.CloudArena.AchieveRewardStatue[1] or Language.CloudArena.AchieveRewardStatue[0])
    self.Done:SetActive(data.flag == 1)
end
