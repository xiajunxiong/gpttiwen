RoleSkillSortView = RoleSkillSortView or DeclareView("RoleSkillSortView", "role/role_skill_sort")
function RoleSkillSortView:LoadCallback()
    --[[ local v2 = {
        Vector2.New(503, 584),
        Vector2.New(673, 584),
        Vector2.New(820, 584)
    } ]]
    --获取技能数
    --[[ local num = SkillData.Instance.skill_num
    local index = 1
    index = num <= 9 and index or 2
    index = num <= 12 and index or 3 ]]
    --UH.SizeDelta(self.RootRect, v2[1])
    self.Board:SetData(self:GetType(), Language.Role.RoleSkillSort, Vector2.New(503, 584))
end
function RoleSkillSortView:CloseCallback()
    RoleCtrl.Instance:SendSkillPositionModify()
    SkillData.Instance.view_data.drag_select = nil
    SkillData.Instance.view_data.drop_select = nil
end
RoleSkillSortViewPanel = RoleSkillSortViewPanel or DeclareMono("RoleSkillSortViewPanel", UIWFlushPanel)
function RoleSkillSortViewPanel:RoleSkillSortViewPanel()
    self.v2 = Vector2.zero
end
function RoleSkillSortViewPanel:onFlush()
    local data = SkillData.Instance:SkillSortList()
    self.SkillList:SetData(data)
    --计算Content
    local num = SkillData.Instance.skill_num - 9
    if num > 0 then
        local row = math.ceil(num / 3)
        local y = row * 150
        self.v2:Set(0, y)
        self.Content.sizeDelta = self.v2
        y = row * -75
        self.v2:Set(0, y)
        self.Content.anchoredPosition = self.v2
    end
end

RoleSkillSortItem = RoleSkillSortItem or DeclareMono("RoleSkillSortItem", UIWidgetBaseItem)
local Input = UnityEngine.Input
function RoleSkillSortItem:RoleSkillSortItem()
end
function RoleSkillSortItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.SaveData == true then
        return
    end
    self.Name.text = data.skill_name
    self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    if self.mainCanvas then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    UH.SetIcon(self.Cell, data.icon_id, ICON_TYPE.SKILL)
end
function RoleSkillSortItem:OnDragBegin()
    --启用ignore
    self.LayoutGroup.enabled = false
    self.LayoutEle.ignoreLayout = true
    local v2 = Vector2.New(0.5, 0.5)
    self.Root.anchorMax = v2
    self.Root.anchorMin = v2
    self.LuaDrop.enabled = false
    self.Block.enabled = false
    SkillData.Instance.view_data.drag_select = self
    self.CurSiblingIndex = self.Root:GetSiblingIndex()
    self.EmTran:SetSiblingIndex(self.CurSiblingIndex)
    self.Root:SetSiblingIndex(SkillData.Instance.skill_num + 2)
    self.CurSiblingIndex = self.EmTran:GetSiblingIndex()
    -- LogError("拖动开始",self.CurSiblingIndex)
end
function RoleSkillSortItem:OnDragEnd()
    -- self.EmTran:SetSiblingIndex(SkillData.Instance.skill_num + 1)
    self.LayoutEle.ignoreLayout = false
    self.LuaDrop.enabled = true
    self.Block.enabled = true
    self.LayoutGroup.enabled = true
    self.EmLayoutEle.ignoreLayout = true
    local drag = SkillData.Instance.view_data.drag_select
    local drop = SkillData.Instance.view_data.drop_select
    if drop ~= nil then
        drag.Root:SetSiblingIndex(drop.Root:GetSiblingIndex())
        local drag_key = SkillData.Instance:GetSortKey(drag.data.sort_index)
        local drop_key = SkillData.Instance:GetSortKey(drop.data.sort_index)
        SkillData.Instance:ChangeSortList(drag_key, drag.data.sort_index, drop_key)
    else
        drop = self.EmLuaMono
        --一种判断 拖来拖去还是在原来的位置
        --LogError("放下的位置",drop.Root:GetSiblingIndex())
        -- LogError(self.CurSiblingIndex,drop.Root:GetSiblingIndex())
        if drop.data ~= nil and self.CurSiblingIndex ~= drop.Root:GetSiblingIndex() then
            -- LogError(drop.Root:GetSiblingIndex())
            drag.Root:SetSiblingIndex(drop.Root:GetSiblingIndex())
            local drag_key = SkillData.Instance:GetSortKey(drag.data.sort_index)
            local drop_key = SkillData.Instance:GetSortKey(drop.data.sort_index)
            SkillData.Instance:ChangeSortList(drag_key, drag.data.sort_index, drop_key)
        else
            drag.Root:SetSiblingIndex(self.CurSiblingIndex)
        end
    end
    self.EmLuaMono:SetData(nil)
    SkillData.Instance.view_data.drag_select = nil
    SkillData.Instance.view_data.drop_select = nil
end
function RoleSkillSortItem:OnDrag()
    local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.Content,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    UH.AnchoredPosition(self.Root, pos)
    local y = (self.Root.localPosition.y + 300) / 600
    y = y < 0 and 0 or y
    y = y > 1 and 1 or y
    self.ScrollRect.verticalNormalizedPosition = y
end
function RoleSkillSortItem:OnDrop()
    -- LogError("drop", self.data.skill_name)
    -- SkillData.Instance.view_data.drop_select = self
end
function RoleSkillSortItem:OnPointerEnter()
    --触发范围可能要改小一点
    local drag = SkillData.Instance.view_data.drag_select
    if drag ~= nil then
        -- SkillData.Instance.view_data.drop_select = self
        -- LogError("与", self.data.skill_name, "交换位置")
        --[[ if self.EmLuaMono.data == self.data then
            --如果多次交换则取消
            self.EmLuaMono:SetData(nil)--这是一种判断
        else ]]
        self.EmLuaMono:SetData(self.data)
        --[[ end ]]
        self.LayoutGroup.enabled = false
        self.EmTran:SetSiblingIndex(self.Root:GetSiblingIndex())
        self.EmLayoutEle.ignoreLayout = false
        self.LayoutGroup.enabled = true
    else
        self.EmLayoutEle.ignoreLayout = true
    end
end
--[[ function RoleSkillSortItem:OnPointerExit()
    self.EmLayoutEle.ignoreLayout = true
end ]]
