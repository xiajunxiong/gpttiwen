
local AGArrowRotate = {
    [0] = {
        [1] = 0,
        [-1] = 180,
    },
    [1] = {
       [0] = 90,
    },
    [-1] = {
        [0] = -90,
    }
}

-- SN_3
ArrayGhostDirector = ArrayGhostDirector or DeclareMono("ArrayGhostDirector")

function ArrayGhostDirector:ArrayGhostDirector()
	self.data = ArrayGhostData.Instance

    self.data:ResetGrid()
    self:CreateMap()

    self.handle_arrow = self.data.arrow_info:Care(BindTool.Bind(self.FlushArrow, self))
    self.handle_succ = self.data.end_info:Care(BindTool.Bind(self.IsSucc, self))
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))

    self.show_arrow = false

    ViewMgr:OpenView(ArrayGhostGuideView)
end

function ArrayGhostDirector:OnDestroy()
    self.data.arrow_info:Uncare(self.handle_arrow)
    self.data.end_info:Uncare(self.handle_succ)
    EventSys:UnBind(self.role_pos_event)

    if nil ~= self.data.move_handle then
        Runner.Instance:RemoveRunUntilTrue(self.data.move_handle)
        self.data.move_handle = nil
    end       
end

function ArrayGhostDirector:OnMainRolePosChange()
    local grid_list = self.data.grid_list
    local show_arrow = false
    for _, grid in ipairs(grid_list) do
        if grid:CanMove(MainPlayer:GetPos()) then
            show_arrow = true
            break
        end
    end
    if not show_arrow then
        self.data:SetArrowInfo(0,0,0)
    end
end

function ArrayGhostDirector:FlushArrow(old)
    local ai = self.data.arrow_info
    if ai.index > 0 then
        local value, pos = self.data.grid_list[ai.index]:GetPos()
        if value then
            self.ArrowPos.position = Vector3.New(pos.x + ai.x, pos.y, pos.z + ai.z)
            self.ArrowPos.localEulerAngles = Vector3.New(0, AGArrowRotate[ai.x] and AGArrowRotate[ai.x][ai.z] or 0, 0)
            self.ArrowPos:SetObjActive(true)
        end
    else
        self.ArrowPos:SetObjActive(false)
    end
end

function ArrayGhostDirector:IsSucc()
    if self.data.end_info.is_succ then
        self.GhostEffet:SetActive(true)
        AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.ArrayGhost)
    end
end

function ArrayGhostDirector:CreateMap()
    self.map_loaded = false

    local item_side = self.data:CoItemSide()

    local grid_list = self.data.grid_list
    for _, grid in ipairs(grid_list) do
        if grid:ResId() > 0 then
            ResMgr:LoadGameObject(DrawerHelper.GetNpcPath(grid:ResId()), function (obj)
                obj:SetParent(self.Grid1.gameObject)
                obj:SetLocalEulerAngles(Vector3.zero)
                grid:SetShowObj(obj.transform)
            end, true)
        end
    end
end

ArrayGhostMove = ArrayGhostMove or DeclareMono("ArrayGhostMove", UIWFlushPanel)

function ArrayGhostMove:ArrayGhostMove()
    self.data = ArrayGhostData.Instance

    self.data_cares = {
		{data = self.data.arrow_info, func = self.FlushBtnMove, init_call = false},
	}
end

function ArrayGhostMove:FlushBtnMove()
    self.BtnMoveObj:SetActive(self.data.arrow_info.index > 0)
end

function ArrayGhostMove:OnClickMove()
    if self.data.is_moving then
        PublicPopupCtrl.Instance:Center(Language.ArrayGhost.MovingTips)
        return
    end
    local ai = self.data.arrow_info
    self.data:GridMove(ai.index, ai.x, ai.z)
end