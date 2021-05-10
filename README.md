# ksp_2d

Coursework for a second year C++ module at University of Nottingham. 

This game models a to-scale solar system, with 'realistic' gravity and a vague simulation of air resistance and collision. 
You can build a modular spaceship, the aerodynamic profile and thrust dependant on the shape of the ship and the parts used, and fly it around the system, speeding up time to get to other bodies faster.
The planetary bodies are essentially the same as those in our solar system. Landing on them is challenging, due to no information on their orbits and no ability to plan flight paths, but the ship is 'technically' able to get into stable orbits and move between these planets. :'D

As per the repository name, it was inspired by Kerbal Space Program, a game similar to this, but in 3D and about as realistic of a spaceflight simulator as possible to get while still being incredibly fun and inspiring.

This game was written using a minimal wrapper around SDL2 (provided by our lecturer - the assigned task included utilising this library) and C++ 17. 

The CMakeLists file will require changing to run the code given here, but for those interested but unable to run the code (I don't blame you, this certainly wasn't made with other users in mind) please check out the demonstation video on youtube: https://youtu.be/LfaOHIZrmag
