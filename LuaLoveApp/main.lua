images = {}
handCards = { 11, 29, 7, 1, 41 }
cardSelected = -1

--  DEFINES
handOffsetX = 100
handOffsetY = 50
cardOffsetBetweenX = 60

--  LOVE.LOAD
function love.load(arg)
  if arg[#arg] == "-debug" then require("mobdebug").start() end
  
  success = love.window.setMode( 960, 640,  {resizable=false, vsync=false, minwidth=400, minheight=300} )
  
  for nameIndex, name in ipairs({
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    'pip_heart', 'pip_diamond', 'pip_club', 'pip_spade',
    'card',
  }) do
    images[name] = love.graphics.newImage('images/'..name..'.png')
  end
end

--  LOVE.FOCUS
function love.focus(f)
  if not f then
    print("LOST FOCUS")
  else
    print("GAINED FOCUS")
  end
end

--  LOVE.QUIT
function love.quit()
  print("Thanks for playing! Come back soon!")
end

--  LOVE.MOUSEPRESSED
function love.mousepressed(x, y, button, istouch)
end

--  LOVE.MOUSERELEASED
function love.mousereleased(x, y, button, istouch)
  if button == 1 then
    for i=1,5,1 do
      if x >= handOffsetX + (i * cardOffsetBetweenX) then
        cardSelected = i
        end
    end
  end
end

--  LOVE.KEYPRESSED
function love.keypressed(key)
end

--  LOVE.KEYRELEASED
function love.keyreleased(key)
  if cardSelected == -1 then return end
  
  if key == '1' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 0
  elseif key == '2' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 1
  elseif key == '3' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 2
  elseif key == '4' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 3
  elseif key == '5' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 4
  elseif key == '6' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 5
  elseif key == '7' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 6
  elseif key == '8' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 7
  elseif key == '9' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 8
  elseif key == '0' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 9
  elseif key == 'j' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 10
  elseif key == 'q' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 11
  elseif key == 'k' then
    handCards[cardSelected] = (math.floor(handCards[cardSelected] / 13) * 13) + 12
  elseif key == 'h' then
    handCards[cardSelected] = (handCards[cardSelected] % 13)
  elseif key == 'd' then
    handCards[cardSelected] = (handCards[cardSelected] % 13) + 13
  elseif key == 'c' then
    handCards[cardSelected] = (handCards[cardSelected] % 13) + 26
  elseif key == 's' then
    handCards[cardSelected] = (handCards[cardSelected] % 13) + 39
  end
end

--  LOVE.UPDATE
function love.update(dt)
end

--  LOVE.DRAW
function love.draw()
  for i=1,5,1  do DrawCard(handCards[i], handOffsetX + (i * cardOffsetBetweenX), handOffsetY) end
end

--  Draw a card
function DrawCard(index, x, y)
  love.graphics.setColor(255,255,255,255)
  love.graphics.draw(images.card, x, y)
  
  rank = index % 13
  suit = math.floor(index / 13)
  
  suitImage = ((suit == 0) and 'pip_heart' or ((suit == 1) and 'pip_diamond' or ((suit == 2) and 'pip_club' or 'pip_spade')))
  
  if suit < 2 then love.graphics.setColor(255, 0, 0, 255) else love.graphics.setColor(0,0,0,255) end
  love.graphics.draw(images[rank + 1], x + 4, y + 4)
  love.graphics.draw(images[suitImage], x + 20, y + 4)
end