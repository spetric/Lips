require ("_prolog")
require ("_opencv")
--[[
This is experimental function. It's similar to Gaborius, but with (mostly) triginometric kernels.
--]]
function RequireParams()  
   lips_RequireParams("kernel_func = combo([cos, sin, cos_c, sin_c, triangle] 0);"
    .. " step = int([1, 32] 16); kernel_size = int([3, 101] 31);"
    .. " hSigma = float([0, 30] 0.15); freq = float([1, 30] 9);"
    .. " ks_factor = float([0.001, 10] 1.2); margin = int([-10, 10] 2); "
    .. " vSigma = float([0, 10] 0);")
end   
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end      
  if (SourceImage.Width ~= TargetImage.Width or SourceImage.Height ~= TargetImage.Height) then
     lips_SendMessage("Source.Size <> Target.Size")
     return     
  end     
  -- collect parameters to params string
  local params = tostring(kernel_func)..", "..tostring(step)..", "..tostring(kernel_size)..", "..tostring(hSigma)..", "
  ..tostring(freq)..", "..tostring(ks_factor)..", "..tostring(margin)..", "..tostring(vSigma)
  -- set OpenCV src/tgt images
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)   
  -- perform Gaborius
  lips_SendMessage("Applying multistep trigonometric filter.")  
  ocv_Process("Trigonometrius", params)
end