ElementGuideView = ElementGuideView or DeclareView("ElementGuideView", "task/element_guide_view", Mod.Guide.ElementGame)
function ElementGuideView:ElementGuideView()
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    -- 1、2、3、4依次代表水火风地  value表示放入了哪个地方，为0代表没有放入
    self.put_data = {
        [1] = 0,
        [2] = 0,
        [3] = 0,
        [4] = 0,
    }

    -- 指引数据
    self.guide_cfg = {
        [1] = {tip_pos_x = 0, tip_pos_y = 0, tips = Language.ElementGuide.Tip1},
        [2] = {tip_pos_x = -80, tip_pos_y = -200, tips = Language.ElementGuide.Tip2}
    }

    -- 特效
    self.effect_cfg = {
        [1] = "5167007",
        [2] = "5167008",
        [3] = "5167009",
        [4] = "5167006",
    }
    self.cur_guide = 1
end

function ElementGuideView:LoadCallback()
    self.target_pos_list = {}
    for i = 1, self.PosList:Length() do
        self.PosList[i]:SetData({value = 0})
        local target = self.PosList[i].gameObject
        local t_pos = target:GetPosition()
        self.target_pos_list[i] = t_pos
    end
    self:HandleGuide()
end

function ElementGuideView:CloseCallback()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    TimeHelper:CancelTimer(self.time_ht)
end

function ElementGuideView:GetTouchPos()
	local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.PanelRect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    return pos
end

function ElementGuideView:OnCloseClick()
    ViewMgr:CloseView(ElementGuideView)
end

function ElementGuideView:OnBeginDrag(key)
    local scroll_com = self.ElementScrolls[key]
    if self.put_data[key] > 0 then
        scroll_com.enabled = true
        local pos_index = self.put_data[key]
        self.PosList[pos_index]:SetData({value = 0})
        self.put_data[key] = 0
        self.is_lock = true
        return
    end
end

function ElementGuideView:OnDrag(key)
    if self.is_lock then
        return
    end
    local scroll_com = self.ElementScrolls[key]
    self.drag_key = key
    scroll_com.enabled = false
    local element_rect = self.ElementRects[key]
    local pos = self:GetTouchPos()
    UH.AnchoredPosition(element_rect,pos)
end

function ElementGuideView:OnEndDrag(key)
    local scroll_com = self.ElementScrolls[key]
    if scroll_com.enabled then
        self.is_lock = false
        return
    end
    local self_pos = self.ElementRects[key].gameObject:GetPosition()
    for i = 1, #self.target_pos_list do
        local t_pos = self.target_pos_list[i]
        local dis = Vector2.Distance(self_pos,t_pos)
        local last_value = self.PosList[i]:GetData().value or 0
        if dis < 0.2 and last_value == 0 then
            self.put_data[key] = i
            self.ElementRects[key].gameObject:SetPosition(t_pos)
            self.PosList[i]:SetData({value = key})

            -- 是否放对了
            local result = self:Check(i)
            if result == 2 then     -- 全部完成
                PublicPopupCtrl.Instance:Error(Language.ElementGuide.Tip4)
                self.Mask:SetActive(true)
                self.UIEffectTool:Play(5167010)
                TimeHelper:CancelTimer(self.time_ht)
                for j = 1,4 do
                    self.PosList[i].Point.gameObject:SetActive(false)
                end
                self.time_ht = Invoke(function ()
                    self:SubTask()
                end,2)
            elseif result == 1 then -- 单个完成
                local effect_id = self.effect_cfg[key]
                self.UIEffectTool:Play(effect_id,self.PosList[i].gameObject)
            end
            break
        end
    end
    if self.put_data[key] == 0 then
        scroll_com.enabled = true
    end
end

