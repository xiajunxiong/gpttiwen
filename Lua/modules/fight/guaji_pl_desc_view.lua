GuaJiPlDescView = GuaJiPlDescView or DeclareView("GuaJiPlDescView", "fight/guaji_pl_desc")

VIEW_DECLARE_LEVEL(GuaJiPlDescView, ViewLevel.Lt)
VIEW_DECLARE_MASK(GuaJiPlDescView, ViewMask.BlockClose)

function GuaJiPlDescView:GuaJiPlDescView()
    self.language=Language.guaji
    UH.SetText(self.Tip1,self.language.pldTip1)
    UH.SetText(self.Tip2,self.language.pldTip2)
    UH.SetText(self.Tip3,self.language.pldTip3)
end

function GuaJiPlDescView:LoadCallback(param_t)
end
