require ("_prolog")
function RequireParams()  
   lips_RequireParams("local_stretch = bool([] false); ")
end   
function main()
  -- target images must be loaded
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end  
  contrast_stretch() 
end
function get_min_max()
    local min, max = 1000, -1000         
    local top, bottom, left, right
    if local_stretch then
       top, bottom, left, right = Roi.Top, Roi.Bottom, Roi.Left, Roi.Right; 
    else
       top, bottom, left, right = 0, SourceImage.Height-1, 0, SourceImage.Width-1; 
    end

    for y=top, bottom do
      for x=left, right do
        value = lips_GetLuminance(SourceImage, y, x)
        if value<min then
          min = value
        end
        if value>max then
          max = value
        end
      end  
    end
    return min*255, max*255
end

function remap(v, min, max)
    return (v-min) * 255.0/(max-min)
end

function cs_get_rgb(x,y,min,max)
    r,g,b = lips_GetPixel(SourceImage, y, x)
    r = remap(r, min, max)
    g = remap(g, min, max)
    b = remap(b, min, max)
    return r,g,b
end

function contrast_stretch()
    local min, max = get_min_max()

    for y=Roi.Top, Roi.Bottom do
      for x=Roi.Left, Roi.Right do
        lips_SetPixel(TargetImage, y, x, cs_get_rgb(x,y,min,max))
      end
      lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)       
    end
end