-- 返回1单个ok,返回2全部完成
function ElementGuideView:Check(i)
    -- 是否全部大于0
    local is_all = true
    for i = 1, self.PosList:Length() do
        local value = self.PosList[i]:GetData().value
        if value == 0 then
            is_all = false
            break
        end
    end

    if is_all then
        local is_ok = true
        local first_pos = 1 
        for i = 1, self.PosList:Length() - 1 do
            local value = self.PosList[i]:GetData().value
            value = value == 4 and 0 or value
            local next_value = self.PosList[i + 1]:GetData().value
            if value + 1 ~= next_value then
                is_ok = false
                break
            end
            if value == 1 then
                first_pos = i
            end
        end
        if is_ok then
            self.FinishEffectObj.transform.localRotation = Quaternion.Euler(0,0,(first_pos - 1) * -90)
            return 2
        else
            PublicPopupCtrl.Instance:Error(Language.ElementGuide.Tip3)
        end
    else
        local last_i = i - 1 > 0 and i - 1 or 4
        local next_i = i + 1 > 4 and 1 or i + 1
        local last_value = self.PosList[last_i]:GetData().value
        local next_value = self.PosList[next_i]:GetData().value
        local cur_value = self.PosList[i]:GetData().value
        if last_value > 0 then
            last_value = last_value == 4 and 0 or last_value
            -- 单个完成
            if last_value + 1 == cur_value then
                return 1
            else
                PublicPopupCtrl.Instance:Error(Language.ElementGuide.Tip3)
            end
            return
        end
        if next_value > 0 then
            local effect_value = cur_value
            cur_value = cur_value == 4 and 0 or cur_value
            -- 单个完成
            if cur_value + 1 == next_value then
                return 1
            else
                PublicPopupCtrl.Instance:Error(Language.ElementGuide.Tip3)
            end
            return
        end
    end
end

function ElementGuideView:PlayEffect(value_list)
    for i,v in pairs(value_list) do
        local effect_id = self.effect_cfg[v]
        self.UIEffectTool:Play(effect_id)
    end
end

function ElementGuideView:SubTask()
    ViewMgr:CloseView(ElementGuideView)
    -- local task = TaskData.Instance:GetTaskInfoByTaskId(60010)
    -- if task then
    --     AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
    -- end
    TaskCtrl.Instance:SendTaskGameReq(TaskConfig.TaskGameList.ElementGuide, 1)
end

function ElementGuideView:HandleGuide()
    local guide_cfg = self.guide_cfg[self.cur_guide]
    self.Img:SetActive(guide_cfg == nil)
    self.TipsObj:SetActive(guide_cfg ~= nil)
    self.GuideObj:SetActive(self.cur_guide == 2)
    self.GuideMask:SetActive(self.cur_guide == 1)
    if guide_cfg then
        self.TipsObj:SetLocalPosition(Vector3.New(guide_cfg.tip_pos_x,guide_cfg.tip_pos_y,0))
        UH.SetText(self.GuideTips, guide_cfg.tips)
    end
    
end

function ElementGuideView:OnGuideClick(guide_index)
    if guide_index == 1 then
        self.cur_guide = 2
        self:HandleGuide()
    end
    if guide_index == 2 then
        --self:HandleGuide()
        self:MoveGuide()
    end
end

function ElementGuideView:MoveGuide()
    if self.is_move then
        return
    end
    self.is_move = true
    local target_pos = self.target_pos_list[4]
    local begin_pos = self.ElementRects[4].gameObject:GetPosition()
    self.GuideObj:SetPosition(begin_pos)
    local move_x = (target_pos.x - begin_pos.x) / 50
    local move_y = (target_pos.y - begin_pos.y) / 50
    local move_z = (target_pos.z - begin_pos.z) / 50
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.ElementScrolls[4].enabled = false
    self.run_ht = Runner.Instance:RunUntilTrue(function ()
        local pos = self.ElementRects[4].gameObject:GetPosition()
        local new_pos = Vector3.New(pos.x + move_x, pos.y + move_y, pos.z + move_z)
        self.ElementRects[4].gameObject:SetPosition(new_pos)
        self.GuideObj:SetPosition(new_pos)
        local dis = Vector2.Distance(new_pos,target_pos)
        if dis < 0.2 then
            self.ElementRects[4].gameObject:SetPosition(target_pos)
            self.GuideObj:SetPosition(target_pos)
            self.put_data[4] = 4
            self.PosList[4]:SetData({value = 4})
            self.cur_guide = 3
            self:HandleGuide()
            return true
        end 
    end)
end

ElementPosItem = ElementPosItem or DeclareMono("ElementPosItem", UIWidgetBaseItem)
function ElementPosItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local value = data.value or 0
    UH.SpriteName(self.Frame, "YuanHuan".. value)
    UH.SpriteName(self.Point, "JianTou".. value)
    self.Flag:SetActive(value == 0)
end