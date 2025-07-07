ConditionTip = ConditionTip or DeclareView("ConditionTip", "public_popup/condition_tip")
VIEW_DECLARE_MASK(ConditionTip, ViewMask.BlockClose)

function ConditionTip:ConditionTip()
end
-- {list = {}, title = string}
function ConditionTip:LoadCallback(data)
    UH.SetText(self.Title, data.title)
    self.List:SetData(data.list)
end

ConditionItem = ConditionItem or DeclareMono('ConditionItem', UIWidgetBaseItem)
function ConditionItem:ConditionItem()
end
--{desc = string ,  green = boolean}
function ConditionItem:SetData(data)
    UH.SetText(self.Desc, data.green and ColorStr(data.desc, COLORSTR.Green7) or data.desc)
    self.DescImg.color = data.green and COLORS.Green7 or COLORS.White
end
