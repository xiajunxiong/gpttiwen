UnlockNewMapView = UnlockNewMapView or DeclareView("UnlockNewMapView","unlock_newmap/unlock_newmap")
---PetList======UIWidgetList[UnlockPetItem]
function UnlockNewMapView:LoadCallback()
	TimeHelper:AddDelayTimer(function()
		ViewMgr:CloseView(UnlockNewMapView)
	end,3)
	self.data = self:FindDataByID(UnlockNewMapCtrl.Instance.active_id)
	self.name.text = self.data.name
	local data_is_pet = self.data.is_pet
	if data_is_pet ~= nil and data_is_pet ~= 0 and data_is_pet ~= "" then
		local pats = self:split(data_is_pet,"|")
		local list_data = {}
		for _, v in pairs(pats) do
			-- Pet.New({item_id = data.monster_icon})
			table.insert(list_data,Pet.New({item_id = tonumber(v)}))
		end
		self.PetList:SetData(list_data)
	else
		self.textOj:SetActive(false)
	end	
end

function UnlockNewMapView:OnClose()
	ViewMgr:CloseView(UnlockNewMapView)
end

function UnlockNewMapView.delayTime(seconds, callback)
	local since_time = Time.realtimeSinceStartup
	Runner.Instance:RunUntilTrue(function ()
		local now_time = Time.realtimeSinceStartup
		if now_time - since_time >= seconds then
			callback()
			return true
		else
			return false
		end
	end)
end

function UnlockNewMapView:FindDataByID(id)
	local config = Config.world_map_auto.word_type_show
	for k, v in pairs(config)do
		if v.id == id then
			return v
		end
	end
	Debuger.LogError("[ERROR] UnlockNewMapView:FindDataByID   can't not find id:"..id)
	return nil
end

UnlockPetItem = UnlockPetItem or DeclareMono("UnlockPetItem",UIWidgetBaseItem)
function UnlockPetItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local icon = self:GetIcon(data.pet_id)
	if icon then
		UH.SetIcon(self.icon,icon)
	end
end

function UnlockPetItem:GetIcon(pet_id)
	if pet_id == nil then
		Debuger.LogError("[ERROR] pet id is null")
		return
	end
	local config = Config.pet_cfg_auto.pet_info
	for k, v in pairs(config)do
		if v.id == tonumber(pet_id) then
			return v.icon_id
		end
	end
	Debuger.LogError("[ERROR] PetItem:GetIcon ===> can't not find pet id:"..pet_id)
	return nil
end

function UnlockPetItem:PetItemClick()
	-- local pet = Pet.New()
	-- pet.pet_id = self.data
end


function UnlockNewMapView:split(str,delimiter)
    local dLen = string.len(delimiter)
    local newDeli = ''
    for i=1,dLen,1 do
        newDeli = newDeli .. "["..string.sub(delimiter,i,i).."]"
    end

    local locaStart,locaEnd = string.find(str,newDeli)
    local arr = {}
    local n = 1
    while locaStart ~= nil
    do
        if locaStart>0 then
            arr[n] = string.sub(str,1,locaStart-1)
            n = n + 1
        end

        str = string.sub(str,locaEnd+1,string.len(str))
        locaStart,locaEnd = string.find(str,newDeli)
    end
    if str ~= nil then
        arr[n] = str
    end
    return arr
end 