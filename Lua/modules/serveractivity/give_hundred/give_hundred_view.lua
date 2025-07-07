GiveHundredView = GiveHundredView or DeclareView("GiveHundredView", "serveractivity/give_hundred_view", Mod.RandActivity.GiveHundred)
-- VIEW_DECLARE_MASK(GiveHundredView, ViewMask.BgBlockClose)
function GiveHundredView:GiveHundredView()
    self.data = GiveHundredData.Instance
end
function GiveHundredView:LoadCallback()
    local list = DataHelper.TableCopy(self.data:GetConfig())
    local pass_num
    for i, v in ipairs(list) do
        pass_num = self.data:PassNum(v.type)
        v.sort = v.type
        if pass_num >= v.give_num then
            v.sort = v.type * 100
        end
    end
    table.sort(list, DataHelper.SortFunc("sort"))
    self.List:SetData(list)
end
function GiveHundredView:OnClickClose()
    ViewMgr:CloseView(GiveHundredView)
end

GiveHundredViewItem = GiveHundredViewItem or DeclareMono("GiveHundredViewItem", UIWidgetBaseItem)
function GiveHundredViewItem:GiveHundredViewItem()
    self.give_data = GiveHundredData.Instance
end
function GiveHundredViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pass_num, cur = self.give_data:PassNum(data.type)
    UH.SetText(self.Name, data.get_way)
    UH.SetText(self.Num, string.format("x%s", data.give_num))
    UH.SetText(self.PassNum, string.format(Language.GiveHundred.PassNum, pass_num))
   --[[  if pass_num >= data.give_num then
        self.gameObject:SetAsLastSibling()
    end ]]
    self.BtnInter.Interactable = pass_num < data.give_num
    if data.type == 1 then
        UH.SetText(self.Desc, string.format(data.desc, cur))
    else
        UH.SetText(self.Desc, data.desc)
    end
    if data.prompt == 0 then
        UH.SetText(self.Desc2, "")
    else
        UH.SetText(self.Desc2, data.prompt)
    end
end
function GiveHundredViewItem:OnClickGoto()
    local pass_num, cur = self.give_data:PassNum(self.data.type)
    if pass_num >= self.data.give_num then
        PublicPopupCtrl.Instance:Center(Language.GiveHundred.GiveAll)
        return
    end
   -- ViewMgr:CloseView(GiveHundredView)
   ViewMgr:OpenViewByKey(self.data.jump_mod)
end