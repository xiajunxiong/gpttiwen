UIWHelper = {}
--初始化UIWidgetList、UIWScrollList列表
function UIWHelper.CheckContent(self)
    if self.ContentParent == nil then return true end
    self.Rect = self.Rect or self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
    self.ContentRect = self.ContentRect or self.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))
    self.ContSize = self.ContSize or (self.ContentRect and self.ContentRect.rect or nil)
    self.RectSize = self.RectSize or (self.Rect and self.Rect.rect or nil)
    self.ItemSize = self.ItemMono.gameObject.transform.rect
    return self.ContentRect == nil
end
function UIWHelper.CheckOffset(index, value, offset, key)
    return {["" .. key .. ""] = value and value*(index- 1) + (offset or 0) or 0,index = index}
end
--处理SetContentPosition的width或height差值
--存在ItemIndex时当index <= ItemIndex不会执行跳转
function UIWHelper.CheckValve(self, tab, key)
    if tab.is_not_check then return tab[key] end
    if tab.index == 1 then return 0 end
    if self.ItemIndex and tab.index <= self.ItemIndex then return 0 end
    local valve = tab[key]
    if valve and valve == 0 and tab.index ~= nil then
        local product = UIWHelper.GetProduct(self, tab, key) - self.RectSize[key]
        valve = product > 0 and product or nil
    end
    return valve and Valve(math.abs(valve),self.ContSize[key]-self.RectSize[key])*Negative(valve) or nil
end
--计算跳转需要多少高度/宽度 ItemCount=行/列
function UIWHelper.GetProduct(self, tab, key)
    if self.ItemCount ~= nil then
        local k = tab.height and [[width]] or [[height]]
        local axis = self.ItemCount == 0 and math.floor(self.RectSize[k] / self.ItemSize[k]) or self.ItemCount
        return self.ContSize[key] / math.ceil(self:GetItemCount() / axis) * math.ceil(tab.index / axis)
    end
    return self.ContSize[key] / self:GetItemCount() * tab.index
end