VIEW_REQ("modules/pet_weapon/pet_weapon_item_info")
VIEW_REQ("modules/pet_weapon/pet_weapon_view")
VIEW_REQ("modules/pet_weapon/pet_weapon_refine_view")
VIEW_REQ("modules/pet_weapon/pet_weapon_intensify_view")
VIEW_REQ("modules/pet_weapon/pet_weapon_up_show_view")
VIEW_REQ("modules/pet_weapon/pet_weapon_convert")
VIEW_REQ("modules/pet_weapon/pet_weapon_book_view")
VIEW_REQ("modules/pet_weapon/pet_weapon_change_view")

FILE_REQ("modules/pet_weapon/pet_weapon")
FILE_REQ("modules/pet_weapon/pet_weapon_data")

PetWeaponCtrl = PetWeaponCtrl or BaseClass(BaseCtrl)

function PetWeaponCtrl:__init()
    if PetWeaponCtrl.Instance ~= nil then
		Debuger.LogError("[PetWeaponCtrl] attempt to create singleton twice!")
		return
	end
    PetWeaponCtrl.Instance = self
    self.data = PetWeaponData.New()
    self.data = PetWeaponData.Instance
    self:RegisterAllProtocol()
    ItemInfoData.Instance:RegisterBtnRemind(ItemInfoFunc.PetWeaponRefine.name, function(info)
        local pet_weapon = info.item_list[1]
        local num = self.data:GetRefineRemind(pet_weapon)
        return num
    end)
    ItemInfoData.Instance:RegisterBtnRemind(ItemInfoFunc.PetWeaponReplace.name, function(info)
        local pet_weapon = info.item_list[1]
        local num = self.data:GetReplaceRemind(pet_weapon)
        return num
    end)
    ItemInfoData.Instance:RegisterBtnRemind(ItemInfoFunc.PetWeaponIntensify.name, function(info)
        local pet_weapon = info.item_list[1]
        local num = self.data:GetIntensifyRemind(pet_weapon)
        return num
    end)
end

function PetWeaponCtrl:__delete()

end

function PetWeaponCtrl:OnUnloadGameLogic()
    
end

function PetWeaponCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSPetWeaponIntensify)
end

function PetWeaponCtrl:SendIntensify(is_put_on,index,stuff_info,weapon_idx)
    local protocol = GetProtocol(CSPetWeaponIntensify)
    protocol.is_put_on = is_put_on or 1
    protocol.index = index
    protocol.pet_soul_index = weapon_idx or 0
	protocol.stuff_info = stuff_info
	SendProtocol(protocol)
end


-- 精炼
function PetWeaponCtrl:Refine(weapon)

end

-- 升级
function PetWeaponCtrl:Intensify(weapon,func,has_tip)
    local pet_index = weapon:PetIndex()
    local is_bag = 0
    if not pet_index then
        is_bag = 1
    end
    local index = is_bag == 1 and weapon.index or pet_index
    local weapon_idx = weapon:GridPos()
    local stuff_info,dialog_tip = self.data:GetIntensifyStuffInfo()
    if dialog_tip and not has_tip then
        local info = {
            content = dialog_tip,
            confirm = {
                    func = function ()
                        func()
                        self:SendIntensify(is_bag,index,stuff_info,weapon_idx)
                        ViewMgr:CloseView(DialogTipsView)
                    end,
                    wait_time = 5
               },
           }
       PublicPopupCtrl.Instance:DialogTips(info)
    else
        self:SendIntensify(is_bag,index,stuff_info,weapon_idx)
        if func then
            func()
        end
    end
end

-- 穿戴
function PetWeaponCtrl:OnUse(weapon)
    local cur_pet = PetData.Instance:GetCurSelect()
    if TableIsEmpty(cur_pet) then
        ViewMgr:OpenView(PetView)
        ViewMgr:CloseView(BagView)
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.UseTip)
        return
    end
    local pet_index = cur_pet.index
    -- 是不是专武 -- 也可穿戴
    -- local weapon_type,pet_id = weapon:WeaponType()
    -- if weapon_type == PetWeaponType.Only then
    --     if pet_id ~= cur_pet.pet_id then
    --         PublicPopupCtrl.Instance:Error(string.format(Language.PetWeapon.Error1,weapon:OnelyDesc()))
    --         return
    --     end
    -- end
    
    PetCtrl.SendPetOp(PET_OP_TYPE.OnPetWeapon, cur_pet.index, weapon.index, self.data:GetWeaponIdx())
end

-- 卸下
function PetWeaponCtrl:Unfix(weapon)
    local cur_pet = PetData.Instance:GetCurSelect()
    if TableIsEmpty(cur_pet) then
        return
    end
    local pet_index = cur_pet.index
    PetCtrl.SendPetOp(PET_OP_TYPE.UnPetWeapon, cur_pet.index,weapon:GridPos())
    self.data:ClearSelectedBagCell()
end

-- 替换
function PetWeaponCtrl:Replace(weapon)
    local cur_weapon = self.data:CurWeapon()
    self.data:AddSelectedBagCell(weapon, false)
    self:OnUse(weapon)
end

function PetWeaponCtrl:NoWeaponTip(tip_str)
    local param_t = {
        content = tip_str,
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                TreasureHuntCtrl.Instance:OpenTreasureHunQi()
            end,
            name = Language.PetWeapon.Goto,
        },
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end