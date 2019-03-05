require ("_prolog")
function RequireParams()  
   lips_RequireParams("y_offset = int([0, 30] 3); x_offset = int([0, 30] 3); scale = float([0, 8] 4.0);"
     .. "mask_blur_radius = int([0, 10] 0 );"     
     .. "threshold = int([0, 255] 0); roughness = int([0, 255] 0); soft_bumping = bool([] false);"
     .. "threshold_option = combo([Input cutoff, Input decay, Output cutoff, Output decay] 0);"     
     .. "texture = imagefile([*.jpg] #textures);") 
end   
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
  math.randomseed( os.time())
  -- create single channel grayscale global image
  gsMask = lips_CreateImage("grayscale", SourceImage.Width, SourceImage.Height)
  -- convert inut image to grayscale
  lips_Convert_rgb2grayscale(SourceImage, gsMask)  
  -- color texure image
  rgbTexture = lips_LoadImage(texture, "rgb24") 
  if mask_blur_radius > 0 then
     lips_Blur(gsMask, gsMask, mask_blur_radius)  
   end     
  for y = Roi.Top, Roi.Bottom do
    for x = Roi.Left, Roi.Right do
        local r, g, b = bumpPixel(y, x, y_offset, x_offset, scale, roughness)
        lips_SetPixel(TargetImage, y, x, r, g, b)
    end
    lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)      
  end
  
end
function bumpPixel(y, x, off_y, off_x, scale, roughness)
  -- bumping length
  local mask_val = lips_GetPixelWrap(gsMask, y, x)  
  local mask_off = lips_GetPixelWrap(gsMask, y + off_y, x + off_x) 
  local	cor_len  = (mask_val - mask_off) * scale
  local r, g, b = lips_GetPixelWrap(rgbTexture, y, x)
  local luminance = mask_val/255.0  
  local strength, rough_val
  if roughness > 0 then 
      rough_val= roughness - math.random(0, roughness * 2)
  else
      rough_val = 0    
  end  
  if soft_bumping then
    strength = mask_val/255.0
  else
    strength = 1.0
  end
  if mask_val < threshold and threshold_option < 2 then
    if threshold_option == 0 then
       r, g, b  = 0, 0, 0
    else
       r = r * luminance
       g = g * luminance
       b = b * luminance
    end
  end 
  r = r + cor_len * strength + rough_val
  g = g + cor_len * strength + rough_val
  b = b + cor_len * strength + rough_val
  if (mask_val < threshold)  and (threshold_option >= 2) then
    if threshold_option == 2 then
       r, g, b = 0, 0, 0
    else
      r = r * luminance
      g = g * luminance
      b = b * luminance
    end
  end 
  r, g, b = lips_NormalizeRGB(r, g, b)  
  return r, g, b
end

