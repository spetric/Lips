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
  component_stretch()
end
function get_min_max_rgb ()
    local min = {}
    local max = {}
    local value = {}
    for i = 0, 2 do
      min[i] = 1000
      max[i] = -1000
      value[i] = 0
    end     
    local top, bottom, left, right
    if local_stretch then
       top, bottom, left, right = Roi.Top, Roi.Bottom, Roi.Left, Roi.Right; 
    else
       top, bottom, left, right = 0, SourceImage.Height-1, 0, SourceImage.Width-1; 
    end
    
    for y=top, bottom do
      for x=left, right do
        value[0], value[1], value[2] = lips_GetPixel(SourceImage, y, x)       
        for i=0, 2 do
          if value[i] < min[i] then
            min[i] = value[i]
          end
          if value[i] > max[i] then
            max[i] = value[i]
          end
        end                
      end  
    end
    return min, max
end


function remap(v, min, max)
    return (v-min) * 255.0/(max-min)
end

function cs_get_rgb(x,y, min, max)
    r,g,b = lips_GetPixel(SourceImage, y ,x)
    r = remap(r, min[0], max[0])
    g = remap(g, min[1], max[1])
    b = remap(b, min[2], max[2])
    return r,g,b
end

function component_stretch()
    local min, max = get_min_max_rgb()
    for y=Roi.Top, Roi.Bottom do
      for x=Roi.Left, Roi.Right do
        lips_SetPixel(TargetImage, y, x, cs_get_rgb(x, y, min, max))
      end
      lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top) 
    end
end

