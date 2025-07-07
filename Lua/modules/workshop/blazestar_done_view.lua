BlazeStarDoneView = BlazeStarDoneView or DeclareView("BlazeStarDoneView", "workshop/blaze_star_done")
function BlazeStarDoneView:BlazeStarDoneView()
    self.data = WorkshopData.Instance
end

function BlazeStarDoneView:LoadCallback(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)

    self.cell:SetData(param_t.item)
    UH.SetText(self.name,param_t.item:QuaName())

    local blaze_cfg = self.data:GetBlazeCfg(param_t.item.item_id,param_t.item:BlazeStar())
    local str = Language.Workshop.BlazeStar.DoneShow
    if blaze_cfg ~= nil then 
        for i = 1,6 do 
            local at_type = blaze_cfg["attr_type_"..i]
            local at_value = blaze_cfg["attr_min_"..i]
            if at_value > 0 then 
                str = str .. string.format(Language.Workshop.BlazeStar.AttrDoneShow, 
                    Language.Common.AttrList[at_type], DataHelper.IsPercent(at_type) and Percent(at_value) or at_value).."    "
            end 
        end 
    else 
        self:OnClickBlock()
    end 
    UH.SetText(self.attr_desc,str)
end 

function BlazeStarDoneView:OnClickBlock()
    ViewMgr:CloseView(BlazeStarDoneView)
end 