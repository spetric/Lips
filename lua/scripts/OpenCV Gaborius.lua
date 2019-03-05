require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("step = int([1, 32] 16); kernel_size = int([3, 101] 31);"
    .. " sigma = float([0.1, 10] 4.0); lambda = float([1, 30] 10);"
    .. " gamma = float([0.1, 10] 0.2); psi = float([0, 1.57] 0); ks_factor = float([0.001, 10] 1.5); ")
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
  local params = tostring(step)..", "..tostring(kernel_size)..", "..tostring(sigma)..", "
  ..tostring(lambda)..", "..tostring(gamma)..", "..tostring(psi)..", "..tostring(ks_factor)  
  -- set OpenCV src/tgt images
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)   
  -- perform Gaborius
  lips_SendMessage("Applying multistep Gabor filter.")  
  ocv_Process("Gaborius", params)
end