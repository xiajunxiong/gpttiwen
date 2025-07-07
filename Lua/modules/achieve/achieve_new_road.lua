AchieveNewRoad = AchieveNewRoad or DeclareView("AchieveNewRoad","achieve/achieve_new_road_detail")

function AchieveNewRoad:AchieveNewRoad()
    -- local config = Config.accomplishment_auto.accomplishment
    -- for i = 1,self.item_array:Length() do
    --     self.item_array[i]:SetData(config[i])
    -- end
end

function AchieveNewRoad:OnClose()
    ViewMgr:CloseView(AchieveNewRoad)
end



AchieveNewRoadItem = AchieveNewRoadItem or DeclareMono("AchieveNewRoadItem")
---StageImg=======UIImageSpriteSet
---CurStage=======GameObject
function AchieveNewRoadItem:AchieveNewRoadItem()
    -- self.cur_stage_oj:SetActive(self.stage == AchieveNewData.Instance:GetCurStage(true))
    self.name.text =  AchieveNewData.GetStageName(self.stage)
    local cur_stage = AchieveNewData.Instance:GetCurStage(true)
    if cur_stage == self.stage then
    	local localPos = self.unityMono.gameObject:GetLocalPosition()
    	localPos.y = localPos.y + 117
    	self.CurStage:SetLocalPosition(localPos)
    end
    self.StageImg.SpriteName = AchieveNewData.GetTitleSpriteName(self.stage)
end
