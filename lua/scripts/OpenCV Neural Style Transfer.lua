require ("_prolog")
require ("_opencv")
function RequireParams()  
  --[[
   lips_RequireParams("style = combo([Pablo Picasso - La Muse, Mosaic/glass painting, Edvard Munch - The Scream, Francis Picabia - Udnie, Natasha Wescoat - June Tree alias Candy, Cezanne - CycleGAN] 0); scale = float([0.1, 10.0] 1.0);"
     .." meanR = float([0, 255] 103.99); meanG = float([0, 255] 116.779); meanB = float([0, 255] 123.68);"
     .." swapRB = bool([] false);")
     --]]
  -- alternative -> use file
  -- Here is an example with file
  lips_RequireParams("style_name = file([*.t7] "..ExePath.."data/stylemodels);" 
     .." scale = float([0.1, 10.0] 1.0);"
     .." meanR = float([0, 255] 103.99); meanG = float([0, 255] 116.779); meanB = float([0, 255] 123.68);"
     .." swapRB = bool([] false);")
  --
  end
--[[
--
--  Check these urls for more pre-trained models:
--
https://github.com/jcjohnson/fast-neural-style
https://github.com/ProGamerGov/Torch-Models
https://github.com/floydhub/fast-neural-style
https://github.com/hwalsuklee/tensorflow-fast-style-transfer
https://github.com/lengstrom/fast-style-transfer
https://github.com/junyanz/CycleGAN/tree/master/pretrained_models
--]]
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabled) 
     return     
  end     
  if (style_name == nil or style_name == "") then
     lips_SendMessage("No style selected") 
  lips_HostDialog('No style selected. IMHO, style models are probably missing. Download models package: <A href="https://github.com/spetric/Lips/releases/download/v2.0/data_stylemodels.zip">data_stylemodels.zip</A><br> and unpack it under <b>data/stylemodels/</b> folder.', 2)       
     return
  end   
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  if (SourceImage.Width ~= TargetImage.Width or SourceImage.Height ~= TargetImage.Height) then
     lips_SendMessage("Source.Size <> Target.Size")
     return     
  end   
  if (ExeWin32) then
      if (SourceImage.Width > 1200 or SourceImage.Height > 1200) then
          local hdret = lips_HostDialog("Image size is too large for 32-bit version. Do you really want to proceed?", 1) 
          if (hdret == 0) then
             return
          end
      end
  end  
  local normResult = false
  -- pretrained models (using combo box)  
  --[[
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
  --]]
  -- pretrained model (using selected file via file or filelist)
  -- NOTE: for styles that need to be normalized, change the filename so that it contains NORM keyword:
  -- Example gan_cezanne.t7 => gan_cezanne_NORM.t7
  local model = style_name 
  if (string.find(model, "NORM") ~= nil) then
     normResult = true 
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
  lips_SendMessage("Loading style.");  
  ocv_Process("LoadDNN", model, "")  -- no config file for this example
  lips_SendMessage("Applying style transfer.");
  ocv_Process("StyleTransfer", params) 
  -- release dk
  ocv_ClearDK(dkIdx)
end
