-- 追月商城 抽奖记录
ZyscCjjlView = ZyscCjjlView or DeclareView('ZyscCjjlView', 'serveractivity/act_zysc/zysc_cjjl')
VIEW_DECLARE_LEVEL(ZyscCjjlView, ViewLevel.Lt)
VIEW_DECLARE_MASK(ZyscCjjlView, ViewMask.BgBlock)
function ZyscCjjlView:ZyscCjjlView()
    self.List:SetData(ActZyscData.Instance:ZyscCjjlDel())
end

function ZyscCjjlView:LoadCallback(param_t)
end

function ZyscCjjlView:OnCloseClick()
    ViewMgr:CloseView(ZyscCjjlView)
end

function ZyscCjjlView:OnClickName(reward_item)
    CellClicks.BagGridNoButton({
        GetData = function()
            return reward_item
        end,
    })
end

----------------------------ZyscCjjlItem----------------------------
ZyscCjjlItem = ZyscCjjlItem or DeclareMono('ZyscCjjlItem', UIWidgetBaseItem)
function ZyscCjjlItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Text, data.text)
    local name = data.reward_item:QuaName(true)
    name = string.gsub(name, ">", ">【", 1)
    name = string.gsub(name, "</", "】</")
    UH.SetText(self.Name, name)
    UH.SetText(self.Num, "x" .. data.reward_item.num)
end

function ZyscCjjlItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
end

function ZyscCjjlItem:OnReturnSelfData()
    return self.data.reward_item
end

