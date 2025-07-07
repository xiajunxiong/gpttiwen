HalloweenPrayGaiLv = HalloweenPrayGaiLv or DeclareView("HalloweenPrayGaiLv", "serveractivity/halloween_pray/halloween_pray_gailv" )
VIEW_DECLARE_LEVEL(HalloweenPrayGaiLv, ViewLevel.Lt)
VIEW_DECLARE_MASK(HalloweenPrayGaiLv, ViewMask.BgBlock)

function HalloweenPrayGaiLv:HalloweenPrayGaiLv()
end

function HalloweenPrayGaiLv:LoadCallback(param_t)
       self.Panels[ HalloweenPrayData.Instance:GetShowStyle()]:SetActive(true)
end


function HalloweenPrayGaiLv:OnCloseClick()
    ViewMgr:CloseView(HalloweenPrayGaiLv)
end

HalloweenPrayGaiLvPanel = HalloweenPrayGaiLvPanel or DeclareMono("HalloweenPrayGaiLvPanel", UIWFlushPanel)
function HalloweenPrayGaiLvPanel:HalloweenPrayGaiLvPanel()
    self:InitShow();
end

function HalloweenPrayGaiLvPanel:InitShow()
     local pray_data = HalloweenPrayData.Instance:GetCfg()
    for k, v in pairs(pray_data.big_reward) do
        if self.Items_Big[k] then
            self.Items_Big[k]:SetData(v)
        end
    end
    for k, v in pairs(pray_data.common_reward) do
        if self.Items_Common[k] then
            self.Items_Common[k]:SetData(v)
        end
    end
end

HalloweenPrayGaiLvItem = HalloweenPrayGaiLvItem or DeclareMono("HalloweenPrayGaiLvItem", UIWidgetBaseItem)

function HalloweenPrayGaiLvItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item=Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.TextName,Item.GetName(data.reward_item.item_id))
    UH.SetText(self.TextNum,Language.Halloween_Pray.gailv..data.show_rate)
end

