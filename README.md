This was my first time using OpenGL. Decided to make a simple simulation of the Solar systems planets.

It uses Verlet Velocity Integration to calculate the trajectories of the planets.

I had issues with Mercury since it is too close to the sun for it to form a reasonable trajectory using this system.

it uses the time for each frame to occur as a timestep and has the masses in solar masses and distances in AU.

The radius of the planets and sun are scaled up so they can be easily seen.

The Shader and Camera classes are mainly taken from the LearnOpenGl website with a few small changes. 
