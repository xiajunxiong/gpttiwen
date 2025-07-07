PalacesHurt = PalacesHurt or DeclareView("PalacesHurt", "god_areas_palaces/palaces_hurt")
VIEW_DECLARE_MASK(PalacesHurt, ViewMask.BgBlockClose)
function PalacesHurt:PalacesHurt()
    
end
function PalacesHurt:LoadCallback(param_t)
    UH.SetText(self.Hurt, param_t.total_hurt or 0)
end