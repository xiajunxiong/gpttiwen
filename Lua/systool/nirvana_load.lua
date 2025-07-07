
NirvanaLoad = {}

local is_loaded_nr = false

function NirvanaLoad.CheckNirvanaRuntime()
    if not is_loaded_nr then
        is_loaded_nr = true
        tolua.loadassembly('NirvanaRuntime')
    end
end