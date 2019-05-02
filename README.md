# Navigation-Link-Generator
Generator to create Unreal's Nav Link dynamically using recast navmesh's data.
-----------

Note: This is an old project I made. Code in the project is very experimental and not tested throughly yet. 

-----------
Navigation links can be used in any game which requires either jumping, climbing or just connecting two or more navigation meshes 
in order to the pathfinding on it. It's an approach to connect two nav mesh with an start and end location so a region of nav mesh 
would know how to reach to the next region of mesh. 
Placing these nav links on nav meshes can be a lot of works specially if the maps are very large or if the map changes during runtime.
I have tried creating a small system which would generate the nav link automatically in the region specified. 

I have also made some modifiers
* To not generate links in certain region.
* Only left directional links.
* Only right directional links.
* Modified distance from left or right.

## Link Modifier:
![Modifier](https://raw.githubusercontent.com/swastik1992/Navigation-Link-Generator/master/Images/Capture02.PNG)

## Link Generator:
![Modifier](https://raw.githubusercontent.com/swastik1992/Navigation-Link-Generator/master/Images/Capture01.PNG)

## Demo:
![](Images/demo1.gif)


