GuideTips = GuideTips or DeclareView("GuideTips", "guide/guide_tips")
VIEW_DECLARE_MASK(GuideTips,ViewMask.BgBlockClose)
function GuideTips:GuideTips()
end
function GuideTips:LoadCallback(data)
    UH.SetText(self.Desc, data.desc)
end
function GuideTips:CloseCallback()
end