-- =================== 宠物徽章 ======================
PetBagdeData = PetBagdeData or BaseClass()

function PetBagdeData:__init()
    if PetBagdeData.Instance ~= nil then
		Debuger.LogError("[PetBagdeData] attempt to create singleton twice!")
		return
	end
    PetBagdeData.Instance = self

    self.cfg = Config.pet_badges_auto

    -- 选中的分解徽章
    self.resolve_data = SmartData.New()

    -- 属性配置
    Cfg.Register("PetBagdeAttriCfg",Cfg.RealTimeSearch2Key(self.cfg.att_group, "group_id", "att_type"))
end

function PetBagdeData:__delete()
end

-- 根据徽章品质配置
function PetBagdeData:GetBagdeQuaCfg(qua_level)
    for i,v in pairs(self.cfg.pet_badges) do
        if v.quality == qua_level then
            return v
        end
    end
end

-- 根据碎片道具获取品质配置
function PetBagdeData:GetBagdeQuaCfgByClipItem(clip_item_id)
    for i,v in pairs(self.cfg.pet_badges) do
        if v.synthesis_item == clip_item_id then
            return v
        end
    end
end

-- 获取属性配置
function PetBagdeData:GetAttriCfg(group_id, attri_type)
    local cfg = Cfg.PetBagdeAttriCfg(group_id, attri_type)
    return cfg
end

-- 获取徽章列表 qua 对应的品质, is_pet 是否获取宠物身上的
function PetBagdeData:GetBagdeList(qua, is_pet)
    is_pet = is_pet == nil and true or is_pet
    local b_list = self:GetAllBagdeOfBag(qua)

    local p_list = {}
    if is_pet then
        p_list = self:GetAllBagdeOfPet(qua)
    end
    local list = AndList(b_list, p_list)
    return list
end

-- 获取背包全部徽章
function PetBagdeData:GetAllBagdeOfBag(qua, is_sort)
    if qua == 0 then
        qua = nil
    end
    is_sort = is_sort == nil and true or is_sort
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    -- local col_grids = {
    --     Item.Create(PetBagde.CreateVo(9010)),
    --     Item.Create(PetBagde.CreateVo(9010)),
    --     Item.Create(PetBagde.CreateVo(9011)),
    --     Item.Create(PetBagde.CreateVo(9011)),
    --     Item.Create(PetBagde.CreateVo(9012)),
    --     Item.Create(PetBagde.CreateVo(9013)),
    --     Item.Create(PetBagde.CreateVo(9014)),
    --     Item.Create(PetBagde.CreateVo(9015)),
    --     Item.Create(PetBagde.CreateVo(9015)),
    --     Item.Create(PetBagde.CreateVo(9010)),
    -- }
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.PET_BAGDE then
            if not qua or v:QuaLevel() == qua then
                table.insert(list,v)
            end
        end
    end
    if is_sort then
        self:SortBagdeList(list)
    end
    return list
end

-- 获取宠物身上全部徽章
function PetBagdeData:GetAllBagdeOfPet(qua, is_sort)
    if qua == 0 then
        qua = nil
    end
    is_sort = is_sort == nil and true or is_sort
    local list = {}
    local pet_list = PetData.Instance:GetPetList()
    local cur_pet = PetData.Instance:GetCurSelect()
    for i,v in pairs(pet_list) do
        local item = v:BagdeItem()
        if item and v.index ~= cur_pet.index then
            if not qua or item:QuaLevel() == qua then
                table.insert(list,item)
            end
        end
    end
    if is_sort then
        self:SortBagdeList(list)
    end
    return list
end

function PetBagdeData:SortBagdeList(list)
    table.sort(list, function (a, b)
        return a:QuaLevel() > b:QuaLevel()
    end)
end


-- 添加分解的徽章
function PetBagdeData:AddReslove(item, value)
    self.resolve_data[item] = value
end

function PetBagdeData:ClearReslove()
    self.resolve_data:Set({})
end

-- 分解可获得的材料列表
function PetBagdeData:ResloveOutputStuffList()
    local item_id_list = {}
    for item,v in pairs(self.resolve_data) do
        if v then
            local cfg = item:QuaCfg()
            if not item_id_list[cfg.dec_item_id] then
                item_id_list[cfg.dec_item_id] = 0
            end
            item_id_list[cfg.dec_item_id] = item_id_list[cfg.dec_item_id] + cfg.dec_num
        end
    end
    local item_list = {}
    for item_id,num in pairs(item_id_list) do
        local item = Item.Create({item_id = item_id, num = num})
        table.insert(item_list, item)
    end
    return item_list
end


function PetBagdeData:SetSelectData(data)
    self.select_data = data
end

function PetBagdeData:GetSelectData()
    return self.select_data
end