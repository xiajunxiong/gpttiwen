-- 偃月兔护盾
local root_path = 'actor/__ActorRoot.prefab'
BattleRabbitShield = BattleRabbitShield or BaseClass()
function BattleRabbitShield:__init(vo)
    self.vo = vo
    self.grid_idx = vo.grid_idx  
    self.root_node = GameObjectPool:Get(root_path)
    local shield_idx = self:ShieldIdx()
    self.root_node.name = "偃月兔护盾" .. shield_idx
    self.is_show = true
end

function BattleRabbitShield:ShieldIdx()
    return self.grid_idx - RABBIT_SHIELD_GRID_BEGIN_INDEX
end

function BattleRabbitShield:__delete()
    if self.shield_effect then
        SE:Stop(self.shield_effect)
        self.shield_effect = nil
    end
    if self.beattakced_effect then
        SE:Stop(self.beattakced_effect)
        self.beattakced_effect = nil
    end
    if self.destroy_effect then
        SE:Stop(self.destroy_effect)
        self.destroy_effect = nil
    end

    GameObjectPool:Release(self.root_node)
    self.root_node = nil

    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    TimeHelper:CancelTimer(self.destroy_effect)
end

function BattleRabbitShield:GetRoot()
    return self.root_node
end

function BattleRabbitShield:IsShow()
    return self.is_show
end

function BattleRabbitShield:Load()
    self.handle_runner = Runner.Instance:RunUntilTrue(function()
        if BattleData.Instance:SceneData() then
            self:Prepare()
            if self.is_show then
                self:CreateShield()
            end
            return true
        end
    end)
end


function BattleRabbitShield:Prepare()
    local shield_idx = self:ShieldIdx()
    local idx = shield_idx <= 5 and (5 + shield_idx) or (10 + shield_idx)
    local fw = self:getIdxForward(idx)
    local pos = self:getIdxPos(idx,Vector3.New(0,0,0))
    self:SetPos(pos)
    self:SetForward(fw)
end

function BattleRabbitShield:getIdxPos(idx, offset)
    return BattleDisplayer.Instance:getIdxPos(idx, offset)
end

function BattleRabbitShield:getIdxForward(idx)
    return BattleDisplayer.Instance:getIdxForward(idx)
end

function BattleRabbitShield:SetPos(pos)
    self.root_node:SetPosition(pos)
end
function BattleRabbitShield:SetForward(forward)
    forward.y = 0
    self.root_node:SetForward(forward)
end
function BattleRabbitShield:GetPos()
    return self.root_node:GetPosition()
end

-- 生成护盾
function BattleRabbitShield:CreateShield(loaded_func)
    self.is_show = true
    if not self.shield_effect then
        self.shield_effect = SE:Play(SceneEffectConfig.EffectList.RabbitShieldCreate, Vector3.zero, nil , self.root_node, loaded_func)
    else
        if loaded_func then
            loaded_func()
        end
    end
end

-- 护盾受击
function BattleRabbitShield:BeAttacked(loaded_func)
    if self.beattakced_effect then
        SE:Stop(self.beattakced_effect)
        self.beattakced_effect = nil
    end
    if not self.beattakced_effect then
        self.beattakced_effect = SE:Play(SceneEffectConfig.EffectList.RabbitShieldBeAttacked, Vector3.zero, nil , self.root_node, loaded_func)
    end
end

-- 护盾消失
function BattleRabbitShield:DestroyShiled(loaded_func)
    self.is_show = false
    if self.destroy_effect then
        SE:Stop(self.destroy_effect)
        self.destroy_effect = nil
    end
    if not self.destroy_effect then
        self.destroy_effect = SE:Play(SceneEffectConfig.EffectList.RabbitShieldDestroy, Vector3.zero, nil , self.root_node, loaded_func)
    end
    TimeHelper:CancelTimer(self.destroy_time_ht)
    self.destroy_time_ht = Invoke(function ()
        if self.shield_effect then
            SE:Stop(self.shield_effect)
            self.shield_effect = nil
        end
    end, 0.5)
end