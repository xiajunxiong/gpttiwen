LBFestivalView = LBFestivalView or DeclareView("LBFestivalView", "lb_festival/lb_festival",Mod.LBFestival.main)
function LBFestivalView:LBFestivalView()

end

function LBFestivalView:LoadCallback(param_t)

end 

function LBFestivalView:OpenCallback()
    LBFestivalCtrl.Instance:ReqTreasureInfo()    

    self.Panel.tag_subs[1].ItemClick:Trigger()
end

function LBFestivalView:OnClickClose() 
    ViewMgr:CloseView(LBFestivalView)
end 

LBFestivalPanel = LBFestivalPanel or DeclareMono("LBFestivalPanel", UIWFlushPanel)
function LBFestivalPanel:LBFestivalPanel()
    self.tag_subs[1]:SetData({
        name= Language.LBFestival.Treasure.Title,
        func_index = 1,
    })
end

function LBFestivalPanel:Awake()
    UIWFlushPanel.Awake(self)
end 

function LBFestivalPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end 

function LBFestivalPanel:OnClickSubTag(data)
    for i = 1,self.subs:Length() do 
        self.subs[i]:SetActive(data.func_index == i)
    end 
end 
--
LbFestivalSubTag = LbFestivalSubTag or DeclareMono("LbFestivalSubTag", UIWidgetBaseItem)
function LbFestivalSubTag:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    

    UH.SetText(self.Name,data.name)
end
