require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("style = combo([Pablo Picasso - La Muse, Mosaic/glass painting, Edvard Munch - The Scream, Francis Picabia - Udnie, Natasha Wescoat - June Tree alias Candy, Cezanne - CycleGAN] 0); scale = float([1.0, 10.0] 1.0);"
     .." meanR = float([0, 255] 103.99); meanG = float([0, 255] 116.779); meanB = float([0, 255] 123.68);"
     .." swapRB = bool([] false);")
  end
--[[ tested models
Pablo Picasso - La Muse
Mosaic/glass painting -  
Edvard Munch - The Scream
Henri Matisse - Woman with a Hat
Francis Picabia - Udnie
Unknown      - Feathers
Wassily Kandinsky - Composition VII
Natasha Wescoat - June Tree alias Candy
Monet CycleGAN (normResult = true) -
Cezanne CycleGAN (normResult = true) -
Ukiyoe CycleGAN (normRsult = true) - 
--]]
--[[  Check these urls for more pre-trained models:
https://github.com/jcjohnson/fast-neural-style
https://github.com/ProGamerGov/Torch-Models
https://github.com/floydhub/fast-neural-style
https://github.com/hwalsuklee/tensorflow-fast-style-transfer
https://github.com/lengstrom/fast-style-transfer
https://github.com/junyanz/CycleGAN/tree/master/pretrained_models
--]]
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  if (SourceImage.Width ~= TargetImage.Width or SourceImage.Height ~= TargetImage.Height) then
     lips_SendMessage("Source.Size <> Target.Size")
     return     
  end   
  if (ExeWin32) then
      if (SourceImage.Width > 2000 or SourceImage.Height > 2000) then
          local hdret = lips_HostDialog("Image size is too large for 32-bit version. Do you really want to proceed?") 
          if (hdret == 0) then
             return
          end
      end
  end  
  local normResult = false
  -- pretrained models (test)  
  local model = ExePath
  if (style == 0) then
      model = model.."data/stylemodels/la_muse.t7"
  elseif (style == 1) then
      model = model.."data/stylemodels/mosaic.t7"
  elseif (style == 2) then
      model = model.."data/stylemodels/scream.t7"      
  elseif (style == 3) then      
      model = model.."data/stylemodels/udnie.t7"            
  elseif (style == 4) then
      model = model.."data/stylemodels/candy.t7"            
  else 
      normResult = true  
      model = model.."data/stylemodels/gan_cezanne.t7"                    
  end
  -- collect parameters to params string
  local params = tostring(scale)..", "..tostring(meanR)..", "..tostring(meanG)..", "
  ..tostring(meanB)..", "..tostring(normResult)..", "..tostring(swapRB)
  -- set source and target image
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)   
    -- create empty DK
  local dkIdx = ocv_CreateDK()
  -- set DK idx  
  local rc = ocv_SetDK(dkIdx)
  -- load style (model)
  lips_SendMessage("Loading style");  
  ocv_Process("LoadDNN", model)
  lips_SendMessage("Applying style transfer.");
  ocv_Process("StyleTransfer", params) 
  -- release dk
  ocv_ClearDK(dkIdx)
end
