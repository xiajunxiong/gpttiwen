-- 神龙
local root_path = 'actor/__ActorRoot.prefab'
local move_speed = 15 * 3.5
local step_dis = move_speed * 0.025
BattleDragon = BattleDragon or BaseClass(BattleCh)
-- 神龙位置类型
BattleDragon.SideType = {
    -- BATTLE_GOD_DRAGON_GRID_BEGIN_INDEX = 30;  //神龙格子起始(伪格子)
    -- 神龙格子BATTLE_GOD_DRAGON_GRID_BEGIN_INDEX+side
    -- 但是客户端格子是+1处理，也就是31和32
    Attack = 31,     -- 攻
    Def = 32,        -- 守
}

BattleDragon.ShowRagePointSkin = {
    [8] = 4,
    [9] = 4,
}

function BattleDragon.Decode()
    local t = {}
    t.level = MsgAdapter.ReadShort()    -- 神龙等级
    t.skin_seq = MsgAdapter.ReadShort() -- -1没有幻化
    return t
end


function BattleDragon:__init(data)
    data = data or {}
    self.vo = data
    self.level = data.level or 1
    self.skin_seq = data.skin_seq or -1
    self.grid_idx = data.grid_idx
    self.name = data.name or self:Name()

    self.root_node = GameObjectPool:Get(root_path)
    self.root_node.name = self.name
    self.vo.root_node = self.root_node
    self.default_scale = 0.7
end

function BattleDragon:Name()
    local skin = self:Skin()
    if skin then
        return skin:Name()
    else
        return Language.FairyDragon.Name
    end
end

function BattleDragon:ResId()
    local skin = self:Skin()
    local res_id
    if skin then
        res_id = skin:ResId()
    else
        res_id = FairyDragonData.Instance.other_cfg.res
    end
    return res_id
end

function BattleDragon:Skin()
    if self.skin_seq == -1 then
        self.skin = nil
        return
    end
    if not self.skin then
        local cfg = FairyDragonData.Instance:GetSkinCfgBySeq(self.skin_seq)
        self.skin = FairyDragonSkin.New(cfg)
    end
    return self.skin
end

function BattleDragon:__delete()
    self.loaded_call_back = nil
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.body_show then
        self.body_show:DeleteMe()
        self.body_show = nil
    end
    GameObjectPool:Release(self.root_node)
    self.root_node = nil
end

function BattleDragon:Load(call_back)
    self.vo.root_obj = self.root_node
    self.body_show = BattleObjDrawer.New(self.root_node, true)
    local body_path = DrawerHelper.GetNpcPath(self:ResId())
    self.loaded_call_back = call_back
    self.body_show:SetData(body_path, nil, BindTool.Bind(self.BodyLoadedCallback, self))
    self:setScale(self.default_scale)
end

function BattleDragon:BodyLoadedCallback()
    self.handle_runner = Runner.Instance:RunUntilTrue(function()
        if SceneCtrl.Instance:CurScene():LoadingComplete() then
            self:Prepare()
            return true
        end
    end)
    if self.loaded_call_back then
        self.loaded_call_back()
        self.loaded_call_back = nil
    end
end

function BattleDragon:Prepare()
    -- 获取携带者的forward
    -- 携带者的pos
    -- 负forward 1单位的pos y + 1单位的pos
    --local pos = Vector3.New()
    --自己这边idx是3，对面是13

    if self.grid_idx == BattleDragon.SideType.Attack then
        local pos = self:getIdxPos(3, Vector3.New(0,0,-3))
        local fw = self:getIdxForward(3)
        self:SetPos(pos)
        self:SetForward(fw)
    elseif self.grid_idx == BattleDragon.SideType.Def then
        local pos = self:getIdxPos(13, Vector3.New(0,0,-3))
        local fw = self:getIdxForward(13)
        self:SetPos(pos)
        self:SetForward(fw)
    end
end

function BattleDragon:SetPos(pos)
    self.root_node:SetPosition(pos)
end
function BattleDragon:SetForward(forward)
    forward.y = 0
    self.root_node:SetForward(forward)
end
function BattleDragon:GetPos()
    return self.root_node:GetPosition()
end

function BattleDragon:getIdxPos(idx, offset)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetPos(idx, offset)
end

function BattleDragon:getIdxForward(idx)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetForward(idx)
end

function BattleDragon:EnableNCA()

end

function BattleDragon:SkillPlayer()
    return self.body_show:SkillPlayer()
end