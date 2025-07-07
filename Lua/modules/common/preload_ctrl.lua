PreloadCtrl = PreloadCtrl or BaseClass(BaseCtrl)-- or BaseClass(BaseController)

local preload_funcs = {}

local preload_count = 0

local preload_callback = nil

local is_in_preload_func = false

function PreloadCtrl.RegisterPreload(func)
	table.insert(preload_funcs,func)
end

function PreloadCtrl.RegisterLoading()
	preload_count = preload_count + 1
end

function PreloadCtrl.UnregisterLoading()
	preload_count = preload_count - 1
	if preload_callback ~= nil and is_in_preload_func == false and preload_count <= 0 then
		preload_callback()
		preload_callback = nil
	end
end

function PreloadCtrl.Preload(call_back)
	is_in_preload_func = true
	preload_callback = call_back

	preload_count = 0
	for i,v in ipairs(preload_funcs) do 
		if not ( GLOBAL_CONFIG:PkgInfo().single or GLOBAL_CONFIG:Flags().mini_game) then
            v()
        end
	end
	if preload_callback ~= nil and preload_count <= 0 then
		preload_callback()
		preload_callback = nil
	end
	is_in_preload_func = false
end



PreloadCtrl.RegisterPreload(
    function ()
    	PreloadCtrl.RegisterLoading()
        ResMgr:LoadObject("ui/Localization.txt",
        	function (loc_txt_obj)
        		Game.Localization.SetupData(loc_txt_obj)
         		PreloadCtrl.UnregisterLoading()
       		end
        	,false)
    end)