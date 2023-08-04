# CS488 Final Project HongFei Huang (Fracture Simulation)
### The maximum late days(3) are used for this project.
<br />
<br />

## Desription <br />
In this project, I implemented a fracure simulation using the prefractured models generated upon existing ones using blender. The program can take a input point from mouse clicking and cast a ray to any direction. If the ray collides with the model, then regarding the magnitude of the force, the object will either stay intact or break under the force hit. The broken pieces can interact with each other upon breaking and boucing, but they will become completely static after the stop criteria is reached, which is not very physically accurate(one of the comprimises that I had to make). 


## How to run program
The program should be built/run on Windows 10/11 with a either Visual Studio 2019 or 2022. Any graphics card that come with OpenGL 4.6 should be able to run this program without any problems(tested across different Windows PCs). My apologies for not having a proper Makefile; my time to finish the project was quite tight.

After opening the CollodeSimulation.sln, simply use the DEBUG/RELEASE mode to run the program. There no command line parameters required





















## Final Thoughts <br />
To be honest, I'm not quite happy about how this project looks like. The reason why I was not using the starter code is because I want to get some hand-on experiences on OpenGL, and I was pretty confident at the beginning that it wouldn't take too much time to rebuild the interfaces/tools we already have, but I was wrong; I ended up investing quite a lot of time on them(especiall SAH-BVH) and my tight schedule for other courses really didn't leave me much time for a proper implementation of rigid body simulation. So as I result, I was using way too many "hacks" and "janky approches" in the code in order to make the project look somewhat presentable(for example, only use the broad phase AABB bounding box to detect and determine collision and didn't use any narrow phase techniques). 

The manner in which I completed this project fell short of the expectations I had set for myself, and I'm certain it didn't meet yours either.  I want to apologize for this outcome, especially since I had previously sent an email to you all saying "I want to make this project as successful as possible". It appears I over-promised and under-delivered... 

With that said, although this project is a setback for me, but I'm confident in my ability to produce better work given more time, and I still want to learn more about Computer Graphics since it's such an interesting topic. I want to thank you all for your efforts in making this course so enjoyable.

