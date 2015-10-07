# OpenGLGame
A small mini-game created in OpenGL

This game consists of zombies in a simple FPS world.

Features:
- Not Frame dependent. It will not depend on frames.
- Using OpenGL 1.1, I tried to preload walls and floor to reduce drawing new vertices every time.
- Uses an FPS camera. Freelooking. Not like the ugly GLUT movements forward and backwards
- Has 5 rounds. Once all zombies die, a new round of zombies will appear. 3 different kinds of zombies.
- Zombies don't chase you unless you're near them.
- Self-built physics engine.
- Collision detection that will detect collision from both sides of a polygon. (also, it doesn't make a stuttering movement, which is cool in my opinion)
- Health Bar and Respawn timer when dead. (This broke when i changed the code a bit)
- Can shoot bullets and kill zombies. Zombies knockback when you hit them.
- A really, really janky skybox with really really simple pixel art for walls, floor, zombies.
- Fog because why not.
- Spotlight lighting

Controls:
- WASD - Movement
- Right Click - Shoot bullets
- G - Fog
- L - Lighting
- 

Known Problems:
- If you sprint jump constantly, you'll slowly sink into the floor.
- If you try to jump onto the platform in the middle, things start glitching with jumps and collisions
- You don't actually lose health for some reason.
- Zombies hitboxes can actually screw up a little bit. (but its quite accurate 95% of the time)
- Zombies stack on top of each other eventually, which can be annoying.
- You can jump over the wall if you sprint jump.
- Jumping with sprint on changes your Y velocity as well.
- Jumping is just really glitched.
