require ("_prolog")
function RequireParams()  
   lips_RequireParams("radius = int([1, 32] 10); dots = int([1, 100] 20); pick_on_move = bool([] false); ")
end   
function main()
  -- order: MouseDown, MouseMove, MouseUp
  lips_RegisterMouseEvent(MouseDown, MouseMove, MouseUp)
  math.randomseed( os.time())  
  lips_SendMessage("Waiting for mouse...")
end    
function MouseDown(X, Y)
  mr,mg,mb = lips_GetPixelSafe(SourceImage, Y ,X)  
  lips_RefreshParams() -- refresh params values
  --lips_SendMessage("Down!")
end
function MouseUp(X, Y)
  --lips_SendMessage("Up!")
end
function MouseMove(X, Y)
  local position = "x =" .. tostring(X) .. " y = " .. tostring(Y)   
  if (MouseState["ssLeft"]) then
      if (pick_on_move) then  
          mr,mg,mb = lips_GetPixelSafe(SourceImage, Y ,X)
      end
      local Y1, X1
      for i = 0, dots do
         Y1 = Y + radius - math.random(0, radius * 2)
         X1 = X + radius - math.random(0, radius * 2)
         lips_SetPixelSafe(SourceImage, Y1 , X1, mr, mg, mb) 
      end
      lips_SendCommand("UpdateScreen")      
  end  
  lips_SendCommand("Info", position)
end