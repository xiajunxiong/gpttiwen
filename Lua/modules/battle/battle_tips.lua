BattleTips = BattleTips or DeclareView("BattleTips", "battle/battle_tips", 0)--, ViewLayer.Top - 1)
VIEW_DECLARE_LEVEL(BattleTips, ViewLevel.Lt)
VIEW_DECLARE_MASK(BattleTips, ViewMask.None)

function BattleTips:BattleTips()
end

function BattleTips:LoadCallback(data)
    UH.SetText(self.DescTex, data.strategic_tips_content)
end