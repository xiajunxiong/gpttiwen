Cinemachine = Cinemachine or {}
Cinemachine.is_loaded = false
function Cinemachine.loadassembly()
    if not Cinemachine.is_loaded then
        tolua.loadassembly('Cinemachine')
        Cinemachine.is_loaded = true
    end
end