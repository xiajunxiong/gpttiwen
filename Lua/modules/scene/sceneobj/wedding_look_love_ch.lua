WeddingLookLoveCh = WeddingLookLoveCh or BaseClass(SceneObj)
function WeddingLookLoveCh:__init()
    self.vo = nil
    self.data = MarryCtrl.Instance:Data()
end 

function WeddingLookLoveCh:__delete()
    TimeHelper:CancelTimer(self.range_invoke_timer)
end 

function WeddingLookLoveCh:InitEnd()
    self:SetScenePos(SceneLogic.Instance:SceneID(), self.vo.x,  self.vo.y, true)
    self:SetVisible(false)
    self:CheckMarriedRange()
end

function WeddingLookLoveCh:CheckMarriedRange()
    TimeHelper:CancelTimer(self.range_invoke_timer)
    self.range_invoke_timer = TimeHelper:AddRunTimer(function()
        if SceneData.Instance:SceneId() == Config.get_married_auto.other[1].scene_id then
            local info = self.data.wedding_scene_info or {}
            local link_seq = info.link_seq or 0
            if tonumber(link_seq) == MarryConfig.FBEventType.act_xqma then
                if MainPlayer and MainPlayer:GetDistance(self:GetPos()) < 30 then
                    self:SetVisible(true)
                else
                    self:SetVisible(false)
                end
            else
                self:SetVisible(false)
            end
        else
            self:SetVisible(false)
        end
    end, 1)
end